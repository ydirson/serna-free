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

#ifndef XS_DATA_H_
#define XS_DATA_H_

#include "xs/xs_defs.h"
#include "xs/Component.h"
#include "xs/Piece.h"
#include "xs/XsIdTable.h"
#include "xs/ComponentSpace.h"
#include "xs/NcnCred.h"
#include "grove/grove_defs.h"
#include "grove/Decls.h"
#include "common/RefCntPtr.h"
#include "common/RefCounted.h"
#include "common/String.h"
#include "common/OwnerPtr.h"

class Schema;
class GroveAstParser;

XS_NAMESPACE_BEGIN

class XsDataImpl;

#define DECLARE_XS_OSPACE(type, ln) \
public: \
    typedef ComponentSpace<Xs##type> type##Space; \
    type##Space& ln##Space() { return *ln##Space_; } \
protected: \
    COMMON_NS::OwnerPtr<type##Space> ln##Space_;

/*! A class which contains all reusable Schema components
 */
class XS_EXPIMP XsData : public COMMON_NS::RefCounted<> {
    DECLARE_XS_OSPACE(Type,           type)
    DECLARE_XS_OSPACE(Attribute,      attribute)
    DECLARE_XS_OSPACE(Group,          group)
    DECLARE_XS_OSPACE(AttributeGroup, attributeGroup)
    DECLARE_XS_OSPACE(Element,        element)
    DECLARE_XS_OSPACE(Notation,       notation)
    DECLARE_XS_OSPACE(IdentityConstraint, identityConstraint)

public:
    typedef XsIdTable<Component> ComponentIdTable;
    typedef XsIdTable<Piece>     PieceIdTable;

    ////////////////////////////////////////////

    const Component*        getComponentById(const COMMON_NS::String& id) const;
    const Piece*            getPieceById(const COMMON_NS::String& id) const;

    ComponentSpaceBase*     getOspaceByType(Component::ComponentType id);

    GROVE_NAMESPACE::Element*      schemaRoot() const;
    GROVE_NAMESPACE::Grove*        grove() const;

    const COMMON_NS::String&           targetNsUri() const;
    const COMMON_NS::String&           schemaNs() const;

    NcnCred::Qualification          elemForm() const { return elemForm_; }

    XsData();
    virtual ~XsData();

    XS_OALLOC(XsData);

protected:
    friend class ::GroveAstParser;
    friend class ::Schema;

    GROVE_NAMESPACE::GrovePtr            schemaGrove_;
    COMMON_NS::RefCntPtr<GROVE_NAMESPACE::Element>  schemaRoot_;
    ComponentIdTable    componentIdt_;
    PieceIdTable        pieceIdt_;
    COMMON_NS::String              schemaNs_;
    COMMON_NS::String              targetNs_;
    NcnCred::Qualification         elemForm_;
    NcnCred::Qualification         attrForm_;
};

//////////////////////////////////////////////////////

inline GROVE_NAMESPACE::Grove* XsData::grove() const
{
    return schemaGrove_.pointer();
}

inline const COMMON_NS::String& XsData::targetNsUri() const
{
    return targetNs_;
}

inline const COMMON_NS::String& XsData::schemaNs() const
{
    return schemaNs_;
}

inline GROVE_NAMESPACE::Element* XsData::schemaRoot() const
{
    return schemaRoot_.pointer();
}

XS_NAMESPACE_END

#endif // XS_DATA_H_
