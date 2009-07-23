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
/*! \file Implements Node member functions.
 */

#include "grove/grove_defs.h"
#include "grove/grove_trace.h"
#include "grove/Node.h"
#include "grove/Nodes.h"
#include "grove/XNodePtr.h"
#include "grove/Grove.h"
#include "grove/EntityReferenceTable.h"
#include "grove/GroveSectionRoot.h"
#include "grove/impl/gsutils.h"
#include "common/Vector.h"
#include "common/asserts.h"
#include "common/OwnerPtr.h"
#include "common/TreelocRep.h"

GROVE_NAMESPACE_BEGIN
USING_COMMON_NS

void Node::setGSR(GroveSectionRoot* g)
{
    if (gsr_ == g)
        return;
    gsr_ = g;
    if (nodeType() == ELEMENT_NODE) {
        Attr* a = static_cast<Element*>(this)->attrs().firstChild();
        for (; a; a = a->nextSibling())
            a->setGSR(g);
    }
    for (Node* n = firstChild(); n; n = n->nextSibling())
        n->setGSR(g);
}

Grove* Node::grove() const
{
    return gsr_ ? gsr_->grove() : 0;
}

void Node::saveAsXml(GroveSaverContext& gsc, int cFlags) const
{
    for (const Node* n = firstChild(); n; ) {
        n->saveAsXml(gsc, cFlags);
        if (n->nodeType() != ENTITY_REF_START_NODE)
            n = n->nextSibling();
        else
            n = CONST_ERS_CAST(n)->getSectEnd();
    }
}

const String& Node::nodeName() const
{
    static String baseNN("#base-node");
    return baseNN;
}

GROVE_EXPIMP Node::NodeType section_start_node_type_map[] =
{
    Node::MARKED_SECTION_START_NODE, Node::ENTITY_REF_START_NODE,
    Node::REDLINE_START_NODE
};

GROVE_EXPIMP Node::NodeType section_end_node_type_map[] =
{
    Node::MARKED_SECTION_END_NODE, Node::ENTITY_REF_END_NODE,
    Node::REDLINE_END_NODE
};

static bool getSectionPair(int t, /* grove section type*/
                           Node* n,
                           GroveSectionStart*& ss,
                           GroveSectionEnd*& se)
{
    Node* ps = n->prevSibling();
    n = ps ? ps : parentNode(n);
    if (0 == n)
        return false;
    Node::NodeType nt;
    const Node::NodeType start_type(section_start_node_type_map[t]),
        end_type(section_end_node_type_map[t]);
    do {
        nt = n->nodeType();
        if (nt == start_type) {
            ss = static_cast<GroveSectionStart*>(n);
            se = 0;
            return true;
        }
        if (nt == end_type) {
            se = static_cast<GroveSectionEnd*>(n);
            ss = 0;
            return true;
        }
        ps = n->prevSibling();
        n = ps ? ps : parentNode(n);
    } while (n);
    return false;
}

void Node::insertToEnclosingSection(GroveSectionStart* ngs)
{
    GroveSectionStart* gs;
    GroveSectionEnd* ge;
    const int sectType = ngs->nodeType() & SECT_TYPE_MASK;
    if (getSectionPair(sectType, this, gs, ge)) {
        if (gs) {
            if (gs->getFirstSect())
                gs->getFirstSect()->insertSectBefore(ngs);
            else
                gs->appendSect(ngs);
            return;
        }
        if (ge) {
            gs = ge->getSectStart()->getNextSect();
            if (gs)
                gs->insertSectBefore(ngs);
            else
                ge->getSectStart()->getSectParent()->appendSect(ngs);
        }
        return;
    }
    GroveSectionRoot* gsr = getGSR();
    if (!gsr)
        return;
    gs = gsr->sections_[sectType];
    if (!gs) 
        gs = gsr->ensureSectionRoot(sectType);
    if (gs->getFirstSect()) 
        gs->getFirstSect()->insertSectBefore(ngs);
    else
        gs->appendSect(ngs);
}

namespace {

struct SectData {
    struct Data {
        GroveSectionStart* ss;
        uint               ssLevel;
    } sections[Node::MAX_SECTION_ID];
    uint    level;

    SectData() 
        : level(0)
    {
        for (uint i = 0; i < Node::MAX_SECTION_ID; ++i) {
            sections[i].ss = 0;
            sections[i].ssLevel = ~0;
        }
    }
};

} // namespace

