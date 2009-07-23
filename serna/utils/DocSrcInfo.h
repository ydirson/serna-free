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

#ifndef UTILS_DOC_SRC_INFO_H_
#define UTILS_DOC_SRC_INFO_H_

#include "utils/utils_defs.h"

namespace Common {
    class String;
    class PropertyNode;
}

namespace DocSrcInfo {

// Well-known document properties

UTILS_EXPIMP extern const char* const DOC_PATH;
UTILS_EXPIMP extern const char* const IS_TEXT_DOCUMENT;
UTILS_EXPIMP extern const char* const NS_MAP;

UTILS_EXPIMP extern const char* const STYLE_PATH;
UTILS_EXPIMP extern const char* const RESOLVED_STYLE_PATH;

UTILS_EXPIMP extern const char* const SCHEMA_PATH;
UTILS_EXPIMP extern const char* const NONS_SCHEMA_PATH;
UTILS_EXPIMP extern const char* const RESOLVED_SCHEMA_PATH;
UTILS_EXPIMP extern const char* const NONS_SCHEMA_REWRITE;

UTILS_EXPIMP extern const char* const ENCODING;
UTILS_EXPIMP extern const char* const DOC_ENCODING;
UTILS_EXPIMP extern const char* const GENERATE_UNICODE_BOM;

UTILS_EXPIMP extern const char* const MODIFIED_TIME;
UTILS_EXPIMP extern const char* const WATCHED_FILE;
UTILS_EXPIMP extern const char* const WATCHED_ENTITY;
UTILS_EXPIMP extern const char* const WATCHED_ENTITY_PATH;
UTILS_EXPIMP extern const char* const WATCHED_ENTITY_TIME;

// Following properties are used as input by NewFileDialog/OpenFileDialog
UTILS_EXPIMP extern const char* const BROWSE_DIR;
UTILS_EXPIMP extern const char* const STYLESHEETS_DIR;
UTILS_EXPIMP extern const char* const SCHEMAS_DIR;

UTILS_EXPIMP extern const char* const ROOT_ELEMENTS;
UTILS_EXPIMP extern const char* const DOCUMENT_SKELETON;

UTILS_EXPIMP extern const char* const SCALE;
UTILS_EXPIMP extern const char* const ZOOM;
UTILS_EXPIMP extern const char* const SELECTION_BALANCING;
UTILS_EXPIMP extern const char* const SHOW_TAGS;
UTILS_EXPIMP extern const char* const SHOW_PAGINATED;

UTILS_EXPIMP extern const char* const DSI;
UTILS_EXPIMP extern const char* const DSI_ID;
UTILS_EXPIMP extern const char* const IS_NEW_DOCUMENT;
UTILS_EXPIMP extern const char* const RECENT_LIST;
UTILS_EXPIMP extern const char* const RECENT_MAXIMUM;

UTILS_EXPIMP extern const char* const HIDE_ELEMENTS;
UTILS_EXPIMP extern const char* const HIDE_ATTRIBUTES;

UTILS_EXPIMP extern const char* const FOLDING_RULES;
UTILS_EXPIMP extern const char* const FOLD_IF_LARGE;
UTILS_EXPIMP extern const char* const ELEMS_PER_PAGE;
UTILS_EXPIMP extern const char* const LARGE_DOC_LIMIT;
UTILS_EXPIMP extern const char* const FOLDING_LIST;
UTILS_EXPIMP extern const char* const FOLDING_MODE;
UTILS_EXPIMP extern const char* const HIDE_IN_NEWDOC_DIALOG;

UTILS_EXPIMP extern const char* const CURSOR_TREELOC;
UTILS_EXPIMP extern const char* const LINE_NUMBER;
UTILS_EXPIMP extern const char* const CURSOR_FILENAME;

UTILS_EXPIMP extern const char* const ZOOM_VALUES;

UTILS_EXPIMP extern const char* const VALIDATION_MODE;
UTILS_EXPIMP extern const char* const TOC_IS_ON;

UTILS_EXPIMP extern const char* const SPELLER_PROPS;
UTILS_EXPIMP extern const char* const SPELLER_SKIPELEMS;

UTILS_EXPIMP extern const char* const TEMPLATE_CATEGORY;
UTILS_EXPIMP extern const char* const TEMPLATE_NAME;
UTILS_EXPIMP extern const char* const TEMPLATE_DOCROOTGROVE;
UTILS_EXPIMP extern const char* const TEMPLATE_WHENOPEN;
UTILS_EXPIMP extern const char* const TEMPLATE_DTD_GUESS;
UTILS_EXPIMP extern const char* const TEMPLATE_FN_GUESS;
UTILS_EXPIMP extern const char* const TEMPLATE_ROOT_GUESS;
UTILS_EXPIMP extern const char* const TEMPLATE_NONS_SCHEMA_GUESS;
UTILS_EXPIMP extern const char* const TEMPLATE_PRIORITY;
UTILS_EXPIMP extern const char* const TEMPLATE_SHOWUP_PRIORITY;
UTILS_EXPIMP extern const char* const TEMPLATE_PATH;
UTILS_EXPIMP extern const char* const TEMPLATE_DIR;
UTILS_EXPIMP extern const char* const TEMPLATE_CATEGORY_PIXMAP;
UTILS_EXPIMP extern const char* const TEMPLATE_CATALOGS_LIST;
UTILS_EXPIMP extern const char* const RESOLVED_CATALOGS;
UTILS_EXPIMP extern const char* const TEMPLATE_ELEM_HELP_FILE;
UTILS_EXPIMP extern const char* const RESOLVED_ELEM_HELP_FILE;

UTILS_EXPIMP extern const char* const DOCINFO_DOCTYPE;
UTILS_EXPIMP extern const char* const DOCINFO_SYSID;
UTILS_EXPIMP extern const char* const DOCINFO_PUBID;
UTILS_EXPIMP extern const char* const DOCINFO_DOCELEM;

UTILS_EXPIMP extern const char* const DUMP_FO_PATH;
UTILS_EXPIMP extern const char* const ELEMENT_ID_FORMAT;

UTILS_EXPIMP extern const char* const PROCESS_XINCLUDE;

UTILS_EXPIMP Common::String makeDsiTooltip(const Common::PropertyNode*);
UTILS_EXPIMP Common::String docPathFilename(const Common::PropertyNode*);
UTILS_EXPIMP Common::String myDocumentsPath();

UTILS_EXPIMP void add_recent_document(Common::PropertyNode* dsi);
UTILS_EXPIMP Common::String get_browse_dir(Common::PropertyNode* dsi);
UTILS_EXPIMP void resolve_dsi(Common::PropertyNode* dsi,
                              const Common::PropertyNode* pdsi = 0);
UTILS_EXPIMP Common::PropertyNode* get_dsi(const int recentDepth);

} // namespace DocSrcInfo

#endif // UTILS_DOC_SRC_INFO_H_

