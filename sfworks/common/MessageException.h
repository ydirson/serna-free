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
#ifndef COMMON_MESSAGE_EXCEPTION_H_
#define COMMON_MESSAGE_EXCEPTION_H_

#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/Exception.h"

namespace Common {

class COMMON_EXPIMP MessageExceptionBase : public Exception {
public:
    const Message* message() const throw() { return message_.pointer(); }
    int     messageId() const throw() 
        { return message_ ? message_->messageId() : 0; }
    virtual String whatString() const throw()
    {
        format();
        return Exception::whatString();
    }
    virtual const char* what() const throw()
    {
        format();
        return Exception::what();
    }
    virtual ~MessageExceptionBase() throw() {}
protected:
    MessageExceptionBase(UintIdMessage* message)
        : message_(message) {}
    void append(const String& str)
        { message_->appendArgInstance(new MessageArg<String>(str)); }

    RefCntPtr<UintIdMessage> message_;

private:
    void format() const
    {
        if (message_)
            set_what(message_->format(BuiltinMessageFetcher::instance()));
    }
};

template <typename MT> class MessageException : public MessageExceptionBase {
public:
    MessageException(uint m)
        : MessageExceptionBase(new UintIdMessage(m, MT::getFacility()))
    {
    }
    MessageException(uint m, const String& str)
        : MessageExceptionBase(new UintIdMessage(m, MT::getFacility()))
    {
        append(str);
    }
    MessageException(uint m, const String& str, const String& str1)
        : MessageExceptionBase(new UintIdMessage(m, MT::getFacility()))
    {
        append(str); append(str1);
    }
    MessageException(uint m, const String& str, const String& str1,
                              const String& str2)
        : MessageExceptionBase(new UintIdMessage(m, MT::getFacility()))
    {
        append(str); append(str1); append(str2);
    }
    MessageException(uint m, const String& str,  const String& str1,
                              const String& str2, const String& str3)
        : MessageExceptionBase(new UintIdMessage(m, MT::getFacility()))
    {
        append(str); append(str1); append(str2); append(str3);
    }
    MessageException(uint m, const String& str,  const String& str1,
                             const String& str2, const String& str3,
                             const String& str4)
        : MessageExceptionBase(new UintIdMessage(m, MT::getFacility()))
    {
        append(str); append(str1); append(str2); append(str3); append(str4);
    }
    virtual ~MessageException() throw() {}
};

} // namespace Common

#endif // COMMON_MESSAGE_EXCEPTION_H_

