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
#include "xs/XsIdentityConstraint.h"
#include "grove/Nodes.h"
#include "xs/components/XsUniqueImpl.h"
#include "xs/components/XsKeyImpl.h"
#include "xs/components/XsKeyRefImpl.h"

#ifndef NO_XPATH
#include "xpath/ExprContext.h"
#include "xpath/Engine.h"
#include "xpath/ValueHolder.h"
#include "xpath/NodeSet.h"
#endif  //NO_XPATH

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

class XsIdentityConstraintStub : public XsIdentityConstraint {
public:
    virtual bool    validate(Schema* s, GROVE_NAMESPACE::Element*) const;

    virtual void    dump(int) const {}

    XsIdentityConstraintStub(const NcnCred& cr);
};

XsIdentityConstraintStub::XsIdentityConstraintStub(const NcnCred& c)
  : XsIdentityConstraint(XsIdentityConstraint::unknown, Origin(0), c)
{
}

bool XsIdentityConstraintStub::validate(Schema* s, GROVE_NAMESPACE::Element*) const
{
    return refToUndef(s);
}

XsIdentityConstraint* XsIdentityConstraint::makeEmptyStub(const NcnCred& c)
{
    return new XsIdentityConstraintStub(c);
}

#ifndef NO_XPATH
RefCntPtr<const XPATH_NAMESPACE::Value>
    XsIdentityConstraint::evaluate(String& exprstr,
                                   GROVE_NAMESPACE::Element* elem) const
{
    RefCntPtr<Xpath::Expr> expr;
    try {
        expr = XPATH_NAMESPACE::Engine::makeExpr(exprstr, origin());
    }
    catch (...) {
        return 0;
    }
    XPATH_NAMESPACE::NodeSetItem nsi(elem);
    XPATH_NAMESPACE::ValueHolderPtr  value;
    try {
        value = expr->makeInst(nsi, *this);
    }
    catch (...) {
        return 0;
    }
    return value->value();
}
#endif  //NO_XPATH

XsIdentityConstraint::XsIdentityConstraint(TypeClass tclass,
                                           const Origin& origin,
                                           const NcnCred& cred)
    : Component(Component::identityConstraint, origin, cred),
      typeClass_(tclass)
{
}

XsIdentityConstraint::~XsIdentityConstraint()
{
}

PRTTI_BASE_STUB(XsIdentityConstraint, XsKeyImpl)
PRTTI_BASE_STUB(XsIdentityConstraint, XsKeyRefImpl)
PRTTI_BASE_STUB(XsIdentityConstraint, XsUniqueImpl)

XS_NAMESPACE_END
