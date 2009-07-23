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
#ifndef SERNA_MESSAGE_VIEW_H_
#define SERNA_MESSAGE_VIEW_H_

#include "docview/MessageTree.h"
#include "common/OwnerPtr.h"
#include "common/CommandEvent.h"

#include "ui/UiProps.h"
#include "ui/UiItem.h"

#include "utils/SernaUiItems.h"

#include <qobject.h>
#include <qpointer.h>
//Added by qt3to4:
#include <QEvent>

namespace GroveLib {
    class Node;
}

class QtMessageList;
class QEvent;

class DOCVIEW_EXPIMP MessageView : public QObject,
                                   public Sui::Item,
                                   public Common::CommandEventContext {
public:
    MessageView(Sui::Action*, PropertyNode* properties);
    virtual ~MessageView();

    void                setMessageTree(MessageTreeNode*);
    MessageTreeNode*    messageTree() const { return messageTree_.pointer(); }
    MessageTreeNode*    selectedMessage() const;
    void                subscribeToUpdates(Common::DynamicEventFactory&);

    Common::DynamicEventFactory& gotoMessage() { return gotoMsg_; }

    virtual String      itemClass() const { return Sui::MESSAGE_VIEW; }
    virtual String      widgetClass() const { return Sui::TERMINAL_WIDGET; }

    virtual QWidget*    widget(const Sui::Item* child) const;
    virtual void        showContextMenu(const QPoint& pos);

    void                update();

    /// Put plugin message to message box
    void                emitPluginMessage(const String& pluginName,
                                          const String& msg,
                                          const GroveLib::Node* n = 0);
protected:
    virtual bool        eventFilter(QObject* watched, QEvent* e);
    virtual void        propertyChanged(Common::PropertyNode* prop);

private:
    MessageView& operator=(const MessageView&);
    MessageView(const MessageView&);
    
    MessageTreeNodePtr              messageTree_;
    QPointer<QtMessageList>         messageView_;
    Common::DynamicEventFactory     gotoMsg_;
};

#endif // SERNA_MESSAGE_VIEW_H_


