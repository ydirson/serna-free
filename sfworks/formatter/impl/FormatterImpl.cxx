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

#include "common/safecast.h"
#include "common/Message.h"
#include "common/XTreeNode.h"
#include "common/ScopeGuard.h"

#include "grove/Nodes.h"
#include "grove/EntityReferenceTable.h"
#include "xslt/ResultOrigin.h"

#include "formatter/XslFoExt.h"
#include "formatter/impl/debug.h"
#include "formatter/impl/ReferencedFo.h"
#include "formatter/impl/FormatterImpl.h"
#include "formatter/impl/PageMaster.h"
#include "XslMessages.h"

#include <iostream>

USING_GROVE_NAMESPACE;
USING_COMMON_NS;

//#define VERIFY_MODREGISTRY

namespace Formatter
{

#define NOTIFY_FO(node, func) \
    Chain* chain = XslFoExt::areaChain(node); \
    if (chain) { \
        ReferencedFo* rfo = \
            dynamic_cast<ReferencedFo*>(chain); \
        if (rfo) \
            rfo->func ; \
    }

class FormatterGroveVisitor : public GroveLib::GroveVisitor {
public:
    FormatterGroveVisitor(GroveLib::Grove* grove)
        : GroveVisitor(grove->document())
    {
        grove->document()->registerVisitor(this);
    }
    virtual void nodeDestroyed(const GroveLib::Node*) {}

