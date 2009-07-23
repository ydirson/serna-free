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

#ifndef CATALOG_VISITOR_H_
#define CATALOG_VISITOR_H_

namespace CatMgrImpl {

class CatalogEntry;
// ExternalId entries
class PublicEntry;
class SystemEntry;
class RewriteSystemEntry;
class DelegatePublicEntry;
class DelegateSystemEntry;
// Uri entries
class UriEntry;
class RewriteUriEntry;
class DelegateUriEntry;
//
class NextCatalogEntry;

class CatalogVisitor {
public:
    virtual ~CatalogVisitor() {}
    virtual bool visit(CatalogEntry&) { return false; }
    //
    virtual bool visit(PublicEntry&) { return false; }
    virtual bool visit(SystemEntry&) { return false; }
    virtual bool visit(RewriteSystemEntry&) { return false; }
    virtual bool visit(DelegatePublicEntry&) { return false; }
    virtual bool visit(DelegateSystemEntry&) { return false; }
    //!
    virtual bool visit(UriEntry&) { return false; }
    virtual bool visit(RewriteUriEntry&) { return false; }
    virtual bool visit(DelegateUriEntry&) { return false; }
    //!
    virtual bool visit(NextCatalogEntry&) { return false; }
};

}

#endif // CATALOG_VISITOR_H_
