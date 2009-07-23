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
#ifndef COMMON_SUBSCRIBER_PTR_H
#define COMMON_SUBSCRIBER_PTR_H

#include "common/common_defs.h"
#include "common/RefCntPtr.h"
#include "common/RefCounted.h"
#include "common/CDList.h"
#include "common/SList.h"

COMMON_NS_BEGIN

class SubscriberPtrPublisher;
class SubscriberPtrBase;

/*! A class which receives notifcations originating from
 *  SubscriberPtrPublisher via SubscriberPtr
 */
class SubscriberPtrWatcher {
public:
    // Processes the notification about resoure`s change
    virtual void    notifyChanged(const SubscriberPtrBase* ptr) = 0;
    virtual ~SubscriberPtrWatcher() {}
};

class SubscriberPtrBase : public COMMON_NS::CDListItem<SubscriberPtrBase> {
public:
    SubscriberPtrWatcher* watcher() const { return watcher_; }
    void setWatcher(SubscriberPtrWatcher* watcher) { watcher_ = watcher; }

    // Processes the notification about resoure`s change
    void             notifyChanged() const
    {
        if (watcher_)
            watcher_->notifyChanged(this);
    }
    SubscriberPtrBase(SubscriberPtrWatcher* w = 0,
                      SubscriberPtrPublisher* res = 0);

protected:
#if defined(_MSC_VER) && (_MSC_VER > 1300)
    void init(SubscriberPtrWatcher* w, SubscriberPtrPublisher* res);
#endif
    SubscriberPtrWatcher* watcher_;
};

/*! Base for the Publisher class (the one that distributes event to
 *  it's subscribers).
 */
class SubscriberPtrPublisher {
public:
    void            registerSubscriber(SubscriberPtrBase* client)
    {
        clients_.push_back(client);
    }
    void            clearSubscribers() { clients_.clear(); }
    virtual ~SubscriberPtrPublisher() {}

protected:
    typedef COMMON_NS::CDList<SubscriberPtrBase> SubscriberList;

    // Notifies all the clients about changes
    void            notifyChanged() const
    {
        SubscriberList::iterator it = clients_.begin();
        for (; it != clients_.end(); ++it)
            it->notifyChanged();
    }
    SubscriberList& subscriberList() { return clients_; }
    bool            hasSubscribers() const { return !clients_.isEmpty(); }

private:
    SubscriberList clients_;
};

/*! SubscriberPtr subscribes to Publisher and controls it's lifetime
 *  with RefCntPtr.
 */
template <class T> class SubscriberPtr : public SubscriberPtrBase,
                                         public RefCntPtr<T> {
    typedef COMMON_NS::RefCntPtr<T> P;
public:
    SubscriberPtr()
        : SubscriberPtrBase(0, 0) {}
    SubscriberPtr(SubscriberPtrWatcher* watcher, T* ptr)
        : SubscriberPtrBase(watcher, ptr), P(ptr) {}
    SubscriberPtr<T>& operator=(T* ptr)
    {
        remove();
        P::operator=(ptr);
        if (!P::isNull())
            P::pointer()->registerSubscriber(this);
        return *this;
    }
    SubscriberPtr(const SubscriberPtr<T>& other)
        : SubscriberPtrBase(other.watcher(), other.pointer()),
          P(other.pointer()) {}
};

template <class T> class SubscriberPointer : public SubscriberPtrBase,
                                             public PtrBase<T> {
    typedef PtrBase<T> P;
public:
    SubscriberPointer()
        : SubscriberPtrBase(0, 0) {}
    SubscriberPointer(SubscriberPtrWatcher* watcher, T* ptr)
        : SubscriberPtrBase(watcher, ptr), P(ptr) {}
    SubscriberPointer<T>& operator=(T* ptr)
    {
        remove();
        P::p_ = ptr;
        if (!P::isNull())
            P::pointer()->registerSubscriber(this);
        return *this;
    }
    SubscriberPointer(const SubscriberPointer<T>& other)
        : SubscriberPtrBase(other.watcher(), other.pointer()),
          P(other.pointer()) {}
};

/////////////////////////////////////////////////////////////

class SubscriberWatcherProxy : public Common::SListItem<SubscriberWatcherProxy>,
                               public Common::SubscriberPtrBase {
public:
    SubscriberWatcherProxy(Common::SubscriberPtrWatcher* watcher,
                           Common::SubscriberPtrPublisher* publisher)
      : SubscriberPtrBase(watcher, publisher) {}
      USE_SUBALLOCATOR
};

class SubscriberWatcherProxyList :
    public Common::SList<SubscriberWatcherProxy> {};

/////////////////////////////////////////////////////////////

inline SubscriberPtrBase::SubscriberPtrBase(SubscriberPtrWatcher* w,
                                            SubscriberPtrPublisher* pres)
    : watcher_(w)
{
    if (pres)
        pres->registerSubscriber(this);
}

#if defined(_MSC_VER) && (_MSC_VER > 1300)
inline void SubscriberPtrBase::init(SubscriberPtrWatcher* w,
                                    SubscriberPtrPublisher* res)
{
    setWatcher(w);
    if (res)
        res->registerSubscriber(this);
}
#endif

COMMON_NS_END

#endif // SUBSCRIBER_PTR_H
