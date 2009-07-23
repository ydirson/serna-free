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
 #include "IdleHandlerImpl.h"
#include <iostream>

#include <QApplication>
#include <QTimer>

static const int HOLDOFF_TIMEOUT = 100;    // 0.1 sec

static IdleProcessManagerImpl* impl = 0;

IdleProcessManagerImpl::IdleProcessManagerImpl()
    : QObject(qApp),
      reprocessTimerActive_(false),
      needReprocessing_(false)
{
    holdoffTimer_   = new QTimer(this, NOTR("IdleHdlHoldoffTimer"));
    reprocessTimer_ = new QTimer(this, NOTR("IdleHdlReprocessTimer"));
    connect(holdoffTimer_, SIGNAL(timeout()), this, SLOT(holdoffTimerEvent()));
    connect(reprocessTimer_,
        SIGNAL(timeout()), this, SLOT(reprocessTimerEvent()));
}

void IdleProcessManagerImpl::holdoffTimerEvent()
{
#ifndef _WIN32
    if (qApp->hasPendingEvents()) {     // not a good time for processing
        holdoffTimer_->start(1, true);
        return;
    }
#endif // _WIN32
    holdoffTimer_->start(HOLDOFF_TIMEOUT, true);
    if (hlist_.begin() == hlist_.end())
        return;
    IdleHandlerListIter i = hlist_.begin();
    IdleHandlerListIter next;
    while (i != hlist_.end()) {
        if (i->needReprocessing_) {
            ++i;
            continue;             // will be re-processed separately
        }
        next = i;
        ++next;
        if (i->processQuanta()) { // will need re-processing
           if (!reprocessTimerActive_) {
               reprocessTimer_->start(1, true);
               reprocessTimerActive_ = true;
           }
           i->needReprocessing_ = true;
           needReprocessing_ = true;
       }
       i = next;
    }
}

void IdleProcessManagerImpl::reprocessTimerEvent()
{
    reprocessTimerActive_ = false;
    if (!needReprocessing_)
        return;
    needReprocessing_ = false;
    IdleHandlerListIter i = hlist_.begin();
    while (i != hlist_.end()) {
        IdleHandlerListIter next(i);
        ++next;
        if (i->processQuanta()) 
            needReprocessing_ = true;
        else
            i->needReprocessing_ = false;
        i = next;
    }
    if (needReprocessing_) {
        reprocessTimer_->start(1, true);
        reprocessTimerActive_ = true;
    }
}

static IdleProcessManagerImpl& instance()
{
    if (impl)
        return *impl;
    impl = new IdleProcessManagerImpl;
    return *impl;
}

void IdleHandler::registerHandler(IdleHandler* h)
{
    if (!h->isByItself())
        return;
    IdleProcessManagerImpl& i = instance();
    i.hlist_.push_back(h);
    if (!i.holdoffTimer_->isActive())
        i.holdoffTimer_->start(HOLDOFF_TIMEOUT, true);
}

void IdleHandler::deregisterHandler(IdleHandler* h)
{
    if (h->isByItself())
        return;
    IdleProcessManagerImpl& i = instance();
    h->remove();
    if (i.hlist_.isEmpty())
        i.holdoffTimer_->stop();
}

void IdleHandler::resetIdleTimer()
{
    IdleProcessManagerImpl& i = instance();
    if (!i.holdoffTimer_->isActive())
        i.holdoffTimer_->start(HOLDOFF_TIMEOUT, true);
}

/////////////////////////

IdleHandler::~IdleHandler()
{
    deregisterHandler(this);
}

///////////////////////////////////////////////////////////////////////

// WARNING: the following mess is because Qt sometimes sends timer
// event to the already dead object, so we cache timer QObjects for
// some time. Do not touch if you don't know what you're doing.

SingleShotTimer::SingleShotTimer(QObject* receiver, const char* m, 
                                 int interval)
    : funcp_(0), funcarg_(0)
{
    id_ = startTimer(interval);
    connect(this, SIGNAL(timeout()), receiver, m);
}

SingleShotTimer::SingleShotTimer(__delayed_sst_funcp funcp,
                                 void* funcarg, int interval)
    : funcp_(funcp), funcarg_(funcarg)
{
    id_ = startTimer(interval);    
}

SingleShotTimer::~SingleShotTimer() 
{
    stop();
}

bool SingleShotTimer::event(QEvent *e)
{
    if (id_ < 0 || e->type() != QEvent::Timer)
        return false;
    stop();
    if (funcp_) 
        (*funcp_)(funcarg_);
    else
        emit timeout();
    return true;
}

void SingleShotTimer::stop()
{
    if (id_ >= 0) {
        killTimer(id_);
        id_ = -1;
    }
}

static SS_List* sst_list = 0;
static int sst_count = 0;

static void qt_destroy_sst()
{
    if (sst_list)
        sst_list->destruct();
}

UTILS_EXPIMP void qt_clean_sst()
{
    if (0 == sst_list || !sst_list->first())
        return;
    SS_List::iterator it = sst_list->begin();
    for (; it != sst_list->end(); ++it)
        it->stop();
}

static void add_sst(SingleShotTimer* sst)
{
    if (!sst_list) {
        sst_list = new SS_List;
        qAddPostRoutine(qt_destroy_sst);
    }
    if (sst_count > 20)
        delete sst_list->first();
    else
        sst_count++;
    sst_list->push_back(sst);
}

UTILS_EXPIMP void qt_single_shot_timer(QObject* receiver, 
                                 const char* slot,
                                 int interval)
{
    add_sst(new SingleShotTimer(receiver, slot, interval));
}

UTILS_EXPIMP void sst_delayed_call(__delayed_sst_funcp funcp, void* arg)
{
    add_sst(new SingleShotTimer(funcp, arg));
}
