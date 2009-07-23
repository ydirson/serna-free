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
#ifndef UI_ITEMS_H_
#define UI_ITEMS_H_

#include "ui/ui_defs.h"
#include "ui/UiItem.h"

namespace Sui {

    UI_EXPIMP extern const char* MAIN_MENU;
    UI_EXPIMP extern const char* POPUP_MENU;
    UI_EXPIMP extern const char* CONTEXT_MENU;
    UI_EXPIMP extern const char* MENU_ITEM;
    UI_EXPIMP extern const char* MENU_BUTTON;
    UI_EXPIMP extern const char* MENU_SEPARATOR;

    UI_EXPIMP extern const char* TOOL_BAR;
    UI_EXPIMP extern const char* TOOL_BUTTON;
    UI_EXPIMP extern const char* TOOL_BAR_SEPARATOR;

    UI_EXPIMP extern const char* STATUS_BAR;
    UI_EXPIMP extern const char* LABEL;
    UI_EXPIMP extern const char* SPACER;

    UI_EXPIMP extern const char* SPLITTER;
    UI_EXPIMP extern const char* TAB_ITEM;
    UI_EXPIMP extern const char* TOOL_BOX;

    UI_EXPIMP extern const char* WIDGET;
    UI_EXPIMP extern const char* MAIN_WINDOW;
    UI_EXPIMP extern const char* DOCUMENT;
    UI_EXPIMP extern const char* ROOT_ITEM;

    UI_EXPIMP extern const char* DIALOG;

    UI_EXPIMP extern const char* LAYOUT;
    UI_EXPIMP extern const char* GRID_LAYOUT;
    UI_EXPIMP extern const char* GRID_WIDGET;

    UI_EXPIMP extern const char* LINE_EDIT;

    UI_EXPIMP extern const char* PUSH_BUTTON;
    UI_EXPIMP extern const char* RADIO_BUTTON;
    UI_EXPIMP extern const char* BUTTON_GROUP;
    UI_EXPIMP extern const char* COMBO_BOX;

    UI_EXPIMP extern const char* LIST_VIEW;
    UI_EXPIMP extern const char* LIST_VIEW_ITEM;
    UI_EXPIMP extern const char* STRETCH;

    UI_EXPIMP extern const char* LIQUID_DIALOGS;

    class MainMenu;
    class PopupMenu;
    class ContextMenu;
    class MenuItem;
    class MenuButton;
    class MenuSeparator;

    class ToolBar;
    class ToolButton;
    class ToolBarSeparator;

    class StatusBar;
    class Label;
    class Spacer;

    class Splitter;
    class TabItem;
    class ToolBox;
    class Widget;

    class Dialog;
    class Layout;
    class GridLayout;
    class GridWidget;
    class LineEdit;

    class PushButton;
    class RadioButton;
    class ButtonGroup;
    class ComboBox;

    class ListView;
    class ListViewItem;
    class Stretch;

} // namespace Sui

#endif // UI_ITEMS_H_
