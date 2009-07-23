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

#include "structeditor/impl/StructAutoSave.h"
#include "structeditor/StructEditor.h"
#include "structeditor/impl/debug_se.h"

#include "docview/SernaDoc.h"

#include "utils/TimedEventGenerator.h"
#include "utils/AutoSaveMgr.h"
#include "utils/DocSrcInfo.h"
#include "utils/Properties.h"
#include "utils/Config.h"
#include "utils/struct_autosave_utils.h"

#include "grove/Grove.h"
#include "common/PropertyTreeEventFactory.h"

class SaveStructDocument;

using namespace Common;
using namespace AutoSaveUtils;

class StructAutoSaver : public AutoSaver, public CommandEventContext {
public:
    StructAutoSaver(StructEditor* se);
    ~StructAutoSaver();
    virtual void notifyUpdated();
    virtual void setTimeout(int timeout);
    void idleTimerExpired();
    void delayTimerExpired();
private:
    void waitForIdle(bool wait);
    void resetDelayTimer(unsigned timeout);
    TimedEventGenerator& get_timer();
    StructEditor*                           se_;
    String                                  topSysid_;
    Common::OwnerPtr<TimedEventGenerator>   timerGen_;
    int                                     timeout_;
    bool                                    isWaitingForDelay_;
    bool                                    isWaitingForIdle_;
};

SIMPLE_COMMAND_EVENT_IMPL(AutoSaveIdleExpiredEvent, StructAutoSaver)

