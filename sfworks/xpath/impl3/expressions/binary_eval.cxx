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
#include "xpath/xpath_defs.h"
#include "xpath/ExprContext.h"
#include "xpath/NodeSet.h"
#include "xpath/impl3/FpMake.h"
#include "xpath/impl3/debug.h"
#include "xpath/impl3/ValueImpl.h"
#include "xpath/impl3/NumericValue.h"
#include "xpath/impl3/BooleanValue.h"
#include "xpath/impl3/NodeSetValue.h"
#include "xpath/impl3/ExprInst.h"

#include "xpath/impl3/expressions/BinaryExpr.h"
#include "xpath/impl3/expressions/UnaryExpr.h"

#include "common/stack_buffer.h"

#include <algorithm>
#include <set>

namespace Xpath {

template<> ConstValueImplPtr BinaryAddExpr::eval(const NodeSetItem& context,
                                                 ExprInst& ei) const
{
    ConstValueImplPtr left_value = left_->eval(context, ei);
    ConstNumericValuePtr lvalue = static_cast<const NumericValue*>(
        left_value->to(Value::NUMERIC, &ei).pointer());
    if (lvalue->isNan() || lvalue->isInfinity())
        return lvalue.pointer();
    ConstValueImplPtr right_value = right_->eval(context, ei);
    ConstNumericValuePtr rvalue = static_cast<const NumericValue*>(
        right_value->to(Value::NUMERIC, &ei).pointer());
    if (rvalue->isNan() || rvalue->isInfinity())
        return rvalue.pointer();
    return new NumericValue(lvalue->getDouble() + rvalue->getDouble());
}

template<> ConstValueImplPtr BinaryAndExpr::eval(const NodeSetItem& context,
                                                 ExprInst& ei) const
{
    ConstValueImplPtr lvalue =
        left_->eval(context, ei)->to(Value::BOOLEAN, &ei);
    if (!lvalue->getBool()) // here goes the partial evaluation
        return BooleanValue::instance(false);
    ConstValueImplPtr rvalue =
        right_->eval(context, ei)->to(Value::BOOLEAN, &ei);
    return BooleanValue::instance(lvalue->getBool() && rvalue->getBool());
}

class NsiCmp {
public:
    bool operator()(const NodeSetItem* a, const NodeSetItem* b) const
    {
        return a->node() < b->node();
    }
};

#define NSI_ALLOCA(var, n) STACK_BUFFER(var, const NodeSetItem*, n + 1)

template<> ConstValueImplPtr BinaryBarExpr::eval(const NodeSetItem& context,
                                                 ExprInst& ei) const
{
    ConstValueImplPtr left_value  = left_->eval(context, ei);
    ConstValueImplPtr right_value = right_->eval(context, ei);
    NodeSet res_node_set;

    const NodeSetItem* ip = 0;

    if (Value::NODESET != left_value->type() ||
        Value::NODESET != right_value->type())
        throw Xpath::Exception(XpathMessages::evalBinaryInst,
                               ei.exprContext().contextString());
    const NodeSet& ns1 = left_value->getNodeSet();
    const NodeSet& ns2 = right_value->getNodeSet();
    NSI_ALLOCA(nsi1b, ns1.realSize());
    const NodeSetItem** nsi1p = nsi1b;
    const NodeSetItem** nsi1e = nsi1b + ns1.realSize();
    NSI_ALLOCA(nsi2b, ns2.realSize());
    const NodeSetItem** nsi2p = nsi2b;
    const NodeSetItem** nsi2e = nsi2b + ns2.realSize();
    NSI_ALLOCA(outb, ns1.realSize() + ns2.realSize());
    const NodeSetItem** outp  = 0;
    for (ip = ns1.realFirst(); ip; ip = ip->realNext())
        *nsi1p++ = ip;
    for (ip = ns2.realFirst(); ip; ip = ip->realNext())
        *nsi2p++ = ip;
    std::stable_sort(nsi1b, nsi1e, NsiCmp());
    std::stable_sort(nsi2b, nsi2e, NsiCmp());
    outp = std::set_union(nsi1b, nsi1e, nsi2b, nsi2e, outb, NsiCmp());
    for (const NodeSetItem** ipp = outb; ipp < outp; ++ipp)
        res_node_set += (*ipp)->node();
    res_node_set.docOrderSort();
    return new NodeSetValue(res_node_set);
}

template<> ConstValueImplPtr BinaryDivExpr::eval(const NodeSetItem& context,
                                                 ExprInst& ei) const
{
    ConstNumericValuePtr lvalue = static_cast<const NumericValue*>(
        left_->eval(context, ei)->to(Value::NUMERIC, &ei).pointer());
    if (lvalue->isNan() || lvalue->isInfinity())
        return lvalue.pointer();
    ConstNumericValuePtr rvalue = static_cast<const NumericValue*>(
        right_->eval(context, ei)->to(Value::NUMERIC, &ei).pointer());
    if (rvalue->isNan() || rvalue->isInfinity())
        return rvalue.pointer();
    if (0 == rvalue->getDouble()) {
        if (0 == lvalue->getDouble())
            return new NumericValue(getNaN());
        else
            if (0 < lvalue->getDouble())
                return new NumericValue(getInf());
            else
                return new NumericValue(getNegInf());
    }
    else
        return new NumericValue(lvalue->getDouble() / rvalue->getDouble());
}

template<> ConstValueImplPtr BinaryEqExpr::eval(const NodeSetItem& context,
                                                ExprInst& ei) const
{
    return BooleanValue::instance(left_->eval(context, ei)->
        eq(*right_->eval(context, ei), &ei));
}

template<> ConstValueImplPtr BinaryGtEqExpr::eval(const NodeSetItem& context,
                                                  ExprInst& ei) const
{
    return BooleanValue::instance(left_->eval(context, ei)->
        gtEq(*right_->eval(context, ei), &ei));
}

template<> ConstValueImplPtr BinaryGtExpr::eval(const NodeSetItem& context,
                                                ExprInst& ei) const
{
    return BooleanValue::instance(left_->eval(context, ei)->
        gt(*right_->eval(context, ei), &ei));
}

template<> ConstValueImplPtr BinaryLtEqExpr::eval(const NodeSetItem& context,
                                                  ExprInst& ei) const
{
    return BooleanValue::instance(left_->eval(context, ei)->
        ltEq(*right_->eval(context, ei), &ei));
}

template<> ConstValueImplPtr BinaryLtExpr::eval(const NodeSetItem& context,
                                                ExprInst& ei) const
{
    return BooleanValue::instance(left_->eval(context, ei)->
        lt(*right_->eval(context, ei), &ei));
}

template<> ConstValueImplPtr BinaryModExpr::eval(const NodeSetItem& context,
                                                 ExprInst& ei) const
{
    ConstNumericValuePtr lvalue = static_cast<const NumericValue*>(
        left_->eval(context, ei)->to(Value::NUMERIC, &ei).pointer());
    if (lvalue->isNan() || lvalue->isInfinity())
        return lvalue.pointer();
    ConstNumericValuePtr rvalue = static_cast<const NumericValue*>(
        right_->eval(context, ei)->to(Value::NUMERIC, &ei).pointer());
    if (rvalue->isNan() || rvalue->isInfinity())
        return rvalue.pointer();
    return new NumericValue(truncate(lvalue->getDouble()) %
                              truncate(rvalue->getDouble()));
}

template<> ConstValueImplPtr BinaryMulExpr::eval(const NodeSetItem& context,
                                                 ExprInst& ei) const
{
    ConstNumericValuePtr lvalue = static_cast<const NumericValue*>(
        left_->eval(context, ei)->to(Value::NUMERIC, &ei).pointer());
    if (lvalue->isNan() || lvalue->isInfinity())
        return lvalue.pointer();
    ConstNumericValuePtr rvalue = static_cast<const NumericValue*>(
        right_->eval(context, ei)->to(Value::NUMERIC, &ei).pointer());
    if (rvalue->isNan() || rvalue->isInfinity())
        return rvalue.pointer();
    return new NumericValue(lvalue->getDouble() * rvalue->getDouble());
}

template<> ConstValueImplPtr BinaryNotEqExpr::eval(const NodeSetItem& context,
                                                   ExprInst& ei) const
{
    return BooleanValue::instance(left_->eval(context, ei)->
        notEq(*right_->eval(context, ei), &ei));
}

template<> ConstValueImplPtr BinaryOrExpr::eval(const NodeSetItem& context,
                                                ExprInst& ei) const
{
    ConstValueImplPtr lvalue =
        left_->eval(context, ei)->to(Value::BOOLEAN, &ei);
    if (lvalue->getBool())
        return BooleanValue::instance(true);
    ConstValueImplPtr rvalue =
        right_->eval(context, ei)->to(Value::BOOLEAN, &ei);
    return BooleanValue::instance(lvalue->getBool() || rvalue->getBool());
}

template<> ConstValueImplPtr BinarySubExpr::eval(const NodeSetItem& context,
                                                 ExprInst& ei) const
{
    ConstNumericValuePtr lvalue = static_cast<const NumericValue*>(
        left_->eval(context, ei)->to(Value::NUMERIC, &ei).pointer());
    if (lvalue->isNan() || lvalue->isInfinity())
        return lvalue.pointer();
    ConstNumericValuePtr rvalue = static_cast<const NumericValue*>(
        right_->eval(context, ei)->to(Value::NUMERIC, &ei).pointer());
    if (rvalue->isNan() || rvalue->isInfinity())
        return rvalue.pointer();
    return new NumericValue(lvalue->getDouble() - rvalue->getDouble());
}

////////////////////////////////////////////////////////////////////

ConstValueImplPtr UnaryExpr::eval(const NodeSetItem& context,
                                  ExprInst& ei) const
{
    ConstValueImplPtr left_value = left_->eval(context, ei);
    ConstNumericValuePtr val;
    if (left_value->type() == Value::NUMERIC)
        val = static_cast<const NumericValue*>(left_value.pointer());
    else
        throw Xpath::Exception(XpathMessages::evalUnaryInst,
                               ei.exprContext().contextString());
    if (val->isNan())
        return val.pointer();
    if (val->isInfinity())
        if (0 < val->getDouble())
            return new NumericValue(getInf());
        else
            return new NumericValue(getNegInf());
    return new NumericValue(-val->getDouble());
}

} // namespace Xpath
