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
/*! \file
 *  Core of messaging framework: implementation
 */
#include "common/common_defs.h"
#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/String.h"
#include "common/StrdupNew.h"
#include "common/printf_fmt.h"
#include "common/asserts.h"

#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <QCoreApplication>

#if defined(_MSC_VER) && !defined(SFWORKS_STATIC)
# if defined(__ICL)
#  define EXPORT_PARTIAL_SPEC __declspec(dllexport)
// warning 697: declaration modifiers are incompatible with previous declaration
#  pragma warning (disable: 697)
# else
#  define EXPORT_PARTIAL_SPEC template<> __declspec(dllexport)
# endif
#else
# define EXPORT_PARTIAL_SPEC template<>
#endif

COMMON_NS_BEGIN

using namespace std;

String MessageArgBase::format() const
{
    return String::null();
}

MessageArgBase::MessageArgBase(MessageArgType t, bool ctx, char* fmod)
  : fmod_(strdup_new(fmod)), isContext_(ctx),
    free_fmod_(true), type_(t)
{
}

MessageArgBase::MessageArgBase(const MessageArgBase& other)
{
    this->operator=(other);
}

MessageArgBase& MessageArgBase::operator=(const MessageArgBase& other)
{
    type_ = other.type_;
    isContext_ = other.isContext_;
    if (free_fmod_)
        delete[] fmod_;
    free_fmod_ = other.free_fmod_;
    if (free_fmod_)
        fmod_ = strdup_new(other.fmod_);
    else
        fmod_ = other.fmod_;
    return *this;
}

MessageArgBase::~MessageArgBase()
{
    if (free_fmod_)
        delete[] fmod_;
}

const char* Message::severityLevelName() const
{
    const char* const severityNames[] = {
        "NONE", "INFO", "WARNING", "ERROR", "FATAL"
    };
    if (level_ >= MAX_SEVERITY_LEVEL)
        return "<Unknown Severity>";
    return severityNames[unsigned(level_)];
}

String Message::format(const MessageFetcher& mf) const
{
    String ts, os;
    Vector<const MessageArgBase*> av;
    ulong i = 0;

    for (i = 0; i < nArgs(); ++i) {
        if (!getArg(i)->isContext())
            av.push_back(getArg(i));
    }
    if (!fetchString(mf, ts)) {
        os = "<UNDEFINED MESSAGE>";
        if (av.size() > 0)
            os += ':';
        for (i = 0; i < av.size(); ++i) {
            os += ' ';
            os += av[i]->format();
        }
        os += '\n';
        return os;
    }
//    if (qApp)
    ts = QCoreApplication::translate("msg", ts.utf8().c_str());
    os.truncate(0);
    Char as('%');
    const Char* bs = ts.unicode();
    const Char* es = bs + ts.length();
    bool gotas = 0;
    for (; bs < es; bs++) {
        if (*bs == as) {
            if (gotas) {
                os += as;
                gotas = false;
            } else
                gotas = true;
            continue;
        }
        if (!gotas) {
            os += *bs;
            continue;
        }
        gotas = 0;
        if (!bs->isNumber()) {
            os += as;
            os += *bs;
            continue;
        }
        i = bs->digitValue();
        if (i >= av.size())
            os += "<bad argument number>";
        else
            os += av[i]->format();
    }
    return os;
}

Message::Message(MessageClass mclass, uint32 facility, SeverityLevel lv)
  : facility_(facility), mclass_(mclass), level_(lv)
{
}

Message::~Message()
{
}

UintIdMessage::UintIdMessage(uint32 msgid, uint32 facility)
 :  Message(UINT_ID_MESSAGE, facility), messageId_(msgid)
{
}

UintIdMessage::~UintIdMessage()
{
}

bool UintIdMessage::fetchString(const MessageFetcher& mf, String& os) const
{
    return mf.fetchString(facility(), messageId_, os);
}

bool CstrIdMessage::fetchString(const MessageFetcher& mf, String& os) const
{
    return mf.fetchString(facility(), messageId_, os);
}

CstrIdMessage::CstrIdMessage(const char* msgid, uint32 facility)
  : Message(CSTR_ID_MESSAGE, facility),
    messageId_(const_cast<char*>(msgid)), free_msgid_(false) {}

CstrIdMessage::CstrIdMessage(char* msgid, uint32 facility)
  : Message(CSTR_ID_MESSAGE, facility),
    messageId_(strdup_new(msgid)), free_msgid_(true)
{
}

CstrIdMessage::~CstrIdMessage()
{
    if (free_msgid_)
        delete[] messageId_;
}

//
// Implementation of MessageArgs for builtin types
//
#define FORMAT_NUMERIC_MSGARG(t) \
EXPORT_PARTIAL_SPEC String MessageArg<t>::format() const \
  { \
      if (formatModifier()) { \
        char nbuf[24]; \
        sprintf(nbuf, formatModifier(), value_); \
        return String(nbuf); \
      } else \
        return String::number(value_); \
  }

FORMAT_NUMERIC_MSGARG(int32)
FORMAT_NUMERIC_MSGARG(uint32)
FORMAT_NUMERIC_MSGARG(float)
FORMAT_NUMERIC_MSGARG(double)

