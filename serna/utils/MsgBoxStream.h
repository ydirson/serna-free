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

#ifndef MSG_BOX_STREAM_H_
#define MSG_BOX_STREAM_H_

#include "utils/utils_defs.h"
#include "common/common_types.h"
#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/String.h"

class QWidget;

/*
 */
template<typename T> struct MsgParam {
    MsgParam(T v) : value_(v) {}
    T value_;
};

/*
 */
struct MsgFacility : public MsgParam<uint32> {
    MsgFacility(uint32 f) : MsgParam<uint32>(f) {}
};

/*
 */
class UTILS_EXPIMP MsgBoxStream {
public:
    static MsgBoxStream make(QWidget* msgParentWidget);
    //!
    typedef COMMON_NS::MessageStreamItem Item;
    //!
    Item operator<<(const char* msgid);
    //!
    Item operator<<(const COMMON_NS::MessageStream::UintMessageIdBase& msgid);
    //!
    void setMsgParam(uint32 f) { facility_ = f; }
    //!
    COMMON_NS::Messenger* getMessenger() const { return messenger_.pointer(); }

    ~MsgBoxStream();
protected:
    MsgBoxStream(QWidget* msgParentWidget = 0);

private:
    COMMON_NS::MessengerPtr messenger_;
    uint32                  facility_;
};

/*
 */
template<typename T> inline MsgBoxStream&
operator<<(MsgBoxStream& mbs, const MsgParam<T>& msgp)
{
    mbs.setMsgParam(msgp.value_);
    return mbs;
}

UTILS_EXPIMP MsgBoxStream msgbox_stream(QWidget* = 0);
UTILS_EXPIMP void show_in_msgbox(COMMON_NS::Message* msg, QWidget* = 0);

#endif // MSG_BOX_STREAM_H_
