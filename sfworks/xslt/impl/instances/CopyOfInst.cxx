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
#include "xslt/Result.h"
#include "xslt/impl/instances/CopyOfInst.h"
#include "xslt/impl/instructions/CopyOf.h"
#include "xslt/impl/debug.h"
#include "xpath/NodeSet.h"
#include "grove/Nodes.h"
#include "grove/NodeVisitor.h"
#include "common/CDList.h"

namespace Xslt {

//
// A base class for all copiers; process notifications and places them to queue
//
class NodeCopier : public GroveLib::NodeVisitor,
                   public COMMON_NS::CDListItem<NodeCopier> {
public:
    NodeCopier(CopyOfInstance* ci, const GroveLib::Node* on)
        : ci_(ci), copyOrigin_(on) {}
    virtual ~NodeCopier() {}

    /////////////////////////////////////////////////////////////
    // Notifications reimplemented from NodeVisitor
    virtual void    nodeDestroyed(const GroveLib::Node*) {}
    virtual void    childInserted(const GroveLib::Node* n)
    {
        ci_->set_modified(CopyOfInstance::CHILD_INSERTED, this, n);
    }
    virtual void    childRemoved(const GroveLib::Node*,
                                 const GroveLib::Node* n)
    {
        ci_->set_modified(CopyOfInstance::CHILD_REMOVED, this, n);
    }
    virtual void    attributeChanged(const GroveLib::Attr* a)
    {
        ci_->set_modified(CopyOfInstance::ATTRIBUTE_CHANGED, this, a);
    }
    virtual void    textChanged(const GroveLib::Text* t)
    {
        ci_->set_modified(CopyOfInstance::TEXT_CHANGED, this, t);
    }
    // Reimplemented from NodeVisitor
    virtual void    attributeAdded(const GroveLib::Attr* a)
    {
        ci_->set_modified(CopyOfInstance::ATTRIBUTE_ADDED, this, a);
    }
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr* a)
    {
        ci_->set_modified(CopyOfInstance::ATTRIBUTE_REMOVED, this, a);
    }
    /////////////////////////////////////////////////////////////

    static NodeCopier* make(CopyOfInstance* inst,
                            const GroveLib::Node* node,
                            const ResultContext& rctx);

    virtual GroveLib::Node* asNode() = 0;
    virtual void            removeCopy() = 0;
    GroveLib::Node*         find_ncn(const GroveLib::Node* selfResult,
                                     const GroveLib::Node* node);

protected:
    friend class CopyOfInstance;

    virtual void    process_change(CopyOfInstance::NotificationType,
                                   const GroveLib::Node*) = 0;
    void            do_change(CopyOfInstance::NotificationType,
                              const GroveLib::Node*,
                              CopyOfInstance* inst);
    void register_nv(const GroveLib::Node* n, short mask)
    {
        const_cast<GroveLib::Node*>(n)->registerNodeVisitor(this, mask);
    }
    CopyOfInstance*         ci_;
    const GroveLib::Node*   copyOrigin_;
};

class TextCopier : public NodeCopier, public TextResult {
public:
    XSLT_OALLOC(TextCopier);

    TextCopier(CopyOfInstance* inst,
               const GroveLib::Text* t,
               const ResultContext& rctx);
    ~TextCopier() { DBG(XSLT.COPY) << "~TextCopier: " << this << std::endl; }
    
    virtual void        removeCopy() 
    { 
        DBG(XSLT.COPY) << "TextCopier::RemoveCopy, RC=" << getRefCnt() << "\n";
        deregisterFromAllNodes(); 
        Node::remove();
    }
    virtual GroveLib::Node* asNode() { return this; }
    virtual const GroveLib::Node* copyOrigin() const { return copyOrigin_; }

private:
    virtual void    process_change(CopyOfInstance::NotificationType,
                                   const GroveLib::Node*);
};

class ElementCopier : public NodeCopier, public ElementResult {
public:
    XSLT_OALLOC(ElementCopier);

    ElementCopier(CopyOfInstance* inst,
                  const GroveLib::Element* elem,
                  const ResultContext& rctx);
    ~ElementCopier() { DBG(XSLT.COPY) << "~ElemCopier: " << this << std::endl; }
    
    virtual void        removeCopy() 
    {
        DBG(XSLT.COPY) << "ElemCopier::RemoveCopy, RC=" << getRefCnt() << "\n";
        deregisterFromAllNodes();
        Node::remove();
    }
    virtual GroveLib::Node* asNode() { return this; }
    virtual const GroveLib::Node* copyOrigin() const { return copyOrigin_; }

private:
    virtual void    process_change(CopyOfInstance::NotificationType,
                                   const GroveLib::Node*);
};

