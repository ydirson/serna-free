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
 #ifndef SPELLCHECKER_IMPL_H_
#define SPELLCHECKER_IMPL_H_

#include "SpellChecker.h"
#include "common/StringDecl.h"
#include "common/StringCvt.h"
#include "common/RefCounted.h"
#include "common/Message.h"
#include "common/MessageUtils.h"

class SpellChecker::Strings::Impl : public COMMON_NS::RefCounted<> {
public:
    virtual void addString(const COMMON_NS::String&) = 0;
protected:
    Impl() {}
    virtual ~Impl() {}
};

class SpellChecker::Speller {
public:
    //!
    virtual ~Speller() {}
    //!
    static Speller* make(const char* speller, const nstring& dict);
    //!
    static bool     getDictList(Strings::Impl& si, Status* = 0);
    //!
    virtual bool    check(const Char* w, unsigned l, Status* = 0) = 0;
    //!
    virtual bool    suggest(const Char* w, unsigned l, Strings::Impl& si,
                            Status* = 0) = 0;
    //!
    virtual bool    addToPersonal(const Char* word, unsigned len,
                                  Status* = 0) = 0;
    //!
    virtual const COMMON_NS::String& getDict() const = 0;
protected:
    Speller() {}
};

class SpellChecker::Error::Info : public COMMON_NS::Messenger {
public:
    typedef COMMON_NS::ustring              ustring;
    typedef COMMON_NS::MessageStreamItem    Item;
    //!
    Info() {}
    Info(const COMMON_NS::ustring& s) : nWhat_(local_8bit(s)), what_(s) {}
    Info(const char* s) : nWhat_(s), what_(Common::from_local_8bit(s)) {}
    virtual ~Info() {}
    //!
    Item            operator<<(const COMMON_NS::MessageStream::
                               UintMessageIdBase& msgid);
    //!
    const char* c_str() const throw() { return nWhat_.c_str(); }
    const ustring&  whatString() const throw() { return what_; }
    void            clear() { what_.resize(0); nWhat_.resize(0); }

    //! Messenger interface implementation
    virtual void    dispatch(COMMON_NS::RefCntPtr<COMMON_NS::Message>&);
    virtual COMMON_NS::Messenger* copy() const;
private:
    Common::nstring    nWhat_;
    COMMON_NS::ustring what_;
};

typedef COMMON_NS::RefCntPtr<SpellChecker::Error::Info> SpellErrorInfoPtr;

#endif // SPELLCHECKER_IMPL_H_
