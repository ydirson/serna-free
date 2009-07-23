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

#include "xpath/NodeSet.h"
#include "xpath/ExprContext.h"
#include "xpath/impl3/debug.h"
#include "xpath/impl3/ValueImpl.h"
#include "xpath/impl3/ExprImpl.h"
#include "xpath/impl3/ExprInst.h"
#include "xpath/impl3/expressions/NodeTestExpr.h"
#include "grove/Nodes.h"

USING_COMMON_NS

namespace Xpath {

class DosVisitor : public ExprSubInst, public GroveLib::NodeVisitor {
public:
    XPATH_OALLOC(DosVisitor);

    virtual int     type() const { return DOS_SUBINST; }
    virtual void    exprChanged() { deregisterFromAllNodes(); }

    virtual void    childInserted(const GroveLib::Node*);
    virtual void    childRemoved(const GroveLib::Node*,
                                 const GroveLib::Node*);
    bool            processDosInsert(GroveLib::Node* node);
    bool            processDosRemove(const GroveLib::Node* node);

    virtual void    nodeDestroyed(const GroveLib::Node*) {}
    virtual void    attributeChanged(const GroveLib::Attr*) {}
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*);
    virtual void    attributeAdded(const GroveLib::Attr*);
    virtual void    textChanged(const GroveLib::Text*) {}
    virtual void    forceModified(const GroveLib::Node* node);
};

ExprInst::ExprInst(const ExprImpl& expr, const NodeSetItem& item,
                   const ExprContext& context)
    : expr_(expr),
      nsi_(item), context_(context),
      conversionVisitor_(*this),
      isModified_(false)
{
}

// idea: possibly create eval queue, so clients are not notified immediately
// if something changed

ConstValuePtr ExprInst::value() const
{
    ExprInst& self = const_cast<ExprInst&>(*this);
    if (!isModified_ && value_)
        return value_.pointer();
    if (value_) {
        const_cast<ExprInst&>(*this).deregisterFromAllNodes();
        conversionVisitor_.deregisterFromAllNodes();
        ExprSubInst* subInst = subInsts_.firstChild();
        while (subInst) {
            ExprSubInst* next = subInst->nextSibling();
            subInst->exprChanged();
            subInst = next;
        }
    }
    self.value_ = expr_.eval(nsi_, self);
    isModified_ = false;
    return value_.pointer();
}

void ExprInst::setValue(const ConstValueImplPtr& val)
{
    value_ = val;
}

void ExprInst::setModified()
{
    if (!isModified_) {
        isModified_ = true;
        SubscriberPtrPublisher::notifyChanged();
    }
}

void ExprInst::watchFor(Common::SubscriberPtrPublisher* publisher)
{
    if (!exprContext().watchFor(publisher))
        appendSubInst(new ValueWatcher(publisher));
}

const ValueImpl* ExprInst::impl(const ConstValuePtr& value) const
{
    return static_cast<const ValueImpl*>(value.pointer());
}

bool ExprInst::nodeTest(const GroveLib::Node* n,
                        GroveLib::Node* parent) const
{
    if (isModified_)
        return false;
    if (NodeSet::isSpecialNode(n))
        return true;
    const ExprImpl* expr = expr_.nextNodeTest();
    if (0 == expr)
        return true;
    if (parent) {
        for (; expr; expr = expr->nextNodeTest()) {
            const NodeTestExpr& nte = static_cast<const NodeTestExpr&>(*expr);
            const_cast<GroveLib::Node*>(n)->setParent(parent);
            const bool result = nte.nodeTest(n);
            const_cast<GroveLib::Node*>(n)->setParent(0);
            if (result)
                return true;
        }
    } else {
        for (; expr; expr = expr->nextNodeTest())
            if (static_cast<const NodeTestExpr*>(expr)->nodeTest(n))
                return true;
    }
    return false;
}

void ExprInst::childInserted(const GroveLib::Node* n)
{
    if (nodeTest(n))
        setModified();
}

void ExprInst::childRemoved(const GroveLib::Node* parent,
                            const GroveLib::Node* child)
{
   if (nodeTest(child, const_cast<GroveLib::Node*>(parent)))
       setModified();
}

void ExprInst::attributeChanged(const GroveLib::Attr* attr)
{
    if (nodeTest(attr))
        setModified();
}

void ExprInst::attributeRemoved(const GroveLib::Element*,
                                const GroveLib::Attr* attr)
{
    if (nodeTest(attr))
        setModified();
}

void ExprInst::attributeAdded(const GroveLib::Attr* attr)
{
    if (nodeTest(attr))
        setModified();
}

void ExprInst::textChanged(const GroveLib::Text*)
{
    setModified();
}

void ExprInst::forceModified(const GroveLib::Node*)
{
    setModified();
}

ExprInst::~ExprInst()
{
    while (subInsts_.firstChild())
        delete subInsts_.firstChild();
}

ExprInst& ExprSubInst::exprInst() const
{
    return *ExprInst::calculateSubInstParent(parent());
}

////////////////////////////////////////////////////////////////////

