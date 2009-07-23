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
#include "xs/XsType.h"
#include "xs/XsMessages.h"
#include "xs/SchemaNamespaces.h"
#include "xs/XsIdentityConstraint.h"
#include "xs/Piece.h"
#include "xs/XsDataImpl.h"
#include "xs/Component.h"
#include "xs/XsNodeExt.h"
#include "xs/debug.h"
#include "xs/complex/FixupSet.h"
#include "xs/complex/XsComplexTypeImpl.h"
#include "xs/complex/SimpleContent.h"
#include "xs/components/AttributeSet.h"
#include "xs/components/XsElementImpl.h"
#include "xs/datatypes/XsSimpleTypeImpl.h"
#include "xs/datatypes/SimpleDerivedType.h"
#include "xs/datatypes/DerivationTypes.h"
#include "grove/ValidationCommandsMaker.h"
#include "grove/Nodes.h"
#include "grove/XmlNs.h"

#include <iostream>

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

typedef XS_EXPIMP const char* const exported_literal;

exported_literal ELEM_VALUE_DEFAULT   = NOTR("default");
exported_literal ELEM_VALUE_ENUM      = NOTR("enum");
exported_literal ELEM_VALUE_ENUM_ITEM = NOTR("item");
exported_literal ELEM_VALUE_IS_LIST   = NOTR("is-list");

XsElementImpl::~XsElementImpl()
{
}

XsElementImpl::XsElementImpl(const Origin& origin,
                             const NcnCred& cred, bool isTopLevel)
    : XsElement(origin, cred),
      abstract_(false),
      default_(String::null()),
      fixed_(String::null()),
      nullable_(false),
      isAnyType_(false),
      isTopLevel_(isTopLevel)
{
}

void XsElementImpl::addIConstraint(const XsIdentityConstraintInst& key)
{
     keyVector_.push_back(key);
}

bool XsElementImpl::checkIConstraint(Schema* schema,
                                     Element* elem) const
{
    bool ok = true;
    for (uint i = 0; i < keyVector_.size(); i++) {
        ok = keyVector_[i]->validate(schema, elem) && ok;
    }
    return ok;
}

XsType* XsElementImpl::xstype() const
{
    if (baseType_.isNull())
        return 0;
    return baseType_.pointer();
}

bool XsElementImpl::isValid() const
{
   return true;
}

bool XsElementImpl::substGroup(ExpandedName& ename) const
{
    if (substitutionGroup_.localName().isNull())
        return false;
    ename = substitutionGroup_;
    return true;
}

bool XsElementImpl::cdataAllowed() const
{
    if (baseType_.isNull() || isAnyType_)
        return true;
    return baseType_->cdataAllowed();
}

bool XsElementImpl::validate(Schema* schema,
                             Element* elem,
                             bool recursive) const
{
    DDBG <<  "Validating " + elem->nodeName() << std::endl;
    XsNodeExt::set(elem, this);
    if (baseType_.isNull() || isAnyType_) {
        if (!substitutionGroup_.localName().isNull()) {
            Vector<XsElementImpl*> grp;
            schema->xsi()->substGroup(substitutionGroup_, grp);
            if (0 < grp.size()) {
                for (uint k = 0; k < grp.size(); ++k) {
                    if (true == grp[k]->abstract_ && 
                        grp[k]->substitutionGroup_.localName().isNull())
                            return grp[k]->validate(schema, elem, recursive);
                }
            }
        }
        return true;
    }
    XsTypeInst type_inst = baseType_;
    bool nullable = nullable_;
    for (Attr* a = elem->attrs().firstChild(); a; a = a->nextSibling()) {
        if (a->xmlNsUri() ==  XSI_NAMESPACE)
            if (a->localName() == "type") {
                XsDataImpl* xsi = schema->xsi();
                String xs_type = a->value();
                if (!ends_with(xs_type, "anyType")) {
                    if (xsi->elemForm() == NcnCred::QUALIFIED) {
                        String pref = xsi->schemaRoot()->
                            getPrefixByXmlNs(xsi->targetNsUri());
                        if (!pref.isEmpty() && xs_type.find(':') < 0)
                            xs_type.insert(0, pref + ':');
                    }
                    xsi->makeRefInst(type_inst, schema,
                        const_cast<Element*>(this->origin()),
                        xs_type, Component::type);
                }
                else
                    return checkIConstraint(schema, elem);
            }
            else if ("nil" == a->localName()) {
                if ("true" == a->value().lower())
                nullable = true;
            }
    }
    if (nullable) {  // NOTE: NILLABLE - content might be empty
        bool hasContent = false;
        for (Node* n = elem->firstChild(); n ; n = n->nextSibling()) {
            if (n->nodeType() == Node::ELEMENT_NODE) {
                hasContent = true;
                break;
            }
            if (n->nodeType() == Node::TEXT_NODE) {
                const Text* t = CONST_TEXT_CAST(n);
                if (t->data().length() > 0) {
                    hasContent = true;
                    break;
                }
            }
        }
        if (nullable && !hasContent)
            return checkIConstraint(schema, elem);
    }
    if (!fixed_.isNull() || !default_.isNull()) {
        String content;
        for (Node* n = elem->firstChild(); n ; n = n->nextSibling()) {
            if (n->nodeType() == Node::ELEMENT_NODE) {
                if (schema)
                    schema->mstream() << XsMessages::defaultMixedContent
                                      << Message::L_ERROR
                                      << elem->name()
                                      << DV_ORIGIN(elem);
                break;
            }
            if (n->nodeType() == Node::TEXT_NODE) {
                const Text* t = CONST_TEXT_CAST(n);
                content += t->data();
            }
        }
        const String& v_data = fixed_.isNull() ? default_ : fixed_;
        if (!v_data.isNull()) {            
            if (content.isNull()) {
                Text* t = make_text_choice(baseType_.pointer());
                t->setData(v_data);
                if (schema->commandMaker()) 
                    schema->commandMaker()->insertNode(0, elem, t);
                else
                    elem->appendChild(t);
            } else if (!fixed_.isNull() && content != v_data) {
                if (schema)
                    schema->mstream() << XsMessages::fixedNotEqContent
                                      << Message::L_ERROR
                                      << elem->name()
                                      << DV_ORIGIN(elem);
            }
        }
    }
    bool ok = true;
    if (!type_inst.isNull())
        ok = type_inst->validate(schema, elem, recursive) && ok;
    return ok && checkIConstraint(schema, elem);
}

