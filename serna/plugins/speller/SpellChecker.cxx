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
 *  Implementation of a spellchecker using aspell shared library
 *
 */

#include "SpellChecker.h"
#include "SpellCheckerImpl.h"
#include "common/String.h"
#include "common/StringCvt.h"
#include "common/RefCounted.h"
#include "utils/SernaMessages.h"
#include <list>

USING_COMMON_NS
using namespace std;

//!
SpellChecker::SpellChecker(const nstring& lang)
 :  impl_(SpellChecker::Speller::make(NOTR("aspell"), lang))
{
}

SpellChecker::SpellChecker()
 :  impl_(SpellChecker::Speller::make(NOTR("aspell"), nstring()))
{
}

SpellChecker::~SpellChecker()
{
}

typedef SpellChecker::Strings Strings;

bool SpellChecker::check(const Char* word, unsigned len, Status* ps) const
{
    return impl_->check(word, len, ps);
}

class Strings::StringCont : public Strings::Impl {
public:
    typedef std::list<COMMON_NS::String> StringContainer;
    virtual void addString(const String& w) { word_list_.push_back(w); }
    StringContainer::const_iterator begin() const { return word_list_.begin(); }
    StringContainer::const_iterator end() const { return word_list_.end(); }
    virtual ~StringCont() {}
private:
    list<COMMON_NS::String> word_list_;
};

Strings SpellChecker::suggest(const Char* word, unsigned len, Status* ps) const
{
    auto_ptr<Strings::StringCont> psl(new Strings::StringCont);
    impl_->suggest(word, len, *psl.get(), ps);
    return Strings(psl.release());
}

bool SpellChecker::addToPersonal(const Char* word, unsigned len,
                                 Status* ps) const
{
    return impl_->addToPersonal(word, len, ps);
}

const String& SpellChecker::getDict() const
{
    return impl_->getDict();
}

void SpellChecker::setDict(const Char* did, unsigned dlen, Status* ps)
{
    nstring id(did, did + dlen);
    try {
        OwnerPtr<SpellChecker::Speller> sc_ptr;
        sc_ptr.reset(SpellChecker::Speller::make(NOTR("aspell"), id));
        if (0 != &*sc_ptr)
            impl_.reset(sc_ptr.release());
    }
    catch (SpellChecker::Error& e) {
        if (0 != ps)
            ps->reset(e);
    }
}

Strings SpellChecker::getDictList(Status* ps)
{
    auto_ptr<Strings::StringCont> psl(new Strings::StringCont);
    SpellChecker::Speller::getDictList(*psl.get(), ps);
    return Strings(psl.release());
}

SpellChecker::Error::Error(const Info* pi) : what_(pi) {}
SpellChecker::Error::Error(const Error& other)
 :  std::exception(other), what_(other.what_) {}
SpellChecker::Error::Error(const char* s) : what_(new Info(s)) {}
SpellChecker::Error::Error(const ustring& s) : what_(new Info(s)) {}
SpellChecker::Error::~Error() throw() {}

const SpellChecker::Error::Info* SpellChecker::Error::getInfo() const
{
    return what_.get();
}

const char* SpellChecker::Error::what() const throw()
{
    const Info* pi = getInfo();
    return pi ? pi->c_str() : NOTR("Spellchecker error");
}

const ustring& SpellChecker::Error::whatString() const throw()
{
    const Info* pi = getInfo();
    static const ustring& err(from_local_8bit(NOTR("Spellchecker error")));
    return pi ? pi->whatString() : err;
}

typedef SpellChecker::Status Status;

Status::Status(const Status& o) : Error(o.getInfo()) {}
Status::Status(const SpellChecker::Error& e) : Error(e.getInfo()) {}
Status::Status(const Info* pi) : Error(pi) {}
Status::~Status() throw() {}

Status& Status::operator=(const Status& o)
{
    what_ = o.getInfo();
    return *this;
}

void Status::reset(const Info* pi) { what_ = pi; }
void Status::reset(const SpellChecker::Error& e) { what_ = e.getInfo(); }

bool Status::isOk() const { return 0 == getInfo(); }

const ustring& Status::errMsg() const throw()
{
    return whatString();
}

void SpellChecker::Error::Info::dispatch(RefCntPtr<Message>& msg)
{
    what_.assign(msg->format(BuiltinMessageFetcher::instance()));
}

Messenger* SpellChecker::Error::Info::copy() const { return new Info(*this); }

typedef SpellChecker::Error::Info Info;

Info::Item
Info::operator<<(const MessageStream::UintMessageIdBase& msgid)
{
    const int facility = SernaMessages::getFacility();
    return Item(this, new UintIdMessage(msgid.id, facility));
}


///////////////////////////// Strings

Strings::Strings(const StringCont* impl) : impl_(impl) {}
Strings::Strings(const Strings& o) : impl_(o.impl_) {}
Strings::Strings() : impl_(new StringCont) {}
Strings::~Strings() {}

typedef Strings::iterator Iterator;

//!
class Iterator::Position {
public:
    typedef Strings::StringCont::StringContainer::const_iterator iterator;
    Position(iterator it) : it_(it) {}
    Position(const Strings::StringCont* cont) : it_(cont->begin()) {}
    Position(const Position& pos) : it_(pos.it_) {}
    inline const Iterator::value_type& ref() const { return *it_; }
    iterator it_;
};

Iterator Strings::begin()
{
    return Iterator(new Iterator::Position(impl_->begin()));
}

Iterator Strings::end()
{
    return Iterator(new Iterator::Position(impl_->end()));
}

Iterator::iterator(const Strings& psug)
 :  pos_(new Position(psug.impl_.pointer()))
{
}

Iterator::iterator(Position* ppos) : pos_(ppos) {}

Iterator::iterator(const Iterator& other)
 :  pos_(new Position(*other.pos_))
{
}

Iterator::~iterator() {}

Iterator& Iterator::operator=(const Iterator& other)
{
    pos_.reset(new Position(*other.pos_));
    return *this;
}

Iterator& Iterator::operator++() { ++pos_->it_; return *this; }

bool Iterator::operator==(const Iterator& other) const
{
    return pos_->it_ == other.pos_->it_;
}

const Iterator::value_type&
Iterator::operator*() const { return pos_->ref(); }

const Iterator::value_type*
Iterator::operator->() const { return &pos_->ref(); }

