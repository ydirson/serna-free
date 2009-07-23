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
#include "ui/UiItemSearch.h"
#include "ui/UiAction.h"

#include "docview/impl/debug_dv.h"
#include "docview/MessageView.h"
#include "docview/MessageViewHolder.h"
#include "docview/MessageTree.h"
#include "docview/MessageTreeHolder.h"
#include "docview/GoToOriginEventData.h"
#include "common/MessageUtils.h"
#include "common/StringTokenizer.h"

#include "docview/qt/QtMessageList.h"
#include "utils/SernaMessages.h"

#include <QIcon>
#include <QApplication>
#include <QEvent>
#include <QContextMenuEvent>

using namespace Common;

SIMPLE_COMMAND_EVENT_IMPL(UpdateMessageView, MessageView)

MessageView::MessageView(Sui::Action* action, PropertyNode* properties)
    : Sui::Item(action, properties)
{
    setBool(Sui::IS_VISIBLE, false);
}

void MessageView::setMessageTree(MessageTreeNode* mtn)
{
    messageTree_ = mtn;
    messageView_ = new QtMessageList(mtn, this, gotoMsg_);
    messageView_->installEventFilter(this);
    for (mtn = mtn->firstChild(); mtn; mtn = mtn->nextSibling())
        messageView_->childInserted(mtn);
    update();
}

void MessageView::subscribeToUpdates(DynamicEventFactory& evf)
{
    evf.subscribe(makeCommand<UpdateMessageView>, this);
}

MessageView::~MessageView()
{
    delete messageView_;
}

MessageTreeNode* MessageView::selectedMessage() const
{
    if (messageView_)
        return messageView_->selectedMessage();
    return 0;
}

QWidget* MessageView::widget(const Sui::Item*) const
{
    return messageView_;
}

void MessageView::showContextMenu(const QPoint& pos)
{
    Sui::Item* menu = findItem(Sui::ItemName(NOTR("messageContextMenu")));
    if (menu)
        menu->showContextMenu(pos);
}

bool MessageView::eventFilter(QObject* watched, QEvent* e)
{
    if (watched == messageView_ && QEvent::ContextMenu == e->type()) {
        showContextMenu(static_cast<QContextMenuEvent*>(e)->globalPos());
        return true;
    }
    return false;
}

void MessageView::propertyChanged(PropertyNode* prop)
{
    if (isLockedNotify())
        return;
    Item::propertyChanged(prop);
    if (prop && Sui::ICON == prop->name())
        messageView_->headerItem()->setIcon(0, 
            Sui::icon_provider().getIconSet(get(Sui::ICON)));
}

void MessageView::emitPluginMessage(const String& pluginName,
                                    const String& msg,
                                    const GroveLib::Node* n) 
{
    MessageStream ms(SernaMessages::getFacility(), messageTree());
    for (StringTokenizer st(msg, "\n"); st; ) {
        String tok = st.next();
        if (tok.stripWhiteSpace().isEmpty())
            continue;
        if (n)
            ms << SernaMessages::pluginMessage << pluginName << tok 
                << GroveLib::CompositeOrigin(new GroveLib::NodeOrigin(n));
        else
            ms << SernaMessages::pluginMessage << pluginName << tok;
    }
}

void MessageView::update()
{
    messageView_->update();
}

//////////////////////////////////////////////////////////////////////

MessageViewHolder::MessageViewHolder(const Sui::Item* top,
                                     const MessageTreeHolder* mth)
    : topItem_(top), mth_(mth)
{
}

MessageView* MessageViewHolder::messageView(bool doFind) const
{
    if (messageView_ || !doFind || !topItem_)
        return messageView_.pointer();
    Sui::Item* message_item = 
        topItem_->findItem(Sui::ItemClass(Sui::MESSAGE_VIEW));
    if (message_item) 
        messageView_ = dynamic_cast<MessageView*>(message_item);
    if (messageView_ && mth_)
        messageView_->setMessageTree(mth_->messageTree());
    return messageView_.pointer();
}

void MessageViewHolder::setMessageTreeHolder(const MessageTreeHolder* mth)
{
    mth_ = mth;
}

MessageViewHolder::~MessageViewHolder()
{
}

//////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(GoToMessageOrigin, MessageView)

bool GoToMessageOrigin::doExecute(MessageView* mv, EventData*)
{
    MessageTreeNode* mtn = mv->selectedMessage();
    if (0 == mtn)
        return false;
    const GroveLib::CompositeOrigin* co = mtn->findOrigin();
    if (0 == co)
        return false;
    GoToOriginEventData ed(*co);
    mv->gotoMessage().dispatchEvent(&ed);
    return true;
}

//////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(ClearMessages, MessageView)

bool ClearMessages::doExecute(MessageView* mv, EventData*)
{
    mv->messageTree()->removeAllChildren();
    mv->update();
    return true;
}

bool UpdateMessageView::doExecute(MessageView* mv, EventData*)
{
    mv->update();
    return true;
}

namespace Sui {

CUSTOM_ITEM_MAKER(MessageView, MessageView)

} // namespace Sui
