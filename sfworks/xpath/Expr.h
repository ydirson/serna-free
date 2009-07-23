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

#ifndef XPATH_EXPR_H
#define XPATH_EXPR_H

#include "common/common_defs.h"
#include "common/String.h"
#include "common/RefCntPtr.h"
#include "common/SernaApiRefCounted.h"

#include "xpath/xpath_defs.h"
#include "xpath/ExprContext.h"
#include "xpath/Exception.h"

#ifdef CONST
#undef CONST
#endif

namespace Xpath {

class ValueHolder;
class Expr;
class NodeSetItem;
typedef COMMON_NS::RefCntPtr<Expr> ExprPtr;

/*! \brief Expr is syntactic representaion of xpath expression.

  Expr and it`s derivatives represent the syntactic tree of parsed
  xpath expression. Syntactic tree is made of Expr`s linked
  hierarchically in accordance with xpath syntax rules.
 */
class XPATH_EXPIMP Expr : public Common::SernaApiRefCounted {
public:
    XPATH_OALLOC(Expr);
    enum Type {
        AXIS,       BINARY,     CONST,  FUNCTION,
        NODE_TEST,  PREDICATE,  SLASH,  UNARY,  VAR,
        UNKNOWN_EXPR
    };
    virtual ~Expr() {}
    //! Returns the type of expression
    virtual Type            type() const = 0;
    //! Compares expression with given one
    //virtual bool            operator==(const Expr& other) const = 0;
    //!
    virtual void            dump() const = 0;
    //! Instantiates itself in given context to calculate the value
    virtual ValueHolder*    makeInst(const NodeSetItem& item,
                                     const ExprContext& context) const = 0;
    //! Expression dependency on context position/size
    enum ContextDepType {
        NO_DEP = 0, SIZE_DEP = 01, POS_DEP = 02, UNDEF_DEP = 255
    };
    virtual int             contextDependency() const = 0;

protected:
    Expr() {}

private:
    // Disabled operations
    Expr(const Expr&);
    Expr& operator=(const Expr& e);
};

} // namespace Xpath

#endif // XPATH_EXPR_H
