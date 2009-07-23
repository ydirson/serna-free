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
 *  Core of messaging framework
 */
#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "common/common_defs.h"
#include "common/String.h"
#include "common/StringCvt.h"
#include "common/common_types.h"
#include "common/Vector.h"
#include "common/OwnerPtr.h"
#include "common/RefCounted.h"
#include "common/RefCntPtr.h"
#include "common/MessageUtils.h"

#if defined(_MSC_VER) && !defined(__ICL)
# pragma warning (push)
# pragma warning (disable : 4251)
#endif

COMMON_NS_BEGIN

class Messenger;
class MessageFetcher;
class FixedDecimal;
class IPAddress;
class QuadInt;
class Cookie;

/*! A common interface for message arguments. If isContext() is true,
 *  then this argument is a context one (it is not formatted
 *  with Message::format() function).
 */
class COMMON_EXPIMP MessageArgBase {
public:
    enum MessageArgType {
        UNDEFINED_MT = 0, INT32, UINT32, INT64, UINT64, FLOAT,
        DOUBLE, QUAD_INT, FIXED_DECIMAL, STRING, IPADDRESS, COOKIE,
        USER_DEFINED
    };
    /// returns message arg type. For not-builtin types, return USER_DEFINED.
    MessageArgType type() const { return MessageArgType(type_); }

    /// Format argument into a text string.
    virtual String format() const;

    /// Deep copy message argument.
    virtual MessageArgBase* copy() const = 0;

    /// Returns true if this argument is contextual (shouldn't be formatted)
    bool    isContext() const { return isContext_; }

    /// Returns format modifier (if any)
    const char* formatModifier() const { return fmod_; }

    /// Initialize message arg base, possibly with format modifier fmod.
    /// fmod is copied
    MessageArgBase(MessageArgType t, bool ctx, char* fmod);

    /// Initialize message arg base, possibly with format modifier fmod.
    /// fmod is used by reference only.
    MessageArgBase(MessageArgType t, bool ctx, const char* fmod)
      : fmod_(const_cast<char*>(fmod)), isContext_(ctx),
        free_fmod_(false), type_(t) {}

    MessageArgBase(const MessageArgBase&);
    MessageArgBase& operator=(const MessageArgBase&);

    virtual ~MessageArgBase();

public:
    char* fmod_;
    bool  isContext_;
    bool  free_fmod_;
    char  type_;
};

//
// The above mess is because of M$VC6, which does not support
// partial template instantiations, and, therefore, typelists.
//

/// @if MESSAGE_MESS
namespace MessagePrivate {
#define MESSAGEARG_TYPE_MAPPER__(type, id) \
    template <> struct MessageArgTypeHelper<type> { \
        enum { result = MessageArgBase::id }; \
    };
    template <class T> struct MessageArgTypeHelper {
        enum { result = T::messageArgType };
    };
#if defined(_MSC_VER) && !defined(__ICL) || defined __SUNPRO_CC
    MESSAGEARG_TYPE_MAPPER__(int, INT32)
#endif
    MESSAGEARG_TYPE_MAPPER__(int32,  INT32)
    MESSAGEARG_TYPE_MAPPER__(uint32, UINT32)
    MESSAGEARG_TYPE_MAPPER__(int64,  INT64)
    MESSAGEARG_TYPE_MAPPER__(uint64, UINT64)
    MESSAGEARG_TYPE_MAPPER__(float,  FLOAT)
    MESSAGEARG_TYPE_MAPPER__(double, DOUBLE)
    MESSAGEARG_TYPE_MAPPER__(String, STRING)
}
#undef MESSAGEARG_TYPE_MAPPER__
// end of mess
/// @endif

/*! Helper template for implementation of concrete built-in MessageArgs.
 *  Users may add custom types by implementing MessageArg<T>::format()
 *  function, and providing enum messageArgType in the custom class which
 *  should be larger than MessageArgBase::USER_DEFINED.
 *  The constraint on all types kept in MessageArg<T> is that T must
 *  be copiable.
 */
template <class T> class MessageArg : public MessageArgBase {
public:
    enum { messageArgType = MessagePrivate::MessageArgTypeHelper<T>::result };

    /// returns original value
    const T& value() const { return value_; }

    virtual String format() const;
    virtual MessageArgBase* copy() const {
        return new MessageArg(*this);
    }
    MessageArg(const T& value, bool isContext, char* fmod)
      : MessageArgBase(MessageArgType(messageArgType),
        isContext, fmod), value_(value) {}
    MessageArg(const T& value, bool isContext, const char* fmod)
      : MessageArgBase(MessageArgType(messageArgType),
        isContext, fmod), value_(value) {}
    MessageArg(const T& value, bool isContext = false)
      : MessageArgBase(MessageArgType(messageArgType),
        isContext, (const char*)0), value_(value) {}

    virtual ~MessageArg() {}

private:
    void fmt_num(String&) const; // helper - used for numeric types only
    T value_;
};

#define FORMAT_DECL(type) \
template<> COMMON_EXPIMP String MessageArg<type>::format() const

