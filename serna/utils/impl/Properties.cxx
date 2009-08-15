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

#include "utils/Properties.h"
#include "formatter/types.h"

typedef UTILS_EXPIMP const char* const exported_literal;
typedef UTILS_EXPIMP const char exported_char;


// !NOTICE:
// START_IGNORE_LITERALS
namespace CmdLineParams {

exported_char CMD_LINE_PARAMS[]             = "#command-line-params";
exported_char CMD_FILE_ARGS  []             = "file-args";
exported_char CMD_RECENT_FILE_ID[]          = "recent-id";
exported_char INSTALL_CONFIG_PACKAGE[]      = "install-config-package";
exported_char INSTALL_CONFIG_PACKAGE_W_DIAG[] =
                                              "install-config-package-w-diag";

exported_char ENABLE_DEBUG_TAGS[]           = "enable-debug-tags";
exported_char DISABLE_DEBUG_TAGS[]          = "disable-debug-tags";

exported_char OALLOC_ABORT_ON_ERROR[]       = "oalloc-abort-on-error";
exported_char OALLOC_DISABLED[]             = "oalloc-disabled";
exported_char OALLOC_PRINT_STATS_ON_EXIT[]  = "oalloc-print-stats-on-exit";

exported_char FILE_ARGS[]                   = "file-args";

}

namespace SernaFileType {

exported_literal TYPE               = "filetype";
exported_literal TYPE_LIST          = "filetypes";
exported_literal PATTERN            = "pattern";
exported_literal DESCRIPTION        = "description";

}

namespace FileHandler {

exported_literal HANDLER_LIST       = "file-handler-list";
exported_literal REGISTERED_VIEWERS = "pdf,txt,ps,html,htm";
exported_literal HANDLER_LIST_TEMP  = "#file-handler-list";
exported_literal APP                = "file-handler";
exported_literal APP_EXT            = "extension";
exported_literal APP_PATH           = "executable";

}

#include "common/StringTokenizer.h"

namespace DocLook {

exported_literal DOC_LOOK           = "document-look";

exported_literal CONTEXT_HINTS      = "context-hints";
exported_literal HINTS_ENABLED      = "enabled";
exported_literal HINTS_PERSISTENT   = "persistent";
exported_literal HINTS_SHOW_PARENT  = "show-parent";
exported_literal HINTS_DELAY        = "delay";

exported_literal INDENT             = "indent";
exported_literal TAG_FONT           = "tag-font";
exported_literal CURSOR_WIDTH       = "cursor-width";

exported_literal EVEN_COLOR         = "even-color";
exported_literal ODD_COLOR          = "odd-color";

exported_literal NS_COLOR_MAP       = "ns-color-map";
exported_literal NS_COLOR_MAP_ENTRY = "ns-color-map-entry";
exported_literal DEFAULT_NS_COLOR   = "is-default";
exported_literal NS_URI             = "ns-uri";
exported_literal NS_TAG_COLOR       = "tag-color";
exported_literal NS_TEXT_COLOR      = "text-color";
exported_literal SHOW_NBSP          = "show-nbsp";

Formatter::Rgb parse_rgb_color(const Common::String& color)
{
    Common::StringTokenizer st(color, ",");
    Formatter::Rgb rgb;
    rgb.r_ = st.next().toUInt();
    rgb.g_ = st.next().toUInt();
    rgb.b_ = st.next().toUInt();
    return rgb;
}

using namespace Common;

String rgb_color(const Formatter::Rgb& rgb)
{
    return String::number(rgb.r_) + ',' + String::number(rgb.g_)
        + ',' + String::number(rgb.b_);
}

}

namespace App {

exported_literal APP                = "app";
exported_literal SYS_FONT           = "system-font";
exported_literal DOC_PATH           = "doc-path";
exported_literal ICON_PATH          = "icon-path";
exported_literal DOC_BROWSE         = "#doc_path";
exported_literal CLOSE_DOCKWINDOW_ON_ESCAPE = "close-dockwindow-on-escape";
exported_literal DONT_SHOW_SWITCH_EXPERT_MODE_FLAG = "app/dont-show-switch-expert-mode-dialog";
exported_literal DONT_SHOW_VALIDATION_ON_WARNING_FLAG = "app/dont-show-validation-on-warning-dialog";
exported_literal CRLF_LINE_BREAKS   = "crlf-line-breaks";
exported_literal APP_CRLF_LINE_BREAKS = "app/crlf-line-breaks";
exported_literal AUTOSAVE_DELAY     = "autosave/delay";
exported_literal AUTOSAVE_ENABLED   = "autosave/enabled";
exported_literal AUTOSAVE_IDLE_TIME = "autosave/idle-time";
exported_literal TRANSLATIONS_PATH  = "translations";
exported_literal UI_LANG            = "lang";
exported_literal MAX_UNDO_DEPTH     = "max-undo-depth";
exported_literal DISABLE_RESOURCE_MODTIME_CHECK = 
    "disable-resource-modtime-check";
exported_literal DISABLE_DOCUMENT_MODTIME_CHECK = 
    "disable-document-modtime-check";
}

namespace Speller {

exported_literal SPELLER            = "speller";
exported_literal SPELLER_LANGUAGES      = "#languages";
exported_literal SPELLER_DEFAULT_DICT   = "default-dict";

}

namespace Vars {

exported_literal VARS               = "vars";
exported_literal SCHEMA_PATH        = "schemas";
exported_literal STYLE_PATH         = "stylesheets";
exported_literal TEMPLATE_PATH      = "templates";
exported_literal XML_PATH           = "xml";
exported_literal PLUGINS_PATH       = "ext_plugins";

}

namespace Examples {

exported_literal EXAMPLES           = "examples";
exported_literal DONT_SHOW_ON_START = "dont-show-on-start";
exported_literal EXAMPLE            = "example";
exported_literal EXAMPLE_FILE       = "file";
exported_literal INSCRIPTION        = "inscription";
exported_literal READ               = "read";

}

namespace Registration {

exported_literal REGISTRATION       = "registration";
exported_literal LATER_DAY          = "register-later-remind-day";
exported_literal IS_REGISTERED      = "is-registered";
exported_literal DONT_SHOW_ON_START = "dont-show-on-start";

}

namespace XmlCatalogs {

exported_literal XML_CATALOGS       = "xml-catalogs";
exported_literal CAT_PATH           = "files";
exported_literal DTD_PATH           = "dtds";
exported_literal USE_CATALOGS       = "use";
exported_literal PREFER             = "prefer";

}

namespace ElementSpace {
    exported_literal ELEMENT_NAME       = "element-name";
}

// STOP_IGNORE_LITERALS
