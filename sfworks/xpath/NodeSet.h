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
#ifndef XPATH_NODE_SET_H
#define XPATH_NODE_SET_H

#include "xpath/xpath_defs.h"
#include "xpath/ValueHolder.h"
#include "common/STQueue.h"
#include "common/DefaultSmallObject.h"
#include "common/SubAllocator.h"
#include "grove/Node.h"

namespace Xpath {

class NodeSet;

class XPATH_EXPIMP NodeSetItem : public Common::STQueueItem<NodeSetItem> {
public:
    typedef Common::STQueueItem<NodeSetItem> STI;
    USE_SUBALLOCATOR

    NodeSetItem(NodeSet* p, GroveLib::Node* node, uint position)
        : node_(node), position_(position), parent_(p) {}
    NodeSetItem(GroveLib::Node* n = 0)
        : node_(n), position_(1), parent_(0) {}

    GroveLib::Node*     node() const { return node_; }
    uint                pos() const  { return position_; }
    uint                size() const;

    void                setNode(GroveLib::Node* n) { node_ = n; }
    void                setPos(uint pos) { position_ = pos; }

    NodeSetItem*        next() const;
    NodeSetItem*        realNext() const { return STI::next(); }

private:
    friend class NodeSet;
    NodeSetItem(const NodeSetItem&);
    void operator=(const NodeSetItem&);

    GroveLib::Node*         node_;
    uint                    position_;
    const NodeSet*          parent_;
};

class XPATH_EXPIMP NodeSet {
public:
    XPATH_OALLOC(NodeSet);

    typedef COMMON_NS::STQueue<NodeSetItem> NodeList;
    typedef NodeList::iterator NodeIterator;

    NodeSet() : realSize_(0) {}
    ~NodeSet() { nodes_.destroyAll(); }

    // This constructor really swaps node sets
    NodeSet(NodeSet&);
    void        reassign(NodeSet&);

    // Returns size of the node set
    size_t      realSize() const { return realSize_; }

    // Returns context size
    size_t      contextSize() const 
        { return nodes_.last() ? nodes_.last()->pos() : 0; }

    // Returns true when nodes in two nodesets are equal
    bool        operator==(const NodeSet& nodeSet) const;

    // Appens another nodeset (only nodes, not values!)
    void        operator+=(const NodeSet& nodeSet);

    // Adds node to node set
    void        operator+=(GroveLib::Node* node)
    {
        if (isIgnoredNode(node))
            return;
        if (isSpecialNode(node))
            nodes_.push_back(new NodeSetItem(this, node, contextSize()));
        else
            nodes_.push_back(new NodeSetItem(this, node, contextSize() + 1));
        ++realSize_;
    }
    void        append(NodeSetItem* nsi)
    {
        nodes_.push_back(nsi);
        nsi->parent_ = this;
        ++realSize_;
    }
    /// Returns pointer to the first node set item
    NodeSetItem*    first() const
    {
        NodeSetItem* nsi = realFirst();
        if (0 == nsi)
            return 0;
        if (isSpecialNode(nsi->node()))
            return nsi->next();
        return nsi;
    }
    NodeSetItem*    realFirst() const
    {
        return const_cast<NodeSet*>(this)->nodes_.first();
    }
    void                    dump() const;

    /// release the list (don't delete the items)
    void                    release()
    {
        nodes_.clear();
        realSize_ = 0;
    }
    void                    clear()
    {
        nodes_.destroyAll();
        realSize_ = 0;
    }
    /// Sort nodeset in document order
    void                    docOrderSort();

    /// Copies node set to another node set (resetting it)
    void                    copyTo(NodeSet& to) const;

    /// Checks whether given node is a special node. Special nodes
    /// are included into node-sets, but do not affect other node
    /// positions and context size.
    static bool isSpecialNode(const GroveLib::Node* n)
    {
        return n->isGroveSectionNode();
    }
    /// Checks whether some node type must be completely ignored and not
    /// included into nodesets.
    static bool isIgnoredNode(const GroveLib::Node* n)
    {
        return n->nodeType() == GroveLib::Node::SSEP_NODE;
    }

private:
    void operator=(const NodeSet&);
    NodeList    nodes_;
    uint        realSize_;
};

inline uint NodeSetItem::size() const
{
    if (parent_)
        return parent_->contextSize();
    const NodeSetItem* n = realNext();
    if (0 == n)
        return pos();
    for (; n->realNext(); n = n->realNext())
        ;
    return n->pos();
}

inline NodeSetItem* NodeSetItem::next() const
{
    NodeSetItem* nsi = realNext();
    if (0 == nsi)
        return 0;
    if (NodeSet::isSpecialNode(nsi->node()))
        return nsi->next();
    return nsi;
}

} // namespace Xpath

#endif // XPATH_NODE_SET_H
