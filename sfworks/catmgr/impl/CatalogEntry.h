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

#ifndef CATALOG_ENTRY_H_
#define CATALOG_ENTRY_H_

#include "common/common_defs.h"
#include "common/StringDecl.h"
#include "common/RefCounted.h"
#include "common/RefCntPtr.h"
#include "common/MessageUtils.h"

#include "Resolver.h"
#include "ResolverOptions.h"
#include "CatalogVisitor.h"

namespace GroveLib {
class Element;
}

namespace CatMgrImpl {

class CatalogEntry : public Common::RefCounted<> {
public:
    virtual ~CatalogEntry();
    virtual bool accept(CatalogVisitor& v) = 0;
    static CatalogEntry* make(const GroveLib::Element* node,
                              const Common::String& parentBaseUri,
                              ResolverOptions opts,
                              Common::Messenger*);
protected:
    CatalogEntry(Common::Messenger* m) : messenger_(m) {}
    template <class T> static bool acceptImpl(T& visited, CatalogVisitor& v)
    {
        return v.visit(visited);
    }
    Common::Messenger*      getMessenger() const { return &*messenger_; }
private:
    Common::MessengerPtr messenger_;
    DEFAULT_COPY_CTOR_DECL(CatalogEntry)
    DEFAULT_ASSIGN_OP_DECL(CatalogEntry)
};

}

#endif // CATALOG_ENTRY_H_
