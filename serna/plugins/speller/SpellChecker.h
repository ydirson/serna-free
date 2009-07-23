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
#include "common/RefCntPtr.h"
#include "common/OwnerPtr.h"
#include <list>
#include <exception>

class SpellChecker {
public:
    typedef COMMON_NS::String   CString;
    typedef COMMON_NS::nstring  nstring;
    typedef COMMON_NS::Char     Char;
    //!
    class Error;
    class Status;
    //!
    SpellChecker(const nstring& lang);
    SpellChecker();
    ~SpellChecker();
    //!
    class Strings;
    //!
    static Strings  getDictList(Status* = 0);
    //!
    const CString&  getDict() const;
    //!
    void            setDict(const Char* id, unsigned idlen, Status* = 0);
    //!
    bool            check(const Char* word,
                          unsigned wordlen, Status* = 0) const;
    //!
    Strings         suggest(const Char* word, unsigned wordlen,
                            Status* = 0) const;
    //!
    bool            addToPersonal(const Char* word, unsigned wordlen,
                                  Status* = 0) const;
    //!
    class Speller;
private:
    COMMON_NS::OwnerPtr<Speller> impl_;
};

class SpellChecker::Strings {
    friend class SpellChecker;
    friend class SpellChecker::Speller;
public:
    Strings(const Strings&);
    ~Strings();

    class iterator;
    iterator        begin();
    iterator        end();

    class Impl;
    class StringCont;
private:
    friend class StringCont;
    friend class iterator;
    COMMON_NS::RefCntPtr<const StringCont> impl_;
private:
    //! Disabled methods
    Strings(const StringCont*);
    Strings();
    DEFAULT_ASSIGN_OP_DECL(Strings)
};

/*
 */
class SpellChecker::Strings::iterator {
    friend class SpellChecker::Strings;
    typedef SpellChecker::Strings Strings;
public:
    typedef std::forward_iterator_tag iterator_category;
    typedef COMMON_NS::String value_type;

    iterator(const iterator& other);
    ~iterator();

    iterator&           operator=(const iterator& other);
    bool                operator==(const iterator& other) const;
    bool                operator!=(const iterator& other) const
        {
            return !operator==(other);
        }
    const value_type*   operator->() const;
    const value_type&   operator*() const;
    iterator&           operator++();
    iterator            operator++(int)
        {
            iterator t(*this); ++(*this); return t;
        }
private:
    iterator();
    class Position;
    iterator(const Strings&);
    iterator(Position*);
    COMMON_NS::OwnerPtr<Position> pos_;
};

/*
 */
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

/*
 */
class SpellChecker::Status : protected SpellChecker::Error {
public:
    Status(const Status&);
    Status& operator=(const Status&);
    virtual ~Status() throw();
    //!
    bool    isOk() const;
    //!
    void    reset(const Info* = 0);
    //!
    const COMMON_NS::ustring& errMsg() const throw();
    //!
    Status(const Info* = 0);
private:
    Status(const SpellChecker::Error&);
    //!
    void    reset(const SpellChecker::Error&);

    friend class SpellChecker;
};

#endif

