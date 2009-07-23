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
#include "xslt/xslt_defs.h"
#include "xslt/XsltResource.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/instructions/Template.h"
#include "xslt/impl/instructions/BuiltInTemplate.h"
#include "xslt/impl/TemplateProvider.h"
#include "xslt/impl/instructions/instructions.h"
#include "xslt/impl/DocumentHolder.h"
#include "xslt/impl/instructions/Attribute.h"
#include "xslt/impl/WhitespaceStripper.h"
#include "xslt/impl/XsltFunctionFactory.h"
#include "xslt/impl/debug.h"
#include "xpath/Engine.h"
#include "grove/Nodes.h"
#include "common/MessageUtils.h"
#include "common/PropertyTree.h"
#include "common/Url.h"
#include <set>

USING_COMMON_NS;

namespace Xslt {

class TopStylesheet::TopVariableMap :
    public std::map<COMMON_NS::String, const Variable*> {};
class TopStylesheet::AttributeSetMap :
    public std::map<COMMON_NS::String, const AttributeSet*> {};
class TopStylesheet::UseAttributeSetList :
    public std::list<const UseAttributeSet*> {};
class TopStylesheet::CallTemplateList :
    public std::list<CallTemplate*> {};

static const char SERNA_STYLE_DIR_PARAM[] = "serna.stylesheet.dir";

TopStylesheet::TopStylesheet(const GroveLib::Element* element,
                             XsltResource* xsltResource)
    : Stylesheet(element, xsltResource->uri(), 0, 0, this),
      ApplicatingInstruction("/", this),
      topVarMap_(new TopVariableMap),
      attrSetMap_(new AttributeSetMap),
      usl_(new UseAttributeSetList),
      callTemplateList_(new CallTemplateList),
      builtinTemplates_(new BuiltinTemplateProvider(this))
{
    docHolder_ = new DocumentHolder(xsltResource);
    setSpc("preserve");
    setCanFold("false");
    String opts = attr("optimization", false);
    int xpath_opts = XsltFunctionFactory::OPT_NORMAL;
    if (opts.contains("dita"))
        xpath_opts |= XsltFunctionFactory::OPT_DITA;
    DDBG << "XSLT: optimization=" << xpath_opts << std::endl;
    functionFactory_ = new XsltFunctionFactory(xpath_opts);
}

void TopStylesheet::registerAttributeSetUse(const UseAttributeSet* u) const
{
    usl_->push_back(u);
}

void TopStylesheet::registerCallTemplate(CallTemplate* c) const
{
    callTemplateList_->push_back(c);
}

static void combine_stylesheets_include(Instruction* i,
                                        const TopStylesheet* top)
{
    Instruction* ci = i->firstChild();
    while (ci) {
        if (ci->type() == Instruction::IMPORT ||
            ci->type() == Instruction::INCLUDE) {
                Instruction* stylesheet = ci->firstChild();
                if (!stylesheet || stylesheet->type() !=
                    Instruction::STYLESHEET)
                        throw Xslt::Exception(XsltMessages::instrInclude,
                                  stylesheet->contextString());
                combine_stylesheets_include(stylesheet, top);
                if (ci->type() == Instruction::IMPORT) {
                    ci = ci->nextSibling();
                    continue;
                }
                COMMON_NS::RefCntPtr<Instruction> ti;
                Instruction* next = 0;
                Instruction* sci = stylesheet->firstChild();
                const Stylesheet* upperStylesheet = 0;
                for (next = ci->parent(); next; next = next->parent()) {
                    upperStylesheet = dynamic_cast<const Stylesheet*>(next);
                    if (upperStylesheet)
                        break;
                }
                if (0 == upperStylesheet)
                    upperStylesheet = top;
                while (sci) {
                    ti = sci;
                    next = sci->nextSibling();
                    sci->remove();
                    ti->setStyleContext(upperStylesheet);
                    ci->insertBefore(ti.pointer());
                    sci = next;
                }
                next = ci->nextSibling();
                ci->remove();
                ci = next;
        } else
            ci = ci->nextSibling();
    }
}

static void combine_stylesheets_import(Stylesheet* cs, Stylesheet*& prev)
{
    Instruction* ci = cs->lastChild();
    while (ci) {
        if (ci->type() == Instruction::IMPORT) {
            Instruction* stn = ci->firstChild();
            if (!stn || stn->type() != Instruction::STYLESHEET)
                throw Xslt::Exception(XsltMessages::instrImportChild,
                                      stn->contextString());
            Stylesheet* stylesheet = static_cast<Stylesheet*>(stn);
            prev->setNextImport(stylesheet);
            prev = stylesheet;
            combine_stylesheets_import(stylesheet, prev);
        }
        ci = ci->prevSibling();
    }
}

static void register_templates(Instruction* i)
{
    Instruction* ci = i->lastChild();
    for (; ci; ci = ci->prevSibling()) {
        if (ci->type() == Instruction::TEMPLATE) {
            Template* t = static_cast<Template*>(ci);
            TemplateProvider* tp = const_cast<TemplateProvider*>
                (t->styleContext()->templateProvider());
            if (!tp->registerTemplate(t)) {
                t->warning(t->element(),
                    "Template: skipped due to duplication");
                DBG(XSLT.TEST) << "Skipped template: name="
                    << t->name() << " mode=" << t->mode()
                    << " match=" << t->match() << std::endl;
            }
        } else if (ci->type() == Instruction::UNPROCESSED_ATTRIBUTE)
            ci = static_cast<Attribute*>(ci)->remakeSelf();
        register_templates(ci);
    }
}

void TopStylesheet::buildVariables()
{
    typedef std::set<COMMON_NS::String> NameSet;
    const Stylesheet* s = this;
    NameSet shadowVars;
    for (;;) {
        if (0 == s)
            break;
        NameSet varSet;
        const Instruction* i = s->firstChild();
        for (; i; i = i->nextSibling()) {
            if (Instruction::VARIABLE != i->type() &&
                Instruction::PARAM != i->type())
                    continue;
            const Variable* vi = static_cast<const Variable*>(i);
            // variables with the same name but lowest imp. prec. are shadowed
            if (vi->name().isEmpty())
                throw Xslt::Exception(XsltMessages::instrNullVar,
                                      contextString());
            if (varSet.find(vi->name()) != varSet.end()) {
                throw Xslt::Exception(XsltMessages::instrDupVar, vi->name(),
                                      contextString());
            }
            varSet.insert(vi->name());
            if (shadowVars.find(vi->name()) != shadowVars.end())
                continue;
            (*topVarMap_)[vi->name()] = vi;
            shadowVars.insert(vi->name());
        }
        s = s->nextImported();
    }
    TopVariableMap::iterator vit = topVarMap_->find(SERNA_STYLE_DIR_PARAM);
    if (vit != topVarMap_->end())
        vit->second->setConstString
            (Url(element()->grove()->topSysid())[Url::DIRPATH]);
    DBG_EXEC(XSLT.VARS, dump_variables());
}

void TopStylesheet::buildAttributeSets()
{
    const Stylesheet* s = this;
    AttributeSetMap::const_iterator asi;
    while (s) {
        const Instruction* i = s->lastChild();
        for (; i; i = i->prevSibling()) {
            if (Instruction::ATTRIBUTE_SET != i->type())
                continue;
            const AttributeSet* as = static_cast<const AttributeSet*>(i);
            asi = attrSetMap_->find(as->name());
            if (asi == attrSetMap_->end()) {
                (*attrSetMap_)[as->name()] = as;
                continue;
            }
            asi->second->merge(as); // merges use-string and attrs
        }
        s = s->nextImported();
    }
    DBG_IF(XSLT.ATTRS) {
        DDBG << "Resulting attribute sets:\n";
        AttributeSetMap::const_iterator asi = attrSetMap_->begin();
        for (; asi != attrSetMap_->end(); ++asi)
            asi->second->dump();
    }
    UseAttributeSetList::const_iterator itl = usl_->begin();
    for (; itl != usl_->end(); ++itl)
        (*itl)->createAttributes();
}

const AttributeSet*
TopStylesheet::getAttributeSet(const COMMON_NS::String& name) const
{
    AttributeSetMap::const_iterator it = attrSetMap_->find(name);
    if (it == attrSetMap_->end())
        return 0;
    return it->second;
}

void TopStylesheet::combineStylesheets()
{
    combine_stylesheets_include(this, this);
    Stylesheet* prev = this;
    combine_stylesheets_import(this, prev);
    buildVariables();
    buildAttributeSets();
    register_templates(this);

    CallTemplateList::iterator it = callTemplateList_->begin();
    for (; it != callTemplateList_->end(); ++it)
        (*it)->resolve();
    callTemplateList_ = 0;
    whitespaceStripper_ = new WhitespaceStripper(this, false);
}

const Variable*
TopStylesheet::getTopVariable(const COMMON_NS::String& name) const
{
    TopVariableMap::const_iterator it = topVarMap_->find(name);
    if (it != topVarMap_->end())
        return it->second;
    return 0;
}

void TopStylesheet::getTopParamList(PropertyNode* root) const
{
    TopVariableMap::const_iterator it = topVarMap_->begin();
    for (; it != topVarMap_->end(); ++it) {
        if (it->second->type() != PARAM)
            continue;
        const TopParam* tp = static_cast<const TopParam*>(it->second);
        if (tp->varExpr().isNull())
            continue;
        PropertyNode* pn = new PropertyNode(it->first);
        if (!tp->valueType().isEmpty())
            pn->appendChild(new PropertyNode("type", tp->valueType()));
        if (!tp->annotation().isEmpty())
            pn->appendChild(new PropertyNode("annotation", tp->annotation()));
        root->appendChild(pn);
    }
}

TopStylesheet::~TopStylesheet()
{
}

void TopStylesheet::dump_variables() const
{
#ifndef NDEBUG
    TopVariableMap::const_iterator vi = topVarMap_->begin();
    DDBG << "Top-level var decl list: ";
    for (; vi != topVarMap_->end(); ++vi)
        DDBG << '<' << vi->first << "> ";
    DDBG << std::endl;
#endif // DEBUG
}

} // namespace Xslt
