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
#ifndef SERNA_DOCVIEW_MESSAGE_TREE_H_
#define SERNA_DOCVIEW_MESSAGE_TREE_H_

#include "docview/dv_defs.h"
#include "common/String.h"
#include "common/XTreeNode.h"
#include "common/MessageUtils.h"
#include "grove/Origin.h"

class MessageTreeNode;
class CommandLink;

/*
 */
class DOCVIEW_EXPIMP MessageNodeWatcher {
public:
    MessageNodeWatcher(MessageTreeNode* msg);
    virtual ~MessageNodeWatcher();

    MessageTreeNode*    messageNode() const { return msgNode_; }

    virtual void        childInserted(MessageTreeNode* child) = 0;
    virtual void        childRemoved(MessageTreeNode* child) = 0;

protected:
    MessageTreeNode*    msgNode_;
};

/*
 */
class DOCVIEW_EXPIMP MessageTreeNode :
    public COMMON_NS::Messenger,
    public COMMON_NS::XTreeNode<MessageTreeNode, MessageTreeNode> {

public:
    typedef COMMON_NS::RefCntPtr<COMMON_NS::Message>    MessagePtr;
    typedef MessageNodeWatcher                          Watcher;

    MessageTreeNode(COMMON_NS::Message* = 0);
    MessageTreeNode(const COMMON_NS::String& name);
    virtual ~MessageTreeNode();

    //! Watcher settings
    Watcher*                getWatcher() const { return watcher_; }
    void                    setWatcher(Watcher* w) { watcher_ = w; }

    //! Acessors to messages
    const COMMON_NS::String& name() const { return name_; }
    COMMON_NS::Message*     getMessage() const { return message_.pointer(); }
    void                    setMessage(COMMON_NS::Message*);

    virtual MessageTreeNode*    copy(bool recursive = true) const;

    /// Copies children of this node to another node. Returns \a to.
    MessageTreeNode*            copyChildren(MessageTreeNode* to) const;

    void                        dump() const;

    const GroveLib::CompositeOrigin* findOrigin() const;
    Common::String              formatMessage() const;
    void                        clearMessagesFrom(int facility);

    void                        cleanEmpty();
    void                        appendWithFacility(MessageTreeNode*);

protected:
    //! Child notifications
    typedef MessageTreeNode Mtn;
    typedef Common::XList<Mtn, Mtn> MessageList;
    friend class Common::XListItem<Mtn, Mtn>;
    friend class Common::XList<Mtn, Mtn>;

    static void             notifyChildInserted(Mtn* m);
    static void             notifyChildRemoved(MessageList* parent,
                                               Mtn* child);

    //! Reference counting: use RefCounted interface
    static void             xt_incRefCnt(Mtn* m) { m->incRefCnt(); }
    static int              xt_decRefCnt(Mtn* m) { return m->decRefCnt(); }

private:
    virtual void            dispatch(MessagePtr& message);

protected:
    MessagePtr              message_;
    COMMON_NS::String       name_;
    MessageNodeWatcher*     watcher_;
};

typedef COMMON_NS::RefCntPtr<MessageTreeNode> MessageTreeNodePtr;

#endif // SERNA_DOCVIEW_MESSAGE_TREE_H_

