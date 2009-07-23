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
# include "CatalogEntries.h"
# include "catmgr_utils.h"
# include "catmgr_debug.h"

# include "common/String.h"
# include "common/StringUtils.h"
#else
# include "catmgr_pch.hpp"
#endif

namespace CatMgrImpl {

using namespace Common;

MapEntry::MapEntry(const String& extId, const String& uri,
                   ResolverOptions opts, Messenger* m)
 :  CatalogEntry(m), extId_(extId), uri_(norm_uri(uri)), opts_(opts)
{
}

RewriteEntry::RewriteEntry(const String& matchIdStart,
                           const String& rewritePrefix, Messenger* m)
 :  CatalogEntry(m), matchIdStart_(matchIdStart), rewritePrefix_(rewritePrefix)
{
}

PublicEntry::PublicEntry(const EntryParam& ep)
 :  MapEntry(ep.makePubId("publicId"), ep.makeUri("uri"), ep.opts_, &*ep.m_)
{
    DBG(CATMGR.TRACE) << "PublicEntry(), this:" << abr(this) << ", pubId: "
                      << getPubId() << ", uri: " << getUri() << std::endl;
}

SystemEntry::SystemEntry(const EntryParam& ep)
 :  MapEntry(ep.makeSysId("systemId"), ep.makeUri("uri"), ep.opts_, &*ep.m_)
{
    DBG(CATMGR.TRACE) << "SystemEntry(), this:" << abr(this) << ", sysId: "
                      << getSysId() << ", uri: " << getUri() << std::endl;
}

RewriteSystemEntry::RewriteSystemEntry(const EntryParam& ep)
 :  RewriteEntry(ep.makeSysId("systemIdStartString"),
                 ep.makeUri("rewritePrefix"), &*ep.m_)
{
    DBG(CATMGR.TRACE) << "RewriteSystemEntry(), this:" << abr(this)
                      << ", sysIdStart: " << getSysIdStart()
                      << ", rewritePrefix: " << getRewritePrefix() << std::endl;
}

DelegatePublicEntry::DelegatePublicEntry(const EntryParam& ep)
 :  DelegateEntry(ep.makePubId("publicIdStartString"), ep.makeUri("catalog"),
                  ep.opts_, &*ep.m_)
{
    DBG(CATMGR.TRACE) << "DelegatePublicEntry(), this:" << abr(this)
                      << ", pubIdStart: " << getPubIdStart() << ", catalog: "
                      << getCatalogUri() << std::endl;
}

DelegateSystemEntry::DelegateSystemEntry(const EntryParam& ep)
 :  DelegateEntry(ep.makePubId("systemIdStartString"), ep.makeUri("catalog"),
                  ep.opts_, &*ep.m_)
{
    DBG(CATMGR.TRACE) << "DelegateSystemEntry(), this:" << abr(this)
                      << ", sysIdStart: " << getSysIdStart() << ", catalog: "
                      << getCatalogUri() << std::endl;
}

UriEntry::UriEntry(const EntryParam& ep)
 :  MapEntry(ep.makeSysId("name"), ep.makeUri("uri"), ep.opts_, &*ep.m_)
{
    DBG(CATMGR.TRACE) << "UriEntry(), this:" << abr(this) << ", name: "
                      << getName() << ", uri: " << getUri() << std::endl;
}


RewriteUriEntry::RewriteUriEntry(const EntryParam& ep)
 :  RewriteEntry(ep.makeSysId("uriStartString"),
                 ep.makeUri("rewritePrefix"), &*ep.m_)
{
    DBG(CATMGR.TRACE) << "RewriteUriEntry(), this:" << abr(this)
                      << ", uriStart: " << getUriStart()
                      << ", rewritePrefix: " << getRewritePrefix() << std::endl;
}

DelegateUriEntry::DelegateUriEntry(const EntryParam& ep)
 :  DelegateEntry(ep.makeSysId("uriStartString"), ep.makeUri("catalog"),
                  ep.opts_, &*ep.m_)
{
    DBG(CATMGR.TRACE) << "DelegateUriEntry(), this:" << abr(this)
                      << ", uriStart: " << getUriStart() << ", catalog: "
                      << getCatalogUri() << std::endl;
}


NextCatalogEntry::NextCatalogEntry(const EntryParam& ep)
 :  CatalogEntry(&*ep.m_), catalog_(ep.makeUri("catalog")), opts_(ep.opts_)
{
    DBG(CATMGR.TRACE) << "NextCatalogEntry(), this:" << abr(this)
                      << ", catalog: " << getCatalogUri() << std::endl;
}

}
