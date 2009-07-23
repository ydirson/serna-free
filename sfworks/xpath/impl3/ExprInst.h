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
#ifndef XPATH_EXPR_INST_H
#define XPATH_EXPR_INST_H

#include "xpath/xpath_defs.h"
#include "common/OwnerPtr.h"
#include "common/SubscriberPtr.h"
#include "xpath/Exception.h"
#include "xpath/ValueHolder.h"
#include "xpath/impl3/ExprSubinst.h"
#include "xpath/impl3/ValueImpl.h"
#include "xpath/impl3/ExprImpl.h"
#include "grove/NodeVisitor.h"

namespace Xpath {

class Expr;
class ExprInst;

class ConversionVisitor : public GroveLib::NodeVisitor {
public:
    ConversionVisitor(ExprInst& inst)
        : exprInst_(inst) {}

    ExprInst& exprInst() { return exprInst_; }

    virtual void    nodeDestroyed(const GroveLib::Node*) {}
    virtual void    childInserted(const GroveLib::Node*);
    virtual void    childRemoved (const GroveLib::Node*,
                                  const GroveLib::Node*);
    virtual void    attributeChanged(const GroveLib::Attr*);
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*);
    virtual void    attributeAdded(const GroveLib::Attr*);
    virtual void    textChanged(const GroveLib::Text*);
    virtual void    forceModified(const GroveLib::Node* node);

private:
    ExprInst& exprInst_;
};

class XPATH_EXPIMP ExprInst : public ValueHolder,
                              public GroveLib::NodeVisitor {
public:
    XPATH_OALLOC(ExprInst);

    //! Returns the expression evaluation context
    const ExprContext&      exprContext() const { return context_; }
    //! Returns the pointer to (node-level) evaluation context
    const NodeSetItem&      context() const { return nsi_; }

    //! Reimplemented from Value holder
    ConstValuePtr           value() const;

    //! Sets the value equal to given one
    void                    setValue(const ConstValueImplPtr& val);
    //! Returns reference to the origin expression
    const ExprImpl&         expr() const { return expr_; }
    //! Casts the value to ValueImpl
    const ValueImpl*        impl(const ConstValuePtr& value) const;

    virtual ExprInst*       exprInst() const
    {
        return const_cast<ExprInst*>(this);
    }
    void registerVisitor(const GroveLib::Node* node, short mask)
    {
        exprContext().registerVisitor(const_cast<GroveLib::Node*>(node),
            this, mask);
    }
    void registerConvVisitor(const GroveLib::Node* node, short mask)
    {
        exprContext().registerVisitor(const_cast<GroveLib::Node*>(node),
            &conversionVisitor_, mask);
    }
    void registerDosVisitor(const GroveLib::Node* node, short mask) const;
    void watchFor(Common::SubscriberPtrPublisher*);

    bool nodeTest(const GroveLib::Node* n,
                  GroveLib::Node* parent = 0) const;

    void            setModified();

    virtual ~ExprInst();

    typedef Common::XList<ExprSubInst> SubInstList;
    static ExprInst* calculateSubInstParent(SubInstList* p)
    {
        static const size_t offs = ((size_t)&((ExprInst*)16)->subInsts_) - 16;
        return (ExprInst*)((char*)p - offs);
    }
    ExprSubInst* findSubInst(int type) const;
    void         appendSubInst(ExprSubInst* si) { subInsts_.appendChild(si); }

protected:
    friend class ExprImpl;

    ExprInst(const ExprImpl& expr, const NodeSetItem& item,
             const ExprContext& context);

    virtual void    nodeDestroyed(const GroveLib::Node*) {}
    virtual void    childInserted(const GroveLib::Node*);
    virtual void    childRemoved (const GroveLib::Node*,
                                  const GroveLib::Node*);
    virtual void    attributeChanged(const GroveLib::Attr*);
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*);
    virtual void    attributeAdded(const GroveLib::Attr*);
    virtual void    textChanged(const GroveLib::Text*);
    virtual void    forceModified(const GroveLib::Node* node);

private:
    const ExprImpl&     expr_;
    ConstValueImplPtr   value_;
    const NodeSetItem&  nsi_;
    const ExprContext&  context_;
    mutable ConversionVisitor conversionVisitor_;
    SubInstList         subInsts_;
    mutable bool        isModified_;
};

inline ExprSubInst* ExprInst::findSubInst(int type) const
{
    ExprSubInst* subInst = subInsts_.firstChild();
    for (; subInst; subInst = subInst->nextSibling())
        if (subInst->type() == type)
            return subInst;
    return 0;
}

} // namespace Xpath

#endif // XPATH_EXPR_INST_H
