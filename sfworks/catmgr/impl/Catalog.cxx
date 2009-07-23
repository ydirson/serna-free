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
# include "Catalog.h"
# include "UriResolver.h"
# include "ExternalIdResolver.h"
# include "ResolverOptions.h"
# include "CatalogEntry.h"
# include "CatalogEntries.h"
# include "CatalogBuilder.h"
# include "CatalogHolder.h"
# include "CatMgrMessages.h"
# include "CatMgrMessageUtils.h"
# include "catmgr_utils.h"
# include "catmgr_debug.h"

# include "common/MessageUtils.h"
# include "common/String.h"
# include "common/RefCounted.h"
# include "common/PathName.h"
# include <map>
# include <list>
# include <vector>
# include <functional>
# include <algorithm>
#else
# include "catmgr_pch.hpp"
#endif

namespace CatMgrImpl {

using namespace Common;
using namespace std;

Catalog::Catalog() {}
Catalog::~Catalog() {}

inline void releaseCatalog(const Catalog* cat)
{
    if (cat)
        CatalogHolder::instance().releaseCatalog(cat->getUri());
}

Catalog* Catalog::make(const String& absUri, Messenger*)
{
    return new CatalogImpl(absUri);
}

CatalogImpl::CatalogImpl(const String& absUri)
 :  uri_(absUri), mtime_(0), hasEntries_(false), wasNotFound_(false),
    isBeingBuilt_(false)
{
    DBG(CATMGR.TRACE) << "CatalogImpl(), uri: " << uri_ << std::endl;
}

static void recycle(Catalog* pCatalog) { releaseCatalog(pCatalog); }
static void recycle(CatalogEntry* entry) { delete entry; }
static void recycle(const DelegateCatalog& dc) { releaseCatalog(dc.catalog_); }
static void recycle(const EntryMap::value_type& v) { delete v.second; }

template <class V> struct Recycler {
    Recycler operator()(const V& v) { recycle(v); return *this; }
};

template <class Cont> void recycle_cont(Cont& cont)
{
    Recycler<typename Cont::value_type> recycler;
    for_each(cont.begin(), cont.end(), recycler);
    cont.clear();
}

CatalogImpl::~CatalogImpl()
{
    DBG(CATMGR.TRACE) << "~CatalogImpl(), this:" << abr(this)
                      << ", uri: " << getUri() << ", nexts: "
                      << nextList_.size() << endl;
    recycle();
}

void CatalogImpl::recycle()
{
    recycle_cont(systemMap_);
    recycle_cont(systemRewriteVec_);
    recycle_cont(systemDelegateVec_);
    recycle_cont(publicList_);
    recycle_cont(publicDelegateVec_);
    recycle_cont(uriMap_);
    recycle_cont(uriRewriteVec_);
    recycle_cont(uriDelegateVec_);
    recycle_cont(nextList_);
}

template<class Map, class Vec>
bool find_match_or_rewrite(Map& map, Vec& vec, const String& uri, Resolver& r)
{
    typename Map::const_iterator it(map.find(&uri));
    if (map.end() != it) {
        it->second->accept(r);
        return true;
    }
    typename Vec::const_iterator vecIt(vec.begin());
    for (; vec.end() != vecIt; ++vecIt) {
        if ((*vecIt)->accept(r))
            return true;
    }
    return false;
}

static bool process_nexts(NextCatalogList& lst, Resolver& r, Catalog& cat)
{
    NextCatalogList::iterator it = lst.begin();
    while (it != lst.end()) {
        const String& uri((*it)->getUri());
        if (r.isVisited(uri)) {
            msg_router(r.getMessenger(), CatMgrMessages::circularReference)
                << Message::L_ERROR << uri << cat.getUri();
            NextCatalogList::iterator erase_it(it);
            ++it;
            recycle(*erase_it);
            lst.erase(erase_it);
            continue;
        }
        else if (r.visit(*(*it)))
            return true;
        ++it;
    }
    return false;
}

struct ResBase {
    virtual Resolver& getResolver() = 0;
    virtual void resetId() = 0;
    virtual ~ResBase() {}
};

template <class Res> struct ResOp : ResBase {
    typedef void (Res::*ClearPtr)();
    ResOp(Res& r, ClearPtr ptr) : resolver_(r), ptr_(ptr) {}

    virtual void resetId() { if (ptr_) (resolver_.*ptr_)(); }
    virtual Resolver& getResolver() { return resolver_; }
private:
    Res&        resolver_;
    ClearPtr    ptr_;
};

static bool process_delegates(const String& idStr, DelegateCatalogVector& vec,
                              ResBase& resOp, Catalog& catalog)
{
    Resolver& r(resOp.getResolver());
    bool isMatched(false);
    DelegateCatalogVector::iterator it = vec.begin();
    while (vec.end() != it) {
        DBG(CATMGR.TRACE) << "Check delegate match, id: " << idStr
                          << ", prefix: " << it->prefix_ << endl;
        if (starts_with(idStr, it->prefix_)) {
            DBG(CATMGR.TRACE) << "Found delegate match, id: " << idStr
                              << ", prefix: " << it->prefix_ << endl;
            const String& uri(it->catalog_->getUri());
            if (r.isVisited(uri)) {
                msg_router(r.getMessenger(), CatMgrMessages::circularReference)
                    << Message::L_ERROR << uri << catalog.getUri();
                unsigned dist(it - vec.begin());
                recycle(*it);
                vec.erase(it);
                it = vec.begin() + dist;
                continue;
            }
            isMatched = true;
            resOp.resetId();
            if (r.visit(*(it->catalog_)))
                return true;
        }
        ++it;
    }
    return isMatched;
}

bool CatalogImpl::resolve(ExternalIdResolver& r)
{
    DBG(CATMGR.TRACE) << "CatalogImpl::resolve(), sysid: " << sqt(r.getSysId())
                      << ", pubid: " << sqt(r.getPubId()) << ", catalog uri = "
                      << sqt(uri_) << std::endl;
    if (isBeingBuilt_ || !buildCatalog(r))
        return false;
    const String& sysId(r.getSysId());
    if (!sysId.empty()) {
        if (find_match_or_rewrite(systemMap_, systemRewriteVec_, sysId, r))
            return true;
        ResOp<ExternalIdResolver> op(r, &ExternalIdResolver::clearPubId);
        if (process_delegates(sysId, systemDelegateVec_, op, *this))
            return true;
    }
    const String& pubId(r.getPubId());
    if (!pubId.empty()) {
        CatalogEntryList::iterator pubIt = publicList_.begin();
        for (; pubIt != publicList_.end(); ++pubIt) {
            if ((*pubIt)->accept(r))
                return true;
        }
        ResOp<ExternalIdResolver> op(r, &ExternalIdResolver::clearSysId);
        if (process_delegates(pubId, publicDelegateVec_, op, *this))
            return true;
    }
    else if (sysId.empty())
        return false;
    return process_nexts(nextList_, r, *this);
}

bool CatalogImpl::resolve(UriResolver& r)
{
    DBG(CATMGR.TRACE) << "CatalogImpl::resolve(), uri: " << r.getUriRef()
                      << endl;
    if (isBeingBuilt_ || !buildCatalog(r))
        return false;
    const String& uri(r.getUriRef());
    if (uri.empty())
        return false;
    if (find_match_or_rewrite(uriMap_, uriRewriteVec_, uri, r))
        return true;
    ResOp<UriResolver> op(r, 0);
    if (process_delegates(uri, uriDelegateVec_, op, *this))
        return true;
    return process_nexts(nextList_, r, *this);
}

const String& get_prefix(const RewriteEntry* e) { return e->getMatchIdStart(); }
const String& get_prefix(const DelegateCatalog& dc) { return dc.prefix_; }

template <class Entry> struct EntryCmp {
    bool operator()(const Entry& lhs, const Entry& rhs)
    {
        return get_prefix(lhs).size() > get_prefix(rhs).size();
    }
};

bool CatalogImpl::buildCatalog(Resolver& r)
{
    if (is_file_uri(uri_)) {
        if (!PathName::exists((get_filepath(uri_)))) {
            if (!wasNotFound_) {
                msg_router(r.getMessenger(),
                           CatMgrMessages::catalogFileDoesNotExist)
                    << Message::L_WARNING << uri_;
            }
            mtime_ = 0;
            wasNotFound_ = true;
            DBG(CATMGR.TRACE) << "CatalogImpl::buildCatalog(), this:"
                              << abr(this) << ", uri_:" << sqt(uri_)
                              << " was not found" << endl;
            return false;
        }
        unsigned mtime = get_modtime(uri_);
        wasNotFound_ = false;
        if (mtime_ >= mtime)
            return true;
        mtime_ = mtime;
    }
    else
        if (hasEntries_)
            return true;
    DBG(CATMGR.TRACE) << "CatalogImpl::buildCatalog(), this:" << abr(this)
                      << ", uri_:" << uri_ << ", mtime:" << mtime_ << endl;
    recycle();
    try {
        CatalogBuilder cb(r, *this);
        isBeingBuilt_ = true;
        hasEntries_ = cb.build();
        isBeingBuilt_ = false;
        if (!hasEntries_)
            return false;
        EntryCmp<RewriteEntry*> rcmp;
        stable_sort(systemRewriteVec_.begin(), systemRewriteVec_.end(), rcmp);
        stable_sort(uriRewriteVec_.begin(), uriRewriteVec_.end(), rcmp);

        EntryCmp<DelegateCatalog> dcmp;
        stable_sort(systemDelegateVec_.begin(), systemDelegateVec_.end(), dcmp);
        stable_sort(publicDelegateVec_.begin(), publicDelegateVec_.end(), dcmp);
        stable_sort(uriDelegateVec_.begin(), uriDelegateVec_.end(), dcmp);

        return true;
    }
    catch (CatMgrException& cme) {
        msg_router(r.getMessenger(), cme.message_id_) << cme.severity_ << uri_;
    }
    catch (...) {
        msg_router(r.getMessenger(), CatMgrMessages::unknownError)
            << Message::L_ERROR << uri_;
    }
    return false;
}

}

