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
#ifndef GROVE_NODE_H_
#define GROVE_NODE_H_

#ifdef _MSC_VER
# pragma warning( disable : 4251 )
#endif

#include "grove/grove_defs.h"
#include "grove/Decls.h"
#include "grove/grove_exports.h"
#include "grove/Grove.h"
#include "grove/NodeExt.h"
#include "grove/NodeVisitor.h"
#include "grove/NodeVisitorLink.h"

#include "common/SernaApiRefCounted.h"
#include "common/XTreeNode.h"
#include "common/Visitor.h"
#include "common/prtti.h"

namespace Xslt {
    class XsltResult;
}

GROVE_NAMESPACE_BEGIN

class Document;
class GroveSectionStart;
class GroveSectionRoot;
class NodeWithNamespace;
class GroveSaverContext;

/*! This is a base class for all grove node types.
 */
class GROVE_EXPIMP Node : public Common::SernaApiRefCounted,
                          public COMMON_NS::XTreeNode<Node, Node> {
    friend class COMMON_NS::XListItem<Node, Node>;
    friend class COMMON_NS::XList<Node, Node>;

public:
    typedef COMMON_NS::XList<Node, Node> XLPT;
    typedef COMMON_NS::XTreeNode<Node, Node> XTN;
    typedef Common::String String;

    enum GroveSectionType {
        MARKED_SECTION   = 0,
        ENTITY_SECTION   = 1,
        REDLINE_SECTION  = 2,
        MAX_SECTION_ID
    };
    /// Node types. All node type names are DOM-compatible, where applicable.
    enum NodeType {
        UNDEFINED_NODE    = 0,
        SECT_NODE_MASK    = 020, 
        DECL_NODE_MASK    = 040,
        // normal nodes 
        ELEMENT_NODE = 1, ATTRIBUTE_NODE = 2, TEXT_NODE = 3,
        PI_NODE = 7,      COMMENT_NODE = 8,   DOCUMENT_NODE = 9,
        DOCUMENT_FRAGMENT_NODE = 11,          SSEP_NODE = 12,
        // section nodes
        SECT_END_BIT              = 010,
        SECT_TYPE_MASK            = 007,
        MARKED_SECTION_START_NODE = (SECT_NODE_MASK | MARKED_SECTION),
        MARKED_SECTION_END_NODE   =
            (SECT_NODE_MASK | SECT_END_BIT | MARKED_SECTION), 
        ENTITY_REF_START_NODE     = (SECT_NODE_MASK | ENTITY_SECTION),
        ENTITY_REF_END_NODE       =
            (SECT_NODE_MASK | SECT_END_BIT | ENTITY_SECTION),
        REDLINE_START_NODE        = (SECT_NODE_MASK | REDLINE_SECTION),
        REDLINE_END_NODE          = 
            (SECT_NODE_MASK | SECT_END_BIT | REDLINE_SECTION),
        // declaration (prolog) nodes
        DOCUMENT_PROLOG_NODE = (DECL_NODE_MASK),
        ENTITY_DECL_NODE,
        ELEMENT_DECL_NODE, 
        ATTR_DECL_NODE,
        // other nodes
        DOCUMENT_HEADING,
        CHOICE_NODE,
        MAX_NODETYPE
    };
    /// Node name - the same as name(). For DOM-compatibility only.
    virtual const String& nodeName() const;

    /// Node type.
    NodeType    nodeType() const { return (NodeType) data_.nodeType_; }

    /// Re-set node type
    void        setNodeType(NodeType t) { data_.nodeType_ = t; }

    /*! Create (possibly deep) copy of a node, setting new parent, if needed.
     *  Please note that with this method you can only safely copy nodes
     *  which are not section nodes and whose chidlren does not contain
     *  any sections or entity references. If in doubt, use copyAsFragment()
     *  instead.
     */
    Node*       cloneNode(const bool deep, Node* futureParent = 0) const;

    /*! Take nodes from current node to the \a endNode inclusive and move
     *  them into created DocumentFragment. Nodes must be on the same
     *  level (be the children of the same parent) and must not cross
     *  grove section boundaries. If any of these conditions occur, this
     *  function will do nothing and return 0.
     */
    DocumentFragment* takeAsFragment(const Node* endNode = 0);

    /*! Same as takeAsFragment(), but does deep copy of the nodes instead
     *  of moving them to DocumentFragment.
     */
    DocumentFragment* copyAsFragment(const Node* endNode = 0) const;

    /*! Find nearest enclosing grove section enclosure relative to
     *  current node. \a sectType specifies which type of closure
     *  node you're trying to find. For getPrevClosure(), only
     *  nodes with type *START_NODE make sense, and for getNextClosure()
     *  only *END_NODE's, respectively.
     */
    Node*       getPrevClosure(NodeType sectType) const;
    Node*       getNextClosure(NodeType sectType) const;

    /*! returns sequental list of all nodes in a subtree from current
     *  node downwards (BFS).
     */
    void        subtree(COMMON_NS::Vector<Node*>& subtreeList) const;

    /// Returns pointer to the governing grove.
    Grove*      grove() const;

    /// Change governing GSR for the current node and it's children
    void        setGSR(GroveSectionRoot*);

    /// Returns pointer to the governing document
    Document*   document() const { return grove()->document(); }

    /// Returns GroveSectionRoot for this node
    GroveSectionRoot* getGSR() const { return gsr_; }

    /// Returns TRUE if this node is one of GroveSection separator nodes.
    bool        isGroveSectionNode() const;

    /// Register a NodeVisitor on a node. If visitor is already there,
    /// it is not added, but it's bitmask is OR'ed.
    void        registerNodeVisitor(NodeVisitor* nv,
                                    short bitmask = NodeVisitor::NOTIFY_ALL);

    /// De-register a NodeVisitor
    void        deregisterNodeVisitor(NodeVisitor* nv);

    /// Clean up node from all node visitors
    void        deregisterAllNodeVisitors();

    /// Advanced: direct access to node visitor link objects
    VisitorLink* firstVisitorLink() const { return vlist_.firstChild(); }

    /*! Copy of a node. Node that this function DOES NOT COPY
     *  node children; use cloneNode for recursive deep copys.
     *  Note that node is not attached to the \a futureParent in
     *  any way, but copy() needs it to get correct context information.
     */
    virtual Node* copy(Node* futureParent) const;

    /// Reimplementation of XTreeNode comparePos, which handles ATTRIBUTE_NODE
    Common::XTreeNodeCmpResult comparePos(const Node* otherNode) const;

    /// Get/set user data
    uint            udata() const { return data_.udata_; }
    void            setUdata(uint v) { data_.udata_ = v; }

    /// Set/get node extensions
    void            setNodeExt(NodeExt* e) const { nodeExt_ = e; }
    NodeExt*        nodeExt() const { return nodeExt_.pointer(); }

    // Generic notification
    void            genericNotify(void* vp) const;
    void            notifyForceModified() const;

    void            dump() const;
    virtual void    dumpInherited() const;
    static void     dumpSubtree(const Node*);
    virtual int     memSize() const;

    PRTTI_DECL(NodeWithNamespace);
    PRTTI_DECL(GroveSectionRoot);
    PRTTI_DECL_NS(XsltResult, Xslt);

    struct NodeData {
        NodeData(NodeType t)
            : nodeType_(t), udata_(0) {}

        uchar   nodeType_;  // grok
        uint    udata_ : 24;
    };

    Node()
        : gsr_(0),
          data_(UNDEFINED_NODE) {}
    Node(NodeType t)
        : gsr_(0), data_(t) {}

    virtual ~Node();

    friend class Grove;
    friend class Document;
    friend class GroveSectionStart;
    friend class GroveSectionRoot;

    GROVE_OALLOC(Node);

    virtual void    accept(COMMON_NS::Visitor<Node>& v) { v.visit(*this); }

    // for internal use
    static Node*    calculateVlistParent(NodeVisitorList*);
    virtual void    saveAsXml(GroveSaverContext&, int) const;

protected:
    static void     build_subtree(Node*, COMMON_NS::Vector<Node*>&);
    static void     copy_children(const Node* from, Node* to);
    void            insertToEnclosingSection(GroveSectionStart*);
    int             node_size() const;

    void            assign(const Node* from, Node* newParent)
    {
        data_     = from->data_;
        setParent(newParent);
        if (newParent)
            gsr_ = newParent->gsr_;
        else
            gsr_ = 0;
    }
    // Notifications
    static void notifyNodeDestroyed(Node* n);
    static void notifyChildInserted(Node* n);
    static void notifyChildRemoved(XLPT* p, Node* n);
    static void xt_incRefCnt(Node* n) { n->incRefCnt(); }
    static int  xt_decRefCnt(Node* n) { return n->decRefCnt(); }

    NodeVisitorList     vlist_;     // visitor list
    GroveSectionRoot*   gsr_;       // governing doc section
    NodeData            data_;

private:
    mutable COMMON_NS::OwnerPtr<NodeExt> nodeExt_;
};

// Inlines
////////////////////////////////////////////////////////////////////////

inline bool Node::isGroveSectionNode() const
{
    return data_.nodeType_ & SECT_NODE_MASK;
}

inline Node* Node::calculateVlistParent(NodeVisitorList* p)
{
    // 16 is to kill gcc warning
    static const size_t offs = ((size_t)&((Node*)16)->vlist_) - 16;
    return (Node*)((char*)p - offs);
}

inline Node* NodeVisitorLink::visitedNode() const
{
    return Node::calculateVlistParent(VisitorLink::parent());
}

extern GROVE_EXPIMP Node::NodeType section_start_node_type_map[];
extern GROVE_EXPIMP Node::NodeType section_end_node_type_map[];

GROVE_NAMESPACE_END

#endif // GROVE_NODE_H_
