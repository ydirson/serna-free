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
#ifndef SERNA_MESSAGE_LIST_H_
#define SERNA_MESSAGE_LIST_H_

#include "docview/MessageTree.h"
#include "common/CommandEvent.h"

#include <QTreeWidget>

namespace Sui {
    class Item;
};

class QtMessageList : public QTreeWidget, 
                      public MessageNodeWatcher {
    Q_OBJECT
public:
    class MessageEvent : public Common::XListItem<MessageEvent> {
    public:
        enum TransType { INSERTED, REMOVED, SUBNODE_INSERTED, SUBNODE_REMOVED };
        Common::RefCntPtr<MessageTreeNode> node_;
        TransType                          type_;
        MessageEvent(MessageTreeNode* n, TransType t)
            : node_(n), type_(t) {}
    };
    typedef Common::XList<MessageEvent> MessageEventList;
    
    QtMessageList(MessageTreeNode* msgRoot, Sui::Item* item,
                  Common::DynamicEventFactory& goToOriginFactory);
    virtual ~QtMessageList() {};

    MessageTreeNode*    selectedMessage() const;
    virtual void        childInserted(MessageTreeNode* child);
    virtual void        childRemoved(MessageTreeNode* child);
    void                update();
    void                addEvent(MessageTreeNode*, MessageEvent::TransType);


protected slots:
    void                messageClick(QTreeWidgetItem* item, 
                                     int column) const;
private:
    Sui::Item*                      item_;
    Common::DynamicEventFactory&    gotoMsg_;
    MessageEventList                eventQueue_;
};

class QtMessageListItem : public QTreeWidgetItem, public MessageNodeWatcher {
public:
    QtMessageListItem(MessageTreeNode* msg, QTreeWidget* listView);
    QtMessageListItem(MessageTreeNode* msg, QTreeWidget* listView,
                      QTreeWidgetItem* after);

    QtMessageListItem(MessageTreeNode* msg, QTreeWidgetItem* parent);
    QtMessageListItem(MessageTreeNode* msg, QTreeWidgetItem* parent,
                      QTreeWidgetItem* after);

    virtual ~QtMessageListItem() {};

    virtual void        childInserted(MessageTreeNode* child);
    virtual void        childRemoved(MessageTreeNode* child);
};

#endif // SERNA_MESSAGE_LIST_H_


