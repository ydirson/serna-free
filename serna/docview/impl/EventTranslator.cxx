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
#include "ui/UiAction.h"

#include "docview/impl/debug_dv.h"
#include "docview/EventTranslator.h"
#include "common/CommandEvent.h"

#include <map>
#include <typeinfo>

USING_COMMON_NS;
using namespace Sui;

class EventTranslator::EventMap :
    public std::map<const Sui::Action*, EventMakerPtr> {};
class EventTranslator::ActionExecutorMap :
    public std::map<const Sui::Action*, ActionExecutor*> {};

EventTranslator::EventTranslator()
    : document_(0),
      eventMap_(new EventMap),
      actionExecutorMap_(new ActionExecutorMap)
{
}

void EventTranslator::registerEvent(const Sui::Action* action,
                                    EventMakerPtr eventMaker)
{
    DBG(DV.EVENT) << "EventTranslator: register " << action->get(NAME)
                  << " maker " << (void *)eventMaker << std::endl;
    (*eventMap_)[action] = eventMaker;
}

void EventTranslator::deregisterEvent(const Sui::Action* action)
{
    DBG(DV.EVENT)
        << "EventTranslator: deregister " << action->get(NAME)
        << std::endl;
    EventMap::iterator it = eventMap_->find(action);
    if (it == eventMap_->end())
        return;
    eventMap_->erase(it);
}

EventMakerPtr EventTranslator::findEventMaker(const Sui::Action* action) const
{
    EventMap::const_iterator it = eventMap_->find(action);
    if (eventMap_->end() == it) {
        DBG(DV.EVENT)
            << "EventTranslator<" << document_->get(NAME)
            << ">: [" << document_ << "] no registered UiAction: "
            << action->get(NAME) << std::endl;
        return 0;
    }
    return it->second;
}

void EventTranslator::dispatch(Sui::Action* action)
{
    ActionExecutorMap::const_iterator it = actionExecutorMap_->find(action);
    if (it != actionExecutorMap_->end()) {
        DBG(DV.EVENT)
            << "UiExecutor dispatch: " << action->get(Sui::NAME) << std::endl;
        document_->makeUiEventExecutor(it->second)->execute(document_);
        return;
    }
    EventMakerPtr event = findEventMaker(action);
    if (!event) 
        return;
    DBG(DV.EVENT) << "EventTranslator<" << document_->get(Sui::NAME) 
                  << ">: [" << document_ << "] UiAction dispatch: " 
                  << action->get(Sui::NAME) << std::endl;
    UiEventData ui_data(document_, action);
    (*event)(&ui_data)->execute(document_);
}

BuiltinUiActions& EventTranslator::uiActions() const
{
    return *builtinActions_;
}

void EventTranslator::setUiActions(BuiltinUiActions* actions)
{
    builtinActions_ = actions;
}

BuiltinActionGroups& EventTranslator::actionGroups() const
{
    return *builtinActionGroups_;
}

void EventTranslator::setActionGroups(BuiltinActionGroups* g)
{
    builtinActionGroups_ = g;
}

void EventTranslator::setDocument(SernaDoc* document)
{
    document_ = document;
}

EventTranslator::~EventTranslator()
{
    ActionExecutorMap::iterator it = actionExecutorMap_->begin();
    for (; it != actionExecutorMap_->end(); ++it)
        it->second->eventTranslator_ = 0;
}

void EventTranslator::registerActionExecutor(ActionExecutor* executor)
{
    DBG(DV.EVENT) << "EventTranslator: register "  << executor << "/"
                  << executor->uiAction_->get(NAME)
                  << " action-executor " << (void *)executor << std::endl;
    (*actionExecutorMap_)[executor->uiAction()] = executor;
    executor->eventTranslator_ = this;
}

void EventTranslator::deregisterActionExecutor(const ActionExecutor* executor)
{
    DBG(DV.EVENT) << "EventTranslator: deregister action executor "
                  << executor << std::endl;
    ActionExecutorMap::iterator it =
        actionExecutorMap_->find(executor->uiAction());
    if (it == actionExecutorMap_->end())
        return;
    actionExecutorMap_->erase(it);
}

ActionExecutor::~ActionExecutor()
{
    if (eventTranslator_)
        eventTranslator_->deregisterActionExecutor(this);
}
