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
#include "xslt/xslt_defs.h"
#ifdef USE_XPATH3
#include "xslt/impl/FunctionExprImpl.h"
#include "xslt/impl/debug.h"
#include "xslt/impl/utils.h"
#include "xslt/Exception.h"
#include "xslt/impl/xpath_values.h"
#include "xpath/NodeSet.h"
#include "grove/Nodes.h"
#include "grove/NodeVisitor.h"

static int dbk_funcid = 0;

namespace Xslt {

class DbkOrdlistSubinst : public Xpath::ExprSubInst,
                          public GroveLib::NodeVisitor {
public:
    virtual int type() const
    { 
        return XSLT_SUBINST + XsltFunctionId::DbkOrdlistItemnumber; 
    }
    DbkOrdlistSubinst(const Xpath::NodeSetItem& nsi)
        : contextNode_(nsi.node()), oldVal_(-1), mustNotify_(false)
    {
        GroveLib::Node* pn = parentNode(nsi.node());
        if (!pn || pn->nodeType() != GroveLib::Node::ELEMENT_NODE)
            return;
        pn->registerNodeVisitor(this);
        nsi.node()->registerNodeVisitor(this);
    }
    virtual void    childInserted(const GroveLib::Node* n)
    {
        if (n->parent() == contextNode_->parent())
            exprInst().setModified();
    }
    virtual void    childRemoved (const GroveLib::Node* p,
                                  const GroveLib::Node*)
    {
        if (p == contextNode_->parent())
            exprInst().setModified();
    }
    virtual void    attributeChanged(const GroveLib::Attr* a)
    {
        attrChange(a->element(), a);
    }
    virtual void    attributeRemoved(const GroveLib::Element* elem,
                                     const GroveLib::Attr* a)
    {
        attrChange(elem, a);
    }
    virtual void    attributeAdded(const GroveLib::Attr* a)
    {
        attrChange(a->element(), a);
    }
    virtual void    genericNotify(const GroveLib::Node*, void* t)
    {
        if (t == &dbk_funcid)
            exprInst().setModified();
    }
    virtual void    nodeDestroyed(const GroveLib::Node*) {}
    virtual void    textChanged(const GroveLib::Text*) {}

    void            attrChange(const GroveLib::Element* elem,
                               const GroveLib::Attr* a)
    {
        if (elem != contextNode_)
            return;
        if (a->localName() != "override")
            return;
        mustNotify_ = true;
        exprInst().setModified();
    }

public:
    const GroveLib::Node* contextNode_;
    int     oldVal_;
    bool    mustNotify_;
};

template<> Xpath::ConstValueImplPtr
FunctionExprImpl<XsltFunctionId::DbkOrdlistItemnumber>::eval
    (const Xpath::NodeSetItem& nsi, Xpath::ExprInst& ei) const
{
    DbkOrdlistSubinst* subInst = static_cast<DbkOrdlistSubinst*>
        (ei.findSubInst(Xpath::ExprSubInst::XSLT_SUBINST + 
            XsltFunctionId::DbkOrdlistItemnumber));
    if (0 == subInst) {
        subInst = new DbkOrdlistSubinst(nsi);
        ei.appendSubInst(subInst);
    } else if (subInst->contextNode_ != nsi.node()) {
        delete subInst;
        subInst = new DbkOrdlistSubinst(nsi);
        ei.appendSubInst(subInst);
    }
    Xpath::FuncArgEvaluator fargs(nsi, ei, this);
    if (fargs.argNum() < 1)
        throw Xslt::Exception(XsltMessages::xpathInstDocbookArgs,
                              ei.exprContext().contextString());
    int sn = int(fargs.arg(0)->to(Xpath::Value::NUMERIC)->getDouble());
    const GroveLib::Node* selfNode = nsi.node();
    if (!selfNode->parent())
        return new Xpath::NumericValue(subInst->oldVal_);
    GroveLib::Node* n = parentNode(selfNode)->firstChild();
    for (; n; n = n->nextSibling()) {
        if (n->nodeType() != GroveLib::Node::ELEMENT_NODE)
            continue;
        const GroveLib::Element* e = static_cast<const GroveLib::Element*>(n);
        if (e->localName() != "listitem")
            continue;
        const GroveLib::Attr* a = e->attrs().getAttribute("override");
        if (a)
            sn = a->value().toInt();
        if (n == selfNode)
            break;
        ++sn;
    }
    if (n != selfNode)
        return new Xpath::NumericValue(subInst->oldVal_);
    if (sn != subInst->oldVal_) {
        subInst->oldVal_ = sn;
        if (subInst->mustNotify_) {
            n->parent()->genericNotify(&dbk_funcid);
            subInst->mustNotify_ = false;
        }
    }
    return new Xpath::NumericValue(sn);
}

} // namespace Xslt

#endif // USE_XPATH3
