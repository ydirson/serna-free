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
#include "xs/components/XsAttributeImpl.h"
#include "xs/XsMessages.h"
#include "xs/XsType.h"
#include "xs/datatypes/XsSimpleTypeImpl.h"
#include "xs/datatypes/SimpleDerivedType.h"
#include "xs/datatypes/DerivationTypes.h"
#ifdef __SUNPRO_CC
# include "XsDataImpl.h"
#endif
#include "grove/Nodes.h"
#include "grove/XmlPredefNs.h"
#include "grove/IdManager.h"
#include "grove/EntityDeclSet.h"
#include "grove/IdManager.h"
#include "common/PropertyTree.h"
#include <iostream>

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

typedef XS_EXPIMP const char* const exported_literal;

exported_literal ATTR_TYPE          = NOTR("type");
exported_literal ATTR_REQUIRED      = NOTR("is-required");
exported_literal DEFAULT_ATTR_VALUE = NOTR("default-value");
exported_literal FIXED_ATTR_VALUE   = NOTR("fixed-value");
exported_literal ATTR_VALUE_ENUM    = NOTR("enum");

XsAttributeImpl::XsAttributeImpl(const Origin& origin, const NcnCred& cred)
    : XsAttribute(origin, cred),
      form_(AF_UNDEF),
      use_(A_UNDEF),
      default_(String::null()),
      fixed_(String::null())
{
}

XsAttribute::AttributeForm XsAttributeImpl::attributeForm() const
{
    return (form_ == AF_UNDEF) ? QUALIFIED : form_;
}

XsAttribute::AttributeUse XsAttributeImpl::attributeUse() const
{
    if (use_ != A_UNDEF) 
        return use_;
    if (!default_.isNull())
        return A_DEFAULT;
    if (!fixed_.isNull())
        return A_FIXED;
    return A_OPTIONAL;
}

const String& XsAttributeImpl::defValue() const
{
    return default_;
}

const String& XsAttributeImpl::fixValue() const
{
    return fixed_;
}

void XsAttributeImpl::setBaseType(const XsTypeInst& type)
{
    type_ = type;
}

void XsAttributeImpl::setRef(const XsAttributeInst& inst)
{
    ref_ = inst;
}

XsType* XsAttributeImpl::xstype() const
{
    if (type_.isNull())
        return 0;
    return type_.pointer();
}

void XsAttributeImpl::getPossibleEnumlist(EnumList& lst) const
{
    if (type_.isNull())
        return;
    if (XsType::simpleType != type_->typeClass())
        return;
    SimpleDerivedType* st = type_->asXsSimpleTypeImpl()->asSimpleDerivedType();
    if (0 == st)
        return;
    st->fillPossibleEnums(lst);
}

const XsAttribute* XsAttributeImpl::refAttr() const
{
    return ref_.isNull() ? this : ref_.pointer()->refAttr();
}

