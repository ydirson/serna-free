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
#include "xs/XsMessages.h"
#include "xs/complex/Connectors.h"
#include "xs/complex/ElementParticle.h"
#include "xs/complex/WildcardParticle.h"
#include "xs/XsElement.h"
#include "xs/components/XsElementImpl.h"
#include "xs/XsType.h"
#include "grove/Nodes.h"

#include "xs/Schema.h"
#include "xs/XsDataImpl.h"
#include <iostream>

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

Connector::ConnectorType Connector::type() const
{
    return type_;
}

void Connector::setParent(Connector* parent)
{
    parent_ = parent;
}

Connector* Connector::parent()
{
    return parent_;
}

void Connector::addChild(Particle* child)
{
    children_.push_back(child);
}

void Connector::copyContent(Connector* con)
{
    for(ulong i = 0; i < children_.size(); i++)
        con->addChild(children_[i].pointer());
}

void Connector::makeSubst(Schema* schema)
{
    if (substDone_)
        return;
    ExpandedName ename;
    for(ulong i = 0; i < children_.size(); i++) {
        ElementParticle* elem_part = children_[i]->asElementParticle();
        if (elem_part &&
           (static_cast<XsElementImpl*>(elem_part->element())->
                substGroup(ename) ||
            static_cast<XsElementImpl*>
                (elem_part->element())->isTopLevel_)) {
            ename = elem_part->element()->constCred()->expandedName();
            bool is_abstract = 
                static_cast<XsElementImpl*>(elem_part->element())->abstract_;
            Vector<XsElementImpl*> grp;
            schema->xsi()->substGroup(ename, grp);
            if ( 0 == grp.size())
                continue;
            ChoiceConnector* tricky_connector =
                new ChoiceConnector(Connector::CHOICE, origin());
            tricky_connector->setParent(this);
            tricky_connector->setMinOccur(schema, elem_part->minOccur());
            tricky_connector->setMaxOccur(schema, elem_part->maxOccur());
            elem_part->setMinOccur(schema, 1);
            elem_part->setMaxOccur(schema, 1);
            if (!is_abstract)
                tricky_connector->addChild(elem_part);
            for (uint j = 0; j < grp.size(); ++j) {
                if (grp[j]->abstract_)
                    continue;
                ElementParticle* new_elem_part =
                    new ElementParticle(elem_part->element()->origin());
                new_elem_part->elem_ = grp[j];
                tricky_connector->addChild(new_elem_part);
            }
            tricky_connector->substDone_ = true;
            children_[i] = tricky_connector;
        }
        else if (children_[i]->asConnector())
            children_[i]->asConnector()->makeSubst(schema);
    }
    substDone_ = true;
}

bool Connector::check(Schema* schema,
                      const Node* o,
                      const Connector* base) const
{
//TODO : resctrictions Schema Part1: 3.9.6 - Constraints on ParticleSchema Components
//    if (!base->substDone())
//        base->makeSubst(schema);

//base is CHOICE, cur SEQ
//    for(ulong i = 0; i < children_.size(); i++) {
//        if (children_[i]->asElementParticle()) {
//            base->findParticle(children_[i]->asElementParticle())
//
//        }

    if (children_.size() > base->children_.size()) {
        if (schema)
            schema->mstream() << XsMessages::diffContent
                              << Message::L_ERROR
                              << DV_ORIGIN(o);
        return false;
    }
    for(ulong i = 0; i < children_.size(); i++) {
        if( (children_[i]->minOccur() < base->children_[i]->minOccur()) ||
            (children_[i]->maxOccur() > base->children_[i]->maxOccur()) ) {
            if (schema)
                schema->mstream() << XsMessages::occursWider
                                  << Message::L_ERROR
                                  << DV_ORIGIN(o);
            return false;
        }
        if (( children_[i]->asElementParticle() ) &&
            ( base->children_[i]->asElementParticle() )) {
            if ((!children_[i]->asElementParticle()->element()->isValid()) ||
                (!base->children_[i]->asElementParticle()->element()->
                 isValid())) {
                if (schema)
                    schema->mstream() << XsMessages::elemNotValid
                                      << Message::L_ERROR
                                      << DV_ORIGIN(o);
                return false;
            }
            XsElementImpl* cur = static_cast<XsElementImpl*>
                (children_[i]->asElementParticle()->element());
            XsElementImpl* curBase = static_cast<XsElementImpl*>
                (base->children_[i]->asElementParticle()->element());
            if (! (cur->constCred()->expandedName() == curBase->constCred()->
                   expandedName())) {
                if (!schema)
                    return false;
                Vector<XsElementImpl*> grp;
                schema->xsi()->substGroup
                             (curBase->constCred()->expandedName(), grp);
                bool found = false;
                for (uint j = 0; j < grp.size(); ++j) {
                    if (cur->constCred()->expandedName() ==
                        grp[j]->constCred()->expandedName()) {
                        found = true;
                        curBase = grp[j];
                    }
                }
                if (!found) {
                    schema->mstream() << XsMessages::diffElemNames
                                      << Message::L_ERROR
                                      << DV_ORIGIN(o);
                    return false;
                }
            }
            if ((!curBase->fixed_.isNull()) || (!curBase->default_.isNull())) {
                if ((cur->fixed_.isNull()) && (cur->default_.isNull())) {
                    if (schema)
                        schema->mstream() << XsMessages::missDefault
                        << Message::L_ERROR << DV_ORIGIN(o);
                    return false;
                }
            }
            if (!curBase->baseType_.isNull()) {
                if (cur->baseType_.isNull()) {
                    if (schema)
                        schema->mstream() << XsMessages::noElemType
                        << Message::L_ERROR << DV_ORIGIN(o);
                    return false;
                }
                else {
                    if (!(curBase->baseType_->constCred()->expandedName() ==
                          cur->baseType_->constCred()->expandedName())) {
                        if (schema)
                            schema->mstream() << XsMessages::diffBaseTypes
                            << Message::L_ERROR << DV_ORIGIN(o);
                        return false;
                    }
                }
            }
        }
        else if (( children_[i]->asConnector() ) &&
                 ( base->children_[i]->asConnector() )) {
            if (children_[i]->asConnector()->type() !=
                base->children_[i]->asConnector()->type()) {
                if (schema)
                    schema->mstream() << XsMessages::diffConnectors
                    << Message::L_ERROR  << DV_ORIGIN(o);
                return false;
            }
            else {
                children_[i]->asConnector()->check(schema, o,
                    base->children_[i]->asConnector());
            }
        }
        else if (( children_[i]->asWildcardParticle() ) &&
                 ( base->children_[i]->asWildcardParticle() )) {
        }
        else {
            if (schema)
                schema->mstream() << XsMessages::mismatchedStructure
                << Message::L_ERROR << DV_ORIGIN(o);
            return false;
        }
    }
    return true;
}

void Connector::dumpChildren(int indent) const
{
#ifdef XS_DEBUG
    indent += 2;
    for(ulong i = 0; i < children_.size(); i++)
        children_[i]->dump(indent);
#endif // XS_DEBUG
}

PRTTI_BASE_STUB(Connector, AllConnector)
PRTTI_BASE_STUB(Connector, ChoiceConnector)
PRTTI_BASE_STUB(Connector, SequenceConnector)
PRTTI_BASE_STUB(Connector, GroupConnector)
PRTTI_IMPL(Connector)

XS_NAMESPACE_END
