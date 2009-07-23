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

#include "impl/file_utils_pvt.h"
#include "impl/config_utils.h"
#include "utils/file_utils.h"
#include "utils/DocSrcInfo.h"
#include "utils/Properties.h"
#include "utils/Config.h"
#include "sys_deps.h"
#include "utils/utils_debug.h"

#include "grove/Nodes.h"
#include "grove/Grove.h"
#include "grove/EntityDeclSet.h"
#include "groveeditor/GrovePos.h"
#include "dav/DavManager.h"
#include "dav/IoStream.h"
#include "common/Url.h"
#include "common/PathName.h"
#include "common/String.h"
#include "common/PropertyTree.h"
#include "common/StringCvt.h"

#include <QDir>
#include <qprocess.h>
#include <qapplication.h>
#include <fstream>
#include <vector>

#if !defined(_WIN32)
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
#endif

using namespace Common;
using namespace GroveLib;
//
// START_IGNORE_LITERALS
//
namespace FileUtils {

#if !defined(_WIN32)

String get_default_doc_dir()
{
    return CfgUtils::get_home_dir();
}

#endif

bool unlink(const String& path)
{
    return QFile::remove(to_string<QString>(path));
}

bool rename(const String& oldpath, const String& newpath)
{
    return QDir().rename(to_string<QString>(oldpath),
                         to_string<QString>(newpath));
}

static bool qmkdirp(const QDir& dstdir)
{
    if (dstdir.exists())
        return true;
    QDir parentDir(QFileInfo(dstdir.absPath()).dir());
    if (parentDir.absPath() != dstdir.absPath() && qmkdirp(parentDir))
        return dstdir.mkdir(dstdir.absPath(), true);
    return false;
}

bool mkdirp(const String& dir)
{
    return qmkdirp(QDir(to_string<QString>(dir)).absPath());
}

/////////////////////////////////////////////////////////////////////

static bool modtime_check_disabled()
{
    return config().root()->getSafeProperty(App::APP)->
        getSafeProperty(App::DISABLE_DOCUMENT_MODTIME_CHECK)->getBool();
}

String getFileModifiedTime(const Url& url)
{
    if (!url.isValid() || url[Url::PROTOCOL] != "file")
        return String::null();
    PropertyTree pt;
    if (Dav::DavManager::instance().getResourceInfo(url, pt.root()) !=
        Dav::DAV_RESULT_OK)
            return String::null();
    return pt.root()->getSafeProperty("modtime")->getString();
}

bool isFileModified(const Url& url,
                    const String& lastTime)
{
    DBG(UTILS.TEST) << "isFileModified: url=" << String(url)
        << ", lastTime=" << lastTime << std::endl;
    if (!url.isValid() || url[Url::PROTOCOL] != "file")
        return false;
    bool ok = false;
    unsigned int last_time = lastTime.toUInt(&ok);
    if (!ok)
        return false;
    PropertyTree pt;
    if (Dav::DavManager::instance().getResourceInfo(url, pt.root()) !=
        Dav::DAV_RESULT_OK)
            return false;
    unsigned int urlTime =
        pt.root()->getSafeProperty("modtime")->getString().toUInt(&ok);
    if (!ok)
        return false;
    DBG(UTILS.TEST) << "isFileModified: url " << String(url) << ", url time="
        << urlTime << ", lastTime=" << lastTime
        << std::endl;
    return urlTime > (1 + last_time);
}

void doEntityTimeStamp(const Grove* grove,
                       PropertyNode* mtime,
                       const ExternalEntityDecl* eed,
                       bool disable_mcheck)
{
    String entityPath = eed->entityPath(grove);
    PropertyNode* went = new PropertyNode(DocSrcInfo::WATCHED_ENTITY);
    went->makeDescendant(DocSrcInfo::WATCHED_ENTITY_PATH)->
        setString(entityPath);
    if (!disable_mcheck)
        went->makeDescendant(DocSrcInfo::WATCHED_ENTITY_TIME)->
            setString(getFileModifiedTime(entityPath));
    went->makeDescendant("entity-name")->setString(eed->name());
    bool is_rel = Url(eed->sysid()).isRelative();
    went->makeDescendant("is-relative")->setBool(is_rel);
    mtime->appendChild(went);
}

static void do_grove_time_stamps(const Grove* grove,
                                 PropertyNode* mtime,
                                 bool disable_mcheck)
{
    DBG(UTILS.TEST) << "DoGroveTimeStamps, grove=" << grove
        << ", disable_mcheck=" << disable_mcheck << std::endl;
    if (!grove || !mtime)
        return;
    PropertyNode* wf_prop = new PropertyNode(DocSrcInfo::WATCHED_FILE);
    wf_prop->makeDescendant(DocSrcInfo::WATCHED_ENTITY_PATH)->
        setString(grove->topSysid());
    if (!disable_mcheck)
        wf_prop->makeDescendant(DocSrcInfo::WATCHED_ENTITY_TIME)->
            setString(getFileModifiedTime(grove->topSysid()));
    mtime->appendChild(wf_prop);
    EntityDeclSet::iterator iter = grove->entityDecls()->begin();
    for (; iter != grove->entityDecls()->end(); ++iter) {
        if ((*iter)->declType() == EntityDecl::externalGeneralEntity)
            doEntityTimeStamp(grove, mtime,
                (*iter)->asConstExternalEntityDecl(), disable_mcheck);
    }
    iter = const_cast<Grove*>(grove)->parameterEntityDecls().begin();
    for (;iter != const_cast<Grove*>(grove)->parameterEntityDecls().end();
        ++iter) {
        if ((*iter)->declType() == EntityDecl::externalParameterEntity &&
            (*iter)->declOrigin() == EntityDecl::prolog)
            doEntityTimeStamp(grove, mtime,
                (*iter)->asConstExternalEntityDecl(), disable_mcheck);
    }
}

void doTimeStamps(const Grove* grove, PropertyNode* mtime)
{
    mtime->removeAllChildren();
    bool disable_modtime_checks = modtime_check_disabled();
    do_grove_time_stamps(grove, mtime, disable_modtime_checks);
    for (grove = grove->firstChild(); grove; grove = grove->nextSibling())
        do_grove_time_stamps(grove, mtime, disable_modtime_checks);
}

String check_document_reload(Common::PropertyNode* dsi)
{
    String need_reload;
    if (modtime_check_disabled())
        return need_reload;
    PropertyNode* node =
        dsi->makeDescendant(DocSrcInfo::MODIFIED_TIME)->firstChild();
    for (; node; node = node->nextSibling()) {
        const PropertyNode* path =
            node->getSafeProperty(DocSrcInfo::WATCHED_ENTITY_PATH);
        PropertyNode* time =
            node->makeDescendant(DocSrcInfo::WATCHED_ENTITY_TIME);
        if (isFileModified(path->getString(), time->getString())) {
            time->setString(getFileModifiedTime(path->getString()));
            need_reload += "\n" + path->getString();
        }
    }
    return need_reload;
}

const String& argv0(const char* path)
{
    static String& ARGV0(*new String); // intentional memory leak
    if (0 != path)
        ARGV0 = full_exe_path(from_local_8bit(path));
    return ARGV0;
}


/**
 * copies one file to another. destination is overwritten
 *
 * @param src    source
 * @param dstdir destination
 *
 * @return destination
 */
bool copy_file(const String& srcfile, const String& dstfile)
{
    Url src_url(srcfile), dst_url(dstfile);
    if (src_url == dst_url)
        return true;
    if (src_url[Url::PROTOCOL] == "file" && dst_url[Url::PROTOCOL] == "file") {
        if (PathName::isSameFile(srcfile, dstfile))
            return true;
#ifdef _WIN32
        LPCWSTR nsrc = reinterpret_cast<LPCWSTR>(srcfile.c_str());
        LPCWSTR ndst = reinterpret_cast<LPCWSTR>(dstfile.c_str());
        if (CopyFileW(nsrc, ndst, FALSE))
            return true;
#else
        nstring ndst(local_8bit(dstfile));
        nstring nsrc(local_8bit(srcfile));

        std::ifstream ifs(nsrc.c_str());
        std::ofstream ofs(ndst.c_str());
        if (ifs && ofs) {
            ofs << ifs.rdbuf();
            if (ofs) {
                ofs.close();
                struct stat st;
                if (0 == stat(nsrc.c_str(), &st))
                    chmod(ndst.c_str(), st.st_mode);
                return true;
            }
            else
                unlink(ndst.c_str());
        }
#endif
    }
    std::vector<char> io_buffer(1024*64);
    Dav::DavManager& dav_mgr = Dav::DavManager::instance();
    Dav::IoStream ifs, ofs;
    if (dav_mgr.open(src_url, Dav::DAV_OPEN_READ, ifs) != Dav::DAV_RESULT_OK)
        return false;
    if (dav_mgr.open(dst_url, Dav::DAV_OPEN_WRITE, ofs) != Dav::DAV_RESULT_OK)
        return false;
    int n = 0;
    bool ok = true;
    do {
        n = ifs.readRaw(io_buffer.size(), &io_buffer[0]);
        if (n < 0)
            return false;
        if (n != 0)
            ok &= (n == ofs.writeRaw(n, &io_buffer[0]));
    } while(n);
    ok &= (ofs.close() == Dav::DAV_RESULT_OK);
    return ok;
}

// input:  full long path name
// output: corresponding short name (if any)
String get_short_path(const String& path)
{
#ifdef _WIN32
    QString native_path = QDir::convertSeparators(path);

    unsigned len = native_path.length();
    std::auto_ptr<WCHAR> ph(new WCHAR[len + 1]);
    memcpy(ph.get(), native_path.unicode(), len * sizeof(WCHAR));
    *(ph.get() + len) = '\0';
    len = GetShortPathNameW(ph.get(), ph.get(), len + 1);
    QString short_path = QString::fromUcs2(
                         reinterpret_cast<unsigned short*>(ph.get()));
    //if (0 < len)// && QFileInfo(short_path).exists())
    //    return short_path;
    //else
        return '"' + native_path + '"';
#endif
    return path;
}

static inline const String& get_prop(const PropertyNode* prop, const char* name)
{
    return prop->getSafeProperty(name)->getString();
}

static LaunchCode launch_handler_app(const String& app, const String& url,
                                     const String&)
{
    QProcess* proc = new QProcess(qApp);
    QStringList args;
    args.push_back(url);
    QObject::connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
        proc, SLOT(deleteLater()));
    return proc->startDetached(app, args) ? HANDLER_OK : HANDLER_ERROR;
}

