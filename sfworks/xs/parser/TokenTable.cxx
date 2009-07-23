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
#include "xs/GroveAstParserTokenTypes.hpp"
#include "xs/parser/TokenTable.h"

USING_COMMON_NS

XS_NAMESPACE_BEGIN

struct InitialTokenTable {
    const char*     name;
    int             token;
};

// START_IGNORE_LITERALS
static struct InitialTokenTable initialTokenTable[] = {
    { "schema",         GroveAstParserTokenTypes::XmlSchema},
    { "annotation",     GroveAstParserTokenTypes::Annotation},
    { "appinfo",        GroveAstParserTokenTypes::Appinfo},
    { "python",         GroveAstParserTokenTypes::Script},
    { "documentation",  GroveAstParserTokenTypes::Documentation},
    { "import",         GroveAstParserTokenTypes::Import},
    { "include",        GroveAstParserTokenTypes::Include},
    { "redefine",       GroveAstParserTokenTypes::Redefine},
    { "element",        GroveAstParserTokenTypes::Element},
    { "unique",         GroveAstParserTokenTypes::Unique},
    { "key",            GroveAstParserTokenTypes::Key},
    { "keyref",         GroveAstParserTokenTypes::Keyref},
    { "selector",       GroveAstParserTokenTypes::Selector},
    { "field",          GroveAstParserTokenTypes::Field},
    { "attribute",      GroveAstParserTokenTypes::Attribute},
    { "simpleType",     GroveAstParserTokenTypes::SimpleType},
    { "list",           GroveAstParserTokenTypes::List},
    { "restriction",    GroveAstParserTokenTypes::Restriction},
    { "union",          GroveAstParserTokenTypes::Union},
    { "complexType",    GroveAstParserTokenTypes::ComplexType},
    { "simpleContent",  GroveAstParserTokenTypes::SimpleContent},
    { "extension",      GroveAstParserTokenTypes::Extension},
    { "attributeGroup", GroveAstParserTokenTypes::AttributeGroup},
    { "anyAttribute",   GroveAstParserTokenTypes::AnyAttribute},
    { "complexContent", GroveAstParserTokenTypes::ComplexContent},
    { "group",          GroveAstParserTokenTypes::Group},
    { "all",            GroveAstParserTokenTypes::All},
    { "choice",         GroveAstParserTokenTypes::Choice},
    { "sequence",       GroveAstParserTokenTypes::Sequence},
    { "any",            GroveAstParserTokenTypes::Any},
    { "notation",       GroveAstParserTokenTypes::Notation},
    { "enumeration",    GroveAstParserTokenTypes::Enumeration},
    { "length",         GroveAstParserTokenTypes::Length},
    { "maxExclusive",   GroveAstParserTokenTypes::MaxExclusive},
    { "maxInclusive",   GroveAstParserTokenTypes::MaxInclusive},
    { "maxLength",      GroveAstParserTokenTypes::MaxLength},
    { "minExclusive",   GroveAstParserTokenTypes::MinExclusive},
    { "minInclusive",   GroveAstParserTokenTypes::MinInclusive},
    { "minLength",      GroveAstParserTokenTypes::MinLength},
    { "pattern",        GroveAstParserTokenTypes::Pattern},
    { "totalDigits",    GroveAstParserTokenTypes::TotalDigits},
    { "fractionDigits", GroveAstParserTokenTypes::FractionDigits},
    { "whiteSpace",     GroveAstParserTokenTypes::Whitespace},
    { 0, 0 }
};
// STOP_IGNORE_LITERALS

Tokens::Tokens()
{
    const InitialTokenTable* p = initialTokenTable;
    for (; p->name; ++p)
    map_.insert(new XmlSchemaToken(p->name, p->token));
}

XS_NAMESPACE_END
