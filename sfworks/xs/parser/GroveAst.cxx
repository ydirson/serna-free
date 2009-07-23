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
 */

#include "xs/xs_defs.h"
#include "xs/Schema.h"
#include "xs/XsMessages.h"
#include "xs/XsDataImpl.h"
#include "xs/complex/Particle.h"
#include "xs/parser/GroveAst.h"
#include "xs/parser/TokenTable.h"
#include "xs/GroveAstParserTokenTypes.hpp"
#include "grove/XNodePtr.h"
#include "common/asserts.h"
#include "common/StringCvt.h"
#include <string>
#include <iostream>


#define UNIMPLEMENTED_AST_OP2 \
    RT_MSG_ABORT("unimplemented GroveAST operation"); \

#define UNIMPLEMENTED_AST_OP \
    RT_MSG_ABORT("unimplemented GroveAST operation"); \
    return false;   // MSVC needs this

USING_COMMON_NS
USING_GROVE_NAMESPACE

GroveAst::~GroveAst()
{
}

XS_NAMESPACE::Origin GroveAst::origin() const
{
    XS_NAMESPACE::Origin orig(elem_);
    return orig;
}

std::string GroveAst::toString() const
{
    return std::string(local_8bit(elem_->name()).c_str());
}

int GroveAst::getType() const
{
    if (!schema_->xsi()->isSchemaNode(elem_)) {
        if ((!schema_->xsi()->isSchemaExtNode(elem_)) ||
            (NOTR("python") != elem_->localName())) {
            schema_->mstream() << XsMessages::notSchemaNsElement
                               << Message::L_ERROR
                               << elem_->name()
                               << XSN_ORIGIN(origin());
            return 0;
        }
    }
    int token = XS_NAMESPACE::TokenTable::instance().
        getToken(elem_->localName());
    if (0 == token)
        schema_->mstream() << XsMessages::unknownSchemaElement
                           << Message::L_ERROR
                           << elem_->name()
                           << XSN_ORIGIN(origin());
    return token;
}

antlr::RefAST GroveAst::getFirstChild() const
{
    GROVE_NAMESPACE::Node* n = elem_->firstChild();
    if (0 == n)
        return 0;
    while (n && n->nodeType() != GROVE_NAMESPACE::Node::ELEMENT_NODE)
        n = n->nextSibling();
    if (0 == n)
        return 0;
    antlr::RefAST ast(new GroveAst(ELEMENT_CAST(n), schema_));
    return ast;
}

antlr::RefAST GroveAst::getNextSibling() const
{
    GROVE_NAMESPACE::Node* n = elem_->nextSibling();
    if (0 == n)
        return 0;
    while (n && n->nodeType() != GROVE_NAMESPACE::Node::ELEMENT_NODE)
        n = n->nextSibling();
    if (0 == n)
        return 0;
    antlr::RefAST ast(new GroveAst(ELEMENT_CAST(n), schema_));
    return ast;
}

std::string GroveAst:: getText() const
{
    return std::string(elem_->name().local8Bit().c_str());
}

void GroveAst::addChild(antlr::RefAST)
{
    UNIMPLEMENTED_AST_OP2
}

bool GroveAst::equals(antlr::RefAST) const
{
    UNIMPLEMENTED_AST_OP
}

bool GroveAst::equalsList(antlr::RefAST) const
{
    UNIMPLEMENTED_AST_OP
}

bool GroveAst::equalsListPartial(antlr::RefAST) const
{
    UNIMPLEMENTED_AST_OP
}

bool GroveAst::equalsTree(antlr::RefAST) const
{
    UNIMPLEMENTED_AST_OP
}

bool GroveAst::equalsTreePartial(antlr::RefAST) const
{
    UNIMPLEMENTED_AST_OP
}

std::vector<antlr::RefAST> GroveAst::findAll(antlr::RefAST)
{
    UNIMPLEMENTED_AST_OP2
    return std::vector<antlr::RefAST>();
}

std::vector<antlr::RefAST> GroveAst::findAllPartial(antlr::RefAST)
{
    UNIMPLEMENTED_AST_OP2
    return std::vector<antlr::RefAST>();
}

void GroveAst::initialize(int, const std::string&)
{
    UNIMPLEMENTED_AST_OP2
}

void GroveAst::initialize(antlr::RefAST)
{
    UNIMPLEMENTED_AST_OP2
}

void GroveAst::initialize(antlr::RefToken)
{
    UNIMPLEMENTED_AST_OP2
}

void GroveAst::setFirstChild(antlr::RefAST)
{
    UNIMPLEMENTED_AST_OP2
}

void GroveAst::setNextSibling(antlr::RefAST)
{
    UNIMPLEMENTED_AST_OP2
}

void GroveAst::setText(const std::string&)
{
    UNIMPLEMENTED_AST_OP2
}

void GroveAst::setType(int)
{
    UNIMPLEMENTED_AST_OP2
}

std::string GroveAst::toStringList() const
{
    UNIMPLEMENTED_AST_OP2
    return std::string("STRING-LIST");
}

std::string GroveAst::toStringTree() const
{
    UNIMPLEMENTED_AST_OP2
    return std::string("STRING-TREE");
}
