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

#include "common/String.h"
#include "common/StringUtils.h"
#include "common/StringCvt.h"
#include "common/MessageUtils.h"
#include "common/Message.h"
#include "common/PathName.h"
#include "common/PropertyTree.h"


#include "grove/GroveBuilder.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityReferenceTable.h"
#include "grove/Nodes.h"
#include "grove/Grove.h"

#include "utils/file_utils.h"
#include "utils/struct_autosave_utils.h"
#include "utils/Config.h"
#include "utils/utils_debug.h"
#include "dav/DavManager.h"

#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QLinkedList>
#include <QDateTime>
#include <QApplication>
#include <QMessageBox>

#include <fstream>
#include <list>

#ifdef DBG_DEFAULT_TAG
# undef DBG_DEFAULT_TAG
#endif
#define DBG_DEFAULT_TAG UTILS.TEST

namespace AutoSaveUtils {

using namespace Common;
using namespace GroveLib;

//////////////////////////////////////////////////////////////////

Sysids::Sysids()
 :  root_(config().root()->makeDescendant("app/autosave/not-saved"))
{
}

PropertyNode* Sysids::first() const
{
    return root_->firstChild();
}

PropertyNode* Sysids::find(const String& sysid) const
{
    for (PropertyNode* pn = first(); 0 != pn; pn = pn->nextSibling()) {
        if (NOTR("sysid") == pn->name() && sysid == pn->getString())
            return pn;
    }
    return 0;
}

void Sysids::add(const String& sysid) const
{
    PropertyNodePtr new_entry(new PropertyNode(from_latin1(NOTR("sysid")),
                                               sysid));
    root_->appendChild(new_entry.pointer());
    config().save();
}

void Sysids::remove(const String& sysid) const
{
    if (PropertyNode* pn = find(sysid)) {
        pn->remove();
        config().save();
    }
}

void Sysids::clear() const
{
    if (PropertyNode* pn = first()) {
        for (; 0 != pn; pn = first())
            pn->remove();
        config().save();
    }
}

//////////////////////////////////////////////////////////////////

static unsigned get_time_t(const String& uri)
{
    PropertyTree pt;
    if (Dav::DavManager::instance().getResourceInfo(uri, pt.root()) !=
        Dav::DAV_RESULT_OK)
            return 0;
    return pt.root()->getSafeProperty("modtime")->getString().toUInt();
}

struct EntityInfo {
    String      origSysid_;
    String      saveSysid_;
    EntityReferenceStart* ers_;
    unsigned    modTime_;
    bool        isModified_;
    bool        newerThanOrig_;
    EntityInfo(const String& sysid, EntityReferenceStart* ers, bool isModified)
      : origSysid_(sysid), ers_(ers), modTime_(get_time_t(origSysid_)),
        isModified_(isModified), newerThanOrig_(false) {}
    EntityInfo()
        : ers_(0), modTime_(0), isModified_(0), newerThanOrig_(false) {}
};

typedef std::list<EntityInfo> EntityUriMap;

static String make_entity_path(const ustring& dir, int num)
{
    ustring result(from_latin1(NOTR("entity-")));
    result.append(from_integer<ustring>(num)).append(from_latin1(NOTR(".xml")));
    return PathName(dir).append(result).name();
}

static QString make_auto_save_path(const String& topSysid)
{
    Url top_url(topSysid);
    String dirname = "." + top_url[Url::FILENAME] + NOTR("-autosave");
    if (top_url[Url::PROTOCOL] == NOTR("file"))
        return String(top_url.combinePath2Path(dirname));
    dirname = top_url[Url::PROTOCOL] + top_url[Url::HOST] +
        top_url[Url::DIRPATH].replace(String('/'), String('_')).
            replace(String('~'), String('_')).
            replace(String('%'), String('_'))+ dirname;
    DDBG << "dav auto-save path: " << dirname << std::endl;
    return config().getConfigDir() + PathName::DIR_SEP + dirname;
}

static QDir make_save_dir(const String& topSysid)
{
    QString saveDir(make_auto_save_path(topSysid));
    QDir dir(saveDir);
    if (dir.exists() || dir.mkdir(saveDir))
        dir.cd(saveDir);

    return dir;
}

static nstring get_line(std::istream& is)
{
    nstring buf;
    IsSpace<char> wsPr;
    while (is) {
        getline(is, buf, '\n');
        NCRange r = find_longest_range_between(make_range(buf), wsPr);
        if (r.empty())
            continue;
        return nstring(r.begin(), r.end());
    }
    return nstring();
}

static bool
get_num(SepRangeIter<char>& iter, const nstring& line, unsigned& result)
{
    if (line.end() == iter)
        return false;
    bool ok = false;
    unsigned rv = to_integer<unsigned>(*iter, &ok);
    ++iter;
    if (ok)
        result = rv;
    return ok;
}

static bool
get_str(SepRangeIter<char>& iter, const nstring& line, ustring& result)
{
    if (line.end() == iter)
        return false;
    result.assign(iter->begin(), iter->end());
    ++iter;
    return true;
}

static bool parse_entity_info(EntityUriMap& uriMap,
                              const nstring& line,
                              const PathName& path)
{
    EntityInfo ei;
    SepRangeIter<char> info_iter(make_range(line), '\t');
    unsigned mod = 0;

    if (get_num(info_iter, line, mod))
        ei.isModified_ = (mod > 0);
    else
        return false;

    if (!get_str(info_iter, line, ei.saveSysid_))
        return false;

    if (!get_num(info_iter, line, ei.modTime_))
        return false;

    if (!get_str(info_iter, line, ei.origSysid_))
        return false;

    if (PathName::exists(ei.saveSysid_))
        return true;
    ei.saveSysid_ = PathName(path).dirname().append(ei.saveSysid_).name();
    ei.saveSysid_.append(from_latin1(NOTR(".xml")));
    uriMap.push_back(ei);
    return true;
}

static bool restore_from_entity_map(EntityUriMap& uriMap)
{
    DDBG << "restore_from_entity_map" << std::endl;
    EntityUriMap::iterator it(uriMap.begin());
    QStringList filesToAsk;
    for (; uriMap.end() != it; ++it) {
        it->newerThanOrig_ = true;
        unsigned int orig_time = get_time_t(it->origSysid_);
        if (0 == orig_time)
            it->isModified_ = true;
        else {
            if (it->modTime_ < orig_time) {
                it->newerThanOrig_ = false;
                filesToAsk.push_back(to_string<QString>(it->origSysid_));
            }
        }
    }
    bool globalRestore = false;
    if (!filesToAsk.isEmpty()) {
        QString question(tr("The following files were modified after the " \
                            "last autosave:"));
        question.append(NOTR("<b><br/>") + filesToAsk.join(QChar('\n')));
        question.append(NOTR("</b><br/>") + tr("Do you want to restore them " \
                                               "from the last autosave?"));
        int result = QMessageBox::question(qApp->activeWindow(),
                                           tr("Restore autosaved document"),
                                           question, 
                                           QMessageBox::Yes|QMessageBox::No,
                                           QMessageBox::No);
        globalRestore = (QMessageBox::Yes == result);
    }
    bool rv = true;
    for (it = uriMap.begin(); uriMap.end() != it; ++it) {
        if ((it->newerThanOrig_ && it->isModified_) ||
            (!it->newerThanOrig_ && globalRestore)) {
            DDBG << "copy from " << sqt(it->saveSysid_)
                 << " to " << sqt(it->origSysid_) << std::endl;
            rv = rv && FileUtils::copy_file(it->saveSysid_, it->origSysid_);
        }
    }
    return rv;
}

static EntityUriMap load_entity_map(const String& srcdir)
{
    DDBG << "load_entity_map from " << sqt(srcdir) << std::endl;
    EntityUriMap result;
    PathName path(srcdir);
    if (!path.exists())
        return result;
    path.append(NOTR("entity.map"));
    if (!path.exists())
        return result;
    std::ifstream ifs(latin1(path.name()).c_str());
    for (nstring line(get_line(ifs)); !line.empty(); line = get_line(ifs)) {
        parse_entity_info(result, line, path);
    }
    return result;
}

static void
write_entity_map_entry(std::ostream& os, int num,
                       const EntityInfo& entry)
{
    os << entry.isModified_ << NOTR("\tentity-") << num << "\t"
       << entry.modTime_ << NOTR("\t")
       << entry.origSysid_ << std::endl;
}

static bool save_entity_map(EntityUriMap& uriMap,
                            Grove* grove)
{
    QDir dir(make_save_dir(grove->topSysid()));
    DDBG << "save_entity_map to '" << dir.absPath() << '\'' << std::endl;
    if (!dir.exists())
        return false;
    DDBG << "save_entity_map: entity map abspath = "
         << dir.filePath("entity.map").latin1() << std::endl;
    std::ofstream ofs(dir.filePath(NOTR("entity.map")).latin1());

    ustring baseDir(from_string<ustring>(dir.absPath()));
    EntityUriMap::iterator it(uriMap.begin());
    for (unsigned int idx = 0; uriMap.end() != it; ++it, ++idx) {
        EntityReferenceStart* ers = it->ers_;
        if (ers->entityDecl()->declType() == EntityDecl::document) {
            if (ers->grove()->saveAsXmlFile(Grove::GS_SAVE_PROLOG|
                Grove::GS_SAVE_CONTENT|Grove::GS_FORCE_SAVE,
                0, make_entity_path(baseDir, idx)))
                    write_entity_map_entry(ofs, idx, *it);
            continue;
        }
        String entityFile(make_entity_path(baseDir, idx));
        if (ers->entityDecl()->declType() != EntityDecl::externalGeneralEntity)
            continue;
        if (ers->saveAsXmlFile(Grove::GS_FORCE_SAVE, 0, entityFile))
            write_entity_map_entry(ofs, idx, *it);
    }
    return true;
}

static void check_config(const String& sysid, Common::PropertyNode* dsi = 0)
{
    DDBG << "check_config(), sysid=" << sysid << ", dsi=" << dsi << std::endl;
    if (dsi && dsi->getProperty("#not-saved")) {
        Sysids sysids;
        if (0 == sysids.find(sysid)) {
            DDBG << "check_config(), adding: " << sysid << std::endl;
            sysids.add(sysid);
        }
    }
}

static void fill_entity_map(EntityUriMap& uriMap, Grove* grove, bool isModified)
{
    EntityReferenceTable* ertp = grove->document()->ert();
    if (0 == ertp)
        return;
    DDBG << "make_auto_save: grove->topSysid() = " << grove->topSysid()
         << std::endl;
    String topSysid(grove->topSysid());
    DDBG << "make_auto_save: topSysId = " << grove->topSysid() << std::endl;
    uriMap.push_back(EntityUriMap::value_type(grove->topSysid(),
        grove->document()->ers(), isModified));
    EntityReferenceTable::ErtTable::iterator it = ertp->begin();
    for (; it != ertp->end(); ++it) {
        const ExternalEntityDecl* ee =
            (*it)->decl()->asConstExternalEntityDecl();
        if (ee)
            uriMap.push_back(EntityInfo((*it)->node(0)->xmlBase(),
                (*it)->node(0), ee->isContentModified()));
    }
}

UTILS_EXPIMP bool make_auto_save(Grove* grove, Common::PropertyNode* dsi)
{
    EntityUriMap uriMap;
    fill_entity_map(uriMap, grove, grove->topDecl()->isContentModified());
    for (Grove* g = grove->firstChild(); g; g = g->nextSibling()) {
        bool subgrove_modified = false;
        EntityReferenceTable* ert = g->document()->ert();
        EntityReferenceTable::ErtTable::iterator ertIter = ert->begin();
        EntityDecl* decl = 0;
        if (g->topDecl()->isContentModified())
            subgrove_modified = true;
        for (; !subgrove_modified && ertIter != ert->end(); ++ertIter) {
            decl = (*ertIter)->decl();
            if (decl->isContentModified() || decl->isDeclModified())
                subgrove_modified = true;
        }
        DDBG << "autosave: traversed subgrove " << g->topSysid()
            << ", modified=" << subgrove_modified << std::endl;
        if (subgrove_modified)
            fill_entity_map(uriMap, g, 1);
    }
    if (uriMap.empty())
        return true;
    bool rv = save_entity_map(uriMap, grove);
    if (rv) {
        check_config(grove->topSysid(), dsi);
        for (Grove* g = grove->firstChild(); g; g = g->nextSibling())
            check_config(g->topSysid());
    }
    return rv;
}

UTILS_EXPIMP bool restore_auto_save(const String& topSysid)
{
    EntityUriMap uriMap(load_entity_map(make_auto_save_path(topSysid)));
    if (!uriMap.empty()) {
        if (get_time_t(uriMap.front().origSysid_)) {
            QString q;
            q = tr("This document was not saved properly. Recover document?");
            int result = QMessageBox::question(qApp->activeWindow(),
                                               tr("Restore autosaved document"),
                                               q, 
                                               QMessageBox::Yes|QMessageBox::No,
                                               QMessageBox::No);
            if (QMessageBox::Yes != result)
                return true;
        }
        return restore_from_entity_map(uriMap);
    }
    return true;
}

static bool is_dot2(const QString& path)
{
    const unsigned len = path.length();
    return (1 == len && '.' == path.at(0)) ||
           (2 == len && '.' == path.at(0) && '.' == path.at(1));
}

static bool recursive_remove(QDir& dir)
{
    DDBG << "recursive_remove for " << dir.absPath() << std::endl;
    bool res = true;
    dir.setFilter(QDir::All | QDir::Hidden | QDir::System);
    QFileInfoList fiList(dir.entryInfoList());
    QFileInfoList::iterator it = fiList.begin();
    for (; fiList.end() != it; ++it) {
        QFileInfo* fi = &*it;
        QString fileName(fi->fileName());
        if (is_dot2(fileName))
            continue;
        if (!fi->isSymLink() && fi->isDir()) {
            QDir subdir(fi->absFilePath());
            res = res && recursive_remove(subdir);
        }
        else
            res = res && dir.remove(fileName);
    }
    return dir.rmdir(dir.absPath());
}

UTILS_EXPIMP bool clean_auto_save(const String& topSysid)
{
    DDBG << "clean_auto_save for " << topSysid << std::endl;
    QString path(make_auto_save_path(topSysid));
    QDir dir(path);

    Sysids().remove(topSysid);
    if (!dir.exists())
        return true;
    return recursive_remove(dir);
}

}
