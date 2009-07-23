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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "Config.h"
#include "config_defaults.h"
#include "common/Singleton.h"
#include "common/PropertyTree.h"
#include "common/PropertyTreeEventFactory.h"
#include "common/PathName.h"
#include "common/Url.h"
#include "common/StringCvt.h"
#include "proputils/EnvPropertyReader.h"
#include "proputils/PropertyTreeSaver.h"
#include "proputils/VariableResolver.h"
#include "utils/utils_debug.h"
#include "utils/file_utils.h"
#include "utils/Properties.h"
#include "config_utils.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <QObject>

#ifdef _WIN32
# include <direct.h>
#endif // _WIN32

USING_COMMON_NS
using namespace CfgUtils;

class ConfigImpl : public Config {
public:
    ConfigImpl();

    virtual const PropertyNode* getProperty(const String& propPath) const;

    virtual PropertyTreeEventFactory& eventFactory() { return evf_; }
    virtual DynamicEventFactory& updateFinishFactory()
        { return updateFactory_; }
    virtual PropertyNode*       root() const { return evf_.root(); }
    virtual void                save();
    virtual void                init(int argc = 0, const char* const* argv = 0,
                                     const String& cfgTplFile = String::null());
    virtual void                merge(const String& cfgTplFile);
    virtual String              getHomeDir() const;
    virtual String              getConfigDir() const { return configDir_; }
    virtual String              getDataDir()   const;

    virtual String              resolveResource(const String& defaultPropName,
            const String& src, const String& baseUri,
            const Common::PropertyNode*) const;
    virtual void                update();
    virtual String              configVersion() const
    {
        return SERNA_CONFIG_VERSION;
    }
private:
    void                        resolve_init_variables(PropertyNode* ptn);

    PropertyTreeEventFactory    evf_;
    DynamicEventFactory         updateFactory_;
    PropertyNodePtr             nullProperty_;
    String                      dataDir_;
    String                      configFile_;
    String                      configDir_;
};

String ConfigImpl::getHomeDir() const
{
    return get_home_dir();
}

ConfigImpl::ConfigImpl()
    : nullProperty_(new PropertyNode(NOTR("No-Such-Property")))
{
}

static void apply_prop_map(ConfigPropMapTable* pmt,
                           PropertyNode* old_config, PropertyNode* new_config)
{
    const char** p = pmt->pmap;
    for (; *p; p += 2) {
        PropertyNode* old_prop = old_config->getProperty(*p);
        if (0 == old_prop)
            continue;
        new_config->makeDescendant(p[1])->merge(old_prop, true);
    }
}

static void merge_old_props(PropertyNode* old_config,
                            PropertyNode* new_config)
{
    extern ConfigPropMapTable prop_map_tables[];
    double my_version = String(SERNA_CONFIG_VERSION).toDouble();
    ConfigPropMapTable* pmt = prop_map_tables;
    for (; pmt->version; ++pmt) {
        if (String(pmt->version).toDouble() >= my_version)
            break;
        apply_prop_map(pmt, old_config, new_config);
    }
}

void ConfigImpl::init(int argc, const char* const* argv,
                      const String& cfgTplFile)
{
    DBG(UTILS.CONFIG) << "CONFIG: init()\n";

    const PropertyNode* pn = 0;
    PropertyNode* p_root = evf_.root();
    bool  mergeVersions = false;

    // 1. read environment (for SERNA_CONFIG_DIR or/and SERNA_DATA_DIR)
    PropertyNodePtr env(new PropertyNode);
    PropUtils::readEnvProperties(env.pointer(), SERNA_VAR_PREFIX);

    DBG(UTILS.CONFIG) << "CONFIG: argc=<" << argc
                      << "> argv[0]=<" << (argv ? argv[0] : "null") << ">\n";

    PropertyNodePtr args(parse_cmdline(argc, argv));
    if (&*args) {
        using CmdLineParams::CMD_LINE_PARAMS;
        if (PropertyNode* cfgDir = args->getProperty(SERNA_CONFIG_DIR_ENVVAR))
            env->makeDescendant(SERNA_CONFIG_DIR_ENVVAR)->setValue(cfgDir);
        if (PropertyNode* cfgFile = args->getProperty(SERNA_CONFIG_FILE_ENVVAR))
            env->makeDescendant(SERNA_CONFIG_FILE_ENVVAR)->setValue(cfgFile);

        DBG_IF(UTILS.CONFIG) args->dump();
    }
    DBG_IF(UTILS.CONFIG) env->dump();

    // 2. try to open config dir. if not found, try to create it.
    configDir_ = get_config_dir(&*env);
    if (!PathName::exists(configDir_))
        FileUtils::mkdirp(configDir_);
    configFile_ = get_config_file(&*env);

    DBG(UTILS.CONFIG) << "CONFIG: CONFIG_DIR=<" << configDir_
                      << "> CONFIG_FILE=<" << configFile_ << ">\n";

    // 3. if config file exists, read it
    if (PathName::exists(configFile_)) {
        PropUtils::PropertyTreeSaver psaver(p_root, SERNA_XMLCONFIG_ROOT);
        if (!psaver.readPropertyTree(configFile_)) {
            DBG(UTILS.CONFIG) << "CONFIG: Failed to read config file\n";
            // if error occured, ignore and re-save config
            p_root->removeAllChildren();
        } else {
            if (!p_root->getProperty("erase-config-on-startup")) {
                if (p_root->getSafeProperty("version")->getString() ==
                    SERNA_CONFIG_VERSION) {
                        if (&*args)
                            p_root->appendChild(&*args);
                        fill_file_handlers_list(p_root);
                        return; // OK
                }
                else
                    mergeVersions = true;
            } else {
                PropertyNodePtr license =
                    p_root->getProperty("app/license-file");
                p_root->removeAllChildren();
                if (license)
                    p_root->makeDescendant(NOTR("app/license-file"),
                                           license->getString());
            }
        }
    }
    // 4. merge values from environment
    p_root->merge(env.pointer(), false);
    if (&*args)
        p_root->appendChild(&*args);

    // don't leave config file property in the config itself
    if (PropertyNode* cfProp = p_root->getProperty(SERNA_CONFIG_FILE_ENVVAR))
        cfProp->remove();

    // 5. determine SERNA_DATA_DIR (= installation directory)
    pn = p_root->getProperty(SERNA_DATA_DIR_PROPERTY);
    if (pn)
        dataDir_ = pn->getString();
    else
        dataDir_ = get_data_dir();

    DBG(UTILS.CONFIG) << "CONFIG: data_dir=<" << dataDir_ << ">\n";

    // 6. merge builtin default values from config template
    PropertyNodePtr tplProps(read_from_template(dataDir_, configDir_,
                                                cfgTplFile));
    if (mergeVersions)
        merge_old_props(p_root, tplProps.pointer());
    p_root->merge(tplProps.pointer(), true);

    // 7. Fill in file handlers list
    fill_file_handlers_list(p_root);
    p_root->makeDescendant("version")->setString(SERNA_CONFIG_VERSION);
    save();
    evf_.clearUpdates();
}

