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

#ifndef CATALOG_IMPL_H_
#define CATALOG_IMPL_H_

# include "CatalogEntries.h"
# include "Catalog.h"
# include "UriResolver.h"
# include "ExternalIdResolver.h"
# include "UriPtrCmp.h"

# include "common/RefCounted.h"
# include "common/String.h"

#include <map>
#include <list>
#include <vector>

namespace CatMgrImpl {

struct DelegateCatalog {
    DelegateCatalog(Catalog* pCatalog, const Common::String& pfx)
     :  catalog_(pCatalog), prefix_(pfx) {}
    Catalog*        catalog_;
    Common::String  prefix_;
};

typedef std::map<const Common::String*, CatalogEntry*, UriPtrCmp> EntryMap;
typedef std::vector<RewriteEntry*> RewriteEntryVector;
typedef std::vector<DelegateCatalog> DelegateCatalogVector;
typedef std::list<Catalog*> NextCatalogList;
typedef std::list<CatalogEntry*> CatalogEntryList;

class CatalogBuilder;

class CatalogImpl : public Catalog {
public:
    CatalogImpl(const Common::String& uri);
    ~CatalogImpl();

    virtual bool resolve(ExternalIdResolver& r);
    virtual bool resolve(UriResolver& r);
    virtual const Common::String& getUri() const { return uri_; }
    virtual const NextCatalogList*    nextCatalogs() const { return &nextList_; }

private:
    bool buildCatalog(Resolver& r);
    void recycle();
    friend class CatalogBuilder;
    //!
    EntryMap                systemMap_;
    RewriteEntryVector      systemRewriteVec_;
    DelegateCatalogVector   systemDelegateVec_;
    //
    CatalogEntryList        publicList_;
    DelegateCatalogVector   publicDelegateVec_;
    //!
    EntryMap                uriMap_;
    RewriteEntryVector      uriRewriteVec_;
    DelegateCatalogVector   uriDelegateVec_;
    //!
    NextCatalogList         nextList_;
    //!
    Common::String          uri_;
    unsigned                mtime_;
    bool                    hasEntries_;
    bool                    wasNotFound_;
    bool                    isBeingBuilt_;
};

}

#endif // CATALOG_IMPL_H_
