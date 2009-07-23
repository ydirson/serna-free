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

/** \file
 */

#if !defined(USE_PCH)
# include "catmgr/catmgr_defs.h"
# include "XMLCatalogManager.h"
# include "CatalogHolder.h"
# include "Catalog.h"
# include "UriResolver.h"
# include "ExternalIdResolver.h"
# include "CatMgrMessages.h"
# include "CatMgrMessageUtils.h"
# include "catmgr_utils.h"
# include "catmgr_debug.h"
# include "ResolverOptions.h"

# include "common/String.h"
# include "common/StringCvt.h"
# include "common/RangeIterator.h"
# include "common/Message.h"
# include "common/MessageUtils.h"
# include "common/Vector.h"
# include "common/PathName.h"
# include "common/StringUtils.h"

# include "grove/Grove.h"
# include "spgrovebuilder/SpGroveBuilder.h"

# include <list>
# include <stdlib.h>
# include <string.h>
#else
# include "catmgr_pch.hpp"
#endif

namespace CatMgrImpl {

using namespace Common;
using namespace GroveLib;
using std::endl;

XMLCatalogManager::XMLCatalogManager() {}

XMLCatalogManager::XMLCatalogManager(const ResolverOptions& opts)
 :  ResolverOptions(opts)
{
}

XMLCatalogManager::~XMLCatalogManager()
{
    clearCatalogs();
}

void XMLCatalogManager::setMessenger(Messenger* msgr)
{
    messenger_ = msgr;
}

Messenger* XMLCatalogManager::getMessenger() const
{
    return &*messenger_;
}

struct Res {
    Res(Resolver& r) : resolver_(r) {}
    bool operator()(Catalog* cat) { return resolver_.visit(*cat); }
private:
    Resolver& resolver_;
};

String
XMLCatalogManager::resolveExternalId(const String& pubid, const String& sysid,
                                     Messenger* msgr) const
{
    DBG(CATMGR.TRACE) << "CatalogManager::resolveEID(), this:" << abr(this)
                      << ", pubid: " << pubid << ", sysid: " << sysid << endl;
    if (0 == msgr)
        msgr = getMessenger();
    String pubId, sysId;
    pubId = norm_pubid(in_urn_ns(pubid) ? unwrap_id(pubid) : pubid);
    if (in_urn_ns(sysid)) {
        sysId = norm_pubid(unwrap_id(sysid));
        if (pubId.empty())
            pubId = sysId;
        else if (pubId != sysId) {
            msg_router(msgr, CatMgrMessages::urnSysidAndPubidDiffer)
                << Message::L_WARNING << sysid << pubid;
        }
        sysId.clear();
    }
    else
        sysId = norm_uri(sysid);
    ExternalIdResolver resolver(pubId, sysId, getOptions(), msgr);
    std::find_if(catalogList_.begin(), catalogList_.end(), Res(resolver));
    const String& result(resolver.getUri());
        DBG(CATMGR.TRACE) << "CatalogManager::resolveEID(), this:" << abr(this)
                          << ", found: '" << result << "', pubid was: '"
                          << pubid << "', sysid was: '" << sysid << '\''
                          << endl;
    return result;
}

String
XMLCatalogManager::resolveUri(const String& uriref, Messenger* msgr) const
{
    if (0 == msgr)
        msgr = getMessenger();
    DBG(CATMGR.TRACE) << "CatalogManager::resolveUri(), this:" << abr(this)
                      << ", uriRef: " << uriref << endl;
    UriResolver resolver(uriref, getOptions(), msgr);
    std::find_if(catalogList_.begin(), catalogList_.end(), Res(resolver));
    const String& result(resolver.getUri());
    DBG(CATMGR.TRACE) << "CatalogManager::resolveUri(), this:" << abr(this)
                      << ", found: '" << result << "', uri was: '" << uriref
                      << '\'' << endl;
    return result;
}

bool XMLCatalogManager::setOption(const char* name, const char* value,
                                  Messenger*)
{
    return ResolverOptions::setOption(name, value);
}

const ResolverOptions& XMLCatalogManager::getOptions() const { return *this; }

bool XMLCatalogManager::add_catalog_file_path(const String& uri,
                                              Messenger* msgr)
{
    if (is_file_uri(uri) && !PathName(get_filepath(uri)).exists()) {
        msg_router(msgr, CatMgrMessages::catalogFileDoesNotExist)
            << Message::L_WARNING << uri;
    }
    else {
        String abs_uri = make_abs_uri(uri);
        Catalog* pCatalog = CatalogHolder::instance().addCatalog(abs_uri, msgr);
        if (0 != pCatalog) {
            catalogList_.push_back(pCatalog);
            return true;
        }
    }
    return false;
}

bool XMLCatalogManager::addCatalogList(const String& path, Messenger* msgr)
{
    if (path.empty())
        return false;
    if (0 == msgr)
        msgr = getMessenger();
    unsigned catalogsAdded = 0;
    SepRangeIter<Char> path_iter(make_range(path), ' ');
    for (String uri; path.end() != path_iter; ++path_iter) {
        uri.assign(path_iter->begin(), path_iter->size());
        DDBG << "XMLCatalogManager::addCatalogList, uri = " << uri << std::endl;
        if (add_catalog_file_path(uri, msgr))
            ++catalogsAdded;
    }
    return catalogsAdded > 0;
}

bool XMLCatalogManager::addCatalogList(const Common::Vector<String>& pathList,
                                       Messenger* msgr)
{
    if (0 == msgr)
        msgr = getMessenger();
    unsigned catalogsAdded = 0;
    for (unsigned int i = 0; i < pathList.size(); ++i) {
        if (add_catalog_file_path(pathList[i], msgr))
            ++catalogsAdded;
    }
    return catalogsAdded > 0;
}

bool XMLCatalogManager::clearCatalogs()
{
    CatalogList::const_iterator it = catalogList_.begin();
    for (; it != catalogList_.end(); ++it)
        CatalogHolder::instance().releaseCatalog((*it)->getUri());
    catalogList_.clear();
    return true;
}

CatMgr::CatalogManager* XMLCatalogManager::copy() const
{
    XMLCatalogManager* cat_copy = new XMLCatalogManager(getOptions());
    CatalogHolder& cat_holder = CatalogHolder::instance();
    CatalogList::const_iterator it = catalogList_.begin();
    for (; it != catalogList_.end(); ++it) {
        if (Catalog* catalog = cat_holder.addCatalog((*it)->getUri(), 0))
            cat_copy->catalogList_.push_back(catalog);
    }
    return cat_copy;
}

void XMLCatalogManager::getCatalogList(Common::Vector<String>& pathList)
{
    pathList.clear();
    CatalogList::const_iterator it = catalogList_.begin();
    for (; it != catalogList_.end(); ++it)
        pathList.push_back((*it)->getUri());
}

void get_catalog_props(Catalog* catalog, PropertyNode* prop)
{
    PropertyNode* cat_prop =
        new PropertyNode(NOTR("catalog"), catalog->getUri());
    prop->appendChild(cat_prop);
    NextCatalogList::const_iterator it = catalog->nextCatalogs()->begin();
    for (; it != catalog->nextCatalogs()->end(); ++it)
        get_catalog_props(*it, cat_prop);
}

void XMLCatalogManager::getCatalogTree(PropertyNode* root)
{
    if (!root)
        return;
    root->removeAllChildren();
    NextCatalogList::const_iterator it = catalogList_.begin();
    for (; it != catalogList_.end(); ++it)
        get_catalog_props(*it, root);
}


class GlobalCatalogManager : public XMLCatalogManager {};

XMLCatalogManager& XMLCatalogManager::instance()
{
    return SingletonHolder<GlobalCatalogManager>::instance();
}

}

