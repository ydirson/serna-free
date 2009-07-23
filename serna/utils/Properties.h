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

#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#include "utils/utils_defs.h"

namespace CmdLineParams {

extern UTILS_EXPIMP const char CMD_LINE_PARAMS[];
extern UTILS_EXPIMP const char CMD_FILE_ARGS[];
extern UTILS_EXPIMP const char CMD_RECENT_FILE_ID[];
extern UTILS_EXPIMP const char INSTALL_CONFIG_PACKAGE[];
extern UTILS_EXPIMP const char INSTALL_CONFIG_PACKAGE_W_DIAG[];

extern UTILS_EXPIMP const char ENABLE_DEBUG_TAGS[];
extern UTILS_EXPIMP const char DISABLE_DEBUG_TAGS[];

extern UTILS_EXPIMP const char OALLOC_ABORT_ON_ERROR[];
extern UTILS_EXPIMP const char OALLOC_DISABLED[];
extern UTILS_EXPIMP const char OALLOC_PRINT_STATS_ON_EXIT[];

extern UTILS_EXPIMP const char FILE_ARGS[];

}

namespace SernaFileType {  //FileType is reserved in Mac

UTILS_EXPIMP extern const char* const TYPE_LIST;
UTILS_EXPIMP extern const char* const TYPE;
UTILS_EXPIMP extern const char* const PATTERN;
UTILS_EXPIMP extern const char* const DESCRIPTION;

}

namespace FileHandler {

UTILS_EXPIMP extern const char* const HANDLER_LIST;
UTILS_EXPIMP extern const char* const REGISTERED_VIEWERS;
UTILS_EXPIMP extern const char* const HANDLER_LIST_TEMP;
UTILS_EXPIMP extern const char* const APP;
UTILS_EXPIMP extern const char* const APP_EXT;
UTILS_EXPIMP extern const char* const APP_PATH;

}

#include "common/String.h"
#include "formatter/types.h"

namespace DocLook {

UTILS_EXPIMP extern const char* const DOC_LOOK;

UTILS_EXPIMP extern const char* const CONTEXT_HINTS;
UTILS_EXPIMP extern const char* const HINTS_ENABLED;
UTILS_EXPIMP extern const char* const HINTS_PERSISTENT;
UTILS_EXPIMP extern const char* const HINTS_SHOW_PARENT;
UTILS_EXPIMP extern const char* const HINTS_DELAY;

UTILS_EXPIMP extern const char* const INDENT;
UTILS_EXPIMP extern const char* const TAG_FONT;

UTILS_EXPIMP extern const char* const EVEN_COLOR;
UTILS_EXPIMP extern const char* const ODD_COLOR;

UTILS_EXPIMP extern const char* const NS_COLOR_MAP;
UTILS_EXPIMP extern const char* const NS_COLOR_MAP_ENTRY;
UTILS_EXPIMP extern const char* const DEFAULT_NS_COLOR;
UTILS_EXPIMP extern const char* const NS_URI;
UTILS_EXPIMP extern const char* const NS_TAG_COLOR;
UTILS_EXPIMP extern const char* const NS_TEXT_COLOR;
UTILS_EXPIMP extern const char* const SHOW_NBSP;

UTILS_EXPIMP Formatter::Rgb parse_rgb_color(const Common::String& color);
UTILS_EXPIMP Common::String rgb_color(const Formatter::Rgb& rgb);

}

namespace App {

UTILS_EXPIMP extern const char* const APP;
UTILS_EXPIMP extern const char* const SYS_FONT;
UTILS_EXPIMP extern const char* const DOC_PATH;
UTILS_EXPIMP extern const char* const ICON_PATH;
UTILS_EXPIMP extern const char* const DOC_BROWSE;
UTILS_EXPIMP extern const char* const CLOSE_DOCKWINDOW_ON_ESCAPE;
UTILS_EXPIMP extern const char* const DONT_SHOW_SWITCH_EXPERT_MODE_FLAG;
UTILS_EXPIMP extern const char* const DONT_SHOW_VALIDATION_ON_WARNING_FLAG;
UTILS_EXPIMP extern const char* const AUTOSAVE_DELAY;
UTILS_EXPIMP extern const char* const AUTOSAVE_ENABLED;
UTILS_EXPIMP extern const char* const AUTOSAVE_IDLE_TIME;
UTILS_EXPIMP extern const char* const CRLF_LINE_BREAKS;
UTILS_EXPIMP extern const char* const APP_CRLF_LINE_BREAKS;
UTILS_EXPIMP extern const char* const TRANSLATIONS_PATH;
UTILS_EXPIMP extern const char* const UI_LANG;
UTILS_EXPIMP extern const char* const MAX_UNDO_DEPTH;
UTILS_EXPIMP extern const char* const DISABLE_RESOURCE_MODTIME_CHECK;
UTILS_EXPIMP extern const char* const DISABLE_DOCUMENT_MODTIME_CHECK;

}

namespace Speller {

UTILS_EXPIMP extern const char* const SPELLER;
UTILS_EXPIMP extern const char* const SPELLER_LANGUAGES;
UTILS_EXPIMP extern const char* const SPELLER_DEFAULT_DICT;

}

namespace Vars {

UTILS_EXPIMP extern const char* const VARS;
UTILS_EXPIMP extern const char* const SCHEMA_PATH;
UTILS_EXPIMP extern const char* const STYLE_PATH;
UTILS_EXPIMP extern const char* const TEMPLATE_PATH;
UTILS_EXPIMP extern const char* const XML_PATH;
UTILS_EXPIMP extern const char* const PLUGINS_PATH;

}

namespace Examples {

UTILS_EXPIMP extern const char* const EXAMPLES;
UTILS_EXPIMP extern const char* const DONT_SHOW_ON_START;
UTILS_EXPIMP extern const char* const EXAMPLE;
UTILS_EXPIMP extern const char* const EXAMPLE_FILE;
UTILS_EXPIMP extern const char* const INSCRIPTION;
UTILS_EXPIMP extern const char* const READ;

}

namespace XmlCatalogs {

UTILS_EXPIMP extern const char* const XML_CATALOGS;
UTILS_EXPIMP extern const char* const CAT_PATH;
UTILS_EXPIMP extern const char* const DTD_PATH;
UTILS_EXPIMP extern const char* const USE_CATALOGS;
UTILS_EXPIMP extern const char* const PREFER;

}

namespace ElementSpace {

UTILS_EXPIMP extern const char* const ELEMENT_NAME;

}
#endif // PROPERTIES_H_

