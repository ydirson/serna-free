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

#include "xs/datatypes/DerivationTypes.h"
#include "xs/XsDataImpl.h"
#include "xs/complex/Particle.h"
#include "xs/XsMessages.h"
#include "xs/Schema.h"
#include "xs/ExtPythonScript.h"
#include "xs/datatypes/SimpleDerivedType.h"
#include "xs/datatypes/AtomicType.h"
#include "xs/datatypes/Facets.h"
#include "xs/complex/SimpleContent.h"
#include "xs/complex/XsComplexTypeImpl.h"
#include "grove/Nodes.h"

#include <qstringlist.h>
#include <iostream>

USING_COMMON_NS
USING_GROVE_NAMESPACE
using namespace std;

XS_NAMESPACE_BEGIN

/*****************************************************************************
 * Restriction                                                               *
 *****************************************************************************/

Restriction::Restriction(const Origin& origin,
                         const NcnCred&  cred)
  : SimpleDerivedType(SimpleDerivedType::RESTRICTION, origin, cred)
{
}

Restriction::~Restriction()
{
}

bool Restriction::fillPossibleEnums(EnumList& lst)
{
    if (enumList_.begin() == enumList_.end()) {
        if (baseType().isNull())
            return false;
        XsType* base = baseType().pointer();
        if (XsType::simpleType == base->typeClass() &&
            XsSimpleTypeImpl::DERIVED == base->asXsSimpleTypeImpl()->type()) 
            return base->asXsSimpleTypeImpl()->asSimpleDerivedType()->
                fillPossibleEnums(lst);
        else
            return false;
    }
    std::list<EnumListItem>::iterator it = enumList_.begin();
    for (; it != enumList_.end(); ++it) 
        lst.push_back(EnumListItem(it->value_));
    return true;
}

void Restriction::addFacet(RefCntPtr<XsFacet>& facet)
{
    facetsVector_.push_back(facet);
    if (XsFacet::ENUMERATION == facet->type()) {
        EnumerationFacet* ef = facet->asEnumerationFacet();
        if (!ef->title().isNull())
            enumList_.push_back(EnumListItem(ef->title(), ef->value()));
        else
            enumList_.push_back(EnumListItem(ef->value()));
    }
}

bool Restriction::validate(Schema* schema, const GroveLib::Node* o,
                           const String& source,
                           String* result,
                           bool silent) const
{
    for (const XsType* type = baseType().pointer(); type ;) {
        if (type->typeClass() == XsType::unknown) {
            schema->mstream() << XsMessages::referenceToUndefined
                              << Message::L_ERROR
                              << type->constCred()->format()
                              << DV_ORIGIN(o);
            return false;
        }
        const XsSimpleTypeImpl* xsi = type->getSimpleType();
        if (0 == xsi)
            break;
        if (XsSimpleTypeImpl::ATOMIC == xsi->type()) {
            bool valid = validate(schema, o, source, result, silent, xsi);
            if ((!valid) && checkCustomMessage())
                if (!silent)
                    schema->mstream() << XsMessages::userDefined
                                      << Message::L_ERROR << DV_ORIGIN(o);
            return valid;
        } else if (XsSimpleTypeImpl::DERIVED == xsi->type()) 
            type = xsi->asConstSimpleDerivedType()->baseType().pointer();
        else
            break;
    }
    if (schema)
        schema->mstream() << XsMessages::invalidBaseType
                          << Message::L_ERROR
                          << DV_ORIGIN(o);
    return false;
}


bool Restriction::validate(Schema* schema, const GroveLib::Node* o,
                           const String& source,
                           String* result,
                           bool silent,
                           const XsSimpleTypeImpl* root) const
{
    SetNoMsg noMsg(const_cast<Restriction*>(this));
    
    if (0 == root)
        return false;
    
    XsType* base = baseType().pointer();
    
    String src = getTrimmed(schema, o, source);

    bool valid = true;
#ifdef USE_PYTHON
    if (!pyPreFunc().isNull() && schema) {
        *result = src;
        valid = schema->xsi()->pyScript()->
            pyValidate(schema, o, this, pyPreFunc(), *result);
        if ((!valid) && (!silent))
            schema->mstream() << XsMessages::preScriptReturn
                              << Message::L_ERROR
                              << DV_ORIGIN(o);
        src = *result;
    }
#endif // USE_PYTHON

    if (XsType::simpleType == base->typeClass()) {
        XsSimpleTypeImpl* xsi = base->asXsSimpleTypeImpl();
        SimpleDerivedType* sdt = xsi->asSimpleDerivedType();
        if (restricted_.isNull() && 
            (XsSimpleTypeImpl::ATOMIC == xsi->type() || facetsVector_.size())) {
            // ZATYCHKA - facets don't work for lists
            if (!sdt || sdt->derivationMethod() != SimpleDerivedType::LIST) {
                restricted_ = root->asConstAtomicType()->clone();
                for (uint i = 0; i < facetsVector_.size(); i++)
                    restricted_->addFacet(schema, &*facetsVector_[i]);
            }
            facetsVector_.clear();
        }
        if (restricted_)   
            valid = restricted_->validate(schema, o, src, result, silent) &&
                valid;
        if (XsSimpleTypeImpl::DERIVED == xsi->type())
            valid = xsi->asSimpleDerivedType()->
                validate(schema, o, src, result, silent) && valid;
    } else if (XsType::complexType == base->typeClass()) {
        if (SimpleContent* sc = base->asXsComplexTypeImpl()->
            content()->asSimpleContent())
            valid = sc->validate(silent ? 0 : schema, o, src, result);
    }
    else
        valid = false;

#ifdef USE_PYTHON

    if (!pyPostFunc().isNull() && schema) {
        valid = schema->xsi()->pyScript()->
            pyValidate(schema, o, this, pyPostFunc(), *result) && valid;
        if ((!valid) && (!silent))
            schema->mstream() << XsMessages::postScriptReturn
                              << Message::L_ERROR  << DV_ORIGIN(o);
    }

#endif // USE_PYTON

    return valid;
}

