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

#ifndef XSLT_PATTERN_EXPR_H
#define XSLT_PATTERN_EXPR_H

#include "xslt/xslt_defs.h"
#include "common/RefCntPtr.h"
#include "xpath/ExprContext.h"
#include "xpath/Expr.h"
#include "grove/Decls.h"

namespace Xpath {
    class ExprInstInit;
    class ValueHolder;
    class NodeSetItem;
    class FunctionFactory;
}

namespace Xslt {

/*! \brief PatternExpr is extension of Xpath::Expr for needs of template
  matching. PatternExpr is a representation of Xslt patterns. They (patterns)
  are parsed similarly to Xpath expressions using XsltParser. Then they are
  instantiated in selected context to be evaluated into the boolean value.

  Each PatternExpr has its priority which is calculated during the parsing of
  expression. The priority of the instantiated pattern (PatternInst) is usually
  taken from its PatternExpr. The difference occures in PatternImplInst priority
  calculation. See the PatternImplInst documentation for details.
*/

class PatternExpr;

typedef COMMON_NS::RefCntPtr<PatternExpr>    PatternExprPtr;

class XSLT_EXPIMP PatternExpr : public virtual Xpath::Expr {
public:
    XSLT_OALLOC(PatternExpr);
    enum PatternType {
        ALT_PATTERN,
        SLASH_PATTERN,
        STEP_PATTERN,
        ROOT_PATTERN,
        UNKNOWN_PATTERN
    };
    //! Returns the type of expression
    virtual Type            type() const = 0;

    //! Returns the type of expression
    virtual PatternType     pType() const = 0;

    virtual double          priority() const = 0;

    virtual Xpath::ValueHolder*
        makeInst(const Xpath::NodeSetItem& nsi,
                    const Xpath::ExprContext& context) const = 0;

    /// Check whether this pattern expression is applicable for given node
    virtual bool        isApplicable(const GroveLib::Node*) const = 0;

    /// Compile pattern expression. May throw XSLT exceptions.
    static PatternExprPtr makePattern(const COMMON_NS::String& expression,
                                      const GroveLib::NodeWithNamespace*,
                                      const Xpath::FunctionFactory* fact = 0);
    
    /// same as makePattern, but w/o xslt functions. May throw XSLT exceptions.
    static PatternExprPtr makeSimplePattern(const Common::String& expression,
                                   const GroveLib::NodeWithNamespace*,
                                   const Xpath::FunctionFactory* fact = 0);

    //! Create XPath expression, but with XSLT function set.
    static Xpath::ExprPtr makeExpr(const COMMON_NS::String& expression,
                                   const GroveLib::NodeWithNamespace*,
                                   const Xpath::FunctionFactory* fact = 0);
};

} // namespace Xslt

#endif // XSLT_PATTERN_IMPL_H
