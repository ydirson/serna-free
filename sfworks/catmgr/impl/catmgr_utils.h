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

/** \file
 */

#ifndef CATMGR_UTILS_H_
#define CATMGR_UTILS_H_

#include "common/StringDecl.h"

namespace GroveLib {
class Element;
}

namespace CatMgrImpl {

const Common::String& get_attr(const GroveLib::Element* node,
                               const Common::String& name);

bool in_catalog_ns(const GroveLib::Element* ep);

bool elem_has_name(const GroveLib::Element* ep, const char* name);

extern const char CAT_NS_URI[];

Common::String uri_from_path(const Common::String& path);

Common::String make_abs_uri(const Common::String& uri);

bool is_file_uri(const Common::String& uri);

Common::String get_filepath(const Common::String& uri);

Common::String get_base_uri(const Common::String& uri);

Common::String rebase_uri(const Common::String& uri, const Common::String& ref);

Common::String norm_uri(const Common::String& uri);

Common::String norm_pubid(const Common::String& pubid);

bool in_urn_ns(const Common::String& s);

Common::String unwrap_id(const Common::String& idstr);

unsigned get_modtime(const Common::String& uri);

}

#endif // CATMGR_UTILS_H_
