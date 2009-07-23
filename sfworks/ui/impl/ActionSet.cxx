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
#include "ui/ui_defs.h"
#include "ui/ActionSet.h"
#include "ui/UiProps.h"
#include "ui/impl/ui_debug.h"

#include "common/StringTokenizer.h"
#include "common/PropertyTree.h"

#include <QKeySequence>

using namespace Common;

namespace Sui {

ActionSet::ActionSet()
{
}

ActionSet::~ActionSet()
{
}

static ActionSet::ActionPtrMap::iterator 
accel_for_action(ActionSet::ActionPtrMap& accelMap, Action* action)
{
    ActionSet::ActionPtrMap::iterator i;
    for (i = accelMap.begin(); i != accelMap.end(); i++) {
        if ((*i).second.pointer() == action) 
            return i;
    }   
    return accelMap.end();
}

void ActionSet::checkAccel(Action* action)
{
    //! Check accel for duplicates
    PropertyNode* accel = action->properties()->getProperty(ACCEL);
    if (accel) {
        if (!isRegisteredIn(accel))
            accel->addWatcher(this);
        QKeySequence key_seq(accel->getString());
        if (!key_seq.isEmpty()) {
            String key_str = key_seq.operator::QString();
            if (accelMap_.end() != accelMap_.find(key_str)) {
                DBG(UI.TEST) << action->get(NAME) 
                             << " accel exists: " << key_str << std::endl;
                accel->setString(String());
            }
            else 
                accelMap_[key_str] = action;
        }
    }
}

void ActionSet::propertyChanged(PropertyNode* prop)
{
    static bool lock = false;   
    if (lock)
        return;
    lock = true;
    if (prop->name() != ACCEL)
        return;
    Action* action = 0;
    for (ActionPtrMap::const_iterator i = actionMap_.begin();
         i != actionMap_.end(); i++) {
        if ((*i).second->properties()->getProperty(prop->name()) == prop) {
            action = (*i).second.pointer();
            break;
        }
    }
    DBG(UI.TEST) << "Accel changed: " << action->get(NAME) 
                 << prop->getString() << std::endl;
    ActionPtrMap::iterator ai = accel_for_action(accelMap_, action);
    if (ai != accelMap_.end()) {
        DBG(UI.TEST) << "Accel removed: " << (*ai).first << std::endl;
        accelMap_.erase(ai);
    }
    checkAccel(action);
    lock = false;
}

Action* ActionSet::makeAction(const PropertyNode* actionProps)
{
    PropertyNodePtr action_props = actionProps->copy(true);
    action_props->setName(ACTION);
    String name = action_props->getSafeProperty(NAME)->getString();
    Action* action = find_action(name);
    if (action)
        return action;
    //! Make action
    action = Action::make(action_props.pointer());
    actionMap_[name] = action;
    checkAccel(action);
    return action;
}

Action* ActionSet::makeAction(const String& name,
                              const String& inscription,
                              const String& icon,
                              const String& toolTip,
                              const String& whatsThis,
                              const String& accel,
                              bool isTogglable)
{
    Action* action = find_action(name);
    if (action)
        return action;
    PropertyNode* prop = new PropertyNode(ACTION);
    prop->appendChild(new PropertyNode(NAME, name));
    prop->appendChild(new PropertyNode(INSCRIPTION, inscription));
    prop->appendChild(new PropertyNode(ICON, icon));
    prop->appendChild(new PropertyNode(TOOLTIP, toolTip));
    prop->appendChild(new PropertyNode(WHATS_THIS, whatsThis));
    prop->appendChild(new PropertyNode(ACCEL, accel));
    prop->appendChild(new PropertyNode(IS_TOGGLEABLE, isTogglable));

    action = Action::make(prop);
    
    //action = new Action(name, inscription, icon, toolTip,
    //                    whatsThis, accel, isTogglable);
    actionMap_[name] = action;
    checkAccel(action);
    return action;
}

Action* ActionSet::find_action(const String& name) const
{
    ActionPtrMap::const_iterator i = actionMap_.find(name);
    if (i != actionMap_.end())
        return (*i).second.pointer();
    return 0;
}

Action* ActionSet::findAction(const ActionPred& pred) const
{
    for (ActionPtrMap::const_iterator i = actionMap_.begin();
         i != actionMap_.end(); i++)
        if (pred((*i).second.pointer()))
            return (*i).second.pointer();
    return 0;
}

Action* ActionSet::findAction(const String& actionName) const
{
    if (-1 == actionName.find('/')) 
        return find_action(actionName);
    Action* action = 0;
    for (StringTokenizer token(actionName, "/"); token; ) {
        String name = token.next();
        if (name.isEmpty())
            return 0;
        if (!action) {
            action = find_action(name);
            if (!action)
                return 0;
        }
        else
            action = action->getSubAction(name);
    }
    return action;
}

void ActionSet::remapAction(const String& oldName, Action* action)
{
    String new_name = action->get(NAME);
    if (oldName != new_name) {
        ActionPtr action_ptr = action;
        ActionPtrMap::iterator i = actionMap_.find(oldName);
        if (i != actionMap_.end() && (*i).second.pointer() == action)
            actionMap_.erase(i);
        actionMap_[new_name] = action;
    }
}

void ActionSet::removeAction(Action* action)
{
    //! Remove hotkey map item
    ActionPtrMap::iterator i = accel_for_action(accelMap_, action);
    if (i != accelMap_.end()) {
        //std::cerr << "Accel removed: " << (*i).first << std::endl;
        accelMap_.erase(i);
    }
    i = actionMap_.find(action->get(NAME));
    if (i != actionMap_.end() && (*i).second.pointer() == action) {
        (*i).second->removeAction();
        actionMap_.erase(i);
    }
}

} // namespace Sui
