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
#ifndef XPATH_EXPR_IMPL_H
#define XPATH_EXPR_IMPL_H

#include "common/CDList.h"
#include "xpath/xpath_defs.h"
#include "xpath/Expr.h"
#include "xpath/ValueHolder.h"
#include "xpath/impl3/ValueImpl.h"

namespace Xpath {

class ExprImpl;
class ExprInst;

typedef COMMON_NS::RefCntPtr<ExprImpl> ExprImplPtr;

/*! \brief ExprImpl is an Expr which limits the lifetime of it`s instantiations.
 */
class XPATH_EXPIMP ExprImpl : public virtual Expr {
public:
    XPATH_OALLOC(ExprImpl);
    virtual ~ExprImpl() {}

    //! Implements Expr::contextDependency()
    virtual int         contextDependency() const;
    virtual bool        isNumeric() const;

    ExprImpl*           parent() const { return parent_; }
    void                setParent(ExprImpl* p) { parent_ = p; }
    virtual bool        operator==(const ExprImpl& other) const = 0;

    virtual ExprImplPtr optimize(int) { return this; }

    virtual ConstValueImplPtr eval(const NodeSetItem& nsi,
                                   ExprInst& ei) const = 0;
    virtual ValueHolder*    makeInst(const NodeSetItem& nsi,
                                     const ExprContext& context) const;
    const ExprImpl* nextNodeTest() const { return nextNodeTest_; }
    virtual void        makeNodeTestChain(ExprImpl*& /*prev*/) {}

protected:
    friend class NodeTestExpr;

    virtual int         dep_type() const = 0;
    mutable int         dep_type_;
    ExprImpl*           parent_;
    const ExprImpl*     nextNodeTest_;

    ExprImpl()
        : dep_type_(UNDEF_DEP), parent_(0), nextNodeTest_(0) {}
};

} // namespace Xpath

#endif // XPATH_EXPR_IMPL_H
