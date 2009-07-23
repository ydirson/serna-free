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

#ifndef XPATH_AXIS_EXPR_H
#define XPATH_AXIS_EXPR_H

#include "common/String.h"
#include "xpath/xpath_defs.h"
#include "xpath/impl3/ExprImpl.h"
#include "xpath/impl3/expressions/NodeTestExpr.h"

namespace Xpath {

class NodeTestExpr;
class AxisExpr;
class ExprInst;

typedef COMMON_NS::RefCntPtr<AxisExpr> AxisExprPtr;

class XPATH_EXPIMP AxisExpr : public ExprImpl {
public:
    XPATH_OALLOC(AxisExpr);
    enum Name {
        FIRSTNAME,
        ANCESTOR = FIRSTNAME,
        ANCESTOR_OR_SELF,
        ATTRIBUTE,
        CHILD,
        DESCENDANT,
        DESCENDANT_OR_SELF,
        FOLLOWING,
        FOLLOWING_SIBLING,
        NAMESPACE,
        PARENT,
        PRECEDING,
        PRECEDING_SIBLING,
        ROOT,
        SELF,
        LASTNAME
    };

    AxisExpr(const NodeTestExprPtr& nodeTestExpr);

    virtual Name    name() const = 0;
    Type            type() const { return AXIS; }
    bool            operator==(const ExprImpl& other) const;
    void            dump() const;
    NodeTestExpr*   nodeTestExpr() const { return nodeTestExpr_.pointer(); }
    double          priority() const;

    static Name     toName(const COMMON_NS::String& strName);
    virtual int     dep_type() const { return NO_DEP; }

    static ExprImplPtr make(const COMMON_NS::String& name,
                            const NodeTestExprPtr& nt);
    static ExprImplPtr make(Name name,
                            const NodeTestExprPtr& nt);

    // combined node-tests composition
    AxisExprPtr     joinOr(const AxisExprPtr& other);
    AxisExprPtr     joinAnd(const AxisExprPtr& other);
    AxisExprPtr     joinNot();

    bool            nodeTest(const GroveLib::Node* n) const;

protected:
    virtual void    makeNodeTestChain(ExprImpl*&);

    void    collectDescendants(ExprInst& ei,
                               NodeSet& nodeSet,
                               GroveLib::Node* node) const;
    void    collectFollowings(ExprInst& ei, NodeSet& nodeSet,
                              GroveLib::Node* parent,
                              GroveLib::Node* child) const;
    void    collectPrecedings(ExprInst& ei, NodeSet& nodeSet,
                              GroveLib::Node* parent,
                              GroveLib::Node* child) const;
    void    check_same_axis(const AxisExprPtr& other) const;

    NodeTestExprPtr nodeTestExpr_;
};

template <int NAME> class  XPATH_EXPIMP AxisTemplate : public AxisExpr {
public:
    AxisTemplate(const NodeTestExprPtr& nodeTestExpr = 0)
        : AxisExpr(nodeTestExpr) {}

    virtual Name    name() const { return (Name)NAME; }
    virtual ConstValueImplPtr eval(const NodeSetItem&, ExprInst&) const;
};

typedef AxisTemplate<AxisExpr::ANCESTOR> AxisAncestorExpr;
typedef AxisTemplate<AxisExpr::ANCESTOR_OR_SELF> AxisAosExpr;
typedef AxisTemplate<AxisExpr::ATTRIBUTE> AxisAttributeExpr;
typedef AxisTemplate<AxisExpr::CHILD>    AxisChildExpr;
typedef AxisTemplate<AxisExpr::DESCENDANT> AxisDescendantExpr;
typedef AxisTemplate<AxisExpr::DESCENDANT_OR_SELF> AxisDosExpr;
typedef AxisTemplate<AxisExpr::FOLLOWING> AxisFollowingExpr;
typedef AxisTemplate<AxisExpr::FOLLOWING_SIBLING> AxisFsExpr;
typedef AxisTemplate<AxisExpr::PARENT> AxisParentExpr;
typedef AxisTemplate<AxisExpr::PRECEDING> AxisPrecedingExpr;
typedef AxisTemplate<AxisExpr::PRECEDING_SIBLING> AxisPsExpr;
typedef AxisTemplate<AxisExpr::ROOT> AxisRootExpr;
typedef AxisTemplate<AxisExpr::SELF> AxisSelfExpr;

} // namespace Xpath

#endif // XPATH_AXIS_EXPR_H
