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
# include "CatalogHolder.h"
# include "Catalog.h"
# include "UriPtrCmp.h"
# include "catmgr_debug.h"

# include "common/common_defs.h"
# include "common/String.h"
# include "common/Singleton.h"
# include "common/RefCounted.h"
# include "common/RefCntPtr.h"
# include <map>
#else
# include "catmgr_pch.hpp"
#endif

namespace CatMgrImpl {

using namespace Common;
using namespace std;

struct CatPtr : public RefCounted<> {
public:
    CatPtr(Catalog* pCatalog) : catalog_(pCatalog) {}
    Catalog* catalog_;
};

using namespace Common;

CatalogHolder::CatalogHolder() {}
CatalogHolder::~CatalogHolder() {}

class CatalogHolderImpl : public CatalogHolder {
public:
    CatalogHolderImpl() : reuseCatalog_(true) {}
    ~CatalogHolderImpl() {}

    virtual Catalog* addCatalog(const String &uri,
                                Common::Messenger*);
    virtual void releaseCatalog(const String &uri);
private:
    typedef std::map<const String*, RefCntPtr<CatPtr>, UriPtrCmp> CatalogMap;
    CatalogMap  catalogMap_;
    bool        reuseCatalog_;
};

Catalog* CatalogHolderImpl::addCatalog(const String &uri,
                                       Common::Messenger* messenger)
{
    Catalog* pCatalog(0);
    CatalogMap::iterator it = catalogMap_.find(&uri);
    if (catalogMap_.end() != it)
        pCatalog = it->second->catalog_;
    else if ((pCatalog = Catalog::make(uri, messenger))) {
        CatalogMap::mapped_type ptr(new CatPtr(pCatalog));
        it = catalogMap_.insert(it, std::make_pair(&pCatalog->getUri(), ptr));
    }
    if (pCatalog)
        it->second->incRefCnt();
    DBG(CATMGR.TRACE) << "CatalogHolderImpl::addCatalog(), uri: "
                      << pCatalog->getUri() << ", pCatalog:"
                      << pCatalog << endl;
    return pCatalog;
}

void CatalogHolderImpl::releaseCatalog(const String &uri)
{
    DBG(CATMGR.TRACE) << "CatalogHolderImpl:" << ":releaseCatalog(), uri: "
                      << uri << endl;
    CatalogMap::iterator it = catalogMap_.find(&uri);
    if (catalogMap_.end() != it) {
        CatalogMap::mapped_type& ptr(it->second);
        if (!reuseCatalog_) {
            if (1 == ptr->decRefCnt()) {
                delete ptr->catalog_;
                DBG(CATMGR.TRACE) << "CatalogHolderImpl:" << "deleted:"
                                  << abr(ptr->catalog_) << endl;
                catalogMap_.erase(it);
            }
        }
        else {
            DBG(CATMGR.TRACE) << "CatalogHolderImpl:" << " reused:"
                              << abr(ptr->catalog_) << endl;
        }
    }
}

CatalogHolder& CatalogHolder::instance()
{
    return SingletonHolder<CatalogHolderImpl,
                           CreateUsingNew<CatalogHolderImpl>,
                           NoDestroy<CatalogHolderImpl> >::instance();
}

}

