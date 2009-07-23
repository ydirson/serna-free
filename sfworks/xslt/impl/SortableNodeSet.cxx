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

#ifdef XSLT_SORTING

#include <algorithm>
#include "xpath/impl/NodeSetImpl.h"
#include "xpath/impl/NodeSetValue.h"
#include "xslt/impl/SortableNodeSet.h"

namespace Xslt {

SortInfo::SortInfo(const Sort::List& sortSpecs,
                   const Xpath::NodeSetItem& context,
                   Sortable* sortable)
    : node_(context->node()),
      values_(sortSpecs.size()),
      sortable_(sortable)
{
    Sort::CIterator i = sortSpecs.begin();
    for (uint pos = 0; pos < values_.size(); i++, pos++) {
        values_[pos].setWatcher(this);
        values_[pos] = (*i)->selectExpr().makeInst(context.pointer());
    }
}

void SortInfo::notifyChanged(const COMMON_NS::SubscriberPtrBase*)
{
    //sortable_->sort();
    SubscriberPtrPublisher::notifyChanged();
}

/*
*/
SortableNodeSet::SortableNodeSet(const Sort::List& sortSpecs,
                                 const Xpath::Expr& selectExpr,
                                 const Xpath::NodeSetItem& context)
    : selectInst_(this, selectExpr.makeInst(context.pointer())),
      sortSpecs_(sortSpecs),
      sortOrder_(sortSpecs_),
      sortInfoVector_(0),
      context_(context),
      changed_(0)
{
    fillInfoVector();
    sort();
}

SortableNodeSet::~SortableNodeSet()
{
    clearSortInfos();
}

void SortableNodeSet::clearSortInfos()
{
    for (uint i = 0; i < sortInfoVector_.size(); i++) {
        delete sortInfoVector_[i];
        sortInfoVector_[i] = 0;
    }
}

void SortableNodeSet::fillInfoVector()
{
    const Xpath::NodeSet& unsorted_set = selectInst_->value()->getNodeSet();
    clearSortInfos();
    sortInfoVector_.resize(unsorted_set.size());
    const Xpath::NodeSetItem* n = unsorted_set.first();
    const uint size = sortInfoVector_.size();
    for (int pos = 0; n; n = n->next(), ++pos) {
        ContextPtr cont = static_cast<const Context*>
            (context_->make(n->node(), n->pos(), size));
        sortInfoVector_[pos] =
            new SortInfoSubscriber(this, new SortInfo(sortSpecs_, cont, this));
    }
}

void SortableNodeSet::sort()
{
    std::sort(sortInfoVector_.begin(), sortInfoVector_.end(), sortOrder_);
    Xpath::NodeSet sorted_set;
    const uint size = sortInfoVector_.size();
    for (uint pos = 0; pos < size; pos++)
        sorted_set += (*sortInfoVector_[pos])->node();
    if (nodeSet_.isNull() || !(sorted_set == nodeSet_->getNodeSet()))
        nodeSet_ = new Xpath::NodeSetValue(sorted_set);
}

void SortableNodeSet::notifyChanged(const COMMON_NS::SubscriberPtrBase* ptr)
{
    if (0 == changed_ || &selectInst_ == ptr) {
        changed_ = ptr;
        SubscriberPtrPublisher::notifyChanged();
    }
}

Xpath::ConstValuePtr SortableNodeSet::value() const
{
    if (0 != changed_) {
        if (&selectInst_ == changed_) {
            const_cast<SortableNodeSet*>(this)->clearSortInfos();
            const_cast<SortableNodeSet*>(this)->fillInfoVector();
        }
        const_cast<SortableNodeSet*>(this)->sort();
        const_cast<SortableNodeSet*>(this)->changed_ = 0;
    }
    return nodeSet_;
}

} // namespace Xslt

#endif // XSLT_SORTING

