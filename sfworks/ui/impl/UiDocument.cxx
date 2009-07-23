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
#include "ui/UiItems.h"
#include "ui/MainWindow.h"
#include "ui/UiDocument.h"
#include "ui/UiItemSearch.h"
#include "ui/ActionSet.h"
#include "ui/MimeHandler.h"
#include "ui/impl/UiMultiItem.h"
#include "ui/impl/ui_debug.h"

#include "common/StringTokenizer.h"

using namespace Common;

namespace Sui {

//////////////////////////////////////////////////////////////////////////

static const char* const ACCEPTABLE_IN      = "acceptable-in";
static const char* const IS_MULTIPLICABLE   = "is-multiplicable";

class ItemMakerInfo {
public:
    ItemMakerInfo()
        : maker_(0),
          props_(new PropertyNode("item-maker-info")) 
    {
    };
    ItemMakerInfo(ItemMakerPtr maker, const String& acceptableIn)
        : maker_(maker),
          props_(new PropertyNode("item-maker-info")) 
    {
        props_->makeDescendant(ACCEPTABLE_IN)->setString(acceptableIn);
    };
    ItemMakerInfo(const ItemMakerInfo& info)
        : maker_(info.maker_),
          props_(info.props_) {};

    ItemMakerPtr        maker_;
    PropertyNodePtr     props_;
};

class Document::ItemMakerMap :
    public std::map<const String, ItemMakerInfo> {};

////////////////////////////////////////////////////////////////////////////

Document::Document(ActionDispatcher* dispatcher)
    : Item("document", 0),
      itemMakerMap_(new ItemMakerMap),
      actionSet_(new ActionSet),
      dispatcher_(dispatcher),
      mimeHandler_(MimeHandler::make())
{
    PropertyNode nop;
    nop.appendChild(new PropertyNode(NAME, "NOP"));
    nop.appendChild(new PropertyNode(ICON, "null"));
    nop.appendChild(new PropertyNode(INSCRIPTION, "no action"));
    nop.appendChild(new PropertyNode(TOOLTIP, "no action"));
    actionSet()->makeAction(&nop);

    PropertyNodePtr maker_props;

    //////
    maker_props = addItemMaker(
        MAIN_MENU, new CustomItemMaker(makeUiItem<MainMenu>), 
        MAIN_WINDOW_WIDGET);
    maker_props->makeDescendant(IS_SINGLETON)->setBool(true);

    //////
    addItemMaker(POPUP_MENU,        
                 new CustomItemMaker(makeUiItem<PopupMenu>), MENU_WIDGET);
    addItemMaker(CONTEXT_MENU,      
                 new CustomItemMaker(makeUiItem<ContextMenu>),
                 SIMPLE_WIDGET + String(' ') + TERMINAL_WIDGET);
    //////
    maker_props = addItemMaker(
        MENU_ITEM, new CustomItemMaker(makeUiItem<MenuItem>), MENU_WIDGET);
    maker_props->makeDescendant(IS_MULTIPLICABLE)->setBool(true);

    //////
    //addItemMaker(MENU_BUTTON,       
    //             new CustomItemMaker(makeUiItem<MenuButton>), MENU_WIDGET);
    addItemMaker(MENU_SEPARATOR,    
                 new CustomItemMaker(makeUiItem<MenuSeparator>), MENU_WIDGET);
    addItemMaker(TOOL_BAR,          
                 new CustomItemMaker(makeUiItem<ToolBar>), MAIN_WINDOW_WIDGET);

    //////
    maker_props = addItemMaker( 
        TOOL_BUTTON, new CustomItemMaker(makeUiItem<ToolButton>),
        TOOL_BAR_WIDGET + String(' ') + STATUS_BAR_WIDGET);
    maker_props->makeDescendant(IS_MULTIPLICABLE)->setBool(true);

    //////
    addItemMaker(TOOL_BAR_SEPARATOR, 
                 new CustomItemMaker(makeUiItem<ToolBarSeparator>),
                 TOOL_BAR_WIDGET);
    //////
    maker_props = addItemMaker(
        STATUS_BAR, new CustomItemMaker(makeUiItem<StatusBar>), 
        MAIN_WINDOW_WIDGET);
    maker_props->makeDescendant(IS_SINGLETON)->setBool(true);

    //////
    addItemMaker(LABEL, new CustomItemMaker(makeUiItem<Label>), 
                 SIMPLE_WIDGET + String(' ') + TOOL_BAR_WIDGET + 
                 String(' ') + STATUS_BAR_WIDGET);

    addItemMaker(SPLITTER,          
                 new CustomItemMaker(makeUiItem<Splitter>), SIMPLE_WIDGET);
    addItemMaker(WIDGET,            
                 new CustomItemMaker(makeUiItem<Widget>),
                 MAIN_WINDOW_WIDGET + String(' ') + SIMPLE_WIDGET);
    addItemMaker(TAB_ITEM,          
                 new CustomItemMaker(makeUiItem<TabItem>), SIMPLE_WIDGET);
    addItemMaker(TOOL_BOX,          
                 new CustomItemMaker(makeUiItem<ToolBox>), SIMPLE_WIDGET);

    addItemMaker(DIALOG, new CustomItemMaker(makeUiItem<Dialog>), 
                 MAIN_WINDOW_WIDGET + String(' ') + SIMPLE_WIDGET +
                 String(' ') + TERMINAL_WIDGET);
    
    addItemMaker(LAYOUT,            
                 new CustomItemMaker(makeUiItem<Layout>), SIMPLE_WIDGET);
    addItemMaker(GRID_LAYOUT,       
                 new CustomItemMaker(makeUiItem<GridLayout>), SIMPLE_WIDGET);
    addItemMaker(GRID_WIDGET,       
                 new CustomItemMaker(makeUiItem<GridWidget>), SIMPLE_WIDGET);

    addItemMaker(PUSH_BUTTON, new CustomItemMaker(makeUiItem<PushButton>),
                 SIMPLE_WIDGET + String(' ') + TOOL_BAR_WIDGET);
    addItemMaker(COMBO_BOX, new CustomItemMaker(makeUiItem<ComboBox>),
                 SIMPLE_WIDGET + String(' ') + TOOL_BAR_WIDGET);
    addItemMaker(RADIO_BUTTON,
                 new CustomItemMaker(makeUiItem<RadioButton>), SIMPLE_WIDGET);
    addItemMaker(BUTTON_GROUP,      
                 new CustomItemMaker(makeUiItem<ButtonGroup>), SIMPLE_WIDGET);

    addItemMaker(LIST_VIEW,         
                 new CustomItemMaker(makeUiItem<ListView>), SIMPLE_WIDGET);
    addItemMaker(LIST_VIEW_ITEM, 
                 new CustomItemMaker(makeUiItem<ListViewItem>), 
                 TERMINAL_WIDGET);
    addItemMaker(LINE_EDIT, 
                 new CustomItemMaker(makeUiItem<LineEdit>), SIMPLE_WIDGET);
    addItemMaker(STRETCH, 
                 new CustomItemMaker(makeUiItem<Stretch>), SIMPLE_WIDGET);
}

Document::~Document()
{
}

bool Document::doDetach()
{
    for (Item* child = firstChild(); child; child = child->nextSibling())
        child->detach();
    return true;
};

void Document::showContextMenu(const QPoint& pos)
{
    Item* menu = findItem(ItemName(NOTR("documentContextMenu")));
    if (menu)
        menu->showContextMenu(pos);
}

bool Document::isItemAcceptableIn(const String& itemClass,
                                  const Item* item) const
{
    ItemMakerMap::const_iterator i = itemMakerMap_->find(itemClass);
    if (itemMakerMap_->end() == i || 0 == item)
        return false;
    const String& str = 
        (*i).second.props_->getSafeProperty(ACCEPTABLE_IN)->getString();
    for (StringTokenizer st(str); st; ) {
        if (st.next() == item->widgetClass())
            return true;
    }
    return false;
}

void Document::getItemMakerList(std::list<String>& list) const
{
    list.clear();
    for (ItemMakerMap::const_iterator i = itemMakerMap_->begin();
         i != itemMakerMap_->end(); i++)
        list.push_back((*i).first);
}

PropertyNodePtr Document::getItemMakerProps(const String& itemClass) const
{
    ItemMakerMap::const_iterator i = itemMakerMap_->find(itemClass);
    if (itemMakerMap_->end() != i) 
        return (*i).second.props_;
    return 0;
}

PropertyNodePtr Document::addItemMaker(const String& itemClass, 
                                       ItemMakerPtr itemMaker,
                                       const String& acceptableIn)
{
    DBG(UI.TEST) << "Document: register item maker " << itemClass
                 << " maker " << (int)itemMaker << std::endl;
    ItemMakerInfo maker_info(itemMaker, acceptableIn);
    (*itemMakerMap_)[itemClass] = maker_info;
    return maker_info.props_;
}

void Document::removeItemMaker(const String& itemClass)
{
    DBG(UI.TEST) << "Document: REMOVE item maker " << itemClass << std::endl;
    ItemMakerMap::iterator i = itemMakerMap_->find(itemClass);
    if (i != itemMakerMap_->end())
        itemMakerMap_->erase(i);
}

Item* Document::makeItem(const String& itemClass,
                         PropertyNode* properties) const
{
    DBG(UI.TEST) << "UiDocument makeItem: " << itemClass
                 << " props: " << properties << std::endl;
    ItemMakerMap::const_iterator i = itemMakerMap_->find(itemClass);
    if (itemMakerMap_->end() == i) {
        DBG(UI.TEST)
            << "Document<" << get(NAME) << ">: [" << this
            << "] no registered itemMaker: " << itemClass << std::endl;
        return 0;
    }
    if (properties) {
        if (!isToMakeItem(properties)) {
            DBG(UI.TEST) << "Document refused to make <" << get(NAME) << ">\n";
            return 0;
        }
        PropertyNode* action_prop = properties->getProperty(ACTION);
        Action* action = 
            (action_prop) ? findAction(action_prop->getString()) : 0;
        if (action_prop && !action_prop->firstChild() && !action) {
            DBG(UI.ITEM) << "Action <" << action_prop->getString() << 
                "> not found for item <" << 
                properties->getSafeProperty(NAME)->getString() << ">\n";
            return 0;
        }
        const bool is_multiplicable = 
            (*i).second.props_->getSafeProperty(IS_MULTIPLICABLE)->getBool();
        if (is_multiplicable && action && (action->firstChild() || 
            action->getBool(IS_MULTIPLICABLE))) 
                return new MultiItem(action, properties->copy(true), 
                    (*i).second.maker_, itemClass, "");
        Item* item = (*i).second.maker_->makeItem(action, 
                                                  properties->copy(true));
        return item;
    }
    return (*i).second.maker_->makeItem(0, 0);
}

bool Document::isToMakeItem(const Common::PropertyNode*) const
{
    return true;
}
        
Action* Document::findAction(const String& actionName) const
{
    return actionSet_->findAction(actionName);
}

void Document::dispatch(Action* action)
{
    if (action) 
        dispatcher_->dispatch(actionToDispatch(action));
}

void Document::updateProperties()
{
    saveDockingState();
}

} // namespace Sui
