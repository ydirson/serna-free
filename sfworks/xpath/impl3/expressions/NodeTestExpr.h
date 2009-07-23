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

#ifndef XPATH_NODE_TEST_EXPR_H
#define XPATH_NODE_TEST_EXPR_H

#include "common/String.h"
#include "xpath/xpath_defs.h"
#include "xpath/impl3/ExprImpl.h"
#include "grove/Decls.h"
#include "grove/Node.h"
#include "grove/XmlName.h"

/*!
 */
namespace Xpath {

class FunctionExpr;

class XPATH_EXPIMP NodeTestExpr : public ExprImpl {
public:
    enum Name {
        NODE_NODE_TEST,
        NAME_NODE_TEST,
        COMMENT_NODE_TEST,
        PI_NODE_TEST,
        TEXT_NODE_TEST,
        NODE_TYPE_TEST,
        OR_NODE_TEST,
        AND_NODE_TEST,
        NOT_NODE_TEST,
        LANG_NODE_TEST,
        DITA_CONTAINS_NODE_TEST
    };
    virtual Name name() const = 0;
    virtual Type type() const { return NODE_TEST; }
    virtual bool nodeTest(const GroveLib::Node*) const = 0;
    virtual double priority() const = 0;

    bool                        operator==(const ExprImpl& other) const;
    int                         dep_type() const { return NO_DEP; }
    virtual void                makeNodeTestChain(ExprImpl*&);
    virtual ConstValueImplPtr eval(const NodeSetItem&, ExprInst&) const
    {
        return 0;
    }
};

typedef COMMON_NS::RefCntPtr<NodeTestExpr> NodeTestExprPtr;

class XPATH_EXPIMP NameNodeTestExpr : public NodeTestExpr,
                                      public GroveLib::ExpandedName {
public:
    XPATH_OALLOC(NameNodeTestExpr);
    NameNodeTestExpr(const GroveLib::QualifiedName&,
                     const GroveLib::NodeWithNamespace*);
    Name                        name() const { return NAME_NODE_TEST; }

    virtual bool nodeTest(const GroveLib::Node*) const;

    virtual double              priority() const;
    bool                        operator==(const ExprImpl& other) const;
    void                        dump() const;
};

class XPATH_EXPIMP LangNodeTestExpr : public NodeTestExpr {
public:
    XPATH_OALLOC(LangNodeTestExpr);
    Name                        name() const { return LANG_NODE_TEST; }

    virtual bool nodeTest(const GroveLib::Node*) const;

    virtual double              priority() const;
    void                        dump() const;
};

class XPATH_EXPIMP NodeNodeTestExpr : public NodeTestExpr {
public:
    XPATH_OALLOC(NodeNodeTestExpr);
    Name                        name() const { return NODE_NODE_TEST; }
    virtual bool nodeTest(const GroveLib::Node*) const;
    virtual double              priority() const;
    void                        dump() const;
};

class XPATH_EXPIMP TextNodeTestExpr : public NodeTestExpr {
public:
    XPATH_OALLOC(TextNodeTestExpr);
    Name                        name() const { return TEXT_NODE_TEST; }
    virtual bool nodeTest(const GroveLib::Node*) const;
    virtual double              priority() const;
    void                        dump() const;
};

class XPATH_EXPIMP CommentNodeTestExpr : public NodeTestExpr {
public:
    XPATH_OALLOC(CommentNodeTestExpr);
    Name                        name() const { return COMMENT_NODE_TEST; }
    virtual bool nodeTest(const GroveLib::Node*) const;
    virtual double              priority() const;
    void                        dump() const;
};

class XPATH_EXPIMP PiNodeTestExpr : public NodeTestExpr {
public:
    XPATH_OALLOC(PiNodeTestExpr);
    PiNodeTestExpr(const COMMON_NS::String& piname);

