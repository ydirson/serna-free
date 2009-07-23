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
#include "ui/UiAction.h"
#include "ui/UiProps.h"
#include "ui/impl/ui_debug.h"
#include "ui/impl/UiMultiItem.h"
#include <iostream>

using namespace Common;

namespace Sui {

//////////////////////////////////////////////////////////////////////////

MultiItem::MultiItem(Action* action, PropertyNode* properties,
                     ItemMakerPtr itemMaker, const String& itemClass, 
                     const String& widgetClass)
    : Item(action, properties),
      itemMaker_(itemMaker),
      itemClass_(itemClass),
      widgetClass_(widgetClass)
{
    itemProps()->makeDescendant("#generates_children")->setBool(true);
    Action* sub_action = action->firstChild(); 
    for (; sub_action; sub_action = sub_action->nextSibling()) 
        make_subitem(sub_action);
    action->addWatcher(this);
}

Item* MultiItem::make_subitem(Action* sub_action)
{
    if (sub_action->getBool("is-hidden"))
        return 0;
    PropertyNodePtr props = new PropertyNode("ui-item");
    props->makeDescendant(NAME)->setString(
        itemProps()->getSafeProperty(NAME)->getString() +
        "_" + sub_action->get(NAME));
    Item* sub_item = itemMaker_->makeItem(sub_action, props.pointer());
    if (sub_item)
        appendChild(sub_item);
    return sub_item;
}
    
void MultiItem::actionPropChanged(PropertyNode* actionProp, Action* subact)
{
    if (isLockedNotify())
        return;
    if (!subact)
        Item::actionPropChanged(actionProp);
}

bool MultiItem::doAttach()
{
    return true;
}

bool MultiItem::doDetach()
{
    return true;
}

QWidget* MultiItem::widget(const Item* item) const
{
    return item ? parent()->widget(this) : 0;
}
    
void MultiItem::childInserted(Action* action)
{
    Item* item = make_subitem(action);
    if (item)
        item->attach();
}
    
void MultiItem::childRemoved(Action*, Action* action)
{
    action->removeItems();
}
    
} // namespace Sui