FORMAT_DECL(int32);
FORMAT_DECL(uint32);
FORMAT_DECL(int64);
FORMAT_DECL(uint64);
FORMAT_DECL(float);
FORMAT_DECL(double);
FORMAT_DECL(String);
#if defined(_MSC_VER) && !defined(__ICL) || defined(__SUNPRO_CC)
FORMAT_DECL(int);
#endif

#undef FORMAT_DECL

/*! A message base class. Never used directly - only as common interface
 *  for CstrIdMessage, UintIdMessage etc.
 */
class COMMON_EXPIMP Message : public RefCounted<> {
public:
    enum MessageClass {
        UINT_ID_MESSAGE = 0, ///< Message with uint32 as message ID
        CSTR_ID_MESSAGE      ///< Message with char* as message ID
    };
    /// Class of the message
    MessageClass messageClass() const { return MessageClass(mclass_); }

    /// Message severity levels
    enum SeverityLevel {
        L_NONE, L_INFO, L_WARNING, L_ERROR, L_FATAL,
        MAX_SEVERITY_LEVEL
    };
    /// returns severity level
    SeverityLevel severityLevel() const { return SeverityLevel(level_); }

    /// sets new severity level
    void  setSeverityLevel(SeverityLevel lvl) { level_ = lvl; }

    /// returns severity level as a character string
    const char* severityLevelName() const;

    /// Returns message facility
    uint32      facility() const { return facility_; }

    /// Format the message
    String      format(const MessageFetcher&) const;

    /// Fetch the argument by it's index
    const MessageArgBase* getArg(ulong index) const
    {
        return args_[index];
    }
    /// total number of arguments
    ulong       nArgs() const { return args_.size(); }

    /// Append an argument instance. Ownership is transferred to Message
    void        appendArgInstance(MessageArgBase* inst)
    {
        args_.push_back(inst);
    }
    virtual ~Message();

    Message(MessageClass mclass, uint32 facility, SeverityLevel lv = L_NONE);

private:
    //! may not be copied or assigned at all
    DEFAULT_COPY_CTOR_DECL(Message)
    DEFAULT_ASSIGN_OP_DECL(Message)
    //!
    virtual bool fetchString(const MessageFetcher&, String&) const = 0;
    //!
    typedef OwnerVector<MessageArgBase> ArgVector;
    ArgVector       args_;
    uint32          facility_;
    char            mclass_;
    char            level_;
};

/*! Message referenced by message ID. See Message for common
 *  interface.
 */
class COMMON_EXPIMP UintIdMessage : public Message {
public:
    uint32 messageId() const { return messageId_; }
    //!
    UintIdMessage(uint32 msgid, uint32 facility);
    //!
    virtual ~UintIdMessage();

private:
    DEFAULT_COPY_CTOR_DECL(UintIdMessage)
    DEFAULT_ASSIGN_OP_DECL(UintIdMessage)
    //!
    virtual bool fetchString(const MessageFetcher&, String&) const;
    uint32 messageId_;
};

/*! Message referenced by ASCII text string. See Message
 *  for common interface.
 */
class COMMON_EXPIMP CstrIdMessage : public Message {
public:
    const char* messageId() const { return messageId_; }

    /// Initialize message with const string key; msgid used by refernece
    CstrIdMessage(const char* msgid, uint32 facility);
    /// Initialize message with string key; msgid is copied
    CstrIdMessage(char* msgid, uint32 facility);

    virtual ~CstrIdMessage();

private:
    DEFAULT_COPY_CTOR_DECL(CstrIdMessage)
    DEFAULT_ASSIGN_OP_DECL(CstrIdMessage)

    virtual bool fetchString(const MessageFetcher&, String&) const;
    char*   messageId_;
    bool    free_msgid_;
};

class MessageStreamItem;

class COMMON_EXPIMP MessageArgFmt {
public:
    MessageArgFmt(const char* fmt, bool isContext = false)
        : fmt_(fmt), ctx_(isContext), nonconst_(false) {}
    MessageArgFmt(char *fmt, bool isContext = false)
        : fmt_(fmt), ctx_(isContext), nonconst_(true) {}
    MessageArgFmt()
        : fmt_(0), ctx_(true), nonconst_(false) {}

private:
    friend class MessageStreamItem;
    const char* fmt_;
    bool  ctx_, nonconst_;
};

/*! Message stream item - a building block for message streams,
 *  which provides automatic MessageArg creation for all predefined
 *  types. Users may inherit from MessageStreamItem if additional
 *  MessageArg types is required.
 */
class MessageStreamItem {
public:
    typedef MessageStreamItem MI;
private:
    template <typename T>  MI& arg_generator(const T& v) {
        if (next_nonconst_)
            message_->appendArgInstance(new MessageArg<T>
              (v, next_ctx_, const_cast<char*>(next_fmt_)));
        else
            message_->appendArgInstance(new MessageArg<T>
              (v, next_ctx_, next_fmt_));
        next_ctx_  = next_nonconst_ = false;
        next_fmt_  = 0;
        return *this;
    }
public:
    /*! Generic operator for message argument instance generation;
     *  provides stream semantics.
     */
    template <typename T> MI& operator<<(const T& v) {
        return arg_generator(v);
    }

