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
#include <iostream>
#include <stdlib.h>

#include "xs/xs_defs.h"
#include "xs/Component.h"
#include "xs/XsType.h"
#include "xs/XsElement.h"
#include "xs/Schema.h"
#include "xs/XsData.h"
#include "xs/XsMessages.h"
#include "xs/XsNodeExt.h"
#include "xs/SchemaNamespaces.h"
#include "xs/XsDataImpl.h"
#include "xs/complex/XsComplexTypeImpl.h"
#include "xs/complex/ComplexContent.h"
#include "xs/complex/Connectors.h"
#include "xs/complex/InputTokenSequence.h"
#include "xs/complex/ElementParticle.h"
#include "xs/complex/GeneratedParticle.h"
#include "xs/complex/WildcardParticle.h"
#include "xs/complex/FixupSet.h"
#include "xs/complex/Matcher.h"
#include "xs/complex/ChoiceParticle.h"
#include "xs/components/AttributeSet.h"
#include "grove/Nodes.h"
#include "grove/ValidationCommandsMaker.h"
#include "grove/udata.h"

#ifdef __SUNPRO_CC
# include "XsDataImpl.h"
#endif

#include "common/asserts.h"

using namespace Common;
using namespace GroveLib;

XS_NAMESPACE_BEGIN

ComplexContent::ComplexContent(DerivationType type,
                               const Origin& o,
                               const XsId& xsid)
    : XsContent(type, o, xsid),
      connector_(0), baseProcessed_(false),
      mixed_(false), baseIsAny_(false)
{
}

bool ComplexContent::cdataAllowed() const
{
    if (mixed_)
        return true;
    if (baseType_.isNull())
        return false;
    XsComplexTypeImpl* ctype = baseType_->asXsComplexTypeImpl();
    return (ctype && ctype->content()) ? ctype->cdataAllowed() : false;
}

bool ComplexContent::checkDerivation(Schema* schema,
                                     Element* elem,
                                     bool mixed) const
{
    bool ok = true;
    if (XsContent::NONE != derivationType_ && !baseProcessed_) {
        const_cast<ComplexContent*>(this)->baseProcessed_ = true;
        if (baseType_.isNull()) {
            if (baseIsAny_)
                return true;
            if (schema)
                schema->mstream() << XsMessages::noBaseType
                                  << Message::L_ERROR
                                  << DV_ORIGIN(elem);
            return false;
        }
        XsComplexTypeImpl* ctype = baseType_->asXsComplexTypeImpl();
        if (0 == ctype) {
            if (schema)
                schema->mstream() << XsMessages::baseIsNotComplex
                                  << Message::L_ERROR
                                  << DV_ORIGIN(elem);
            return false;
        }
        bool mixed_parent_content = cdataAllowed();
        if (XsContent::EXTENSION == derivationType_ && mixed_parent_content)
            const_cast<ComplexContent*>(this)->mixed_ = true;
        if (!ctype->content()) {
            if (schema)
                schema->mstream() << XsMessages::baseHasNoContent
                                  << Message::L_ERROR
                                  << DV_ORIGIN(elem);
            return false;
        }
        ComplexContent* content = ctype->content()->asComplexContent();
        if (0 == content) {
            if (schema)
                schema->mstream() << XsMessages::baseHasSimpleCont
                                  << Message::L_ERROR
                                  << DV_ORIGIN(elem);
            return false;
        }
        if (!content->checkDerivation(schema, elem, mixed))
            return false;
        if (XsContent::EXTENSION == derivationType_ ||
            XsContent::RESTRICTION == derivationType_)
                attributeSet_->join(content->attributeSet_.pointer());
        if (!content->connector()) 
            return true;
        if (XsContent::EXTENSION == derivationType_) {
            if (XsComplexTypeImpl::EXTENSION == ctype->noDerivation()) {
                if (schema)
                    schema->mstream() << XsMessages::noDerivationExt
                                      << Message::L_ERROR
                                      << DV_ORIGIN(elem);
                return false;
            }
            if (connector_.isNull()) 
                const_cast<ComplexContent*>(this)->connector_ =
                    content->connector();
            else if (content->connector()->type() == Connector::ALL) {
                if (connector_->type() != Connector::ALL) {
                    if (schema)
                        schema->mstream() 
                            << XsMessages::mismatchedStructure
                            << Message::L_ERROR
                            << DV_ORIGIN(elem);
                    return false;
                }
                content->connector()->copyContent(connector_.pointer());
            } else {
                SequenceConnector* joined =
                    new SequenceConnector(Connector::SEQUENCE,
                                          origin());
                if (Connector::SEQUENCE == content->
                    connector()->type())
                    content->connector()->copyContent(joined);
                else
                    joined->addChild(content->connector());
                if (Connector::SEQUENCE == connector_->type())
                    connector_->copyContent(joined);
                else
                    joined->addChild(connector_.pointer());
                joined->setParent(connector_->parent());
                const_cast<ComplexContent*>(this)->
                    connector_ = joined;
            }
        } else if (mixed_parent_content || 
          mixed == mixed_parent_content) {
            if (XsContent::RESTRICTION == derivationType_) {
                if(XsComplexTypeImpl::RESTRICTION ==
                   ctype->noDerivation()) {
                    if (schema)
                        schema->mstream() << XsMessages::noDerivationRest
                                          << Message::L_ERROR
                                          << DV_ORIGIN(elem);
                    return false;
                }
                if (!connector_.isNull() &&
                    content->connector()->type() != connector_->type()) {
                    bool skip = false;
                    if (Connector::CHOICE == content->connector()->type() &&
                        Connector::SEQUENCE == connector_->type())
                        skip = true;
                    if (Connector::ALL == content->connector()->type() &&
                        Connector::SEQUENCE == connector_->type())
                        skip = true;
                    if (schema && !skip) {
                        schema->mstream() << XsMessages::diffContent
                                          << Message::L_ERROR
                                          << DV_ORIGIN(elem);
                        return false;
                    }
                }
                ok = true;
//TODO : resctrictions Schema Part1: 3.9.6 - Constraints on ParticleSchema Components
//                        ok = connector_->check(schema, DV_ORIGIN(elem),
//                                               content->connector());
//                        ok = attributeSet_->check(schema, DV_ORIGIN(elem),
//                                                  content->attributeSet_.pointer()) && ok;
            }
        } else {
            if (schema)
                schema->mstream() << XsMessages::diffMixed
                                  << Message::L_ERROR
                                  << DV_ORIGIN(elem);
            return false;
        }
    }
    return ok;
}

