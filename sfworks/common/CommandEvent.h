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
#ifndef COMMAND_EVENT_H_
#define COMMAND_EVENT_H_

#include "common/common_defs.h"
#include "common/RefCounted.h"
#include "common/RefCntPtr.h"
#include "common/OwnerPtr.h"
#include "common/SernaApiBase.h"
#include "common/StringDecl.h"
#include "common/i18n_translate.h"
#include <typeinfo>

namespace Common {

/*  Design overview:
    1. Events are active, and they are executed in _context_
    2. Events can be executed only in their appropriate context. If the
       context is not appropriate, event will be forwarded to the
       default next context (if any), or discarded.
    3. EventData is a base class which is used to pass data to event
       maker functions and get optional data response from event execution.
    4. Event life-time:
       - event is hold via RefCntPtr by it's initial dispatcher
       - if after execution no one holds an event, it dies
    5. In future, in CommandEventContext we can fork/redirect events

    DynamicEventFactory is used in cases where we must create N events
    with the same initial data (e.g. preferences update sent to multiple
    consumers).

    Event implementation looks like:

    class OpenDocument : public CommandEvent<StructEditor> {
    public:
       virtual bool doExecute(StructEditor* context, EventData* result)
       {
           ...
       }
    If event is executed successfully, it's doExecute() must return TRUE.
*/

class PropertyNode;
class CommandEventBase;
class DynamicEventFactory;

/* Event data is used in two ways:
    - as an optional parameter to maker function at the time of event creation
    - as an optional return value
 */
class COMMON_EXPIMP EventData {
public:
    virtual ~EventData() {}
};

/*
 */
class COMMON_EXPIMP CommandEventContext {
public:
    typedef CommandEventContext CeContext;

    CommandEventContext();
    virtual ~CommandEventContext();

    //! Set next default executor in chain
    void        setNextContext(CeContext* next) { nextContext_ = next; }
    //! Get default next context
    CeContext*  getNextContext() const { return nextContext_; }
    //! Execute an event in the default next context
    void        dispatchToNext(CommandEventBase* event,
                               EventData* data = 0) const
    {
        if (nextContext_)
            nextContext_->executeEvent(event, data);
    }
    void        unsubscribeFromAll();

protected:
    virtual bool executeEvent(CommandEventBase* event, EventData*);

private:
    friend class DynamicEventFactory;
    friend class CommandEventBase;

    class FactorySet;
    typedef COMMON_NS::OwnerPtr<FactorySet> FactorySetOwner;

    CeContext*      nextContext_;
    FactorySetOwner factorySet_;
};

/*
 */
class COMMON_EXPIMP CommandEventBase : public COMMON_NS::RefCounted<> {
public:
    virtual ~CommandEventBase() {}

    //! Execute command event in context
    bool    execute(CommandEventContext* context, EventData* result = 0)
        {
            return context->executeEvent(this, result);
        }

protected:
    friend class    CommandEventContext;

    virtual bool    validEventContext(CommandEventContext*) const = 0;
    virtual bool    executeEvent(CommandEventContext* context,
                                 EventData* result) = 0;
};

typedef COMMON_NS::RefCntPtr<CommandEventBase> CommandEventPtr;

// Make event of type T, provided we have only it's forward declaration
template <class T> CommandEventPtr makeCommand(const EventData* = 0);

/*
 */
template <class ContextImpl>
class CommandEvent : public CommandEventBase {
public:
    virtual bool validEventContext(CommandEventContext* ctx) const 
    {
        return dynamic_cast<ContextImpl*>(ctx);
    }
protected:
    virtual bool    executeEvent(CommandEventContext* context,
                                 EventData* result)
        {
            return doExecute(static_cast<ContextImpl*>(context), result);
        }
    virtual bool    doExecute(ContextImpl*, EventData*) = 0;
};

typedef CommandEventPtr (*EventMakerPtr)(const EventData*);

class COMMON_EXPIMP DynamicEventFactory : public SernaApiBase {
public:
    DynamicEventFactory();
    virtual ~DynamicEventFactory();

