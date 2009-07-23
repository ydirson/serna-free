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

#include "utils/config_defaults.h"
#include "utils/appver.hpp"

const char SERNA_CONFIG_VERSION[] = "2.5"; // no EXPIMP


// ! NOTICE:
// START_IGNORE_LITERALS

/// This file contains precompiled defaults for Serna configuration.

/* !!!!!!!! NOTE: serna2 prefix is temporarily used instead of serna */

// Property name for data directory (= install directory). This means
// that corresponding env. variable will be SERNA_DATA_DIR
extern UTILS_EXPIMP const char SERNA_DATA_DIR_PROPERTY[] = "data_dir";

// Prefix for environment variables which are considered Serna config prop.
extern UTILS_EXPIMP const char SERNA_VAR_PREFIX[] = "serna_";

#if !defined(SERNA_CONFDIR_PREFIX)
# define SERNA_CONFDIR_PREFIX ".serna-free-"
#endif
// Name of Serna per-user configuration directory
extern UTILS_EXPIMP const char SERNA_CONFIG_DIR[] = SERNA_CONFDIR_PREFIX SERNA_VERSION_ID;

// Name of Serna application configuration file
#ifdef NDEBUG
extern UTILS_EXPIMP const char SERNA_CONFIG_FILE[] = "config.xml";
#else  // NDEBUG
extern UTILS_EXPIMP const char SERNA_CONFIG_FILE[] = "config-debug.xml";
#endif // NDEBUG

// Name for explicit Serna Config dir env. variable (= SERNA_CONFIG_DIR)
extern UTILS_EXPIMP const char SERNA_CONFIG_DIR_ENVVAR[] = "config_dir";

// Name for explicit Serna Config file env. variable (= SERNA_CONFIG_FILE)
extern UTILS_EXPIMP const char SERNA_CONFIG_FILE_ENVVAR[] = "config_file";

// Root tag name for configuration property file
extern UTILS_EXPIMP const char SERNA_XMLCONFIG_ROOT[] = "serna-config";

// Name for the configuration file template (must be located in DATA_DIR)
extern UTILS_EXPIMP const char SERNA_CONFIG_TEMPLATE[] = "config-template.xml";

extern UTILS_EXPIMP const char
SERNA_CONFIG_UPDATE_TEMPLATE[] = "config-update.xml";

///////////////////////////

// names of variables which can be used in config template file. They
// are not used once the per-user config file is constructed.
extern UTILS_EXPIMP const char INITCONFIG_DATADIR_VAR[]   = "DATA_DIR";
extern UTILS_EXPIMP const char INITCONFIG_CONFIGDIR_VAR[] = "CONFIG_DIR";

// Prefix names for target platforms
#if defined(_WIN32)
extern UTILS_EXPIMP const char TARGET_PLATFORM_PROPERTY_PREFIX[]  = "win";
#elif defined(__APPLE__)
extern UTILS_EXPIMP const char TARGET_PLATFORM_PROPERTY_PREFIX[]  = "mac";
#else // _WIN32
extern UTILS_EXPIMP const char TARGET_PLATFORM_PROPERTY_PREFIX[]  = "unix";
#endif // _WIN32

// Property merge tables that prescibe which properties from old configuraution
// are moved to new configurations automatically on new version config.
// (~/.serna2/old_config -> ~/.serna2/new_config). Tree properties are
// also copied (merged with override).
// Merge tables should be sorted by version number. They are applied
// sequentally (e.g. both none_to_2_1 and 2_1_to_2_2 will be applied
// for 2.2 config), so further versions must contain ONLY differences
// from previous version.

static const char* none_to_2_1_map[] = {
    // in OLD config        should become in NEW config
    // -------------------------------------------------
    // "app/system-font",  "app/system-xren",
    //"document-look/indent", "document-look/indent",
    //"document-look/even-color", "document-look/even-color",
    //"document-look/odd-color", "document-look/odd-color",
    //"document-look/tag-font", "document-look/tag-font",
    "app/doc-path",                  "app/doc-path",
    "app/system-font",               "app/system-font",
    "app/close-dockwindow-on-escape","app/close-dockwindow-on-escape",
    "app/autosave",                  "app/autosave",
    "app/disabled-plugins",          "app/disabled-plugins",
    "app/insert-element-item/close-on-insert",  "app/insert-element-item/close-on-insert",
    "speller/default-dict",          "speller/default-dict",
    "vars/stylesheets",              "vars/stylesheets",
    "vars/templates",                "vars/templates",
    "vars/xml",                      "vars/xml",
    "vars/schemas",                  "vars/schemas",
    "vars/dtds",                     "vars/dtds",
    "doc-defaults/folding-rules",    "doc-defaults/folding-rules",
    "xml-catalogs",                  "xml-catalogs",
    "document-look",                 "document-look",
    "filetypes",                     "filetypes",
    "file-handler-list",             "file-handler-list",
    0,          0
};