class AttributeCopier : public NodeCopier, public AttributeResult {
public:
    XSLT_OALLOC(AttributeCopier);

    AttributeCopier(CopyOfInstance* inst,
                    const GroveLib::Attr* attr,
                    const ResultContext& rctx);
    ~AttributeCopier()
        { DBG(XSLT.COPY) << "~AttrCopier: " << this << std::endl; }

    virtual void        removeCopy() { deregisterFromAllNodes(); } // can't be
    virtual GroveLib::Node* asNode() { return this; }
    virtual const GroveLib::Node* copyOrigin() const { return copyOrigin_; }

public:
    virtual void    process_change(CopyOfInstance::NotificationType,
                                   const GroveLib::Node*);
    COMMON_NS::String   selfName_;
};

/////////////////////////////////////////////////////////////////////

CopyOfInstance::CopyOfInstance(const CopyOf* copyOf,
                               const InstanceInit& init, Instance* p)
    : Instance(copyOf, init, p),
      selectInst_(this,
        copyOf->selectExpr()->makeInst(init.context_, exprContext())),
      selectInstChanged_(false)
{
    DBG(XSLT.COPY) << "CopyOfInst::constructor\n";
    makeCopiers(init.resultContext_);
}

void CopyOfInstance::makeCopiers(const ResultContext& rctx)
{
    const Xpath::NodeSet& node_set = selectInst_->value()->getNodeSet();
    const Xpath::NodeSetItem* n = node_set.first();
    NodeCopier* nc;
    for (; n; n = n->next()) {
        GroveLib::Node* nn = n->node();
        if (nn->nodeType() == GroveLib::Node::DOCUMENT_FRAGMENT_NODE
          || nn->nodeType() == GroveLib::Node::DOCUMENT_NODE) {
                nn->registerNodeVisitor(this,
                    GroveLib::NodeVisitor::NOTIFY_CHILD_INSERTED|
                    GroveLib::NodeVisitor::NOTIFY_CHILD_REMOVED);
            const GroveLib::Node* cn = n->node()->firstChild();
            for (; cn; cn = cn->nextSibling()) {
                nc = makeCopier(cn, rctx);
                if (nc)
                    topInstances_.push_back(nc);
            }
        } else {
            nc = makeCopier(nn, rctx);
            if (nc)
                topInstances_.push_back(nc);
        }
    }
    setInstanceResults();
    modQueue_.destroyAll();
}

NodeCopier* CopyOfInstance::makeCopier(const GroveLib::Node* n,
                                const ResultContext& rctx)
{
    NodeCopier* self = NodeCopier::make(this, n, rctx);
    if (0 == self)
        return self;
    GroveLib::Node* rself = self->asNode();
    if (n->nodeType() != GroveLib::Node::ELEMENT_NODE)
        return self;
    const GroveLib::Node* cn = n->firstChild();
    for (; cn; cn = cn->nextSibling())
        makeCopier(cn, ResultContext(rself, 0));
    return self;
}

const InstanceResult* CopyOfInstance::firstResult() const
{
    if (firstResult_.node())
        return &firstResult_;
    return 0;
}

const InstanceResult* CopyOfInstance::lastResult() const
{
    if (lastResult_.node())
        return &lastResult_;
    return 0;
}

void CopyOfInstance::setInstanceResults()
{
    NodeCopier* nc = topInstances_.first();
    GroveLib::Node* rn;
    if (nc) {
        rn = nc->asNode();
        firstResult_.parent_ = parentNode(rn);
        firstResult_.resultNode_ = rn;
    } else
        firstResult_.parent_ = firstResult_.resultNode_ = 0;
    nc = topInstances_.last();
    if (nc) {
        rn = nc->asNode();
        lastResult_.parent_ = parentNode(rn);
        lastResult_.resultNode_ = rn;
    } else
        lastResult_.parent_ = lastResult_.resultNode_ = 0;
}

void CopyOfInstance::disposeResult()
{
    DBG(XSLT.COPY) << "CopyOfInstance::disposeResult called\n";
    NodeCopier* nc;
    while ((nc = topInstances_.pop_front()))
        nc->removeCopy();
    setInstanceResults();
}

//////////////// dynamics

void CopyOfInstance::set_modified(NotificationType nt,
                                  NodeCopier* nc,
                                  const GroveLib::Node* aux)
{
    ModQueueItem* mqi = new ModQueueItem;
    mqi->nt  = nt;
    mqi->nc  = nc;
    mqi->aux = aux;
    if (nt == CHILD_REMOVED)
        mqi->aux = nc->find_ncn(nc->asNode(), aux);
    modQueue_.push_back(mqi);
    DBG(XSLT.COPY) << "CopyOf:: add to modqueue: "
        << aux->nodeName() << " nt " << nt << std::endl;
    setModified();
}

