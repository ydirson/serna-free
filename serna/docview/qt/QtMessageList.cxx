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
#include "ui/IconProvider.h"
#include "ui/UiAction.h"

#include "utils/message_utils.h"

#include "docview/impl/debug_dv.h"
#include "docview/qt/QtMessageList.h"
#include "docview/MessageView.h"
#include "docview/GoToOriginEventData.h"
#include "docview/Clipboard.h"

#include <QApplication>
#include <QPixmap>
#include <QIcon>
#include <QHeaderView>

#define MAX_MSG_CHILDREN 100

using namespace Sui;
using namespace Common;

static QtMessageListItem* msg_item(MessageTreeNode* node)
{
    if (node && node->getWatcher())
        return static_cast<QtMessageListItem*>(node->getWatcher());
    return 0;
}

static void format_item(MessageTreeNode* node, QTreeWidgetItem* item)
{
    item->setText(0, node->formatMessage());
    item->setToolTip(0, item->text(0));
    if (!node->getMessage())
        return;
    String msg_icon;
    switch (node->getMessage()->severityLevel()) {
        case Message::L_ERROR :   msg_icon = NOTR("error"); break;
        case Message::L_WARNING : msg_icon = NOTR("warning"); break;
        case Message::L_INFO :    msg_icon = NOTR("info"); break;
        default:
            msg_icon = NOTR("info");
    }
    item->setIcon(0, Sui::icon_provider().getPixmap(msg_icon));
}

static void make_item(MessageTreeNode* child)
{
    QTreeWidgetItem* parent = msg_item(child->parent());
    if (0 == parent)
        return;
    QTreeWidgetItem* after = msg_item(child->prevSibling());
    QTreeWidgetItem* item = (after)
        ? new QtMessageListItem(child, parent, after)
        : new QtMessageListItem(child, parent);
    format_item(child, item);

    child = child->firstChild();
    for (int i = 0; child && i < MAX_MSG_CHILDREN; 
        ++i, child = child->nextSibling())
            make_item(child);
    item->setExpanded(true);
}

////////////////////////////////////////////////////////////////////////

QtMessageList::QtMessageList(MessageTreeNode* msgRoot, Sui::Item* item,
                             DynamicEventFactory& gotoMsgFactory)
    : MessageNodeWatcher(msgRoot),
      item_(item),
      gotoMsg_(gotoMsgFactory)
{
    setColumnCount(1);
    header()->setClickable(false);
    header()->setDefaultAlignment(Qt::AlignLeft);
    header()->setResizeMode(QHeaderView::Stretch);
    setSortingEnabled(false);
    setRootIsDecorated(true);
    headerItem()->setText(0, tr("Message"));
    headerItem()->setIcon(0, Sui::icon_provider().
        getIconSet(item_->get(Sui::ICON)));

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            SLOT(messageClick(QTreeWidgetItem*, int)));
    setWordWrap(true);
}

MessageTreeNode* QtMessageList::selectedMessage() const
{
    QList<QTreeWidgetItem*> items = selectedItems();
    if (!items.size())
        return 0;
    QtMessageListItem* selected_item =
        static_cast<QtMessageListItem*>(items[0]);
    if (selected_item)
        return selected_item->messageNode();
    return 0;
}

void QtMessageList::messageClick(QTreeWidgetItem* item, int) const
{
    if (0 == item)
        return;
    MessageTreeNode* mtn =
        static_cast<QtMessageListItem*>(item)->messageNode();
    if (0 == mtn)
        return;
    const GroveLib::CompositeOrigin* co = mtn->findOrigin();
    if (0 == co)
        return;
    GoToOriginEventData ed(*co);
    gotoMsg_.dispatchEvent(&ed);
}

