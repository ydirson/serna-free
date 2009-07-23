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
#include "sapi/grove/GroveEntity.h"
#include "sapi/grove/GroveNodes.h"
#include "sapi/common/Vector.h"
#include "grove/EntityDecl.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityReferenceTable.h"
#include "grove/SectionNodes.h"

#define SELF_ENT static_cast<GroveLib::EntityDecl*>(getRep())

namespace SernaApi {

GroveEntityDecl::GroveEntityDecl(SernaApiBase* base)
    : RefCountedWrappedObject(base)
{
}

SString GroveEntityDecl::name() const
{
    return getRep() ? SString(SELF_ENT->name()) : SString();
}

GroveEntityDecl::DeclType GroveEntityDecl::declType() const
{
    return getRep() ? (DeclType)SELF_ENT->declType() : invalidDeclType;
}

GroveEntityDecl::DataType GroveEntityDecl::dataType() const
{
    return getRep() ? (DataType)SELF_ENT->dataType() : invalidDataType;
}

GroveEntityDecl::DeclOrigin GroveEntityDecl::declOrigin() const
{
    if (0 == getRep())
        return invalidDeclOrigin;
    switch (SELF_ENT->declOrigin()) {
        case GroveLib::EntityDecl::prolog: return prolog;
        case GroveLib::EntityDecl::dtd:    return dtd;
        case GroveLib::EntityDecl::schema: return schema;
        case GroveLib::EntityDecl::sd:     return sd;
        case GroveLib::EntityDecl::special:return special;
        default: return invalidDeclOrigin;
    }
}

bool GroveEntityDecl::isReadOnly() const
{
    return getRep() ? SELF_ENT->isReadOnly() : false;
}

GroveInternalEntityDecl GroveEntityDecl::asGroveInternalEntityDecl() const
{
    if (!getRep() ||
        (SELF_ENT->declType() != GroveLib::EntityDecl::internalGeneralEntity &&
         SELF_ENT->declType() != GroveLib::EntityDecl::internalParameterEntity))
            return 0;
    return getRep();
}

GroveExternalEntityDecl GroveEntityDecl::asGroveExternalEntityDecl() const
{
    if (!getRep() ||
        (SELF_ENT->declType() != GroveLib::EntityDecl::externalGeneralEntity &&
         SELF_ENT->declType() != GroveLib::EntityDecl::externalParameterEntity))
            return 0;
    return getRep();
}

GroveXincludeDecl GroveEntityDecl::asGroveXincludeDecl() const
{
    if (!getRep() || SELF_ENT->declType() != GroveLib::EntityDecl::xinclude)
        return 0;
    return getRep();
}

GroveNotationDecl GroveEntityDecl::asGroveNotationDecl() const
{
    if (!getRep() || SELF_ENT->declType() != GroveLib::EntityDecl::notation)
        return 0;
    return getRep();
}

////////////////////////////////////////////////////////////////////////

#define SELF_IENT static_cast<GroveLib::InternalEntityDecl*>(getRep())

GroveInternalEntityDecl::GroveInternalEntityDecl(SernaApiBase* base)
    : GroveEntityDecl(base)
{
}
    
SString GroveInternalEntityDecl::content() const
{
    return getRep()
        ? SString(SELF_IENT->content())
        : SString();
}

#define SELF_EENT static_cast<GroveLib::ExternalEntityDecl*>(getRep())

GroveExternalEntityDecl::GroveExternalEntityDecl(SernaApiBase* base)
    : GroveEntityDecl(base)
{
}
    
SString GroveExternalEntityDecl::sysid() const
{
    return getRep() ? SString(SELF_EENT->sysid()) : SString();
}

SString GroveExternalEntityDecl::encoding() const
{
    return getRep() ? SString(Common::Encodings::encodingName(
                                  SELF_EENT->encoding())) : SString();
}

SString GroveExternalEntityDecl::notationName() const
{
    return getRep() ? SString(SELF_EENT->notationName()) : SString();
}

GroveXincludeDecl::GroveXincludeDecl(SernaApiBase* base)
    : GroveEntityDecl(base)
{
}
    
#define SELF_XENT static_cast<GroveLib::XincludeDecl*>(getRep())
    
SString GroveXincludeDecl::url() const
{
    return getRep() ? SString(SELF_XENT->url()) : SString();
}
    
SString GroveXincludeDecl::expr() const
{
    return getRep() ? SString(SELF_XENT->expr()) : SString();
}

bool GroveXincludeDecl::isFallback() const
{
    return getRep() ? SELF_XENT->isFallback() : false;
}

#define SELF_NOTDECL static_cast<GroveLib::Notation*>(getRep())

GroveNotationDecl::GroveNotationDecl(SernaApiBase* base)
    : GroveEntityDecl(base)
{
}
SString GroveNotationDecl::sysid() const
{
    return getRep() ? SString(SELF_NOTDECL->sysid()) : SString();
}

////////////////////////////////////////////////////////////////////////

#define SELF_ENTSET static_cast<GroveLib::EntityDeclSet*>(getRep())
#define SELF_ENTITER static_cast<EntityDeclIteratorImpl*>(getRep())

namespace {

class EntityDeclIteratorImpl : public Common::SernaApiRefCounted {
public:
    EntityDeclIteratorImpl(GroveLib::EntityDeclSet& eds)
        : eds_(eds), it_(eds_.begin()) {}
    GroveLib::EntityDeclSet& eds_;
    GroveLib::EntityDeclSet::iterator it_;
};

} // namespace

GroveEntityDeclSet::GroveEntityDeclSet(SernaApiBase* base)
    : SimpleWrappedObject(base)
{
}

GroveEntityDecl GroveEntityDeclSet::lookupDecl(const SString& name) const
{
    if (!getRep())
        return 0;
    return SELF_ENTSET->lookupDecl(name);
}

GroveEntityDeclSet::Iterator GroveEntityDeclSet::first() const
{
    if (!getRep())
        return 0;
    if (SELF_ENTSET->begin() == SELF_ENTSET->end())
        return 0;
    return new EntityDeclIteratorImpl(*SELF_ENTSET);
}

GroveEntityDeclSet::Iterator::Iterator(SernaApiBase* base)
    : RefCountedWrappedObject(base)
{
}

bool GroveEntityDeclSet::Iterator::ok() const
{
    return (0 != getRep() && SELF_ENTITER->it_ != SELF_ENTITER->eds_.end());
}

GroveEntityDecl GroveEntityDeclSet::Iterator::next() 
{
    if (!ok())
        return 0;
    GroveEntityDecl decl(SELF_ENTITER->it_->pointer());
    ++SELF_ENTITER->it_;
    return decl;
}

////////////////////////////////////////////////////////////////

#define SELF_ERT static_cast<GroveLib::EntityReferenceTable*>(getRep())
#define SELF_ERTENT \
    static_cast<GroveLib::EntityReferenceTable::ErtEntry*>(getRep())

GroveEntityReferenceTable::GroveEntityReferenceTable(SernaApiBase* base)
    : SimpleWrappedObject(base)
{
}

GroveEntityReferenceTable::ErtEntry 
GroveEntityReferenceTable::lookupErs(const GroveEntityDecl& ed) const
{
    if (!getRep() || !ed.getRep())
        return 0;
    return const_cast<GroveLib::EntityReferenceTable::ErtEntry*>
        (SELF_ERT->lookup(static_cast<GroveLib::EntityDecl*>(ed.getRep())));
}

GroveEntityReferenceTable::ErtEntry::ErtEntry(SernaApiBase* rep)
    : SimpleWrappedObject(rep)
{
}

unsigned int GroveEntityReferenceTable::ErtEntry::numOfRefs() const
{
    return getRep() ? SELF_ERTENT->numOfRefs() : 0;
}
        
GroveErs GroveEntityReferenceTable::ErtEntry::node(unsigned int index) const
{
    return getRep() ? SELF_ERTENT->node(index) : 0;
}

} // namespace SernaApi