void CopyOfInstance::update()
{
    if (selectInstChanged_) {
        DBG(XSLT.COPY) << "CopyOf:update: selectInst changed\n";
        modQueue_.destroyAll();
        disposeResult();
        removeSubInstances();
        makeCopiers(Instance::resultContext());
        selectInstChanged_ = false;
        return;
    }
    DBG(XSLT.COPY) << "CopyOf:update: processing queue\n";
    ModQueueItem* mqi;
    while ((mqi = modQueue_.pop_front())) {
        mqi->nc->do_change(mqi->nt, mqi->aux.pointer(), this);
        delete mqi;
    }
    setInstanceResults();
}

void CopyOfInstance::updateContext(int)
{
    updateExpr(selectInst_);
}

// change of selectInst
void CopyOfInstance::notifyChanged(const COMMON_NS::SubscriberPtrBase*)
{
    DBG(XSLT.COPY) << "CopyOfInstance:: NotifyChanged from SelectInst\n";
    selectInstChanged_ = true;
    setModified();
}

void CopyOfInstance::childInserted(const GroveLib::Node*)
{
    selectInstChanged_ = true;
    setModified();
}

void CopyOfInstance::childRemoved(const GroveLib::Node*,
                                  const GroveLib::Node*)
{
    selectInstChanged_ = true;
    setModified();
}

CopyOfInstance::~CopyOfInstance()
{
    DBG(XSLT.COPY) << "CopyOfInstance::Destroyed\n";
}

/////////////////////////////////////////////////////////////////////

static inline const GroveLib::Node* copy_origin(const GroveLib::Node* rn)
{
    return rn->asConstXsltResult()->copyOrigin();
}

GroveLib::Node* NodeCopier::find_ncn(const GroveLib::Node* selfResultNode,
                                     const GroveLib::Node* node)
{
    GroveLib::Node* rn = selfResultNode->firstChild();
    for (; rn; rn = rn->nextSibling()) {
        if (copy_origin(rn) == node)
            return rn;
    }
    return 0;
}

void NodeCopier::do_change(CopyOfInstance::NotificationType nt,
                           const GroveLib::Node* node,
                           CopyOfInstance* inst)
{
    DBG(XSLT.COPY) << "NodeCopier: do_change type = " << (int)nt << " node:\n";
    DBG_IF(XSLT.COPY) { if (node) GroveLib::Node::dumpSubtree(node); }
    DBG(XSLT.COPY) << "----------------- end of node dump\n";

    GroveLib::Node* selfResultNode;
    GroveLib::Node* rn = 0;
    if (nt == CopyOfInstance::CHILD_REMOVED) {
        if (node)
            const_cast<GroveLib::Node*>(node)->remove();
        return;
    }
    if (nt != CopyOfInstance::CHILD_INSERTED)
        return process_change(nt, node);

    selfResultNode = asNode();
    GroveLib::Node* ns = node->nextSibling();
    for (; ns; ns = ns->nextSibling()) {
        rn = find_ncn(selfResultNode, ns);
        if (rn)
            break;
    }
    if (rn)
        inst->makeCopier(node, ResultContext(selfResultNode, rn));
    else
        inst->makeCopier(node, ResultContext(selfResultNode, 0));
}

NodeCopier* NodeCopier::make(CopyOfInstance* inst,
                             const GroveLib::Node* n,
                             const ResultContext& rctx)
{
    switch (n->nodeType()) {
        case GroveLib::Node::TEXT_NODE:
            return new TextCopier(inst, CONST_TEXT_CAST(n), rctx);

        case GroveLib::Node::ELEMENT_NODE:
            return new ElementCopier(inst, CONST_ELEMENT_CAST(n), rctx);

        case GroveLib::Node::ATTRIBUTE_NODE:
            return new AttributeCopier(inst, CONST_ATTR_CAST(n), rctx);

        default:
            return 0;
    }
}

/////////////////////////////////////////////////////////////////////

static const GroveLib::Node* node_origin(const GroveLib::Node* node)
{
    const XsltResult* xsres = node->asConstXsltResult();
    return xsres ? xsres->origin() : node;
}

