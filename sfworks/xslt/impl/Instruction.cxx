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
#include "grove/Nodes.h"
#include "grove/NodeExt.h"
#include "grove/PrologNodes.h"
#include "xslt/impl/debug.h"
#include "xslt/impl/Instruction.h"
#include "xslt/impl/InstructionFactory.h"
#include "xslt/impl/instructions/Stylesheet.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/XsltFunctionFactory.h"
#include "common/Factory.h"
#include "grove/XmlName.h"
#include "xslt/XsltMessages.h"

using namespace Common;
using namespace GroveLib;

namespace Xslt {

Instruction::Instruction(const GroveLib::Element* element,
                         const Stylesheet* styleContext,
                         Instruction* p)
    : element_(element), stylesheet_(styleContext)
{
    if (p)
        p->appendChild(this);
}

Instruction::~Instruction()
{
    // prohibit myself from being killed twice in case of exception
    if (1 == getRefCnt()) {
        incRefCnt();
        remove();
    }
}

static String line_info(const GroveLib::Node* n)
{
    String str;
    if (n->grove())
        str += "URL: " + n->grove()->topSysid();
    if (n->nodeExt() && n->nodeExt()->asLineLocExt()) {
        LineLocExt* ext = n->nodeExt()->asLineLocExt();
        str += " (line: " + String::number(ext->line());
        str += " column: " + String::number(ext->column() + 1) + ")";
    }
    str += "\nname: " + n->nodeName();
    return str;
}

void Instruction::buildSubInstructions(bool isTopLevel)
{
    const GroveLib::Node* node = element()->firstChild();
    while (node) {
        switch (node->nodeType()) {
            case GroveLib::Node::SSEP_NODE:
                break;
            case GroveLib::Node::TEXT_NODE: {
                String text(static_cast<const GroveLib::Text*>(node)->data());
                /* FALL THROUGH */
            }
            case GroveLib::Node::ELEMENT_NODE: {
                const InstructionFactory* factory =
                    Common::Factory<InstructionFactory>::instance();
                Type type = factory->getTypeOf(node, styleContext());
                if (IGNORED_INSTRUCTION == type)
                    break;
                bool allowed_child =
                    (UNKNOWN_INSTRUCTION != type) && allowsChild(type);
                if (allowed_child)
                    addChild(factory->makeInstruction(node,
                        type, styleContext(), this));
                else {
                    if (LITERAL_RESULT_ELEMENT == type && isTopLevel) {
                        String err("Non-xslt instruction on top level <");
                        err += name(node) + "> : ignored";
                        warning(node, err);
                        addChild(factory->makeInstruction(node,
                            IGNORED_INSTRUCTION, styleContext(), this));
                        break;
                    }
                    if (UNKNOWN_INSTRUCTION == type &&
                            styleContext()->isForwardCompatible()) {
                        String err("Unknown xslt instruction <");
                        err += name(node) + "> : ignored";
                        warning(node, err);
                        if (isTopLevel)
                            addChild(factory->makeInstruction
                                       (node, IGNORED_INSTRUCTION,
                                        styleContext(), this));
                        else
                            addChild(factory->makeInstruction
                                       (node, UNKNOWN_INSTRUCTION,
                                        styleContext(), this));
                        break;
                    }
                    throw Xslt::Exception(XsltMessages::instrNotAllowed,
                          name(node), name(element_), "\n" + line_info(node));
                }
                break;
            }
            case GroveLib::Node::COMMENT_NODE:
                break;
            default:
                {
                    throw Xslt::Exception(XsltMessages::instrInvNodeType,
                          String(node->nodeType()), contextString());
                }
        }
        node = node->nextSibling();
    }
}

// This can be reimplemented by certain instructions, to know if the
// child was added
void Instruction::addChild(Instruction* i)
{
    i->checkAttributes();
}

const GroveLib::Element* Instruction::element() const
{
    return element_;
}

void Instruction::warning(const GroveLib::Node*, const String& msg) const
{
    DDBG << "instruction warning: " << msg << std::endl;
}

String Instruction::name(const GroveLib::Node* node) const
{
    return node->nodeName();
}

// TODO: distinguish namespaces
String Instruction::attr(const String& attr_name,
                         bool isRequired, bool mustbeQn) const
{
    GroveLib::QualifiedName aqn;
    aqn.parse(attr_name);
    attrList_.push_front(new AttrItem(aqn.localName()));
    const GroveLib::Element* e = element();
    const GroveLib::Attr* attr = e->attrs().firstChild();
    for (; attr; attr = attr->nextSibling())
        if (attr->localName() == aqn.localName())
            break;
    if (attr) {
        const String& sv = attr->value();
        if (mustbeQn) {
            const Char* ss = sv.unicode();
            const Char* se = ss + sv.length();
            for (; ss && ss < se; ++ss) {
                if (ss->isLetter() || ss->isDigit() || *ss == '.'
                    || *ss == '_' || *ss == '-' || *ss == ':')
                        continue;
                throw Xslt::Exception(XsltMessages::instrAttrName,
                                      attr_name, contextString());
            }
        }
        return sv;
    }
    else
        if (isRequired) {
            throw Xslt::Exception(XsltMessages::instrAttrReq,
                                  attr_name, contextString());
        }
    return String::null();
}

void Instruction::checkAttributes()
{
    const GroveLib::Element* e = element();
    if (0 == e || !isXsltNsUri(e->xmlNsUri()))
        return;
    const GroveLib::Attr* a = e->attrs().firstChild();
    for (; a; a = a->nextSibling()) {
        String attrNs = a->xmlNsUri();
        if (!attrNs.isEmpty() || !isXsltNsUri(attrNs))
            continue;
        const AttrItem* ai = attrList_.first();
        for (; ai; ai = ai->next())
            if (ai->attr == a->localName())
                break;
        if (ai)
            continue;
        throw Xslt::Exception(XsltMessages::instrAttrExtra,
                                  a->name(), contextString());
    }
    attrList_.destroyAll();
}

Instance* Instruction::makeInst(const InstanceInit&, Instance*) const
{
    throw Xslt::Exception(XsltMessages::instrNoInst, contextString());
}

void Instruction::setStyleContext(const Stylesheet* s)
{
    if (type() == STYLESHEET)
        return;
    stylesheet_ = s;
    for (Instruction* i = firstChild(); i; i = i->nextSibling())
        i->setStyleContext(s);
}

PatternExprPtr Instruction::makePattern(const Common::String& expression) const
{
    return PatternExpr::makePattern(expression, element(), 
        styleContext()->topStylesheet()->functionFactory());
}

Xpath::ExprPtr Instruction::makeExpr(const Common::String& expression) const
{
    return PatternExpr::makeExpr(expression, element(),
        styleContext()->topStylesheet()->functionFactory());
}

COMMON_NS::String Instruction::contextString() const
{
    String str;
    if (element()) {
        str = "\nInstruction context:\n";
        str += line_info(element());
    }
    return str;
}

void Instruction::dump() const
{
    DDINDENT;
    for (const Instruction* i = firstChild(); i; i = i->nextSibling())
        i->dump();
}

} // namespace Xslt

