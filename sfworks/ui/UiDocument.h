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
 #ifndef UI_DOCUMENT_H_
#define UI_DOCUMENT_H_

#include "ui/ui_defs.h"
#include "ui/UiProps.h"
#include "ui/UiItem.h"
#include "ui/UiAction.h"

#include "common/String.h"
#include "common/OwnerPtr.h"

namespace Sui {

class ActionSet;
class MimeHandler;

class UI_EXPIMP ActionDispatcher {
public:
    virtual ~ActionDispatcher() {}

    virtual void    dispatch(Action* uiAction) = 0;
};

///////////////////////////////////////////////////////////////////////////

class CustomItemMaker : public ItemMaker {
public:
    CustomItemMaker(ItemMakerFunc itemMaker)
        : makerPtr_(itemMaker) {}

    virtual Item*   makeItem(Action* action,
                             Common::PropertyNode* properties)
    {
        return makerPtr_(action, properties);
    }

private:
    ItemMakerFunc   makerPtr_;
};

///////////////////////////////////////////////////////////////////////////

#include <list>

class StackWidget;

class UI_EXPIMP Document : public Item {
public:
    typedef Common::String String;

    Document(ActionDispatcher* dispatcher);
    ~Document();

    virtual void        showContextMenu(const QPoint& pos);
    virtual QWidget*    widget(const Item* child) const;

    virtual String      itemClass() const { return "Document"; }
    virtual String      widgetClass() const { return MAIN_WINDOW_WIDGET; }
    virtual bool        isVisual() const { return true; }
    virtual Document*   documentItem() { return this; }

    virtual void        saveDockingState();
    virtual void        restoreDockingState();

    ActionDispatcher*   dispatcher() const { return dispatcher_.pointer(); }
    ActionSet*          actionSet() const { return actionSet_.pointer(); }

    bool                isItemAcceptableIn(const String& itemClass,
                                           const Item* item) const;
    virtual Item*       makeItem(const String& itemClass,
                                 Common::PropertyNode* properties) const;

    void                    getItemMakerList(std::list<String>& list) const;
    Common::PropertyNodePtr getItemMakerProps(const String& itemClass) const;
    Common::PropertyNodePtr addItemMaker(const String& itemClass,
                                         ItemMakerPtr itemMaker,
                                         const String& acceptableIn);
    void                    removeItemMaker(const String& itemClass);

    // must return false if we don't want this item to be created
    virtual bool        isToMakeItem(const Common::PropertyNode*) const;
    virtual void        openUrl(const String&) {}

    StackWidget*        stackWidget() const { return stackWidget_.pointer(); }
    virtual void        makeStackWidget();

    MimeHandler&        mimeHandler() const { return *mimeHandler_.pointer(); }

    // for internal use
    virtual void        dispatch(Action* action);

protected:
    virtual bool        doAttach() { return true; }
    virtual bool        doDetach();
    virtual Action*     findAction(const String& name) const;
    virtual void        updateProperties();
    virtual void        setStackWidget(StackWidget* stackWidget);

private:
    class ItemMakerMap;
    Common::OwnerPtr<ItemMakerMap>       itemMakerMap_;
    Common::OwnerPtr<ActionSet>          actionSet_;
    Common::OwnerPtr<ActionDispatcher>   dispatcher_;
    Common::OwnerPtr<StackWidget>        stackWidget_;
    Common::OwnerPtr<MimeHandler>        mimeHandler_;
};

} // namespace Sui

#endif // UI_DOCUMENT_H_