PropertyNode* XsAttributeImpl::makeSpec(const GroveLib::Element* elem) const
{
    const XsAttribute* ref_attr = refAttr();
    String attr_pref;
    if (!constCred()->xmlns().isEmpty()) 
        attr_pref = elem->getPrefixByXmlNs(constCred()->xmlns());
    PropertyNode* spec = 
        new PropertyNode(attr_pref.isEmpty() ? constCred()->name() :
            attr_pref + ':' + constCred()->name(), constCred()->xmlns());
    String attr_type = (ref_attr->xstype() && 
        !ref_attr->xstype()->constCred()->name().isEmpty())
        ? ref_attr->xstype()->constCred()->name() : NOTR("string");
    spec->appendChild(new PropertyNode(Xs::ATTR_TYPE, attr_type));
    switch (ref_attr->attributeUse()) {
        case A_REQUIRED:
            spec->appendChild(new PropertyNode(ATTR_REQUIRED, NOTR("true")));
            break;
        case A_DEFAULT:
            spec->appendChild(new PropertyNode(DEFAULT_ATTR_VALUE, 
                ref_attr->defValue()));
            break;
        case A_FIXED:
            spec->appendChild(new PropertyNode(FIXED_ATTR_VALUE, 
                ref_attr->fixValue()));
            break;

        default:
            break;
    }
    //! Value enumeration
    PropertyNodePtr value_enum = new PropertyNode(Xs::ATTR_VALUE_ENUM);
    // at this point, sub-elements are not connected into tree, but parent
    // is already set
    const GroveLib::Grove* grove = elem->root()->grove();
    if (grove && "IDREF" == attr_type) {
        if (grove->idManager())
            grove->idManager()->getIdList(value_enum.pointer());
    } else {
        if (grove && "ENTITY" == attr_type) {
            GroveLib::EntityDeclSet::iterator i =
                grove->entityDecls()->begin();
            for (; i != grove->entityDecls()->end(); ++i) {
                if ((*i)->declOrigin() != GroveLib::EntityDecl::prolog &&
                    (*i)->declOrigin() != GroveLib::EntityDecl::dtd)
                        continue;
                if ((*i)->dataType() != GroveLib::EntityDecl::ndata)
                    continue;
                GroveLib::EntityDecl::DeclType dt = (*i)->declType();
                if (GroveLib::EntityDecl::internalGeneralEntity == dt ||
                    GroveLib::EntityDecl::externalGeneralEntity == dt)
                        value_enum->appendChild(new PropertyNode((*i)->name()));
            }
        } else {
            EnumList xs_enum;
            ref_attr->getPossibleEnumlist(xs_enum);
            EnumList::iterator i = xs_enum.begin();
            for (; i != xs_enum.end(); ++i)
                value_enum->appendChild(new PropertyNode(i->value_));
        }
    }
    if (value_enum->firstChild())
        spec->appendChild(value_enum.pointer());
    return spec;
}

static bool validate_id(Schema* schema, Attr* attr, const Piece* p)
{
    if (attr->idClass() == Attr::IS_ID || attr->idClass() == Attr::IS_IDREF) 
        return true;   // ID already checked/set
    if (schema->validationFlags() & Schema::makeChanges)  // open mode
        attr->grove()->idManager()->attributeAdded(attr);
    // validate scoped-ids
    if (attr->idSubClass() != Attr::NOT_SCOPED_ID) {
        switch (attr->idSubClass()) {
            case Attr::SCOPED_IDREF: {
                String v = attr->value();
                if (v.rfind('#') > 0) // do not validate non-local references
                    break;
                schema->idrefDelayedCheck(attr);
                break;
            }
            case Attr::SCOPED_ID: 
                if (attr->isDuplicateId()) {
                    schema->mstream() << XsMessages::duplicateId 
                        << Message::L_ERROR
                        << attr->value() << DVXS_ORIGIN(attr, p);
                    return false;
                }
                break;    
            default:
                break;
        }
    } 
    return true;
}

