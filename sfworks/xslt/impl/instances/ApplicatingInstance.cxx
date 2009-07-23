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
#include "xpath/Expr.h"
#include "xslt/impl/instructions/Template.h"
#include "xslt/impl/instructions/ApplicatingInstruction.h"
#include "xslt/impl/debug.h"
#include "xslt/impl/TemplateSelector.h"
#include "xslt/impl/instances/ApplicatingInstance.h"
#include "xslt/impl/instances/TemplateInst.h"
#include "xslt/impl/instances/TopStylesheetInst.h"
#include "grove/udata.h"

#include "common/asserts.h"
#include "common/stack_buffer.h"

#include <set>
#include <algorithm>

USING_COMMON_NS;

namespace Xslt {

ApplicatingInstance::ApplicatingInstance(const Instruction* instr,
                                         const ApplicatingInstruction* appInstr,
                                         const InstanceInit& init,
                                         Instance* parentInst,
                                         bool importsOnly)
    : Instance(instr, init, parentInst),
      selectInstChanged_(false),
      importsOnly_(importsOnly)
{
    const Instance* pi = parentInst;
    for (; pi; pi = pi->parent())
        if (pi->instruction()->asConstApplicatingInstruction())
            break;
    if (pi) {
        const ApplicatingInstance* const appi =
            static_cast<const ApplicatingInstance*>(pi);
        if (appInstr->sectProcType() == ApplicatingInstruction::UNSPEC_SECTIONS)
            sectProcType_ = appi->sectProcType_;
        else
            sectProcType_ = appInstr->sectProcType();
        if (appInstr->foldProcType() == ApplicatingInstruction::UNSPEC_FOLD)
            canFold_ = appi->canFold_;
        else
            canFold_ = appInstr->foldProcType();
    } else {
        sectProcType_ = ApplicatingInstruction::BALANCE_SECTIONS;
        canFold_ = ApplicatingInstruction::FOLDING_ENABLED;
    }
    selectInst_ = appInstr->selectExpr().makeInst(nsiContext(), exprContext());
    selectInst_.setWatcher(this);
}

const ApplicatingInstance*
ApplicatingInstance::asConstApplicatingInstance() const
{
    return this;
}

const String& ApplicatingInstance::mode() const
{
    return String::null();
}

ApplicatingInstance::~ApplicatingInstance()
{
}

const TemplateProvider* ApplicatingInstance::templateProvider() const
{
    return styleContext().templateProvider();
}

void ApplicatingInstance::notifyChanged(const COMMON_NS::SubscriberPtrBase* p)
{
    if (p != &selectInst_) {
        DBG(XSLT.UPDATE) << "ApplicatingInstance " << this
            << ": received TS UPDATE" << std::endl;
        UpdateListItem* udi = new UpdateListItem;
        udi->ts = static_cast<const TemplateSelectorPtr*>(p)->pointer();
        updateList_.push_back(udi);
    } else {
        selectInstChanged_ = true;
        DBG(XSLT.UPDATE) << "ApplicatingInstance " << this
            << ": received SELECTEXPR UPDATE" << std::endl;
    }
    setModified();
}

void ApplicatingInstance::append_instance(const Xpath::NodeSetItem* n,
                                          const ResultContext& rcontext)
{
    COMMON_NS::RefCntPtr<TemplateSelector> ts =
        new TemplateSelector(*n, this, rcontext.parent());
    COMMON_NS::RefCntPtr<Instance> inst =
        ts->selectedTemplate()->makeInst(InstanceInit(*n, rcontext), this);
    inst->asTemplateInstBase()->setSelector(ts.pointer(), this);
    ts->setTemplateInstance(inst.pointer());
}

typedef Xpath::NodeSetItem Nsi;

#define NSI_ALLOCA(var, n) STACK_BUFFER(var, const Nsi*, n + 1)

static const Nsi** trim_sect(const Xpath::NodeSet& nset,
                             const Nsi** rp,
                             bool preserveLeft)
{
    const Nsi* n = nset.realFirst();
    if (preserveLeft) {
        for (; n && n->node()->isGroveSectionNode();  n = n->realNext())
            *rp++ = n;
        for (; n && !n->node()->isGroveSectionNode(); n = n->realNext())
            *rp++ = n;
    } else {
        for (; n && n->node()->isGroveSectionNode(); n = n->realNext())
            ;
        for (; n && !n->node()->isGroveSectionNode(); n = n->realNext())
            *rp++ = n;
        for (; n; n = n->realNext())
            *rp++ = n;
    }
    return rp;
}

static inline bool is_sect_start(const GroveLib::Node* n)
{
    return ((n->nodeType() & (GroveLib::Node::SECT_NODE_MASK|
        GroveLib::Node::SECT_END_BIT)) == GroveLib::Node::SECT_NODE_MASK);
}

static inline bool is_sect_end(const GroveLib::Node* n)
{
    return ((n->nodeType() & (GroveLib::Node::SECT_NODE_MASK|
        GroveLib::Node::SECT_END_BIT)) ==
            (GroveLib::Node::SECT_NODE_MASK|GroveLib::Node::SECT_END_BIT));
}

static const Nsi** balance_sect(const Xpath::NodeSet& nset,
                                const Nsi** rp,
                                char bal)
{
    DBG(XSLT.SECT) << "ApplicatingInstance::balance_sect" << std::endl;
    const int ibal = bal;
    if (ibal != ApplicatingInstruction::BALANCE_SECTIONS)
        return trim_sect(nset, rp,
            ibal == ApplicatingInstruction::PRESERVE_LEFT_SECTIONS);
    const Nsi* n = 0;
    typedef std::set<GroveLib::Node*> Sset;
    Sset sset;
    for (n = nset.realFirst(); n; n = n->realNext())
        if (n->node()->isGroveSectionNode())
            sset.insert(n->node());
    for (n = nset.realFirst(); n; n = n->realNext()) {
        GroveLib::Node* cn = n->node();
        if (!cn->isGroveSectionNode()) {
            *rp++ = n;
            continue;
        }
        if (is_sect_start(cn)) {
            GroveLib::GroveSectionStart* ers =
                static_cast<GroveLib::GroveSectionStart*>(cn);
            GroveLib::GroveSectionEnd* ere = static_cast
                <GroveLib::GroveSectionEnd*>(ers->getSectEnd());
            if (sset.find(ere) == sset.end())
                continue;
            const Nsi* n1 = n->realNext();
            if (n1 && n1->node() == ere && cn->nextSibling() != ere) {
                n = n1;
                continue;
            }
            *rp++ = n;
        } else { // section end
            GroveLib::GroveSectionEnd* ere =
                static_cast<GroveLib::GroveSectionEnd*>(cn);
            if (sset.find(ere->getSectStart()) == sset.end())
                continue;
            *rp++ = n;
        }
    }
    return rp;
}

static void balance_sect(const Xpath::NodeSet& nset,
                         Xpath::NodeSet& out,
                         char bal)
{
    NSI_ALLOCA(nb1, nset.realSize());
    const Nsi** np1 = nb1;
    np1 = balance_sect(nset, np1, bal);
    out.clear();
    for (const Nsi** ip = nb1; ip < np1; ++ip)
        out += (*ip)->node();
}

void ApplicatingInstance::apply(const ResultContext& rcontext)
{
    const Xpath::NodeSet& new_nset = selectInst_->value()->getNodeSet();
    DBG(XSLT.RSEL) << "ApplicatingInstance " << this << "::apply SP"
        << (int)sectProcType_ << " nodeset: " << std::endl;
    DBG_EXEC(XSLT.RSEL, new_nset.dump());

    const Xpath::NodeSetItem* n = 0;

    switch (sectProcType_) {
        case ApplicatingInstruction::OMIT_SECTIONS:
            nodeSet_.clear();
            for (n = new_nset.first(); n; n = n->next())
                nodeSet_ += n->node();
            for (n = nodeSet_.first(); n; n = n->next())
                append_instance(n, rcontext);
            break;

        case ApplicatingInstruction::PRESERVE_LEFT_SECTIONS:
        case ApplicatingInstruction::PRESERVE_RIGHT_SECTIONS:
        case ApplicatingInstruction::BALANCE_SECTIONS:
            balance_sect(new_nset, nodeSet_, sectProcType_);
            for (n = nodeSet_.realFirst(); n; n = n->realNext())
                append_instance(n, rcontext);
            break;

        case ApplicatingInstruction::PRESERVE_SECTIONS:
            new_nset.copyTo(nodeSet_);
            for (n = nodeSet_.realFirst(); n; n = n->realNext())
                append_instance(n, rcontext);
            break;

        default:
            break; // can't happen
    }
}

void ApplicatingInstance::updateList()
{
    while (updateList_.first()) {
        DBG(XSLT.UPDATE) << "ApplicatingInstance::update updateList iteration"
            << std::endl;
        COMMON_NS::RefCntPtr<TemplateSelector> ts = updateList_.first()->ts;
        updateList_.pop_front();
        const Template* prev_template = ts->selectedTemplate();
        ts->setModified(false);
        ts->update();
        if (ts->selectedTemplate() == prev_template)
            continue;   // same template matched - no changes
        DBG(XSLT.UPDATE) << "ApplicatingInstance::update: remaking TI"
            << std::endl;
        Instance* ibefore = ts->templateInstance()->nextSibling();
        ResultContext rcontext(ts->templateInstance()->resultContext());
        const Xpath::NodeSetItem& ctx = ts->templateInstance()->nsiContext();
        ts->templateInstance()->disposeResult();
        ts->templateInstance()->remove();
        // trick to preserve tree consistency: first append instance,
        // then remove it and place it properly.
        COMMON_NS::RefCntPtr<Instance> inst =
            ts->selectedTemplate()->makeInst(InstanceInit(ctx, rcontext), this);
        inst->remove();
        inst->asTemplateInstBase()->setSelector(ts.pointer(), this);
        ts->setTemplateInstance(inst.pointer());
        if (ibefore) {
            ibefore->InstanceTreeBase::insertBefore(inst.pointer());
            inst->setSiblingIndex();
        } else
            appendChild(inst.pointer());
    }
}

void ApplicatingInstance::update()
{
    if (selectInstChanged_) {
        DBG(XSLT.UPDATE) << "ApplicatingInstance::update() selectInst"
            << std::endl;
        selectInstChanged_ = false;
        reselect();
    }
    updateList();
}

class NsiCmp {
public:
    bool operator()(const Nsi* a, const Nsi* b) const
    {
        return a->node() < b->node();
    }
};

//
// Process change of selectInst_ nodeset
//
void ApplicatingInstance::reselect()
{
    const Xpath::NodeSet& new_nset =
        selectInst_->value()->getNodeSet();
    bool ulc = !!updateList_.first();
    const int sectProcType = sectProcType_;
    DBG(XSLT.SECT) << "Reselect: SPT = " << sectProcType << std::endl;
    bool skipers = (sectProcType == ApplicatingInstruction::OMIT_SECTIONS);
    bool balsect = (sectProcType >= ApplicatingInstruction::BALANCE_SECTIONS);

    DBG(XSLT.RSEL) << "Reselect (OLD) nodeset:" << std::endl;
    DBG_EXEC(XSLT.RSEL, nodeSet_.dump());

    DBG(XSLT.RSEL) << "Reselect (new) nodeset:" << std::endl;
    DBG_EXEC(XSLT.RSEL, new_nset.dump());

    // buffer for "new" nodeset
    uint  nsize = new_nset.realSize();
    NSI_ALLOCA(nb1, nsize);
    const Nsi** np1 = nb1;
    const Nsi** ne1 = nb1 + nsize;

    // buffer for "old" nodeset
    NSI_ALLOCA(nb2, nodeSet_.realSize());
    const Nsi** np2 = nb2;
    const Nsi** ne2 = nb2 + nodeSet_.realSize();

    // buffer for items in old nodeset which appear in new nodeset
    uint maxsize = (nsize > nodeSet_.realSize()) ? nsize : nodeSet_.realSize();
    NSI_ALLOCA(outb, maxsize);
    const Nsi** outp = outb;

    // fill new/old buffers with pointers to corresponding NSI's
    const Nsi* ip = 0;
    if (skipers) {
        for (ip = new_nset.first(); ip; ip = ip->next())
            *np1++ = ip;
        ne1 = np1;
        RT_ASSERT((np1 - nb1) == (int)new_nset.contextSize());
    } else {
        if (!balsect) {
            for (ip = new_nset.realFirst(); ip; ip = ip->realNext())
                *np1++ = ip;
            ne1 = np1;
            RT_ASSERT((np1 - nb1) == (int)new_nset.realSize());
        } else {
            np1 = balance_sect(new_nset, np1, sectProcType_);
            ne1 = np1;
        }
    }
    for (ip = nodeSet_.realFirst(); ip; ip = ip->realNext())
        *np2++ = ip;
    RT_ASSERT((np2 - nb2) == (int)nodeSet_.realSize());

    // need copy of new_nset before sorting
    uint ksize = balsect ? (ne1 - nb1) : 0;
    NSI_ALLOCA(nb3, ksize);
    const Nsi** ne3 = nb3 + ksize;
    if (ksize)
        (void) memcpy(nb3, nb1, ksize * sizeof (Nsi*));

    // sort buffers - need sorted sequences for difference, intersect etc.
    std::stable_sort(nb1, ne1, NsiCmp());
    std::stable_sort(nb2, ne2, NsiCmp());

    // make 'outb' to contain pointers to Nsi's in the 'old' list
    outp = std::set_intersection(nb2, ne2, nb1, ne1, outb, NsiCmp());

    // save old nodeset; zero nodeSet_; save old nodeset (context)size
    ulong saved_size = nodeSet_.contextSize();
    nodeSet_.release();

    // in the following loop, create nodeSet_ with the property that
    // it uses the NodeSetItem's from the 'old' set if node() is the same.
    // These items (with the same node but possibly different size/pos)
    // are candidates for updateContext (rather than re-creating instances)
    if (skipers) {
        for (const Nsi* n = new_nset.first(); n; n = n->next()) {
            // check whether current item is in old_set
            const Nsi** on = std::lower_bound(outb, outp, n, NsiCmp());
            if (on != outp && (*on)->node() == n->node()) {  // found
                // take from old list, add to new one
                nodeSet_.append(const_cast<Nsi*>(*on));
            } else
                nodeSet_ += n->node();
        }
    } else if (!balsect) {
        for (const Nsi* n = new_nset.realFirst(); n; n = n->realNext()) {
            const Nsi** on = std::lower_bound(outb, outp, n, NsiCmp());
            if (on != outp && (*on)->node() == n->node()) {
                nodeSet_.append(const_cast<Nsi*>(*on));
            } else
                nodeSet_ += n->node();
        }
    } else {
        for (const Nsi** n = nb3; n < ne3; ++n) {
            const Nsi** on = std::lower_bound(outb, outp, *n, NsiCmp());
            if (on != outp && (*on)->node() == (*n)->node()) {
                nodeSet_.append(const_cast<Nsi*>(*on));
            } else
                nodeSet_ += (*n)->node();
        }
    }
    typedef std::map<const GroveLib::Node*,
                     COMMON_NS::RefCntPtr<Instance> > OldMap;
    OldMap oldMap;

    // in this step, we preserve TemplateInstances which we can keep
    Instance* ci = 0;
    Instance* lastWp = 0;
    for (ci = firstChild(); ci; ) {
        if (ci->instruction()->type() == Instruction::WITH_PARAM) {
            lastWp = ci;
            ci = ci->nextSibling();
            continue;
        }
        GroveLib::Node* const cn = ci->nsiContext().node();
        if (GroveLib::is_node_modified(cn)) {
            DBG(XSLT.RSEL) << "Reselecting modified node: "
                << cn->nodeName() << std::endl;
            topInst_->markModifiedNode(cn);
            if (ulc)
                remove_update(ci);
            ci->disposeResult();
            Instance* cnext = ci->nextSibling();
            ci->remove();
            ci = cnext;
            continue;
        }
        const Nsi** on =
            std::lower_bound(outb, outp, &ci->nsiContext(), NsiCmp());
        if (on != outp && (*on)->node() == cn) {
            oldMap[cn] = ci;
            ci = ci->nextSibling();
        } else {
            if (ulc)
                remove_update(ci);
            ci->disposeResult();
            Instance* cnext = ci->nextSibling();
            ci->remove();
            ci = cnext;
        }
    }
    // if some of kept instances have result, insert new instances before
    // old instances with this context.
    const InstanceResult* firstRes = firstResult();
    DBG_IF(XSLT.RESULT) {
        if (firstRes) {
            DDBG << "reselect: First result: ";
            firstRes->node()->dump();
        } else
            DDBG << "reselect: no firstResult found\n";
    }
    // Kill everything except WithParams and preserved TI's
    // (they are kept in oldMap)
    if (lastWp) {
        lastWp = lastWp->nextSibling();
        while (lastWp) {
            ci = lastWp->nextSibling();
            if (ulc)
                remove_update(lastWp);
            lastWp->remove();
            lastWp = ci;
        }
    } else
        removeAllChildren();

    // delete unused items in old list. old_list is invalidated now.
    NSI_ALLOCA(todel, maxsize);
    const Nsi** todelp =
        std::set_difference(nb2, ne2, outb, outp, todel, NsiCmp());
    np1 = todel;
    while (np1 < todelp)
        delete *np1++;

    // Re-instantiate/updateContext when necessary
    OldMap::iterator it;
    uint updateSize =
        (new_nset.contextSize() == saved_size) ? 0 : Instance::SIZE_CHANGED;
    uint rpos = 1;
    for (Nsi* nsi = nodeSet_.realFirst(); nsi; nsi = nsi->realNext()) {
        it = oldMap.find(nsi->node());
        if (it != oldMap.end()) {
            uint utype = updateSize;
            if (rpos != nsi->pos()) {
                utype |= Instance::POS_CHANGED;
                nsi->setPos(rpos);
            }
            DBG(XSLT.RSEL) << " *Rsel append-old, utype " << utype << std::endl;
            DBG_EXEC(XSLT.RSEL, it->second->dump());
            appendChild(it->second.pointer());
            if (firstRes && lastChild()->firstResult())
                firstRes = 0;
            it->second->updateContext(utype);
            ++rpos;
        } else {
            if (firstRes) {
                append_instance(nsi,
                    ResultContext(firstRes->parent(), firstRes->node()));
                if (lastChild()->firstResult())
                    firstRes = 0;
            } else {
                append_instance(nsi, resultContext());
            }
            DBG(XSLT.RSEL) << "*Rsel append-new" << std::endl;
            DBG_EXEC(XSLT.RSEL, lastChild()->dump());
            ++rpos;
        }
    }
    //DBG(XSLT.RSEL) << "===================================================\n";
    //DBG_EXEC(XSLT.RSEL, GroveLib::Node::dumpSubtree(firstResult()->parent()));
    //DBG(XSLT.RSEL) << "===================================================\n";
}

// Removes update token from updateList_
void ApplicatingInstance::remove_update(Instance* i)
{
    UpdateList::iterator it = updateList_.begin();
    for (; it != updateList_.end(); ++it) {
        if (it->ts->templateInstance() == i) {
            DBG(XSLT.UPDATE) << "Removing instance " << i
                << " from update list because of SelectExpr update\n";
            it->remove();
            return;
        }
    }
}

// This isolates context updates from propagating further down
void ApplicatingInstance::updateContext(int /*utype*/)
{
}

void ApplicatingInstance::dump() const
{
    Instance::dump();
}

///////////////////////////////////////////////////////////////////////////

void Instance::notifyTemplateStateChange(const GroveLib::Node* origin) const
{
    for (const Instance* i = this; i; i = i->parent()) {
        const Instance* ti = i->asConstTemplateInst();
        if (0 == ti)
            continue;
        const Instance* pi = i->parent();
        if (!pi)
            return;
        pi = pi->asConstApplicatingInstance();
        if (!pi || ti->nsiContext().node() != origin ||
            pi->nsiContext().node() == origin)
                continue;
        TemplateSelector* tsel = const_cast<Instance*>(ti)->
            asTemplateInstBase()->templateSelector();
        if (0 == tsel)
            return;
        DDBG << "TemplateSelector: Altering fold state for "
            << nsi_->node()->nodeName() << " to "
            << GroveLib::is_node_folded(nsi_->node()) << std::endl;
        tsel->setChanged();
        return;
    }
}

} // namespace Xslt
