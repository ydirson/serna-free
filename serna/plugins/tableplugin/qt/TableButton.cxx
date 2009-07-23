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
#include "TableButton.h"
#include "TableGridView.h"
#include "ui/IconProvider.h"

#include "structeditor/StructEditor.h"
#include "genui/StructDocumentActions.hpp"
#include "common/CommandEvent.h"
#include "common/OwnerPtr.h"
#include "common/PropertyTreeEventData.h"

#include <QApplication>
#include <QShortcut>
#include <QToolButton>
#include <QPainter>

using namespace Common;

namespace Sui {
  CUSTOM_ITEM_MAKER(TableButton, TableButton)
}

TableButton::TableButton(Sui::Action* action, PropertyNode* properties)
    : Sui::Item(action, properties)
{
    button_ = new QToolButton(0, String(Sui::TABLE_BUTTON +
                              action->get(Sui::NAME)).utf8().c_str());
    button_->setAutoRaise(true);
    button_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                                       QSizePolicy::Fixed));
    if (action) {
        button_->setEnabled(action->getBool(Sui::IS_ENABLED));
        button_->setIconSet(
            Sui::icon_provider().getIconSet(action->get(Sui::ICON)));
        connect(button_, SIGNAL(clicked()), this, SLOT(clicked()));
    }
    //! Set Accel
    String accel = action->get(Sui::ACCEL);
    if (!accel.isEmpty()) {
        if (properties->getProperty("use_accel"))
            button_->setAccel(QKeySequence(accel));
        accel = NOTR(" <b>") + accel + NOTR("</b>");
    }
    else
        button_->setAccel(QKeySequence());
    String str = action->get(Sui::TOOLTIP);
    button_->setToolTip(NOTR("<qt><nobr> ") + 
        str + accel + NOTR("</nobr></qt>"));
    popup_ = new QMenu(button_);
    TableGridView* grid = new TableGridView(popup_, action->properties());
    //popup_->insertItem(grid); NONEXISTENT IN Qt4
    button_->setPopup(popup_);
    button_->setPopupDelay(1);
    connect(grid, SIGNAL(activated()), this, SLOT(clicked()));
    connect(button_, SIGNAL(released()), grid, SLOT(buttonReleased()));
}

QWidget* TableButton::widget(const Sui::Item*) const
{
    return button_;
}

void TableButton::clicked()
{
    dispatch();
}
