// 
// Copyright(c) 2009 Syntext, Inc. All Rights Reserved.
// Contact: info@syntext.com, http://www.syntext.com
// 
// This file is part of Syntext Serna XML Editor.
// 
// COMMERCIAL USAGE
// Licensees holding valid Syntext Serna commercial licenses may use this file
// in accordance with the Syntext Serna Commercial License Agreement provided
// with the software, or, alternatively, in accorance with the terms contained
// in a written agreement between you and Syntext, Inc.
// 
// GNU GENERAL PUBLIC LICENSE USAGE
// Alternatively, this file may be used under the terms of the GNU General 
// Public License versions 2.0 or 3.0 as published by the Free Software 
// Foundation and appearing in the file LICENSE.GPL included in the packaging 
// of this file. In addition, as a special exception, Syntext, Inc. gives you
// certain additional rights, which are described in the Syntext, Inc. GPL 
// Exception for Syntext Serna Free Edition, included in the file 
// GPL_EXCEPTION.txt in this package.
// 
// You should have received a copy of appropriate licenses along with this 
// package. If not, see <http://www.syntext.com/legal/>. If you are unsure
// which license is appropriate for your use, please contact the sales 
// department at sales@syntext.com.
// 
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
// 
// Copyright (c) 2005 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#include "config_utils.h"
#include "sys_deps.h"
#include "utils/config_defaults.h"
#include "utils/env_utils.h"
#include "utils/file_utils.h"
#include "utils/Config.h"
#include "utils/Properties.h"
#include "utils/utils_debug.h"

#include "common/PropertyTree.h"
#include "common/PathName.h"
#include "common/StringCmp.h"
#include "common/StringCvt.h"
#include "common/OsEnv.h"

#include "proputils/PropertyTreeSaver.h"

#include <stdlib.h>

#ifdef _WIN32
# include "win32/config_win32.h"
# include <windows.h>
typedef Common::Char ArgChar;
static const wchar_t HOME_VAR[] = NOTR(L"USERPROFILE");
static const char PATH_VAR[] = NOTR("PATH");
#else
# if defined(__APPLE__)
static const char PATH_VAR[] = NOTR("DYLD_LIBRARY_PATH");
# else
static const char PATH_VAR[] = NOTR("LD_LIBRARY_PATH");
# endif
typedef char ArgChar;
static const char HOME_VAR[] = NOTR("HOME");
#endif

void set_ldlibpath()
{
    using namespace Common;

    const ustring pathVar(from_latin1(PATH_VAR));
    ustring path(get_env(pathVar));
    PathName exePath(full_exe_path(String::null()));
    ustring exeDir(exePath.dirname().name());
    if (!path.empty())
        exeDir.append(1, PathName::PATH_SEP);
    path.insert(0, exeDir);
    set_env(pathVar, path);
}