    enum Predicate { And, Or };

    /// Create events and dispatch them to subscribers.
    /// Predicate defines the rule for return value of dispatchEvent().
    bool            dispatchEvent(const EventData* = 0,
                                  Predicate pred = And) const;

    void            subscribe(EventMakerPtr eventMakerFunc,
                              CommandEventContext* subscriber);
    void            unsubscribe(CommandEventContext* subscriber);

    bool            isSubscribed(const CommandEventContext* subscriber) const;

    bool            hasSubscribers() const;

protected:
    virtual void    doUnsubscribe(CommandEventContext*) {}

private:
    class SubscriberSet;
    COMMON_NS::OwnerPtr<SubscriberSet> subscriberSet_;
};

#if !defined(NO_CMD_MAKER_EXPORT)
# define CMD_MAKER_EXPORT COMMON_EXPORT
#else
# define CMD_MAKER_EXPORT
#endif

#define SIMPLE_COMMAND_EVENT_IMPL(EventName, ContextType) \
class EventName : public COMMON_NS::CommandEvent<ContextType> { \
public:                                                         \
    virtual bool doExecute(ContextType* context, COMMON_NS::EventData*); \
    DECLARE_I18N_TRANSLATE_FUNCTIONS(EventName) \
};                                                              \
COMMON_NS_BEGIN \
template<> CMD_MAKER_EXPORT CommandEventPtr \
makeCommand<EventName>(const EventData*) \
{                                        \
    return new EventName;                \
}                                        \
COMMON_NS_END \
COND_REGISTER_COMMAND_EVENT_MAKER(EventName, "-", "-")

#define COMMAND_EVENT_WITH_DATA_IMPL(EventName, ContextType) \
class EventName : public COMMON_NS::CommandEvent<ContextType> { \
public: \
    EventName(const COMMON_NS::EventData* ed) : ed_(ed) {} \
    virtual bool doExecute(ContextType* context, COMMON_NS::EventData*); \
    DECLARE_I18N_TRANSLATE_FUNCTIONS(EventName) \
private: \
    const COMMON_NS::EventData* ed_; \
};                                   \
COMMON_NS_BEGIN \
template<> CMD_MAKER_EXPORT CommandEventPtr      \
makeCommand<EventName>(const EventData* ed)   \
{                                             \
    return new EventName(ed);                 \
} \
COMMON_NS_END \
COND_REGISTER_COMMAND_EVENT_MAKER(EventName, "EventData", "EventData")

/////////////////////////////////////////////////////////////////////

class COMMON_EXPIMP CommandEventMakerRegistry {
public:
    class MakerInfo {
    public:
        MakerInfo(const char* name, EventMakerPtr maker,
                  const char* inClass, const char* outClass)
            : name_(name), maker_(maker),
              inClass_(inClass), outClass_(outClass)
        {
            CommandEventMakerRegistry::registerEventMaker(*this);
        }
        const char*     name_;
        EventMakerPtr   maker_;
        const char*     inClass_;
        const char*     outClass_;
    };
    static void  registerEventMaker(const MakerInfo&);
    static const MakerInfo* getMakerInfo(const char* name);
    static bool  execute(CommandEventContext*, const String& name);
    static bool  execute(CommandEventContext*, const String& name,
                         PropertyNode* in, PropertyNode* out);            
};

#define REGISTER_COMMAND_EVENT_MAKER(name, inClass, outClass) \
 static ::CommandEventMakerRegistry::MakerInfo CommandEvent_Maker_Info_##name \
        (#name, Common::makeCommand<name>, inClass, outClass);

#ifdef AUTO_EVENT_MAKER_REGISTRY
# define COND_REGISTER_COMMAND_EVENT_MAKER(name, inClass, outClass) \
    REGISTER_COMMAND_EVENT_MAKER(name, inClass, outClass)
#else
# define COND_REGISTER_COMMAND_EVENT_MAKER(name, inClass, outClass)
#endif // AUTO_EVENT_MAKER_REGISTRY

} // namespace Common

#endif // COMMAND_EVENT_H_