static void reuse_choice_node(ValidationCommandsMaker* cm,
                              Node* parent,
                              Element* elemp)
{
    ChoiceNode* lc = static_cast<ChoiceNode*>(parent->lastChild());
    const ChoiceNode* old_cn = static_cast<const ChoiceNode*>(elemp);
    // if elem-list is different, need to re-create choice
    const PropertyNode* pn1 = old_cn->elemList()->firstChild();
    const PropertyNode* pn2 = lc->elemList()->firstChild();
    for (; pn1 && pn2; ) {
        if (pn1->name() != pn2->name())
            break;
        pn1 = pn1->nextSibling();
        pn2 = pn2->nextSibling();
    }
    if (pn1 || pn2) {
        cm->insertNode(0, parent, elemp);
        return;
    }
    lc->CDListItem<ChoiceNode>::remove();
}

bool ComplexContent::validate(Schema* schema,
                              Element* elem,
                              bool mixed,
                              bool recursive) const
{
    bool ok = checkDerivation(schema, elem, mixed);

    InputTokenSequence iseq;
    MatchState ms;

    if (mixed)
        const_cast<ComplexContent*>(this)->mixed_ = true;

    if (schema->validationFlags() & Schema::fixupComplex) 
        ms.editMode = true;
    
    GroveLib::ValidationCommandsMaker* cm = schema->commandMaker();

    if (schema->validationFlags() & Schema::editMode)
        ms.editMode = true;

    ok = iseq.build(schema, elem,
                    mixed_ ? InputTokenSequence::cdataAllowed : 0, cm) && ok;

    Vector<Attr*> attrs;
    ulong i;

    for (GroveLib::Attr* a = elem->attrs().firstChild();
        a; a = a->nextSibling())
            attrs.push_back(a);

    ok = attributeSet_->validate(schema, elem, attrs) && ok;
    for (i = 0; i < attrs.size(); ++i) {
        if (schema) {
            if (schema->validationFlags() & Schema::rmExtraAttrs)
                attrs[i]->remove();
            else if (XSI_NAMESPACE != attrs[i]->expandedName().uri()) {
                schema->mstream() << XsMessages::attrExtra << elem->nodeName()
                    << attrs[i]->nodeName() << Message::L_ERROR
                    << DV_ORIGIN(attrs[0]);
                ok = false;
            }
        }
    }
    if (!connector_.isNull()) {
        connector_->makeSubst(schema);
        // build matcher, if necessary
        (void) matcher(schema, elem->nodeName());
        ok = matcher_->exactMatch(schema, iseq, ms) && ok;
/*!DEBUG:
*        connector_->dump(0);
*       static_cast<FsmMatcher*>(matcher_.pointer())->dfa_dumpAsDot("dfa.txt");
*        static_cast<FsmMatcher*>(matcher_.pointer())->nfa_dumpAsDot("nfa.txt");
*/
    }
    if (!recursive)
        return ok;
    WildcardParticle* wp;
    ElementParticle* ep;
    for (i = 0; i < iseq.size(); ++i) {
        if (!iseq[i].matchedParticle) {
            if (!baseIsAny_) {
                if (schema)
                    schema->mstream() << XsMessages::contentShouldBeEmpty
                              << elem->nodeName()
                              << Message::L_ERROR
                              << DV_ORIGIN(elem);

               ok = false;
            }
            continue;
        }
        if (!iseq[i].element) { // generated entry
            if (!iseq[i].matchedParticle)
                continue;
            Node* before = 0;
            ulong j = i;
            while (j < iseq.size() && !iseq[j].element)
                ++j;
            if (j < iseq.size())
                before = iseq[j].element;
            if (ms.editMode) {
                ElementPtr elemp;
                iseq[i].matchedParticle->makeElement(schema, elemp,
                    iseq.parent());
                if (!elemp)
                    continue;
                if ((schema->validationFlags() & Schema::dontMakeElems) &&
                    !iseq[i].matchedParticle->asConstChoiceParticle()) {
                        schema->mstream() << XsMessages::missingMandatoryElem
                            << Message::L_ERROR << elemp->nodeName()
                            << DV_ORIGIN(elemp.pointer());
                        continue;
                }
                if (cm) {
                    if (0 == before && iseq.parent()->lastChild() &&
                        elemp->nodeType() == Node::CHOICE_NODE &&
                        iseq.parent()->lastChild()->nodeType() ==
                        Node::CHOICE_NODE) 
                            reuse_choice_node(cm, iseq.parent(), 
                                elemp.pointer());
                     else
                         cm->insertNode(before, iseq.parent(), elemp.pointer());
                } else {
                    if (before)
                        before->insertBefore(elemp.pointer());
                    else
                        iseq.parent()->appendChild(elemp.pointer());
                }
                iseq[i].element = elemp.pointer();
                iseq[i].token = new QnameToken(elemp->expandedName(),
                    iseq[i].matchedParticle.pointer());
            }
        }
        if (!iseq[i].matchedParticle)
            continue;
        ep = iseq[i].matchedParticle->asElementParticle();
        if (!ep) {
            GeneratedParticle* gp =
                iseq[i].matchedParticle->asGeneratedParticle();
            if (gp)
                ep = gp->elementParticle();
        }
        if (ep) {
            GROVE_NAMESPACE::Element* elem = iseq[i].element;
            bool ok2 = ep->element()->validate(schema, elem, true);
            ok = ok2 && ok;
            continue;
        }
        wp = iseq[i].matchedParticle->asWildcardParticle();
        if (0 == wp)
            continue;
        switch(wp->wildcard()->processMode()) {
            default:
                RT_MSG_ABORT("ComplexContent::match: bad wildcard type");

            case Wildcard::NONE:
            case Wildcard::SKIP:
                continue;

            case Wildcard::LAX:
            case Wildcard::WC_STRICT: {
                GROVE_NAMESPACE::Element* ce = iseq[i].element;
                Schema* s = schema->xsi()->importedSchema(ce->xmlNsUri());
                XsElement* xse = 0;
                if (s)
                    xse = s->xsd()->elementSpace().lookup
                        (ce->localName(), ce->xmlNsUri());
                else
                    xse = schema->xsd()->elementSpace().lookup
                        (ce->localName(), ce->xmlNsUri());
                if (0 == xse) {
                    if (schema)
                        schema->mstream() << XsMessages::elementTypeNotFound
                                          << Message::L_ERROR
                                          << ce->xmlNsUri()
                                          << ce->localName()
                                          << DV_ORIGIN(ce);
                    ok = false;
                    continue;
                }
                if (wp->wildcard()->processMode() == Wildcard::LAX)
                    continue;
                bool ok2 = xse->validate(schema, ce, recursive);
                ok = ok2 && ok;
                if (ms.editMode)
                    XsNodeExt::set(ce, xse);
            }
        }
    }
    return ok;
}

