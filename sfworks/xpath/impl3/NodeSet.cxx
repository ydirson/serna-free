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
#include "xpath/impl3/debug.h"
#include "xpath/NodeSet.h"
#include "xpath/ValueHolder.h"
#include "grove/Node.h"

#include "common/String.h"
#include "common/stack_buffer.h"

#include <algorithm>

namespace {

typedef const Xpath::NodeSetItem* node_p;

inline bool nodeLessThan(node_p a, node_p b)
{
    COMMON_NS::XTreeNodeCmpResult r = a->node()->comparePos(b->node());
    if (r <= COMMON_NS::EQUAL)
        return a->node()->nodeName() < b->node()->nodeName();
    return COMMON_NS::LESS == r;
}

class DocOrderCmp {
public:
    bool operator()(node_p a, node_p b) const {
        return nodeLessThan(a, b);
    }
};

} // namespace


USING_COMMON_NS;

namespace Xpath {

NodeSet::NodeSet(NodeSet& other)
{
    reassign(other);
}

void NodeSet::reassign(NodeSet& other)
{
    realSize_    = other.realSize_;
    nodes_.assign(other.nodes_);
    other.realSize_ = 0;
    for (NodeSetItem* n = realFirst(); n; n = n->realNext())
        n->parent_ = this;
}

bool NodeSet::operator==(const NodeSet& nodeSet) const
{
    if (nodeSet.realSize() != realSize())
        return false;
    const NodeSetItem* n1 = first();
    const NodeSetItem* n2 = nodeSet.first();
    for (; n1 && n2; ) {
        if (n1->node() != n2->node())
            return false;
        n1 = n1->next();
        n2 = n2->next();
    }
    if (n1 || n2)
        return false;
    return true;
}

void NodeSet::operator+=(const NodeSet& nodeSet)
{
    const NodeSetItem* n = nodeSet.realFirst();
    for (; n; n = n->realNext())
        *this += n->node();
}

void NodeSet::copyTo(NodeSet& other) const
{
    other.nodes_.destroyAll();
    for (const NodeSetItem* it = realFirst(); it; it = it->realNext())
        other.nodes_.push_back(new NodeSetItem(&other, it->node(), it->pos()));
    other.realSize_    = realSize_;
}

void NodeSet::docOrderSort()
{
    if (0 == contextSize())
        return;
    STACK_BUFFER(temp, NodeSetItem*, realSize() + 1);
    NodeSetItem** tempend = temp + realSize();
    NodeSetItem** ni = temp;
    for (NodeSetItem* n = realFirst(); n; n = n->realNext())
        *ni++ = n;
    std::stable_sort(temp, tempend, DocOrderCmp());
    nodes_.clear();
    uint pos = 0;
    for (ni = temp; ni < tempend; ++ni) {
        nodes_.push_back(*ni);
        if (!isSpecialNode((*ni)->node()))
            ++pos;
        (*ni)->position_ = pos;
    }
}

void NodeSet::dump() const
{
    DDINDENT;
    DDBG << "NodeSet: " << realSize() << " items (context-size: "
        << contextSize() << ")" << std::endl;
    DDINDENT;
    for (const NodeSetItem* n = realFirst(); n; n = n->realNext())
        DBG(XPATH.ADDR) << "(" << n << ")" << n->node() << "/"
                        << n->node()->nodeName().utf8() <<"/"
                        << n->size() << ":" << n->pos() << std::endl;
}

} // namespace Xpath
