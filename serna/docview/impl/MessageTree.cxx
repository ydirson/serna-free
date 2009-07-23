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
#include "docview/MessageTree.h"
#include "docview/MessageTreeHolder.h"
#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/Url.h"
#include "grove/Node.h"
#include "grove/Grove.h"
#include "utils/message_utils.h"
#include "docview/impl/debug_dv.h"

USING_COMMON_NS;

/*
 */
MessageNodeWatcher::MessageNodeWatcher(MessageTreeNode* msgNode)
    : msgNode_(msgNode)
{
    msgNode_->setWatcher(this);
}

MessageNodeWatcher::~MessageNodeWatcher()
{
    msgNode_->setWatcher(0);
}

/*
 */
MessageTreeNode::MessageTreeNode(Message* m)
    : message_(m),
      watcher_(0)
{
}

MessageTreeNode::MessageTreeNode(const String& name)
    : name_(name),
      watcher_(0)
{
}

MessageTreeNode::~MessageTreeNode()
{
}

void MessageTreeNode::setMessage(Message* m)
{
    message_ = m;
}

MessageTreeNode* MessageTreeNode::copyChildren(MessageTreeNode* to) const
{
    const MessageTreeNode* cn = firstChild();
    for (; cn; cn = cn->nextSibling())
        to->appendChild(cn->copy(true));
    return to;
}

MessageTreeNode* MessageTreeNode::copy(bool recursive) const
{

    MessageTreeNode* n = new MessageTreeNode(message_.pointer());
    if (false == recursive)
        return n;
    return copyChildren(n);
}

void MessageTreeNode::notifyChildInserted(MessageTreeNode* msg)
{
    if (msg->parent() && msg->parent()->watcher_)
        msg->parent()->watcher_->childInserted(msg);
}

void MessageTreeNode::notifyChildRemoved(MessageList* parent,
                                         MessageTreeNode* msg)
{
    if (static_cast<MessageTreeNode*>(parent)->watcher_)
        static_cast<MessageTreeNode*>(parent)->watcher_->childRemoved(msg);
}

void MessageTreeNode::dump() const
{
    DBG_IF(DV.MTREE) {
        DBG(DV.MTREE) << "MTN " << this << ": ";
        if (!message_.isNull())
            DBG(DV.MTREE) << "[" << message_->facility() << "] "
                 << message_->format(BuiltinMessageFetcher::instance());
        DBG(DV.MTREE) << std::endl;
        DDINDENT;
        const MessageTreeNode* mtn = firstChild();
        for (; mtn; mtn = mtn->nextSibling())
            mtn->dump();
    }
}

void MessageTreeNode::cleanEmpty()
{
    MessageTreeNode* mtn = firstChild();
    while (mtn) {
        MessageTreeNode* next = mtn->nextSibling();
        if (!mtn->firstChild())
            mtn->remove();
        mtn = next;
    }
}

void MessageTreeNode::clearMessagesFrom(int facility)
{
    MessageTreeNode* mtn = firstChild();
    while (mtn) {
        MessageTreeNode* next = mtn->nextSibling();
        if (mtn->getMessage() && mtn->getMessage()->facility() ==
            (uint)facility) {
                mtn->remove();
        } 
        else {
            MessageTreeNode* child = mtn->firstChild();
            while (child) {
                MessageTreeNode* nextChild = child->nextSibling();
                if (child->getMessage() &&
                    child->getMessage()->facility() == (uint)facility)
                        child->remove();
                child = nextChild;
            }
            if (!mtn->firstChild())
                mtn->remove();
        }
        mtn = next;
    }
}



void MessageTreeNode::appendWithFacility(MessageTreeNode* mtn)
{
    if (!mtn->getMessage()) {
        appendChild(mtn);
        return;
    }
    const uint facility = mtn->getMessage()->facility();
    MessageTreeNode* tn = lastChild();
    for (; tn; tn = tn->prevSibling()) {
        const MessageTreeNode* tn2 = tn;
        while (!tn2->getMessage() && tn2->firstChild())
            tn2 = tn2->firstChild();
        if (tn2 && tn2->getMessage() &&
                tn2->getMessage()->facility() == facility) {
            tn = tn2->parent();
            break;
        }
    }
    if (tn)
        tn->appendChild(mtn);
    else {
        String facility_name;
        switch (facility) {
            case 1 : /* FALL THROUGH */
            case 2 : facility_name = NOTR("XML"); break;
            case 3 : facility_name = NOTR("Formatter"); break;
            case 4 : facility_name = NOTR("XPath"); break;
            case 5 : facility_name = NOTR("XSLT"); break;
            case 6 : facility_name = NOTR("Validator"); break;
            case 10 :
            default:
                facility_name = NOTR("Serna"); break;
        }
        appendChild(new MessageTreeNode(facility_name));
        lastChild()->appendChild(mtn);
    }
}

void MessageTreeNode::dispatch(COMMON_NS::RefCntPtr<Message>& message)
{
    DBG(DV.MTREE) << "MTREE DISPATCH: "
                  << message->format(BuiltinMessageFetcher::instance())
                  << std::endl;
    appendWithFacility(new MessageTreeNode(message.pointer()));
}

const GroveLib::CompositeOrigin* MessageTreeNode::findOrigin() const
{
    Message* m = getMessage();
    const GroveLib::CompositeOrigin* co = 0;
    if (m) {
        co = GroveLib::getCompositeOrigin(m);
        if (co)
            return co;
    }
    for (MessageTreeNode* n = firstChild(); n; n = n->nextSibling()) {
        co = n->findOrigin();
        if (co)
            return co;
    }
    return 0;
}

String MessageTreeNode::formatMessage() const 
{
    if (!getMessage())
        return name();
    const GroveLib::CompositeOrigin* co = findOrigin();
    if (co && co->docOrigin()) {
        const GroveLib::Node* n = co->docOrigin()->node();
        if (n && n->grove() && n->grove()->parent()) {
            return "(" + Url(n->grove()->topSysid())[Url::FILENAME] + ") " +
                MessageUtils::msg_str(getMessage());
        }
    }
    return MessageUtils::msg_str(getMessage());
}

//////////////////////////////////////////////////////////////////

MessageTreeHolder::MessageTreeHolder(MessageTreeNode* mtn)
    : messageTree_(mtn ? mtn : new MessageTreeNode)
{
}

MessageTreeHolder::~MessageTreeHolder()
{
}
