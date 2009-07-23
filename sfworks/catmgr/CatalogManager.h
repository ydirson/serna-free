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

#ifndef CATALOG_MANAGER_H_
#define CATALOG_MANAGER_H_

#include "common/common_defs.h"
#include "common/StringDecl.h"
#include "common/PropertyTree.h"
#include "catmgr/catmgr_defs.h"
#include "common/SernaApiBase.h"

namespace Common {
    class Messenger;
    template <class T> class Vector;
}

namespace CatMgr {

class CATMGR_EXPIMP CatalogManager : public SernaApiBase {
public:
    typedef Common::String String;
    typedef Common::Messenger Messenger;

    static  CatalogManager* make();
    virtual CatalogManager* copy() const = 0;

    virtual ~CatalogManager() {}

    virtual String resolveExternalId(const String& pubid, const String& sysid,
                                     Messenger* = 0) const = 0;
    virtual String resolveUri(const String& uri, Messenger* = 0) const = 0;

    virtual bool setOption(const char* name, const char* value,
                           Messenger* = 0) = 0;

    virtual bool addCatalogList(const String& path, Messenger* = 0) = 0;
    virtual bool addCatalogList(const Common::Vector<String>& pathList,
                                Messenger* = 0) = 0;
    virtual void getCatalogList(Common::Vector<String>& pathList) = 0;
    virtual void getCatalogTree(Common::PropertyNode* root) = 0;
    virtual bool clearCatalogs() = 0;

    virtual void setMessenger(Messenger* msgr) = 0;
    virtual Messenger* getMessenger() const = 0;

protected:
    CatalogManager();
private:
    DEFAULT_COPY_CTOR_DECL(CatalogManager)
    DEFAULT_ASSIGN_OP_DECL(CatalogManager)
};

CATMGR_EXPIMP CatalogManager& global_catalog();

}

#endif // CATALOG_MANAGER_H_
