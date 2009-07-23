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
#include "ui/UiProps.h"

namespace Sui {

    typedef UI_EXPIMP const char* const exported_literal;

    exported_literal TRANSLATION_CONTEXT= "translation-context";
    exported_literal DEFAULT_TRANSLATION_CONTEXT= "sui";

    exported_literal ACTION             = "action";
    exported_literal CURRENT_ACTION     = "current-action";

    exported_literal IS_ENABLED         = "is-enabled";
    exported_literal IS_TOGGLED         = "is-toggled";
    exported_literal IS_TOGGLEABLE      = "is-toggleable";
    exported_literal IS_SEPARATOR       = "is-separator";

    exported_literal NAME               = "name";
    exported_literal INSCRIPTION        = "inscription";
    exported_literal ICON               = "icon";
    exported_literal PIXMAP             = "pixmap";
    exported_literal TOOLTIP            = "tooltip";
    exported_literal WHATS_THIS         = "whats_this";
    exported_literal ACCEL              = "accel";
    exported_literal MENU_ROLE          = "menu-role";
    exported_literal TOGGLED_ON_INSCRIPTION     = "toggled_on_inscription";
    exported_literal TOGGLED_OFF_INSCRIPTION    = "toggled_off_inscription";
    exported_literal TOGGLED_ON_TOOLTIP         = "toggled_on_tooltip";
    exported_literal TOGGLED_OFF_TOOLTIP        = "toggled_off_tooltip";
    exported_literal BREAK_BEFORE       = "break-before"; // toolbar only
    exported_literal SHORTCUTS          = "shortcuts"; 

    exported_literal ITEM_PROPS         = "properties";
    exported_literal ITEM_PROP_SPECS    = "#prop_specs";
    exported_literal IS_VISIBLE         = "is-visible";
    exported_literal IS_SINGLETON       = "is-singleton";

    exported_literal MAIN_WINDOW_WIDGET = "main_window_widget";
    exported_literal SIMPLE_WIDGET      = "simple_widget";
    exported_literal MENU_WIDGET        = "menu_widget";
    exported_literal TOOL_BAR_WIDGET    = "tool_bar_widget";
    exported_literal STATUS_BAR_WIDGET  = "status_bar_widget";
    exported_literal TERMINAL_WIDGET    = "terminal_widget";

} // namespace
