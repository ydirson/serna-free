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

#include "grove/grove_defs.h"
#include "grove/Node.h"
#include "grove/XNodePtr.h"
#include "grove/Nodes.h"
#include "grove/Grove.h"
#include "grove/GroveBuilder.h"
#include "grove/EntityReferenceTable.h"
#include "grove/EntityDecl.h"
#include "grove/EntityDeclSet.h"
#include "grove/GroveVisitor.h"
#include "grove/grove_trace.h"

GROVE_NAMESPACE_BEGIN

#define ERS sections_[ENTITY_SECTION]
#define MSS sections_[MARKED_SECTION]

GroveSectionRoot::GroveSectionRoot(NodeType t)
    : Node(t), grove_(0)
{
    for (uint i = 0; i < MAX_SECTION_ID; ++i)
        sections_[i] = 0;
    setGSR(this);
}

void GroveSectionRoot::releaseSectionData()
{
    for (uint i = 0; i < MAX_SECTION_ID; ++i) {
        GroveSectionStart* const sect = sections_[i];
        if (!sect) 
            continue;
        sect->setGSR(0);
        if (1 == sect->getRefCnt()) {
            sect->decRefCnt();
            delete sect;
        }
        sections_[i] = 0;
    }
}

EntityReferenceTable* GroveSectionRoot::ert() const
{
    return ers() ? ers()->ert() : 0;
}

EntityReferenceStart*
GroveSectionRoot::insertEntityRef(EntityDecl* decl, Node* data,
                                  Node* beforeNode, Node* parent)
{
    EntityReferenceStart* ers = new EntityReferenceStart;
    EntityReferenceEnd* ere = new EntityReferenceEnd;
    ers->setSectEnd(ere);
    ere->setSectStart(ers);
    if (beforeNode) {
        beforeNode->insertToEnclosingSection(ers);
        beforeNode->insertBefore(ers);
        if (data)
            beforeNode->insertBefore(data);
        beforeNode->insertBefore(ere);
    } else {
        parent->insertToEnclosingSection(ers);
        parent->appendChild(ers);
        if (data)
            parent->appendChild(data);
        parent->appendChild(ere);
    }
    ers->setDecl(decl);
    ert()->addEntityRef(ers);
    return ers;
}

EntityReferenceStart*
GroveSectionRoot::insertEntityRef(EntityReferenceStart* master,
                                  Node* beforeNode,
                                  Node* parent)
{
    Node* firstNode = master->nextSibling();
    Node* lastNode  = master->getSectEnd()->prevSibling();
    DocumentFragment* df;
    if (lastNode == master) {
        df = new DocumentFragment;
        df->setGrove(grove());
    } else
        df = firstNode->copyAsFragment(lastNode);
    return insertEntityRef(master->entityDecl(), df, beforeNode, parent);
}

void GroveSectionRoot::updateEntityRefs(EntityReferenceStart* master)
{
    const EntityReferenceTable::ErtEntry* e =
        ert()->lookup(master->entityDecl());
    if (0 == e || e->numOfRefs() < 2)
        return;
    EntityReferenceStart* ers;
    for (uint i = 0; i < e->numOfRefs(); ++i) {
        ers = e->node(i);
        if (ers == master)
            continue;
        ers->setDecl(master->entityDecl());
        DocumentFragment* df;
        Node* lastSrcNode = master->getSectEnd()->prevSibling();
        if (ers->nextSibling() != ers->getSectEnd())
            ers->nextSibling()->removeGroup(ers->getSectEnd()->prevSibling());
        if (lastSrcNode != master) {
            df = master->nextSibling()->copyAsFragment(lastSrcNode);
            ers->insertAfter(df);
        }
    }
}

///////////////////////////////////////////////////////////////////////

//
// check whether grove sections are balanced
//
///////////////////////////////////////////////////////////////////////

void GroveSectionRoot::registerVisitor(GroveVisitorBase* v)
{
    if (vtab_.isNull())
        vtab_ = new GvTable;
    ulong nv = vtab_->size();
    for (ulong i = 0; i < nv; ++i)
        if ((*vtab_)[i] == v)
            return;
    vtab_->push_back(v);
}

