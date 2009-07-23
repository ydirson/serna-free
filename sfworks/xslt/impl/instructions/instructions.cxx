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

#include "xslt/xslt_defs.h"
#include "common/String.h"
#include "common/StringTokenizer.h"
#include "common/MessageUtils.h"
#include "grove/Nodes.h"

#include "xpath/Engine.h"
#include "common/Factory.h"

#include "xslt/PatternExpr.h"
#include "xslt/XsltResource.h"
#include "xslt/impl/DocumentHolder.h"
#include "xslt/impl/InstructionFactory.h"
#include "xslt/impl/XsltFunctionFactory.h"
#include "xslt/impl/AttributeValueTemplate.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/instructions/instructions.h"
#include "xslt/impl/instructions/BuiltInTemplate.h"
#include "xslt/impl/WhitespaceStripper.h"
#include "xslt/impl/instructions/CopyOf.h"
#include "xslt/impl/debug.h"
#include "xslt/impl/utils.h"

#include <set>
#include <algorithm>

USING_COMMON_NS
USING_GROVE_NAMESPACE

namespace Xslt {

ApplicatingInstruction::ApplicatingInstruction(const String& select,
                                               const Instruction* icontext)
    : select_(select),
      spc_(UNSPEC_SECTIONS),
      canFold_(UNSPEC_FOLD)
{
    if (select == "/") {
        // special case for TopStylesheet
        selectExpr_ = PatternExpr::makeExpr(select, icontext->element());
        return;
    }
    if (!select.isEmpty()) {
        try {
            selectExpr_ = icontext->makeExpr(select);
        }
        catch (Xpath::Exception& e) {
            String what = e.what();
            const GroveLib::NodeWithNamespace* nselem = icontext->element();
            String path = nselem->grove()->topSysid();
            String name = nselem->nodeName();
            String line, column;
            if (nselem->nodeExt() &&
                nselem->nodeExt()->asLineLocExt()) {
                LineLocExt* ext = nselem->nodeExt()->asLineLocExt();
                line = String::number(ext->line());
                column = String::number(ext->column());
            }
            throw Xslt::Exception(XsltMessages::instrContext,
                        what, path, line, column, name);
        }
    } else
        selectExpr_ = PatternExpr::makeExpr("node()", icontext->element());
}

void ApplicatingInstruction::setSpc(const COMMON_NS::String& sect)
{
    // todo: to make this work, need to pass parent to instruction
    if (!sect.isEmpty()) {
        if ("omit" == sect)
            spc_ = OMIT_SECTIONS;
        else if ("balance" == sect)
            spc_ = BALANCE_SECTIONS;
        else if ("preserve" == sect)
            spc_ = PRESERVE_SECTIONS;
        else if ("preserve-left" == sect)
            spc_ = PRESERVE_LEFT_SECTIONS;
        else if ("preserve-right" == sect)
            spc_ = PRESERVE_RIGHT_SECTIONS;
        else
            throw Xslt::Exception(XsltMessages::instrApplSections);
    }
}

void ApplicatingInstruction::setCanFold(const Common::String& v)
{
    if (v.isEmpty())
        return;
    if ("true" == v)
        canFold_ = FOLDING_ENABLED;
    else if ("false" == v)
        canFold_ = FOLDING_DISABLED;
    else
        throw Xslt::Exception(XsltMessages::instrApplFoldTmpl);
}

PRTTI_IMPL(ApplicatingInstruction);
PRTTI_BASE_STUB(Instruction, ApplicatingInstruction);
PRTTI_IMPL2(ApplyImports, ApplicatingInstruction);
PRTTI_IMPL2(ApplyTemplates, ApplicatingInstruction);
PRTTI_IMPL2(ElementTemplate, ApplicatingInstruction);

///////////////////////////////////////////////////////////////////////

ApplyImports::ApplyImports(const GroveLib::Element* elem,
                           const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      ApplicatingInstruction(".", this)
{
    setSpc(attr("sections", false));
    setCanFold(attr("apply-serna-fold-template", false));
}

void ApplyImports::dump() const
{
    DDBG << "ApplyImports:" << std::endl;
    Instruction::dump();
}

///////////////////////////////////////////////////////////////////////

ApplyTemplates::ApplyTemplates(const GroveLib::Element* elem,
                               const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      ApplicatingInstruction(attr("select", false), this),
      mode_(attr("mode", false))
{
    setSpc(attr("sections", false));
    setCanFold(attr("apply-serna-fold-template", false));
    buildSubInstructions();
}

bool ApplyTemplates::allowsChild(Type type) const
{
    switch (type) {
        case SORT :
        case WITH_PARAM :
            return true;
        default :
            return false;
    }
    return false;
}

void ApplyTemplates::dump() const
{
    DDBG << "ApplyTemplates: select=" << selectString()
        << " mode=" << mode_ << " SPROC" << (int)sectProcType() << std::endl;
    selectExpr().dump();
    Instruction::dump();
}

/////////////////////////////////////////////////////////////////

Attribute::Attribute(const GroveLib::Element* elem,
                     const Stylesheet* context, Instruction* p)
    : AttributeCommon(elem, context, p)
{
    attr("name", true);
    attr("namespace", false);
    buildSubInstructions();
}

const COMMON_NS::String& Attribute::nameString() const
{
    return element()->attrs().getAttribute("name")->value();
}

AttributeCommon* Attribute::remakeSelf()
{
    typedef COMMON_NS::OwnerPtr<AttributeValueTemplate> AvtOwner;
    const GroveLib::Attr* a = element()->attrs().getAttribute("name");
    const XsltFunctionFactory* xff =
        styleContext()->topStylesheet()->functionFactory();
    AvtOwner name(new AttributeValueTemplate(a, xff)); 
    AvtOwner ns;
    a = element()->attrs().getAttribute("namespace");
    if (a)
        ns = new AttributeValueTemplate(a, xff);
    AttributeCommon* newInstr = 0;
    bool isConst = true;
    bool isExpr  = true;
    Instruction* i = 0;
    for (i = firstChild(); i; i = i->nextSibling()) {
        if (i->type() != VALUE_OF)
            isExpr = false;
        if (i->type() != LITERAL_RESULT_TEXT)
            isConst = false;
    }
    if (isConst)
        isExpr = false;
    if (isExpr && firstChild() != lastChild())
        isExpr = false; // multiple instances
    COMMON_NS::String constValue;
    Xpath::ExprPtr exprValue;
    if (isConst) {
        for (i = firstChild(); i; i = i->nextSibling())
            constValue += static_cast<const LiteralResultText*>(i)->text();
    }
    if (isExpr)
        exprValue = static_cast<const ValueOf*>(firstChild())->selectExpr();
    if (name->isAvt() || (ns && ns->isAvt())) {
        if (isConst)
            newInstr = new AvtConstAttribute(this,
                name.release(), ns.release(), constValue);
        else if (isExpr)
            newInstr = new AvtExprAttribute(this,
                name.release(), ns.release(), exprValue);
        else
            newInstr = new AvtRtfAttribute(this,
                name.release(), ns.release());
    } else {
        if (isConst)
            newInstr = new SimpleConstAttribute(this,
                name.pointer(), ns.pointer(), constValue);
        else if (isExpr)
            newInstr = new SimpleExprAttribute(this,
                name.pointer(), ns.pointer(), exprValue);
        else
            newInstr = new SimpleRtfAttribute(this,
                name.pointer(), ns.pointer());
    }
    if (!isConst && !isExpr && firstChild()) {
        for (Instruction* ci = firstChild(); ci; ) {
            COMMON_NS::RefCntPtr<Instruction> ip = ci;
            ci = ci->nextSibling();
            ip->remove();
            newInstr->appendChild(ip.pointer());
        }
    }
    insertBefore(newInstr);
    remove();
    return newInstr;
}

bool Attribute::allowsChild(Type type) const
{
    switch (type) {
        case IMPORT:
        case INCLUDE:
        case ATTRIBUTE_SET:
        case DECIMAL_FORMAT:
        case KEY:
        case NAMESPACE_ALIAS:
        case OUTPUT:
        case PRESERVE_SPACE:
        case STRIP_SPACE:
        case SORT:
        case TEMPLATE:
            return false;

        default :
            return true;
    }
    return true;
}

////////

void AttributeAvtNameProvider::dump_np() const
{
    DDBG << "Attribute (AVT Name): name=" << nameString() << " ns="
        << (ns() ? nsString() : String()) << std::endl;
    {
        DDINDENT;
        name()->valueExpr()->dump();
        if (ns())
            ns()->valueExpr()->dump();
    }
}

void AttributeSimpleNameProvider::dump_np() const
{
    DDBG << "Attribute(SimpleName): name=" << nameString()
        << " ns=" << nsString() << std::endl;
}

void AttributeConstValueProvider::dump_vp() const
{
    DDBG << "  ConstAttrValue: " << value_ << std::endl;
}

void AttributeExprValueProvider::dump_vp() const
{
    DDBG << "  ValueExpr: ";
    DDINDENT;
    valueExpr()->dump();
}

void AttributeRtfValueProvider::dump_vp() const
{
}

////////////////////////////////////////////////////

class UseAttributeSet::UseSet  : public std::set<COMMON_NS::String> {};
class UseAttributeSet::AttrSet : public std::set<const AttributeCommon*> {};

class AttributeSet::ChildAttrMap :
    public std::map<COMMON_NS::String, const AttributeCommon*> {};

AttributeSet::AttributeSet(const GroveLib::Element* elem,
                           const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      name_(attr("name", true))
{
    String us = attr("use-attribute-sets", false);
    if (!us.isEmpty()) {
        useSet_ = new UseSet;
        for (StringTokenizer t(us); t; )
            useSet_->insert(t.next());
        if (useSet_->find(name_) != useSet_->end())
            throw Xslt::Exception(XsltMessages::instrAttrSetCircular,
                                  contextString());
    }
    buildSubInstructions();
    childMap_ = new ChildAttrMap;
    for (Instruction* i = firstChild(); i; ) {
        AttributeCommon* a = static_cast<AttributeCommon*>(i);
        if (i->type() == UNPROCESSED_ATTRIBUTE)
            a = static_cast<Attribute*>(a)->remakeSelf();
        (*childMap_)[a->nameString()] = a;
        i = a->nextSibling();
    }
}

void AttributeSet::merge(const AttributeSet* other) const
{
    AttributeSet* const self = const_cast<AttributeSet*>(this);
    // merge use-attribute list
    if (!other->useSet_.isNull()) {
        if (useSet_.isNull())
            self->useSet_ = new UseSet;
        UseSet::const_iterator aui = other->useSet_->begin();
        for (; aui != other->useSet_->end(); ++aui)
            self->useSet_->insert(*aui);
    }
    // merge attributes
    ChildAttrMap::const_iterator it, cai = other->childMap_->begin();
    for (; cai != other->childMap_->end(); ++cai) {
        it = childMap_->find(cai->second->nameString());
        if (it == childMap_->end())
            (*self->childMap_)[cai->second->nameString()] = cai->second;
    }
}

void AttributeSet::collect_attributes(AttrSet& aset, UseSet& uset) const
{
    // collect my own attributes
    Instruction* i = firstChild();
    while (i) {
        AttributeCommon* a = 0;
        if (Instruction::UNPROCESSED_ATTRIBUTE == i->type()) {
            a = static_cast<Attribute*>(i)->remakeSelf();
            i = a->nextSibling();
        } else if (Instruction::ATTRIBUTE == i->type()) {
            a = static_cast<AttributeCommon*>(i);
            i = i->nextSibling();
        } else {
            i = i->nextSibling();
            continue;
        }
        if (uset.find(a->nameString()) != uset.end())
            continue;
        uset.insert(a->nameString());
        aset.insert(a);
    }
    if (useSet_.isNull())
        return;
    const TopStylesheet* ts = styleContext()->topStylesheet();
    // collect from use-sets
    UseSet::const_iterator ui = useSet_->begin();
    for (; ui != useSet_->end(); ++ui) {
        const AttributeSet* as = ts->getAttributeSet(*ui);
        if (0 == as)
            throw Xslt::Exception(XsltMessages::instrAttrUndef, *ui,
                                  contextString());
        as->collect_attributes(aset, uset);
    }
}

void AttributeSet::dump() const
{
    DDBG << "AttributeSet: " << this << " name: " << name_ << std::endl;
    DDINDENT;
    if (!useSet_.isNull()) {
        DDBG << "Uses attribute-sets: ";
        UseSet::const_iterator it = useSet_->begin();
        for (; it != useSet_->end(); ++it)
            DDBG << *it << ' ';
        DDBG << std::endl;
    }
    ChildAttrMap::const_iterator ci = childMap_->begin();
    for (; ci != childMap_->end(); ++ci)
        ci->second->dump();
}

AttributeSet::~AttributeSet()
{
}

UseAttributeSet::UseAttributeSet(const GroveLib::Element* element,
                                 Instruction* p)
    : p_(p)
{
    const GroveLib::Attr* a = element->attrs().firstChild();
    for (; a; a = a->nextSibling()) {
        if (a->localName() != "use-attribute-sets")
            continue;
        useString_ = a->value();
        if (!useString_.isEmpty())
            p->styleContext()->topStylesheet()->registerAttributeSetUse(this);
        return;
    }
}

void UseAttributeSet::createAttributes() const
{
    if (useString_.isEmpty())
        return;
    // collect all attributes we already have
    UseSet usedAttrs;
    if (Instruction::LITERAL_RESULT_ELEMENT == p_->type()) {
        const LiteralResult* lre = static_cast<const LiteralResult*>(p_);
        LiteralResult::AttrValueIterator ati = lre->attributes().begin();
        for (; ati != lre->attributes().end(); ++ati)
            usedAttrs.insert((*ati)->name());
    }
    DBG_IF(XSLT.ATTRS) {
        DDBG << "UseAttributeSet: used attrs: ";
        UseSet::const_iterator ui = usedAttrs.begin();
        for (; ui != usedAttrs.end(); ++ui)
            DDBG << *ui << ' ';
        DDBG << std::endl;
    }
    AttrSet aset;
    const TopStylesheet* ts = p_->styleContext()->topStylesheet();
    for (StringTokenizer t(useString_); t; ) {
        String asname(t.next());
        const AttributeSet* as = ts->getAttributeSet(asname);
        if (0 == as)
            throw Xslt::Exception(XsltMessages::instrAttrSet,
                                  asname, p_->contextString());
        as->collect_attributes(aset, usedAttrs);
    }
    Instruction* before = p_->firstChild();
    for (AttrSet::const_iterator asi = aset.begin(); asi != aset.end(); ++asi) {
        DBG(XSLT.ATTRS) << "Making use-attribute child: "
            << (*asi)->nameString() << std::endl;
        Instruction* uai = new UseAttributeInstruction(*asi);
        if (before)
            before->insertBefore(uai);
        else
            p_->appendChild(uai);
    }
}

///////////////////////////////////////////////////

CallTemplate::CallTemplate(const GroveLib::Element* elem,
                           const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      name_(attr("name", true)),
      template_(0)
{
    p->styleContext()->topStylesheet()->registerCallTemplate(this);
    buildSubInstructions();
}

void CallTemplate::resolve()
{
    template_ = styleContext()->topStylesheet()->
        templateProvider()->getTemplate(name());
    if (0 == template_) {
        throw Xslt::Exception(XsltMessages::instrCallTmpl, name(),
                              contextString());
    }
}

bool CallTemplate::allowsChild(Type type) const
{
    return type == WITH_PARAM;
}

void CallTemplate::dump() const
{
    DDBG << "CallTemplate: name=" << name_ << std::endl;
    Instruction::dump();
}

///////////////////////////////////////////////////

When::When(const GroveLib::Element* elem,
           const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      test_(attr("test", true))
{
    String expr = "boolean(" + test_ + ")";
    try {
        testExpr_= makeExpr(expr);
    }
    catch (Xpath::Exception& e) {
        throw Xslt::Exception(XsltMessages::instrXpath, e.what(),
                              contextString());
    }
    buildSubInstructions();
}

bool When::allowsChild(Type type) const
{
    switch (type) {
        case IMPORT:
        case INCLUDE:
        case ATTRIBUTE_SET:
        case DECIMAL_FORMAT:
        case KEY:
        case NAMESPACE_ALIAS:
        case OUTPUT:
        case PRESERVE_SPACE:
        case STRIP_SPACE:
        case SORT:
        case TEMPLATE:
            return false;

        default :
            return true;
    }
    return true;
}

void When::dump() const
{
    DDBG << "When:" << std::endl;
    testExpr_->dump();
    Instruction::dump();
}

///////////////////////////////////////////////////

Otherwise::Otherwise(const GroveLib::Element* elem,
                     const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p)
{
    buildSubInstructions();
}

bool Otherwise::allowsChild(Type type) const
{
    switch (type) {
        case IMPORT:
        case INCLUDE:
        case ATTRIBUTE_SET:
        case DECIMAL_FORMAT:
        case KEY:
        case NAMESPACE_ALIAS:
        case OUTPUT:
        case PRESERVE_SPACE:
        case STRIP_SPACE:
        case SORT:
        case TEMPLATE:
            return false;

        default :
            return true;
    }
    return true;
}

void Otherwise::dump() const
{
    DDBG << "Otherwise:" << std::endl;
    Instruction::dump();
}

///////////////////////////////////////////////////

Choose::Choose(const GroveLib::Element* elem,
               const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p)
{
    buildSubInstructions();
}

bool Choose::allowsChild(Type type) const
{
    switch (type) {
        case WHEN: {
            if (firstChild() &&
                OTHERWISE == lastChild()->type())
                    throw Xslt::Exception(XsltMessages::instrChooseLast,
                                          contextString());
            return true;
        }
        case OTHERWISE: {
            if (firstChild() &&
                OTHERWISE == lastChild()->type())
                    throw Xslt::Exception(XsltMessages::instrChoosePer,
                                          contextString());
            return true;
        }
        default:
            return false;
    }
    return false;
}

void Choose::addChild(Instruction* child)
{
    Instruction::addChild(child);
    if (WHEN == lastChild()->type())
        whenList_.push_back(static_cast<When*>(child));
}

const Instruction* Choose::otherwise() const
{
    Instruction* lc = lastChild();
    if (0 == lc)
        return 0;
    if (OTHERWISE == lc->type())
        return lc;
    return 0;
}

void Choose::dump() const
{
    DDBG << "Choose:" << std::endl;
    Instruction::dump();
}

//////////////////////////////////////////////////

Copy::Copy(const GroveLib::Element* elem,
           const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      UseAttributeSet(elem, this)
{
    buildSubInstructions();
}

bool Copy::allowsChild(Type type) const
{
    switch (type) {
        case IMPORT:
        case INCLUDE:
        case ATTRIBUTE_SET:
        case DECIMAL_FORMAT:
        case KEY:
        case NAMESPACE_ALIAS:
        case OUTPUT:
        case PRESERVE_SPACE:
        case STRIP_SPACE:
        case SORT:
        case TEMPLATE:
            return false;

        default :
            return true;
    }
    return true;
}

void Copy::dump() const
{
    DDBG << "Copy:" << std::endl;
    Instruction::dump();
}

//////////////////////////////////////////////////

CopyOf::CopyOf(const GroveLib::Element* elem,
               const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      select_(attr("select", true))
{
    try {
        selectExpr_ = makeExpr(select_);
    }
    catch (Xpath::Exception& e) {
        throw Xslt::Exception(XsltMessages::instrXpath, e.what(),
                              contextString());
    }
}

void CopyOf::dump() const
{
    DDBG << "CopyOf: select=" << select_ << std::endl;
    selectExpr_->dump();
    Instruction::dump();
}

////////////////////////////////////////////////

DecimalFormat::DecimalFormat(const GroveLib::Element* elem,
                             const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p)
{
    attr("name", false);
    attr("decimal-separator", false);
    attr("grouping-separator", false);
    attr("infinity", false);
    attr("minus-sign", false);
    attr("NaN", false);
    attr("percent", false);
    attr("per-mille", false);
    attr("zero-digit", false);
    attr("digit", false);
    attr("pattern-separator", false);
}

void DecimalFormat::dump() const
{
    DDBG << "DecimalFormat: " << std::endl;
    Instruction::dump();
}

//// other stubs

ProcessingInstruction::ProcessingInstruction(const GroveLib::Element* elem,
                                             const Stylesheet* context,
                                             Instruction* p)
    : Instruction(elem, context, p)
{
    attr("name", false);
}

void ProcessingInstruction::dump() const
{
    DDBG << "ProcessingInstruction: " << std::endl;
    Instruction::dump();
}

////

Comment::Comment(const GroveLib::Element* elem,
                 const Stylesheet* context,
                 Instruction* p)
    : Instruction(elem, context, p)
{
}

void Comment::dump() const
{
    DDBG << "Comment: " << std::endl;
    Instruction::dump();
}

/////////////////////////////////////////////////

Element::Element(const GroveLib::Element* elem,
                 const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      UseAttributeSet(elem, this)
{
    attr("name", true); //! Just to check existance
    attr("namespace", false);
    const XsltFunctionFactory* xff =
        styleContext()->topStylesheet()->functionFactory();
    name_ = new AttributeValueTemplate(elem->attrs().getAttribute("name"), xff);
    if (elem->attrs().getAttribute("namespace"))
        namespace_ = new AttributeValueTemplate(
            elem->attrs().getAttribute("namespace"), xff);
    buildSubInstructions();
}

bool Element::allowsChild(Type type) const
{
    switch (type) {
        case IMPORT:
        case INCLUDE:
        case ATTRIBUTE_SET:
        case DECIMAL_FORMAT:
        case KEY:
        case NAMESPACE_ALIAS:
        case OUTPUT:
        case PRESERVE_SPACE:
        case STRIP_SPACE:
        case SORT:
        case TEMPLATE:
            return false;

        default :
            return true;
    }
    return true;
}

void Element::dump() const
{
    DDBG << "Element: name=" << name_->value() << " namespace="
        << (namespace_.isNull() ? String("") : namespace_->value())
        << std::endl;
    name_->valueExpr()->dump();
    if (!namespace_.isNull())
        namespace_->valueExpr()->dump();
    Instruction::dump();
}

///////////////////////////////////////////////


Fallback::Fallback(const GroveLib::Element* elem,
                   const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p)
{
    buildSubInstructions();
}

void Fallback::dump() const
{
    DDBG << "Fallback:" << std::endl;
    Instruction::dump();
}

////////////////////////////////////////////////////


ForEach::ForEach(const GroveLib::Element* elem,
                 const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      select_(attr("select", true))
{
    try {
        selectExpr_ = makeExpr(select_);
    }
    catch (Xpath::Exception& e) {
        throw Xslt::Exception(XsltMessages::instrXpath, e.what(),
                              contextString());
    }

    buildSubInstructions();
}

ForEach::~ForEach()
{
    while (!sortList_.empty()) {
        delete sortList_.back();
        sortList_.pop_back();
    }
}

bool ForEach::allowsChild(Type type) const
{
    switch (type) {
        case IMPORT:
        case INCLUDE:
        case ATTRIBUTE_SET:
        case DECIMAL_FORMAT :
        case KEY:
        case NAMESPACE_ALIAS:
        case OUTPUT:
        case PRESERVE_SPACE:
        case STRIP_SPACE:
        case TEMPLATE:
            return false;

        case SORT :
            if (!firstChild())
                return true;
            else
                throw Xslt::Exception(XsltMessages::instrForEach,
                                      contextString());
        default:
            return true;
    }
    return true;
}

void ForEach::addChild(Instruction* child)
{
    if (SORT == child->type()) {
        sortList_.push_back(static_cast<Sort*>(child));
        return;
    }
    Instruction::addChild(child);
}

void ForEach::dump() const
{
    DDBG << "ForEach: select=" << select_ << std::endl;
    selectExpr_->dump();
    if (!sortList_.empty())
        DDBG << "Sorting specs: " << std::endl;
    for (Sort::CIterator i = sortList_.begin(); i != sortList_.end(); i++)
        (*i)->dump();
    Instruction::dump();
}

///////////////////////////////////////////////////

If::If(const GroveLib::Element* elem,
       const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      test_(attr("test", true))
{
    String expr = "boolean(" + test_ + ")";
    try {
        testExpr_= makeExpr(expr);
    }
    catch (Xpath::Exception& e) {
        throw Xslt::Exception(XsltMessages::instrXpath, e.what(),
                              contextString());
    }

    buildSubInstructions();
}

bool If::allowsChild(Type type) const
{
    switch (type) {
        case IMPORT:
        case INCLUDE:
        case ATTRIBUTE_SET:
        case DECIMAL_FORMAT:
        case KEY:
        case NAMESPACE_ALIAS:
        case OUTPUT:
        case PRESERVE_SPACE:
        case STRIP_SPACE:
        case SORT:
        case TEMPLATE:
            return false;

        default:
            return true;
    }
    return true;
}

void If::dump() const
{
    DDBG << "If: test=" << test_ << std::endl;
    testExpr_->dump();
    Instruction::dump();
}

////////////////////////////////////////////////

Ignored::Ignored(const GroveLib::Element* elem,
                 const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p)
{
}

void Ignored::dump() const
{
    DDBG << "Ignored: <" << name(element()) << '>' << std::endl;
    Instruction::dump();
}

/////////////////////////////////////////////////

IncludeBase::IncludeBase(const GroveLib::Element* elem,
                         const Stylesheet* cont, Instruction* p)
    : Instruction(elem, cont, p)
{
    DocumentHolder* dh = styleContext()->topStylesheet()->docHolder();
    DocumentHolder::UriStack& uri_stk = dh->uriStack();
    String href = attr("href", true);
    String altHref = attr("alt-href", false);
    if (!altHref.isEmpty())
        href = altHref;
    uriRef_ = getRelativePath(href, cont->origin());
    if (uri_stk.end() != std::find(uri_stk.begin(), uri_stk.end(), uriRef_)) {
        throw Xslt::Exception(XsltMessages::instrImportCycle,
                              uriRef_, contextString());
    }
    MessageStream* pstream = dh->xsltResource()->pstream();
    if (pstream) {
        String s = "XSLT: import " + uriRef_;
        *pstream << s;
    }
    RefCntPtr<Uri::GroveResource> gr = dynamic_cast<Uri::GroveResource*>
        (dh->xsltResource()->getResource("grove",
            *dh->xsltResource()->builder(), uriRef_));
    if (gr.isNull())
        throw Xslt::Exception(XsltMessages::instrImportNoDoc,
                              uriRef_, contextString());
    RefCntPtr<GroveLib::Grove> style_grove = gr->grove();
    dh->addGrove(gr.pointer());
    GroveLib::Element* root = style_grove->document()->documentElement();
    if (0 == root)
        throw Xslt::Exception(XsltMessages::instrImportParse,
                              uriRef_, contextString());
    WhitespaceStripper ws;
    ws.stripStyle(root);
    const InstructionFactory* factory =
        Common::Factory<InstructionFactory>::instance();
    Instruction::Type type = factory->getTypeOf(root, styleContext());
    if (Instruction::STYLESHEET != type)
        throw Xslt::Exception(XsltMessages::instrImport, contextString());
    uri_stk.push_back(uriRef_);
    factory->makeStylesheet(root, styleContext(), uriRef_, this);
    uri_stk.pop_back();
}

Import::Import(const GroveLib::Element* elem,
               const Stylesheet* cont, Instruction* p)
    : IncludeBase(elem, cont, p)
{
}

void Import::dump() const
{
    DDBG << "Import: href=" << uriRef_ << std::endl;
    Instruction::dump();
}

//////////////////////////////////////////////

Include::Include(const GroveLib::Element* elem,
                 const Stylesheet* cont, Instruction* p)
    : IncludeBase(elem, cont, p)
{
}

void Include::dump() const
{
    DDBG << "Include: href=" << uriRef_ << std::endl;
    Instruction::dump();
}

///////////////////////////////////////////////

Key::Key(const GroveLib::Element* elem,
         const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p)
{
    attr("name",   false);
    attr("match", false);
    attr("use",   false);
}

void Key::dump() const
{
    DDBG << "Key:" << std::endl;
    Instruction::dump();
}

////////////////////////////////////////////////

LiteralResult::LiteralResult(const GroveLib::Element* elem,
                             const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      UseAttributeSet(elem, this)
{
    const XsltFunctionFactory* xff =
        styleContext()->topStylesheet()->functionFactory();
    GroveLib::Attr* a = elem->attrs().firstChild();
    for (; a; a = a->nextSibling()) {
        if (a->xmlNsPrefix() != "xsl")
            attributes_.push_back(new AttributeValueTemplate(a, xff));
    }
    buildSubInstructions();
}

void LiteralResult::copyPrefixesTo(const GroveLib::Element* from,
                                   GroveLib::Element* elem)
{
    const GroveLib::XmlNsMapItem* nsitem = from->nsMapList().firstChild();
    for (; nsitem; nsitem = nsitem->nextSibling())
        if (nsitem->prefix() != "xsl")
            elem->addToPrefixMap(nsitem->prefix(), nsitem->uri());
}

LiteralResult::~LiteralResult()
{
    while (!attributes_.empty()) {
        delete attributes_.back();
        attributes_.pop_back();
    }
}

bool LiteralResult::allowsChild(Type type) const
{
    switch (type) {
        case IMPORT:
        case INCLUDE:
        case ATTRIBUTE_SET:
        case DECIMAL_FORMAT:
        case KEY:
        case NAMESPACE_ALIAS:
        case OUTPUT:
        case PRESERVE_SPACE:
        case STRIP_SPACE:
        case SORT:
        case TEMPLATE:
            return false;

        default :
            return true;
    }
    return true;
}

const String& LiteralResult::elementName() const
{
    return element()->nodeName();
}

void LiteralResult::dump() const
{
    DDBG << "LiteralResultElement: <" << name(element()) << '>' << std::endl;
    AttrValueIterator i = attributes_.begin();
    for (; i != attributes_.end(); i++) {
        DDBG << "Attr " << (*i)->name() << std::endl;
        (*i)->valueExpr()->dump();
    }
    Instruction::dump();
}

/////////////////////////////////////////////////

LiteralResultText::LiteralResultText(const GroveLib::Text* text,
                                     const Stylesheet* context, Instruction* p)
    : Instruction(0, context, p),
      node_(text)
{
}

void LiteralResultText::dump() const
{
    DDBG << "LiteralResultText: " << text() << std::endl;
    Instruction::dump();
}

/////////////////////////////////////////////////////

NamespaceAlias::NamespaceAlias(const GroveLib::Element* elem,
                               const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p)
{
    (void) attr("stylesheet-prefix", false);
    (void) attr("result-prefix", false);
}

void NamespaceAlias::dump() const
{
    DDBG << "NamespaceAlias:" << std::endl;
    Instruction::dump();
}

/////////////////////////////////////////////////////

Output::Output(const GroveLib::Element* elem,
               const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p)
{
    (void) attr("method", false);
    (void) attr("indent", false);
    (void) attr("encoding", false);
    (void) attr("version", false);
    (void) attr("omit-xml-declaration", false);
    (void) attr("standalone", false);
    (void) attr("doctype-public", false);
    (void) attr("doctype-system", false);
    (void) attr("cdata-section-elements", false);
    (void) attr("media-type", false);
}

void Output::dump() const
{
    DDBG << "Output:" << std::endl;
    Instruction::dump();
}

////////////////////////////////////////////////


PreserveSpace::PreserveSpace(const GroveLib::Element* elem,
                             const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      elements_(attr("elements", true))
{
}

void PreserveSpace::dump() const
{
    DDBG << "PreserveSpace:" << std::endl;
    Instruction::dump();
}

/////////////////////////////////////////////////

Sort::Sort(const GroveLib::Element* elem,
           const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      select_(attr("select", false)),
      order_(("descending" == attr("order", false)) ? DESCENDING : ASCENDING)
{
    attr("lang", false);
    attr("data-type", false);
    attr("case-order", false);
    if (select_.isEmpty())
        selectExpr_ = makeExpr("string(.)");
    else {
        String str;
        str = "string(" + select_ + ')';
        try {
            selectExpr_ = makeExpr(str);
        }
        catch (Xpath::Exception& e) {
            throw Xslt::Exception(XsltMessages::instrXpath, e.what(),
                              contextString());
        }
    }
    buildSubInstructions();
}

void Sort::dump() const
{
    DDBG << "Sort: select=" << select_ << std::endl;
    DDBG << "Select expr:" << std::endl;
    selectExpr_->dump();
    Instruction::dump();
}

///////////////////////////////////////////////

StripSpace::StripSpace(const GroveLib::Element* elem,
                       const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      elements_(attr("elements", true))
{
}

void StripSpace::dump() const
{
    DDBG << "StripSpace:" << std::endl;
    Instruction::dump();
}

/////////////////////////////////////////////////

Text::Text(const GroveLib::Element* elem,
           const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p)
{
    attr("disable-output-escaping", false);
    const GroveLib::Node* child = elem->firstChild();
    while (child) {
        if (GroveLib::Node::TEXT_NODE == child->nodeType())
            text_ += static_cast<const GroveLib::Text*>(child)->data();
        child = child->nextSibling();
    }
}

void Text::dump() const
{
    DDBG << "Text: <" << text_ << '>' << std::endl;
    Instruction::dump();
}

////////////////////////////////////////////////

Unknown::Unknown(const GroveLib::Element* elem,
                 const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p)
{
    buildSubInstructions();
}

void Unknown::dump() const
{
    DDBG << "Unknown: <" << name(element()) << '>' << std::endl;
    Instruction::dump();
}

//////////////////////////////////////////////////

ValueOf::ValueOf(const GroveLib::Element* elem,
                 const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      select_(attr("select", true))
{
    attr("disable-output-escaping", false);
    String select = "string(" + select_ + ')';
    try {
        selectExpr_ = makeExpr(select);
    }
    catch (Xpath::Exception& e) {
        throw Xslt::Exception(XsltMessages::instrXpath, e.what(),
                              contextString());
    }
}

void ValueOf::dump() const
{
    DDBG << "ValueOf: select=" << select_ << std::endl;
    Instruction::dump();
}

///////////////////////////////////////////////////

Message::Message(const GroveLib::Element* elem,
                 const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      messageExpr_(makeExpr("string(.)"))
{
    String term = attr("terminate", false);
    if (term.lower() == "yes")
        terminate_ = true;
    else
        terminate_ = false;
    buildSubInstructions();
}

bool Message::allowsChild(Type type) const
{
    switch (type) {
        case IMPORT:
        case INCLUDE:
        case ATTRIBUTE_SET:
        case DECIMAL_FORMAT:
        case KEY:
        case NAMESPACE_ALIAS:
        case OUTPUT:
        case PRESERVE_SPACE:
        case STRIP_SPACE:
        case SORT:
        case TEMPLATE:
        case WITH_PARAM:
            return false;

        default:
            return true;
    }
}

void Message::dump() const
{
    DDBG << "Message: " << this << std::endl;
    Instruction::dump();
}

///////////////////////////////////////////////

Variable::Variable(const GroveLib::Element* elem,
                   const Stylesheet* context, Instruction* p)
    : Instruction(elem, context, p),
      name_(attr("name", true, true))
{
    const String select(attr("select", false));
    if (!select.isEmpty()) {
        try {
            varExpr_ = makeExpr(select);
        }
        catch (Xpath::Exception& e) {
            throw Xslt::Exception(XsltMessages::instrXpath, e.what(),
                              contextString());
        }
    }
    buildSubInstructions();
}

void Variable::setConstString(const String& s) const
{
    const_cast<Variable*>(this)->varExpr_ = makeExpr("'" + s + "'");
}

bool Variable::allowsChild(Type type) const
{
    if (!varExpr_.isNull())
        throw Xslt::Exception(XsltMessages::instrVar, contextString());
    switch (type) {
        case IMPORT:
        case INCLUDE:
        case ATTRIBUTE_SET:
        case DECIMAL_FORMAT:
        case KEY:
        case NAMESPACE_ALIAS:
        case OUTPUT:
        case PRESERVE_SPACE:
        case STRIP_SPACE:
        case SORT:
        case TEMPLATE:
        case WITH_PARAM:
            return false;

        default :
            return varExpr_.isNull();
    }
    return varExpr_.isNull();
}

void Variable::dump() const
{
    DDBG << "Variable: name=" << name_ << std::endl;
    if (!varExpr_.isNull()) {
        DDBG << "Select expr:" << std::endl;
        varExpr_->dump();
    }
    Instruction::dump();
}

//////////////////////////////////////////////////

WithParam::WithParam(const GroveLib::Element* elem,
                     const Stylesheet* context, Instruction* p)
    : Variable(elem, context, p)
{
    const Instruction* i = prevSibling();
    for (; i; i = i->prevSibling()) {
        if (i->type() != WITH_PARAM)
            continue;
        if (static_cast<const WithParam*>(i)->name() == name())
            throw Xslt::Exception(XsltMessages::instrParam, contextString());
    }
}

void WithParam::dump() const
{
    DDBG << "WithParam:" << std::endl;
    Instruction::dump();
}

///////////////////////////////////////////////////

Param::Param(const GroveLib::Element* elem,
             const Stylesheet* context, Instruction* p)
    : Variable(elem, context, p)
{
}

void Param::dump() const
{
    DDBG << "Param-";
    Variable::dump();
}

TopParam::TopParam(const GroveLib::Element* elem,
             const Stylesheet* context, Instruction* p)
    : Param(elem, context, p),
      valueType_(attr("type", false)),
      annotation_(attr("annotation", false))
{
}

} // namespace Xslt
