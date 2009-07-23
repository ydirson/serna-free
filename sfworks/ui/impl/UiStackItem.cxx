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
#include "ui/ui_defs.h"
#include "ui/UiStackItem.h"

#include <QWidget>

using namespace Common;

namespace Sui {

/////////////////////////////////////////////////////////////////////////

StackWidget::StackWidget(StackItem* stackItem)
    : stackItem_(stackItem)
{
}

void StackWidget::currentChanged(Item* newItem)
{
    if (newItem != stackItem()->currentItem()) 
        stackItem()->setCurrent(newItem);
}

/////////////////////////////////////////////////////////////////////////

StackItem::StackItem(Action* action, PropertyNode* properties)
    : Item(action, properties),
      currentItem_(0),
      currentChangeLock_(false)
{}

QWidget* StackItem::widget(const Item* item) const
{
    if (item && item->parent() == this) {
        StackWidget* sw = stackWidget();
        for (; sw; sw = sw->nextStackWidget()) {
            QWidget* widget = sw->widget(item);
            if (widget)
                return widget;
        }
    }
    return 0;
}

void StackItem::setCurrent(Item* item)
{
    if (currentChangeLock_)
        return;
    currentChangeLock_ = true;
    if (currentItem_ == item)
        return;
    currentChanged(currentItem(), item);
    currentItem_ = item;
    for (StackWidget* sw = stackWidget(); sw; sw = sw->nextStackWidget())
        sw->setCurrent(item);
    currentChangeLock_ = false;
}

void StackItem::propertyChanged(PropertyNode* property)
{
    if (isLockedNotify())
        return;
    Item::propertyChanged(property);
    for (StackWidget* sw = stackWidget(); sw; sw = sw->nextStackWidget())
        sw->itemPropertyChanged(property);
}

void StackItem::updateWidgetProperties(const Item* item, 
                                       const PropertyNode* prop)
{
    for (StackWidget* sw = stackWidget(); sw; sw = sw->nextStackWidget())
        sw->updateProperty(item, prop);
}

void StackItem::childInserted(Item* item)
{
    for (StackWidget* sw = stackWidget(); sw; sw = sw->nextStackWidget())
        sw->insertItem(item);
    setCurrent(item);
    Item::childInserted(item);
}

void StackItem::childRemoved(Item* item)
{
    for (StackWidget* sw = stackWidget(); sw; sw = sw->nextStackWidget())
        sw->removeItem(item);
    setCurrent(lastChild());
    if (currentItem() == item)
        currentItem_ = lastChild();
    //!TRICK: Removed for fast exit in serna
    //Item::childRemoved(item);
}

} // namespace
