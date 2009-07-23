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
#include "ui/impl/ui_debug.h"
#include "ui/UiProps.h"
#include "ui/UiItems.h"
#include "common/ScopeGuard.h"

#include <QToolBar>
#include <QMainWindow>
#include <QApplication>
#include <QPointer>

using namespace Common;

namespace Sui {

/////////////////////////////////////////////////////////////////////////

class UI_EXPIMP QtToolBar : public Item {
public:
    QtToolBar(Action* action, PropertyNode* properties);
    ~QtToolBar();

    virtual String      itemClass() const { return TOOL_BAR; }
    virtual String      widgetClass() const { return TOOL_BAR_WIDGET; }
    virtual QWidget*    widget(const Item*) const { return toolbar_; }

    void                setupCaption();

protected:
    virtual bool        doAttach();
    virtual bool        doDetach();
    virtual void        propertyChanged(PropertyNode* prop);

private:
    QPointer<QToolBar>  toolbar_;
};

CUSTOM_ITEM_MAKER(ToolBar, QtToolBar)

QtToolBar::QtToolBar(Action*, PropertyNode* props)
    : Item(0, props),
      toolbar_(0)
{
}

void QtToolBar::propertyChanged(PropertyNode* prop)
{
    if (isLockedNotify())
        return;
    Item::propertyChanged(prop);
    if (toolbar_.isNull() || !prop)
        return;
    if (INSCRIPTION == prop->name())
        setupCaption();
}

void QtToolBar::setupCaption()
{
    UI_LOCK_PROP_GUARD
    PropertyNode* caption =
        itemProps()->makeDescendant(INSCRIPTION, get(NAME), false);
    toolbar_->setCaption(caption->getString().qstring());
}

bool QtToolBar::doAttach()
{
    QMainWindow* main_win = 
        dynamic_cast<QMainWindow*>(Item::parent()->widget(0));
    if (0 == main_win) {
        DBG(UI.ITEM) << "QtToolBar: can only attach to MainWindow\n";
        return false;
    }
    if (toolbar_.isNull()) {
        toolbar_ = new QToolBar(main_win);
        toolbar_->setObjectName("qtToolBar_" + get(NAME));
        toolbar_->setWindowTitle(getTranslated(INSCRIPTION));
        toolbar_->setPalette(QPalette(qApp->palette().window()));
        toolbar_->setAutoFillBackground(true);
    }
    DBG_IF(UI.DYN) dump_item_attach(this, toolbar_, main_win);
    if (getBool(BREAK_BEFORE))
        main_win->addToolBarBreak();
    main_win->addToolBar(toolbar_);
    setupCaption();
    return true;
}

bool QtToolBar::doDetach()
{
    if (toolbar_) {
        QMainWindow* main_win = 
            dynamic_cast<QMainWindow*>(Item::parent()->widget(0));
        if (main_win && main_win->toolBarBreak(toolbar_))
            main_win->removeToolBarBreak(toolbar_);
        for (Item* i = firstChild(); i; i = i->nextSibling()) 
            i->detach(false);
        delete toolbar_;
    }
    return true;
}

QtToolBar::~QtToolBar()
{
    delete toolbar_;
}

} // namespace Sui
