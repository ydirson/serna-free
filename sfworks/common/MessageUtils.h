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
 *  Message handling facilities
 */
#ifndef MESSAGE_UTILS_H
#define MESSAGE_UTILS_H

#include "common/common_defs.h"
#include "common/StringDecl.h"
#include "common/Vector.h"
#include "common/RefCntPtr.h"
#include "common/RefCounted.h"

COMMON_NS_BEGIN
# ifdef COMMON_PRAGMA_BEGIN
# pragma COMMON_PRAGMA_BEGIN
# endif // - COMMON_PRAGMA_BEGIN

class Message;

/*! Abstract interface for messengers (message handlers).
 */
class COMMON_EXPIMP Messenger : public RefCounted<> {
public:
    /*! Dispatch a message. Message ownership is transferred
        to the messenger.
     */
    virtual void dispatch(RefCntPtr<Message>& message) = 0;
    virtual ~Messenger() {}
};

typedef RefCntPtr<Messenger> MessengerPtr;

/*! A default messenger which provides messaging to std::cerr.
 */
class COMMON_EXPIMP DefaultMessenger : public Messenger {
public:
    virtual void dispatch(RefCntPtr<Message>& message);
    virtual ~DefaultMessenger() {}
};

/*! A messenger which is silent (does nothing, just releases the message).
 */
class COMMON_EXPIMP SilentMessenger : public Messenger {
public:
    virtual void dispatch(RefCntPtr<Message>& msg);
    virtual ~SilentMessenger() {}
};

typedef Vector<RefCntPtr<Message> > MessageList;

class COMMON_EXPIMP MessageListMessenger : public Messenger {
public:
    MessageListMessenger(MessageList& mvec)
        : mvec_(mvec) {}

    virtual void dispatch(RefCntPtr<Message>& m);
    virtual ~MessageListMessenger();

private:
    MessageList& mvec_;
};

/*! Abstract interface for message fetchers. Message fetcher must
 *  be able to extract text string from dictionary by integer id
 *  or by ASCII string id (or by both - depending on the application).
 */
class COMMON_EXPIMP MessageFetcher {
public:
    virtual bool fetchString(uint32 fcl, uint32 id, String& os) const = 0;
    virtual bool fetchString(uint32 fcl, const char* id, String& os) const = 0;
    virtual ~MessageFetcher() {}
};

/*! Builtin message fetcher; uses compiled-in strings
 *  and compiled-in dictionary.
 */
class COMMON_EXPIMP BuiltinMessageFetcher : public MessageFetcher {
public:
    virtual bool fetchString(uint32 fcl, uint32 id, String& os) const = 0;
    virtual bool fetchString(uint32 fcl, const char* id, String& os) const = 0;

    /*! Set default language. Language is the language ID
     *  text string used in msggen. Returns false if language is not
     *  available (not registered). Language is "en" by default.
     */
    static bool setLanguage(const char* lang);

    /*! lists all available (loaded) languages
     */
    static void listLanguages(Vector<const char*>& langList);

    /*! open additional message library (.so/.dll). If forceAbs is true,
     *  then library won't be searched in %PATH/$LD_LIBRARY_PATH,
     *  but local file will be opened.
     */
    static bool loadMessageLibrary(const String& path, bool forceAbs = false);

    /*! Close/unload  messgage library
     */
    static void unloadMessageLibrary(const String& path);

    /*! Return list of filenames of loaded message libraries
     */
    static void messageLibraryList(Vector<String>&);

    /*! Returns global instance of BuiltinMessageFetcher.
     */
    static BuiltinMessageFetcher& instance();

    virtual ~BuiltinMessageFetcher();
};

# ifdef COMMON_PRAGMA_END
# pragma COMMON_PRAGMA_END
# endif // - COMMON_PRAGMA_END
COMMON_NS_END

#endif // MESSAGE_UTILS_H