static const PropertyNode* search_handler(const String& type,
                                          const String& propName)
{
    DBG(UTILS.TEST) << "search_handler: " << "type = " << type
                    << ", propName = " << propName << std::endl;
    const PropertyNode* handlers = config().root()->getSafeProperty(propName);
    if (handlers) {
        using namespace FileHandler;
        const PropertyNode* handler = handlers->firstChild();
        for (; handler; handler = handler->nextSibling())
            if (APP == handler->name() && get_prop(handler, APP_EXT) == type) {
                DBG(UTILS.TEST) << "search found: "
                                << get_prop(handler, APP_PATH) << std::endl;
                return handler;
            }
    }
    return 0;
}

const PropertyNode* get_handler(const String& urlString, const String& type)
{
    DBG(UTILS.TEST) << "get_handler: " << "url = " << urlString
                    << ", type = " << type << std::endl;
    Url url(urlString);
    String extension;
    if (url.isLocal()) {
        if (type.empty())
            extension = PathName(url).extension();
        else if ('.' == type[0])
            extension = type.substr(1);
        else
            extension = type;
    }
    else {
        String scheme(to_lower<String>(url[Url::PROTOCOL]));
        if (starts_with(scheme, "http") || starts_with(scheme, "ftp"))
            extension = "html";
    }

    using namespace FileHandler;
    if (!extension.empty()) {
        if (const PropertyNode* handler = search_handler(extension,
                                                         HANDLER_LIST))
            return handler;
        return search_handler(extension, HANDLER_LIST_TEMP);
    }
    return 0;
}

LaunchCode launch_file_handler(const String& url, const String& type,
                               const String& action)
{
    using namespace FileHandler;
    if (url.empty())
        return HANDLER_BADURL;
    if (const PropertyNode* handler = get_handler(url, type))
        return launch_handler_app(get_prop(handler, APP_PATH), url, action);
    return os_launch_handler_app(url, type, action);
}

}
