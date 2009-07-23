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
/*! \file
 *  $RCSfile: XsSimpleTypeImpl.cxx,v $
 *
 ***********************************************************************/

#include "xs/datatypes/XsSimpleTypeImpl.h"
#include "xs/XsMessages.h"
#include "xs/XsMessages.h"
#include "xs/Schema.h"
#include "xs/XsNodeExt.h"
#include "xs/Origin.h"
#include "xs/SchemaNamespaces.h"
#include "grove/ValidationCommandsMaker.h"
#include "grove/Nodes.h"
#include "grove/ChoiceNode.h"
#include <iostream>

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

XsSimpleTypeImpl::XsSimpleTypeImpl(WhatType type,
                                   const Origin& origin,
                                   const NcnCred& cred)
    : XsType(XsType::simpleType, origin, cred),
      type_(type)
{
}

bool XsSimpleTypeImpl::validate(Schema* schema,
                                GROVE_NAMESPACE::Element* elem,
                                bool /*recursive*/) const
{
    bool ok = true;
    for (Attr* a = elem->attrs().firstChild(); a; a = a->nextSibling()) {
        if (schema && XSI_NAMESPACE != a->xmlNsUri()) {
            schema->mstream() << XsMessages::attrExtra << elem->nodeName()
                              << a->nodeName()
                              << Message::L_ERROR
                              << DV_ORIGIN(a);
            ok = false;
        }
    }
    return ok && validate_simple_data(schema, this, elem);
}

bool validate_simple_data(Schema* schema, 
                          const XsSimpleTypeImpl* xsti,
                          GroveLib::Element* elem)
{                     
    String source, result;
    bool is_choice = false, have_choice_node = false;
    for (const Node* n = elem->firstChild(); n ; n = n->nextSibling()) {
        if (GroveLib::Node::TEXT_NODE == n->nodeType()) {
            source += CONST_TEXT_CAST(n)->data();
            if (n->nodeName()[1] != 't') {
                have_choice_node = true;
                if (GroveLib::is_text_choice(CONST_TEXT_CAST(n)))
                    is_choice = true;
            }
        } else if (GroveLib::Node::ELEMENT_NODE == n->nodeType()) {
            if (schema)
                schema->mstream() << XsMessages::elemsInSimpleContent
                                  << Message::L_ERROR
                                  << SRC_ORIGIN(elem);
            return false;
        }
    }
    if (is_choice && source.isEmpty())
        return true;
    if (0 == xsti) {
        if (source.isEmpty())
            return true;
        if (schema)
            schema->mstream() << XsMessages::simpleContentEmpty
                              << Message::L_ERROR
                              << SRC_ORIGIN(elem);
        return false;
    }
    RefCntPtr<Text> t = make_text_choice(xsti);
    bool can_modify = 
        (schema->validationFlags() & (Schema::editMode|Schema::makeChanges));
    bool silent = source.isEmpty() && can_modify;
    bool ok = xsti->validate(schema, elem, source, &result, silent);
    if (!can_modify)
        return ok;
    if (have_choice_node)
        return ok;
    t->setData(result);
    GroveLib::set_text_choice(t.pointer(), true);
    if (schema->commandMaker()) {
        GroveLib::Node* n = elem->firstChild();
        while (n) {
            GroveLib::Node* ns = n->nextSibling();
            schema->commandMaker()->removeNode(n);
            n = ns;
        }
        schema->commandMaker()->insertNode(0, elem, t.pointer());
    } else {
        elem->removeAllChildren();
        elem->appendChild(t.pointer());
    }
    return ok;
}

bool XsSimpleTypeImpl::validate(Schema* schema,
                                const String& source,
                                String* result) const
{
    return validate(schema, 0, source, result);
}

void XsSimpleTypeImpl::makeAttrs(Schema*, GROVE_NAMESPACE::Element*) const
{
}

bool XsSimpleTypeImpl::makeContent(Schema* schema,
                                  GROVE_NAMESPACE::Element* parentElem,
                                  FixupSet*,
                                  const String* defv) const
{
    String os, res;
    validate(schema, 0, defv ? *defv : String(), &os, true);
    RefCntPtr<Text> t = make_text_choice(this);
    if (t->nodeName() != "#text" || os.length() > 0) {
        t->setData(os);
        if (schema->commandMaker()) 
            schema->commandMaker()->insertNode(0, parentElem, t.pointer());
        else
            parentElem->appendChild(t.pointer());
    }
    return true;
}

XsSimpleTypeImpl::~XsSimpleTypeImpl()
{
}

XsSimpleTypeImpl::WhatType XsSimpleTypeImpl::type() const
{
    return type_;
}

void XsSimpleTypeImpl::dump(int indent) const
{
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++)
         std::cerr << ' ';
    std::cerr << NOTR("XsSimpleType:") << std::endl;
#endif // XS_DEBUG
}

PRTTI_IMPL(XsSimpleTypeImpl)
PRTTI_BASE_STUB(XsSimpleTypeImpl, AtomicType)
PRTTI_BASE_STUB(XsSimpleTypeImpl, SimpleDerivedType)

XS_NAMESPACE_END
