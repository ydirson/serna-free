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
#include "structeditor/se_defs.h"
#include "structeditor/impl/XsUtils.h"

#include "common/PropertyTree.h"

#include "grove/Nodes.h"
#include "grove/XmlPredefNs.h"
#include "grove/IdManager.h"

#include "groveeditor/GrovePos.h"

#include "xs/Schema.h"
#include "xs/XsType.h"
#include "xs/XsElement.h"
#include "xs/XsNodeExt.h"

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;

typedef STRUCTEDITOR_EXPIMP const char* const exported_literal;


namespace ElementSpace {
exported_literal ELEM_VALUE_ENUM        = Xs::ELEM_VALUE_ENUM;
exported_literal ELEM_VALUE_DEFAULT     = Xs::ELEM_VALUE_DEFAULT;
exported_literal ELEM_VALUE_IS_LIST     = Xs::ELEM_VALUE_IS_LIST;
}

namespace AttributesSpace {
// START_IGNORE_LITERALS
exported_literal ATTR_TYPE              = Xs::ATTR_TYPE;
exported_literal ATTR_SPEC_LIST         = "attr-spec-list";
exported_literal ATTR_VALUE_ENUM        = Xs::ATTR_VALUE_ENUM;
exported_literal EXISTING_ATTRS         = "existing-attrs";
exported_literal POSSIBLE_ATTRS         = "possible-attrs";
exported_literal NS_MAP                 = "ns-map";

exported_literal TABLE_COL_SPECS        = "table-col-specs";
exported_literal TABLE_COL              = "table-col";
exported_literal TABLE_COL_TITLE        = "col-title";
exported_literal TABLE_COL_STRETCHABLE  = "col-stretchable";
exported_literal TABLE_COL_EDITABLE     = "col-editable";
// STOP_IGNORE_LITERALS
}

using namespace AttributesSpace;
using namespace ElementSpace;

GroveLib::Element* traverse_to_element(const GrovePos& pos)
{
    GroveLib::Node* node = pos.node();
    while (0 != node && GroveLib::Node::ELEMENT_NODE != node->nodeType())
        node = node->parent();
    if (node)
        return static_cast<GroveLib::Element*>(node);
    return 0;
}

static void get_predefined_attributes(PropertyNode* attrSpecs)
{
    if (0 == attrSpecs->getProperty("xml:base")) {
        PropertyNode* base = new PropertyNode(NOTR("xml:base"), W3C_XML_NAMESPACE);
        base->appendChild(new PropertyNode(ATTR_TYPE, NOTR("xml")));
        attrSpecs->appendChild(base);
    }
    if (0 == attrSpecs->getProperty("xml:lang")) {
        PropertyNode* lang = new PropertyNode(NOTR("xml:lang"), W3C_XML_NAMESPACE);
        lang->appendChild(new PropertyNode(ATTR_TYPE, NOTR("xml")));
        attrSpecs->appendChild(lang);
    }
    if (0 == attrSpecs->getProperty("xml:space")) {
        PropertyNode* space = new PropertyNode(NOTR("xml:space"), W3C_XML_NAMESPACE);
        space->appendChild(new PropertyNode(ATTR_TYPE, NOTR("xml")));
        PropertyNode* value_enum = space->makeDescendant(Xs::ATTR_VALUE_ENUM);
        value_enum->appendChild(new PropertyNode(NOTR("default")));
        value_enum->appendChild(new PropertyNode(NOTR("preserve")));
        attrSpecs->appendChild(space);
    }
}

void get_schema_attributes(Element* elem, PropertyNode* attrSpecs)
{
    if (0 == elem)
        return;
    attrSpecs->removeAllChildren();
    XsNodeExt::getAttrs(attrSpecs, elem);
    get_predefined_attributes(attrSpecs);
}

void get_schema_attributes(const GrovePos& pos, const String& name,
                           PropertyNode* attrSpecs)
{
    GroveLib::Element* parent = 0;
    GroveLib::Node* child = 0;
    attrSpecs->removeAllChildren();

    if (pos.type() == GroveEditor::GrovePos::ELEMENT_POS) {
        parent = ELEMENT_CAST(pos.node());
        child = pos.before();
    }
    else
        if (pos.type() == GroveEditor::GrovePos::TEXT_POS) {
            parent = ELEMENT_CAST(pos.text()->parent());
            child = pos.node();
        }
        else
            return;
    if (child)
        child = child->prevSibling();
    else
        if (parent)
            child = parent->lastChild();
    GroveLib::QualifiedName qname;
    qname.parse(name);
    GroveLib::ExpandedName ename;
    ename.resolve(parent, qname);
    if (ename.localName().isNull())
        return;
    const Xs::XsElement* xs_elem =
        XsNodeExt::getNextXsElem(parent, child, ename);
    XsNodeExt::getAttrs(attrSpecs, xs_elem, parent);
    get_predefined_attributes(attrSpecs);
}

bool is_cdata_allowed(const Node* node)
{
    if (Node::CHOICE_NODE == node->nodeType())
        return false;
    if (Node::TEXT_NODE == node->nodeType())    
        return true;
    const Xs::XsElement* xs_element = XsNodeExt::xsElement(node);
    return (xs_element) ? xs_element->cdataAllowed() : true;
}

bool get_simpletype_content(Element* elem, PropertyNode* spec)
{
    if (0 == elem)
        return false;
    spec->removeAllChildren();
    return XsNodeExt::getSTCEvalue(spec, elem);
}

const PropertyNode* find_attr_spec(const PropertyNode* specList,
                                   const Attr* attr)
{
    String localName = attr->localName();
    String ns = attr->xmlNsUri();
    const PropertyNode* pn = specList->firstChild();
    for (; pn; pn = pn->nextSibling())
        if (pn->name() == localName && pn->getString() == ns)
            return pn;
    return 0;
}
