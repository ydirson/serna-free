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

#include "xs/XsData.h"
#include "xs/Schema.h"
#include "xs/XsMessages.h"
#include "xs/XsAttribute.h"
#include "xs/XsAttributeGroup.h"
#include "xs/components/XsAnyAttribute.h"
#include "xs/components/AttributeSet.h"
#include "xs/components/XsAttributeImpl.h"
#include "xs/components/XsAttributeGroupImpl.h"
#ifdef __SUNPRO_CC
# include "XsDataImpl.h"
#endif
#include "grove/ValidationCommandsMaker.h"
#include "grove/Nodes.h"
#include "grove/XmlPredefNs.h"
#include "common/PropertyTree.h"
#include <iostream>

USING_COMMON_NS
USING_GROVE_NAMESPACE

namespace {

template <class T> 
bool find_attr(const Vector<T>& vec, const T value)
{
    for (ulong i = 0; i < vec.size(); ++i) 
        if (vec[i]->constCred()->name() == value->constCred()->name())
            return true;
    return false;
}

template<class T> 
bool compareVectors(const Vector<T>& vec1, const Vector<T>& vec2)
{
    for(ulong i = 0; i < vec1.size(); i++) 
        if (!find_attr(vec2, vec1[i]))
            return false;
    return true;
}

}

XS_NAMESPACE_BEGIN

AttributeSet::AttributeSet()
{
}

AttributeSet::~AttributeSet()
{
}

void AttributeSet::addAttribute(const XsAttributeInst& attr)
{
    attrVec_.push_back(attr);
}

void AttributeSet::addAttributeGroup(const XsAttributeGroupInst& attr_grp)
{
    attrGroupVec_.push_back(attr_grp);
}

void AttributeSet::setAnyAttribute(RefCntPtr<XsAnyAttribute>& any_attr)
{
    anyAttribute_ = any_attr;
}

void AttributeSet::join(AttributeSet* attr)
{
    ulong i = 0;
    for (i = 0; i < attr->attrVec_.size(); i++) {
        if (!find_attr(attrVec_, attr->attrVec_[i]))
            attrVec_.push_back(attr->attrVec_[i]);
    }
    for (i = 0; i < attr->attrGroupVec_.size(); i++)
        attrGroupVec_.push_back(attr->attrGroupVec_[i]);
    if (0 == anyAttribute_.pointer())
        if (0 != attr->anyAttribute_.pointer())
            anyAttribute_ =  attr->anyAttribute_.pointer();
}

bool AttributeSet::check(Schema* schema,
                         const Node* o,
                         AttributeSet* attr) const
{
    if (!compareVectors(attrVec_, attr->attrVec_)) {
        schema->mstream() << XsMessages::noAttrInBase
            << Message::L_ERROR  << NOTR("attribute") << SRC_ORIGIN(o);
        return false;
    }

    if (!compareVectors(attrGroupVec_, attr->attrGroupVec_)) {
        schema->mstream() << XsMessages::noAttrInBase
            << Message::L_ERROR  << NOTR("attribute group") << SRC_ORIGIN(o);
        return false;
    }

    if (0 != anyAttribute_.pointer()) {
        schema->mstream() << XsMessages::noAttrInBase
            << Message::L_ERROR  << NOTR("wildcard") << SRC_ORIGIN(o);
        return false;
    }
    return true;
}

bool AttributeSet::isEmpty() const
{
    if ((0 == anyAttribute_.pointer()) &&
        (0 == attrVec_.size()) &&
        (0 == attrGroupVec_.size()))
        return true;
    return false;
}

