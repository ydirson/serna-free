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
#ifndef USE_XPATH3
#include "grove/Nodes.h"
#include "xpath/ExprContext.h"
#include "xpath/NodeSet.h"
#include "xslt/impl/xpath_values.h"
#include "xslt/impl/xpath_exprinst.h"
#include "xslt/impl/xpath_exprs.h"
#include "xslt/impl/debug.h"
#include "xslt/impl/patterns.h"

USING_COMMON_NS;

namespace Xslt {

double PatternInst::matchPriority() const
{
    return static_cast<const PatternExprImpl&>(expr()).priority();
}

const PatternExprImpl* PatternInst::matchedAltExpr() const
{
    return &static_cast<const PatternExprImpl&>(expr());
}

AltPatternExpr::AltPatternExpr()
{
}

void AltPatternExpr::addAlternative(const PatternExprPtr& p)
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

bool AltPatternExpr::operator==(const Xpath::Expr& other) const
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

////////////////////////////////////////////////////////


AltPatternInst::AltPatternInst(const AltPatternExpr& expr,
                               const Xpath::NodeSetItem& nsi,
                               const Xpath::ExprContext& context)
    : PatternInst(expr, nsi, context),
      altExpr_(expr),
      alts_(altExpr_.alternatives().size()),
      matchPriority_(0)
{
    for (size_t c = 0; c < alts_.size(); c++)
        alts_[c].setWatcher(this);
    eval();
}

void AltPatternInst::eval()
{
    for (size_t c = 0; c < altExpr_.alternatives().size(); c++)
        if (alts_[c].isNull())
            alts_[c] = altExpr_.alternatives()[c]->
                makeInst(context(), exprContext());

    double max_priority = 0;
    bool first_priority = true;
    bool matched = false;
    for (size_t c1 = 0; c1 < altExpr_.alternatives().size(); ++c1) {
        if (alts_[c1]->value()->getBool()) {
            // if alternative matched - compare its prority with the highest one
            double alt_priority = altExpr_.alternatives()[c1]->priority();
            if (first_priority || (alt_priority > max_priority)) {
                max_priority = alt_priority;
                first_priority = false;
            }
            matched = true;
        }
    }
    if (matched) {
        matchPriority_ = max_priority;
        setValue(Xpath::BooleanValue::instance(true));
    } else
        setValue(Xpath::BooleanValue::instance(false));
}

double AltPatternInst::matchPriority() const
{
    return matchPriority_;
}

const PatternExprImpl* AltPatternInst::matchedAltExpr() const
{
    for (size_t c = 0; c < altExpr_.alternatives().size(); ++c) {
        if (alts_[c]->value()->getBool()) {
            const Xpath::ExprInst& pi =
                static_cast<const Xpath::ExprInst&>(*alts_[c].pointer());
            return &static_cast<const PatternExprImpl&>(pi.expr());
        }
    }
    return 0;
}

Xpath::ValueHolder*
AltPatternExpr::makeInst(const Xpath::NodeSetItem& nsi,
                         const Xpath::ExprContext& context) const
{
    return new AltPatternInst(*this, nsi, context);
}

/////////////////////////////////////////////////////////

bool RootPatternExpr::operator==(const Xpath::Expr& other) const
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

//////////////////////////////////////////////////////////

RootPatternInst::RootPatternInst(const RootPatternExpr& expr,
                                 const Xpath::NodeSetItem& nsi,
                                 const Xpath::ExprContext& context)
    : PatternInst(expr, nsi, context),
      expr_(expr)
{
    eval();
}

void RootPatternInst::eval()
{
    setValue(Xpath::BooleanValue::instance(!context().node()->parent()));
}

Xpath::ValueHolder*
RootPatternExpr::makeInst(const Xpath::NodeSetItem& nsi,
                          const Xpath::ExprContext& context) const
{
    return new RootPatternInst(*this, nsi, context);
}

///////////////////////////////////////////////////////////

SlashPatternExpr::SlashPatternExpr(const PatternExprPtr& left,
                                   const PatternExprPtr& right,
                                   bool  parent_only)
    : left_(left),
      right_(right),
      parentOnly_(parent_only)
{
}

bool SlashPatternExpr::operator==(const Xpath::Expr& other) const
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

//////////////////////////////////////////////////////

SlashPatternInst::SlashPatternInst(const SlashPatternExpr& expr,
                                   const Xpath::NodeSetItem& nsi,
                                   const Xpath::ExprContext& context)
    : PatternInst(expr, nsi, context),
      slashExpr_(expr),
      right_(this, 0)
{
    eval();
}

SlashPatternInst::~SlashPatternInst()
{
}

/*!
  Slash pattern evaluates to TRUE only if it`s left and right child expressions
  are evaluated to TRUE. The context for the right sub-expr is the same as for
  slash pattern itself. The left sub-expr is instantiated for the parent node of
  the context node and for it`s parent up to root. Instantiation of the left sub-expr
  stops when the value of the instance is TRUE.
*/
// OPTIMIZE: optimize case for simple slash-patterns (ones without predicates).
// In this case, we can omit instantiation of leftList.
void SlashPatternInst::eval()
{
    if (right_.isNull())
        right_ = slashExpr_.right().makeInst(context(), exprContext());

    // std::cerr << "right val: \n"; right_->value()->dump(0);
    // std::cerr << "-- right val end\n";

    // if right instance`s value is TRUE then evaluate left part
    if (right_->value()->getBool()) {
        // First look through already instantiated list
        if (leftList_.first()) {
            const Xpath::NodeSetItem* n = leftList_.first();
            for (; n; n = n->next()) {
                // std::cerr << "left val: \n"; right_->value()->dump(0);
                // std::cerr << "-- left val end\n";
                if (n->value()->getBool()) {
                    setValue(Xpath::BooleanValue::instance(true));
                    return;
                }
            }
        }
        // If failed - try to instantiate more instances
        for (;;) {
            GroveLib::Node* node = 0; // Find the context node
            if (!leftList_.first())
                node = parentNode(context().node());
            else
                // If ancestors are allowed then take one
                if (!slashExpr_.parentOnly())
                    node = parentNode(leftList_.realLast()->node());

            if (0 == node) {
                setValue(Xpath::BooleanValue::instance(false));
                return;
            }
            leftList_ += node;
            leftList_.realLast()->setPos(1);
            leftList_.realLast()->setValuePtr(slashExpr_.left().makeInst
                (*leftList_.realLast(), exprContext()), this);
            // If evaluated to TRUE then stop evaluation
            if (leftList_.realLast()->value()->getBool()) {
                setValue(Xpath::BooleanValue::instance(true));
                return;
            }
        }
    }
    else
        setValue(Xpath::BooleanValue::instance(false));
}

Xpath::ValueHolder*
SlashPatternExpr::makeInst(const Xpath::NodeSetItem& nsi,
                           const Xpath::ExprContext& context) const
{
    return new SlashPatternInst(*this, nsi, context);
}

////////////////////////////////////////////////////////////

StepPatternExpr::StepPatternExpr(const Xpath::ExprPtr& step,
                                 double priority,
                                 const Xpath::NodeTestExpr* nte,
                                 Xpath::AxisExpr::Name axisid)
    : step_(step),
      priority_(priority),
      nodeTest_(nte),
      axisid_(axisid)
{
}

bool StepPatternExpr::operator==(const Xpath::Expr& other) const
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

////////////////////////////////////////////////////////////

StepPatternInst::StepPatternInst(const StepPatternExpr& expr,
                                 const Xpath::NodeSetItem& nsi,
                                 const Xpath::ExprContext& context)
    : PatternInst(expr, nsi, context),
      stepExpr_(expr),
      step_(0, 0, 1)
{
    eval();
}

void StepPatternInst::eval()
{
    if (step_.valuePtr().isNull()) {
        GroveLib::Node* node;
        if (stepExpr_.axis() == Xpath::AxisExpr::SELF ||
            stepExpr_.axis() == Xpath::AxisExpr::ATTRIBUTE)
                node = context().node();
        else
            node = parentNode(context().node());
        if (0 == node) {
            setValue(Xpath::BooleanValue::instance(false));
            return;
        }
        step_.setNode(node);
        step_.setValuePtr(stepExpr_.step().makeInst(step_,
            exprContext()), this);
    }
    const Xpath::NodeSet& node_set = step_.value()->getNodeSet();
    const Xpath::NodeSetItem* n = node_set.first();
    for (; n; n = n->next())
        if (context().node() == n->node()) {
            setValue(Xpath::BooleanValue::instance(true));
            return;
        }
    setValue(Xpath::BooleanValue::instance(false));
}

Xpath::ValueHolder*
StepPatternExpr::makeInst(const Xpath::NodeSetItem& nsi,
                          const Xpath::ExprContext& context) const
{
    return new StepPatternInst(*this, nsi, context);
}

} // namespace Xslt

#endif // USE_XPATH3
