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
# include "CatalogBuilder.h"
# include "CatalogImpl.h"
# include "CatalogEntry.h"
# include "CatalogEntries.h"
# include "CatalogHolder.h"
# include "CatalogManager.h"
# include "ResolverOptions.h"
# include "CatMgrMessages.h"
# include "CatMgrMessageUtils.h"
# include "catmgr_utils.h"
# include "catmgr_debug.h"

# include "spgrovebuilder/SpGroveBuilder.h"
# include "grove/Grove.h"
# include "grove/Nodes.h"

# include "common/MessageUtils.h"
# include "common/String.h"
# include <stdlib.h>
# include <memory>
#else
# include "catmgr_pch.hpp"
#endif

// START_IGNORE_LITERALS

namespace CatMgrImpl {

using namespace Common;
using namespace GroveLib;
using namespace std;

CatalogBuilder::CatalogBuilder(Resolver& r, CatalogImpl& catalog)
 :  resolver_(r), catalog_(catalog)
{
}

bool CatalogBuilder::build()
{
    const String& uri(catalog_.getUri());
    GroveBuilder* gb(new SpGroveBuilder);
    Messenger* m(resolver_.getMessenger());
    gb->setMessenger(m);

    CatMgr::CatalogManager* catMgr = resolver_.getCatalogManager();
    std::auto_ptr<CatMgr::CatalogManager> cmgr;
    if (!catMgr) {
        cmgr.reset(CatMgr::global_catalog().copy());
        cmgr->clearCatalogs();
        String xmlCatPath(getenv("XML_CATALOG_FILES"));
        DBG(CATMGR.TRACE) << "CatalogBuilder::build(): XML_CATALOG_FILES="
                          << xmlCatPath << std::endl;
        cmgr->addCatalogList(xmlCatPath);
        catMgr = &*cmgr;
    }

    gb->setCatalogManager(catMgr);
    String spUri(is_file_uri(uri) ? get_filepath(uri) : uri);
    GrovePtr grove(gb->buildGroveFromFile(spUri, false));
    if (!grove)
        msg_router(m, CatMgrMessages::cantBuildGrove)<< Message::L_ERROR
            << String(uri);
    ElementPtr root(grove->document()->documentElement());
    if (!root || !in_catalog_ns(root.getPtr()) || root->nodeName() != "catalog")
        msg_router(m, CatMgrMessages::invalidCatalogFile)
            << Message::L_ERROR << String(uri);
    else
        return 0 < processElement(root.pointer(), get_base_uri(uri),
                                  ResolverOptions());
    return false;
}

unsigned CatalogBuilder::processElement(const Element* ep,
                                        const String& baseUri,
                                        ResolverOptions opts)
{
    if (!in_catalog_ns(ep))
        return false;
    const String& name(ep->nodeName());
    unsigned rv(0);
    if ("catalog" == name || "group" == name) {
        String xmlbase(rebase_uri(baseUri, get_attr(ep, "xml:base")));
        const String& preferAttr(get_attr(ep, "prefer"));
        if (preferAttr == "system")
            opts.setOption(ResolverOptions::PREFER_SYSTEM);
        else if (preferAttr == "public")
            opts.setOption(ResolverOptions::PREFER_PUBLIC);
        for (Node* np = ep->firstChild(); np; np = np->nextSibling()) {
            if (np->nodeType() == Node::ELEMENT_NODE) {
                ep = static_cast<const Element*>(np);
                rv += processElement(ep, xmlbase, opts);
            }
        }
        return rv;
    }
    if (CatalogEntry* cep = CatalogEntry::make(ep, baseUri, opts,
                                               resolver_.getMessenger())) {
        if (cep->accept(*this))
            return 1;
        delete cep;
    }
    return 0;
}

static inline const String*
uri_ptr(const SystemEntry& se) { return &se.getSysId(); }

static inline const String*
uri_ptr(const UriEntry& ue) { return &ue.getName(); }

template <class Map, class Entry> inline bool update_map(Map& dst, Entry& entry)
{
    const String* uptr(uri_ptr(entry));
    typename Map::iterator lb(dst.lower_bound(uptr));
    if (dst.end() != lb && *(lb->first) == *uptr) {
        return false;
        // TODO report entry unreachability
    }
    else
        dst.insert(lb, typename Map::value_type(uptr, &entry));
    return true;
}

static bool check_circular(Resolver& r, Catalog& cat, const String& uri)
{
    if (r.isVisited(uri)) {
        msg_router(r.getMessenger(), CatMgrMessages::circularReference)
            << Message::L_ERROR << uri << cat.getUri();
        return true;
    }
    return false;
}

bool CatalogBuilder::visit(SystemEntry& entry)
{
    return update_map(catalog_.systemMap_, entry);
}

bool CatalogBuilder::visit(UriEntry& entry)
{
    return update_map(catalog_.uriMap_, entry);
}

bool CatalogBuilder::visit(RewriteSystemEntry& entry)
{
    catalog_.systemRewriteVec_.push_back(&entry);
    return true;
}

bool CatalogBuilder::visit(RewriteUriEntry& entry)
{
    catalog_.uriRewriteVec_.push_back(&entry);
    return true;
}

bool update_delegate(DelegateCatalogVector& v, DelegateEntry& de, Resolver& r,
                     Catalog& cat)
{
    const String& uri(de.getCatalogUri());
    if (check_circular(r, cat, uri))
        return false;

    Messenger* m(r.getMessenger());
    if (Catalog* pCatalog = CatalogHolder::instance().addCatalog(uri, m)) {
        v.push_back(DelegateCatalog(pCatalog, de.getExtIdStart()));
        return true;
    }
    return false;
}

bool CatalogBuilder::visit(DelegateSystemEntry& entry)
{
    return update_delegate(catalog_.systemDelegateVec_, entry, resolver_,
                           catalog_);
}

bool CatalogBuilder::visit(DelegatePublicEntry& entry)
{
    return update_delegate(catalog_.publicDelegateVec_, entry, resolver_,
                           catalog_);
}

bool CatalogBuilder::visit(DelegateUriEntry& entry)
{
    return update_delegate(catalog_.uriDelegateVec_, entry, resolver_,
                           catalog_);
}

bool CatalogBuilder::visit(PublicEntry& entry)
{
    catalog_.publicList_.push_back(&entry);
    return true;
}

bool CatalogBuilder::visit(NextCatalogEntry& entry)
{
    const String& uri(entry.getCatalogUri());
    if (check_circular(resolver_, catalog_, uri))
        return false;

    Messenger* m(resolver_.getMessenger());
    if (Catalog* pCatalog = CatalogHolder::instance().addCatalog(uri, m)) {
        catalog_.nextList_.push_back(pCatalog);
        return true;
    }
    return false;
}

}

