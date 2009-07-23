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
#include "ui/UiProps.h"
#include "ui/UiItem.h"
#include "ui/UiItemWatcher.h"
#include "ui/UiAction.h"
#include "ui/UiDocument.h"
#include "ui/IconProvider.h"
#include "ui/ActionSet.h"
#include "ui/impl/ui_debug.h"
#include "ui/impl/ShortcutAction.h"

#include <QWidget>
#include <QVariant>

#include <list>
#include <typeinfo>

using namespace Common;

namespace Sui
{

#define NOTIFY_ITEM_WATCHERS(item, func) \
    if (!item->watchers_.isNull()) { \
        ItemWatcher** watcher = item->watchers_.list(); \
        for (; *watcher; ++watcher) \
            if (PackedPointerSet<ItemWatcher>::isValid(*watcher)) \
                (*watcher)->func ; \
    } \

#define NOTIFY_SUBTREE_WATCHERS(item, func) \
    if (!item->treeWatchers_.isNull()) { \
        ItemWatcher** watcher = item->treeWatchers_.list(); \
        for (; *watcher; ++watcher) \
            if (PackedPointerSet<ItemWatcher>::isValid(*watcher)) \
                (*watcher)->func ; \
    } \

String ui_item_name(const String& name)
{
    String item_name = name.stripWhiteSpace();
    item_name[0] = to_lower(item_name[0]);
    return item_name;
}

void add_boolean_spec(Sui::Item* item, const char* propName)
{
    PropertyNode* prop_specs = item->itemProps()->makeDescendant(
        Sui::ITEM_PROP_SPECS + String('/') + propName);
    prop_specs->appendChild(new PropertyNode("true"));
    prop_specs->appendChild(new PropertyNode("false"));
}

////////////////////////////////////////////////////////////////////////////

Item::Item(const String& name, Action* action)
    : PropertyTreeHolder(new PropertyNode(ITEM_PROPS)),
      action_(action),
      isAttached_(false)
{
    add_boolean_spec(this, IS_VISIBLE);
    if (action_) {
        action_->items().appendChild(this);
        action_->itemAdded(this);
        itemProps()->makeDescendant(ACTION, action_->get(NAME), false);
    }
    itemProps()->appendChild(new PropertyNode(NAME, name));
    itemProps()->makeDescendant(IS_VISIBLE, "true", false);

    installSyncher(itemProps());
    if (!itemProps()->parent()) {
        itemProps()->setName("ui-item");
        itemProps()->addWatcher(this);
    }
}

Item::Item(Action* action, PropertyNode* properties)
    : PropertyTreeHolder((properties) ?
        properties : new PropertyNode(ITEM_PROPS)),
      action_(action),
      isAttached_(false)
{
    add_boolean_spec(this, IS_VISIBLE);
    if (action_) {
        action_->items().appendChild(this);
        action_->itemAdded(this);
        itemProps()->makeDescendant(ACTION, action_->get(NAME), false);
    }
    itemProps()->makeDescendant(IS_VISIBLE, "true", false);

    installSyncher(itemProps());
    if (!itemProps()->parent()) {
        itemProps()->setName("ui-item");
        itemProps()->addWatcher(this);
    }
}

Action* Item::action() const
{
    return action_.pointer();
}

/////////////

PropertyNode* Item::itemProps(bool update) const
{
    if (update)
        const_cast<Item*>(this)->updateProperties();
    return PropertyTreeHolder::properties();
}

PropertyNode* Item::currActionProp() const
{
    PropertyNode* action = property("action");
    if (!action) 
        return 0;
    int num = action->getSafeProperty(CURRENT_ACTION)->getInt();
    for (PropertyNode* c = action->firstChild(); c; c = c->nextSibling()) {
        if (c->name() != ACTION)
            continue;
        if (0 == num)
            return c;
        num--;
    }
    return 0;
}

void Item::languageChanged()
{
    PropertyNode* inscription = property(INSCRIPTION);
    if (inscription)
        propertyChanged(inscription);
    for (Item* item = firstChild(); item; item = item->nextSibling())
        item->languageChanged();
}

/////////////

void Item::propertyChanged(PropertyNode* prop)
{
    if (isLockedNotify())
        return;
    
    DBG(UI.PROP) << "UiItem<" << get(NAME) << "> property changed: " 
                 << prop->name() << " <" << prop->getString() << ">\n";
    if (prop->parent() && ACTION == prop->name() &&
        (0 == action_ || action_->get(NAME) != prop->getString())) {
        //! TODO: treat item<=>multi-item conversion
        if (action_) 
            ItemEntry::remove();
        detach(false);
        action_ = findAction(prop->getString());
        if (action_) 
            action_->items().appendChild(this);
        attach(true);
        return;
    }
    if (NAME == prop->name()) {
        prop->setString(ui_item_name(prop->getString()));
        return;
    }
    if (IS_ENABLED == prop->name()) {
        setEnabled(prop->getBool());
        return;
    }
    if (IS_VISIBLE == prop->name()) {
        setItemVisible(prop->getBool());
        return;
    }
    if (0 == prop->name().find("qt:") && widget(0)) 
        widget()->setProperty(QString(prop->name().mid(3)), 
                              QVariant(prop->getString()));
}

void Item::actionPropChanged(PropertyNode* actionProp, Action*)
{
    propertyChanged(actionProp);
}

///////////////////////////////////////////////////////////////////////////

Action* Item::findAction(const String& actionName) const
{
    if (parent())
        return parent()->findAction(actionName);
    return 0;
}

Item* Item::findItem(const ItemPred& pred) const
{
    if (pred(this))
        return const_cast<Item*>(this);
    for (Item* item = firstChild(); item; item = item->nextSibling()) {
        Item* foundItem = item->findItem(pred);
        if (foundItem)
            return foundItem;
    }
    return 0;
}

Document* Item::documentItem() 
{
    if (parent())
        return parent()->documentItem();
    return 0;
}

RootItem* Item::rootItem() 
{
    if (parent())
        return parent()->rootItem();
    return 0;
}

void Item::dispatch()
{
    if (action_)
        dispatch(action_.pointer());
}

Action* Item::actionToDispatch(Action* action)
{
    Action* to_dispatch(action);
    if (action->parent()) {
        int idx = 0;
        for (Action* i = action->prevSibling(); i; i = i->prevSibling())
            idx++;
        action->parent()->setInt(CURRENT_ACTION, idx);
        to_dispatch = action->parent();
    }
    return to_dispatch;
}

void Item::dispatch(Action* action)
{
    if (parent())
        return parent()->dispatch(actionToDispatch(action));
}

QWidget* Item::widget(const Item*) const
{
    return 0;
}

void Item::attach(bool recursive)
{
    if (!isAttached_) {
        DBG_IF(UI.DYN) {
            const Item* parent_item = parent();
            DBG(UI.DYN) << "Attach: " << itemClass() << " "  << get(ACTION) 
                        << " To: " << parent_item->itemClass() << " "  
                        << parent_item->get(ACTION) << std::endl;
            DBG_IF(UI.DUMP) dump(false);
        }
        if (doAttach()) {
            setAccelerators();
            isAttached_ = true;
        }
    }
    if (recursive)
        for (Item* i = firstChild(); i; i = i->nextSibling())
            i->attach(recursive);
}

void Item::detach(bool recursive)
{
    if (isAttached_) {
        DBG(UI.DYN) << "Detach: " << itemClass() << " "  
                    << get(ACTION) << std::endl;
        DBG_IF(UI.DUMP) dump(false);

        if (doDetach())
            isAttached_ = false;
    }
    if (recursive)
        for (Item* i = firstChild(); i; i = i->nextSibling())
            i->detach(recursive);
}

QWidget* Item::parentWidget() const
{
    return parent() ? parent()->widget(this) : 0;
}

bool Item::doAttach()
{
    if (!parent())
        return false;

    QWidget* item_widget = widget(0);
    QWidget* parent_widget = parentWidget();

    if (!item_widget)
        return true;
    if (item_widget->parent() == parent_widget)
        return false;

    DBG(UI.DYN) << "doAttach: " << get(NAME) << "(" 
                << typeid(*item_widget).name() << ") To: "
                << parent()->get(NAME) << "(" 
                << typeid(*parent_widget).name() 
                << ") is-visible:" << getBool(IS_VISIBLE) << std::endl;
    DBG_IF(UI.DUMP) dump(false);

    // BUG WORKAROUND FOR Qt4.2 - reparenting of qmenu does not work
    if (!item_widget->inherits("QMenu")) {//
        item_widget->setParent(parent_widget);
        item_widget->setVisible(getBool(IS_VISIBLE)); 
    }
    return true;
}

void dump_item_attach(Item* item, QWidget* widget, QWidget* parentWidget)
{
    DBG(UI.DYN) << "doAttach: " << item->get(NAME);
    if (widget)
        DBG(UI.DYN) << " (" << typeid(*widget).name() << ")";
    DBG(UI.DYN) << " To: " << item->parent()->get(NAME) << "(" 
                << typeid(*parentWidget).name() 
                << ") is-visible:" << item->getBool(IS_VISIBLE) << std::endl;
    DBG_IF(UI.DUMP) item->dump(false);
}

bool Item::doDetach()
{
    QWidget* item_widget = widget(0);
    QWidget* parent_widget = item_widget ? item_widget->parentWidget() : 0;
    if (!item_widget || !parent_widget)
        return false;
    
    DBG(UI.DYN) << "doDetach: " << get(NAME) << "(" 
                << typeid(*item_widget).name() << ") From: " 
                << typeid(*parent_widget).name() << std::endl;
    DBG_IF(UI.DUMP) dump(false);

    item_widget->setParent(0);
    return true;
}

void Item::setItemVisible(bool isVisible)
{
    QWidget* item_widget = widget(0);
    if (item_widget && item_widget->parentWidget())
        item_widget->setVisible(isVisible);
}

void Item::setEnabled(bool isEnabled)
{
    QWidget* item_widget = widget(0);
    if (item_widget)
        item_widget->setEnabled(isEnabled);
}

void Item::grabFocus() const
{
    QWidget* w = widget(0);
    if (w) {
        w->setActiveWindow();
        w->setFocus();
    }
}

/////////////////////////////////////////////////////////////////////////

void Item::addWatcher(ItemWatcher* watcher, bool watchSubtree)
{
    if (watchSubtree) 
        treeWatchers_.add(watcher);
    else
        watchers_.add(watcher);
    watcher->items_.add(this);
}

void Item::removeWatcher(ItemWatcher* watcher)
{
    watchers_.remove(watcher);
    treeWatchers_.remove(watcher);
    watcher->items_.remove(this);

}

void Item::clearPtrSet(PackedPointerSet<ItemWatcher>& ptrSet)
{
    ItemWatcher** watcher = ptrSet.list();
    if (0 == watcher)
        return;
    for (; *watcher; ++watcher) {
        if (PackedPointerSet<ItemWatcher>::isValid(*watcher))
            (*watcher)->items_.remove(this);
    }
    ptrSet.clear();
}

void Item::removeAllWatchers()
{
    clearPtrSet(watchers_);
    clearPtrSet(treeWatchers_);
}

void Item::notifyChildInserted(Item* item)
{
    item->parent()->childInserted(item);
    NOTIFY_ITEM_WATCHERS(item->parent(), childInserted(item));
    for (Item* i = item->parent(); i; i = i->parent())
        NOTIFY_SUBTREE_WATCHERS(i, childInserted(item));

    NOTIFY_ITEM_WATCHERS(item, inserted(item));
}

void Item::notifyChildRemoved(ItemList* parent, Item* item)
{
    static_cast<Item*>(parent)->childRemoved(item);
    NOTIFY_ITEM_WATCHERS(static_cast<Item*>(parent),
                         childRemoved(static_cast<Item*>(parent), item));
    for (Item* i = item->parent(); i; i = i->parent())
        NOTIFY_SUBTREE_WATCHERS(i, childRemoved(static_cast<Item*>(parent), 
                                                item));
    NOTIFY_ITEM_WATCHERS(item, removed(item));
}

void Item::childInserted(Item* item)
{
    DBG(UI.DYN) << "child inserted <" << get(NAME) << ">:" 
                << item->get(NAME) << std::endl;
    item->inserted();
}

void Item::childRemoved(Item* item)
{
    DBG(UI.DYN) << "child removed(" << item << ") " << item->get(NAME)
        << std::endl;
    item->ItemBase::setParent(this);
    item->doDetach();
    item->ItemBase::setParent(0);
    item->removed();
}

PropertyNode* Item::property(const String& name) const 
{
    PropertyNode* pn = properties();
    if (pn->name() == name)
        return pn;
    pn = pn->getProperty(name);
    if (pn)
        return pn;
    return action() ? action()->properties()->getProperty(name) : 0;
}

void Item::releaseFocus() const
{
    if (!parent())
        return;
    Document* doc_item = parent()->documentItem();
    if (doc_item)
        doc_item->grabFocus();
}

UI_EXPIMP const char* make_seqid(int& seq, const char* name)
{
    static char buf[128];
#ifdef WIN32
    _snprintf
#else // WIN32
    snprintf
#endif // WIN32
        (buf, sizeof(buf) - 1, "%s:%d", name, seq);
    ++seq;
    return buf;
}

String Item::translationContext() const
{
    const PropertyNode* context_prop = 
        properties()->getProperty(TRANSLATION_CONTEXT);
    if (context_prop)
        return context_prop->getString();
    return action() ? action()->translationContext() : String();
}

/////////////////////////////////////////////////////////////////////////

ShortcutAction::ShortcutAction(Action* action, Item* item, QWidget* parent)
    : QShortcut(QKeySequence(action->get(ACCEL)), parent),
      action_(action),
      item_(item)
{
    connect(this, SIGNAL(activated()), this, SLOT(activated()));
}

ShortcutAction::~ShortcutAction()
{
}

static void set_shortcut_context(const String& context, QShortcut* sh)
{
    sh->setProperty(NOTR("context"), QString(context));
}

class Item::ShortcutActionList : public QList<QPointer<QShortcut> > {};

void Item::setAccelerators()
{
    QWidget* w = widget(0);
    if (!w)
        return;
    if (shortcutActions_) {
        ShortcutActionList::iterator it = shortcutActions_->begin();
        for (; it != shortcutActions_->end(); ++it)
            delete *it;
        shortcutActions_.clear();
    }
    Document* doc = documentItem();
    if (!doc)
        return;
    ActionSet* aset = doc->actionSet();
    String context;
    const PropertyNode* s_prop = itemProps()->firstChild();
    for (; s_prop; s_prop = s_prop->nextSibling()) {
        if (s_prop->name() != SHORTCUTS)
            continue;
        const PropertyNode* prop = s_prop->firstChild();
        for (; prop; prop = prop->nextSibling()) {
            if (prop->name() == NOTR("context")) {
                context = prop->getString();
                continue;
            }
            if (prop->name() != ACTION)
                continue;
            Action* action = aset->findAction(prop->getString());  
            if (!action)
                continue;
            if (action->property(ACCEL)) {
                if (!shortcutActions_)
                    shortcutActions_ = new ShortcutActionList;
                shortcutActions_->push_back(
                    new ShortcutAction(action, this, w));
                set_shortcut_context(context, shortcutActions_->back());
            }
            // treat sub-actions
            action = action->firstChild();
            for (; action; action = action->nextSibling()) {
                if (action->property(ACCEL)) {
                    if (!shortcutActions_)
                        shortcutActions_ = new ShortcutActionList;
                    shortcutActions_->push_back(
                        new ShortcutAction(action, this, w));
                    set_shortcut_context(context, shortcutActions_->back());
                }
            }
        }
    }
}

Item::~Item()
{
    removeAllWatchers();
}

/////////////////////////////////////////////////////////////////////////

void Item::dump(int indent, bool recursive) const
{
    DINDENT(indent);
    DBG(UI.ITEM) << "Sui::Item <" << get(NAME) << ">:" << this << " action<" 
                 << ((action_) ? action_->get(NAME) : "NONE")
                 << "> widget:" << widget(0) << " RCNT:"
                 << getRefCnt() << std::endl;
    if (recursive && firstChild()) {
        uint num = 0;
        for (Item* i = firstChild(); i; i = i->nextSibling()) {
            DBG(UI.ITEM) << "    child# " << num << std::endl;
            i->dump(4, recursive);
            num++;
        }
    }
}

} // namespace Sui