    // handling builtin types which are not in MessageArgTypes
    COMMON_EXPIMP MI& operator<<(char* v);
    COMMON_EXPIMP MI& operator<<(const char* v);
    COMMON_EXPIMP MI& operator<<(long v)
    { return arg_generator(static_cast<int32>(v)); }
    COMMON_EXPIMP MI& operator<<(unsigned long v)
    { return arg_generator(static_cast<uint32>(v)); }
    COMMON_EXPIMP MI& operator<<(short v)
    { return arg_generator(static_cast<int32>(v)); }
    COMMON_EXPIMP MI& operator<<(unsigned short v)
    { return arg_generator(static_cast<uint32>(v)); }

    /*! Explicit format modifier (printf format) for the next argument.
     *  Usage example: os << ... << MessageArgFmt("%2.2e") << myfloat << ...;
     *  myfloat will be formatted with provided format string.
     *  Syntax os << MessageArgFmt() << arg means that argument shouldn't
     *  be formatted at all (context arg flag will be set).
     */
    COMMON_EXPIMP MI& operator<<(const MessageArgFmt& f)
    {
        next_ctx_ = f.ctx_;
        next_nonconst_ = f.nonconst_;
        next_fmt_ = f.fmt_;
        return *this;
    }
    COMMON_EXPIMP MI& operator<<(Message::SeverityLevel lvl)
    {
        message_->setSeverityLevel(lvl);
        return *this;
    }
    COMMON_EXPIMP MessageStreamItem(Messenger* messenger, Message* msg)
      : messenger_(messenger), message_(msg),
        next_ctx_(false), next_nonconst_(false), next_fmt_(0) {}

    COMMON_EXPIMP virtual ~MessageStreamItem();

private:
    MessengerPtr messenger_;
    RefCntPtr<Message> message_;
    bool         next_ctx_;        // next argument is a context one
    bool         next_nonconst_;   // next_fmt - const or not
    const char*  next_fmt_;
};

/*! Base class for implementing classes which may act as a message
 *  streams.
 */
class COMMON_EXPIMP MessageStream {
public:
    struct UintMessageIdBase {
        int32   id;
        UintMessageIdBase(int32 i) : id(i) {}
    };
    Messenger* getMessenger() const { return messenger_.pointer(); }
    void       setMessenger(Messenger* m);

    MessageStreamItem operator<<(const char* msgid)
    {
        return MessageStreamItem(messenger_.pointer(),
            new CstrIdMessage(msgid, facility_));
    }
    MessageStreamItem operator<<(const UintMessageIdBase& msgid)
    {
        return MessageStreamItem(messenger_.pointer(),
            new UintIdMessage(msgid.id, facility_));
    }
    MessageStreamItem operator<<(const String& s);  // if msgid is String

    MessageStream(uint32 facility = 0, Messenger* m = 0); 

    virtual ~MessageStream();

private:
    int32 facility_;
    RefCntPtr<Messenger> messenger_;
};

/*! This class implements message block - entity which can queue
 *  message events and override message events with lower severity.
 *  Typical use of MessageBlock is passing it into nested functions
 *  which may add messages; all messgages (or one with highest
 *  severity) are dispatched explicitly or when MessageBlock are
 *  destructed.
 */
class COMMON_EXPIMP MessageBlock : public RefCounted<>,
                                   public MessageStream {
    // messenger which appends message to the message block
    class MbMessenger;
    friend class MbMessenger;

    // maximum number of logged messages
    enum { DEFAULT_MAX_MSGBUF_SIZE = 64 };
public:
    /*! Sets minimal "interesting" severity (messages with
     *  less severity won't be normally dispatched)
     */
    void    setMinSeverity(Message::SeverityLevel lvl)
    {
        minSeverity_ = lvl;
    }

    /*! Sets maximum number of messages which can be kept
     *  in MessageBlock. Limiting number of messages is
     *  necessary to avoid message floods.
     *
     *  If there will be an attempt to log more messages than fits
     *  into message buffer, MessageBlock will discard extra messages,
     *  starting with less severe ones.
     */
    void    setMaxMsgBuf(uint size);

    /*! Dispatch message(s). This makes internal message list empty.
     *  If \a logALl is true, dispatch() will dispatch all messages
     *  (not only ones with severity at least equal to SeverityForDispatch).
     */
    void    dispatch(bool logAll = false);

    /*! Clear message buffer, without dispatching the messages
     */
    void    clear();

    MessageBlock(uint32 facility = 0, Messenger* m = 0,
                 Message::SeverityLevel minSv = Message::L_NONE);
    virtual ~MessageBlock();

private:
    typedef Vector<RefCntPtr<Message> > MessageBuffer;
    MessageBuffer messages_;
    RefCntPtr<Messenger> messenger_;
    Message::SeverityLevel minSeverity_;
    uint maxMsgBuf_;
};

COMMON_NS_END

#if defined(_MSC_VER) && !defined(__ICL)
# pragma warning (pop)
#endif

#endif // MESSAGE_H_
