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
#include "ui/UiAction.h"
#include "ui/IconProvider.h"
#include "ui/impl/qt/QtRadioButton.h"
#include "ui/impl/ui_debug.h"

#include <QRadioButton>
#include <QButtonGroup>
#include <QToolTip>

using namespace Common;

/////////////////////////////////////////////////////////////////////////

namespace Sui {

CUSTOM_ITEM_MAKER(RadioButton, QtRadioButton)

QtRadioButton::QtRadioButton(Action* action, PropertyNode* props)
    : Item(action, props),
      noDispatch_(false)
{
    makeButton();
}

void QtRadioButton::makeButton()
{
    if (button_.isNull()) {
        button_ = new QRadioButton;
        button_->setObjectName(get(NAME));
    }
    else
        button_->disconnect(this);
    if (action()) {
        button_->setEnabled(action()->getBool(IS_ENABLED));
        button_->setText(getTranslated(INSCRIPTION));
        button_->setChecked(action()->getBool(IS_TOGGLED));
        connect(button_, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
    }
    //! Set Accel
    String accel = getTranslated(ACCEL);
    if (!accel.isEmpty()) {
        if (getBool("use-accel"))
            button_->setAccel(QKeySequence(accel));
    }
    else
        button_->setAccel(QKeySequence());
    accel = QString(QKeySequence(accel));
    QToolTip::add(button_, "<qt><nobr> " + getTranslated(TOOLTIP) + " " + 
                  accel + "</nobr></qt>");
}

bool QtRadioButton::doAttach()
{
    if (!Item::doAttach())
        return false;
    for (QWidget* parent = button_; parent; parent = parent->parentWidget()) {
        if (parent->inherits("QButtonGroup")) {
            //buttonGroup_ = static_cast<QButtonGroup*>(parent);
            //buttonGroup_->insert(button_);
            break;
        }
        parent = parent->parentWidget();
    }
    return true;
}

bool QtRadioButton::doDetach()
{
    if (!buttonGroup_.isNull()) {
        //buttonGroup_->remove(button_);
        buttonGroup_ = 0;
    }
    return Item::doDetach();
}

void QtRadioButton::propertyChanged(PropertyNode* prop)
{
    if (isLockedNotify())
        return;
    Item::propertyChanged(prop);
    if (button_ && prop) {
        button_->hide();
        makeButton();
        if (button_->parentWidget())
            button_->setVisible(getBool(IS_VISIBLE));
    }
}

QWidget* QtRadioButton::widget(const Item*) const
{
    return button_;
}

void QtRadioButton::setToggled(bool isOn)
{
    if (button_ && getBool(IS_TOGGLEABLE)) {
        if (button_->isOn() != isOn) {
            noDispatch_ = true;
            button_->setChecked(isOn);
            noDispatch_ = false;
        }
        String accel = QString(QKeySequence(getTranslated(ACCEL)));
        QToolTip::add(button_, "<qt><nobr> " + getTranslated(TOOLTIP) + " " + 
                      accel + "</nobr></qt>");
    }
}

void QtRadioButton::clicked()
{
    dispatch();
}

void QtRadioButton::toggled(bool isOn)
{
    if (noDispatch_)
        return;
    Item::property(IS_TOGGLED)->setBool(isOn);
    dispatch();
}

} // namespace Sui
