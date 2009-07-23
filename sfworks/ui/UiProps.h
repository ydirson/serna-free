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
#ifndef UI_PROPS_H_
#define UI_PROPS_H_

#include "ui/ui_defs.h"

namespace Sui {

    UI_EXPIMP extern const char* const TRANSLATION_CONTEXT;
    UI_EXPIMP extern const char* const DEFAULT_TRANSLATION_CONTEXT;

    UI_EXPIMP extern const char* const ACTION;
    UI_EXPIMP extern const char* const CURRENT_ACTION;
    UI_EXPIMP extern const char* const IS_ENABLED;
    UI_EXPIMP extern const char* const IS_TOGGLED;
    UI_EXPIMP extern const char* const IS_TOGGLEABLE;

    UI_EXPIMP extern const char* const NAME;
    UI_EXPIMP extern const char* const INSCRIPTION;
    UI_EXPIMP extern const char* const ICON;
    UI_EXPIMP extern const char* const PIXMAP;
    UI_EXPIMP extern const char* const TOOLTIP;
    UI_EXPIMP extern const char* const WHATS_THIS;
    UI_EXPIMP extern const char* const ACCEL;
    UI_EXPIMP extern const char* const MENU_ROLE;
    UI_EXPIMP extern const char* const TOGGLED_ON_INSCRIPTION;
    UI_EXPIMP extern const char* const TOGGLED_OFF_INSCRIPTION;
    UI_EXPIMP extern const char* const TOGGLED_ON_TOOLTIP;
    UI_EXPIMP extern const char* const TOGGLED_OFF_TOOLTIP;
    UI_EXPIMP extern const char* const IS_SEPARATOR;
    UI_EXPIMP extern const char* const BREAK_BEFORE;    // toolbar only
    UI_EXPIMP extern const char* const SHORTCUTS;

    UI_EXPIMP extern const char* const ITEM_PROPS;
    UI_EXPIMP extern const char* const ITEM_PROP_SPECS;
    UI_EXPIMP extern const char* const IS_VISIBLE;
    UI_EXPIMP extern const char* const IS_SINGLETON;

    UI_EXPIMP extern const char* const MAIN_WINDOW_WIDGET;
    UI_EXPIMP extern const char* const SIMPLE_WIDGET;
    UI_EXPIMP extern const char* const MENU_WIDGET;
    UI_EXPIMP extern const char* const TOOL_BAR_WIDGET;
    UI_EXPIMP extern const char* const STATUS_BAR_WIDGET;
    UI_EXPIMP extern const char* const TERMINAL_WIDGET;

} // namespace Sui

#endif // UI_PROPS_H_