bool AttributeSet::validate(Schema* schema,
                            GROVE_NAMESPACE::Element* elem,
                            Vector<GROVE_NAMESPACE::Attr*>& attrs) const
{
    Vector<XsAttribute*> required_attrs;
    Vector<GroveLib::Attr*> dcheck_attrs;
    bool ok = validate(schema, elem, attrs, &required_attrs);
    if (0 == required_attrs.size())
        return ok;
    if (!schema->requiredAttrsProvider() || !schema->commandMaker()) {
        for (uint i = 0; i < required_attrs.size(); ++i) 
            schema->mstream() << XsMessages::attrRequired
                    << Message::L_ERROR
                    << required_attrs[i]->constCred()->name()
                    << DVXS_ORIGIN(elem, required_attrs[i]);
        return false;
    }
    if (schema->commandMaker()) {
        PropertyTree pt, res;
        for (uint i = 0; i < required_attrs.size(); ++i)
            pt.root()->appendChild(required_attrs[i]->makeSpec(elem));
        schema->requiredAttrsProvider()->
            fillAttributeValues(pt.root(), res.root(), elem);
        schema->commandMaker()->setAttributes(elem, res.root());
        const PropertyNode* pn = res.root()->firstChild();
        for (; pn; pn = pn->nextSibling()) {
            if (pn->getString().isNull())
                continue;
            GroveLib::Attr* a = elem->attrs().getAttribute(pn->name());
            if (a)
                dcheck_attrs.push_back(a);
        }
    }
    return validate(schema, elem, dcheck_attrs, 0) && ok;    
}

bool AttributeSet::validate(Schema* schema,
                            GROVE_NAMESPACE::Element* elem,
                            Vector<GROVE_NAMESPACE::Attr*>& attrs,
                            Vector<XsAttribute*>* required_attrs) const
{
    bool ok = true;
    uint i;
    for (i = 0; i < attrVec_.size(); i++) {
        XsAttribute* cur = attrVec_[i].pointer();
        bool match = false;
        uint to_del;
        for (uint j = 0; j < attrs.size(); j++) {
            GroveLib::Attr* attr = attrs[j];
            if (attr->localName() != attrVec_[i]->constCred()->name() ||
                attr->xmlNsUri() != attrVec_[i]->constCred()->xmlns())
                    continue;
            ok = cur->validate(schema, attr) && ok;
            match = true;
            to_del = j;
            break;
        }
        if (!match) {
            if (XsAttribute::A_REQUIRED == cur->attributeUse())  {
                if (required_attrs)
                    required_attrs->push_back(cur);
                else {
                    schema->mstream() << XsMessages::attrRequired
                                      << Message::L_ERROR
                                      << cur->constCred()->name()
                                      << DVXS_ORIGIN(elem, cur);
                    ok = false;
                }
                continue;
            }
            if (required_attrs && 
                (!cur->defValue().isNull() || !cur->fixValue().isNull())) {
                if (schema->validationFlags() & Schema::makeChanges) {
                    RefCntPtr<GROVE_NAMESPACE::Attr> a;
                    cur->createAttr(schema, elem, a);
                    if (!cur->defValue().isNull()) {
                        a->setValue(cur->defValue());
                    }
                    else if (!cur->fixValue().isNull()) {
                        a->setValue(cur->fixValue());
                    }
                    a->setDefaulted(Attr::DEFAULTED);
                    elem->attrs().appendChild(a.pointer());
                }
            }
        } else {
            attrs.erase(to_del);
        }
    }
    for (i = 0; i < attrs.size(); ) {
        if (attrs[i]->expandedName().uri() == W3C_XML_NAMESPACE)
            attrs.erase(i);
        else
            ++i;
    }
    for (i = 0; i < attrGroupVec_.size(); i++)
        ok = attrGroupVec_[i]->validate(schema, elem, attrs, required_attrs)
            && ok;

    if (0 != anyAttribute_.pointer()) {
        bool is_any = true;
        for (i = 0; i < attrs.size(); i++) {
            is_any = anyAttribute_->validate(schema, attrs[i]);
            if (is_any) {
                attrs.erase(i);
                break;
            }
        }
        ok = (is_any && ok);
    }
    return ok;
}

