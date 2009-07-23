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

#include "xslt/xslt_defs.h"
#ifdef USE_XPATH3
#include "grove/Nodes.h"
#include "xpath/ExprContext.h"
#include "xpath/NodeSet.h"
#include "xslt/impl/xpath_values.h"
#include "xslt/impl/xpath_exprs.h"
#include "xslt/impl/xpath_exprinst.h"
#include "xslt/impl/debug.h"
#include "xslt/impl/patterns.h"

USING_COMMON_NS;

namespace Xslt {

Xpath::ConstValueImplPtr PatternExprImpl::eval(const Xpath::NodeSetItem& nsi,
                                               Xpath::ExprInst& ei) const
{
    return Xpath::BooleanValue::instance(eval(nsi,
        static_cast<PatternInst&>(ei)));
}

Xpath::ValueHolder*
PatternExprImpl::makeInst(const Xpath::NodeSetItem& nsi,
                          const Xpath::ExprContext& ec) const
{
    return new PatternInst(*this, nsi, ec);
}

void AltPatternExpr::addAlternative(const PatternExprImplPtr& p)
{
    alternatives_.push_back(p);
}

double AltPatternExpr::priority() const
{
    if (alternatives_.size() == 0)
        return 0;
    double maxPrio = alternatives_[0]->priority();
    for (uint i = 1; i < alternatives_.size(); ++i)
        if (alternatives_[i]->priority() > maxPrio)
            maxPrio = alternatives_[i]->priority();
    return maxPrio;
}

bool AltPatternExpr::isApplicable(const GroveLib::Node* n) const
{
    for (uint i = 0; i < alternatives_.size(); ++i)
        if (alternatives_[i]->isApplicable(n))
            return true;
    return false;
}

bool AltPatternExpr::operator==(const Xpath::ExprImpl& other) const
{
    if (other.type() == UNKNOWN_EXPR) {
        const AltPatternExpr* pattern =
            dynamic_cast<const AltPatternExpr*>(&other);
        if (pattern) {
            if (pattern->alternatives().size() != alternatives_.size())
                return false;
            for (size_t i = 0; i < alternatives_.size(); i++)
                if (!(*pattern->alternatives()[i].pointer() ==
                    *alternatives_[i].pointer()))
                        return false;
            return true;
        }
    }
    return false;
}

void AltPatternExpr::dump() const
{
    DDINDENT;
    DDBG << "AltPatternExpr:" << std::endl;
    for (size_t i = 0; i < alternatives_.size(); i++) {
        DDBG << "alternative N" << i << std::endl;
        alternatives_[i]->dump();
    }
}

bool AltPatternExpr::eval(const Xpath::NodeSetItem& nsi, PatternInst& ei) const
{
    if (0 == alternatives_.size())
        return false;
    const PatternExprImpl* bestExpr = 0;
    double max_priority = 0;
    bool first_priority = true;
    for (size_t c = 0; c < alternatives_.size(); ++c) {
        if (alternatives_[c]->eval(nsi, ei)) {
            // if alternative matched - compare its prority with the highest one
            double alt_priority = alternatives_[c]->priority();
            if (first_priority || alt_priority >= max_priority) {
                max_priority = alt_priority;
                bestExpr = alternatives_[c].pointer();
                first_priority = false;
            }
        }
    }
    if (bestExpr)
        ei.setMatchedExpr(bestExpr);
    return !!bestExpr; // something matched
}

/////////////////////////////////////////////////////////

bool RootPatternExpr::operator==(const Xpath::ExprImpl& other) const
{
    if (other.type() == UNKNOWN_EXPR) {
        const RootPatternExpr* root =
            dynamic_cast<const RootPatternExpr*>(&other);
        return (0 != root);
    }
    return false;
}

void RootPatternExpr::dump() const
{
    DDBG << "RootPatternExpr:" << std::endl;
}

bool RootPatternExpr::isApplicable(const GroveLib::Node* n) const
{
    return !n->parent();
}

bool RootPatternExpr::eval(const Xpath::NodeSetItem& nsi, PatternInst&) const
{
    return !nsi.node()->parent();
}

///////////////////////////////////////////////////////////

SlashPatternExpr::SlashPatternExpr(const PatternExprImplPtr& left,
                                   const PatternExprImplPtr& right,
                                   bool  parent_only)
    : left_(left),
      right_(right),
      parentOnly_(parent_only)
{
}

bool SlashPatternExpr::operator==(const Xpath::ExprImpl& other) const
{
    if (other.type() == UNKNOWN_EXPR) {
        const SlashPatternExpr* slash =
            dynamic_cast<const SlashPatternExpr*>(&other);
        if (slash)
            return (slash->left() == *left_ &&
                    slash->right() == *right_ &&
                    slash->parentOnly() == parentOnly_);
    }
    return false;
}

bool SlashPatternExpr::isApplicable(const GroveLib::Node* n) const
{
    if (!parentOnly())
        return right_->isApplicable(n);
    if (!n->parent())
        return false;
    return right_->isApplicable(n) &&
        left_->isApplicable(parentNode(n));
}

void SlashPatternExpr::dump() const
{
    DDINDENT;
    DDBG << "SlashPatternExpr: mode "
        << ((parentOnly_) ? "/" : "//") << std::endl;
    DDBG << "left:" << std::endl;
    left_->dump();
    DDBG << "right:" << std::endl;
    right_->dump();
}

/*!
  Slash pattern evaluates to TRUE only if it`s left and right child expressions
  are evaluated to TRUE. The context for the right sub-expr is the same as for
  slash pattern itself. The left sub-expr is instantiated for the parent node of
  the context node and for it`s parent up to root. Instantiation of the left
  sub-expr stops when the value of the instance is TRUE.
*/

bool SlashPatternExpr::eval(const Xpath::NodeSetItem& nsi,
                            PatternInst& ei) const
{
    if (!right_->eval(nsi, ei))
        return false;
    Xpath::NodeSetItem node_nsi(parentNode(nsi.node()));
    if (parentOnly()) 
        return left_->eval(node_nsi, ei);
    while (node_nsi.node()) {
        if (left_->eval(node_nsi, ei))
            return true;
        node_nsi.setNode(parentNode(node_nsi.node()));
    }
    return false;
}

////////////////////////////////////////////////////////////

StepPatternExpr::StepPatternExpr(const Xpath::ExprImplPtr& step,
                                 double priority,
                                 const Xpath::NodeTestExpr* nte,
                                 Xpath::AxisExpr::Name axisid)
    : step_(step),
      priority_(priority),
      nodeTest_(nte),
      axisid_(axisid)
{
}

bool StepPatternExpr::operator==(const Xpath::ExprImpl& other) const
{
    if (other.type() == UNKNOWN_EXPR) {
        const StepPatternExpr* step =
            dynamic_cast<const StepPatternExpr*>(&other);
        if (step)
            return (step->step() == *step_);
    }
    return false;
}

bool StepPatternExpr::isApplicable(const GroveLib::Node* n) const
{
    const GroveLib::Node::NodeType nt = n->nodeType();
    if (nt == GroveLib::Node::CHOICE_NODE) {
        if (0 == nodeTest_)
            return false;
        if (nodeTest_->name() == Xpath::NodeTestExpr::NODE_TYPE_TEST &&
            nodeTest_->nodeTest(n))
                return true;
        return false;
    }
    switch (axisid_) {
        case Xpath::AxisExpr::SELF:
        case Xpath::AxisExpr::CHILD:
            if (0 == nodeTest_)
                return true;
            return nt != GroveLib::Node::ATTRIBUTE_NODE &&
                nodeTest_->nodeTest(n);
        case Xpath::AxisExpr::ATTRIBUTE:
            if (0 == nodeTest_)
                return true;
            return nt == GroveLib::Node::ATTRIBUTE_NODE
                && nodeTest_->nodeTest(n);
        default:
            return true;
    }
}

void StepPatternExpr::dump() const
{
    DDINDENT;
    DDBG << "StepPatternExpr: priority=" << priority_ << std::endl;
    DDBG << "step:" << std::endl;
    step_->dump();
}

bool StepPatternExpr::eval(const Xpath::NodeSetItem& nsi, PatternInst& ei) const
{
    GroveLib::Node* node;
    if (axis() == Xpath::AxisExpr::SELF ||
        axis() == Xpath::AxisExpr::ATTRIBUTE)
            node = nsi.node();
    else
        node = parentNode(nsi.node());
    if (0 == node)
        return false;
    Xpath::NodeSetItem node_nsi(node);
    Xpath::ConstValueImplPtr stepValue = step_->eval(node_nsi, ei);
    const Xpath::NodeSet& node_set = stepValue->getNodeSet();
    const Xpath::NodeSetItem* n = node_set.first();
    for (; n; n = n->next())
        if (nsi.node() == n->node())
            return true;
    return false;
}

} // namespace Xslt

#endif // USE_XPATH3