static void do_find_sections(Node* n, SectData& sd)
{
    ++sd.level;
    if ((n->nodeType() & (Node::SECT_NODE_MASK|Node::SECT_END_BIT)) ==
        Node::SECT_NODE_MASK) {
            SectData::Data& d = 
                sd.sections[n->nodeType() & Node::SECT_TYPE_MASK];
            if (sd.level < d.ssLevel) {
                d.ss = static_cast<GroveSectionStart*>(n);
                d.ssLevel = sd.level;
            }
            return;
    }
    if (n->nodeType() == Node::ELEMENT_NODE) {
        Attr* a = static_cast<Element*>(n)->attrs().firstChild();
        for (; a; a = a->nextSibling())
            do_find_sections(a, sd);
    }
    for (Node* c = n->firstChild(); c; c = c->nextSibling())
        do_find_sections(c, sd);
    --sd.level;
}

DocumentFragment* Node::takeAsFragment(const Node* endNode)
{
    if (0 == endNode)
        endNode = this;
    if (!check_sections(this, endNode))
        return 0;
    GroveSectionRoot* gsr = getGSR();
    OwnerPtr<DocumentFragment> frag(new DocumentFragment);
    frag->setGrove(grove());
    NodePtr n = this;
    Node* ns = 0;
    for (;;) {
        if (n.isNull())
            return 0;
        ns = n->nextSibling();
        n->setGSR(frag.pointer());
        n->remove();
        frag->appendChild(n.pointer());
        if (n == endNode)
            break;
        n = ns;
    }
    SectData sd;
    GroveSectionStart *gss = 0, *ngss = 0;
    do_find_sections(frag.pointer(), sd);
    for (uint i = 0; i < MAX_SECTION_ID; ++i) {
        gss = sd.sections[i].ss;
        while (gss) {
            if (gss->getGSR() != frag.pointer())
                break;
            ngss = gss->getNextSect();
            gss->sectRemove();
            frag->ensureSectionRoot(i)->appendSect(gss);
            gss = ngss;
        }
        if (frag->sections_[i])
            frag->sections_[i]->processTakeAsFragment(gsr->sections_[i]);
    }
    return frag.release();
}

DocumentFragment* Node::copyAsFragment(const Node* endNode) const
{
    if (0 == endNode)
        endNode = this;
    if (!check_sections(this, endNode))
        return 0;
    OwnerPtr<DocumentFragment> frag(new DocumentFragment);
    frag->setGrove(grove());
    Node* cn = const_cast<Node*>(this);
    for (;;) {
        if (0 == cn)
            return 0;
        NodePtr cn2 = cn->cloneNode(true, frag.pointer());
        frag->appendChild(cn2.pointer());
        if (cn == endNode)
            break;
        cn = cn->nextSibling();
    }
    return frag.release();
}

/////////////////////////////////////////////////////////////////////////

Node* Node::copy(Node* newParent) const
{
    Node* p = new Node;
    p->assign(this, newParent);
    return p;
}

Node* Node::cloneNode(bool deep, Node* parent) const
{
    Node* np = copy(parent);
    if (deep)
        copy_children(this, np);
    return np;
}

void Node::copy_children(const Node* from, Node* to)
{
    Node* np = from->firstChild();
    if (!np)
        return;
    for (; np; np = np->nextSibling()) {
        Node* nc = np->copy(to);
        to->appendChild(nc);
        copy_children(np, nc);
    }
}

Node* Node::getPrevClosure(NodeType t) const
{
    Node* n = const_cast<Node*>(this);
    Node* ps;
    do {
        if (n->nodeType() == t)
            return n;
        ps = n->prevSibling();
        n = ps ? ps : parentNode(n);
    } while(n);
    return n;
}

Node* Node::getNextClosure(NodeType t) const
{
    Node* n = const_cast<Node*>(this);
    Node* ps;
    do {
        if (n->nodeType() == t)
            return n;
        ps = n->nextSibling();
        n = ps ? ps : parentNode(n);
    } while(n);
    return n;
}

void Node::subtree(Vector<Node*>& l) const
{
    if (!firstChild())
        return;
    build_subtree(const_cast<Node*>(this), l);
}

void Node::build_subtree(Node* n, Vector<Node*>& l)
{
    l.push_back(n);
    for (Node* np = n->firstChild(); np; np = np->nextSibling())
        build_subtree(np, l);
}

void Node::dumpInherited() const
{
}

///////////////////////////////////////////////////////////////////

