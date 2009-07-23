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
# include "CatalogEntry.h"
# include "CatalogEntries.h"
# include "CatMgrMessageUtils.h"
# include "CatMgrMessages.h"
# include "catmgr_utils.h"
# include "catmgr_debug.h"

# include "common/String.h"
# include "common/StringUtils.h"
# include "common/StringCmp.h"
# include "grove/Nodes.h"
#else
# include "catmgr_pch.hpp"
#endif

namespace CatMgrImpl {

using namespace Common;
using namespace GroveLib;

CatalogEntry::~CatalogEntry()
{
    DBG(CATMGR.TRACE) << "~CatalogEntry(), this:" << abr(this) << std::endl;
}

struct AttrMissingException : public CatMgrException {
    AttrMissingException(const String& elemName, const char* attrName)
     :  CatMgrException(CatMgrMessages::reqAttributeMissing,
                        Message::L_WARNING),
        elemName_(elemName), attrName_(attrName)
    {
    }
    const String&   elemName_;
    const char*     attrName_;
};

EntryParam::EntryParam(const Element* ep, const String& parentBase,
                       ResolverOptions opts, Messenger* m)
 : base_(parentBase), opts_(opts), m_(m), elem_(ep)
{
}

String EntryParam::getAttr(const char* name) const
{
    String attr(get_attr(elem_, from_latin1(name)));
    if (is_null(attr))
        throw AttrMissingException(elem_->nodeName(), name);
    return attr;
}

String EntryParam::makeUri(const char* attrname) const
{
    String base(rebase_uri(base_, get_attr(elem_, "xml:base")));
    return norm_uri(rebase_uri(base, getAttr(attrname)));
}

String EntryParam::makeSysId(const char* attrname) const
{
    return norm_uri(getAttr(attrname));
}

String EntryParam::makePubId(const char* attrname) const
{
    return norm_pubid(getAttr(attrname));
}

template<class Entry> CatalogEntry* make_entry(const EntryParam& eparam)
{
    return new Entry(eparam);
}

typedef CatalogEntry* (*CatalogEntryMaker)(const EntryParam&);

static const struct MakerMap {
    const char*         entryName_;
    CatalogEntryMaker   makerFunc_;
} makers[] = {
    { "delegatePublic", make_entry<DelegatePublicEntry> },
    { "delegateSystem", make_entry<DelegateSystemEntry> },
    { "delegateURI",    make_entry<DelegateUriEntry> },
    { "nextCatalog",    make_entry<NextCatalogEntry> },
    { "public",         make_entry<PublicEntry> },
    { "rewriteSystem",  make_entry<RewriteSystemEntry> },
    { "rewriteURI",     make_entry<RewriteUriEntry> },
    { "system",         make_entry<SystemEntry> },
    { "uri",            make_entry<UriEntry> }
};

struct MakerComp {
    bool operator()(const MakerMap& lhs, const String& rhs)
    { return lhs.entryName_ < rhs; }
    bool operator()(const String& lhs, const MakerMap& rhs)
    { return rhs.entryName_ > lhs; }
};

CatalogEntry* CatalogEntry::make(const Element* ep, const String& parentBase,
                                 ResolverOptions opts, Messenger* m)
{
    try {
        Range<const MakerMap*> r(makers,
                                 makers + sizeof(makers)/sizeof(makers[0]));
        const String& name(ep->nodeName());
        const MakerMap* pMap = std::lower_bound(r.begin(), r.end(), name,
                                                MakerComp());
        if (r.end() != pMap && pMap->entryName_ == name)
            return pMap->makerFunc_(EntryParam(ep, parentBase, opts, m));
        if (opts.isSet(ResolverOptions::TRACE))
            msg_router(m, CatMgrMessages::unknownCatalogEntry)
                << Message::L_WARNING << ep->nodeName();
    }
    catch (AttrMissingException& e) {
        msg_router(m, e.message_id_) << e.severity_
            << e.attrName_ << e.elemName_;
    }
    return 0;
}

}
