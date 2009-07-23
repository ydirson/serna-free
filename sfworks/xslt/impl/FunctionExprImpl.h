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

#ifndef XSLT_FUNCTION_EXPR_IMPL_H_
#define XSLT_FUNCTION_EXPR_IMPL_H_

#include "xslt/xslt_defs.h"
#ifdef USE_XPATH3
# include "xslt/impl/FunctionExprImpl3.h"
#else
#include "xpath/ExprContext.h"
#include "xpath/Expr.h"
#include "xpath/impl/expressions/FunctionExpr.h"
#include "xpath/impl/instances/FunctionInst.h"

namespace Xslt {

namespace FunctionId {
    // note: unparsed-entity-uri is in Xpath
    enum Id {
        Current, Document, FormatNumber, GenerateId, Key, SystemProperty,
        CalsInheritedAttribute, DbkOrdlistItemnumber, CalsAttribute
    };
}

template <FunctionId::Id F> class FunctionExprImpl :
                                  public Xpath::FunctionExpr {
public:
    FunctionExprImpl(const COMMON_NS::String& funcName,
        const Xpath::FunctionArgExpr* args = 0)
            : FunctionExpr(funcName, args) {}
    virtual void calc(Xpath::FunctionInst*) const;
};

class DocumentFuncImpl : public Xpath::FunctionExpr {
public:
    DocumentFuncImpl(const COMMON_NS::String& funcName,
                     const Xpath::FunctionArgExpr* args);

    virtual void calc(Xpath::FunctionInst*) const {}
    virtual Xpath::ValueHolder* makeInst(const Xpath::NodeSetItem& nsi,
                                    const Xpath::ExprContext& context) const;
};


class CalsInheritedAttributeImpl : public Xpath::FunctionExpr {
public:
    CalsInheritedAttributeImpl(const COMMON_NS::String& funcName,
                               const Xpath::FunctionArgExpr* args);

    virtual void calc(Xpath::FunctionInst*) const {}
    virtual Xpath::ValueHolder* makeInst(const Xpath::NodeSetItem& nsi,
                                    const Xpath::ExprContext& context) const;
};

class CalsAttributeImpl : public Xpath::FunctionExpr {
public:
    CalsAttributeImpl(const COMMON_NS::String& funcName,
                      const Xpath::FunctionArgExpr* args);

    virtual void calc(Xpath::FunctionInst*) const {}
    virtual Xpath::ValueHolder* makeInst(const Xpath::NodeSetItem& nsi,
                                    const Xpath::ExprContext& context) const;
};

class DbkOrdlistItemnumberFuncImpl : public Xpath::FunctionExpr {
public:
    DbkOrdlistItemnumberFuncImpl(const COMMON_NS::String& funcName,
                                 const Xpath::FunctionArgExpr* args);

    virtual void calc(Xpath::FunctionInst*) const {}
    virtual Xpath::ValueHolder* makeInst(const Xpath::NodeSetItem& nsi,
                                   const Xpath::ExprContext& context) const;
};

}

#endif // USE_XPATH3
#endif // XSLT_FUNCTION_EXPR_IMPL_H_

