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

#include "xs/XsElement.h"
#include "grove/Nodes.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

class XsElementStub : public XsElement {
public:
    virtual bool validate(Schema* s, GROVE_NAMESPACE::Element* elem, bool) const;

    virtual void makeInstance(Schema* s,
                              GROVE_NAMESPACE::Grove* referenceGrove,
                              GROVE_NAMESPACE::ElementPtr& ep,
                              const GROVE_NAMESPACE::Element* pe = 0) const;

    virtual bool makeContent(Schema* s, GROVE_NAMESPACE::Element* pe, FixupSet* elemSet) const;

    virtual bool makeSkeleton(Schema* s,
                              GROVE_NAMESPACE::Grove* referenceGrove,
                              GROVE_NAMESPACE::ElementPtr& ep,
                              const GROVE_NAMESPACE::Element* pe = 0) const;

    virtual bool isValid() const;
    virtual bool substGroup(GROVE_NAMESPACE::QualifiedName& qname) const;
    virtual void getPossibleEnumlist(Common::PropertyNode* node) const {}
    virtual bool cdataAllowed() const { return true; }
    virtual void dump(int) const {}
    virtual XsType*      xstype() const { return 0; }
    virtual bool isAnyType() const { return false; }

    XsElementStub(const NcnCred& cr);
};

XsElementStub::XsElementStub(const NcnCred& cr)
  : XsElement(Origin(0), cr)
{
}

bool XsElementStub::validate(Schema* s, GROVE_NAMESPACE::Element*, bool) const
{
    return refToUndef(s);
}
bool XsElementStub::makeSkeleton(Schema* s,
                              GROVE_NAMESPACE::Grove*,
                              GROVE_NAMESPACE::ElementPtr&,
                              const GROVE_NAMESPACE::Element*) const
{
    return refToUndef(s);
}

void XsElementStub::makeInstance(Schema* s,
                                 GROVE_NAMESPACE::Grove*,
                                 GROVE_NAMESPACE::ElementPtr&,
                                 const GROVE_NAMESPACE::Element*) const
{
    refToUndef(s);
}

bool XsElementStub::makeContent(Schema* s, GROVE_NAMESPACE::Element*, FixupSet*) const
{
    return refToUndef(s);
}

bool XsElementStub::isValid() const
{
   return false;
}

bool XsElementStub::substGroup(GROVE_NAMESPACE::QualifiedName& qname) const
{
    return true;
}

XsElement* XsElement::makeEmptyStub(const NcnCred& c)
{
    return new XsElementStub(c);
}

XsElement::XsElement(const Origin& origin,
                     const NcnCred& cred)
    : Component(Component::element, origin, cred)
{
}

XsElement::~XsElement()
{
}

PRTTI_IMPL(XsElement);

XS_NAMESPACE_END