void GroveSectionRoot::deregisterVisitor(GroveVisitorBase* v)
{
    if (v->asGroveVisitor())
        v->asGroveVisitor()->gsr_ = 0;
    if (vtab_.isNull())
        return;
    ulong nv = vtab_->size();
    for (ulong i = 0; i < nv; ++i) {
        if ((*vtab_)[i] == v) {
            vtab_->erase(i);
            return;
        }
    }
}

#define NOTIFY_X(what) \
    if (vtab_.isNull()) return; \
    ulong nv = vtab_->size(); \
    for (ulong i = 0; i < nv; ++i) \
        (*vtab_)[i]-> what ;

void GroveSectionRoot::gsr_nodeDestroyed(const Node* node) const
{
    NOTIFY_X(nodeDestroyed(node))
}

void GroveSectionRoot::gsr_childInserted(const Node* node) const
{
    NOTIFY_X(childInserted(node))
}

void GroveSectionRoot::gsr_childRemoved(const Node* node,
                                        const Node* child) const
{
    NOTIFY_X(childRemoved(node, child))
}

void GroveSectionRoot::gsr_genericNotify(const Node* node, void* p) const
{
    NOTIFY_X(genericNotify(node, p))
}

void GroveSectionRoot::gsr_forceModified(const Node* node) const
{
    NOTIFY_X(forceModified(node))
}

void GroveSectionRoot::gsr_attributeChanged(const Attr* node) const
{
    NOTIFY_X(attributeChanged(node))
}

void GroveSectionRoot::gsr_attributeRemoved(const Element* elem,
                                            const Attr* node) const
{
    NOTIFY_X(attributeRemoved(elem, node))
}

void GroveSectionRoot::gsr_attributeAdded(const Attr* node) const
{
    NOTIFY_X(attributeAdded(node))
}

void GroveSectionRoot::gsr_textChanged(const Text* node) const
{
    NOTIFY_X(textChanged(node))
}

void GroveSectionRoot::gsr_nsMappingChanged(const NodeWithNamespace* elem,
                                            const Common::String& prefix) const
{
    NOTIFY_X(nsMappingChanged(elem, prefix))
}

#undef NOTIFY_X

///////////////////////////////////////////////////////////////////////

GroveSectionStart* GroveSectionRoot::makeSectionStart(GroveSectionType t)
{
    typedef GroveSectionStart* (gssMakerFunc)();
    static gssMakerFunc* gss_factory[] = 
    {
        &MarkedSectionStart::make, &EntityReferenceStart::make,
        &RedlineSectionStart::make
    };
    return (*gss_factory[t])(); 
}

GroveSectionEnd* GroveSectionRoot::makeSectionEnd(GroveSectionType t)
{
    typedef GroveSectionEnd* (gseMakerFunc)();
    static gseMakerFunc* gse_factory[] = 
    {
        &MarkedSectionEnd::make, &EntityReferenceEnd::make,
        &RedlineSectionEnd::make
    };
    return (*gse_factory[t])(); 
}

GroveSectionStart* GroveSectionRoot::ensureSectionRoot(int t)
{
    if (t >= MAX_SECTION_ID)
        return 0;
    // note: manual increfing policy is used because we cannot use
    // refcntptr, because these fields are used as temporary for recursive
    // copying process
    if (0 == sections_[t]) {
        sections_[t] = makeSectionStart((GroveSectionType)t);
        sections_[t]->incRefCnt();
        sections_[t]->initTables();
        sections_[t]->setGSR(this);
    }
    return sections_[t];
}

GroveSectionRoot::~GroveSectionRoot()
{
    releaseSectionData();
    grove_ = 0;
    setGSR(0);
}

void GroveSectionRoot::dumpInherited() const
{
#ifdef GROVE_DEBUG
//    DINDENT(4*indent);
    DDBG << "GroveSectionRoot(" << this << "):" << std::endl;
    DDINDENT;
    for (uint i = 0; i < MAX_SECTION_ID; ++i)
        if (sections_[i])
            sections_[i]->dump();
    DDBG << "**** GSR end ******" << std::endl;
#endif // GROVE_DEBUG
}

PRTTI_IMPL(GroveSectionRoot)

GROVE_NAMESPACE_END
