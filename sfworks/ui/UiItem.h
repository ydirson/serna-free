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
#ifndef UI_ITEM_H_
#define UI_ITEM_H_

#include "ui/ui_defs.h"
#include "ui/UiPropSyncher.h"
#include "ui/prop_locker.h"

#include "common/SernaApiRefCounted.h"
#include "common/common_defs.h"
#include "common/String.h"
#include "common/RefCntPtr.h"
#include "common/XTreeNode.h"
#include "common/PackedPtrSet.h"
#include "common/PropertyTree.h"

class QWidget;
class QPoint;

namespace Sui {

void dump_item_attach(Item* item, QWidget* widget, QWidget* parentWidget);    

////////////////////////////////////////////////////////////////////////

class Item;
class Document;

class UI_EXPIMP ItemPred {
public:
    virtual ~ItemPred() {}
    virtual bool        operator()(const Item*) const = 0;
};

void add_boolean_spec(Sui::Item* item, const char* propName);

////////////////////////////////////////////////////////////////////////

class Action;
class ItemWatcher;
class Item;
class RootItem;

class ItemEntry : public Common::XListItem<ItemEntry> {
public:
    Item*   asItem();
    virtual ~ItemEntry() {}
};

class ItemEntryList : public Common::XList<ItemEntry> {};

class UI_EXPIMP Item : public PropertyTreeHolder,
                       public Common::SernaApiRefCounted,
                       public Common::XTreeNode<Item, Item>,
                       public ItemEntry,
                       protected Common::PropertyNodeWatcher,
                       public PropertyLocker {
public:
    typedef COMMON_NS::String       String;
    typedef Common::PropertyNode    PropertyNode;
    typedef Common::XList<Item, Item> ItemList;
    typedef Common::XTreeNode<Item, Item> ItemBase;

    Item(const String& name, Action* action);
    Item(Action* action, PropertyNode* properties);
    virtual ~Item();

    Action*             action() const;
    //! Returns the class name for generating items using factory
    virtual String      itemClass() const = 0;
    //! Returns the widget class to check items compatibility
    virtual String      widgetClass() const = 0;
    //! Returns widget made by this item (for child item if given)
    virtual QWidget*    widget(const Item* child = 0) const;
    QWidget*            parentWidget() const;

    //! Update the properties (if asked) and returns them
    PropertyNode*       itemProps(bool update = false) const;
    //! reimplemented from PropTreeSyncher
    virtual PropertyNode* property(const String&) const;
    virtual String      translationContext() const;

    //! For MultiAction items - returns the current subaction root property
    PropertyNode*       currActionProp() const;

    void                attach(bool recursive = false);
    void                detach(bool recursive = false);
    
    //! Sends focus to widget created by this item
    virtual void        grabFocus() const;
    //! Set focus back to document item
    virtual void        releaseFocus() const;
    //! Updates widget if changes were made

    //! Reimplemented from ItemBase
    virtual void        removeItem() { remove(); }

    virtual void        showContextMenu(const QPoint&) {};
    virtual void        parentUpdate() {}

    //! Dispatch Item`s command if any
    virtual void        dispatch();
    virtual void        dispatch(Action* action);
    virtual Action*     findAction(const String& name) const;
    virtual Item*       findItem(const ItemPred& pred) const;

    //! Management of ItemWatchers
    void                addWatcher(ItemWatcher* watcher, 
                                   bool watchSubtree = false);
    void                removeWatcher(ItemWatcher* watcher);
    void                removeAllWatchers();

    virtual Document*   documentItem();
    virtual RootItem*   rootItem();

    virtual void        dump(int indent, bool recursive = true) const;

    //! Manages visiility of created widget
    virtual void        setItemVisible(bool isVisible);

    virtual void        languageChanged();

    REDECLARE_XTREENODE_INTERFACE_BASE(ItemBase, Item);

protected:
    //! Attaches widget made by this item to parent item`s widget
    virtual bool        doAttach();
    //! Detaches widget made by this item from parent item`s widget
    //  NOTE: never use "parent()" within detach - it can be NULL
    virtual bool        doDetach();

    virtual void        setEnabled(bool isEnabled);

    // reimplemented from PropertyTreeHolder
    virtual PropertyNode* properties() const { return itemProps(); }
    virtual void        updateProperties() {};
    Action*             actionToDispatch(Action*);
    void                setAccelerators();

    //! Property modifications
    virtual void        propertyChanged(PropertyNode* property);
    virtual void        actionPropChanged(PropertyNode* actionProp,
                                          Action* subAction = 0);
    void                clearPtrSet(Common::PackedPointerSet<ItemWatcher>&);
    //! Child notifications
    virtual void        childInserted(Item*);
    virtual void        childRemoved(Item*);
    //! Tells that this item is inserted/removed from parent
    virtual void        inserted() {}
    virtual void        removed() {}

    //! Child notifications
    friend class ActionImpl;
    friend class Common::XListItem<Item, Item>;
    friend class Common::XList<Item, Item>;

    static void         notifyItemDestroyed(Item* item);
    static void         notifyChildInserted(Item* item);
    static void         notifyChildRemoved(ItemList* parent, Item* item);

    //! Reference counting: use RefCounted interface
    static void         xt_incRefCnt(Item* item) { item->incRefCnt(); }
    static int          xt_decRefCnt(Item* item) { return item->decRefCnt(); }

private:
    class ShortcutActionList;

    Item(const Item&);
    Item& operator=(const Item&);
    Item();

    COMMON_NS::RefCntPtr<Action>            action_;
    Common::PropertyNodePtr                 itemProps_;
    Common::PackedPointerSet<ItemWatcher>   watchers_;
    Common::PackedPointerSet<ItemWatcher>   treeWatchers_;
    Common::OwnerPtr<ShortcutActionList>    shortcutActions_;
    bool                                    isAttached_;
};

inline Item* ItemEntry::asItem()
{
    return static_cast<Item*>(this);
}

typedef COMMON_NS::RefCntPtr<Item> ItemPtr;

///////////////////////////////////////////////////////////////////////////

//! TODO: Remove
template <class T> UI_EXPORT
Item*   makeUiItem(const Common::String& name, Action* action);

template <class T> UI_EXPORT
Item*   makeUiItem(Action* action, Common::PropertyNode* properties);

typedef Item* (*ItemMakerFunc)(Action* action,
                               Common::PropertyNode* properties);

///////////////////////////////////////////////////////////////////////////

class ItemMaker : public Common::SernaApiRefCounted {
public:
    virtual ~ItemMaker() {}
    virtual Item* makeItem(Action* action, 
                           Common::PropertyNode* properties) = 0;
};
    
typedef Common::RefCntPtr<ItemMaker>    ItemMakerPtr;

UI_EXPIMP const char* make_seqid(int& seq, const char* name);

///////////////////////////////////////////////////////////////////////////

#if !defined(NO_UI_ITEM_MAKER_EXPORT)
# define UI_ITEM_MAKER_EXPORT UI_EXPORT
#else
# define UI_ITEM_MAKER_EXPORT
#endif

#define CUSTOM_ITEM_MAKER(ItemClass, CustomItem)                    \
template<> UI_ITEM_MAKER_EXPORT Item*                               \
makeUiItem<ItemClass>(Action* action, Common::PropertyNode* properties) \
{                                                                   \
    return new CustomItem(action, properties);                      \
}

#define COMMANDLESS_ITEM_MAKER(ItemClass, CustomItem)               \
template<> UI_ITEM_MAKER_EXPORT Item*                               \
makeUiItem<ItemClass>(Action*, Common::PropertyNode* properties)    \
{                                                                   \
    return new CustomItem(properties);                              \
}

}

#endif // UI_ITEM_H_

