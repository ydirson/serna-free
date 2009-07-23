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

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "xs/xs_defs.h"
#include "xs/Piece.h"
#include "xs/ComponentCred.h"
#include "common/prtti.h"
#include "grove/grove_defs.h"
#include "grove/Decls.h"

class Schema;

XS_NAMESPACE_BEGIN

template <class T> class ComponentInst;

#define XS_FORWARD_DECL(t) \
    class Xs##t; \
    class Xs##t##Impl; \
    typedef ComponentInst<Xs##t> Xs##t##Inst;

XS_FORWARD_DECL(Schema)
XS_FORWARD_DECL(Type)
XS_FORWARD_DECL(Attribute)
XS_FORWARD_DECL(Group)
XS_FORWARD_DECL(AttributeGroup)
XS_FORWARD_DECL(Element)
XS_FORWARD_DECL(Notation)
XS_FORWARD_DECL(IdentityConstraint)

class Annotation;
class XsData;
class XsDataImpl;

/*! Common interface for all Schema components.
 */
class XS_EXPIMP Component : public Piece, public ComponentCred {
public:
    /*! A type of the component
     */
    enum ComponentType {
        unknown, type, attribute, element, group, attributeGroup,
        notation, identityConstraint, MAX
    };

    ComponentType   componentType() const;

    PRTTI_DECL(XsType);
    PRTTI_DECL(XsAttribute);
    PRTTI_DECL(XsGroup);
    PRTTI_DECL(XsAttributeGroup);
    PRTTI_DECL(XsElement);
    PRTTI_DECL(XsNotation);
    PRTTI_DECL(XsIdentityConstraint);

    Component(ComponentType t, const Origin& origin, const NcnCred& cr);

    virtual ~Component();

    virtual void dump(int indent) const  = 0;

    virtual void putIntoTable(Schema* schema, const String& id) const;

    // xs:redefine handling
    void processRedefine(Schema*, Component*);

    XS_OALLOC(Component);

private:
    Component();
    Component(const Component&);
    Component& operator=(const Component&);

    ComponentType componentType_;

protected:
    bool        refToUndef(Schema* s) const;
};

inline Component::ComponentType Component::componentType() const
{
    return componentType_;
}

XS_NAMESPACE_END

#endif // COMPONENT_H_
