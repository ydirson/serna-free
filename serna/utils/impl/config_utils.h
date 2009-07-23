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
// Copyright (c) 2005 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#ifndef CONFIG_UTILS_H_
#define CONFIG_UTILS_H_

#include "utils/utils_defs.h"
#include "common/StringDecl.h"
#include "common/PropertyTree.h"

namespace CfgUtils {

void resolve_init_variables(Common::PropertyNode* ptn,
                            const Common::String& dataDir,
                            const Common::String& configDir);

Common::PropertyNode*
read_from_template(const Common::String& dataDir,
                   const Common::String& configDir,
                   const Common::String& cfgTplFile);

Common::String get_config_dir(const Common::PropertyNode* env);
Common::String get_config_file(const Common::PropertyNode* env);

Common::String get_home_dir();
Common::String get_data_dir();

Common::PropertyNodePtr parse_cmdline(int argc, const char* const* argv);
Common::PropertyNodePtr parse_cmdline(const wchar_t* cmdline);

void fill_file_handlers_list(Common::PropertyNode* cfgRoot);

}

#endif // CONFIG_UTILS_H_