namespace CfgUtils {

using namespace Common;

void resolve_init_variables(PropertyNode* ptn,
                            const Common::String& dataDir,
                            const Common::String& configDir)
{
    if (ptn->firstChild()) {
        PropertyNode* next = 0;
        PropertyNode* n = ptn->firstChild();
        while (n) {
            next = n->nextSibling();
            resolve_init_variables(n, dataDir, configDir);
            n = next;
        }
    }
    else {
        // strip prefix
        int colonpos = ptn->name().find(':');
        if (colonpos > 0) {
            String prefix = ptn->name().left(colonpos);
            if (TARGET_PLATFORM_PROPERTY_PREFIX != prefix) {
                ptn->remove();
                return;
            }
            ptn->setName(ptn->name().
                         right(ptn->name().length() - colonpos - 1));
            DBG(UTILS.CONFIG) << "CONFIG: sysdep property: <" << ptn->name()
                              <<"> value: <" << ptn->getString() << ">\n";
        }
        // resolve variables
        String is = ptn->getString();
        String os;
        const Char* cp = is.unicode();
        const Char* ce = cp + is.length();
        for (;;) {
            while (cp < ce && *cp != '$')
                os += *cp++;
            if (cp >= ce)
                break;
            if (&cp[1] < ce && cp[1] == '$') {
                os += *++cp;
                ++cp;
                continue;
            }
            const Char* ocp = ++cp;
            while (cp < ce && *cp != '/' && *cp != '\\')
                ++cp;
            String varname(ocp, cp - ocp);
            if (INITCONFIG_DATADIR_VAR == varname)
                os += dataDir;
            else
                if (INITCONFIG_CONFIGDIR_VAR == varname)
                    os += configDir;
            if (cp < ce)
                os += *cp++;
        }
        ptn->setString(os);
    }
}

PropertyNode* read_from_template(const String& dataDir,
                                 const String& configDir,
                                 const String& cfgTplFile)
{
    PathName cfgTplPath;
    if (is_null(cfgTplFile))
        cfgTplPath.assign(dataDir).append(from_latin1(SERNA_CONFIG_TEMPLATE));
    else {
        if (PathName::isRelative(cfgTplFile))
            cfgTplPath.assign(dataDir).append(cfgTplFile);
        else
            cfgTplPath.assign(cfgTplFile);
    }

    PropertyNodePtr configTemplatePtr(new PropertyNode);
    PropertyNode* cfgTemplate = configTemplatePtr.pointer();

    PropUtils::PropertyTreeSaver psaver(cfgTemplate, SERNA_XMLCONFIG_ROOT);
    if (!psaver.readPropertyTree(cfgTplPath.name())) {
        DBG(UTILS.CONFIG) << "CONFIG: failed to read template "
                          << sqt(cfgTplPath.name()) << std::endl;
    }
    resolve_init_variables(cfgTemplate, dataDir, configDir);
    return configTemplatePtr.release();
}

static String get_serna_env_var(const String& varName)
{
    ustring envVarName(to_upper<ustring>(from_latin1(SERNA_VAR_PREFIX)));
    str_append(envVarName, make_range(to_upper<ustring>(varName)));
    return get_env(envVarName);
}

String get_config_dir(const PropertyNode* env)
{
    String cfgDirVarName(from_latin1(SERNA_CONFIG_DIR_ENVVAR));
    if (env)
        if (PropertyNode* pn = env->getProperty(cfgDirVarName))
            return pn->getString();

    String value(get_serna_env_var(cfgDirVarName));
    if (!value.empty())
        return value;

    return get_home_dir() + PathName::DIR_SEP + SERNA_CONFIG_DIR;
}

String get_config_file(const PropertyNode* env)
{
    String cfgFileVarName(from_latin1(SERNA_CONFIG_FILE_ENVVAR));
    if (env)
        if (PropertyNode* pn = env->getProperty(cfgFileVarName))
            return pn->getString();

    String value(get_serna_env_var(cfgFileVarName));
    if (!value.empty())
        return value;

    return get_config_dir(env) + PathName::DIR_SEP + SERNA_CONFIG_FILE;
}

String get_home_dir()
{
    return get_env(HOME_VAR);
}

String get_data_dir()
{
    String value(get_serna_env_var(from_latin1(INITCONFIG_DATADIR_VAR)));
    if (!value.empty())
        return value;

#ifdef _WIN32
    return cfg::get_win32_instdir(SERNA_DATA_DIR_PROPERTY).name();
#else
    PathName full_exe_path(FileUtils::argv0());
    return full_exe_path.dirname().dirname().name();
#endif
}

struct ParamDesc {
    const char* optName_;
    const char* cfgVarName_;
    bool        hasValue_;
};

using namespace CmdLineParams;

// START_IGNORE_LITERALS
static const ParamDesc PARAM_DESCS[] = {
    { "r", CMD_RECENT_FILE_ID, true },       // recent file id
    { "c", SERNA_CONFIG_FILE_ENVVAR, true }, // config file
    { "p", SERNA_CONFIG_DIR_ENVVAR, true },  // config dir
    { "q", "#no-splash", false }, // hide splash
    { "i", INSTALL_CONFIG_PACKAGE, true },
    { "I", INSTALL_CONFIG_PACKAGE_W_DIAG, true },
    { "S", "#guitest", false }
#ifndef NDEBUG
    ,
    { "de", ENABLE_DEBUG_TAGS, true }, // enable debug tags
    { "dd", DISABLE_DEBUG_TAGS, true }, // disable debug tags
    { "oa", OALLOC_ABORT_ON_ERROR, false },
    { "od", OALLOC_DISABLED, false },
    { "op", OALLOC_PRINT_STATS_ON_EXIT, false }
#endif
};
// STOP_IGNORE_LITERALS

const ParamDesc* lookup_opt(const ArgChar* arg)
{
    for (uint i = 0; i < sizeof(PARAM_DESCS)/sizeof(PARAM_DESCS[0]); ++i) {
        if (0 == compare(PARAM_DESCS[i].optName_,
                         strlen(PARAM_DESCS[i].optName_), arg))
            return &PARAM_DESCS[i];
    }
    return 0;
}

static String make_string(const ArgChar* arg)
{
#if defined(_WIN32)
    return String(arg);
#else
    return from_local_8bit(arg);
#endif
}

DBG_TAG(UTILS,CMDLINE,false);

static PropertyNodePtr parse_cmdline_pvt(int argc, const ArgChar* const* argv)
{
    PropertyNodePtr argsProp(new PropertyNode(CmdLineParams::CMD_LINE_PARAMS));

    for (++argv; 0 < --argc; ++argv) {
        const ArgChar* arg = argv[0];
        if ('-' == arg[0]) {
            if (const ParamDesc* pd = lookup_opt(arg + 1)) {
                DBG(UTILS_CMDLINE) << "CMDLINE: option '" << pd->cfgVarName_
                                   << '\'' << std::endl;
                if (pd->hasValue_) {
                    if (argc > 1 && '-' != *argv[1]) {
                        String value(make_string(argv[1]));
                        DBG(UTILS_CMDLINE) << "CMDLINE: " << pd->cfgVarName_
                                           << '=' << '\'' << value << '\''
                                           << std::endl;
                        argsProp->makeDescendant(pd->cfgVarName_, value);
                        --argc;
                        ++argv;
                    }
                }
                else
                    argsProp->makeDescendant(pd->cfgVarName_)->setBool(true);
            }
        }
        else {
            PropertyNode* fileargs = argsProp->makeDescendant(CMD_FILE_ARGS);
            DBG(UTILS_CMDLINE) << "CMDLINE: file-arg '" << make_string(arg)
                               << '\'' << std::endl;
            fileargs->appendChild(new PropertyNode(String(),
                Url(make_string(arg)).absolute()));
        }
    }
    return argsProp->empty() ? PropertyNodePtr(0) : argsProp;
}

PropertyNodePtr parse_cmdline(const wchar_t* cmdline)
{
#if defined(_WIN32)
    int argc = 0;
    if (LPWSTR* wargv = CommandLineToArgvW(cmdline, &argc)) {
        const ArgChar* const* argv;
        argv = reinterpret_cast<const ArgChar* const*>(wargv);
        return parse_cmdline_pvt(argc, argv);
    }
#else
    (void) cmdline;
#endif
    return PropertyNodePtr(new PropertyNode);
}

PropertyNodePtr parse_cmdline(int argc, const char* const* argv)
{
#if !defined(_WIN32)
    return parse_cmdline_pvt(argc, argv);
#else
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (0 == wargv)
        return PropertyNodePtr(new PropertyNode);
    return parse_cmdline_pvt(argc,
                             reinterpret_cast<const ArgChar* const*>(wargv));
#endif
}

void fill_file_handlers_list(PropertyNode* cfgRoot)
{
#if defined(__linux__) || defined(__sun__)
    using namespace FileHandler;
    static const char* BROWSERS[] = { NOTR("/usr/bin/firefox"),
                                      NOTR("/usr/bin/mozilla"),
                                      NOTR("/usr/bin/netscape"), 0 };
    for (const char** browser = &BROWSERS[0]; *browser; ++browser) {
    //std::cerr << "try to find browser: " << *browser << std::endl;
        if (PathName::exists(from_local_8bit(*browser))) {
    //std::cerr << "found browser: " << *browser << std::endl;
            PropertyNode* handlers = cfgRoot->makeDescendant(HANDLER_LIST_TEMP);
            PropertyNode* app = new PropertyNode(APP);
            app->appendChild(new PropertyNode(APP_EXT, NOTR("html")));
            app->appendChild(new PropertyNode(APP_PATH, *browser));
            handlers->appendChild(app);
            return;
        }
    }
#else
    (void) cfgRoot;
#endif
}

}