    virtual void childInserted(const GroveLib::Node* child)
    {
        NOTIFY_FO(child->parent(), childInserted(child));
    }
    virtual void childRemoved(const GroveLib::Node* node,
                              const GroveLib::Node* child)
    {
        NOTIFY_FO(node, childRemoved(node, child));
    }
    virtual void attributeChanged(const GroveLib::Attr* attr)
    {
        NOTIFY_FO(attr->element(), attributeChanged(attr));
    }
    virtual void attributeRemoved(const GroveLib::Element* elem,
                                  const GroveLib::Attr* attr)
    {
        NOTIFY_FO(elem, attributeRemoved(elem, attr));
    }
    virtual void attributeAdded(const GroveLib::Attr* attr)
    {
        NOTIFY_FO(attr->element(), attributeAdded(attr));
    }
    virtual void textChanged(const GroveLib::Text* text)
    {
        NOTIFY_FO(text, textChanged(text));
    }
};

Formatter* makeFormatter(Messenger* messenger,
                         const MediaInfo& mediaInfo,
                         FormattedView* view, FontMgr* fontMgr,
                         GroveLib::GrovePtr& fot,
                         const AreaViewFactory* viewFactory,
                         TagMetrixTable& tagMetrixTable)
{
    return new FormatterImpl(messenger, mediaInfo, view, fontMgr, fot,
                             viewFactory, tagMetrixTable);
}

FormatterImpl::FormatterImpl(Messenger* messenger,
                             const MediaInfo& mediaInfo, FormattedView* view,
                             FontMgr* fontMgr, GroveLib::GrovePtr& fot,
                             const AreaViewFactory* viewFactory,
                             TagMetrixTable& tagMetrixTable)
    : isModified_(false),
      isFormatting_(false),
      messenger_(messenger),
      mstream_(XslMessages::getFacility()),
      mediaInfo_(mediaInfo),
      fontMgr_(fontMgr),
      foData_(mstream_, mediaInfo_, fontMgr_, tagMetrixTable, *this),
      view_(view),
      foGrove_(fot),
      viewFactory_(viewFactory)
{
    if (messenger_)
        mstream_.setMessenger(&*messenger_);
    fontMgr_->clear();
    Node* fo_root = foGrove_->document()->documentElement();
    rootFo_ = new RootFo(fo_root, foData_);
    branches_.appendChild(new Branch(rootFo_, view_, viewFactory_, &foMap_, 
                              foData_.mediaInfo_.isPaginated_));
    formatterGroveVisitor_ = new FormatterGroveVisitor(fot.pointer());
}

FormatterImpl::~FormatterImpl() 
{
    branches_.removeAllChildren();
}

bool FormatterImpl::add(Fo* fo)
{
    if (!ModificationRegistry::hasInstance(fo)) {
        ModificationRegistry::registerInstance(fo);
        DBG_IF(XSL.FO) {
            COMMON_NS::TreelocRep tl, tl1, tl2;
            fo->getTreeloc(tl);
            fo->treeloc(tl1);
            Xslt::resultOrigin(fo->headFoNode())->treeLoc(tl2);
            DBG(XSL.FO) << "Register FO: " << fo << "("
                << fo->name() << ")\nTLOC: " << tl.toString() << std::endl;
            DBG(XSL.FO) << "FO TLOC: " << tl1.toString() << std::endl;
            DBG(XSL.FO) << "Head FO Node: " << fo->headFoNode()->nodeName()
                << std::endl;
            DBG(XSL.FO) << "Origin node TL: " << tl2.toString() << std::endl;
            DBG_EXEC(XSL.FO, fo->dump());
            DBG(XSL.FO) << "===================================\n";
        }
        isModified_ = true;
        return true;
    }
    return false;
}

bool FormatterImpl::isFinished() const
{
    return (branches_.empty() && ModificationRegistry::isEmpty());
}

static bool has_area(const Chain* chain)
{
    if (!chain->firstChunk())
        return false;
    if (chain->prevChain())
        return has_area(chain->prevChain());
    else
        if (chain->parentChain())
            return has_area(chain->parentChain());
    return true;
}

bool FormatterImpl::isFormatted(const GroveLib::Node* foNode) const
{
    if (isFinished())
        return (0 != XslFoExt::areaChain(foNode));
    if (!branches_.empty() && branches_.firstChild()->willProcess(foNode))
        return false;
    if (!ModificationRegistry::isEmpty()) {
        const Fo* first_fo =
            const_cast<FormatterImpl*>(this)->ModificationRegistry::getFirst();
        //XTreeNodeCmpResult cmp = first_fo->headFoNode()->comparePos(foNode);
        //if (cmp == LESS || cmp == EQUAL)
        if (is_fo_node_less(first_fo->headFoNode(), foNode))
            return false;
        const GroveLib::Node* node = first_fo->headFoNode();
        while (node) {
            if (foNode == node)
                return false;
            node = node->parent();
        }
    }
    return true;
}

void FormatterImpl::format(bool isPostponable)
{
    ValScopeGuard<bool, bool> lock_guard(isFormatting_, true);

    DBG(XSL.FORMATTER) << "Formatter: formatting " << branches_.countChildren()
                       << "branch(es)" << std::endl;
    //DBG_IF(XSL.FORMATTER) dump();
    for (;;) {
        if (!ModificationRegistry::isEmpty()) {
#ifdef VERIFY_MODREGISTRY
            InstanceSet::const_iterator lower = instanceSet().begin();
            InstanceSet::const_iterator i = lower;
            COMMON_NS::TreelocRep pv(128), pv1(128);
            ++i;
            if (i != instanceSet().end()) {
                for (; i != instanceSet().end(); ++i) {
                    (*lower)->treeloc(pv);
                    (*i)->treeloc(pv1);
                    if (pv1 < pv)
                        lower = i;
                }
            }
            Fo* lowfo = *lower;
            FoPtr first_fo = ModificationRegistry::getFirst();
            RT_ASSERT(lowfo == first_fo.pointer());
#else
            FoPtr first_fo = ModificationRegistry::getFirst();
#endif // VERIFY_MODREGISTRY
            DBG_IF(XSL.FO) {
                COMMON_NS::TreelocRep tl, tl1;
                COMMON_NS::String s, s1;
                first_fo->getTreeloc(tl);
                first_fo->treeloc(tl1);
                DBG(XSL.FO) << "FO MOD (INST TLOC): "
                    << tl.toString() << std::endl;
                DBG(XSL.FO) << "FO MOD (FO TLOC): " <<
                    tl1.toString() << std::endl;
            }
            DBG(XSL.FORMATTER) << "Fo modified:" 
                               << first_fo.pointer() << std::endl;
            DBG_IF(XSL.FORMATTER) first_fo->dump();
            if (branches_.empty()) {
                branches_.appendChild(
                    new Branch(first_fo, view_, viewFactory_, &foMap_,  
                               foData_.mediaInfo_.isPaginated_));
                DBG(XSL.FORMATTER) << "New branch created:"
                                   << branches_.firstChild() << std::endl;
            }
            else
                if (!branches_.firstChild()->willProcess(first_fo)) {
                    branches_.firstChild()->insertBefore(
                        new Branch(first_fo, view_, viewFactory_, &foMap_,
                                   foData_.mediaInfo_.isPaginated_));
                    DBG(XSL.FORMATTER)
                        << "New branch created:" << branches_.firstChild()
                        << " of "<< branches_.countChildren()
                        << " branches" << std::endl;
                }
        }
        if (branches_.empty()) {
            DBG(XSL.FORMATTER) << "No more branches formatting finished\n";
            return;
        }
        DBG(XSL.FORMATTER)
            << "Brocessing branch:" << branches_.firstChild()
            << " of "<< branches_.countChildren() << " branches" << std::endl;
        switch (branches_.firstChild()->process(isPostponable)) {
            case FINISHED :
                DBG(XSL.FORMATTER) << "Finished branch:"
                                   << branches_.firstChild()<< std::endl;
                branches_.firstChild()->remove();
                break;
            case POSTPONED :
                DBG(XSL.FORMATTER) << "Postponed branch:"
                                   << branches_.firstChild()<< std::endl;
                return;
                break;
            default:
                break;
        }
    }
    DBG(XSL.FORMATTER) << "... Formatting finished\n";
}

void FormatterImpl::detachFromFormatter(Fo* fo, 
                                        const FoController* preserveBranch)
{
    if (0 != fo->isFormatting()) {
        Branch* branch = branches_.firstChild();
        while (branch) {
            if (branch->isProcesses(fo, preserveBranch)) {
                Branch* next_branch = branch->nextSibling();
                DBG(XSL.FORMATTER)
                    << "Removing branch:" << branch << std::endl;
                branch->remove();
                branch = next_branch;
            }
            else
                branch = branch->nextSibling();
        }
    }
}

const Area* FormatterImpl::lastAreaMade() const
{
    if (!branches_.empty())
        return branches_.firstChild()->lastAreaMade();
    return 0;
}

Area* FormatterImpl::rootArea() const
{
    return rootFo_->firstChunk();
}

void FormatterImpl::detachAreaTree()
{
    branches_.removeAllChildren();
    ModificationRegistry::clear();
    if (!rootFo_.isNull() && rootArea()) {
        rootArea()->removeFromChain(true);
        SAFE_CAST(RootFo*, rootFo_.pointer())->detachRootArea();
    }
}

}

