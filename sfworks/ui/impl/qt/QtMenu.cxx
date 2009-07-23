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
#include "ui/UiAction.h"
#include "ui/IconProvider.h"
#include "ui/UiProps.h"
#include "ui/UiItems.h"
#include "ui/impl/qt/QtActionSyncher.h"

#include "ui/impl/ui_debug.h"

#include "common/StringTokenizer.h"
#include "common/ScopeGuard.h"

#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>
#include <QPointer>
#include <QToolTip>
#include <QHelpEvent>
#include <typeinfo>

using namespace Common;

namespace Sui {

class QtMenuSeparator : public QtActionItem {
public:
    QtMenuSeparator(PropertyNode* props)
        : QtActionItem(0, props)
    {
        static int separator_seq = 0;
        UI_LOCK_PROP_GUARD
        itemProps()->makeDescendant(NAME, 
            make_seqid(separator_seq, NOTR("separator")), false);
        itemProps()->makeDescendant(IS_SEPARATOR, "true", true);
    }
    virtual String      itemClass() const {   return MENU_SEPARATOR; }
    virtual String      widgetClass() const { return TERMINAL_WIDGET; }
    virtual QWidget*    widget(const Item*) const { return 0; }
    virtual bool        doAttach() { return insertQAction(); }
    virtual bool        doDetach() { return removeQAction(); }
};

class QtToolBarSeparator : public QtMenuSeparator {
public:
    QtToolBarSeparator(PropertyNode* props)
        : QtMenuSeparator(props) {}
    virtual String      itemClass() const { return TOOL_BAR_SEPARATOR; }
};

COMMANDLESS_ITEM_MAKER(MenuSeparator,    QtMenuSeparator)
COMMANDLESS_ITEM_MAKER(ToolBarSeparator, QtToolBarSeparator)

/////////////////////////////////////////////////////////////////////

class UI_EXPIMP PopupMenu : public QtActionItem {
public:
    PopupMenu(Action* action, PropertyNode* properties);
    virtual ~PopupMenu();

    virtual String      itemClass() const { return POPUP_MENU; }
    virtual String      widgetClass() const { return MENU_WIDGET; }
    virtual QWidget*    widget(const Item*) const;
    void                clear(); // removes subitems and clears menu

protected:
    virtual bool        doAttach() { return insertQAction(); }
    virtual bool        doDetach() { return removeQAction(); }
    virtual void        aboutToShow();
    virtual void        parentUpdate();

private:
    class MenuImpl;
    Common::OwnerPtr<MenuImpl> menuImpl_;
};

class PopupMenu::MenuImpl : public QMenu {
    Q_OBJECT
public:
    MenuImpl(PopupMenu* menuItem)
        : menuItem_(*menuItem)
    { 
        connect(this, SIGNAL(aboutToShow()), this, SLOT(about_to_show()));
        setObjectName(NOTR("popupMenu_") + menuItem_.get(NAME));
    }
    PopupMenu&           menuItem_;

private:
    virtual bool event(QEvent*);

private slots:
    void about_to_show() { menuItem_.aboutToShow(); }
};

QWidget* PopupMenu::widget(const Item*) const
{
    return menuImpl_.pointer();
}

CUSTOM_ITEM_MAKER(PopupMenu, PopupMenu)

PopupMenu::PopupMenu(Action* action, PropertyNode* props)
    : QtActionItem(action, props),
      menuImpl_(new MenuImpl(this))
{
    UI_LOCK_PROP_GUARD
    if (!action)
        itemProps()->makeDescendant(IS_ENABLED, "true", false);
    setQAction(menuImpl_->menuAction());
}

void PopupMenu::clear()
{
    if (menuImpl_)
        menuImpl_->clear();
    Item::removeAllChildren();
}

void PopupMenu::aboutToShow()
{
    DBG_IF(UI.DYN) {
        DBG(UI.DYN) << "PopupMenu about-to-show actions: ";
        QList<QAction*> actions = menuImpl_->actions();
        QList<QAction*>::iterator it = actions.begin();
        for (; it != actions.end(); ++it) 
            DBG(UI.DYN) << "[" << (*it)->text() << "/" 
                << *it << "] ";
        DBG(UI.DYN) << std::endl;
    }
    for (Item* item = firstChild(); item; item = item->nextSibling()) 
        item->parentUpdate();
}

bool PopupMenu::MenuImpl::event(QEvent* event)
{   
    if (event->type() != QEvent::ToolTip)
        return QMenu::event(event);
    const QHelpEvent* hev = static_cast<const QHelpEvent*>(event);
    QAction* action = actionAt(hev->pos());
    if (action && !action->toolTip().isEmpty())
        QToolTip::showText(hev->globalPos(), action->toolTip(), this);
    return QMenu::event(event);
}

void PopupMenu::parentUpdate()
{
    dispatch();
}

PopupMenu::~PopupMenu()
{ 
}

/////////////////////////////////////////////////////////////////////

class QtMainMenu : public Item {
public:
    QtMainMenu(PropertyNode* properties)
        : Item(0, properties) {}
    ~QtMainMenu();

