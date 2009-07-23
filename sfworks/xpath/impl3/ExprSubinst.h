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
#ifndef XPATH_EXPR_SUBINST_H
#define XPATH_EXPR_SUBINST_H

#include "xpath/xpath_defs.h"
#include "xpath/ValueHolder.h"
#include "common/XList.h"

namespace Xpath {

class ExprInst;

class XPATH_EXPIMP ExprSubInst : public Common::XListItem<ExprSubInst> {
public:
    enum SubInstType {  // type of subinst, used by extension functions
        DOS_SUBINST,
        VALUEWATCHER_SUBINST,
        RSSWATCHER_SUBINST,
        XSLT_SUBINST = 10,      // subinsts 10+ belong to XSLT
        USER_SUBINST = 1000     // subinsts 1000+ are user-defined
    };
    ExprInst& exprInst() const;

    // called on all subinstances on ExprInst::setModified
    virtual void exprChanged() {}

    // 0 is reserved for DoS visitor, other types are user-defined
    virtual int  type() const = 0;

    virtual ~ExprSubInst() {}
};

template <class PtrBase, class HoldType>
class ValueWatcherImpl : public ExprSubInst,
                         public Common::SubscriberPtrWatcher,
                         public PtrBase {
public:
    typedef ValueWatcherImpl<PtrBase, HoldType> WatcherBase;

    ValueWatcherImpl(HoldType* publisher)
#if defined(_MSC_VER) && (_MSC_VER > 1300)
    {   // work around VS2008 compiler bug
        setWatcher(this); 
        if (publisher)
            publisher->registerSubscriber(this);
    }
#else
        : PtrBase(this, publisher) {}
#endif
    virtual int     type() const { return VALUEWATCHER_SUBINST; }
    virtual void    exprChanged() { delete this; }
    virtual void    notifyChanged(const Common::SubscriberPtrBase*)
    {
        exprInst().setModified();
    }
};

class ValueWatcher : public ValueWatcherImpl<Common::SubscriberPtrBase,
                                             Common::SubscriberPtrPublisher> {
public:
    typedef ValueWatcherImpl<Common::SubscriberPtrBase,
                             Common::SubscriberPtrPublisher> WatcherBase;
    ValueWatcher(Common::SubscriberPtrPublisher* publisher)
        : WatcherBase(publisher) {}
    XPATH_OALLOC(ValueWatcher);
};

class ValueHolderWatcher :
    public ValueWatcherImpl<Common::SubscriberPtr<ValueHolder>, ValueHolder> {
    typedef ValueWatcherImpl<Common::SubscriberPtr<ValueHolder>, ValueHolder>
        WatcherBase;
public:
    ValueHolderWatcher(ValueHolder* vh)
        : WatcherBase(vh) {}
    XPATH_OALLOC(ValueHolderWatcher);
};

} // namespace Xpath

#endif // XPATH_EXPR_SUBINST_H_

