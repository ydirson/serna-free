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
#include "csl/Template.h"
#include "csl/RootInstance.h"
#include "csl/Stylesheet.h"
#include "csl/csl_debug.h"

#include "grove/Nodes.h"
#include "grove/NodeVisitor.h"
#include "xs/XsNodeExt.h"
#include "xs/XsElement.h"

#include "common/SubscriberPtr.h"
#include "common/SList.h"
#include "common/asserts.h"

#include "xslt/TemplateSelectorBase.h"
#include <map>

namespace Csl {

static Template BUILTIN_SECTION_TEMPLATE;
static const uint MAX_DEPTH = ~0;

class TemplateSelector : public Xslt::TemplateSelectorBase {
public:
    CSL_OALLOC(TemplateSelector);
    virtual void notifyChanged(const Common::SubscriberPtrBase*) 
    {
        DBG(CSL.DYN) << "CSL: NodeChanged\n";
        if (!isModified_) {
            instance_->addModification(new Instance::ModItem
                (Instance::ModItem::TEMPLATE_CHANGED, instance_));
            isModified_ = true;
        }
    }
    void            update();

    const Template* selectedTemplate() const { return selectedTemplate_; }
    void            selectTemplate(RootInstance*, 
                                   const Xpath::NodeSetItem& nsi);
    void            setInstance(Instance* i) 
    { 
        i->selectedTemplate_ = selectedTemplate_;
        instance_ = i;
    }
    TemplateSelector(const Xpath::NodeSetItem&, RootInstance* ri);

private:
    Instance*                   instance_;
    const Template*             selectedTemplate_;
    bool                        isModified_;
};

/////////////////////////////////////////////////////////////////////

TemplateSelector::TemplateSelector(const Xpath::NodeSetItem& nsi,
                                   RootInstance* ri)
    : instance_(0),
      isModified_(false)
{
    selectTemplate(ri, nsi);
}

static inline void remove_children2(Instance* inst)
{
    while (inst->lastChild())
        remove_children2(inst->lastChild());
    inst->remove();
}

static void remove_children(Instance* inst)
{
    while (inst->lastChild())
        remove_children2(inst->lastChild());
}

static inline bool is_valid_ers(const GroveLib::EntityReferenceStart* ers)
{
    const GroveLib::EntityDecl* const ed = ers->entityDecl();
    return ed && !ed->isSingleCharEntity();
}

static inline bool is_valid_ere(const GroveLib::EntityReferenceEnd* ere)
{
    return ere->getSectStart() && 
           is_valid_ers(CONST_ERS_CAST(ere->getSectStart()));
}

void TemplateSelector::selectTemplate(RootInstance* ri,
                                      const Xpath::NodeSetItem& nsi)
{
    selectedTemplate_ = 0;
    if (nsi.node()->nodeType() == GroveLib::Node::ENTITY_REF_START_NODE) {
        if (is_valid_ers(CONST_ERS_CAST(nsi.node())))
            selectedTemplate_ = &BUILTIN_SECTION_TEMPLATE;
        return;
    }
    const Profile::TemplatesVector& tv = ri->profile()->templates();
    for (uint i = 0; i < tv.size(); ++i) {
        const Template* const t = tv[i];
        if (t->mixedOnly()) {
            if (nsi.node()->nodeType() != GroveLib::Node::ELEMENT_NODE)
                continue;
            const Xs::XsElement* xselem = XsNodeExt::xsElement(nsi.node());
            if (0 == xselem || !xselem->cdataAllowed())
                continue;
        }
        if (!t->matchPattern().isApplicable(nsi.node()))
            continue;
        Xpath::ValueHolderPtr pinst(0, t->matchPattern().makeInst(nsi, *this));
        if (pinst.isNull())
            continue;
        if (!selectedTemplate_ && pinst->value()->getBool()) {
            selectedTemplate_ = t;
            DBG(CSL.TSEL) << "TemplateSelector: selected template: ";
            DBG_IF(CSL.TSEL) t->dump();
        }
    }
}

void TemplateSelector::update()
{   
    DBG(CSL.DYN) << "CSL: TemplateSelector::update\n";
    deregisterWatchers();
    selectTemplate(instance_->rootInstance(), instance_->nsi());
    isModified_ = false;
}

/////////////////////////////////////////////////////////////////////

static InstanceWatcher dummy_watcher;
typedef Common::OwnedXList<Instance::ModItem> ModItemList;

class RootInstance::ModificationSet : public std::map<uint, ModItemList> {};

RootInstance::RootInstance(GroveLib::Node* node, 
                           const InstanceMaker instMaker,
                           const Stylesheet* stylesheet,
                           const Profile* profile,
                           Xslt::NumberCache* cache,
                           PassType passType)
    : Instance(node, this),
      instanceMaker_(instMaker),
      stylesheet_(stylesheet),
      profile_(profile),
      modSet_(new ModificationSet),
      instWatcher_(&dummy_watcher),
      numberCache_(cache)
{
    templateSelector_ = new TemplateSelector(nsi_, this);
    selectedTemplate_ = templateSelector_->selectedTemplate();
    if (selectedTemplate_)
        evaluate_subinsts();
    do_apply(passType, true);
}

Instance* Instance::newInstance(GroveLib::Node* n) 
{
    if (is_stopped()) {
        Instance* inst = new Instance(InstanceInit(rootInstance(), n, this));
        inst->isStub_ = 1;
        return inst;
    }
    return (*rootInstance()->instanceMaker())
        (InstanceInit(rootInstance(), n, this));
}

void RootInstance::addModification(ModItem* item, uint depth)
{
// START_IGNORE_LITERALS
    DBG_IF(CSL.DYN) {
        static const char* evt[] = 
            { "CINS", "CREM", "SUBCHG", "TCHG", "RESCHG" };
        DBG(CSL.DYN) << "CSL: added modification: type=" << evt[item->type_]
            << " depth=" << depth << " inst=" << item->instance_
            << " node=" << item->node_.pointer() << std::endl;
    }
    if (modSet_)
        (*modSet_)[depth].appendChild(item);
// STOP_IGNORE_LITERALS
}

void RootInstance::removeModification(const Instance* inst, uint depth)
{   
    if (!modSet_)
        return;
    ModificationSet::iterator it = modSet_->find(depth);
    if (it == modSet_->end())
        return;
    ModItemList& mlist = it->second;
    for (ModItem* mi = mlist.firstChild(); mi;) {
        if (mi->instance_ == inst) {
            DBG(CSL.DYN) << "CSL: removing modification, inst=" << inst
                << " depth=" << depth << std::endl;
            ModItem* next_mi = mi->nextSibling();
            delete mi;
            mi = next_mi;
        } else
            mi = mi->nextSibling();
    }
    if (!mlist.firstChild())
        modSet_->erase(it);
}

void RootInstance::update()
{
    ModificationSet::iterator it;
    while ((it = modSet_->begin()) != modSet_->end()) {
        ModItemList& mlist = it->second;
        for (ModItem* mi = mlist.firstChild(); mi; mi = mi->nextSibling()) {
            if (1 == mi->instance_->getRefCnt())
                continue;   // nobody needs it anymore
            DBG(CSL.DYN) << "CSL Root Update: processing mod, inst="
                << mi->instance_ << std::endl;
            mi->instance_->update(*mi);
        }
        modSet_->erase(it);
    }
}

void RootInstance::dump() const
{
    DDBG << "RootInstance: context node=" << origin() << std::endl;
    Instance::dump();
}

RootInstance::~RootInstance()
{
}

/////////////////////////////////////////////////////////////////////
 
Instance::Instance(GroveLib::Node* node, RootInstance* ri)
    : rootInstance_(ri),
      nsi_(node),
      selectedTemplate_(0),
      depth_(0),
      subinstChanged_(false),
      isStub_(false)
{
}

Instance::Instance(const InstanceInit& init)
    : rootInstance_(init.rootInst_),
      nsi_(init.node_),
      selectedTemplate_(0),
      subinstChanged_(false),
      isStub_(false)
{
    depth_ = init.parent_->depth() + 1;
    init.node_->registerNodeVisitor(this, 
        GroveLib::NodeVisitor::NOTIFY_CHILD_INSERTED|
        GroveLib::NodeVisitor::NOTIFY_CHILD_REMOVED|
        GroveLib::NodeVisitor::NOTIFY_FORCE_MODIFIED|
        GroveLib::NodeVisitor::NOTIFY_CSL_BIT); // type tag
}

void Instance::setTemplateSelector(TemplateSelector* ts)
{
    templateSelector_ = ts;
}

inline void Instance::set_stopped(bool v)
{
    GroveLib::Node& n = *nsi_.node();
    n.setUdata(v ? (n.udata() | GroveLib::CM_FOLD_BIT) :
        (n.udata() & ~GroveLib::CM_FOLD_BIT));
}

void Instance::setOpen(bool v)
{
    DBG(CSL.DYN) << "CSL::Instance: SetOpen: " << v << std::endl;
    if (v) { // open item
        if (!is_stopped() || !selectedTemplate_)
            return;
        set_stopped(false);
        do_apply(RootInstance::OPEN_PASS, true); 
    } else { // close item
        if (is_stopped())
            return;
        for (Instance* i = firstChild(); i; i = i->nextSibling())
            remove_children(i);
        set_stopped(true);
    }
    evaluate_subinsts();
}

void Instance::evaluate_subinsts()
{
    text_.clear();
    subInstances_.destroyAll();
    const Instruction* instruction = 
        selectedTemplate_->instructions(is_stopped()).firstChild();
    for (; instruction; instruction = instruction->nextSibling()) {
        SubInstance* subinst = instruction->makeInst(this);
        subInstances_.appendChild(subinst);
        text_ += subinst->result();
    }
    if (text_.length() >= selectedTemplate_->maxLength()) {
        text_.truncate(selectedTemplate_->maxLength());
        text_ += "...";
    }
}

void Instance::do_apply(int pass_type, bool is_top)
{
    remove_children(this);
    const Template* t = 0;
    if (templateSelector_) {
        t = templateSelector_->selectedTemplate();
        if (!templateSelector_->hasWatchers())
            templateSelector_.clear();
    }
    GroveLib::Node* from_node, *to_node;
    const bool is_ers = 
        nsi_.node()->nodeType() == GroveLib::Node::ENTITY_REF_START_NODE &&
        is_valid_ers(CONST_ERS_CAST(nsi_.node()));
    switch (pass_type) {
        case RootInstance::FIRST_PASS:
            set_stopped((t && t->doFold()) || 
                depth() >= rootInstance()->profile()->cutoffLevel());
            break;
        case RootInstance::UPDATE_PASS:
            set_stopped(is_stopped() || (t && t->doFold()));
            break;
        default:
            break;
    }
    if (is_ers) {
        if (!is_top)
            return;
        from_node = nsi_.node()->nextSibling();
        to_node   = CONST_ERS_CAST(nsi_.node())->getSectEnd()->prevSibling();
        if (to_node == nsi_.node())
            return;
    } else {
        from_node = nsi_.node()->firstChild();
        to_node   = nsi_.node()->lastChild();
    }
    if (is_top && nsi_.node()->nodeType() == GroveLib::Node::DOCUMENT_NODE) {
        while (from_node && 
               from_node->nodeType() != GroveLib::Node::ELEMENT_NODE)
            from_node = from_node->nextSibling();
        to_node = from_node;        
    }
    if (0 == from_node)
        return;
    Instance* parent_inst = this;
    for (;;) {
        Xpath::NodeSetItem tnsi(from_node);
        Common::OwnerPtr<TemplateSelector> 
            ts(new TemplateSelector(tnsi, rootInstance()));
        if (ts->selectedTemplate()) {
            Common::RefCntPtr<Instance> inst = newInstance(from_node);
            ts->setInstance(inst.pointer());
            inst->setTemplateSelector(ts.release());
            inst->evaluate_subinsts();
            if (!is_stopped()) 
                inst->do_apply(pass_type, false);
            else
                evaluate_subinsts();
            parent_inst->appendChild(inst.pointer());
        }
        switch (from_node->nodeType()) {
            case GroveLib::Node::ENTITY_REF_START_NODE:
                if (is_valid_ers(CONST_ERS_CAST(from_node)))
                    parent_inst = parent_inst->lastChild();
                break;
            case GroveLib::Node::ENTITY_REF_END_NODE:
                if (is_valid_ere(CONST_ERE_CAST(from_node)))
                    parent_inst = parent_inst->parent();
                break;
            default:
                break;
        }
        if (from_node == to_node)
            return;
        from_node = from_node->nextSibling();
    }
}

void Instance::addModification(ModItem* item)
{
    rootInstance()->addModification(item, depth_);
}
    
void Instance::notifyChildRemoved(XLPT* xlpt, Instance* inst)
{
    RootInstance* const ri = static_cast<Instance*>(xlpt)->rootInstance();
    if (0 == ri)
        return;
    DBG(CSL.DYN) << "CSL: notifyInstRemove, inst=" << inst << std::endl;
    ri->removeModification(inst, inst->depth_);
}

void Instance::subinstChanged()
{
    if (!subinstChanged_) {
        addModification(new ModItem(ModItem::SUBINST_CHANGED, this));
        subinstChanged_ = true;
    }
}

void Instance::update(const ModItem& item)
{
    using namespace GroveLib;
    
    switch (item.type_) {
        case ModItem::CHILD_INSERTED: {
            DBG(CSL.DYN) << "CSL: update CHILD_INSERTED: node="
                << item.node_.pointer() 
                << "(" << item.node_->nodeName() << ")\n";
            Node* node = item.node_.pointer();
            // check if node was removed later
            if (node->getGSR() != rootInstance()->origin()->getGSR()) 
                return;
            Xpath::NodeSetItem tnsi(node);
            switch (node->nodeType()) {
                case Node::ENTITY_REF_START_NODE:
                    if (!is_valid_ers(CONST_ERS_CAST(node)))
                        return;
                    node = CONST_ERS_CAST(node)->getSectEnd()->nextSibling();
                    break;
                case Node::ENTITY_REF_END_NODE: {
                    if (!is_valid_ere(CONST_ERE_CAST(node)))
                        return;
                    GroveSectionStart* gss = 
                        CONST_ERE_CAST(node)->getSectStart();
                    if (node->prevSibling() == gss)
                        return; // empty entity ref
                    Instance* inst = 0;
                    Node* n = gss->nextSibling();
                    Instance* ers_inst = csl_instance_origin(gss);
                    if (0 == ers_inst)
                        return;
                    DBG(CSL.DYN) << "CSL: checking reconnect\n";
                    while (n != node) {
                        inst = csl_instance_origin(n);
                        if (inst && inst->parent() != ers_inst)
                            inst->reconnect(ers_inst);
                        if (n->nodeType() == Node::ENTITY_REF_START_NODE) 
                            n = CONST_ERS_CAST(n)->getSectEnd();
                        else
                            n = n->nextSibling();
                    }
                    DBG_IF(CSL.DYN) ers_inst->dump();
                    return;
                }
                default:
                    node = node->nextSibling();
                    break;
            }
            Common::OwnerPtr<TemplateSelector> 
                selector(new TemplateSelector(tnsi, rootInstance()));
            if (!selector->selectedTemplate())
                return;
            // need to find position to insert, then create instance
            Instance* before = 0;
            Instance* append = 0;
            while (node) {
                DBG(CSL.DYN) << "CSL: traverse " 
                    << node->nodeName() << std::endl;
                if (node->nodeType() == Node::ENTITY_REF_END_NODE) {
                    append = csl_instance_origin(
                        CONST_ERE_CAST(node)->getSectStart());
                    break;
                }
                before = csl_instance_origin(node);
                if (before)
                    break;
                if (node->nodeType() == Node::ENTITY_REF_START_NODE) 
                    node = CONST_ERS_CAST(node)->getSectEnd()->nextSibling();
                else
                    node = node->nextSibling();
            }
            if (!before && !append) {
                append = item.node_->parent() ?
                    csl_instance_origin(item.node_->parent()) : 0;
                DBG(CSL.DYN) << "CSL: determining appendTo, will be "
                    << (append ? item.node_->parent()->nodeName()
                        : Common::String("<None>")) << std::endl;
                if (0 == append) 
                    break;
            }
            if (!append)
                append = before->parent();
            if (!append) {
                append = csl_instance_origin(tnsi.node()->parent());
                before = 0;
            }
            if (!append || append->isStub_)
                return; // do not append to stubs
            Common::RefCntPtr<Instance> new_instance = 
                append->newInstance(tnsi.node());
            selector->setInstance(new_instance.pointer());
            new_instance->setTemplateSelector(selector.release());
            new_instance->evaluate_subinsts();
            if (!new_instance->isStub())
                new_instance->do_apply(RootInstance::UPDATE_PASS, false);
            if (before) {
                DBG(CSL.DYN) << "CSL: insBefore " 
                    << before->origin()->nodeName() << ": "
                    << new_instance->origin()->nodeName();
                before->insertBefore(new_instance.pointer());
            } else {
                DBG(CSL.DYN) << "Appending to: " 
                    << append->origin()->nodeName() << std::endl;
                DBG_IF(CSL.DYN) append->origin()->dump();
                append->appendChild(new_instance.pointer());
            }
            rootInstance()->instWatcher()->notifyInstanceInserted(
                new_instance.pointer());
            break;
        }
        case ModItem::CHILD_REMOVED: {
            Instance* inst = csl_instance_origin(item.node_.pointer());
            DBG(CSL.DYN) << "CSL: update CHILD_REMOVED: origin-inst=" 
                << inst << "(" << item.node_->nodeName() << ")" << std::endl;
            if (inst) {
                if (item.node_->nodeType() == Node::ENTITY_REF_START_NODE) {
                    DBG(CSL.DYN) << "Reconnecting ERS children on remove\n";
                    Instance* pi = inst->parent();
                    Instance* before = pi ? inst->nextSibling() : 0;    
                    while (pi && inst->firstChild())
                        inst->firstChild()->reconnect(pi, before);
                }
                DBG(CSL.DYN) << "CSL: Found instance, removing\n";
                rootInstance()->instWatcher()->notifyInstanceRemoved(
                    this, inst);
                inst->remove();
            }                
            break;
        }
        case ModItem::SUBINST_CHANGED: {
            DBG(CSL.DYN) << "CSL: SubInst Changed\n";
            subinstChanged_ = false;
            Common::String result_text;
            SubInstance* subinst = subInstances_.firstChild();
            for (; subinst; subinst = subinst->nextSibling()) {
                subinst->update();
                result_text += subinst->result();
            }
            if (text_ != result_text) {
                DBG(CSL.DYN) << "CSL: SubInsts reevaluated for context node=" 
                             << origin() << "(" << origin()->nodeName() 
                             << ")\n";
                text_ = result_text;
                resultChanged();
            }
            break;
        }
        case ModItem::RESULT_CHANGED: {
            DBG(CSL.DYN) << "CSL: RESULT_Changed\n";
            resultChanged();
            break;
        }      
        case ModItem::TEMPLATE_CHANGED: {
            RT_ASSERT(templateSelector_);
            const Template* old_tmpl = templateSelector_->selectedTemplate();
            templateSelector_->update();
            const Template* new_tmpl = templateSelector_->selectedTemplate();
            if (old_tmpl == new_tmpl) {
                DBG(CSL.DYN) << "CSL: update TEMPLATE_CHANGED: same tmpl\n";
                break;
            }
            selectedTemplate_ = new_tmpl;
            DBG(CSL.DYN) << "CSL: update TEMPLATE_CHANGED: new template\n";
            // note: instance children are not dependent on template
            evaluate_subinsts();
            break;
        }
        default:
            RT_MSG_ABORT(NOTR("Unknown modification type"));
            break;
    }
}

inline void Instance::setDepth(uint depth)
{
    depth_ = depth;
    Instance* inst = firstChild();
    for (; inst; inst = inst->nextSibling())
        inst->setDepth(depth + 1);
}

void Instance::reconnect(Instance* parent, Instance* before)
{
    Common::RefCntPtr<Instance> inst_holder(this);
    DBG(CSL.DYN) << "CSL: Reconnect: notifyInstRemoved " 
        << inst_holder->origin()->nodeName() 
        << "(" << inst_holder.pointer() << "): ";
    DBG_IF(CSL.DYN) inst_holder->dump();
    rootInstance()->instWatcher()->notifyInstanceRemoved(
        this, inst_holder.pointer());
    inst_holder->remove();
    DBG(CSL.DYN) << "CSL: Reconnect: Reinsert after remove: "
        << inst_holder->origin()->nodeName() 
        << "(" << inst_holder.pointer() << "), to "
        << parent->origin()->nodeName()
        << "(" << parent << "/" << before << "): ";
    DBG(CSL.DYN) << "----------------------------------\n";
    inst_holder->setDepth(parent->depth_ + 1);
    if (before)
        before->insertBefore(inst_holder.pointer());
    else
        parent->appendChild(inst_holder.pointer());
    rootInstance()->instWatcher()->notifyInstanceInserted(
        inst_holder.pointer());
}

void Instance::childInserted(const GroveLib::Node* node)
{
    addModification(new ModItem(ModItem::CHILD_INSERTED, this, 
        const_cast<GroveLib::Node*>(node)));
}

void Instance::childRemoved(const GroveLib::Node*,
                            const GroveLib::Node* node)
{
    addModification(new ModItem(ModItem::CHILD_REMOVED, this, 
        const_cast<GroveLib::Node*>(node)));
}

void Instance::forceModified(const GroveLib::Node* node)
{
    switch (node->nodeType()) {
        case GroveLib::Node::PI_NODE:
        case GroveLib::Node::COMMENT_NODE:
        case GroveLib::Node::ENTITY_REF_START_NODE:
            addModification(new ModItem(ModItem::RESULT_CHANGED, this));
            break;
        default:
            break;
    }
}

void Instance::dump() const
{
    DBG_IF(CSL.TEST) {
        DDBG << "Instance(" << this
             << "): CTX=" << origin() << "(" << origin()->nodeName()
             << ") P=" << parent() << " D=" << depth_  
             << " TS=" << &*templateSelector_ 
             << " RCNT=" << getRefCnt() << std::endl;
        DDINDENT;
        DDBG << "RESULT: STP=" << is_stopped() << " NC#" << countChildren()
             << " text=<" << text_ << "> isOpen:" << isOpen() 
             << " isStub:" << isStub_ <<"\n";
        const SubInstance* si = subInstances_.firstChild();
        for (; si; si = si->nextSibling())
            si->dump();
        const Instance* i = firstChild();
        if (i) {
            DDINDENT;
            for (; i; i = i->nextSibling())
                i->dump();
        }
    }
}
    
Instance::CursorPlacement Instance::cursorPlacement() const
{
    return selectedTemplate_ 
        ? (CursorPlacement) selectedTemplate_->cursorPlacement() 
        : CURSOR_INSIDE_END;
}

const GroveLib::Node* Instance::cursorPosition() const
{
    if (!selectedTemplate_ || selectedTemplate_->cursorExpr().isNull())
        return origin();
    Xpath::ValueHolderPtr vh;
    try {
        vh = selectedTemplate_->cursorExpr()->
            makeInst(nsi_, Xpath::ExprContext());
    } catch (...) {
        return origin();
    }
    if (vh->value()->type() != Xpath::Value::NODESET)
        return origin();
    const Xpath::NodeSet& nset = vh->value()->getNodeSet();
    return nset.first()->node() ? nset.first()->node() : origin();
}

Instance::~Instance()
{   
    if (rootInstance_) {
        rootInstance_->removeModification(this, depth_);
        rootInstance_ = 0;
    }
    // prohibit myself from being killed twice in case of exception
    if (1 == getRefCnt()) {
        incRefCnt();
        InstanceTreeBase::remove();
    }
    remove_children(this);
}

/////////////////////////////////////////////////////////////

Instance* csl_instance_origin(const GroveLib::Node* n)
{
    GroveLib::VisitorLink* vl = n->firstVisitorLink();
    for (; vl; vl = vl->nextSibling()) {
        GroveLib::NodeVisitorLink& nvl = 
            *static_cast<GroveLib::NodeVisitorLink*>(vl);
        if (nvl.visitorMask() & GroveLib::NodeVisitor::NOTIFY_CSL_BIT)
            return static_cast<Instance*>(nvl.nodeVisitor());
    }
    return 0;
}

/////////////////////////////////////////////////////////////

} // namespace Csl