void ConfigImpl::merge(const String& cfgTplFile)
{
    PropertyNodePtr tplProps(read_from_template(dataDir_, configDir_,
                                                cfgTplFile));
    evf_.root()->merge(tplProps.pointer(), true);
    save();
    evf_.clearUpdates();
}

void ConfigImpl::resolve_init_variables(PropertyNode* ptn)
{
    CfgUtils::resolve_init_variables(ptn, dataDir_, configDir_);
}

const PropertyNode* ConfigImpl::getProperty(const String& propPath) const
{
    const PropertyNode* p = evf_.root()->getProperty(propPath);
    if (0 == p)
        return nullProperty_.pointer();
    return p;
}

void ConfigImpl::save()
{
    PropUtils::PropertyTreeSaver p_root(evf_.root(), SERNA_XMLCONFIG_ROOT);
    p_root.savePropertyTree(configFile_);
}

String ConfigImpl::resolveResource(const String& defaultPropName,
                                   const String& src,
                                   const String& baseUri,
                                   const PropertyNode* localVars) const
{
    DBG(UTILS.CONFIG) << "ResolveRes: defprop=<" << defaultPropName
        << "> src=<" << src << "> base=<" << baseUri << ">\n";
    // 1. path is empty - return default value
    if (src.isEmpty()) {
        DBG(UTILS.CONFIG) << "Empty src: default prop: "
            << defaultPropName << " getProp: "
            << getProperty(defaultPropName)->getString() << std::endl;
        return getProperty(defaultPropName)->getString();
    }
    Url src_url(src);
    // 2. path is absolute - return it
    if (!src_url.isRelative())
        return src;

    Vector<String> altpaths;
    // 3. if path does not contain varables, append baseUri
    //    resolve variables, if any, from vars section in config.
    PropertyTree merged_vars;
    PropertyNode* vars = evf_.root()->getProperty("vars");
    if (vars)
        merged_vars.root()->merge(vars, true);
    if (localVars)
        merged_vars.root()->merge(localVars, true);
    if (!PropUtils::resolveVariable(altpaths, merged_vars.root(), src,
                                    NOTR("serna_")/*SERNA_VAR_PREFIX*/))
        return Url(baseUri).combinePath2Path(src);

    // Check all possible locations.
    for (uint i = 0; i < altpaths.size(); ++i)
        if (Url(altpaths[i]).exists())
            return altpaths[i];
    if (altpaths.size())
        return altpaths[0]; // if not found, return first alternative
    return String();
}

void ConfigImpl::update()
{
    eventFactory().update();
    updateFinishFactory().dispatchEvent();
}

String ConfigImpl::getDataDir() const
{
    if (!dataDir_.isEmpty())
        return dataDir_;
    return root()->getSafeProperty(NOTR("vars/") +
        String(SERNA_DATA_DIR_PROPERTY))->getString();
}


//////////////////

Config& config()
{
    return SingletonHolder<ConfigImpl, CreateUsingNew<ConfigImpl>,
        NoDestroy<ConfigImpl> >::instance();
}

UTILS_EXPIMP void set_trace_tags()
{
#ifdef DEBUG_TRACE
    PropertyNode* property =
        config().root()->getProperty("debug/use_trace_tags");
    const uint mode = (property) ? property->getInt() : 0;
    if (0 == mode)
        return;
    property = config().root()->getProperty("debug/trace_tags_1");
    if (1 == mode && property) {
        DBG_SET_TAGS(property->getString().utf8().c_str(), true);
        return;
    }
    property = config().root()->getProperty("debug/trace_tags_2");
    if (2 == mode && property)
        DBG_SET_TAGS(property->getString().utf8().c_str(), true);
#endif // DEBUG_TRACE
}

/////////////////

UTILS_EXPIMP QString tr(const char* sourceText, const char* context)
{
    return QObject::tr(sourceText, context);
}
