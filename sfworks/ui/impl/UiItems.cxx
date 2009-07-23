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

using namespace Common;

namespace Sui {

    UI_EXPIMP const char* MAIN_MENU         = "MainMenu";
    UI_EXPIMP const char* POPUP_MENU        = "PopupMenu";
    UI_EXPIMP const char* CONTEXT_MENU      = "ContextMenu";
    UI_EXPIMP const char* MENU_ITEM         = "MenuItem";
    UI_EXPIMP const char* MENU_BUTTON       = "MenuButton";
    UI_EXPIMP const char* MENU_SEPARATOR    = "MenuSeparator";

    UI_EXPIMP const char* TOOL_BAR          = "ToolBar";
    UI_EXPIMP const char* TOOL_BUTTON       = "ToolButton";
    UI_EXPIMP const char* TOOL_BAR_SEPARATOR = "ToolBarSeparator";

    UI_EXPIMP const char* STATUS_BAR        = "StatusBar";
    UI_EXPIMP const char* LABEL             = "Label";
    UI_EXPIMP const char* SPACER            = "Spacer";

    UI_EXPIMP const char* SPLITTER          = "Splitter";
    UI_EXPIMP const char* TAB_ITEM          = "TabWidget";
    UI_EXPIMP const char* TOOL_BOX          = "ToolBox";

    UI_EXPIMP const char* WIDGET            = "Widget";
    UI_EXPIMP const char* MAIN_WINDOW       = "MainWindow";
    UI_EXPIMP const char* DOCUMENT          = "Document";
    UI_EXPIMP const char* ROOT_ITEM         = "RootItem";

    UI_EXPIMP const char* DIALOG         = "Dialog";
    UI_EXPIMP const char* LAYOUT         = "Layout";
    UI_EXPIMP const char* GRID_LAYOUT    = "GridLayout";
    UI_EXPIMP const char* GRID_WIDGET    = "GridWidget";
    UI_EXPIMP const char* LINE_EDIT      = "LineEdit";

    UI_EXPIMP const char* PUSH_BUTTON    = "PushButton";
    UI_EXPIMP const char* RADIO_BUTTON   = "RadioButton";
    UI_EXPIMP const char* BUTTON_GROUP   = "ButtonGroup";
    UI_EXPIMP const char* COMBO_BOX      = "ComboBox";

    UI_EXPIMP const char* LIST_VIEW      = "ListView";
    UI_EXPIMP const char* LIST_VIEW_ITEM = "ListViewItem";
    UI_EXPIMP const char* STRETCH        = "stretch";

    UI_EXPIMP const char* LIQUID_DIALOGS        = "LiquidItems";

/*
  x - can be inserted to...
  o - accepts items of...

  Item\Category            MainWindow  Widget  Menu  ToolBar  StatusBar Term.

  Document                      x        o       o       o       o
  MainMenu                      x                o
  ToolBar                       x                        o
  StatusBar                     x        o
  ToolBarSeparator                                      x               o
  ToolComboBox                                          x               o
  ToolButton                                            x       x       o
  Label                                 x               x       x       o
  DocumentEditor                        x                               o
  Plain-textEditor                      x                               o
  MessageWindow                         x                               o
  Splitter                              xo
  TabWidget                             xo
  ToolBox                               xo
  PopupMenu                                     xo
  ContextMenu                           x        o                     x
  MenuItem                                      x                       o
  MenuButton                                    x                       o
  MenuSeparator                                 x                       o
  UndoMenu                                      x                       o
  RedoMenu                                      x                       o
  CollapseMenu                                  x                       o
  ToolbarsMenu                                  x                       o
  RecentFilesMenu                               x                       o
  Widget                        x       xo
  TOC                           x       x                               o
  FindAndReplaceDialog          x       x                               o
  SpellCheckDialog              x       x                               o
  InsertSymbolDialog            x       x                               o
  InsertElementDialog           x       x                               o
  ElementAttributesDialog       x       x                               o

  Dialog                        x       xo                             
  Layout                                xo                              
  LineEdit                              x                               o
  ListView                              x
  ListViewItem                          x                               o
  PushButton                            x               x               o
  ComboBox                              x               x               o
  RadioButton                           x                               o
  ButtonGroup                           xo

  Item/Category            MainWindow  Widget  Menu  ToolBar  StatusBar Term.

  ---------------------------------------------------------------------------
  Property Groups:
  ---------------------------------------------------------------------------

  (common)
    name
    tooltip
    is-enabled
    icon
    inscription

  (multi-action)
    action (1..n)
      $common
    current-action

  (item-action)
    accel
    is-toggleable
    is-toggled
    action ? (text value OR subproperties)
      $common ?
      $multi-action ?

  ---------------------------------------------------------------------------

  UiAction properties:
    $common
    $multi-action ?
    accel
    is-toggleable
    is-toggled

  UiItem properties:
    $common
    is-visible

    (SPD specific properties)
    before
    after
    
  ---------------------------------------------------------------------------
  Properties Specific for Certain Items:
  ---------------------------------------------------------------------------

  ButtonGroup
    title

  ComboBox
    $item-action
    is-editable
    insertion-policy (no-insertion)

  ContextMenu
    
  Dialog
    is-modal
    caption
    width
    height

  Document

  GridLayout
    margin
    spacing
    row-num
    col-num

  GridWidget
    row
    col
    row-span
    col-span

  Label
    $item-action ?
    color
    geometry
      width
      height
    size-policy (ignored)
    qt:frameShape

  Layout
    orientation (vertical, horizontal)
    margin
    spacing

  LineEdit
    text
    editable

  ListViewItem
    data
      column (0..n)  
        text
        icon

  ListView
    col-specs
      column (0..n)  
        text
        icon
    #current-item
  
  MainMenu

  MainWindow

  MenuButton
    $item-action

  MenuItem
    $item-action

  MenuSeparator

  PopupMenu

  PushButton
    $item-action
    use-accel (false)

  RadioButton
    $item-action

  Splitter
    proportions (space-delimited decimal numbers)
    orientation (horizontal, vertical)

  StatusBar
  
  Stretch

  TabWidget
  
  ToolBarSeparator

  ToolBar
    caption
    dock_edge (dock_top, dock_bottom, dock_left, dock_right)
    resize-enabled
    vertically-stretchable
    horizontally-stretchable

  ToolBox

  ToolButton
    $item-action

  Widget

*/

} // namespace
