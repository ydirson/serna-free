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
#include "DocInfoDialog.h"

#include "common/PropertyTreeEventData.h"
#include "utils/HelpAssistant.h"

#include "structeditor/StructEditor.h"

#include "docutils/doctags.h"
#include "docview/Clipboard.h"

#include <QApplication>
#include <QHeaderView>
#include <QMenu>
#include <QCursor>

using namespace Common;

static QTreeWidgetItem* make_item(const PropertyNode* prop,
                                  QTreeWidget* list, 
                                  QTreeWidgetItem* parent, 
                                  QTreeWidgetItem* after)
{
    QTreeWidgetItem* item = (!parent) 
        ? new QTreeWidgetItem(list)
        : ((after) ? new QTreeWidgetItem(parent, after) 
                   : new QTreeWidgetItem(parent));
    item->setText(0, prop->name());
    item->setText(1, prop->getString());
    item->setToolTip(1, prop->getString());
    item->setExpanded(true);
    if (!prop->getString().isEmpty() || prop->firstChild())
        item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    else
        item->setFlags(0);
    for (PropertyNode* c = prop->firstChild(); c; c = c->nextSibling()) 
        after = make_item(c, list, item, after);
    return item;
}

//////////////////////////////////////////////////////////////////

DocInfoDialog::DocInfoDialog(QWidget* parent, const PropertyNode* props)
                             
    : QDialog(parent)
{
    setupUi(this);
    QHeaderView& header = *infoListView_->header();
    header.setStretchLastSection(true);
    header.setResizeMode(0, QHeaderView::ResizeToContents);
    header.setResizeMode(1, QHeaderView::ResizeToContents);
    infoListView_->setAllColumnsShowFocus(true);
    connect(infoListView_, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(contextMenu(const QPoint&)));
    if (props)
        make_item(props, infoListView_, 0, 0);
}

void DocInfoDialog::contextMenu(const QPoint& pos)
{       
    QTreeWidgetItem* item = infoListView_->itemAt(pos);
    if (!item)
        return;
    QString text = item->text(1);
    if (text.isEmpty())
        return;
    QMenu menu(this);
    menu.insertItem(tr("&Copy"));
    if (menu.exec(QCursor::pos()))
        serna_clipboard().setText(false, text);
}

/////////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(ShowDocInfoDialog, StructEditor)

bool ShowDocInfoDialog::doExecute(StructEditor* se, EventData*)
{
    DocInfoDialog d(se->widget(), ed_);
    return (QDialog::Accepted == d.exec());
}
