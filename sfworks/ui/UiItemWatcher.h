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
/*! \file
 */

#ifndef ITEM_WATCHER_H_
#define ITEM_WATCHER_H_

#include "ui/ui_defs.h"
#include "common/PackedPtrSet.h"

namespace Sui {

////////////////////////////////////////////////////////////////////////////

class Item;

class UI_EXPIMP ItemWatcher {
public:
    typedef Common::PackedPointerSet<Item> ItemPtrSet;

    virtual ~ItemWatcher();

    const ItemPtrSet&   itemPtrSet() const;
    bool                isRegisteredIn(const Item* item) const;

protected:
    virtual void        childInserted(Item* item) = 0;
    virtual void        childRemoved(Item* parent, Item* item) = 0;

    virtual void        inserted(Item* item);
    virtual void        removed(Item* item);

private:
    friend class Item;
    ItemPtrSet          items_;
};

////////////////////////////////////////////////////////////////////////////

class Action;

class UI_EXPIMP ActionWatcher {
public:
    typedef Common::PackedPointerSet<Action> ActionPtrSet;

    virtual ~ActionWatcher();

    const ActionPtrSet& actionPtrSet() const;
    bool                isRegisteredIn(const Action* action) const;

protected:
    virtual void        childInserted(Action* action) = 0;
    virtual void        childRemoved(Action* parent, Action* action) = 0;

private:
    friend class ActionImpl;
    friend class Action;

    ActionPtrSet        actions_;
};

}

#endif // ITEM_WATCHER_H_
