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
#include "xslt/impl/Instance.h"
#include "xslt/impl/Instruction.h"
#include "xslt/impl/FunctionRegistry.h"
#include "xslt/impl/instructions/Stylesheet.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/instances/TopStylesheetInst.h"
#include "xslt/impl/instances/VariableInst.h"
#include "xslt/XsltMessages.h"
#include "xslt/impl/xpath_exprinst.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

namespace Xslt {

Instance::~Instance()
{
    // deregister instance from ModificationRegistry
    if (isModified_)
        topStylesheetInst()->modRegistry().deregisterInstance(this);
    // prohibit myself from being killed twice in case of exception
    if (1 == getRefCnt()) {
        incRefCnt();
        InstanceTreeBase::remove();
    }
}

const GroveLib::Element* Instance::element() const
{
    if (instruction_)
        return instruction_->element();
    return 0;
}

const Stylesheet& Instance::styleContext() const
{
    return *instruction_->styleContext();
}

void Instance::setModified(bool isModified)
{
    if (isModified) {
        if (!isModified_) {
            isModified_ = true;
            topStylesheetInst()->modRegistry().registerInstance(this);
        }
    }
    else
        isModified_ = false;
}

// by default, just propagate down
void Instance::updateContext(int contextUpdateType)
{
    updateChildContext(contextUpdateType);
}

void Instance::updateChildContext(int contextUpdateType)
{
    for (Instance* inst = firstChild(); inst; inst = inst->nextSibling()) {
        if (inst->context_dep_ == UNDEF_DEP) {
            inst->context_dep_ = 0;
            inst->updateContext(contextUpdateType);
            context_dep_ |= inst->context_dep_;
        } else if (inst->context_dep_ & contextUpdateType) 
            inst->updateContext(contextUpdateType);
    }
}

bool Instance::updateExpr(Xpath::ValueHolderPtr& vp)
{
    const Xpath::ExprInst* ei = vp->exprInst();
    if (0 == ei)
        return false;
    int dep = ei->expr().contextDependency();
    if (!dep)
        return false;
    context_dep_ |= dep;
    COMMON_NS::RefCntPtr<Xpath::ValueHolder> vp2 =
        ei->expr().makeInst(*nsi_, ei->exprContext());
    if (*vp2->value() != *vp->value()) {
        DBG(XSLT.UPDATE) << "updateExpr: expr reevaluated" << std::endl;
        DBG_EXEC(XSLT.UPDATE, vp2->value()->dump());
        vp = vp2.pointer();
        setModified();
        return true;
    }
    DBG(XSLT.UPDATE) << "updateExpr: expr unchanged" << std::endl;
    return false;
}

void Instance::buildSubInstances(const ResultContext& resultContext,
                                 const Xpath::NodeSetItem* ctx)
{
    if (0 == ctx)
        ctx = nsi_;
    const Instruction* i = instruction_->firstChild();
    for (; i; i = i->nextSibling()) {
        try {
            i->makeInst(InstanceInit(*ctx, resultContext), this);
        }
        catch (std::exception& e) {
            throw;
        }
    }
}

///////////////////////////////////////////////////////////////////////

const InstanceResult* Instance::firstResult() const
{
    const InstanceResult* result = 0;
    for (const Instance* i = firstChild(); i; i = i->nextSibling()) {
        if (i->skipResultContext_)
            continue;
        result = i->firstResult();
        if (result)
            return result;
    }
    return 0;
}

const InstanceResult* Instance::lastResult() const
{
    const InstanceResult* result = 0;
    for (const Instance* i = lastChild(); i; i = i->prevSibling()) {
        if (i->skipResultContext_)
            continue;
        result = i->lastResult();
        if (result)
            return result;
    }
    return 0;
}

void Instance::getResult(ResultContext& rc) const
{
    const InstanceResult* result = 0;
    const Instance* i;
    for (i = nextSibling(); i; i = i->nextSibling()) {
        if (i->skipResultContext_)
            continue;
        result = i->firstResult();
        if (result) {
            rc = ResultContext(result->parent(), result->node());
            return;
        }
    }
    for (i = prevSibling(); i; i = i->prevSibling()) {
        if (i->skipResultContext_)
            continue;
        result = i->lastResult();
        if (result) {
            rc = ResultContext(result->parent(), result->node()->nextSibling());
            return;
        }
    }
    const Instance* pi = parent();
    if (0 == pi)
        return;
    result = pi->selfResult();
    if (pi->skipResultContext_) {
        if (result &&
            result->node()->nodeType() != GroveLib::Node::ATTRIBUTE_NODE)
               rc = ResultContext(result->node(), 0);
        else
            rc = ResultContext(pi->rtfRoot(), 0);
        return;
    }
    if (result) {
        rc = ResultContext(result->node(), 0);
        return;
    }
    pi->getResult(rc);
}

ResultContext Instance::resultContext() const
{
    const InstanceResult* lastRes = lastResult();
    if (lastRes)
        return ResultContext(lastRes->parent(), lastRes->node()->nextSibling());
    if (skipResultContext_)
        return ResultContext(rtfRoot(), 0);
    ResultContext rc;
    getResult(rc);
    if (rc.parent()) {
        DBG_EXEC(XSLT.RESULT, rc.dump());
        return rc;
    }
    return ResultContext(topStylesheetInst()->resultGrove()->document(), 0);
}

void Instance::disposeResult()
{
    for (Instance* i = firstChild(); i; i = i->nextSibling())
        i->disposeResult();
}

////////////////////////////////////////////////////////////////////////

// algorithm: look to the left side of the tree, within the same template
// if found param, also check withParam (first children of ApplicatingInst)
Xpath::ValueHolder*
Instance::getVariable(const COMMON_NS::String& name) const
{
    DBG(XSLT.VARS) << "Instance(" << this << ")::getVar: " << name << std::endl;
    const Instance* p = this;
    const Instance* i = this;
    Instruction::Type t;

    for(;;) {
        if (0 == i) {
            p = p->parent();
            if (0 == p || Instruction::TEMPLATE == p->instruction_->type())
                return topStylesheetInst()->getTopVar(name);
            i = p;
            continue;
        }
        t = i->instruction_->type();
        if (Instruction::VARIABLE != t && Instruction::PARAM != t) {
            i = i->prevSibling();
            continue;
        }
        const VariableInstance* vi = static_cast<const VariableInstance*>(i);
        if (vi->name() != name) {
            i = i->prevSibling();
            continue;
        }
        if (t == Instruction::VARIABLE)
            return vi->getVariable();
        // for PARAM, we need also check WithParam's
        if (p->mayHaveWithParams())
            p = p->parent();
        for (; p && !p->mayHaveWithParams(); p = p->parent())
            ;
        if (0 == p)
            return topStylesheetInst()->getVariable(name);
        for (i = p->firstChild(); i; i = i->nextSibling()) {
            t = i->instruction_->type();
            if (Instruction::SORT == t)
                continue;
            if (Instruction::WITH_PARAM != t)
                break;
            const VariableInstance* wp =
                static_cast<const VariableInstance*>(i);
            if (wp->name() == name)
                return wp->getVariable();
        }
        return vi->getVariable();
    }
}

void Instance::set_root_top()
{
    topInst_ = static_cast<TopStylesheetInst*>(this);
}

TemplateInstanceBase* Instance::asTemplateInstBase()
{
    return 0;
}

const ApplicatingInstance* Instance::asConstApplicatingInstance() const
{
        return 0;
}

bool Instance::doesLoop(const Instruction* instruction,
                               const Xpath::NodeSetItem& context,
                               String&) const
{
    if (instruction == instruction_ &&
        context.node() == nsiContext().node())
            return true;
    return false;
}

void Instance::getTreeloc(COMMON_NS::TreelocRep& tl, bool append) const
{
    if (!append)
        tl.resize(0);
    for (const Instance* i = this; i->parent(); i = i->parent())
        tl += i->siblingIndex_;
}

const Instance* Instance::currentTemplate() const
{
    const Instance* i = this;
    while (i && !i->asConstTemplateInst())
        i = i->parent();
    return i;
}

COMMON_NS::String Instance::contextString() const
{
    const Instruction* instr = instruction();
    if (instr)
        return instr->contextString();
    return String::null();
}
    
const Xpath::ExternalFunctionBase* Instance::getExternalFunction
        (const Common::String& name, const Common::String& uri) const
{
    return topStylesheetInst()->functionRegistry().getExternalFunc(name, uri);
}

void Instance::dump() const
{
    uint nc = 0;
    DDBG << "inst(" << (instruction()->element() ?
        instruction()->element()->nodeName() : String(""))
        << "):" << this << " P:" << parent()
        << " PS:" << prevSibling() << " NS:" << nextSibling()
        << " R" << getRefCnt() << " S#:" << siblingIndex_
        << " ctx "<< nsiContext().node()
        << "/<" << (((intptr_t)nsiContext().node() > 0x1000000) ?
            nsiContext().node()->nodeName() : String("*BAD*"))
        << ">" << std::endl;
    for (const Instance* i = firstChild(); i; i = i->nextSibling()) {
        DDBG << "child #" << nc << ":" << std::endl;
        ++nc;
        DDINDENT;
        i->dump();
    }
}

void ResultContext::dump() const
{
    DDBG << "ResultContext: parent " << parent_ << "(" << parent_->nodeName()
        << ")";
    if (node_)
        DDBG << " before " << node_ << "(" << node_->nodeName() << ")";
    DDBG << std::endl;
    DBG_EXEC(XSLT.RESULT, parent_->dump());
}

} // namespace Xslt
