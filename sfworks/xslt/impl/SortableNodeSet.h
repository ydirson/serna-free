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

#ifndef XSLT_SORTABLE_NODESET_H
#define XSLT_SORTABLE_NODESET_H
#ifdef _MSC_VER
# pragma warning( disable : 4786 )
#endif

#ifdef XSLT_SORTING

#include "xslt/xslt_defs.h"
#include "common/SubscriberPtr.h"
#include "xpath/Expr.h"
#include "xpath/ValueHolder.h"
#include "xslt/impl/xpath_exprinst.h"
#include "xslt/impl/debug.h"
#include "xslt/impl/instructions/Sort.h"
#include <list>
#include <vector>

namespace Xslt {

class Sortable {
public:
    XSLT_OALLOC(Sortable);
    virtual ~Sortable() {}
    //!
    virtual void    sort() = 0;
};

/*
*/
class SortInfo : public COMMON_NS::RefCntPtr<>,
                 public COMMON_NS::SubscriberPtrWatcher,
                 public COMMON_NS::SubscriberPtrPublisher {
public:
    XSLT_OALLOC(SortInfo);
    typedef std::vector<Xpath::ValueHolderPtr> ValueVector;

    SortInfo(const Sort::List& sortSpecs,
             const Xpath::NodeSetItem& context,
             Sortable* sortable);
    //!
    GroveLib::Node*         node() const {return node_;}
    //!
    Xpath::ConstValuePtr    operator[](uint pos) const
    {
        return values_[pos]->value();
    }
    //! Processes the changes of the child subexpression
    void                    notifyChanged(const COMMON_NS::SubscriberPtrBase*);

private:
    GroveLib::Node*         node_;
    ValueVector             values_;
    Sortable*               sortable_;
};

/*
*/
class SortableNodeSet : public Sortable,
                                    public Xpath::ValueHolder,
                                    public COMMON_NS::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(SortableNodeSet);
    typedef COMMON_NS::SubscriberPtr<SortInfo> SortInfoSubscriber;
    typedef SortInfoSubscriber*         SortInfoPtr;
    typedef std::vector<SortInfoPtr>    SortInfoVector;

    class SortOrder {
    public:
        XSLT_OALLOC(SortOrder);
        typedef std::vector<bool>           OptionVector;

        SortOrder(const Sort::List& sortSpecs)
            : sortSpecs_(sortSpecs),
              optionsLength_(sortSpecs_.size()),
              options_(optionsLength_) {
            Sort::CIterator sort = sortSpecs_.begin();
            for (uint i = 0; i < optionsLength_; sort++, i++)
                options_[i] = (Sort::DESCENDING == (*sort)->order());
        }
        //!
        bool    operator()(const SortInfoPtr a, const SortInfoPtr b) const {
            for (uint i = 0; i < optionsLength_; i++) {
                const COMMON_NS::String& a_str = (**a)[i]->getString();
                const COMMON_NS::String& b_str = (**b)[i]->getString();
                if (a_str != b_str)
                    return (a_str < b_str) ^ (options_[i]);
            }
            return false;
        }
    private:
        const Sort::List&   sortSpecs_;
        const uint          optionsLength_;
        OptionVector        options_;
    };

    SortableNodeSet(const Sort::List& sortSpecs,
                    const Xpath::Expr& selectExpr,
                    const Xpath::NodeSetItem& context);
    virtual ~SortableNodeSet();

    //! Reimplemented from Value holder
    Xpath::ConstValuePtr    value() const;
    //! Processes the changes of the child subexpression
    void notifyChanged(const COMMON_NS::SubscriberPtrBase* ptr);

protected:
    //!
    void                    clearSortInfos();
    //!
    void                    fillInfoVector();
    //! Reimplemented from sortable
    void                    sort();
private:
    Xpath::ValueHolderPtr   selectInst_;
    const Sort::List&       sortSpecs_;
    SortOrder               sortOrder_;
    SortInfoVector          sortInfoVector_;
    const Xpath::NodeSetItem& context_;
    Xpath::ConstValuePtr     nodeSet_;
    const COMMON_NS::SubscriberPtrBase* changed_;
};

} // namespace Xslt

#endif // XSLT_SORTING

#endif // XSLT_SORTABLE_NODESET_H