void Restriction::dump(int indent) const
{
#ifdef XS_DEBUG
    int i;
    for(i = 0; i < indent; i++)
         std::cerr << ' ';
    std::cerr << NOTR("Restriction:") << std::endl;
    for(i = 0; i < indent+1; i++) {
         std::cerr << ' ';
    }
    baseType()->dump(0);
#endif // XS_DEBUG
}

PRTTI_IMPL(Restriction)

/*****************************************************************************
 * List                                                                      *
 *****************************************************************************/

List::List(const Origin& origin, const NcnCred& cred)
    : SimpleDerivedType(SimpleDerivedType::LIST, origin, cred)
{
}

List::~List()
{
}

bool List::validate(Schema* schema, const GroveLib::Node* o,
                    const String& source,
                    String* result,
                    bool) const
{
    if (baseType().isNull()) {
        schema->mstream() << XsMessages::invalidBaseType
                          << Message::L_ERROR
                          << DV_ORIGIN(o);
        return false;
    }

    XsType* base = baseType().pointer();
    if (base->typeClass() == XsType::unknown) {
        schema->mstream() << XsMessages::referenceToUndefined
                          << Message::L_ERROR
                          << base->constCred()->format()
                          << DV_ORIGIN(o);
        return false;
    }

    bool valid = false;
    QStringList list = QStringList::split(' ', source.qstring());

    for (uint i = 0; i < list.count(); i++) {
        String token = getTrimmed(schema, o, list[i]);
        list[i] = token.qstring();
    }
    bool pyValid = true;
    bool custom_msg_applied = false;
#ifdef USE_PYTHON
    if (!pyPreFunc().isNull()) {
        for (uint i = 0; i < list.count(); i++) {
            String token = list[i];
            pyValid =  schema->xsi()->
                pyScript()->pyValidate(schema, o, this, pyPreFunc(), token);
            if (!pyValid) {
                schema->mstream() << XsMessages::preScriptReturn
                                  << Message::L_ERROR
                                  << ": " + token
                                  << DV_ORIGIN(o);
                custom_msg_applied = true;
                break;
            }
            list[i] = token.qstring();
        }
    }
#endif // USE_PYTHON

    if (XsType::simpleType == base->typeClass()) {
        for (uint i = 0; i < list.count(); i++) {
            String token = list[i];
            XsSimpleTypeImpl* xsi = base->asXsSimpleTypeImpl();
            if (XsSimpleTypeImpl::ATOMIC == xsi->type()) {
                valid = xsi->asAtomicType()->
                    validate(schema, o, token, *result);
                if (!valid)
                    break;
            }
            else if (XsSimpleTypeImpl::DERIVED == xsi->type()) {
                valid = xsi->asSimpleDerivedType()->
                    validate(schema, o, token, result);
                if (!valid)
                    break;
            }
        }
    }
    else if (XsType::complexType == base->typeClass()) {
        if (SimpleContent* sc =
            base->asXsComplexTypeImpl()->content()->asSimpleContent()) {
            for (uint i = 0; i < list.count(); i++) {
                String token = list[i];
                valid = sc->validate(schema, o, token, result);
                if (!valid)
                    break;
            }
        }
    }

    valid = valid && pyValid;

    if ((!valid) && (checkCustomMessage()) &&  (!custom_msg_applied))
        schema->mstream() << XsMessages::userDefined
                          << Message::L_ERROR
                          << DV_ORIGIN(o);

#ifdef USE_PYTHON
    if (!pyPostFunc().isNull()) {
        for (uint i = 0; i < list.count(); i++) {
            String token = list[i];
            valid =  schema->xsi()->
                pyScript()->pyValidate(schema, o, this, pyPostFunc(), token) && valid;
            if (!valid) {
                schema->mstream() << XsMessages::postScriptReturn
                                  << Message::L_ERROR
                                  << ": " + token << o;
                break;
            }
            list[i] = token.qstring();
        }
    }
#endif // USE_PYTHON

    *result = list.join(" ");

    return valid;
}

