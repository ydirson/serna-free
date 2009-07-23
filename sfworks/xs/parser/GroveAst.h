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

#ifndef GROVE_AST_H_
#define GROVE_AST_H_

#include "xs/xs_defs.h"
#include "xs/Origin.h"
#include "grove/grove_defs.h"
#include "grove/Node.h"
#include "grove/Nodes.h"
#include "antlr/AST.hpp"

/*! A mapping of XML grove into ANTLR abstract syntax tree (AST)
 */
class GroveAst : public antlr::AST {
public:
    GROVE_NAMESPACE::Element*     element() const { return elem_; }
    XS_NAMESPACE::Origin   origin() const;

    GroveAst(GROVE_NAMESPACE::Element* elem, Schema* s)
        : elem_(elem), schema_(s) {}

private:
    GROVE_NAMESPACE::Element*        elem_;
    Schema*                   schema_;

public: // ANTLR interface

    ///////////////////////////////////////////////////////////

    virtual void addChild(antlr::RefAST c);

    virtual bool equals(antlr::RefAST t) const;
    virtual bool equalsList(antlr::RefAST t) const;
    virtual bool equalsListPartial(antlr::RefAST t) const;
    virtual bool equalsTree(antlr::RefAST t) const;
    virtual bool equalsTreePartial(antlr::RefAST t) const;

    virtual std::vector<antlr::RefAST> findAll(antlr::RefAST t);
    virtual std::vector<antlr::RefAST> findAllPartial(antlr::RefAST t0);

    /** Get the first child of this node; null if no children */
    virtual antlr::RefAST getFirstChild() const;
    /** Get  the next sibling in line after this one */
    virtual antlr::RefAST getNextSibling() const;

    /** Get the token text for this node */
    virtual std::string getText() const;
    /** Get the token type for this node */
    virtual int getType() const;

    virtual void initialize(int t, const std::string& txt);
    virtual void initialize(antlr::RefAST t);
    virtual void initialize(antlr::RefToken t);

    /** Set the first child of a node. */
    virtual void setFirstChild(antlr::RefAST c);
    /** Set the next sibling after this one. */
    virtual void setNextSibling(antlr::RefAST n);

    /** Set the token text for this node */
    virtual void setText(const std::string& txt);
    /** Set the token type for this node */
    virtual void setType(int type);

    virtual std::string toString() const;
    virtual std::string toStringList() const;
    virtual std::string toStringTree() const;

    virtual ~GroveAst();
};

class RefGroveAst : public antlr::RefAST {
public:
    typedef antlr::RefAST ANTLR_REF_AST_T;
    GroveAst* get() const {
        return ANTLR_REF_AST_T::get() ?
            static_cast<GroveAst*>(ANTLR_REF_AST_T::get()) : 0;
    }
    operator GroveAst*() const   { return get(); }
    GroveAst* operator->() const { return get(); }

    RefGroveAst() : ANTLR_REF_AST_T(antlr::nullASTptr) {}
    RefGroveAst(const ANTLR_REF_AST_T& ast)
        : ANTLR_REF_AST_T(ast) {}
};

inline bool
operator==(const RefGroveAst& lhs, const RefGroveAst& rhs)
{
    return lhs.get() == rhs.get();
}

// KLUDGE: for broken stl_relops.h
inline bool
operator!=(const RefGroveAst& lhs, const RefGroveAst& rhs)
{
    return !(lhs == rhs);
}

#endif // GROVE_AST_H_