bool XsAttributeImpl::validate(Schema* schema,
                               GROVE_NAMESPACE::Attr* attr) const
{
    if (!ref_.isNull()) {
        XsAttributeImpl* self = const_cast<XsAttributeImpl*>(this);
        const XsAttribute* ref_attr = refAttr();
        self->type_ = ref_attr->xstype();
        if (A_UNDEF == use_)
            self->use_  = ref_attr->attributeUse();
        if (AF_UNDEF == form_)
            self->form_ = ref_attr->attributeForm();
        if (defValue().isNull())
            self->default_ = ref_attr->defValue();
        if (fixValue().isNull())
            self->fixed_ = ref_attr->fixValue();
        self->ref_ = 0;
    }
    if (XsAttribute::A_PROHIBITED == attributeUse()) {
        schema->mstream() << XsMessages::attrProhibited
                          << Message::L_ERROR
                          << attr->name() << DV_ORIGIN(attr);
        return false;
    }   
    if (!fixValue().isNull()) {
        if (fixValue() == attr->value())
            return true;
        schema->mstream() << XsMessages::attrFixed
                          << Message::L_ERROR
                          << fixValue()
                          << attr->name()
                          << DV_ORIGIN(attr);
        if (schema->validationFlags() & Schema::makeChanges) {
            String old_value = attr->value();
            attr->setValue(fixValue());
            attr->setDefaulted(GroveLib::Attr::DEFAULTED);
        }
        return false;
    }
    if (!defValue().isNull() && attr->value().isEmpty()
        && (schema->validationFlags() & Schema::makeChanges)) {
            attr->setValue(defValue());
            attr->setDefaulted(GroveLib::Attr::DEFAULTED);
    }
    if (type_.isNull())
        return validate_id(schema, attr, this);
    XsType* baseType = type_.pointer();
    switch (baseType->typeClass()) {
        case XsType::simpleType: {
            String source = attr->value();
            String result;
            XsSimpleTypeImpl* xsti = baseType->asXsSimpleTypeImpl();
            if (0 == xsti) {
                refToUndef(schema);
                return false;
            }
            const String& type_name = xsti->constCred()->name();
            if (type_name == "ENTITY" || type_name == "IDREF") 
                attr->setEnumerated(true);
            else {
                SimpleDerivedType* st = xsti->asSimpleDerivedType();
                if (st) {
                    EnumList lst;
                    if (st->fillPossibleEnums(lst))
                        attr->setEnumerated(true);
                }
            }
            if (!xsti->validate(schema, attr, source, &result)) {
                schema->mstream() << XsMessages::attrNotValid
                                  << Message::L_ERROR
                                  << attr->name()
                                  << DV_ORIGIN(attr);
                return false;
            }
            return validate_id(schema, attr, this);
        }
        case XsType::complexType: {
            schema->mstream() << XsMessages::attrComplexBaseType
                              << Message::L_ERROR
                              << attr->name()
                              << DV_ORIGIN(attr);
            return false;
        }
        default: {
            schema->mstream() << XsMessages::attrUnknownBaseType
                              << Message::L_ERROR
                              << attr->name()
                              << DV_ORIGIN(attr);
            return false;
        }
    }
}

bool XsAttributeImpl::createAttr(Schema*,
                                 GROVE_NAMESPACE::Element* pe,
                                 RefCntPtr<GROVE_NAMESPACE::Attr>& a) const
{
    if (constCred()->xmlns().isEmpty()) {
        a = new GROVE_NAMESPACE::Attr(TS_COPY(constCred()->name()));
        return true;
    }
    String pv = pe->getPrefixByXmlNs(constCred()->xmlns());
    if (pv.isEmpty()) {
        if (W3C_XML_NAMESPACE == constCred()->xmlns())
            pv = NOTR("xml");
        else {
            // Create prefix local to element
            for (ulong idx = pe->attrs().countChildren(); ; ++idx) {
                pv = String(NOTR("xsa")) + String::number(idx);
                if (pe->lookupPrefixMap(pv).isNull())
                    break;
            }
            pe->addToPrefixMap(pv, TS_COPY(constCred()->xmlns()));
        }
    }
    pv += ':';
    pv += constCred()->name();
    a = new GROVE_NAMESPACE::Attr(pv);
    return true;
}

String XsAttributeImpl::skeletonValue(Schema*) const
{
    String rv("");
    if (type_.isNull())
        return rv;
    XsSimpleTypeImpl* xsti = type_->asXsSimpleTypeImpl();
    if (0 == xsti)
        return rv;
    xsti->validate(0, String(), &rv);
    return rv;
}

void XsAttributeImpl::dump(int indent) const
{
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++)
         std::cerr << ' ';
    std::cerr << NOTR("XsAttribute ") << constCred()->name() << "/"
        << constCred()->xmlns() << NOTR(": use=") << use_ 
        << NOTR("dv=[") << default_ 
        << NOTR("] fv=[") << fixed_ << "]" << std::endl;
    if (!type_.isNull()) {
        std::cerr << std::endl;
        type_->dump(indent + 1);
    }
    else {
        std::cerr << NOTR("no base type") << std::endl;
    }
#endif // XS_DEBUG
}

XS_NAMESPACE_END