    virtual String      itemClass() const { return MAIN_MENU; }
    virtual String      widgetClass() const { return MENU_WIDGET; }
    virtual QWidget*    widget(const Item*) const { return menu_; }

protected:
    virtual bool        doAttach();
    virtual bool        doDetach();

private:
    QPointer<QMenuBar>  menu_;
};

COMMANDLESS_ITEM_MAKER(MainMenu, QtMainMenu)

UI_EXPIMP bool is_mac_app_menu = true;

bool QtMainMenu::doAttach()
{
    Item* first_doc = parent();
    if (!first_doc || !first_doc->parent())
        return false;
    QMainWindow* main_win =
        dynamic_cast<QMainWindow*>(first_doc->widget(0));
#ifdef __APPLE__
    if (is_mac_app_menu) {
        first_doc = first_doc->parent();
        QWidget* parent_widget = first_doc->widget(0);
        menu_ = parent_widget->findChild<QMenuBar*>();
        if (0 == menu_) 
	    menu_ = new QMenuBar(parent_widget);
    } else 
#endif // __APPLE__
    {
        if (0 == main_win) {
            DBG(UI.ITEM) 
                << "QtMainMenu: can only attach to MainWindow" << std::endl;
            return false;
        }
        DBG(UI.ITEM) << "QtMainMenu: attach to: " << main_win << std::endl;
        DBG_IF(UI.ITEM) dump(false);
        menu_ = main_win->menuBar();
    }
#ifdef WIN32
    //! workaround to make menu underline shortcuts under
    //  CustomStyle based from WindowStyle.
    BOOL cues = true;
    SystemParametersInfo(SPI_SETKEYBOARDCUES, 0, &cues, 0);
#endif
    return true;
}

QtMainMenu::~QtMainMenu()
{
    if (is_mac_app_menu)
        delete menu_;
}

bool QtMainMenu::doDetach()
{
    if (menu_) {
        DBG(UI.ITEM) << "QtMainMenu: detach: " << std::endl;
        DBG_IF(UI.ITEM) dump(false);

        for (Item* i = firstChild(); i; i = i->nextSibling())
            i->detach();
        if (is_mac_app_menu)
	    delete menu_;
        else
            menu_ = 0;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////

class QtMenuItem : public QtActionItem {
public:
    QtMenuItem(Action* action, PropertyNode* properties)
        : QtActionItem(action, properties) {}

    virtual String      itemClass() const { return MENU_ITEM; }
    virtual String      widgetClass() const { return TERMINAL_WIDGET; }
    virtual QWidget*    widget(const Item*) const { return 0; }

private:
    virtual bool        doAttach();
    virtual bool        doDetach() { return removeQAction(); }
};

CUSTOM_ITEM_MAKER(MenuItem, QtMenuItem)

bool QtMenuItem::doAttach()
{   
    if (getBool(IS_VISIBLE))
        qAction()->setVisible(true);
    return insertQAction();
}


/////////////////////////////////////////////////////////////////////

class UI_EXPIMP QtContextMenu : public QtActionItem {
public:
    QtContextMenu(PropertyNode* properties)
        : QtActionItem(0, properties) {}
    ~QtContextMenu()
    {
        delete menu_;
    }

    virtual String      itemClass() const { return CONTEXT_MENU; }
    virtual String      widgetClass() const { return MENU_WIDGET; }
    virtual void        showContextMenu(const QPoint& pos);
    virtual QWidget*    widget(const Item*) const { return menu_; }

protected:
    virtual bool        doAttach() { return true; }
    virtual bool        doDetach() { return true; }
    
private:
    QPointer<QMenu>     menu_;
};

COMMANDLESS_ITEM_MAKER(ContextMenu, QtContextMenu)

void QtContextMenu::showContextMenu(const QPoint& pos)
{
    DBG_IF(UI.ITEM) dump(0, true);
    delete menu_;
    menu_ = new QMenu(Item::parent()->widget(0));
    menu_->setObjectName(get(NAME));
    for (Item* item = firstChild(); item; item = item->nextSibling()) {
        item->detach(true);
        item->attach(true);
    }
    for (Item* item = firstChild(); item; item = item->nextSibling())
        item->parentUpdate();
    menu_->exec(pos);
    for (Item* item = firstChild(); item; item = item->nextSibling()) 
        item->detach(true);
    menu_->deleteLater();
}

/////////////////////////////////////////////////////////////////////

} // namespace Sui

#include "moc/QtMenu.moc"
