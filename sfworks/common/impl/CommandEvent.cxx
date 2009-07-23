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
#include "common/CommandEvent.h"
#include "common/Singleton.h"
#include "common/String.h"
#include "common/PropertyTree.h"
#include "common/PropertyTreeEventData.h"
#include <map>
#include <set>
#include <string>

COMMON_NS_BEGIN

class CommandEventContext::FactorySet :
    public std::set<DynamicEventFactory*> {
};

CommandEventContext::CommandEventContext()
    : nextContext_(0),
      factorySet_(new FactorySet)
{
}

bool CommandEventContext::executeEvent(CommandEventBase* event,
                                       EventData* result)
{
    if (event->validEventContext(this))
        return event->executeEvent(this, result);
    if (nextContext_)
        return nextContext_->executeEvent(event, result);
    return false;
}

void CommandEventContext::unsubscribeFromAll()
{
    while (!factorySet_->empty())
        (*factorySet_->begin())->unsubscribe(this);
}

CommandEventContext::~CommandEventContext()
{
    unsubscribeFromAll();
}

//////////////////////////////////////////////////////////////////

class DynamicEventFactory::SubscriberSet :
    public std::multimap<CommandEventContext*, EventMakerPtr> {
};

DynamicEventFactory::DynamicEventFactory()
    : subscriberSet_(new SubscriberSet)
{
}

bool DynamicEventFactory::dispatchEvent(const EventData* data,
                                        Predicate pred) const
{
    bool rvalue = (pred == And);
    SubscriberSet::const_iterator it = subscriberSet_->begin();
    for (; it != subscriberSet_->end(); ++it) {
        CommandEventPtr event = (*it->second)(data);
        bool v = event->execute(it->first, 0);
        if (pred == And)
            rvalue &= v;
        else
            rvalue |= v;
    }
    return rvalue;
}

void DynamicEventFactory::subscribe(EventMakerPtr eventMakerFunc,
                                    CommandEventContext* subscriber)
{
    subscriberSet_->insert
        (SubscriberSet::value_type(subscriber, eventMakerFunc));
    subscriber->factorySet_->insert(this);
}

void DynamicEventFactory::unsubscribe(CommandEventContext* subscriber)
{
    subscriberSet_->erase(subscriber);
    subscriber->factorySet_->erase(this);
    doUnsubscribe(subscriber);
}

bool DynamicEventFactory::isSubscribed(const CommandEventContext* ctx) const
{
    return subscriberSet_->find(const_cast<CommandEventContext*>(ctx)) !=
        subscriberSet_->end();
}

bool DynamicEventFactory::hasSubscribers() const
{
    return subscriberSet_->begin() != subscriberSet_->end();
}

DynamicEventFactory::~DynamicEventFactory()
{
    SubscriberSet::const_iterator it = subscriberSet_->begin();
    for (; it != subscriberSet_->end(); ++it)
        it->first->factorySet_->erase(this);
}

//////////////////////////////////////////////////////////////////

typedef std::map<std::string, CommandEventMakerRegistry::MakerInfo>
    EventMakerInfoMap;

typedef SingletonHolder<EventMakerInfoMap> EventMakerInfoMapHolder;

void CommandEventMakerRegistry::registerEventMaker(const MakerInfo& mi)
{
    EventMakerInfoMapHolder::instance().insert
        (EventMakerInfoMap::value_type(mi.name_, mi));
}

const CommandEventMakerRegistry::MakerInfo*
CommandEventMakerRegistry::getMakerInfo(const char* name)
{
    EventMakerInfoMap& m = EventMakerInfoMapHolder::instance();
    EventMakerInfoMap::const_iterator it = m.find(name);
    if (it == m.end())
        return 0;
    return &(it->second);
}

bool CommandEventMakerRegistry::execute(CommandEventContext* context,
                                        const String& name)
{
    const MakerInfo* mi = getMakerInfo(name.utf8().c_str());
    if (0 == mi || strcmp(mi->inClass_, "-") || strcmp(mi->outClass_, "-"))
        return false;
    return (mi->maker_)(0)->execute(context);
}

bool CommandEventMakerRegistry::execute(CommandEventContext* context,
                                         const String& name,
                                         PropertyNode* in,
                                         PropertyNode* out)
{
    static const char ptn_class[] = NOTR("PropertyTree");
    const MakerInfo* mi = getMakerInfo(name.utf8().c_str());
    if (0 == mi)
        return false;
    if (in) {
        if (strcmp(mi->inClass_, ptn_class))
            return false;
    } else {
        if (strcmp(mi->inClass_, "-"))
            return false;
    }
    if (out) {
        if (strcmp(mi->outClass_, ptn_class))
            return false;
    } else {
        if (strcmp(mi->outClass_, "-"))
            return false;
    }
    PropertyTreeEventData data_in(in);
    PropertyTreeEventData data_out(out);
    return (mi->maker_)(&data_in)->execute(context, &data_out);
}

COMMON_NS_END