void Node::registerNodeVisitor(NodeVisitor* nv, short bitmask)
{
    Grove* g = grove();
    if (g && !g->registerVisitors())
        return;
    VisitorLink* vlink = vlist_.firstChild();
    for (; vlink; vlink = vlink->nextSibling()) {
        if (static_cast<NodeVisitorLink*>(vlink)->nodeVisitor() == nv) {
            static_cast<NodeVisitorLink*>(vlink)->addVisitorMask(bitmask);
            return;
        }
    }
    NodeVisitorLink* new_link = new NodeVisitorLink(bitmask);
    nv->nodes_.appendChild(new_link);
    vlist_.appendChild(new_link);
}

void Node::deregisterNodeVisitor(NodeVisitor* nv)
{
    VisitorLink* vlink = vlist_.firstChild();
    for (; vlink; vlink = vlink->nextSibling()) {
        if (static_cast<NodeVisitorLink*>(vlink)->nodeVisitor() == nv) {
            delete static_cast<NodeVisitorLink*>(vlink);
            return;
        }
    }
}

void Node::deregisterAllNodeVisitors()
{
    VisitorLink* vlink;
    while ((vlink = vlist_.firstChild()))
        delete static_cast<NodeVisitorLink*>(vlink);
}

////////////////////////////////////////////////////////////////////

Node::~Node()
{
    notifyNodeDestroyed(this);
    deregisterAllNodeVisitors();
    gsr_ = 0;
}

#define NODE_NOTIFY_PROTO(base, func, mask) \
        if (base->gsr_) \
            base->gsr_->gsr_##func ; \
        VisitorLink* vlink = base->vlist_.firstChild(); \
        for (; vlink; vlink = vlink->nextSibling()) { \
            NodeVisitorLink& nvl = *static_cast<NodeVisitorLink*>(vlink); \
            if (nvl.visitorMask() & mask) \
                nvl.nodeVisitor()->func ; \
        }

void Node::notifyNodeDestroyed(Node* n)
{
    NODE_NOTIFY_PROTO(n, nodeDestroyed(n), NodeVisitor::NOTIFY_NODE_DESTROYED);
}

void Node::notifyChildInserted(Node* n)
{
    switch (n->nodeType()) {
        case DOCUMENT_FRAGMENT_NODE: {
            DocumentFragmentPtr dfp = static_cast<DocumentFragment*>(n);
            Node* p = n->parent();
            GroveSectionRoot* gsr = p->getGSR();
            Node* ns = dfp->nextSibling();
            Node* ctx = ns ? ns : p;
            dfp->remove();
            NodePtr ccp;
            GroveSectionStart *gss, *sroot;
            for (uint i = 0; i < MAX_SECTION_ID; ++i) {
                sroot = dfp->sections_[i];
                if (0 == sroot)
                    continue;
                gss = sroot->getFirstSect();
                if (gss) {
                    gss->sectRemove();
                    ctx->insertToEnclosingSection(gss);
                    GroveSectionStart* sgss = gss;
                    while ((gss = sroot->getFirstSect())) {
                        gss->sectRemove();
                        sgss->insertSectAfter(gss);
                        sgss = gss;
                    }
                }
                sroot->processInsertFragment(p->getGSR()->sections_[i]);
            }
            if (ns) {
                while ((ccp = dfp->firstChild())) {
                    ccp->setGSR(gsr);
                    ccp->remove();
                    ns->insertBefore(ccp.pointer());
                }
            } else {
                while ((ccp = dfp->firstChild())) {
                    ccp->setGSR(gsr);
                    ccp->remove();
                    p->appendChild(ccp.pointer());
                }
            }
            ccp = 0;
            return;
        }
        case ATTRIBUTE_NODE: {
            Attr* attr = static_cast<Attr*>(n);
            Element* elem = attr->element();
            if (elem->getGSR())
                attr->setGSR(elem->getGSR());
            elem->notifyAttributeAdded(attr);
            break;
        }

        default: {
            Node* p = n->parent();
            if (p->getGSR())
                n->setGSR(p->getGSR());
            NODE_NOTIFY_PROTO(p, childInserted(n),
                NodeVisitor::NOTIFY_CHILD_INSERTED);
            break;
        }
    }
}

