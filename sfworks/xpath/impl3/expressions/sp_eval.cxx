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
#include "xpath/xpath_defs.h"
#include "xpath/NodeSet.h"
#include "xpath/impl3/NodeSetValue.h"
#include "xpath/impl3/debug.h"
#include "xpath/impl3/expressions/PredicateExpr.h"
#include "xpath/impl3/expressions/SlashExpr.h"
#include "xpath/impl3/ExprInst.h"

namespace Xpath {

ConstValueImplPtr
SlashExpr::eval(const NodeSetItem& context, ExprInst& ei) const
{
    ConstValueImplPtr left_val = left_->eval(context, ei);
    if (left_val->type() != Value::NODESET)
        throw Xpath::Exception(XpathMessages::evalFilterInst,
                               ei.exprContext().contextString());
    NodeSet node_set;
    const NodeSetItem* n = left_val->getNodeSet().first();
    for (; n; n = n->next())
        node_set += right_->eval(*n, ei)->getNodeSet();
    return new NodeSetValue(node_set);
}

/////////////////////////////////////////////////////////////////////

ConstValueImplPtr
PredicateExpr::eval(const NodeSetItem& context, ExprInst& ei) const
{
    ConstValueImplPtr left_val = left_->eval(context, ei);
    if (left_val->type() != Value::NODESET)
        throw Xpath::Exception(XpathMessages::evalFilterInst,
                               ei.exprContext().contextString());
    const NodeSet& node_set = left_val->getNodeSet();
    NodeSet result_set;
    for (const NodeSetItem* n = node_set.realFirst(); n; n = n->realNext()) {
        if (NodeSet::isSpecialNode(n->node())) {
            result_set += n->node();
            continue;
        }
        ConstValueImplPtr val = predicate_->eval(*n, ei);
        switch (val->type()) {
            case Value::BOOLEAN:
                if (val->getBool())
                    result_set += n->node();
                break;
            case Value::NUMERIC:
                if (val->getDouble() == n->pos())
                    result_set += n->node();
                break;
            case Value::STRING:
                if (!val->getString().isEmpty())
                    result_set += n->node();
                break;
            case Value::NODESET:
                if (val->getNodeSet().contextSize())
                    result_set += n->node();
                break;
            default:
                break;
        }
    }
    return new NodeSetValue(result_set);
}

} // namespace Xpath