void QtMessageList::update() 
{
    DBG(DV.TEST) << "MessageList update\n";
    MessageEvent* event = eventQueue_.firstChild();
    if (0 == event)
        return; // nothing to do
    QTreeWidgetItem* last_inserted = 0;
    while ((event = eventQueue_.firstChild())) {
        switch (event->type_) {
            case MessageEvent::INSERTED: {
                if (event->node_ && !event->node_->parent()) 
                    break;
                MessageTreeNode* child = event->node_.pointer();
                QTreeWidgetItem* after = msg_item(child->prevSibling());
                QTreeWidgetItem* item = (after)
                    ? new QtMessageListItem(child, this, after)
                    : new QtMessageListItem(child, this);
                format_item(child, item);
                child = child->firstChild();
                for (int i = 0; child && i < MAX_MSG_CHILDREN; 
                    ++i, child = child->nextSibling())
                        make_item(child);
                last_inserted = item;
                break;
            }
            case MessageEvent::REMOVED: {
                delete msg_item(event->node_.pointer());
                break;
            }
            case MessageEvent::SUBNODE_INSERTED: {
                if (event->node_ && !event->node_->parent())
                    break;
                make_item(event->node_.pointer());
                break;
            }
            case MessageEvent::SUBNODE_REMOVED: {
                delete msg_item(event->node_.pointer());
                break;
            }
            default:
                break;
        }
        delete event;
    }
    clearSelection();
    const uint total_cnt = topLevelItemCount();
    for (uint i = 0; i < total_cnt; ++i)
        topLevelItem(i)->setExpanded(i != (total_cnt - 1));
    if (last_inserted || total_cnt == 1) {
        if (!last_inserted)
            last_inserted = topLevelItem(0);
        scrollToItem(last_inserted);
        setCurrentItem(last_inserted);
        last_inserted->setSelected(true);
        last_inserted->setExpanded(true);
    }
    item_->property(Sui::IS_VISIBLE)->setBool(total_cnt);
}
    
void QtMessageList::addEvent(MessageTreeNode* n, MessageEvent::TransType t)
{
    eventQueue_.appendChild(new MessageEvent(n, t));
}

void QtMessageList::childInserted(MessageTreeNode* child)
{
    addEvent(child, MessageEvent::INSERTED);
}

void QtMessageList::childRemoved(MessageTreeNode* child)
{
    addEvent(child, MessageEvent::REMOVED);
}

////////////////////////////////////////////////////////////////////////

QtMessageListItem::QtMessageListItem(MessageTreeNode* msg,
                                     QTreeWidget* listView)
    : QTreeWidgetItem(listView),
      MessageNodeWatcher(msg)
{
}

QtMessageListItem::QtMessageListItem(MessageTreeNode* msg,
                                     QTreeWidget* listView,
                                     QTreeWidgetItem* after)
    : QTreeWidgetItem(listView, after),
      MessageNodeWatcher(msg)
{
}

QtMessageListItem::QtMessageListItem(MessageTreeNode* msg,
                                     QTreeWidgetItem* parent)
    : QTreeWidgetItem(parent),
      MessageNodeWatcher(msg)
{
}

QtMessageListItem::QtMessageListItem(MessageTreeNode* msg,
                                     QTreeWidgetItem* parent,
                                     QTreeWidgetItem* after)
    : QTreeWidgetItem(parent, after),
      MessageNodeWatcher(msg)
{
}

void QtMessageListItem::childInserted(MessageTreeNode* child)
{
    static_cast<QtMessageList*>(treeWidget())->addEvent(child,
        QtMessageList::MessageEvent::SUBNODE_INSERTED);
}

void QtMessageListItem::childRemoved(MessageTreeNode* child)
{
    static_cast<QtMessageList*>(treeWidget())->addEvent(child,
        QtMessageList::MessageEvent::SUBNODE_REMOVED);
}

//////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(CopyMessage, MessageView)

bool CopyMessage::doExecute(MessageView* mv, EventData*)
{
    MessageTreeNode* mtn = mv->selectedMessage();
    if (0 == mtn)
        return false;
    serna_clipboard().setText(false, mtn->formatMessage());
    serna_clipboard().setText(true, mtn->formatMessage());
    return true;
}
