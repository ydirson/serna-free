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
#ifndef UI_STACK_ITEM_H_
#define UI_STACK_ITEM_H_

#include "ui/ui_defs.h"
#include "ui/UiItem.h"

namespace Sui {

class StackItem;

//////////////////////////////////////////////////////////////////////////

class  UI_EXPIMP StackWatcher {
public:
    virtual ~StackWatcher() {}
    virtual void    currentChanged(Item* prev, Item* curr) = 0;
};

//////////////////////////////////////////////////////////////////////////

class  UI_EXPIMP StackWidget {
public:
    virtual void        insertItem(const Item* item) = 0;
    virtual void        removeItem(const Item* item) = 0;

    virtual void        setCurrent(Item* item) = 0;
    virtual void        setItemVisible(bool isVisible) = 0;

    virtual QWidget*    widget(const Item* item) const = 0;
    virtual void        itemPropertyChanged(Common::PropertyNode*) {}
    virtual StackWidget* nextStackWidget() const { return 0; }
    StackItem*          stackItem() const { return stackItem_; }
    virtual void        updateProperty(const Item*,
                                       const Common::PropertyNode*) {}
    virtual void        currentChanged(Item* newItem);

    StackWidget(StackItem* stackItem);
    virtual ~StackWidget() {}

private:
    StackItem*          stackItem_;
};

//////////////////////////////////////////////////////////////////////////

class UI_EXPIMP StackItem : public Item, protected StackWatcher {
public:
    StackItem(Action* action, PropertyNode* properties);

    Item*                   currentItem() const { return currentItem_; }
    virtual void            setCurrent(Item* item);
    virtual QWidget*        widget(const Item* item = 0) const;
    virtual StackWidget*    stackWidget() const = 0;
    virtual void            updateWidgetProperties(const Item* item, 
                                                   const Common::PropertyNode*);
protected:
    friend class StackWidget;

    virtual void        currentChanged(Item*, Item*) {}
    virtual void        childInserted(Item* item);
    virtual void        childRemoved(Item* item);
    virtual void        propertyChanged(PropertyNode* property);
    Item*               currentItem_;
    bool                currentChangeLock_;
};

} // namespace Sui

#endif // UI_ITEMS_H_
