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
#include "xpath/Engine.h"
#include "xpath/NodeSet.h"
#include "xpath/Expr.h"
#include "xpath/ValueHolder.h"

#include "xslt/Result.h"
#include "xslt/impl/ResultTreeFragment.h"
#include "xslt/impl/AttributeValueTemplate.h"
#include "xslt/impl/instances/AttributeInst.h"
#include "xslt/impl/debug.h"

namespace Xslt {

AttributeInstBase::AttributeInstBase(const AttributeCommon* attr,
                                     const InstanceInit& init, Instance* p)
    : ResultMakingInstance(attr, init, p)
{
    if (GroveLib::Node::ELEMENT_NODE != result_.parent()->nodeType())
        throw Xslt::Exception(XsltMessages::instanceAttr, contextString());
}

AttributeResult* AttributeInstBase::resultAttr() const
{
    return static_cast<AttributeResult*>(result_.node());
}

void AttributeInstBase::setAttribute()
{
    COMMON_NS::String qname(qName());
    DBG(XSLT.UPDATE) << "AttributeInst/SetAttribute: " << qname << std::endl;
    if (qname.isEmpty())
        return restoreOldAttr();
    if (result_.node() && qname != resultAttr()->nodeName()) {
        if (oldAttr_ && qname != oldAttr_->nodeName())
            restoreOldAttr();
        else {
            saveOldAttr();
            result_.dispose();
        }
    }
    AttributeResult* ares = resultAttr();
    if (0 == ares) {
        ares = new AttributeResult(qname, value(), 0, this);
        saveOldAttr();
        result_.insertBefore(ares, 0);
    }
    if (value() != ares->value())   // value changed
        ares->setValue(value());
}

void AttributeInstBase::restoreOldAttr()
{
    result_.dispose();
    if (!oldAttr_.isNull())
        static_cast<GroveLib::Element*>(result_.parent())->attrs().
            appendChild(oldAttr_.pointer());
}

void AttributeInstBase::saveOldAttr()
{
    GroveLib::Element* pe = static_cast<GroveLib::Element*>(result_.parent());
    GroveLib::Attr* a = pe->attrs().getAttribute(qName());
    if (0 == a)
        return;
    // check if we created this attribute ourselves
    if (a == result_.node())
        return;
    if (oldAttr_.isNull())
        oldAttr_ = a;
}

AttributeInstBase::~AttributeInstBase()
{
    restoreOldAttr();
}

////////////////////////////////////////////

AvtNameAttributeInstance::AvtNameAttributeInstance
  (const AttributeAvtNameProvider* aanp, Instance* self)
    : nameInst_(0, aanp->name()->valueExpr()->
        makeInst(self->nsiContext(), self->exprContext())),
      namespaceInst_(0, 0)
{
    if (0 == aanp->ns())
        return;
    namespaceInst_ = aanp->ns()->valueExpr()->
        makeInst(self->nsiContext(), self->exprContext());
}

void AvtNameAttributeInstance::setWatchers(COMMON_NS::SubscriberPtrWatcher* w)
{
    nameInst_.setWatcher(w);
    namespaceInst_.setWatcher(w);
}

const COMMON_NS::String AvtNameAttributeInstance::qName() const
{
    if (namespaceInst_.isNull())
        return nameInst_->value()->getString();
    return COMMON_NS::String(namespaceInst_->value()->getString()
        + ':' + nameInst_->value()->getString());
}

void AvtNameAttributeInstance::updateAvtContext(Instance* i)
{
    i->updateExpr(nameInst_);
    if (!namespaceInst_.isNull())
        i->updateExpr(namespaceInst_);
}

const COMMON_NS::String SimpleNameAttributeInstance::qName() const
{
    return nameProvider_->nameString();
}

//////////////////////////////////////////////

const COMMON_NS::String& ConstValueAttributeInstance::value() const
{
    return valueProvider_->value();
}

ExprValueAttributeInstance::ExprValueAttributeInstance
  (const AttributeExprValueProvider* aevp, Instance* self)
    : valueInst_(0,
        aevp->valueExpr()->makeInst(self->nsiContext(), self->exprContext()))
{
}

const COMMON_NS::String& ExprValueAttributeInstance::value() const
{
    return valueInst_->value()->getString();
}

void ExprValueAttributeInstance::setWatchers(COMMON_NS::SubscriberPtrWatcher* w)
{
    valueInst_.setWatcher(w);
}

void ExprValueAttributeInstance::updateValueContext(Instance* i)
{
    if (!valueInst_.isNull())
        i->updateExpr(valueInst_);
}

RtfValueAttributeInstance::RtfValueAttributeInstance
  (const AttributeRtfValueProvider* arvp, Instance* self)
    : rtf_(0, new ResultTreeFragment), valueInst_(0, 0)
{
    nsi_.setNode(rtf_root());
    self->setSkipResultContext(true);
    self->buildSubInstances(ResultContext(nsi_.node(), 0));
    valueInst_ = arvp->valueExpr()->makeInst(nsi_, self->exprContext());
}

void RtfValueAttributeInstance::setWatchers(COMMON_NS::SubscriberPtrWatcher* w)
{
    valueInst_.setWatcher(w);
    rtf_.setWatcher(w);
}

GroveLib::Node* RtfValueAttributeInstance::rtf_root() const
{
    return static_cast<const ResultTreeFragment*>(rtf_.pointer())->rtfRoot();
}

const COMMON_NS::String& RtfValueAttributeInstance::value() const
{
    return valueInst_->value()->getString();
}

///////////////////////////////////////////////

SimpleConstAttributeInst::SimpleConstAttributeInst
  (const SimpleConstAttribute* attr, const InstanceInit& init, Instance* p)
    : AttributeInstBase(attr, init, p),
      SimpleNameAttributeInstance(attr),
      ConstValueAttributeInstance(attr)
{
    setAttribute();
}

/////

AvtConstAttributeInst::AvtConstAttributeInst
  (const AvtConstAttribute* attr, const InstanceInit& init, Instance* p)
    : AttributeInstBase(attr, init, p),
      AvtNameAttributeInstance(attr, this),
      ConstValueAttributeInstance(attr)
{
    setAttribute();
    AvtNameAttributeInstance::setWatchers(this);
}

void AvtConstAttributeInst::updateContext(int utype)
{
    updateAvtContext(this);
    updateChildContext(utype);
}

////

SimpleExprAttributeInst::SimpleExprAttributeInst
  (const SimpleExprAttribute* attr, const InstanceInit& init, Instance* p)
    : AttributeInstBase(attr, init, p),
      SimpleNameAttributeInstance(attr),
      ExprValueAttributeInstance(attr, this)
{
    setAttribute();
    ExprValueAttributeInstance::setWatchers(this);
}

void SimpleExprAttributeInst::updateContext(int utype)
{
    updateValueContext(this);
    updateChildContext(utype);
}

////

AvtExprAttributeInst::AvtExprAttributeInst
  (const AvtExprAttribute* attr, const InstanceInit& init, Instance* p)
    : AttributeInstBase(attr, init, p),
      AvtNameAttributeInstance(attr, this),
      ExprValueAttributeInstance(attr, this)
{
    setAttribute();
    AvtNameAttributeInstance::setWatchers(this);
    ExprValueAttributeInstance::setWatchers(this);
}

void AvtExprAttributeInst::updateContext(int)
{
    updateValueContext(this);
}

////

SimpleRtfAttributeInst::SimpleRtfAttributeInst
  (const SimpleRtfAttribute* attr, const InstanceInit& init, Instance* p)
    : AttributeInstBase(attr, init, p),
      SimpleNameAttributeInstance(attr),
      RtfValueAttributeInstance(attr, this)
{
    setAttribute();
    RtfValueAttributeInstance::setWatchers(this);
}

////

AvtRtfAttributeInst::AvtRtfAttributeInst
  (const AvtRtfAttribute* attr, const InstanceInit& init, Instance* p)
    : AttributeInstBase(attr, init, p),
      AvtNameAttributeInstance(attr, this),
      RtfValueAttributeInstance(attr, this)
{
    setAttribute();
    AvtNameAttributeInstance::setWatchers(this);
    RtfValueAttributeInstance::setWatchers(this);
}

void AvtRtfAttributeInst::updateContext(int utype)
{
    updateAvtContext(this);
    updateChildContext(utype);
}

#define MKINST(Name) template<> \
    Instance* Name::makeInst(const InstanceInit& init, Instance* p) const \
{ \
    return new Name##Inst(this, init, p); \
}

MKINST(SimpleConstAttribute)
MKINST(SimpleExprAttribute)
MKINST(SimpleRtfAttribute)
MKINST(AvtConstAttribute)
MKINST(AvtExprAttribute)
MKINST(AvtRtfAttribute)

Instance* Attribute::makeInst(const InstanceInit&, Instance*) const
{
    throw Xslt::Exception(XsltMessages::instrAttrInst, contextString());
}

} // namespace Xslt
