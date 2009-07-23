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

#include "xs/XsType.h"
#include "xs/ComponentRef.h"
#include "xs/ComponentSpace.h"
#include "xs/Schema.h"
#include "xs/XsDataImpl.h"
#include "xs/complex/Particle.h"
#include "xs/complex/SimpleContent.h"
#include "xs/complex/XsComplexTypeImpl.h"
#include "datatypes/DerivationTypes.h"
#include "grove/Nodes.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

const XsSimpleTypeImpl* XsType::getSimpleType() const
{
    const XsSimpleTypeImpl* xsti = asConstXsSimpleTypeImpl();
    if (!xsti) {
        const XsComplexTypeImpl* xcti = asConstXsComplexTypeImpl();
        if (xcti && xcti->content()) {
            const SimpleContent* sc = xcti->content()->asConstSimpleContent();
            if (sc)
                xsti = sc->restriction();
        }
    }
    return xsti;
}

class XsTypeStub : public XsType {
public:
    virtual bool    validate(Schema* s, GROVE_NAMESPACE::Element*, bool) const;

    virtual bool    validate(Schema* schema,
                             const String& source,
                             String* result = 0) const;

    virtual bool    makeContent(Schema* schema,
                                GROVE_NAMESPACE::Element* parentElem,
                                FixupSet* elemSet,
                                const String* defv = 0) const;

    virtual void    makeAttrs(Schema* schema,
                              GROVE_NAMESPACE::Element* elem) const;
    virtual bool    cdataAllowed() const { return true; }

    virtual void    dump(int) const {}

    XsTypeStub(const NcnCred&);
};

XsTypeStub::XsTypeStub(const NcnCred& cr)
  : XsType(XsType::unknown, Origin(0), cr)
{
}

bool XsTypeStub::validate(Schema* s, GROVE_NAMESPACE::Element*, bool) const
{
    return refToUndef(s);
}

bool XsTypeStub::validate(Schema* s, const String&, String*) const
{
    return refToUndef(s);
}

bool XsTypeStub::makeContent(Schema* schema,
                             GROVE_NAMESPACE::Element*, FixupSet*,
                             const String*) const
{
    return refToUndef(schema);
}

void XsTypeStub::makeAttrs(Schema* schema, GROVE_NAMESPACE::Element*) const
{
    refToUndef(schema);
}

XsType* XsType::makeEmptyStub(const NcnCred& cr)
{
    return new XsTypeStub(cr);
}

XsType::XsType(TypeClass tclass,
               const Origin& origin,
               const NcnCred& cred)
    : Component(Component::type, origin, cred),
      typeClass_(tclass)
{
}

XsType::~XsType()
{
}

PRTTI_BASE_STUB(XsType, XsSimpleTypeImpl)
PRTTI_BASE_STUB(XsType, XsComplexTypeImpl)

XS_NAMESPACE_END
