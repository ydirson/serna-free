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

#include "xpath/ExprContext.h"
#include "xpath/NodeSet.h"
#include "xpath/impl3/NodeSetValue.h"
#include "xpath/impl3/debug.h"
#include "xpath/impl3/ExprInst.h"
#include "xpath/impl3/expressions/AxisExpr.h"
#include "grove/Nodes.h"

namespace Xpath {

typedef GroveLib::NodeVisitor NV;

void AxisExpr::collectDescendants(ExprInst& ec,
                                  NodeSet& nodeSet,
                                  GroveLib::Node* node) const
{
    ec.registerDosVisitor(node, NV::NOTIFY_CHILD_INSERTED|
                                NV::NOTIFY_CHILD_REMOVED|
                                NV::NOTIFY_FORCE_MODIFIED);
    GroveLib::Node* child = node->firstChild();
    while (child) {
        if (nodeTest(child))
            nodeSet += child;
        collectDescendants(ec, nodeSet, child);
        child = child->nextSibling();
    }
}

bool AxisExpr::nodeTest(const GroveLib::Node* n) const
{
    if (NodeSet::isSpecialNode(n))
        return true;
    if (nodeTestExpr_.isNull())
        return true;
    return nodeTestExpr_->nodeTest(n);
}

/////////////////////////////////////////////////

template<> ConstValueImplPtr AxisAncestorExpr::eval(const NodeSetItem& context,
                                                    ExprInst&) const
{
    NodeSet nodeSet;
    GroveLib::Node* node = parentNode(context.node());
    while (node) {
        if (nodeTest(node))
            nodeSet += node;
        node = parentNode(node);
    }
    return new NodeSetValue(nodeSet);
}

template<> ConstValueImplPtr AxisAosExpr::eval(const NodeSetItem& context,
                                               ExprInst&) const

{
    NodeSet nodeSet;
    GroveLib::Node* node = context.node();
    while (node) {
        if (nodeTest(node))
            nodeSet += node;
        node = parentNode(node);
    }
    return new NodeSetValue(nodeSet);
}

template<> ConstValueImplPtr AxisAttributeExpr::eval(const NodeSetItem& context,
                                                     ExprInst& ei) const

{
    NodeSet nodeSet;
    GroveLib::Node* n = context.node();
    const GroveLib::Node::NodeType nt = n->nodeType();
    if (GroveLib::Node::ELEMENT_NODE == nt) {
        GroveLib::Element* element = static_cast<GroveLib::Element*>(n);
        GroveLib::Attr* attr = element->attrs().firstChild();
        for (; attr; attr = attr->nextSibling())
            if (nodeTest(attr))
                nodeSet += attr;
        ei.registerVisitor(n, NV::NOTIFY_ATTRIBUTE_ADDED |
                NV::NOTIFY_ATTRIBUTE_REMOVED);
    } else if (GroveLib::Node::ATTRIBUTE_NODE == nt) {
        ei.registerVisitor(parentNode(n), NV::NOTIFY_ATTRIBUTE_ADDED |
            NV::NOTIFY_ATTRIBUTE_REMOVED);
        if (nodeTest(n))
            nodeSet += n;
    }
    return new NodeSetValue(nodeSet);
}

template<> ConstValueImplPtr AxisChildExpr::eval(const NodeSetItem& context,
                                                 ExprInst& ei) const
{
    NodeSet nodeSet;
    ei.registerVisitor(context.node(),
        NV::NOTIFY_CHILD_INSERTED | NV::NOTIFY_CHILD_REMOVED |
        NV::NOTIFY_FORCE_MODIFIED);
    GroveLib::Node* node = context.node()->firstChild();
    while (node) {
        if (nodeTest(node))
            nodeSet += node;
        node = node->nextSibling();
    }
    return new NodeSetValue(nodeSet);
}

template<>
ConstValueImplPtr AxisDescendantExpr::eval(const NodeSetItem& context,
                                           ExprInst& ei) const

{
    NodeSet nodeSet;
    collectDescendants(ei, nodeSet, context.node());
    return new NodeSetValue(nodeSet);
}

template<> ConstValueImplPtr AxisDosExpr::eval(const NodeSetItem& context,
                                               ExprInst& ei) const
{
    NodeSet nodeSet;
    if (nodeTest(context.node()))
        nodeSet += context.node();
    collectDescendants(ei, nodeSet, context.node());
    return new NodeSetValue(nodeSet);
}

void AxisExpr::collectFollowings(ExprInst& ei,
                                 NodeSet& nodeSet,
                                 GroveLib::Node* parent,
                                 GroveLib::Node* child) const
{
    ei.registerDosVisitor(parent, NV::NOTIFY_CHILD_INSERTED |
        NV::NOTIFY_CHILD_REMOVED|NV::NOTIFY_FORCE_MODIFIED);
    GroveLib::Node* node = child->nextSibling();
    while (node) {
        if (nodeTest(node))
            nodeSet += node;
        collectDescendants(ei, nodeSet, node);
        node = node->nextSibling();
    }
    if (parent->parent())
        collectFollowings(ei, nodeSet, parentNode(parent), parent);
}

template<> ConstValueImplPtr AxisFollowingExpr::eval(const NodeSetItem& context,
                                                     ExprInst& ei) const

{
    NodeSet nodeSet;
    if (context.node()->parent())
        collectFollowings(ei, nodeSet, parentNode(context.node()),
            context.node());
    return new NodeSetValue(nodeSet);
}

template<> ConstValueImplPtr AxisFsExpr::eval(const NodeSetItem& context,
                                              ExprInst& ei) const
{
    NodeSet nodeSet;
    GroveLib::Node* parent = parentNode(context.node());
    ei.registerVisitor(parent, NV::NOTIFY_CHILD_INSERTED |
        NV::NOTIFY_CHILD_REMOVED | NV::NOTIFY_FORCE_MODIFIED);
    GroveLib::Node* node = context.node()->nextSibling();
    while (node) {
        if (nodeTest(node))
            nodeSet += node;
        node = node->nextSibling();
    }
    return new NodeSetValue(nodeSet);
}

template<> ConstValueImplPtr AxisParentExpr::eval(const NodeSetItem& context,
                                                  ExprInst&) const

{
    NodeSet nodeSet;
    GroveLib::Node* parent = parentNode(context.node());
    if (parent && nodeTest(parent))
        nodeSet += parent;
    return new NodeSetValue(nodeSet);
}

void AxisExpr::collectPrecedings(ExprInst& ei,
                                 NodeSet& nodeSet,
                                 GroveLib::Node* parent,
                                 GroveLib::Node* child) const
{
    ei.registerDosVisitor(parent, NV::NOTIFY_CHILD_INSERTED |
        NV::NOTIFY_CHILD_REMOVED|NV::NOTIFY_FORCE_MODIFIED);
    if (parent->parent())
        collectPrecedings(ei, nodeSet, parentNode(parent), parent);
    GroveLib::Node* node = parent->firstChild();
    while (node && (node != child)) {
        if (nodeTest(node))
            nodeSet += node;
        collectDescendants(ei, nodeSet, node);
        node = node->nextSibling();
    }
}

template<> ConstValueImplPtr AxisPrecedingExpr::eval(const NodeSetItem& context,
                                                     ExprInst& ei) const
{
    NodeSet nodeSet;
    if (context.node()->parent())
        collectPrecedings(ei, nodeSet, parentNode(context.node()),
            context.node());
    return new NodeSetValue(nodeSet);
}

template<> ConstValueImplPtr AxisPsExpr::eval(const NodeSetItem& context,
                                              ExprInst& ei) const
{
    NodeSet nodeSet;
    GroveLib::Node* parent = parentNode(context.node());
    ei.registerVisitor(parent, NV::NOTIFY_CHILD_INSERTED |
        NV::NOTIFY_CHILD_REMOVED | NV::NOTIFY_FORCE_MODIFIED);
    GroveLib::Node* node = context.node()->prevSibling();
    while (node) {
        if (nodeTest(node))
            nodeSet += node;
        node = node->prevSibling();
    }
    return new NodeSetValue(nodeSet);
}

template<> ConstValueImplPtr AxisRootExpr::eval(const NodeSetItem& context,
                                                ExprInst&) const
{
    NodeSet nodeSet;
    GroveLib::Node* node = context.node()->root();
    if (nodeTest(node))
        nodeSet += node;
    return new NodeSetValue(nodeSet);
}

template<> ConstValueImplPtr AxisSelfExpr::eval(const NodeSetItem& context,
                                                ExprInst&) const
{
    NodeSet nodeSet;
    if (nodeTest(context.node()))
        nodeSet += context.node();
    return new NodeSetValue(nodeSet);
}

} // namespace Xpath
