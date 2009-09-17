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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "SpellChecker.h"
#include "common/String.h"
#include "common/StringCvt.h"
#include "common/RefCounted.h"
#include "utils/SernaMessages.h"
#include <list>
#include <iostream>

using namespace Common;
using namespace std;

SpellCheckerSet::SpellCheckerSet()
{
    currentChecker_ = defaultChecker_ = SpellChecker::make(nstring());
    spellCheckers_[defaultChecker_->getDict()] = defaultChecker_;
    lastChecker_ = spellCheckers_.begin();
}

SpellCheckerSet::~SpellCheckerSet()
{
    SpellCheckerMap::iterator it = spellCheckers_.begin();
    for (; it != spellCheckers_.end(); ++it)
        delete it->second;
}

SpellChecker& SpellCheckerSet::getChecker(const Common::String& lang,
                                          SpellChecker::Status* status) 
{
    if (lang.isEmpty()) 
        return *currentChecker_;
    if (lastChecker_->first == lang)
        return *lastChecker_->second;
    SpellCheckerMap::const_iterator it = spellCheckers_.find(lang);
    if (it != spellCheckers_.end()) {
        if (!it->second) 
            return defaultChecker();
        lastChecker_ = it;
        return *it->second;
    }
    OwnerPtr<SpellChecker> new_checker;
    try {
        new_checker = SpellChecker::make(lang.utf8());
    } catch(SpellChecker::Error& e) {
        if (status)
            status->reset(e);
    }
    it = spellCheckers_.insert(SpellCheckerMap::value_type(lang,
        new_checker.release())).first;
    return it->second ? *it->second : defaultChecker();
}

void SpellCheckerSet::setDict(const RangeString& dict)
{
    currentChecker_ = &getChecker(dict.toString());    
}

bool SpellCheckerSet::isIgnored(const RangeString& w) const
{
    return 0 < ignoredWords_.count(w.toString());
}

void SpellCheckerSet::addToIgnored(const RangeString& w)
{
    ignoredWords_.insert(w.toString());
}

/////////////////////////////////////////////////////////////////////////

const char SpellChecker::Error::Info::c_str_[] = NOTR("Spellchecker error");

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
