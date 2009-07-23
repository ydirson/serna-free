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
#ifndef XPATH_FUNCTION_BASE_H
#define XPATH_FUNCTION_BASE_H

#include "common/Vector.h"
#include "common/String.h"
#include "xpath/xpath_defs.h"
#include "xpath/impl3/ExprImpl.h"
#include "xpath/impl3/ExprInst.h"

/*!
 */
namespace Xpath {

class ValueHolder;
class FunctionArgExpr;
class FunctionInst;

class XPATH_EXPIMP FunctionExpr : public ExprImpl {
public:
    enum FunctionId {
        Boolean, Ceiling, Concat, Contains, Count, False, Floor,
        Id, Lang, Last, LocalName, Name, NormalizeSpace, Not,
        NsUri, Number, Position, Round, StartWith, StringFunc, 
        StringLength, SubstringAfter, SubstringBefore, Substring, 
        Sum, Translate, True, UnparsedEntityUri, Level, Redline, 
        NodeSetFunc, DitaContains, ExternalFunction, LAST_FUNC
    };

    typedef COMMON_NS::Vector<ExprImplPtr> ExprPtrVector;

    FunctionExpr(const Common::String& funcName,
                 const FunctionArgExpr* args = 0);

    Type                    type() const { return FUNCTION; }
    virtual FunctionId      functionId() const = 0;

    const ExprPtrVector&    args() const { return args_; }
    bool                    operator==(const ExprImpl& other) const;
    void                    dump() const;
    const Common::String&   name() const { return funcName_; }

    virtual ExprImplPtr     optimize(int opt);

    XPATH_OALLOC(FunctionExpr);

protected:
    virtual int             dep_type() const;
    virtual bool            isNumeric() const;
    virtual void            makeNodeTestChain(ExprImpl*& /*prev*/);

    ExprPtrVector           args_;
    COMMON_NS::String       funcName_;
};

// The sole purpose of this class is the linearization of arguments
// presented as a parse tree of FunctionArgExpr's.
//
class XPATH_EXPIMP FunctionArgExpr : public ExprImpl {
public:
    typedef COMMON_NS::RefCntPtr<FunctionArgExpr> FunctionArgExprPtr;

    FunctionArgExpr(const ExprImplPtr& arg, const FunctionArgExprPtr& next)
        : arg_(arg), next_(next) {}

    void    linearize(COMMON_NS::Vector<ExprImplPtr>& argv) const;

    virtual Type            type() const { return UNKNOWN_EXPR; }
    virtual bool            operator==(const ExprImpl&) const { return 1; }
    virtual void            dump() const {}
    ConstValueImplPtr       eval(const NodeSetItem&, ExprInst&) const
    {
        return 0;
    }
    int                     dep_type() const { return NO_DEP; }

private:
    ExprImplPtr        arg_;
    FunctionArgExprPtr next_;
};

class XPATH_EXPIMP FuncArgEvaluator {
public:
    FuncArgEvaluator(const NodeSetItem& context,
                     ExprInst& ei,
                     const FunctionExpr* fe)
        : context_(context), exprInst_(ei), funcExpr_(fe)
    {
        args_.resize(funcExpr_->args().size());
    }
    ConstValueImplPtr arg(uint i);
    uint argNum() const { return args_.size(); }

private:
    const NodeSetItem&          context_;
    ExprInst&                   exprInst_;
    const FunctionExpr*         funcExpr_;
    Common::Vector<ConstValueImplPtr> args_;
};

} // namespace Xpath

#endif // XPATH_FUNCTION_BASE_H
