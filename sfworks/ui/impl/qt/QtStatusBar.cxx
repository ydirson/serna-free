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
#include "ui/IconProvider.h"
#include "ui/UiAction.h"

#include "ui/impl/ui_debug.h"
#include "ui/impl/qt/QtStatusBar.h"

#include <QLayout>
#include <QStatusBar>
#include <QMainWindow>
#include <QApplication>

using namespace Common;

namespace Sui {

/////////////////////////////////////////////////////////////////////////

CUSTOM_ITEM_MAKER(StatusBar, QtStatusBar)

QtStatusBar::QtStatusBar(Action*, PropertyNode* props)
    : Item(0, props),
      statusBar_(0)
{
}

bool QtStatusBar::doAttach()
{
    QMainWindow* main_win = dynamic_cast<QMainWindow*>(parent()->widget(0));
    if (0 == main_win) {
        DBG(UI.ITEM) << "QtStatusBar: cannot attach: parent is not MainWindow"
                     << std::endl;
        return false;
    }
    statusBar_ = main_win->statusBar();
    statusBar_->setSizeGripEnabled(getBool(NOTR("sizeGripEnabled")));
    if (!widget_) {
        widget_ = new QWidget;
        widget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        QLayout* layout = new QHBoxLayout(widget_);
        layout->setSpacing(2);
        layout->setMargin(2);
    } 
    statusBar_->addPermanentWidget(widget_, 1);
    widget_->setVisible(true);
    DBG_IF(UI.DYN) dump_item_attach(this, statusBar_, main_win);

    return true;
}

bool QtStatusBar::doDetach()
{
    if (statusBar_ && widget_ && statusBar_ == widget_->parentWidget()) {
        statusBar_->removeWidget(widget_);
        widget_->setParent(0);
    }
    return true;
}

QWidget* QtStatusBar::widget(const Item*) const
{
    return widget_;
}

/////////////////////////////////////////////////////////////////////////

CUSTOM_ITEM_MAKER(Spacer, QtSpacer)

QtSpacer::QtSpacer(Action* action, PropertyNode* props)
    : Item(action, props),
      stretch_(itemProps()->getSafeProperty(STRETCH)->getInt()),
      spacer_(new QWidget())
{}

QWidget* QtSpacer::widget(const Item*) const
{
    return spacer_;
}

} // namespace Sui