#if defined(_MSC_VER) && !defined(__ICL) || defined(__SUNPRO_CC)
FORMAT_NUMERIC_MSGARG(int)
#endif //_MSC_VER

EXPORT_PARTIAL_SPEC String MessageArg<int64>::format() const
{
    char buf[24];
    if (formatModifier())
        sprintf(buf, formatModifier(), value_);
    else
        sprintf(buf, "%" PRINTF_INT64_FMT, value_);
    return String(buf);
}

EXPORT_PARTIAL_SPEC String MessageArg<uint64>::format() const
{
    char buf[24];
    if (formatModifier())
        sprintf(buf, formatModifier(), value_);
    else
        sprintf(buf, "%" PRINTF_UINT64_FMT, value_);
    return String(buf);
}

EXPORT_PARTIAL_SPEC String MessageArg<String>::format() const
{
    return value_;
}

#ifdef EOL_USED
EXPORT_PARTIAL_SPEC String MessageArg<QuadInt>::format() const
{
    if (value_.isNaN())
        return String("NaN");
    char buf[24];
    if (formatModifier())
        sprintf(buf, formatModifier(), value_.getValue());
    else
        sprintf(buf, "%" PRINTF_INT64_FMT, value_.getValue());
    return String(buf);
}

EXPORT_PARTIAL_SPEC String MessageArg<FixedDecimal>::format() const
{
    String s;
    value_.toString(s);
    return s;
}

EXPORT_PARTIAL_SPEC String MessageArg<IPAddress>::format() const
{
    String s;
    value_.toString(s);
    return s;
}

EXPORT_PARTIAL_SPEC String MessageArg<Cookie>::format() const
{
    String s;
    value_.toString(s);
    return s;
}
#endif // EOL_USED

MessageStreamItem& MessageStreamItem::operator<<(const char* v)
{
    this->operator<<(String(v));
    return *this;
}

MessageStreamItem& MessageStreamItem::operator<<(char* v)
{
    this->operator<<(String(v));
    return *this;
}

MessageStreamItem::~MessageStreamItem()
{
    if (messenger_)
        messenger_->dispatch(message_);
}

MessageStreamItem MessageStream::operator<<(const String& s)
{
    return MessageStreamItem(messenger_.pointer(),
        new CstrIdMessage(strdup_new(s.latin1().c_str()), facility_));
}

void MessageStream::setMessenger(Messenger* m)
{
    messenger_ = m;
}

MessageStream::MessageStream(uint32 facility, Messenger* m)
  : facility_(facility)
{
    if (0 == m)
        messenger_ = new DefaultMessenger;
    else
        messenger_ = m;
}

MessageStream::~MessageStream()
{
}

class MessageBlock::MbMessenger : public Messenger {
public:
    MbMessenger(MessageBlock* mb)
      : mb_(mb) {}
    virtual void        dispatch(RefCntPtr<Message>& message);
    virtual Messenger*  copy() const { return 0; } // no copy

private:
    MessageBlock* mb_;
};

MessageBlock::MessageBlock(uint32 facility, Messenger* m,
                           Message::SeverityLevel minSv)
  : MessageStream(facility, new MbMessenger(this)),
    messenger_(m), minSeverity_(minSv),
    maxMsgBuf_(DEFAULT_MAX_MSGBUF_SIZE)
{
}

void MessageBlock::MbMessenger::dispatch(RefCntPtr<Message>& m)
{
    MessageBuffer& mb = mb_->messages_;
    if (mb.size() < mb_->maxMsgBuf_) {
        mb.push_back(m);
        return;
    }
    // find a room for new message
    for (uint i = 0; i < mb.size(); ++i) {
        if (mb[i]->severityLevel() < m->severityLevel()) {
            mb.erase(i);
            mb.push_back(m);
            return;
        }
    }
    // no room - discard last message (first messages are more useful
    // because they are more often express the true reason for errors
    // than the following messages, which may be derivative).
    mb.erase(mb.size() - 1);
    mb.push_back(m);
}

void MessageBlock::setMaxMsgBuf(uint size)
{
    if (!size)
        maxMsgBuf_ = DEFAULT_MAX_MSGBUF_SIZE;
    else
        maxMsgBuf_ = size;
}

void MessageBlock::dispatch(bool logAll)
{
    for (uint i = 0; i < messages_.size(); ++i) {
        RefCntPtr<Message>& mp = messages_[i];
        const Message::SeverityLevel lvl = mp->severityLevel();
        if (logAll || lvl >= minSeverity_)
            messenger_->dispatch(mp);
        if (lvl == Message::L_FATAL) {
            MessageFetcher& mf = BuiltinMessageFetcher::instance();
            nstring s(local_8bit(mp->format(mf)));
            std::cerr << "FATAL: " << s << std::endl;
            RT_MSG_ABORT("Got message with FATAL severity -- Aborting");
            /* NOTREACHED */
        }
    }
    messages_.clear();
}

void MessageBlock::clear()
{
    messages_.clear();
}

MessageBlock::~MessageBlock()
{
    dispatch();
}

COMMON_NS_END