static const char* v2_1_to_2_2_map[] = {
    // in OLD config        should become in NEW config
    // -------------------------------------------------
    "document-look/indent", "document-look/indent",
    "document-look/even-color", "document-look/even-color",
    "document-look/odd-color", "document-look/odd-color",
    "document-look/tag-font", "document-look/tag-font",
    "app/doc-path",                  "app/doc-path",
    "app/system-font",               "app/system-font",
    "app/close-dockwindow-on-escape","app/close-dockwindow-on-escape",
    "app/autosave",                  "app/autosave",
    "app/disabled-plugins",          "app/disabled-plugins",
    "app/insert-element-item/close-on-insert",  "app/insert-element-item/close-on-insert",
    "speller/default-dict",          "speller/default-dict",
    "vars/stylesheets",              "vars/stylesheets",
    "vars/templates",                "vars/templates",
    "vars/xml",                      "vars/xml",
    "vars/schemas",                  "vars/schemas",
    "vars/dtds",                     "vars/dtds",
    "doc-defaults/folding-rules",    "doc-defaults/folding-rules",
    "xml-catalogs",                  "xml-catalogs",
    "document-look",                 "document-look",
    "filetypes",                     "filetypes",
    "file-handler-list",             "file-handler-list",
    0,          0
};

static const char* v2_2_to_2_3_map[] = {
    // in OLD config        should become in NEW config
    // -------------------------------------------------
    "document-look/indent", "document-look/indent",
    "document-look/even-color", "document-look/even-color",
    "document-look/odd-color", "document-look/odd-color",
    "document-look/tag-font", "document-look/tag-font",
    "app/doc-path",                  "app/doc-path",
    "app/system-font",               "app/system-font",
    "app/close-dockwindow-on-escape","app/close-dockwindow-on-escape",
    "app/autosave",                  "app/autosave",
    "app/disabled-plugins",          "app/disabled-plugins",
    "app/insert-element-item/close-on-insert",  "app/insert-element-item/close-on-insert",
    "speller/default-dict",          "speller/default-dict",
    "vars/stylesheets",              "vars/stylesheets",
    "vars/templates",                "vars/templates",
    "vars/xml",                      "vars/xml",
    "vars/schemas",                  "vars/schemas",
    "vars/dtds",                     "vars/dtds",
    "doc-defaults/folding-rules",    "doc-defaults/folding-rules",
    "xml-catalogs",                  "xml-catalogs",
    "document-look",                 "document-look",
    "filetypes",                     "filetypes",
    "file-handler-list",             "file-handler-list",
    0,          0
};

static const char* v2_3_to_2_5_map[] = {
    // in OLD config        should become in NEW config
    // -------------------------------------------------
    "document-look/indent", "document-look/indent",
    "document-look/even-color", "document-look/even-color",
    "document-look/odd-color", "document-look/odd-color",
    "document-look/tag-font", "document-look/tag-font",
    "app/doc-path",                  "app/doc-path",
    "app/system-font",               "app/system-font",
    "app/close-dockwindow-on-escape","app/close-dockwindow-on-escape",
    "app/autosave",                  "app/autosave",
    "app/disabled-plugins",          "app/disabled-plugins",
    "app/insert-element-item/close-on-insert",  "app/insert-element-item/close-on-insert",
    "speller/default-dict",          "speller/default-dict",
    "vars/stylesheets",              "vars/stylesheets",
    "vars/templates",                "vars/templates",
    "vars/xml",                      "vars/xml",
    "vars/schemas",                  "vars/schemas",
    "vars/dtds",                     "vars/dtds",
    "doc-defaults/folding-rules",    "doc-defaults/folding-rules",
    "xml-catalogs",                  "xml-catalogs",
    "document-look",                 "document-look",
    "filetypes",                     "filetypes",
    "file-handler-list",             "file-handler-list",
    0,          0
};

static const char* v2_5_to_2_6_map[] = {
    // in OLD config        should become in NEW config
    // -------------------------------------------------
    "examples", "examples",
    0,          0
};

// NOTE: There never was no configuration version 2.4 !

ConfigPropMapTable prop_map_tables[] = {
    // from-version     remap-table
    { "0.0",            none_to_2_1_map },
    { "2.1",            v2_1_to_2_2_map },
    { "2.2",            v2_2_to_2_3_map },
    { "2.3",            v2_3_to_2_5_map },
    { "2.5",            v2_5_to_2_6_map },
    //---------------------------------------------
    { 0, 0 } // END MARKER
};

// STOP_IGNORE_LITERALS
