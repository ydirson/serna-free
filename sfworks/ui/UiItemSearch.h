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
#ifndef UI_ITEM_SEARCH_H_
#define UI_ITEM_SEARCH_H_

#include "ui/ui_defs.h"
#include "ui/UiProps.h"
#include "ui/UiItem.h"

namespace Sui {

///////////////////////////////////////////////////////////////////////////

class ItemName : public ItemPred {
public:
    ItemName(const Common::String& name)
        : name_(name) {}
    virtual ~ItemName() {}

    virtual bool        operator()(const Item* item) const
    {
        return item->get(NAME) == name_;
    }
private:
    Common::String      name_;
};

///////////////////////////////////////////////////////////////////////////

class ItemClass : public ItemPred {
public:
    ItemClass(const Common::String& itemClass)
        : class_(itemClass) {}
    virtual ~ItemClass() {}

    virtual bool        operator()(const Item* item) const
    {
        return item->itemClass() == class_;
    }
private:
    Common::String      class_;
};

///////////////////////////////////////////////////////////////////////////

class ItemAction : public ItemPred {
public:
    ItemAction(const Action* action)
        : action_(action) {}
    virtual ~ItemAction() {}

    virtual bool        operator()(const Item* item) const
    {
        return item->action() == action_;
    }
private:
    const Action*       action_;
};

}

#endif // UI_ITEM_SEARCH_H_