TextCopier::TextCopier(CopyOfInstance* inst,
                       const GroveLib::Text* text,
                       const ResultContext& rctx)
    : NodeCopier(inst, text),
      TextResult(text->data(), node_origin(text), inst)
{
    rctx.insert(this);
    register_nv(text, GroveLib::NodeVisitor::NOTIFY_TEXT_CHANGED);
    DBG(XSLT.COPY) << "CopyOf:new text copier:<" << text->data() << ">\n";
}

void TextCopier::process_change(CopyOfInstance::NotificationType nt,
                               const GroveLib::Node* aux)
{
    if (nt != CopyOfInstance::TEXT_CHANGED || !aux)
        return;
    DBG(XSLT.COPY) << "TextCopier: process_change\n";
    const GroveLib::Text* text = static_cast<const GroveLib::Text*>(aux);
    Text::setData(text->data());
}

/////////////////////////////////////////////////////////////////////

ElementCopier::ElementCopier(CopyOfInstance* inst,
                             const GroveLib::Element* elem,
                             const ResultContext& rctx)
    : NodeCopier(inst, elem),
      ElementResult(elem->nodeName(), node_origin(elem), inst)
{
    const GroveLib::Attr* attr = elem->attrs().firstChild();
    for (; attr; attr = attr->nextSibling())
        attrs().appendChild(new AttributeResult(attr->nodeName(),
            attr->value(), node_origin(attr), inst));
    rctx.insert(this);
    register_nv(elem, GroveLib::NodeVisitor::NOTIFY_CHILD_INSERTED|
        GroveLib::NodeVisitor::NOTIFY_CHILD_REMOVED|
        GroveLib::NodeVisitor::NOTIFY_ATTRIBUTE_ADDED|
        GroveLib::NodeVisitor::NOTIFY_ATTRIBUTE_REMOVED|
        GroveLib::NodeVisitor::NOTIFY_ATTRIBUTE_CHANGED);
    DBG(XSLT.COPY) << "ElementCopier created: " << elem->nodeName() << "\n";
}

static void process_attr_change(CopyOfInstance* inst,
                                GroveLib::Element* elem,
                                CopyOfInstance::NotificationType nt,
                                const GroveLib::Attr* a)
{
    switch (nt) {
        case CopyOfInstance::ATTRIBUTE_ADDED: {
            DBG(XSLT.COPY) << "Copier: attribute_added\n";
            elem->attrs().appendChild(new AttributeResult(a->name(),
                a->value(), node_origin(a), inst));
            break;
        }
        case CopyOfInstance::ATTRIBUTE_CHANGED: {
            DBG(XSLT.COPY) << "Copier: attribute_changed\n";
            GroveLib::Attr* oa = elem->attrs().getAttribute(a->name());
            if (0 == oa)
                elem->attrs().appendChild(new AttributeResult(a->name(),
                    a->value(), node_origin(a), inst));
            else
                oa->setValue(a->value());
            break;
        }
        case CopyOfInstance::ATTRIBUTE_REMOVED: {
            DBG(XSLT.COPY) << "Copier: attribute_removed\n";
            elem->attrs().removeAttribute(a->name());
            break;
        }
        default:
            ;
    }
}

void ElementCopier::process_change(CopyOfInstance::NotificationType nt,
                                   const GroveLib::Node* aux)
{
    process_attr_change(ci_, this, nt,
        static_cast<const GroveLib::Attr*>(aux));
}

/////////////////////////////////////////////////////////////////////

AttributeCopier::AttributeCopier(CopyOfInstance* inst,
                                 const GroveLib::Attr* attr,
                                 const ResultContext& rctx)
    : NodeCopier(inst, attr),
      AttributeResult(attr->name(), attr->value(), node_origin(attr), inst),
      selfName_(attr->name())
{
    static_cast<GroveLib::Element*>(rctx.parent_)->attrs().setAttribute(this);
    register_nv(rctx.parent_, GroveLib::NodeVisitor::NOTIFY_ATTRIBUTE_CHANGED);
    DBG(XSLT.COPY) << "CopyOf:new Attr copier " << attr->name()
        << ":<" << attr->value() << ">\n";
}

void AttributeCopier::process_change(CopyOfInstance::NotificationType nt,
                                     const GroveLib::Node* aux)
{
    DBG(XSLT.COPY) << "AttrCopier: process_change\n";
    const GroveLib::Attr* a =
                static_cast<const GroveLib::Attr*>(aux);
    if (a->name() != selfName_)
        return;
    process_attr_change(ci_, element(), nt,
        static_cast<const GroveLib::Attr*>(aux));
}

///////////////////////////////////////////////////////////////

Instance* CopyOf::makeInst(const InstanceInit& init, Instance* p) const
{
    return new CopyOfInstance(this, init, p);
}

} // namespace Xslt
