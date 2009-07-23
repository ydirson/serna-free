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

#ifndef CATALOG_ENTRIES_H_
#define CATALOG_ENTRIES_H_

#include "CatalogEntry.h"
#include "ResolverOptions.h"

#include "common/String.h"
#include "common/MessageUtils.h"

namespace GroveLib {
class Element;
}

#define CATALOG_VISITABLE \
virtual bool accept(CatalogVisitor& visitor) \
{                                            \
    return acceptImpl(*this, visitor);       \
}

namespace CatMgrImpl {

struct EntryParam {
    EntryParam(const GroveLib::Element*, const Common::String&,
               ResolverOptions, Common::Messenger*);
    Common::String makeUri(const char* attrName) const;
    Common::String makePubId(const char* attrName) const;
    Common::String makeSysId(const char* attrName) const;
    //!
    const Common::String&       base_;
    ResolverOptions             opts_;
    Common::MessengerPtr        m_;
private:
    Common::String getAttr(const char* name) const;
    const GroveLib::Element*    elem_;
};

class MapEntry : public CatalogEntry {
public:
    MapEntry(const Common::String& extId, const Common::String& uri,
             ResolverOptions opts, Common::Messenger* m);
    //!
    bool isSystemPreferred() const
    { return opts_.isSet(ResolverOptions::PREFER_SYSTEM); }
    bool isPublicPreferred() const
    { return opts_.isSet(ResolverOptions::PREFER_PUBLIC); }
    const Common::String& getUri() const { return uri_; }
protected:
    const Common::String& getExtId() const { return extId_; }
private:
    Common::String  extId_;
    Common::String  uri_;
    ResolverOptions opts_;
};

class RewriteEntry : public CatalogEntry {
public:
    RewriteEntry(const Common::String& matchIdStart,
                 const Common::String& rewritePrefix,
                 Common::Messenger* m);
    const Common::String& getRewritePrefix() const { return rewritePrefix_; }
    const Common::String& getMatchIdStart() const { return matchIdStart_; }
private:
    Common::String matchIdStart_;
    Common::String rewritePrefix_;
};

class DelegateEntry : public MapEntry {
public:
    DelegateEntry(const Common::String& extIdStart,
                  const Common::String& catalog, ResolverOptions opts,
                  Common::Messenger* m)
     :  MapEntry(extIdStart, catalog, opts, m) {}
    //!
    const Common::String& getCatalogUri() const { return getUri(); }
    //!
    const Common::String& getExtIdStart() const { return getExtId(); }
};

// ExternalId entries
class PublicEntry : public MapEntry {
public:
    CATALOG_VISITABLE
    PublicEntry(const EntryParam& eparam);
    const Common::String& getPubId() const { return getExtId(); }
};

class SystemEntry : public MapEntry {
public:
    CATALOG_VISITABLE
    SystemEntry(const EntryParam& eparam);
    const Common::String& getSysId() const { return getExtId(); }
};

class RewriteSystemEntry : public RewriteEntry {
public:
    CATALOG_VISITABLE
    RewriteSystemEntry(const EntryParam& eparam);
    const Common::String& getSysIdStart() const { return getMatchIdStart(); }
};

class DelegatePublicEntry : public DelegateEntry {
public:
    CATALOG_VISITABLE
    DelegatePublicEntry(const EntryParam& eparam);
    const Common::String& getPubIdStart() const { return getExtIdStart(); }
};

class DelegateSystemEntry : public DelegateEntry {
public:
    CATALOG_VISITABLE
    DelegateSystemEntry(const EntryParam& eparam);
    const Common::String& getSysIdStart() const { return getExtIdStart(); }
};

// Uri entries
class UriEntry : public MapEntry {
public:
    CATALOG_VISITABLE
    UriEntry(const EntryParam& eparam);
    const Common::String& getName() const { return getExtId(); }
};

class RewriteUriEntry : public RewriteEntry {
public:
    CATALOG_VISITABLE
    RewriteUriEntry(const EntryParam& eparam);
    const Common::String& getUriStart() const { return getMatchIdStart(); }
};

class DelegateUriEntry : public DelegateEntry {
public:
    CATALOG_VISITABLE
    DelegateUriEntry(const EntryParam& eparam);
    const Common::String& getUriStart() const { return getExtIdStart(); }
};

//
class NextCatalogEntry : public CatalogEntry {
public:
    CATALOG_VISITABLE
    NextCatalogEntry(const EntryParam& eparam);
    const Common::String& getCatalogUri() const { return catalog_; }
private:
    Common::String  catalog_;
    ResolverOptions opts_;
};

}

#endif // CATALOG_ENTRIES_H_
