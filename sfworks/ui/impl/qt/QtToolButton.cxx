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

#include <QAction>
#include <QLayout>
#include <QToolBar>
#include <QToolButton>
#include <QPushButton>
#include <QPointer>
#include <QMenuBar>

#include <iostream>

using namespace Common;

namespace Sui {

class QtToolButton : public QtActionItem {
public:
    QtToolButton(Action* action, PropertyNode* properties)
        : QtActionItem(action, properties) {}
    virtual String      widgetClass() const { return TERMINAL_WIDGET; }
    virtual QWidget*    widget(const Item*) const { return button_; }
    virtual String      itemClass() const { return TOOL_BUTTON; }

private:
    virtual bool        doAttach();
    virtual bool        doDetach();
    QPointer<QToolButton> button_;
};

bool QtToolButton::doDetach()
{
    removeQAction();
    if (button_)
        delete button_;
    return true;
}

bool QtToolButton::doAttach()
{
    QWidget* parent_widget = parentWidget();
    QMenuBar* main_menu = 0;
    if (Item::parent()->itemClass() == MAIN_MENU) {
        main_menu = dynamic_cast<QMenuBar*>(parentWidget());
        if (0 == main_menu)
            return false;
        parent_widget = main_menu;
    }
    if (!main_menu && parent_widget->inherits(NOTR("QToolBar")))
        return insertQAction();
    QToolButton* tool_button = new QToolButton(parent_widget);
    button_ = tool_button;
    tool_button->setAutoRaise(true);
    tool_button->setDefaultAction(qAction());
    if (main_menu) {
        tool_button->setFocusPolicy(Qt::NoFocus);
        main_menu->setCornerWidget(tool_button, Qt::TopRightCorner);
    } else if(parent_widget->layout())
        parent_widget->layout()->addWidget(button_);
    return true;
}

CUSTOM_ITEM_MAKER(ToolButton, QtToolButton)

////////////////////////////////////////////////////////////////////////

class QtPushButton : public QObject, public Item {
    Q_OBJECT
public:
    QtPushButton(Action* action, PropertyNode* properties)
        : Item(action, properties) {}
    ~QtPushButton() { doDetach(); }

    virtual String      itemClass() const { return PUSH_BUTTON; }
    virtual String      widgetClass() const { return TERMINAL_WIDGET; }
    virtual QWidget*    widget(const Item*) const { return button_; }

public slots:
    void    clicked(bool);

protected:
    virtual bool        doAttach();
    virtual bool        doDetach();
    virtual void        propertyChanged(PropertyNode*);
    QPointer<QPushButton> button_;
};

bool QtPushButton::doAttach()
{
    QWidget* parent_widget = parentWidget();
    button_ = new QPushButton(parent_widget);
    propertyChanged(0);
    connect(button_, SIGNAL(clicked(bool)), this, SLOT(clicked(bool)));
    if (parent_widget->layout())
        parent_widget->layout()->addWidget(button_);
    return true;
}

void QtPushButton::propertyChanged(PropertyNode*)
{
    button_->setText(getTranslated(INSCRIPTION));
    button_->setIcon(icon_provider().getIconSet(get(ICON)));
    button_->setVisible(getBool(IS_VISIBLE));
}

void QtPushButton::clicked(bool)
{
    dispatch();
}

bool QtPushButton::doDetach()
{
    delete button_;
    return true;
}

CUSTOM_ITEM_MAKER(PushButton, QtPushButton)

} // namespace Sui

#include "moc/QtToolButton.moc"