void Node::notifyChildRemoved(XLPT* xp, Node* n)
{
    Node* p = static_cast<Node*>(xp);
    switch (n->nodeType()) {
        case DOCUMENT_FRAGMENT_NODE:
            return;
        case ATTRIBUTE_NODE: {
            Element* elem = Element::calculateAttrParent(xp);
            if (elem->parent())
                elem->notifyAttributeRemoved(static_cast<Attr*>(n));
            return;
        }
        case ENTITY_REF_START_NODE:
        case MARKED_SECTION_START_NODE: 
        case REDLINE_START_NODE: {
            // below means: don't do anything if node is reassigned to fragment
            if (static_cast<Node*>(p)->getGSR() != n->getGSR())
                break;
            GroveSectionStart* gss = static_cast<GroveSectionStart*>(n);
            if (!gss->getSectParent() || !gss->getSectParent()->getSectEnd())
                break;
            GroveSectionStart* gss2;
            while ((gss2 = gss->getFirstSect())) {
                gss2->sectRemove();
                gss->insertSectBefore(gss2);
            }
            break;
        }
        default:
            break;
    }
    NODE_NOTIFY_PROTO(static_cast<Node*>(p),
        childRemoved(static_cast<Node*>(p), n),
            NodeVisitor::NOTIFY_CHILD_REMOVED);
}

void Node::genericNotify(void* vp) const
{
    NODE_NOTIFY_PROTO(this, genericNotify(this, vp),
        NodeVisitor::NOTIFY_GENERIC);
}

void Node::notifyForceModified() const
{
    NODE_NOTIFY_PROTO(this, forceModified(this), 
        NodeVisitor::NOTIFY_FORCE_MODIFIED);
}

// Notifications for Element are passed over to here

void Element::notifyAttributeAdded(Attr* n) const
{
    NODE_NOTIFY_PROTO(this, attributeAdded(n),
        NodeVisitor::NOTIFY_ATTRIBUTE_ADDED);
}

void Element::notifyAttributeChanged(Attr* n) const
{
    NODE_NOTIFY_PROTO(this, attributeChanged(n),
        NodeVisitor::NOTIFY_ATTRIBUTE_CHANGED);
}

void Element::notifyAttributeRemoved(Attr* n) const
{
    NODE_NOTIFY_PROTO(this, attributeRemoved(this, n),
        NodeVisitor::NOTIFY_ATTRIBUTE_REMOVED);
}

void NodeWithNamespace::notifyNsMappingChanged(const String& prefix) const
{
    NODE_NOTIFY_PROTO(this, nsMappingChanged(this, prefix),
        NodeVisitor::NOTIFY_NS_MAPPING_CHANGED);
}

void Text::notifyChanged()
{
    NODE_NOTIFY_PROTO(this, textChanged(this),
        NodeVisitor::NOTIFY_TEXT_CHANGED);
}

//////////////////////////////////////////////////////////////////

int Node::node_size() const
{
    int s = vlist_.countChildren() * sizeof(NodeVisitor);
    for (Node* n = firstChild(); n; n = n->nextSibling())
        s += n->memSize();
    return s;
}

int Node::memSize() const
{
    return sizeof(*this) + node_size();
}

#ifdef GROVE_DEBUG
void Node::dump() const
{
    DDINDENT;
    ulong children = countChildren();

    DDBG << "N:" << this << " nm:" << abr(nodeName())
         << " type:" << (int)nodeType() << " pt:" << parent()
         << " gsr:" << getGSR() << " C:" << getRefCnt() << std::endl;

    DDINDENT;
    COMMON_NS::TreelocRep tl;
    if (nodeType() != ATTRIBUTE_NODE && parent()
        && parent()->nodeType() != ATTRIBUTE_NODE)
            treeLoc(tl);
    
    String udata_str = String::number(udata(), 16);
    DDBG << "children#" << children << " s#" << siblingIndex()
         << " UD: " << udata_str 
         << " treeloc:" << tl.toString() << std::endl; 

    dumpInherited();

    if (!nodeExt_.isNull())
        nodeExt_->dump();
}
#else
void Node::dump() const
{
}
#endif // GROVE_DEBUG

#ifdef GROVE_DEBUG
void Node::dumpSubtree(const Node* n)
{
    n->dump();
    DDINDENT;
    for (Node* c = n->firstChild(); c; c = c->nextSibling())
        dumpSubtree(c);
}
#else
void Node::dumpSubtree(const Node*)
{
}
#endif // GROVE_DEBUG

PRTTI_BASE_STUB(Node, NodeWithNamespace)
PRTTI_BASE_STUB(Node, GroveSectionRoot)
PRTTI_BASE_STUB_NS(Node, XsltResult, Xslt)

GROVE_NAMESPACE_END
