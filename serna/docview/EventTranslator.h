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
#ifndef _CORE_EVENT_TRANSLATOR_
#define _CORE_EVENT_TRANSLATOR_

#include "common/CommandEvent.h"
#include "common/OwnerPtr.h"

#include "docview/SernaDoc.h"
#include "docview/BuiltinUiCommands.h"

typedef Common::CommandEventContext  CmdEventContext;

class EventTranslator;

/* Translator from UiActions to CommandEvents.
 */
class DOCVIEW_EXPIMP EventTranslator : public Sui::ActionDispatcher {
public:
    EventTranslator();
    virtual ~EventTranslator();

    void                    registerEvent(const Sui::Action* action,
                                          Common::EventMakerPtr maker);
    void                    deregisterEvent(const Sui::Action* action);
    Common::EventMakerPtr   findEventMaker(const Sui::Action* action) const;

    void                    registerActionExecutor(ActionExecutor*);
    void                    deregisterActionExecutor(const ActionExecutor*);

    // Access to built-in commands
    BuiltinUiActions&       uiActions() const;
    void                    setUiActions(BuiltinUiActions*);

    BuiltinActionGroups&    actionGroups() const;
    void                    setActionGroups(BuiltinActionGroups*);

    void                    setDocument(SernaDoc* document);

private:
    class EventMap;
    class ActionExecutorMap;
    virtual void            dispatch(Sui::Action* cmd);

private:
    SernaDoc*                               document_;
    Common::OwnerPtr<EventMap>              eventMap_;
    Common::OwnerPtr<ActionExecutorMap>     actionExecutorMap_;
    Common::OwnerPtr<BuiltinUiActions>      builtinActions_;
    Common::OwnerPtr<BuiltinActionGroups>   builtinActionGroups_;
};

/*
 */
class DOCVIEW_EXPIMP UiEventData : public Common::EventData {
public:
    UiEventData(SernaDoc* document, Sui::Action* action)
        : document_(document),
          action_(action) {}

    SernaDoc*   document_;
    Sui::Action* action_;
};

#define UICMD_EVENT_IMPL(eventName, contextType) \
class COMMON_EXPORT eventName : public Common::CommandEvent<contextType> { \
public:                                                            \
    eventName(const Common::EventData* ed)                         \
        : ed_(dynamic_cast<const UiEventData*>(ed)) {}             \
    virtual bool doExecute(contextType*, COMMON_NS::EventData*);   \
    const UiEventData* uiEventData() const { return ed_; }         \
    Sui::Action* action() const { return ed_ ? ed_->action_ : 0; } \
    Sui::Action* activeSubAction() const                           \
        { return action() ? action()->activeSubAction() : 0; }     \
private:                                                           \
    const UiEventData*    ed_;                                     \
};                                                                 \
namespace Common {                                                 \
template<> COMMON_EXPORT Common::CommandEventPtr                   \
makeCommand<eventName>(const Common::EventData* ed)                \
{                                                                  \
    return new eventName(ed);                                      \
} \
} // namespace Common

#endif // _CORE_EVENT_TRANSLATOR_

