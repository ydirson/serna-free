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
#include "xs/XsDataImpl.h"
#include "xs/XsMessages.h"
#include "xs/XsNodeExt.h"
#include "xs/complex/ElementParticle.h"
#include "xs/complex/GeneratedParticle.h"
#include "xs/complex/WildcardParticle.h"
#include "xs/complex/ChoiceParticle.h"
#include "xs/complex/XsComplexTypeImpl.h"
#include "xs/complex/XsContent.h"
#include "xs/complex/ComplexContent.h"
#include "xs/complex/FixupSet.h"
#include "xs/complex/ContentToken.h"
#include "xs/complex/FsmMatcher.h"
#include "xs/components/XsElementImpl.h"
#include "grove/Nodes.h"
#include <iostream>
#include <limits.h>

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

const ulong Particle::unbounded = ~0UL;     // unbounded value for maxOccur

void Particle::setMinOccur(Schema*, ulong n)
{
    minOccur_ = n;
    if (minOccur_ >  maxOccur_)
        maxOccur_ = minOccur_;
}

void Particle::setMaxOccur(Schema*, ulong n)
{
    if (~0UL == n)
        n = unbounded;
    maxOccur_ = n;
    if (minOccur_ >  maxOccur_)
        minOccur_ = maxOccur_;
}

////////////////////////////////////////////////////////////////////
GeneratedParticle::GeneratedParticle(ElementParticle* ep,
                                     const FsmMatcher::DfaState* s)
    : Particle(ep->origin()),ep_(ep), state_(s)
{
}

void GeneratedParticle::makeElement(Schema* s,
                                  GROVE_NAMESPACE::ElementPtr& ep,
                                  GROVE_NAMESPACE::Element* pe) const
{
    ep_->makeElement(s, ep, pe);
    if (ep.isNull())
        return;
    XsNodeExt::set(ep.pointer(), state_);
}

bool GeneratedParticle::makeSkeleton(Schema* s,
                                   GROVE_NAMESPACE::Node* attachTo,
                                   GROVE_NAMESPACE::Element* pe,
                                   FixupSet* elemSet,
                                   ulong occurs) const
{
    return ep_->makeSkeleton(s,attachTo, pe, elemSet, occurs);
}

int GeneratedParticle::checkSplit(const String&, XsElement*&) const
{
    RT_MSG_ABORT("GeneratedParticle::check_split?");
    return 0;
}

ElementParticle*   GeneratedParticle::elementParticle()
{
    return ep_.pointer();
}

void GeneratedParticle::dump(int indent) const
{
    (void) indent;
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++)
         std::cerr << ' ';
    std::cerr << NOTR("GEN:");
    ep_->dump(indent);
#endif // XS_DEBUG
}

GeneratedParticle::~GeneratedParticle()
{
}

////////////////////////////////////////////////////////////////////

ElementParticle::ElementParticle(const Origin& o, const XsId& xsid)
    : Particle(o, xsid)
{
}

void ElementParticle::setElement(const XsElementInst& elem)
{
    elem_ = elem;
}

XsElement* ElementParticle::element() const
{
    return elem_.pointer();
}

bool ElementParticle::cdataAllowed() const
{
    return element()->cdataAllowed();
}

Common::PropertyNode* ElementParticle::makeElemProperty() const
{
    const ExpandedName& ename = element()->constCred()->expandedName();
    PropertyNode* pn = new PropertyNode(ename.localName(), ename.uri());
    if (element()->cdataAllowed())
        pn->appendChild(new PropertyNode("mixed"));
    return pn;
}

int ElementParticle::checkSplit(const String& tokName, XsElement*& e) const
{
    if (elem_->constCred()->name() != tokName)
        return 0;
    e = element();
    if (maxOccur() > 1)
        return 1;
    return 0;
}

RefCntPtr<QnameToken> ElementParticle::token() const
{
    return new QnameToken(element()->constCred()->expandedName(),
        const_cast<ElementParticle*>(this));
}

void ElementParticle::makeElement(Schema* s,
                                  GROVE_NAMESPACE::ElementPtr& ep,
                                  GROVE_NAMESPACE::Element* pe) const
{
    element()->makeInstance(s, pe->grove(), ep, pe);
}

