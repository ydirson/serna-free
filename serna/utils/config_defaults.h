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

/// This file contains precompiled defaults for Serna configuration.
#ifndef CONFIG_DEFAULTS_H_
#define CONFIG_DEFAULTS_H_

#include "utils/utils_defs.h"

extern const char SERNA_CONFIG_VERSION[]; // no EXPIMP

/* !!!!!!!! NOTE: serna2 prefix is temporarily used instead of serna */

// Property name for data directory (= install directory). This means
// that corresponding env. variable will be SERNA_DATA_DIR
extern UTILS_EXPIMP const char SERNA_DATA_DIR_PROPERTY[];

// Prefix for environment variables which are considered Serna config prop.
extern UTILS_EXPIMP const char SERNA_VAR_PREFIX[];

// Name of Serna per-user configuration directory
extern UTILS_EXPIMP const char SERNA_CONFIG_DIR[];

// Name of Serna application configuration file
extern UTILS_EXPIMP const char SERNA_CONFIG_FILE[];

// Name for explicit Serna Config dir env. variable (= SERNA_CONFIG_DIR)
extern UTILS_EXPIMP const char SERNA_CONFIG_DIR_ENVVAR[];

// Name for explicit Serna Config file env. variable (= SERNA_CONFIG_FILE)
extern UTILS_EXPIMP const char SERNA_CONFIG_FILE_ENVVAR[];

// Root tag name for configuration property file
extern UTILS_EXPIMP const char SERNA_XMLCONFIG_ROOT[];

// Name for the configuration file template (must be located in DATA_DIR)
extern UTILS_EXPIMP const char SERNA_CONFIG_TEMPLATE[];
extern UTILS_EXPIMP const char SERNA_CONFIG_UPDATE_TEMPLATE[];

///////////////////////////

// names of variables which can be used in config template file. They
// are not used once the per-user config file is constructed.
extern UTILS_EXPIMP const char INITCONFIG_DATADIR_VAR[]  ;
extern UTILS_EXPIMP const char INITCONFIG_CONFIGDIR_VAR[];

// Prefix names for target platforms
extern UTILS_EXPIMP const char TARGET_PLATFORM_PROPERTY_PREFIX[] ;

struct ConfigPropMapTable {
    const char*         version;
    const char**        pmap;
};

#endif