const Matcher* ComplexContent::matcher(Schema* schema,
                                       const COMMON_NS::String& name) const
{
    if (matcher_.isNull())
        matcher_ = Matcher::build(schema, connector_.pointer(), name);
    return matcher_.pointer();
}

bool ComplexContent::validate(Schema* schema,
                              const Node*,
                              const String&,
                              String*) const
{
    if (schema)
        schema->mstream() << XsMessages::cannotDataValidateElt
                          << Message::L_ERROR
                          << XS_ORIGIN;
    return false;
}

bool ComplexContent::makeContent(Schema* schema,
                                 GROVE_NAMESPACE::Element* parentElem,
                                 FixupSet* elemSet,
                                 const String* defv) const
{
    if (connector_.isNull())
        return true;
    if (defv)
        if (schema)
            schema->mstream() << XsMessages::cannotDefaultComplex
                              << Message::L_ERROR
                              << XS_ORIGIN;
    return connector_->makeSkeleton(schema, parentElem, parentElem, elemSet);
}

void ComplexContent::setBaseType(const XsTypeInst& type)
{
    baseType_ = type;
}

void ComplexContent::setConnector(RefCntPtr<Connector>& connector)
{
    connector_ = connector;
}

void ComplexContent::dump(int indent) const
{
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++)
        std::cerr << ' ';
    std::cerr << NOTR("ComplexContent:") << std::endl;
    connector_->dump(indent+1);
    XsContent::dumpAttrs(indent+1);

    // todo: FSM dump

#endif // XS_DEBUG
}

ComplexContent::~ComplexContent()
{
}

PRTTI_IMPL(ComplexContent);

XS_NAMESPACE_END