bool ElementParticle::makeSkeleton(Schema* s,
                                   GROVE_NAMESPACE::Node* attachTo,
                                   GROVE_NAMESPACE::Element* pe,
                                   FixupSet* elemSet,
                                   ulong occurs) const
{
    if (elemSet->checkLoop(this))
        return true;
    elemSet->insert(this);
    bool ok = true;
    if (0 == occurs)
        occurs = minOccur();
    for (ulong i = 0; i < occurs; ++i) {
        GROVE_NAMESPACE::ElementPtr ep;
        element()->makeInstance(s, pe->grove(), ep, pe);
        if (ep.isNull())
            return false;
        attachTo->appendChild(ep.pointer());
        Node* parent = ep->parent();
        if (pe) //for namespace resolving
            ep->setParent(pe);
        ok = element()->makeContent(s, ep.pointer(), elemSet) && ok;
        ep->setParent(static_cast<GroveLib::Element*>(parent));
        if (s->validationFlags() & Schema::editMode)
            XsNodeExt::set(ep.pointer(), element());
        if (s)
            s->mstream() << XsMessages::elementGenerated
                         << Message::L_INFO
                         << ep->xmlNsUri()
                         << ep->localName()
                         << DV_ORIGIN(ep.pointer());
    }
    return ok;
}

void ElementParticle::dump(int indent) const
{
    (void) indent;
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++)
         std::cerr << ' ';
    std::cerr << NOTR("EP:");
    element()->constCred()->dump();
    std::cerr << '/' << minOccur() << '/' << maxOccur();
    std::cerr << std::endl;
#endif // XS_DEBUG
}

ElementParticle::~ElementParticle()
{
}

///////////////////////////////////////////////////////////////

int WildcardParticle::checkSplit(const String&, XsElement*&) const
{
    return 0;
}

bool WildcardParticle::makeSkeleton(Schema*,
                                    GROVE_NAMESPACE::Node*,
                                    GROVE_NAMESPACE::Element*,
                                    FixupSet*,
                                    ulong) const
{
    // do nothing for wildcards
    return true;
}

void WildcardParticle::dump(int) const
{
    if (!wildcard())
        std::cerr << NOTR("<NO-WILDCARD>");
    else
        std::cerr << NOTR("Wildcard(") << minOccur() << ","
            << maxOccur() << ")<" << wildcard()->format() << NOTR(">");
}

//////////////////////////////////////////////////////////////

int ChoiceParticle::checkSplit(const String&, XsElement*&) const
{
    return -1;
}

bool ChoiceParticle::makeSkeleton(Schema*,
                                  GROVE_NAMESPACE::Node*,
                                  GROVE_NAMESPACE::Element*,
                                  FixupSet*,
                                  ulong) const
{
    return false;
}

void ChoiceParticle::makeElement(Schema*,
                                  GROVE_NAMESPACE::ElementPtr& ep,
                                  GROVE_NAMESPACE::Element* /* pe */) const
{
    ep = new ChoiceNode;
    // const_cast is for RefCntPtr
    XsNodeExt::set(ep.pointer(), const_cast<ChoiceParticle*>(this));
}

void ChoiceParticle::dump(int) const
{
#ifdef XS_DEBUG
    std::cerr << NOTR("CHOICE: ");
    elist_.root()->dump();
#endif // XS_DEBUG
}

void ChoiceParticle::addElem(PropertyNode* elemProp)
{
    if (elemProp)
        elist_.root()->appendChild(elemProp);
}

ChoiceParticle::~ChoiceParticle()
{
}

//////////////////////////////////////////////////////////////

PRTTI_BASE_STUB(Particle, WildcardParticle)
PRTTI_BASE_STUB(Particle, ElementParticle)
PRTTI_BASE_STUB(Particle, GeneratedParticle)
PRTTI_BASE_STUB(Particle, Connector)
PRTTI_BASE_STUB(Particle, ChoiceParticle)

PRTTI_IMPL(ElementParticle)
PRTTI_IMPL(GeneratedParticle)
PRTTI_IMPL(WildcardParticle)
PRTTI_IMPL(ChoiceParticle)

XS_NAMESPACE_END
