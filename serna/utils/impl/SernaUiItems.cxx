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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "utils/SernaUiItems.h"
#include "common/String.h"
#include "common/StringUtils.h"

using namespace Common;

namespace Sui
{

typedef UTILS_EXPIMP const char* const exported_literal;

// START_IGNORE_LITERALS
exported_literal GENERATES_CHILDREN = "#generates_children";
exported_literal IS_GENERATED       = "#is_generated";

exported_literal STRUCT_DOCUMENT    = "StructDocument";
exported_literal EMPTY_DOCUMENT     = "EmptyDocument";
exported_literal STRUCT_EDITOR      = "DocumentEditor";

exported_literal PLAIN_DOCUMENT     = "PlainDocument";
exported_literal PLAIN_EDITOR       = "Plain-textEditor";

exported_literal MESSAGE_VIEW       = "MessageWindow";

exported_literal FINDER             = "FindAndReplaceDialog";
exported_literal SPELLER            = "SpellCheckDialog";
exported_literal RECENT_FILES_MENU  = "RecentFilesMenu";
exported_literal UNDO_MENU          = "UndoMenu";
exported_literal REDO_MENU          = "RedoMenu";
exported_literal FOLD_MENU          = "CollapseMenu";
exported_literal TOOLBARS_MENU      = "ToolbarsMenu";

exported_literal INSERT_SYMBOL      = "InsertSymbolDialog";
exported_literal INSERT_ELEMENT     = "InsertElementDialog";
exported_literal CHANGE_ATTRIBUTES  = "ElementAttributesDialog";
exported_literal TOOL_COMBOBOX      = "ToolComboBox";
exported_literal COMBO_LIST         = "values";
exported_literal COMBO_DEFAULT      = "default";
exported_literal COMBO_CURRENT      = "current";
exported_literal COMBO_EDITABLE     = "editable";

exported_literal CUSTOMIZER         = "CustomizerDialog";
exported_literal PLUGIN_ORIGIN      = "plugin-origin";
exported_literal REMOVED_PLUGIN_ITEMS   = "removed-plugin-items";
// STOP_IGNORE_LITERALS
}