void XsElementImpl::make_inst(Grove*,
                              const Element* pe,
                              ElementPtr& ep) const
{
    String myns = constCred()->xmlns();
    if (pe) {
        String pv = pe->getPrefixByXmlNs(myns);
        if (pv.isNull()) {    // appropriate prefix not found; create
            for (ulong idx = pe->nsMapList().countChildren(); ; ++idx) {
                pv = String("xs") + String::number(idx);
                if (pe->lookupPrefixMap(pv).isNull())
                    break;
            }
            String eln = pv + ':' + constCred()->name();
            ep = new Element(eln);
            ep->addToPrefixMap(pv, TS_COPY(constCred()->xmlns()));
            return;
        }
        if (pv.isEmpty()) {
            pv = TS_COPY(constCred()->name());
        } else {
            pv += ':';
            pv += constCred()->name();
        }
        ep = new Element(pv);
        return;
    }
    ep = new Element(TS_COPY(constCred()->name()));
    ep->addToPrefixMap(XmlNs::defaultNs(), myns);
    return;
}

bool XsElementImpl::makeSkeleton(Schema* s,
                                 Grove* referenceGrove,
                                 ElementPtr& ep,
                                 const Element* pe) const
{
    bool ok = true;
    make_inst(referenceGrove, pe, ep);
    if (baseType_.isNull())
        return ok;
    XsType* bt = baseType_.pointer();
    if (pe)
        ep->setParent(const_cast<Element*>(pe));
    bt->makeAttrs(s, ep.pointer());
    FixupSet elemSet;
    ok = bt->makeContent(s, ep.pointer(), &elemSet,
        default_.isEmpty() ? 0 : &default_);
    if (pe)
        ep->setParent(0);
    return ok;
}

bool XsElementImpl::makeContent(Schema* s,
                                Element* pe,
                                FixupSet* elemSet) const
{
    if (baseType_.isNull())
        return true;
    return baseType_->makeContent(s, pe, elemSet,
        default_.isEmpty() ? 0 : &default_);
}

void XsElementImpl::makeInstance(Schema* s,
                                 Grove* referenceGrove,
                                 ElementPtr& ep,
                                 const Element* pe) const
{
    make_inst(referenceGrove, pe, ep);
    if (baseType_.isNull())
        return;
    if (pe)
        ep->setParent(const_cast<Element*>(pe));
    baseType_->makeAttrs(s, ep.pointer());
    if (pe)
        ep->setParent(0);
}

void XsElementImpl::getPossibleEnumlist(PropertyNode* node) const
{
    if (baseType_.isNull())
        return;
    EnumList lst;
    const XsSimpleTypeImpl* xsti = baseType_->getSimpleType();
    if (0 == xsti)
        return;
    const SimpleDerivedType* sdt = xsti->asConstSimpleDerivedType();
    if (0 == sdt)
        return;
    bool has_list = 
        const_cast<SimpleDerivedType*>(sdt)->fillPossibleEnums(lst);
    if (lst.begin() != lst.end()) {
        PropertyNode* enm = node->makeDescendant(ELEM_VALUE_ENUM);
        EnumList::iterator i = lst.begin();
        for (; i != lst.end(); ++i)
            enm->appendChild(new Common::PropertyNode(
                             ELEM_VALUE_ENUM_ITEM, i->value_));
    }    
    if (!default_.isEmpty())
        node->makeDescendant(ELEM_VALUE_DEFAULT)->setString(default_);
    if (has_list)
        node->makeDescendant(ELEM_VALUE_IS_LIST);
}

void XsElementImpl::dump(int indent) const
{
    (void) indent;
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++) {
         std::cerr << ' ';
    }
    std::cerr << NOTR("XsElement: ") << constCred()->name();
    if (!baseType_.isNull()) {
        std::cerr << std::endl;
        baseType_->dump(indent+1);
    }
    else {
        std::cerr << NOTR(" no base type") << std::endl;
    }
#endif // XS_DEBUG
}

XS_NAMESPACE_END
