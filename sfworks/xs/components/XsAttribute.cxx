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

#include "xs/XsAttribute.h"
#include "grove/Nodes.h"
#include "common/PropertyTree.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

class XsAttributeStub : public XsAttribute {
public:
    virtual bool    validate(Schema* s, GROVE_NAMESPACE::Attr* attr) const;

    virtual String  skeletonValue(Schema* s) const;

    virtual bool    createAttr(Schema* s,
                               GROVE_NAMESPACE::Element*,
                               RefCntPtr<GROVE_NAMESPACE::Attr>& a) const;
    virtual XsType* xstype() const {return 0;}
    virtual const XsAttribute* refAttr() const { return 0; }
    virtual Common::PropertyNode* makeSpec(const GroveLib::Element*) const 
        { return new PropertyNode(constCred()->name(), constCred()->xmlns()); }
    virtual void    getPossibleEnumlist(EnumList&) const {}
    virtual void    dump(int) const {}
    virtual AttributeForm   attributeForm() const { return NONE; }
    virtual AttributeUse    attributeUse() const { return A_OPTIONAL; }
    virtual const String&   defValue() const { return String::null(); }
    virtual const String&   fixValue() const { return String::null(); }

    XsAttributeStub(const NcnCred& c);
};

XsAttributeStub::XsAttributeStub(const NcnCred& c)
  : XsAttribute(Origin(0), c)
{
}

bool XsAttributeStub::validate(Schema* s, GROVE_NAMESPACE::Attr*) const
{
    return refToUndef(s);
}

bool XsAttributeStub::createAttr(Schema* s,
                                 GROVE_NAMESPACE::Element*,
                                 RefCntPtr<GROVE_NAMESPACE::Attr>&) const
{
    return refToUndef(s);
}

String XsAttributeStub::skeletonValue(Schema* s) const
{
    refToUndef(s);
    return "";
}

XsAttribute* XsAttribute::makeEmptyStub(const NcnCred& cr)
{
    return new XsAttributeStub(cr);
}

XsAttribute::XsAttribute(const Origin& origin,
                         const NcnCred& cred)
    : Component(Component::attribute, origin, cred)
{}

XsAttribute::~XsAttribute()
{}

PRTTI_IMPL(XsAttribute)

XS_NAMESPACE_END