bool DosVisitor::processDosInsert(GroveLib::Node* node)
{
    bool hasNodes = false;
    exprInst().exprContext().registerVisitor(node, this,
        GroveLib::NodeVisitor::NOTIFY_CHILD_INSERTED|
        GroveLib::NodeVisitor::NOTIFY_CHILD_REMOVED|
        GroveLib::NodeVisitor::NOTIFY_FORCE_MODIFIED);
    GroveLib::Node* child = node->firstChild();
    while (child) {
        if (!hasNodes && exprInst().nodeTest(child))
            hasNodes = true;
        processDosInsert(child);
        child = child->nextSibling();
    }
    return hasNodes;
}

void DosVisitor::childInserted(const GroveLib::Node* n)
{
    if (exprInst().nodeTest(n)) {
        exprInst().setModified();
        return;
    }
    GroveLib::Node* node = const_cast<GroveLib::Node*>(n);
    if (n->nodeType() != GroveLib::Node::ELEMENT_NODE &&
        n->nodeType() != GroveLib::Node::DOCUMENT_FRAGMENT_NODE)
        return;
    if (processDosInsert(node))
        exprInst().setModified();
}

bool DosVisitor::processDosRemove(const GroveLib::Node* node)
{
    if (exprInst().nodeTest(node))
        return true;
    GroveLib::Node* child = node->firstChild();
    while (child) {
        if (processDosRemove(child))
            return true;
        child = child->nextSibling();
    }
    return false;
}

void DosVisitor::childRemoved(const GroveLib::Node*,
                              const GroveLib::Node* n)
{
    if (processDosRemove(n))
        exprInst().setModified();
}

void DosVisitor::forceModified(const GroveLib::Node*)
{
    exprInst().setModified();
}

void DosVisitor::attributeRemoved(const GroveLib::Element*,
                                  const GroveLib::Attr* a)
{
    if (exprInst().nodeTest(a))
        exprInst().setModified();
}

void DosVisitor::attributeAdded(const GroveLib::Attr* a)
{
    if (exprInst().nodeTest(a))
        exprInst().setModified();
}

void ExprInst::registerDosVisitor(const GroveLib::Node* node, short mask) const
{
    ExprSubInst* subInst = subInsts_.firstChild();
    for (; subInst; subInst = subInst->nextSibling())
        if (subInst->type() == ExprSubInst::DOS_SUBINST)
            break;
    if (0 == subInst) {
        subInst = new DosVisitor;
        const_cast<ExprInst*>(this)->subInsts_.appendChild(subInst);
    }
    exprContext().registerVisitor(node,
        static_cast<DosVisitor*>(subInst), mask);
}

////////////////////////////////////////////////////////////////////

static bool set_conv_visitor(ConversionVisitor* visitor,
                             const GroveLib::Node* node)
{
    if (node->nodeType() == GroveLib::Node::TEXT_NODE) {
        visitor->exprInst().registerConvVisitor(node,
            GroveLib::NodeVisitor::NOTIFY_TEXT_CHANGED|
            GroveLib::NodeVisitor::NOTIFY_FORCE_MODIFIED);
        return true;
    }
    bool hadText = false;
    if (node->nodeType() == GroveLib::Node::ELEMENT_NODE)
        visitor->exprInst().registerConvVisitor(node,
            GroveLib::NodeVisitor::NOTIFY_CHILD_INSERTED|
            GroveLib::NodeVisitor::NOTIFY_CHILD_REMOVED|
            GroveLib::NodeVisitor::NOTIFY_FORCE_MODIFIED);
    for (node = node->firstChild(); node; node = node->nextSibling())
        hadText |= set_conv_visitor(visitor, node);
    return hadText;
}

void ConversionVisitor::childInserted(const GroveLib::Node* child)
{
    if (set_conv_visitor(this, child))
        exprInst_.setModified();
}

void ConversionVisitor::forceModified(const GroveLib::Node*)
{
    exprInst_.setModified();
}

static bool check_has_text(const GroveLib::Node* n)
{
    if (n->nodeType() == GroveLib::Node::TEXT_NODE)
        return true;
    bool hasText = false;
    for (n = n->firstChild(); n; n = n->nextSibling()) {
        if (n->nodeType() == GroveLib::Node::TEXT_NODE)
            return true;
        hasText |= check_has_text(n);
    }
    return hasText;
}

void ConversionVisitor::childRemoved (const GroveLib::Node*,
                                      const GroveLib::Node* child)
{
    if (check_has_text(child))
        exprInst_.setModified();
}

void ConversionVisitor::attributeChanged(const GroveLib::Attr*)
{
    exprInst_.setModified();
}

void ConversionVisitor::attributeRemoved(const GroveLib::Element*,
                                         const GroveLib::Attr*)
{
    exprInst_.setModified();
}

void ConversionVisitor::attributeAdded(const GroveLib::Attr*)
{
    exprInst_.setModified();
}

void ConversionVisitor::textChanged(const GroveLib::Text*)
{
    exprInst_.setModified();
}

////////////////////////////////////////////////////////////////////

ValueHolder* ExprImpl::makeInst(const NodeSetItem& nsi,
                                const ExprContext& context) const
{
    return new ExprInst(*this, nsi, context);
}

void ExprContext::registerVisitor(const GroveLib::Node* node,
                                  GroveLib::NodeVisitor* visitor,
                                  short mask) const
{
    const_cast<GroveLib::Node*>(node)->registerNodeVisitor(visitor, mask);
}

} // namespace Xpath
