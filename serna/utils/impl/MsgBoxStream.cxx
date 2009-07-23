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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#include "utils/utils_defs.h"
#include "utils/message_utils.h"
#include "utils/MsgBoxStream.h"
#include "SernaMessages.h"
#include "common/MessageUtils.h"
#include "common/Message.h"
#include "common/String.h"
#include "common/StringCvt.h"
#include "common/Singleton.h"
#include <QMessageBox>
#include <QApplication>

#include <iostream>

USING_COMMON_NS

class MsgBoxMessenger : public Messenger {
public:
    MsgBoxMessenger(QWidget* parentWidget) : msgParentWidget_(parentWidget) {}
    virtual void        dispatch(RefCntPtr<Message>& message);
    virtual Messenger*  copy() const { return new MsgBoxMessenger(*this); }
    //!
    void setParentWidget(QWidget* pw) { msgParentWidget_ = pw; }
    //!
    virtual ~MsgBoxMessenger() {}
private:
    QWidget* msgParentWidget_;
};

void MsgBoxMessenger::dispatch(RefCntPtr<Message>& msg)
{
    show_in_msgbox(msg.pointer(), msgParentWidget_);
}

MsgBoxStream::MsgBoxStream(QWidget* msgParentWidget)
 :  messenger_(new MsgBoxMessenger(msgParentWidget)),
    facility_(SernaMessages::getFacility())
{
}

MsgBoxStream::~MsgBoxStream() {}

MsgBoxStream::Item MsgBoxStream::operator<<(const char* msgid)
{
    return Item(&*messenger_, new CstrIdMessage(msgid, facility_));
}

MsgBoxStream::Item
MsgBoxStream::operator<<(const MessageStream::UintMessageIdBase& msgid)
{
    return Item(&*messenger_, new UintIdMessage(msgid.id, facility_));
}

MsgBoxStream MsgBoxStream::make(QWidget* msgParentWidget)
{
    return MsgBoxStream(msgParentWidget);
}

MsgBoxStream msgbox_stream(QWidget* msgParentWidget)
{
    return MsgBoxStream::make(msgParentWidget);
}

void show_in_msgbox(Message* msg, QWidget* parentWidget)
{
    if (0 == msg)
        return;

    const Message::SeverityLevel lvl = msg->severityLevel();
    QString qstr = MessageUtils::msg_str(msg);
    QApplication::restoreOverrideCursor();
    QString caption;
    QMessageBox::Icon icon = QMessageBox::NoIcon;
    switch (lvl) {
        case Message::L_INFO:
        case Message::L_NONE:
            caption = tr("Notice");
            icon = QMessageBox::Information;
            break;
        case Message::L_WARNING:
            caption = tr("Warning");
            icon = QMessageBox::Warning;
            break;
        case Message::L_ERROR:
            caption = tr("Error");
            icon = QMessageBox::Critical;
            break;
        case Message::L_FATAL:
            // TODO handle that
            break;
        default:
            RT_ASSERT(!NOTR("unreachable code"));
    };
    QMessageBox msgBox(caption, qstr, icon, QMessageBox::Ok,
                       QMessageBox::NoButton, QMessageBox::NoButton,
                       parentWidget, NOTR("msg-box-stream"), true);
    (void) msgBox.exec();
}