void AttributeSet::makeSkeleton(Schema* s, 
                                GROVE_NAMESPACE::Element* pe) const
{
    Vector<XsAttribute*> required_attrs;
    if (!s->requiredAttrsProvider()) {
        makeSkeleton(s, pe, 0);
        return;
    }
    makeSkeleton(s, pe, &required_attrs);
    if (0 == required_attrs.size())
        return;
    PropertyTree pt, res;
    for (uint i = 0; i < required_attrs.size(); ++i)
        pt.root()->appendChild(required_attrs[i]->makeSpec(pe));
    s->requiredAttrsProvider()->
        fillAttributeValues(pt.root(), res.root(), pe);
    const PropertyNode* pn = res.root()->firstChild();
    for (; pn; pn = pn->nextSibling()) {
        GroveLib::Attr* pe_attr = pe->attrs().getAttribute(pn->name());
        GroveLib::Attr* attr = pe_attr;
        if (!pe_attr)
            attr = new GroveLib::Attr(pn->name());
        attr->setValue(pn->getString());
        String t = pn->getSafeProperty("type")->getString();
        if (t == "ID")
            attr->setIdClass(GroveLib::Attr::IS_ID);
        else if (t == "IDREF")
            attr->setIdClass(GroveLib::Attr::IS_IDREF);
        if (!pe_attr) {
            pe->attrs().appendChild(attr);
        }
    }
}

void AttributeSet::makeSkeleton(Schema* s, 
                                GROVE_NAMESPACE::Element* pe,
                                Vector<XsAttribute*>* required_attrs) const
{
    ulong i;
    for (i = 0; i < attrVec_.size(); ++i) {
        RefCntPtr<GROVE_NAMESPACE::Attr> ap;
        XsAttribute* xa = attrVec_[i].pointer();
        bool has_fixed = !xa->fixValue().isNull();
        bool has_def = !xa->defValue().isNull();
        bool is_req  = (XsAttribute::A_REQUIRED == xa->attributeUse());
        if (has_fixed || has_def || is_req) {
            if (xa->createAttr(s, pe, ap)) {
                if (has_fixed) {
                    ap->setValue(xa->fixValue());
                    ap->setDefaulted(Attr::DEFAULTED);
                } else if (has_def) {
                    ap->setValue(xa->defValue());
                    ap->setDefaulted(Attr::DEFAULTED);
                } else if (is_req) {
                    if (required_attrs) 
                        required_attrs->push_back(xa);
                    else
                        ap->setValue(xa->skeletonValue(s));
                }
                pe->attrs().appendChild(ap.pointer());
            }
        }
    }
    for (i = 0; i < attrGroupVec_.size(); ++i)
        attrGroupVec_[i]->makeSkeleton(s, pe, required_attrs);
}

bool AttributeSet::attrList(COMMON_NS::Vector<XsAttribute*>& attrList) const
{
    bool has_req = false;
    ulong i;
    for (i = 0; i < attrGroupVec_.size(); ++i)
        if (attrGroupVec_[i]->attrList(attrList) && !has_req)
            has_req = true;
    for (i = 0; i < attrVec_.size(); ++i) {
        attrList.push_back(attrVec_[i].pointer());
        if (XsAttribute::A_REQUIRED == attrVec_[i]->attributeUse())
            has_req = true;
    }
    return has_req;
}

void AttributeSet::dump(int indent ) const
{
#ifdef XS_DEBUG
    if (!isEmpty()) {
        for(int i = 0; i < indent; i++) {
            std::cerr << ' ';
        }
        std::cerr << NOTR("AttributeSet:") << std::endl;
        ulong ui;
        for(ui = 0; ui < attrVec_.size(); ui++)
            attrVec_[ui]->dump(indent+1);
        for(ui = 0; ui < attrGroupVec_.size(); ui++)
            attrGroupVec_[ui]->dump(indent+1);

        if (!anyAttribute_.isNull())
            anyAttribute_->dump(indent+1);
    }
#endif // XS_DEBUG
}

XS_NAMESPACE_END