bool AutoSaveIdleExpiredEvent::doExecute(StructAutoSaver* autoSaver, EventData*)
{
    autoSaver->idleTimerExpired();
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(AutoSaveDelayExpiredEvent, StructAutoSaver)

bool AutoSaveDelayExpiredEvent::doExecute(StructAutoSaver* autoSaver,
                                          EventData*)
{
    autoSaver->delayTimerExpired();
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(StructEditorHistoryChangedEvent, StructAutoSaver)

bool StructEditorHistoryChangedEvent::doExecute(StructAutoSaver* autoSaver,
                                                EventData*)
{
    autoSaver->notifyUpdated();
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(AutoSaveDelayChangedEvent, StructAutoSaver)

static String getASPropName(const char* pname)
{
    String propName(App::APP);
    propName.append(1, '/').append(pname);
    return propName;
}

static int getAutosaveTimeout()
{
    const PropertyNode* pn;
    pn = config().getProperty(getASPropName(App::AUTOSAVE_ENABLED));
    if (!pn->getBool())
        return 0;
    pn = config().getProperty(getASPropName(App::AUTOSAVE_DELAY));
    return pn->getInt() * 60 * 1000;
}

bool AutoSaveDelayChangedEvent::doExecute(StructAutoSaver* autoSaver,
                                          EventData*)
{
    autoSaver->setTimeout(getAutosaveTimeout());
    return true;
}

StructAutoSaver::StructAutoSaver(StructEditor* se)
 :  se_(se),
    timeout_(getAutosaveTimeout()),
    isWaitingForDelay_(false), isWaitingForIdle_(false)
{
    PropertyNode* dsi = se_->sernaDoc()->getDsi();
    topSysid_ = dsi->getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
    restore_auto_save(topSysid_);
    clean_auto_save(topSysid_);

    config().eventFactory().subscribe(getASPropName("autosave/*"),
                                      makeCommand<AutoSaveDelayChangedEvent>,
                                      this);
    se->historyChange().subscribe(makeCommand<StructEditorHistoryChangedEvent>,
                                  this);
}

StructAutoSaver::~StructAutoSaver()
{
    DBG(SE.TRACE) << "~StructAutoSaver, this = " << abr(this) << std::endl;
    resetDelayTimer(0);
    if (0 != timerGen_.get())
        timerGen_->unsubscribe(this);
    waitForIdle(false);
}

TimedEventGenerator& StructAutoSaver::get_timer()
{
    if (0 == timerGen_.get()) {
        timerGen_.reset(TimedEventGenerator::make());
        timerGen_->subscribe(makeCommand<AutoSaveDelayExpiredEvent>, this);
    }
    return *timerGen_;
}

void StructAutoSaver::idleTimerExpired()
{
    DBG(SE.TRACE) << "AutoSaving..." << abr(this) << std::endl;

    String stage(from_string<QString>(tr("Autosaving...")));
    se_->sernaDoc()->showStageInfo(stage);
    make_auto_save(se_->grove(), se_->getDsi());
    se_->sernaDoc()->showStageInfo();

    isWaitingForIdle_ = false;
//  When idle timer expires autosave mgr removes subscribers from its queue,
//  so no need to call waitForIdle(false);
}

void StructAutoSaver::delayTimerExpired()
{
    DBG(SE.TRACE) << "delay timer expired" << abr(this) << std::endl;
    resetDelayTimer(0);
    waitForIdle(true);
}

void StructAutoSaver::setTimeout(int timeout)
{
    DBG(SE.TRACE) << "set timeout " << timeout << " for " << this << std::endl;
    timeout_ = timeout;
    if (isWaitingForDelay_)
        resetDelayTimer(timeout);
}

void StructAutoSaver::waitForIdle(bool wait)
{
    if (wait) {
        if (!isWaitingForIdle_) {
            AutoSaveMgr& asMgr(AutoSaveMgr::instance());
            asMgr.subscribe(makeCommand<AutoSaveIdleExpiredEvent>, this);
            isWaitingForIdle_ = true;
            DBG(SE.TRACE) << "waiting for idle tick " << this << std::endl;
        }
    }
    else {
        if (isWaitingForIdle_) {
            AutoSaveMgr& asMgr(AutoSaveMgr::instance());
            asMgr.unsubscribe(this);
            isWaitingForIdle_ = false;
            DBG(SE.TRACE) << "stop waiting for idle tick " << this << std::endl;
        }
    }
}

void StructAutoSaver::resetDelayTimer(unsigned timeout)
{
    DBG(SE.TRACE) << "Reset delay for " << abr(this) << ", timeout = "
                  << timeout << std::endl;
    if (isWaitingForDelay_) {
        if (0 == timeout) {
            if (0 != timerGen_.get())
                timerGen_->reset(timeout);
            isWaitingForDelay_ = false;
        }
    }
    else {
        if (0 != timeout) {
            if (0 == timerGen_.get()) {
                timerGen_.reset(TimedEventGenerator::make());
                timerGen_->subscribe(makeCommand<AutoSaveDelayExpiredEvent>,
                                     this);
            }
            timerGen_->reset(timeout);
            isWaitingForDelay_ = true;
        }
    }
}

void StructAutoSaver::notifyUpdated()
{
    DBG(SE.TRACE) << "StructAutoSaver::notifyUpdated() " << this << std::endl;
    if (!se_->isModified()) {
        resetDelayTimer(0);
        waitForIdle(false);

        String stage(from_string<QString>(tr("Cleaning autosave...")));
        se_->sernaDoc()->showStageInfo(stage);
        clean_auto_save(topSysid_);
        AutoSaveUtils::Sysids().remove(topSysid_);
        se_->sernaDoc()->showStageInfo();

        PropertyNode* dsi = se_->sernaDoc()->getDsi();
        DBG(SE.TRACE) << "StructAutoSaver::notifyUpdated(), old sysid: "
                      << topSysid_ << ", new sysid: "
                      << dsi->getSafeProperty(DocSrcInfo::DOC_PATH)->getString()
                      << std::endl;
        topSysid_ = dsi->getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
    }
    else
        resetDelayTimer(timeout_);
}

AutoSaver* make_struct_autosaver(StructEditor* se)
{
    return new StructAutoSaver(se);
}