bool List::fillPossibleEnums(EnumList& lst)
{
    if (baseType().isNull())
        return false;
    XsType* base = baseType().pointer();
    if (XsType::simpleType == base->typeClass() &&
        XsSimpleTypeImpl::DERIVED == base->asXsSimpleTypeImpl()->type()) {
            base->asXsSimpleTypeImpl()->asSimpleDerivedType()->
                fillPossibleEnums(lst);
                return true;
    }
    return false;
}

void List::dump(int indent) const
{
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++) {
         std::cerr << ' ';
    }
    std::cerr << NOTR("List:");
#endif // XS_DEBUG
}

PRTTI_IMPL(List)

/*****************************************************************************
 * Union                                                                     *
 *****************************************************************************/

Union::Union(const Origin& origin, const NcnCred& cred)
    : SimpleDerivedType(SimpleDerivedType::UNION, origin, cred)
{
}

Union::~Union()
{
}

bool Union::validate(Schema* schema, const GroveLib::Node* o,
                     const String& source,
                     String* result,
                     bool silent) const
{
   //TODO:check for complextype, which base type is simpletype.

    String src =  getTrimmed(schema, o, source);

#ifdef USE_PYTHON
    if (!pyPreFunc().isNull() && schema) {
        *result = src;
        bool valid;
        valid = schema->xsi()->
            pyScript()->pyValidate(schema, o, this, pyPreFunc(), *result);
        if ((!valid) && (!silent))
            schema->mstream() << XsMessages::preScriptReturn
                              << Message::L_ERROR
                              << DV_ORIGIN(o);
        return valid;
    }
#endif //USE_PYTHON

    for (uint i = 0; i < baseVector().size(); i++) {
        if (baseVector()[i].isNull()) {
            if (!silent && schema)
                schema->mstream() << XsMessages::invalidBaseType
                                  << Message::L_ERROR
                                  << DV_ORIGIN(o);
            return false;
        }
        XsType* base = baseVector()[i].pointer();
        if (base->typeClass() == XsType::unknown) {
            if (!silent && schema)
                schema->mstream() << XsMessages::referenceToUndefined
                                  << Message::L_ERROR
                                  << base->constCred()->format()
                                  << DV_ORIGIN(o);
            return false;
        }
        if (XsType::simpleType == base->typeClass()) {
            if (XsSimpleTypeImpl::ATOMIC ==
                baseVector()[i]->asXsSimpleTypeImpl()->type()) {
                if (base->asXsSimpleTypeImpl()->
                    asAtomicType()->validate(0, o, src, *result))
                    return  base->asXsSimpleTypeImpl()->
                        asAtomicType()->validate(schema, o, src, *result);
            }
            else if (XsSimpleTypeImpl::DERIVED ==
                     base->asXsSimpleTypeImpl()->type()) {
                if (base->asXsSimpleTypeImpl()->
                    asSimpleDerivedType()->validate(schema, o, src,
                                                    result, true))
                return base->asXsSimpleTypeImpl()->
                    asSimpleDerivedType()->validate(schema, o, src, result);
            }
        }
    }

    if (!silent && schema)
        schema->mstream() << XsMessages::invalidUnion
                          << Message::L_ERROR
                          << DV_ORIGIN(o);
    return false;
}

bool Union::fillPossibleEnums(EnumList& lst)
{
    bool ret_val = false;
    for (uint i = 0; i < baseVector().size(); i++) {
        bool has_list = false;
        XsType* base = baseVector()[i].pointer();
        if (0 == base)
           continue;
        if (XsType::simpleType == base->typeClass() &&
            XsSimpleTypeImpl::DERIVED == base->asXsSimpleTypeImpl()->type()) {
            has_list = base->asXsSimpleTypeImpl()->asSimpleDerivedType()->
                        fillPossibleEnums(lst);
        }
        if (has_list)
            ret_val = true;
    }
    return ret_val;
/*    std::list<COMMON_NS::String>::iterator it = enumList_.begin();
    for (; it != enumList_.end(); ++it) {
        if (prefix.isEmpty())
            lst.push_back(*it);
        else
            lst.push_back(prefix + "/" + *it);
    }
*/
}

void Union::dump(int indent) const
{
#ifdef XS_DEBUG
    for(int i = 0; i < indent; i++) {
         std::cerr << ' ';
    }
    std::cerr << NOTR("Union:") << std::endl;
//    for(ulong i = 0; i < unionType_.size(); i++)
//        unionType_[i].pointer()->pointer()->dump(indent+1);
#endif // XS_DEBUG
}

PRTTI_IMPL(Union)

XS_NAMESPACE_END
