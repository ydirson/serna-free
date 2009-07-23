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
# include "UriResolver.h"
# include "CatalogEntries.h"
# include "Catalog.h"
# include "CatMgrMessages.h"
# include "CatMgrMessageUtils.h"
# include "catmgr_utils.h"
# include "catmgr_debug.h"

# include "common/String.h"
# include "common/Message.h"
#else
# include "catmgr_pch.hpp"
#endif

namespace CatMgrImpl {

using namespace Common;
using namespace std;

UriResolver::UriResolver(const String& uriref,
                         const ResolverOptions& opts,
                         Common::Messenger* messenger,
                         CatMgr::CatalogManager* catMgr)
 :  Resolver(messenger, catMgr),
    ResolverOptions(opts), uriRef_(norm_uri(uriref))
{
    DBG(CATMGR.TRACE) << "UR(), this: " << abr(this) << ", uri: "
                      << uriRef_ << endl;
}

bool UriResolver::visit(UriEntry& entry)
{
    DBG(CATMGR.TRACE) << "UR visit UriEntry, this:" << abr(this)
                     << ", name: " << entry.getName()
                     << ", uri: " << entry.getUri() << endl;
    if (entry.getName() == getUriRef()) {
        DBG(CATMGR.TEST) << "  UriEntry match found: " << entry.getUri() << endl;
        setUri(entry.getUri());
        return true;
    }
    return false;
}

bool UriResolver::visit(RewriteUriEntry& entry)
{
    DBG(CATMGR.TRACE) << "UR visit RewriteUriEntry, this:" << abr(this)
                      << ", sysIdStart: " << entry.getUriStart()
                      << ", pfx: " << entry.getRewritePrefix() << endl;
    const String& uriStart(entry.getUriStart());
    if (starts_with(getUriRef(), uriStart)) {
        String uriSuffix(getUriRef().substr(uriStart.size()));
        setUri(entry.getRewritePrefix() + uriSuffix);
        DBG(CATMGR.TEST) << "RewriteUriEntry match found: " << getUri() << endl;
        return true;
    }
    return false;
}

bool UriResolver::doVisit(Catalog& cat)
{
    DBG(CATMGR.TRACE) << "UR visit Catalog, this: " << abr(this)
                      << ", uri: " << cat.getUri() << endl;
    return cat.resolve(*this);
}

}
