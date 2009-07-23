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

#ifndef XPATH_BINARY_EXPR_H
#define XPATH_BINARY_EXPR_H

#include "common/String.h"
#include "xpath/xpath_defs.h"
#include "xpath/impl3/ExprImpl.h"

/*!
 */

namespace Xpath {

class XPATH_EXPIMP BinaryExpr : public ExprImpl {
public:
    XPATH_OALLOC(BinaryExpr);
    enum Name {
        FIRSTNAME,
        ADD = FIRSTNAME,
        AND,
        BAR,
        DIV,
        EQ,
        GT_EQ,
        GT,
        LT_EQ,
        LT,
        MOD,
        MUL,
        NOT_EQ,
        OR,
        SUB,
        LASTNAME
    };

    BinaryExpr(const ExprImplPtr& left, const ExprImplPtr& right);

    const ExprImpl& left() const { return *left_; }
    const ExprImpl& right() const { return *right_; }

    virtual Name    name() const = 0;
    Type            type() const { return BINARY; }
    bool            operator==(const ExprImpl& other) const;
    void            dump() const;

    static Name     name(const COMMON_NS::String& strName);

    virtual ExprImplPtr optimize(int opt) { return optimize_binary(opt); }

protected:
    virtual void    makeNodeTestChain(ExprImpl*& /*prev*/);
    ExprImplPtr     optimize_binary(int opt);

protected:
    virtual int     dep_type() const;
    virtual bool    isNumeric() const;

    ExprImplPtr     left_;
    ExprImplPtr     right_;
};

template <int NAME> class BinaryTemplate : public BinaryExpr {
public:
    BinaryTemplate(const ExprImplPtr& left, const ExprImplPtr& right)
        : BinaryExpr(left, right) {}

    virtual Name        name() const { return (Name)NAME; }
    ConstValueImplPtr   eval(const NodeSetItem&, ExprInst&) const;
};

template <int NAME> class OptimizedBinaryTemplate : public BinaryExpr {
public:
    OptimizedBinaryTemplate(const ExprImplPtr& left, const ExprImplPtr& right)
        : BinaryExpr(left, right) {}

    virtual Name        name() const { return (Name)NAME; }
    ConstValueImplPtr   eval(const NodeSetItem&, ExprInst&) const;
    virtual ExprImplPtr optimize(int);
};

typedef BinaryTemplate<BinaryExpr::ADD>   BinaryAddExpr;
typedef BinaryTemplate<BinaryExpr::DIV>   BinaryDivExpr;
typedef BinaryTemplate<BinaryExpr::EQ>    BinaryEqExpr;
typedef BinaryTemplate<BinaryExpr::GT_EQ> BinaryGtEqExpr;
typedef BinaryTemplate<BinaryExpr::GT>    BinaryGtExpr;
typedef BinaryTemplate<BinaryExpr::LT_EQ> BinaryLtEqExpr;
typedef BinaryTemplate<BinaryExpr::LT>    BinaryLtExpr;
typedef BinaryTemplate<BinaryExpr::MOD>   BinaryModExpr;
typedef BinaryTemplate<BinaryExpr::MUL>   BinaryMulExpr;
typedef BinaryTemplate<BinaryExpr::NOT_EQ> BinaryNotEqExpr;
typedef BinaryTemplate<BinaryExpr::SUB>   BinarySubExpr;

typedef OptimizedBinaryTemplate<BinaryExpr::BAR> BinaryBarExpr;
typedef OptimizedBinaryTemplate<BinaryExpr::AND> BinaryAndExpr;
typedef OptimizedBinaryTemplate<BinaryExpr::OR>  BinaryOrExpr;

} // namespace Xpath

#endif // XPATH_BINARY_EXPR_H