    Name                        name() const { return PI_NODE_TEST; }
    virtual bool nodeTest(const GroveLib::Node*) const;
    virtual double              priority() const;
    void                        dump() const;
    static NodeTestExpr*        make(const COMMON_NS::String&);

private:
    COMMON_NS::String pi_name_;
};

class XPATH_EXPIMP NodeTypeTestExpr : public NodeTestExpr {
public:
    XPATH_OALLOC(NodeTypeTestExpr);
    NodeTypeTestExpr(GroveLib::Node::NodeType t);
    Name                        name() const { return NODE_TYPE_TEST; }
    virtual bool nodeTest(const GroveLib::Node*) const;
    virtual double              priority() const;
    void                        dump() const;
    bool                        operator==(const ExprImpl& other) const;

private:
    GroveLib::Node::NodeType    type_;
};

class XPATH_EXPIMP TextChoiceTestExpr : public NodeTestExpr {
public:
    XPATH_OALLOC(TextChoiceTestExpr);
    TextChoiceTestExpr(const Common::String& testFor);
    Name                        name() const { return NODE_TYPE_TEST; }
    virtual bool nodeTest(const GroveLib::Node*) const;
    virtual double              priority() const;
    void                        dump() const;
    bool                        operator==(const ExprImpl& other) const;

private:
    const char* compareWith_;
};

class XPATH_EXPIMP EnumAttrTestExpr : public NodeTestExpr {
public:
    XPATH_OALLOC(EnumAttrTestExpr);
    
    Name                        name() const { return NODE_TYPE_TEST; }
    virtual bool nodeTest(const GroveLib::Node*) const;
    virtual double              priority() const;
    void                        dump() const;
};

class XPATH_EXPIMP DitaContainsTestExpr : public NodeTestExpr {
public:
    XPATH_OALLOC(DitaContainsTestExpr);
    
    DitaContainsTestExpr(const Common::String& ts);
    DitaContainsTestExpr(const FunctionExpr*);

    Name                        name() const { return DITA_CONTAINS_NODE_TEST; }
    virtual bool nodeTest(const GroveLib::Node*) const;
    virtual double              priority() const;
    void                        dump() const;
    bool                        operator==(const ExprImpl& other) const;

private:
    Common::String              test_;
};

///////////////////////////////////////////////////////////////////
//
// Logical node tests
//
//
class XPATH_EXPIMP OrNodeTestExpr : public NodeTestExpr {
public:
    XPATH_OALLOC(OrTestExpr);

    OrNodeTestExpr(const NodeTestExprPtr& left,
                   const NodeTestExprPtr& right)
        : left_(left), right_(right) {}

    Name                        name() const { return OR_NODE_TEST; }
    virtual bool nodeTest(const GroveLib::Node*) const;
    virtual double              priority() const;
    void                        dump() const;

private:
    NodeTestExprPtr left_;
    NodeTestExprPtr right_;
};


class XPATH_EXPIMP AndNodeTestExpr : public NodeTestExpr {
public:
    XPATH_OALLOC(AndNodeTestExpr);

    AndNodeTestExpr(const NodeTestExprPtr& left,
                    const NodeTestExprPtr& right)
        : left_(left), right_(right) {}

    Name                        name() const { return AND_NODE_TEST; }
    virtual bool nodeTest(const GroveLib::Node*) const;
    virtual double              priority() const;
    void                        dump() const;

private:
    NodeTestExprPtr left_;
    NodeTestExprPtr right_;
};

class XPATH_EXPIMP NotNodeTestExpr : public NodeTestExpr {
public:
    XPATH_OALLOC(NotNodeTestExpr);

    NotNodeTestExpr(const NodeTestExprPtr& left)
        : left_(left) {}

    Name                        name() const { return NOT_NODE_TEST; }
    virtual bool nodeTest(const GroveLib::Node*) const;
    virtual double              priority() const;
    void                        dump() const;

private:
    NodeTestExprPtr left_;
};

} // namespace Xpath

#endif // XPATH_NODE_TEST_EXPR_H
