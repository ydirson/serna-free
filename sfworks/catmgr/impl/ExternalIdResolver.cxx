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
# include "ExternalIdResolver.h"
# include "CatalogEntries.h"
# include "Catalog.h"
# include "CatMgrMessages.h"
# include "CatMgrMessageUtils.h"
# include "catmgr_utils.h"
# include "catmgr_debug.h"

# include "common/String.h"
# include "common/StringUtils.h"
# include "common/Message.h"
#else
# include "catmgr_pch.hpp"
#endif

namespace CatMgrImpl {

using namespace Common;
using namespace std;

ExternalIdResolver::ExternalIdResolver(const String& pubid, const String& sysid,
                                       const ResolverOptions& opts,
                                       Common::Messenger* messenger,
                                       CatMgr::CatalogManager* catMgr)
 :  Resolver(messenger, catMgr), ResolverOptions(opts),
    pubId_(pubid), sysId_(sysid)
{
    DBG(CATMGR.TRACE) << "EIR(), this: " << abr(this) << ", pubId: " << pubid
                      << ", sysid: " << sysid << endl;
}

bool ExternalIdResolver::visit(PublicEntry& entry)
{
    DBG(CATMGR.TRACE) << "EIR visit PublicEntry, pubId: " << entry.getPubId()
                      << ", uri: " << entry.getUri() << endl;

    if (entry.isSystemPreferred() ||
        (!entry.isPublicPreferred() && isSet(PREFER_SYSTEM)))
        return false;
    if (entry.getPubId() == pubId_) {
        DBG(CATMGR.TEST) << "  PublicEntry match found: "
                         << entry.getUri() << endl;
        setUri(entry.getUri());
        return true;
    }
    return false;
}

bool ExternalIdResolver::visit(SystemEntry& entry)
{
    DBG(CATMGR.TRACE) << "EIR visit SystemEntry, this:" << abr(this)
                      << ", sysId: " << entry.getSysId()
                      << ", uri: " << entry.getUri() << endl;
    if (entry.getSysId() == sysId_) {
        DBG(CATMGR.TEST) << "  SystemEntry match found: "
                         << entry.getUri() << endl;
        setUri(entry.getUri());
        return true;
    }
    return false;
}

bool ExternalIdResolver::visit(RewriteSystemEntry& entry)
{
    DBG(CATMGR.TRACE) << "EIR visit RewriteSystemEntry, this:" << abr(this)
                      << ", sysIdStart: " << entry.getSysIdStart()
                      << ", pfx: " << entry.getRewritePrefix() << endl;
    const String& sysIdStart(entry.getSysIdStart());
    if (starts_with(getSysId(), sysIdStart)) {
        String sysIdSuffix(getSysId().substr(sysIdStart.size()));
        setUri(entry.getRewritePrefix() + sysIdSuffix);
        DBG(CATMGR.TEST) << "  RewriteSystemEntry match found: "
                         << getUri() << endl;
        return true;
    }
    return false;
}

bool ExternalIdResolver::doVisit(Catalog& cat)
{
    DBG(CATMGR.TRACE) << "EIR visit Catalog, this: " << abr(this)
                      << ", uri: " << cat.getUri() << endl;
    return cat.resolve(*this);
}

}
