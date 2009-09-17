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
 *  Interface to a generic spell checker
 */

#ifndef SPELL_CHECKER_H_
#define SPELL_CHECKER_H_

#include "common/String.h"
#include "common/RangeString.h"
#include "common/RefCntPtr.h"
#include "common/OwnerPtr.h"
#include "common/Message.h"
#include "common/MessageUtils.h"
#include <list>
#include <map>
#include <set>
#include <exception>

class SpellChecker {
public:
    typedef std::list<Common::String> Strings;
    class Error;
    class Status;

    virtual const Common::String&  getDict() const = 0;
    virtual bool    check(const Common::RangeString& word,
                          Status* = 0) const = 0;
    virtual bool    suggest(const Common::RangeString& word,
                            Strings& si, Status* = 0) const = 0;
    virtual bool    addToPersonal(const Common::RangeString& word,
                                  Status* = 0) = 0;
    static  bool    getDictList(Strings&, Status* = 0);

    static SpellChecker* make(const Common::nstring& lang);

    virtual ~SpellChecker() {}
};

class SpellCheckerSet {
public:
    SpellCheckerSet();
    ~SpellCheckerSet();

    SpellChecker&   getChecker(const Common::String& lang, 
                               SpellChecker::Status* = 0);
    SpellChecker&   defaultChecker() const { return *defaultChecker_; }
    void            setDict(const Common::RangeString&);

    void            addToIgnored(const Common::RangeString&);
    bool            isIgnored(const Common::RangeString&) const;
    void            clearIgnoreDict() { ignoredWords_.clear(); }

private:
    typedef std::map<Common::String, SpellChecker*> SpellCheckerMap;
    typedef std::set<Common::String> IgnoreDict;

    SpellCheckerSet(const SpellCheckerSet&);
    SpellCheckerSet& operator=(const SpellCheckerSet&);

    SpellCheckerMap spellCheckers_;
    IgnoreDict      ignoredWords_;
    SpellChecker*   defaultChecker_;
    SpellChecker*   currentChecker_;
    SpellCheckerMap::const_iterator lastChecker_;
};

class SpellChecker::Error : public std::exception {
public:
    virtual const COMMON_NS::ustring&   whatString() const throw();
    virtual const char*                 what() const throw();
    virtual ~Error() throw();
    class Info;
protected:
    const Info* getInfo() const;
    //!
    Error(const Error&);
    Error(const Info* = 0);
    Error(const COMMON_NS::ustring&);
    Error(const char*);
    friend class SpellChecker::Status;
private:
    DEFAULT_ASSIGN_OP_DECL(Error)
    COMMON_NS::RefCntPtr<const Info>    what_;
};

class SpellChecker::Status : protected SpellChecker::Error {
public:
    Status(const Info* = 0);
    
    bool    isOk() const;
    void    reset(const Info* = 0);
    void    reset(const SpellChecker::Error&);
    const Common::ustring& errMsg() const throw();
    //!
    Status(const Status&);
    Status& operator=(const Status&);
    virtual ~Status() throw();

private:
    Status(const SpellChecker::Error&);

    friend class SpellChecker;
};

class SpellChecker::Error::Info : public COMMON_NS::Messenger {
public:
    typedef COMMON_NS::ustring              ustring;
    typedef COMMON_NS::MessageStreamItem    Item;
    //!
    Info() {}
    Info(const COMMON_NS::ustring& s) : what_(s) {}
    Info(const char* s) : what_(COMMON_NS::from_local_8bit(s)) {}
    virtual ~Info() {}
    //!
    Item            operator<<(const COMMON_NS::MessageStream::
                               UintMessageIdBase& msgid);
    //!
    const char*     c_str() const throw() { return c_str_; }
    const ustring&  whatString() const throw() { return what_; }
    void            clear() { what_.resize(0); }

    //! Messenger interface implementation
    virtual void    dispatch(COMMON_NS::RefCntPtr<COMMON_NS::Message>&);
    virtual COMMON_NS::Messenger* copy() const;
private:
    COMMON_NS::ustring what_;
    static const char c_str_[];
};

typedef COMMON_NS::RefCntPtr<SpellChecker::Error::Info> SpellErrorInfoPtr;

#endif

