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
#ifndef STRUCTEDITOR_IMPL_XS_UTILS_H_
#define STRUCTEDITOR_IMPL_XS_UTILS_H_

#include "structeditor/se_defs.h"
#include "common/common_defs.h"

class XsAttributeList;

namespace Common
{
    class PropertyNode;
    class String;
}

namespace GroveLib
{
    class Element;
    class Node;
    class ExpandedName;
    class Attr;
}

namespace GroveEditor
{
    class GrovePos;
}

class AttrNameChecker {
public:
    virtual ~AttrNameChecker() {}

    virtual bool    isValid(const Common::String& value) const = 0;
};

namespace AttributesSpace {

STRUCTEDITOR_EXPIMP extern const char* const ATTR_TYPE;
STRUCTEDITOR_EXPIMP extern const char* const ATTR_SPEC_LIST;
STRUCTEDITOR_EXPIMP extern const char* const ATTR_VALUE_ENUM;
STRUCTEDITOR_EXPIMP extern const char* const EXISTING_ATTRS;
STRUCTEDITOR_EXPIMP extern const char* const POSSIBLE_ATTRS;
STRUCTEDITOR_EXPIMP extern const char* const NS_MAP;

STRUCTEDITOR_EXPIMP extern const char* const TABLE_COL_SPECS;
STRUCTEDITOR_EXPIMP extern const char* const TABLE_COL;
STRUCTEDITOR_EXPIMP extern const char* const TABLE_COL_TITLE;
STRUCTEDITOR_EXPIMP extern const char* const TABLE_COL_STRETCHABLE;
STRUCTEDITOR_EXPIMP extern const char* const TABLE_COL_EDITABLE;

}

namespace ElementSpace {

STRUCTEDITOR_EXPIMP extern const char* const ELEM_VALUE_ENUM;
STRUCTEDITOR_EXPIMP extern const char* const ELEM_VALUE_DEFAULT;
STRUCTEDITOR_EXPIMP extern const char* const ELEM_VALUE_IS_LIST;

}
// Structure of attribute properties tree: each item is:
//
// <attr-spec-list>
//    <attr-name, uri>
//       <type> ?
//       <is-required> ?
//       <default-value> ?
//       <fixed-value> ?
//       <enum> ?
//          <value> *

GroveLib::Element* traverse_to_element(const GroveEditor::GrovePos& pos);

void STRUCTEDITOR_EXPIMP
get_schema_attributes(GroveLib::Element* elem,
                           Common::PropertyNode* attrSpecs);
void STRUCTEDITOR_EXPIMP
get_schema_attributes(const GroveEditor::GrovePos& pos,
                           const COMMON_NS::String& name,
                           Common::PropertyNode* attrSpecs);

bool is_cdata_allowed(const GroveLib::Node* node);

bool get_simpletype_content(GroveLib::Element* elem, 
                            Common::PropertyNode* spec);

const Common::PropertyNode* 
find_attr_spec(const Common::PropertyNode* specList,
               const GroveLib::Attr* attr);

#endif // STRUCTEDITOR_IMPL_XS_UTILS_H_

