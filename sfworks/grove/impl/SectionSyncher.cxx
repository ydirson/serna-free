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
#include "grove/EntityDecl.h"
#include "grove/Nodes.h"
#include "grove/GroveSection.h"
#include "grove/GroveSectionRoot.h"
#include "grove/RedlineUtils.h"
#include "grove/EntityReferenceTable.h"
#include "grove/SectionSyncher.h"
#include "grove/grove_trace.h"
#include "grove/udata.h"
#include "common/asserts.h"
#include "common/XTreeIterator.h"

namespace GroveLib {

typedef LineLocExt EntityExtType;

// Optimizations todo:
// 1. do not set extensions on special nodes (SSEP etc)
// 2. optimize DF insertion (see note below)

static inline EntityReferenceStart* get_entity_ext(const Node* n)
{
    if (!n->nodeExt())
        return 0;
    EntityNodeExt* ex = n->nodeExt()->asEntityNodeExt();
    return ex ? ex->getErs() : 0;
}

static void set_entity_ext(const Node* n, EntityReferenceStart* ers)
{
    DBG_IF(GROVE.SSYNC) {
        DDBG << "Setting entity ext on node:\n";
        n->dump();
        DDBG << "ERS: "; ers->dump();
    }
    if (n->nodeExt()) {
        EntityNodeExt* ex = n->nodeExt()->asEntityNodeExt();
        if (ex) 
            return ex->setErs(ers);
    }
    EntityNodeExt* ex = new EntityExtType;
    ex->setErs(ers);
    n->setNodeExt(ex);
}

static inline GroveSectionEnd* sect_end(const Node* n)
{
    return static_cast<const GroveSectionStart*>(n)->getSectEnd();
}

////////////////////////////////////

static void update_sect_ers(EntityReferenceStart* ers);
static void set_ers_on_subtree(EntityReferenceStart* ers, Node* n);

static inline void checked_ers_update(EntityReferenceStart* ers, Node*& n)
{
    if (n->nodeType() == Node::ENTITY_REF_START_NODE) {
        update_sect_ers(static_cast<EntityReferenceStart*>(n));
        n = sect_end(n);
    } else {
        set_ers_on_subtree(ers, n);
        n = n->nextSibling();
    }
}

static inline void set_ers_on_subtree(EntityReferenceStart* ers, Node* n)
{
    set_entity_ext(n, ers);
    for (n = n->firstChild(); n; )
        checked_ers_update(ers, n);
}

static inline void update_sect_ers(EntityReferenceStart* ers)
{
    set_entity_ext(ers, ers);
    Node* n = ers->nextSibling();
    if (ers->getSectEnd() == n)
        return set_entity_ext(n, ers);
    while (n && n != ers->getSectEnd()) 
        checked_ers_update(ers, n);
    set_entity_ext(ers->getSectEnd(), ers);
}

////////////////////////////////////

static void update_sect_rss(RedlineSectionStart* rss, bool updateAll = false);
static void set_rss_on_subtree(RedlineSectionStart* rss, 
                               Node* n, bool updateAll = false);

static inline void checked_rss_update(RedlineSectionStart* rss, 
                                      Node*& n,
                                      bool updateAll) 
{
    if (n->nodeType() == Node::REDLINE_START_NODE) {
        RedlineSectionStart* lrss = static_cast<RedlineSectionStart*>(n);
        if (updateAll)
            update_sect_rss(lrss, updateAll);
        n = sect_end(n);
    } else if (n->nodeType() == Node::REDLINE_END_NODE) {
        set_redline_info(n, redline_info(n->prevSibling()));
        n = n->nextSibling();
    } else {
        set_rss_on_subtree(rss, n, updateAll);
        n = n->nextSibling();
    }
}

static inline void set_rss_on_subtree(RedlineSectionStart* rss, 
                               Node* n, bool updateAll)
{
    set_redline_info(n, rss ? rss->redlineId() : 0);
    for (n = n->firstChild(); n; )
        checked_rss_update(rss, n, updateAll);
}

static inline void update_sect_rss(RedlineSectionStart* rss, bool updateAll)
{
    set_redline_info(rss, rss->redlineId());
    Node* n = rss->nextSibling();
    while (n && n != rss->getSectEnd()) 
        checked_rss_update(rss, n, updateAll);
    set_redline_info(rss->getSectEnd(), rss->redlineId());
}

////////////////////////////////////

static void set_default(const Node* node, bool set_redline)
{
    Node* n = node->prevSibling();
    if (0 == n)
        n = node->parent();
    // entity
    EntityReferenceStart* ers = get_ers(n);
    if (n->nodeType() == Node::ENTITY_REF_END_NODE)
        ers = static_cast<EntityReferenceStart*>(ers->getSectParent());
    set_ers_on_subtree(ers, const_cast<Node*>(node));
    if (!set_redline)
        return;
    // redline
    RedlineSectionStart* rss = get_redline_section(n);
    if (rss && n->nodeType() == Node::REDLINE_END_NODE)
        rss = static_cast<RedlineSectionStart*>(rss->getSectParent());
    set_rss_on_subtree(rss, const_cast<Node*>(node), set_redline);
}

void SectionSyncher::childInserted(const Node* node)
{
    switch (node->nodeType()) {
        case Node::ENTITY_REF_START_NODE:
            set_entity_ext(node, const_cast<EntityReferenceStart*>(
                static_cast<const EntityReferenceStart*>(node)));
            break;

        // Note: ERS must be already inserted
        case Node::ENTITY_REF_END_NODE:
            update_sect_ers(static_cast<EntityReferenceStart*>
                (static_cast<const EntityReferenceEnd*>(node)->getSectStart()));
            break;

        case Node::REDLINE_START_NODE: {
            RedlineSectionStart* rss = 
                static_cast<RedlineSectionStart*>(const_cast<Node*>(node));
            rss->assignRedlineId();
            set_redline_info(rss, rss->redlineId());
            set_default(node, false);
            break;
        }
        case Node::REDLINE_END_NODE: {
            RedlineSectionStart* rss = static_cast<RedlineSectionStart*>
                (static_cast<const RedlineSectionEnd*>(node)->getSectStart());
            set_redline_info(const_cast<Node*>(node), rss->redlineId());
            update_sect_rss(rss, false);
            break;
        }
        default: 
            set_default(node, true);
            break;
    }
}

static inline void ers_removed(const Node* pnode, const Node* child)
{
    const GroveSectionStart* gss =
        static_cast<const GroveSectionStart*>(child);
    Node* n = gss->prevSibling();
    EntityReferenceStart* ners = get_entity_ext(n ? n : pnode);
    if (n && n->nodeType() == Node::ENTITY_REF_END_NODE)
        ners = ERS_CAST(ners->getSectParent());
    n = gss->nextSibling();
    while (n && n != gss->getSectEnd()) 
        checked_ers_update(ners, n);
}

static inline void rss_removed(const Node* pnode, const Node* child)
{
    const GroveSectionStart* gss =
        static_cast<const GroveSectionStart*>(child);
    Node* n = gss->prevSibling();
    RedlineSectionStart* nrss = get_redline_section(n ? n : pnode);
    if (n && n->nodeType() == Node::REDLINE_END_NODE)
        nrss = static_cast<RedlineSectionStart*>(nrss->getSectParent());
    n = gss->nextSibling();
    while (n && n != gss->getSectEnd()) 
        checked_rss_update(nrss, n, false);
}

// todo: maybe it is better to track ERE deletion - in this case we
// can rule out unnecessary traversal during removeGroup.
void SectionSyncher::childRemoved(const Node* pnode,
                                 const Node* child)
{
    if (child->nodeType() == Node::ENTITY_REF_START_NODE)
        return ers_removed(pnode, child);
    if (child->nodeType() == Node::REDLINE_START_NODE)
        return rss_removed(pnode, child);
}

SectionSyncher::SectionSyncher(GroveSectionRoot* gsr)
    : GroveVisitor(gsr)
{
    RT_ASSERT(gsr->nodeType() == Node::DOCUMENT_NODE);
    // todo: move this to grove builder, for better performance
    EntityReferenceStart* ers = gsr->ers();
    set_ers_on_subtree(ers, static_cast<Document*>(gsr)->documentElement());
    sync_gsr_rss(gsr);
    gsr->registerVisitor(this);
}

static void assign_rid(RedlineSectionStart* rss)
{
    rss->assignRedlineId();
    rss = static_cast<RedlineSectionStart*>(rss->getFirstSect());
    for (; rss; rss = static_cast<RedlineSectionStart*>(rss->getNextSect()))
        assign_rid(rss);
}

void sync_gsr_rss(GroveSectionRoot* gsr)
{
    GroveSectionStart* gss = gsr->rss();
    if (0 == gss)
        return;
    for (gss = gss->getFirstSect(); gss; gss = gss->getNextSect()) {
        RedlineSectionStart* rss = static_cast<RedlineSectionStart*>(gss);
        if (0 == rss->redlineId())
            assign_rid(rss);
        update_sect_rss(rss, true);
    }
}

SectionSyncher::~SectionSyncher()
{
}

///////////////////////////////////////////////////////////////////////////

GROVE_EXPIMP EntityReferenceStart* get_ers(const Node* n)
{
    if (0 == n)
        return 0;
    Node::NodeType nt = n->nodeType();
    switch (nt) {
        case Node::ATTRIBUTE_NODE:
            return get_ers(static_cast<const Attr*>(n)->element());

        case Node::ENTITY_REF_END_NODE:
            return static_cast<EntityReferenceStart*>
                (static_cast<const EntityReferenceEnd*>(n)->getSectStart());

        case Node::ENTITY_REF_START_NODE:
            return static_cast<EntityReferenceStart*>
                (const_cast<Node*>(n));

        default:
            break;
    }
    EntityReferenceStart* ers = get_entity_ext(n);
    return ers ? ers : n->getGSR()->ers();
}

EntityDecl* get_entity_decl(const Node* n)
{
    return get_ers(n)->entityDecl();
}


} // namespace GroveEditor
