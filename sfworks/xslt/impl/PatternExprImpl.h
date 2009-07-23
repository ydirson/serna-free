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
#ifndef XSLT_PATTERN_EXPR_IMPL_H
#define XSLT_PATTERN_EXPR_IMPL_H

#include "xslt/xslt_defs.h"
#include "xslt/PatternExpr.h"
# ifdef USE_XPATH3
#include "xpath/impl3/ExprImpl.h"
#include "xpath/impl3/ValueImpl.h"
# else
#include "xpath/impl/ExprImpl.h"
#include "xpath/impl/ValueImpl.h"
#endif // USE_XPATH3

namespace Xslt {

class PatternExprImpl;
class PatternInst;

typedef Common::RefCntPtr<PatternExprImpl> PatternExprImplPtr;

class PatternExprImpl : public PatternExpr,
                        public Xpath::ExprImpl {
public:
    XSLT_OALLOC(PatternExprImpl);

    //! Returns the type of expression
    virtual Type        type() const { return UNKNOWN_EXPR; }
    virtual int         contextDependency() const
    {
        return ExprImpl::contextDependency();
    }
    static Xpath::ExprImplPtr makeExpr(const COMMON_NS::String& expression,
                                       const GroveLib::NodeWithNamespace*,
                                       const Xpath::FunctionFactory* fact = 0);
    static PatternExprImplPtr makePattern(const COMMON_NS::String& expression,
                                      const GroveLib::NodeWithNamespace*,
                                      const Xpath::FunctionFactory* fact = 0);
#ifdef USE_XPATH3
    virtual Xpath::ConstValueImplPtr eval(const Xpath::NodeSetItem& nsi,
                                          Xpath::ExprInst& ei) const;
    virtual Xpath::ValueHolder* makeInst(const Xpath::NodeSetItem& nsi,
        const Xpath::ExprContext& ec) const;
    virtual bool eval(const Xpath::NodeSetItem& nsi, PatternInst& ei) const = 0;
#endif // USE_XPATH3

private:
    virtual int         dep_type() const { return NO_DEP; }
};

} // namespace Xslt

#endif // XSLT_PATTERN_IMPL_H

