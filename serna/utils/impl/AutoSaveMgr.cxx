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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#include "utils/AutoSaveMgr.h"
#include "utils/IdleHandler.h"
#include "utils/utils_debug.h"

#include "common/CommandEvent.h"
#include "common/Singleton.h"

#include <QObject>
#include <QApplication>
#include <list>
#include <iostream>

#ifdef DBG_DEFAULT_TAG
# undef DBG_DEFAULT_TAG
#endif
#define DBG_DEFAULT_TAG UTILS.TIMERS

class QTimerEvent;

using namespace Common;

AutoSaver::AutoSaver() {}
AutoSaver::~AutoSaver() {}

AutoSaveMgr::AutoSaveMgr() {}
AutoSaveMgr::~AutoSaveMgr() {}

struct SubscriberInfo {
    SubscriberInfo(EventMakerPtr eventMakerFunc,
                   CommandEventContext* subscriber)
     :  eventMakerFunc_(eventMakerFunc), subscriber_(subscriber) {}
    EventMakerPtr           eventMakerFunc_;
    CommandEventContext*    subscriber_;
};

static bool operator==(const SubscriberInfo& lhs, const SubscriberInfo& rhs)
{
    return lhs.eventMakerFunc_ == rhs.eventMakerFunc_ &&
           lhs.subscriber_ == rhs.subscriber_;
}

class AutoSaveMgrImpl : public AutoSaveMgr,
                        private IdleHandler,
                        private QObject {
public:
    AutoSaveMgrImpl();
    ~AutoSaveMgrImpl() {}

    virtual bool processQuanta();
    virtual void timerEvent(QTimerEvent*);

    virtual void subscribe(EventMakerPtr eventMakerFunc,
                           CommandEventContext* subscriber);
    virtual void unsubscribe(Common::CommandEventContext* subscriber);

private:
    void stop();

    std::list<SubscriberInfo>   subscriberList_;
    int                         timerId_;
    static int const            timeout_ = 5000; // milliseconds
};

void AutoSaveMgrImpl::subscribe(EventMakerPtr eventMakerFunc,
                                CommandEventContext* subscriber)
{
    SubscriberInfo sub(eventMakerFunc, subscriber);
    std::list<SubscriberInfo>::iterator it(subscriberList_.begin());
    it = std::find(it, subscriberList_.end(), sub);
    if (subscriberList_.end() == it)
        subscriberList_.push_back(sub);
}

void AutoSaveMgrImpl::unsubscribe(CommandEventContext* subscriber)
{
    std::list<SubscriberInfo>::iterator it(subscriberList_.begin());
    for (; subscriberList_.end() != it; ++it) {
        if (it->subscriber_ == subscriber)
            subscriberList_.erase(it++);
    }
}

bool AutoSaveMgrImpl::processQuanta()
{
    if (!subscriberList_.empty() && -1 == timerId_) {
        timerId_ = startTimer(timeout_);
        DDBG << "AutoSaveMgrImpl started timer " << timerId_ << std::endl;
    }
    return false;
}

void AutoSaveMgrImpl::stop()
{
    if (-1 != timerId_) {
        killTimer(timerId_);
        timerId_ = -1;
    }
}

void AutoSaveMgrImpl::timerEvent(QTimerEvent*)
{
    DDBG << "AutoSaveMgrImpl::timerEvent for " << this << std::endl;

    while (!subscriberList_.empty() && !qApp->hasPendingEvents()) {
        SubscriberInfo& si = subscriberList_.front();
        CommandEventPtr event = (*si.eventMakerFunc_)(0);
        (void) event->execute(si.subscriber_, 0);
        DDBG << "AutoSaveMgrImpl popped " << &si << " from queue" << std::endl;
        subscriberList_.pop_front();
    }
    stop();
}

AutoSaveMgrImpl::AutoSaveMgrImpl()
 :  timerId_(-1)
{
    IdleHandler::registerHandler(this);
}

AutoSaveMgr& AutoSaveMgr::instance()
{
    return SingletonHolder<AutoSaveMgrImpl, CreateUsingNew<AutoSaveMgrImpl>,
                           NoDestroy<AutoSaveMgrImpl> >::instance();
}

