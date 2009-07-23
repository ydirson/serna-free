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

#ifndef XSLT_PATTERNS_H
#define XSLT_PATTERNS_H

#include "xslt/xslt_defs.h"
#ifdef USE_XPATH3
# include "xslt/impl/patterns3.h"
#else
#include "xslt/impl/PatternExprImpl.h"
#include "xslt/impl/PatternInst.h"
#include "xpath/impl/expressions/AxisExpr.h"
#include <vector>

namespace Xpath {
    class NodeTestExpr;
    class ExprContext;
}

namespace Xslt {

class AltPatternExpr : public PatternExprImpl {
public:
    XSLT_OALLOC(AltPatternExpr);

    typedef COMMON_NS::Vector<PatternExprPtr>    PatternVector;

    AltPatternExpr();
    virtual ~AltPatternExpr() {}

    //! Returns the type of expression
    PatternType         pType() const { return ALT_PATTERN; }
    const PatternVector& alternatives() const{ return alternatives_; }
    void                addAlternative(const PatternExprPtr& p);
    bool                operator==(const Xpath::ExprImpl& other) const;
    double              priority() const;
    void                dump() const;
    Xpath::ValueHolder* makeInst(const Xpath::NodeSetItem& nsi,
                                 const Xpath::ExprContext& context) const;
    virtual bool        isApplicable(const GroveLib::Node*) const;

private:
    PatternVector       alternatives_;
};

/*! PatternImplInst represents the construct [1] Xslt specs 5.2.
  * PatternImplInst evaluates to boolean false if there are no alternative
  * matched in given context to boolean true.
  * In other case it evaluates to the numeric value that means priority.
  * This number is the highest priority taken from the alternatives that
  * are matched to TRUE.
  */
class AltPatternInst : public PatternInst {
public:
    XSLT_OALLOC(AltPatternInst);

    typedef std::vector<Xpath::ValueHolderPtr> PatternVector;
    AltPatternInst(const AltPatternExpr& expr,
                   const Xpath::NodeSetItem& nsi,
                   const Xpath::ExprContext& context);

    void           eval();
    virtual double matchPriority() const;
    virtual const PatternExpr* matchedAltExpr() const;

protected:
    const AltPatternExpr&   altExpr_;
    PatternVector           alts_;
    double                  matchPriority_;
};

//////////////////////////////////////////////////////

class RootPatternExpr : public PatternExprImpl {
public:
    XSLT_OALLOC(RootPatternExpr);

    //! Returns the type of expression
    PatternType         pType() const { return ROOT_PATTERN; }
    bool                operator==(const Xpath::ExprImpl& other) const;
    //! See xslt standard v1.0 chapter 5.5.2.5 (most specific kind of pattern)
    double              priority() const { return 0.5; }
    void                dump() const;
    Xpath::ValueHolder* makeInst(const Xpath::NodeSetItem& nsi,
                                 const Xpath::ExprContext& context) const;
    virtual bool        isApplicable(const GroveLib::Node*) const;
};

class RootPatternInst : public PatternInst {
public:
    XSLT_OALLOC(RootPatternInst);
    RootPatternInst(const RootPatternExpr& expr,
                    const Xpath::NodeSetItem& nsi,
                    const Xpath::ExprContext& context);
    //!
    void                    eval();
private:
    const RootPatternExpr&  expr_;
};

///////////////////////////////////////////////////////

class SlashPatternExpr : public PatternExprImpl {
public:
    XSLT_OALLOC(SlashPatternExpr);

    SlashPatternExpr(const PatternExprPtr& left,
                     const PatternExprPtr& right,
                     bool  parent_only);
    //! Returns the type of expression
    PatternType         pType() const { return SLASH_PATTERN; }
    const PatternExpr&  left() const { return *left_; }
    const PatternExpr&  right() const { return *right_; }
    //! See xslt standard v1.0 chapter 5.5.2.5 (most specific kind of pattern)
    double              priority() const {return 0.5;}
    bool                parentOnly() const {return parentOnly_;}
    bool                operator==(const Xpath::ExprImpl& other) const;
    Xpath::ValueHolder* makeInst(const Xpath::NodeSetItem& nsi,
                                 const Xpath::ExprContext& context) const;
    virtual bool        isApplicable(const GroveLib::Node*) const;
    void                dump() const;

private:
    PatternExprPtr      left_;
    PatternExprPtr      right_;
    bool                parentOnly_;
};

class SlashPatternInst : public PatternInst {
public:
    XSLT_OALLOC(SlashPatternInst);

    SlashPatternInst(const SlashPatternExpr& expr,
                     const Xpath::NodeSetItem& nsi,
                     const Xpath::ExprContext& context);
    ~SlashPatternInst();

    void                    eval();

protected:
    const SlashPatternExpr& slashExpr_;
    Xpath::ValueHolderPtr   right_;
    Xpath::NodeSet          leftList_;
};

//////////////////////////////////////////////////////////

class StepPatternExpr : public PatternExprImpl {
public:
    XSLT_OALLOC(StepPatternExpr);

    StepPatternExpr(const Xpath::ExprPtr& step, double priority,
        const Xpath::NodeTestExpr* nte = 0,
        Xpath::AxisExpr::Name axisid = Xpath::AxisExpr::LASTNAME);

    //! Returns the type of expression
    PatternType         pType() const { return STEP_PATTERN; };
    const Xpath::Expr&  step() const { return *step_; };
    //! The priority is calculated during construction
    double              priority() const { return priority_; }
    void                dump() const;
    Xpath::ValueHolder* makeInst(const Xpath::NodeSetItem& nsi,
                                 const Xpath::ExprContext& context) const;
    bool                operator==(const Xpath::ExprImpl& other) const;
    virtual bool        isApplicable(const GroveLib::Node*) const;
    Xpath::AxisExpr::Name axis() const { return axisid_; }

private:
    Xpath::ExprPtr      step_;
    const double        priority_;
    const Xpath::NodeTestExpr* nodeTest_;
    Xpath::AxisExpr::Name axisid_;
};

class StepPatternInst : public PatternInst {
public:
    XSLT_OALLOC(StepPatternInst);

    StepPatternInst(const StepPatternExpr& expr,
                    const Xpath::NodeSetItem& nsi,
                    const Xpath::ExprContext& context);

    void                    eval();
    bool                    operator==(const Xpath::ExprImpl& other) const;

private:
    const StepPatternExpr&  stepExpr_;
    Xpath::NodeSetItem      step_;
};

} // namespace Xslt

#endif // USE_XPATH3
#endif // XSLT_PATTERNS_H
