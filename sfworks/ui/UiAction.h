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
#ifndef UI_ACTION_H_
#define UI_ACTION_H_

#include "ui/ui_defs.h"

#include "ui/UiItem.h"

#include "common/PropertyTree.h"
#include "common/String.h"
#include "common/RefCntPtr.h"
#include "common/SernaApiRefCounted.h"
#include "common/OwnerPtr.h"

class QAction;

namespace Common
{
    class PropertyNode;
}

namespace Sui {

/////////////////////////////////////////////////////////////////////////////

class ActionWatcher;
class Item;
class ItemEntryList;

class UI_EXPIMP Action : public Common::SernaApiRefCounted,
                         public Common::XTreeNode<Action, Action>,
                         public PropertyTreeHolder {
public:
    typedef Common::String          String;
    typedef Common::PropertyNode    PropertyNode;
    
    virtual ItemEntryList&  items() const = 0;

    void            itemAdded(Item* item);
    void            removeItem(Item* item);
    void            removeItems();
    
    //! Delete this action along with its items
    void            removeAction();

    void            setEnabled(bool isEnabled);
    void            setToggled(bool isOn);

    //! Management of ItemWatchers
    virtual void    addWatcher(ActionWatcher* watcher) = 0;
    virtual void    removeWatcher(ActionWatcher* watcher) = 0;
    virtual void    removeAllWatchers() = 0;

    void            dispatch() const;
    void            dump(int indent) const;
    
    // returns currently active subaction (of any)
    Action*         activeSubAction() const;
    // get sub-action by name
    Action*         getSubAction(const String& name) const;
    // sets new active subaction. If named sub-action wasn't found,
    // returns false.
    bool            setActiveSubAction(const String& name = String());
    
    virtual String  translationContext() const;

    // for reused QActions
    virtual QAction*    qAction() const = 0;
    virtual void        setQAction(QAction*) = 0;

    static  Action* make(PropertyNode*);
    virtual ~Action();

protected:
    Action(PropertyNode*);
    typedef Common::XTreeNode<Action, Action> ATN;
    void            remove() { ATN::remove(); }

    //! Child notifications
    friend class Common::XListItem<Action, Action>;
    friend class Common::XList<Action, Action>;
    friend class Item;
    typedef Common::XList<Action, Action> ActionList;

    static void     notifyActionDestroyed(Action* action);
    static void     notifyChildInserted(Action* action);
    static void     notifyChildRemoved(ActionList* parent, Action* action);

    //! Reference counting: use RefCounted interface
    static void     xt_incRefCnt(Action* action) { action->incRefCnt(); }
    static int      xt_decRefCnt(Action* action) { return action->decRefCnt();}
    
private:
    DEFAULT_ASSIGN_OP_DECL(Action);
    DEFAULT_COPY_CTOR_DECL(Action);
};

typedef Common::RefCntPtr<Action> ActionPtr;

} // namespace Sui

#endif // UI_ACTION_H_
