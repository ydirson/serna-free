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
#include "common/PropertyTree.h"
#include "common/ScopeGuard.h"

#include "ui/ui_defs.h"
#include "ui/UiProps.h"
#include "ui/UiItem.h"
#include "ui/UiItemSearch.h"
#include "ui/UiAction.h"
#include "ui/UiDocument.h"
#include "ui/prop_locker.h"
#include "ui/impl/ui_debug.h"

#include <QAction>
#include <QPointer>

using namespace Common;

namespace Sui {

#define NOTIFY_ACTION_WATCHERS(act, func) \
{   ActionImpl* action = static_cast<ActionImpl*>(act); \
    if (!action->watchers_.isNull()) { \
        ActionWatcher** watcher = action->watchers_.list(); \
        for (; *watcher; ++watcher) \
            if (PackedPointerSet<ActionWatcher>::isValid(*watcher)) \
                (*watcher)->func ; \
    } \
}

//////////////////////////////////////////////////////////////////////////

class ActionImpl : public Action,
                   protected Common::PropertyNodeWatcher,
                   public PropertyLocker {
public:
    ActionImpl(PropertyNode*);

    virtual ItemEntryList&  items() const { return itemList_; }
    virtual void            addWatcher(ActionWatcher* watcher);
    virtual void            removeWatcher(ActionWatcher* watcher);
    virtual void            removeAllWatchers();
    virtual void            propertyChanged(PropertyNode* property);
    virtual ~ActionImpl();

private:
    friend class Action;
    void                    notifyPropertyChanged(PropertyNode* prop);
    virtual QAction*        qAction() const;
    virtual void            setQAction(QAction*);

    mutable ItemEntryList                   itemList_;
    Common::PackedPointerSet<ActionWatcher> watchers_;
    QPointer<QAction>                       qAction_;
};

Action::Action(PropertyNode* props)
    : PropertyTreeHolder(props)
{
}

ActionImpl::ActionImpl(PropertyNode* props)
    : Action(props)
{
    props->makeDescendant(IS_ENABLED, "false", false);
    props->makeDescendant(IS_TOGGLEABLE, "false", false);
    props->makeDescendant(IS_TOGGLED, "false", false);
    props->makeDescendant(IS_VISIBLE, "true", false);

    //! NOTE: The root action subscribes to the whole prop-tree, while
    //        subactions - only to their sub-trees
    const bool is_toggleable = getBool(IS_TOGGLEABLE);
    int idx(0), current_act(getInt(CURRENT_ACTION));
    for (PropertyNode* prop = props->firstChild(); prop;
         prop = prop->nextSibling()) {
        if (ACTION == prop->name()) {
            if (is_toggleable) {
                prop->makeDescendant(IS_TOGGLEABLE)->setBool(true);
                prop->makeDescendant(IS_TOGGLED)->setBool(current_act == idx);
            }
            appendChild(Action::make(prop));
            ++idx;
        }
    }
    installSyncher(props);
    props->addWatcher(this);
}

QAction* ActionImpl::qAction() const 
{
    return qAction_;
}

void ActionImpl::setQAction(QAction* qact)
{
    qAction_ = qact;
}

Action::~Action()
{
}

ActionImpl::~ActionImpl()
{
    DBG(UI.CMD) << "~UiAction: " << get(NAME) << std::endl;
    removeAllWatchers();
}

Action* Action::make(PropertyNode* pn)
{
    return new ActionImpl(pn);
}

Action* Action::activeSubAction() const
{
    const Action* action = parent() ? parent() : this;
    return action->getChild(action->getInt(CURRENT_ACTION));
}

Action* Action::getSubAction(const String& name) const
{
    Action* subact = firstChild();
    for (; subact; subact = subact->nextSibling())
        if (subact->get(NAME) == name)
            return subact;
    return 0;
}

bool Action::setActiveSubAction(const String& name)
{
    if (name.isEmpty()) 
        return parent()->setActiveSubAction(get(NAME));
    int i = 0;
    Action* subact = firstChild();
    for (; subact; subact = subact->nextSibling()) {
        if (subact->get(NAME) == name) {
            if (getInt(CURRENT_ACTION) == i)
                return true;    // already set to correct value
            setInt(CURRENT_ACTION, i);
            dispatch();
            return true;
        }
        ++i;
    }
    return false;
}

////////////

void Action::itemAdded(Item* item)
{
    setupSynchers(item);
}

////////////

void ActionImpl::notifyPropertyChanged(PropertyNode* prop)
{
    ItemEntry* i = 0;
    for (i = items().firstChild(); i; i = i->nextSibling())
        i->asItem()->actionPropChanged(prop, 0);
    if (Action::parent()) {
        i = Action::parent()->items().firstChild(); 
        for (; i; i = i->nextSibling())
            i->asItem()->actionPropChanged(prop, this);
    }
}

void ActionImpl::propertyChanged(PropertyNode* prop)
{
    UI_LOCK_PROP_GUARD_CHECKED
    if (prop->parent()) {
        DBG(UI.PROP) << "UiAction<" << get(NAME) << "> prop-parent=<"
                     << prop->parent()->getSafeProperty(NAME)->getString()
                     << "> property changed: " << prop->name() << " <"
                     << prop->getString() << ">" << std::endl;
    }
    else {
        DBG(UI.PROP) << "UiAction<" << get(NAME) << "> property changed: "
                     << prop->name() << " <" << prop->getString() << ">"
                     << std::endl;
    }
    const bool is_curract = CURRENT_ACTION == prop->name();
    if (is_curract && Action::parent())
        return;
    if (is_curract && getBool(IS_TOGGLEABLE)) {
        Action* curr_action = getChild(prop->getInt());
        if (curr_action) {
            for (Action* a = firstChild(); a; a = a->nextSibling()) {
                PropertyNode* is_toggled = a->property(IS_TOGGLED);
                if (!is_toggled)
                    continue;
                const bool old_toggled = is_toggled->getBool();
                const bool new_toggled = (a == curr_action);
                if (old_toggled != new_toggled) {
                    DBG(UI.PROP) << "force change toggle state of "
                        << a->get(NAME) << " to " << new_toggled << std::endl;
                    is_toggled->setBool(new_toggled);
                    static_cast<ActionImpl*>(a)->notifyPropertyChanged(
                        is_toggled);
                }
            }
        }
        notifyPropertyChanged(prop);
        return;
    }
    // track subact->setBool(true)
    if (Action::parent() && Action::parent()->getBool(IS_TOGGLEABLE) &&
        IS_TOGGLED == prop->name() && prop->getBool()) {
        Action* a = prevSibling();
        for (; a; a = a->prevSibling())
            a->setBool(IS_TOGGLED, false);
        for (a = nextSibling(); a; a = a->nextSibling())
            a->setBool(IS_TOGGLED, false);
    }
    notifyPropertyChanged(prop);
}

void Action::setEnabled(bool enable)
{
    properties()->makeDescendant(IS_ENABLED)->setBool(enable);
    for (Action* child = firstChild(); child; child = child->nextSibling())
        child->setEnabled(enable);
}

void Action::setToggled(bool isOn)
{
    properties()->makeDescendant(IS_TOGGLED)->setBool(isOn);
}

void Action::removeItem(Item* item)
{
    ItemEntry* i = items().firstChild();
    while (i && item != i->asItem())
        i = i->nextSibling();
    if (i)
        i->remove();
}

void Action::removeAction()
{
    removeItems();
    remove();
}

void Action::removeItems()
{
    DBG(UI.CMD) << "UiAction " << get(NAME) << " removeItems " << std::endl;
    ItemEntry* ie = items().firstChild();
    for (; ie; ie = items().firstChild()) {
        ie->asItem()->removeItem();
        // if still in action items list (still alive), remove
        if (ie == items().firstChild())
            items().firstChild()->remove();
    }
}

void Action::dispatch() const
{
    ItemEntry* item_entry = items().firstChild();
    Item* dispatch_item = 0;
    for (; item_entry; item_entry = item_entry->ItemEntry::nextSibling()) {
        dispatch_item = item_entry->asItem();
        if (dispatch_item)
            break;
        dispatch_item = 0;
    }
    if (dispatch_item) {
        DBG(UI.DYN) << "Action::dispatch to " 
            << dispatch_item->get(NAME) << std::endl;
        dispatch_item->dispatch(const_cast<Action*>(this));
    }
}

String Action::translationContext() const
{
    const PropertyNode* context_prop = 
        properties()->getProperty(TRANSLATION_CONTEXT);
    if (context_prop)
        return context_prop->getString();
    if (Action::parent())
        context_prop = Action::parent()->
            properties()->getProperty(TRANSLATION_CONTEXT);
    return context_prop ? context_prop->getString() : String();
}


/////////////////////////////////////////////////////////////////////////

void ActionImpl::addWatcher(ActionWatcher* watcher)
{
    watchers_.add(watcher);
    watcher->actions_.insert(this);
}

void ActionImpl::removeWatcher(ActionWatcher* watcher)
{
    if (watchers_.check(watcher))
        watchers_.remove(watcher);
    watcher->actions_.remove(this);
}

void ActionImpl::removeAllWatchers()
{
    ActionWatcher** watcher = watchers_.list();
    if (0 == watcher)
        return;
    for (; *watcher; ++watcher) {
        if (PackedPointerSet<ActionWatcher>::isValid(*watcher))
            (*watcher)->actions_.remove(this);
    }
    watchers_.clear();
}

void Action::notifyChildInserted(Action* ins_action)
{
    if (ins_action->parent()->getBool(IS_TOGGLEABLE)) {
        ins_action->setBool(IS_TOGGLEABLE, true);
        const PropertyNode* current_idx = 
            ins_action->parent()->properties()->getProperty(CURRENT_ACTION);
        if (current_idx && 
            int(ins_action->siblingIndex()) == current_idx->getInt())
                ins_action->setBool(IS_TOGGLED, true);
    }
    NOTIFY_ACTION_WATCHERS(ins_action->parent(), childInserted(ins_action));
}

void Action::notifyChildRemoved(ActionList* rem_parent, Action* rem_action)
{
    NOTIFY_ACTION_WATCHERS(static_cast<Action*>(rem_parent),
                           childRemoved(static_cast<Action*>(rem_parent),
                           rem_action));
}

void Action::dump(int indent) const
{
    DINDENT(indent);
    DBG_IF(UI.CMD) {
        DBG(UI.CMD) << "UiAction:" << std::endl;
        properties()->dump(indent);
        ItemEntry* ie = items().firstChild();
        for (; ie; ie = ie->nextSibling())
            ie->asItem()->dump(indent + 4, false);
        for (Action* a = firstChild(); a; a = a->nextSibling())
            a->dump(indent + 4);
    }
}

} // namespace

