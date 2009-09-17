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

#include "DocSpeller.h"
#include "OnlineSpeller.h"
#include "SpellChecker.h"
#include "common/OwnerPtr.h"
#include "common/String.h"

#include <map>
#include <set>

using namespace Common;

class DocSpeller::ChangeDict : public std::map<String, String> {};

DocSpeller::DocSpeller(SpellCheckerSet& sset, OnlineSpeller* osp)
    : change_dict_(new ChangeDict),
      spellChecker_(0),
      spellCheckerSet_(sset),
      onlineSpeller_(osp)
{
}

DocSpeller::~DocSpeller()
{
}

void DocSpeller::resetDict(const Common::Char* did, 
                         unsigned dlen, SpellChecker::Status* ps)
{
    spellChecker_ = &spellCheckerSet_.getChecker(String(did, dlen), ps);
}

template <class Dict> const typename Dict::value_type*
query_val(Dict& dict, const typename Dict::key_type& key)
{
    typename Dict::iterator fit = dict.find(key);
    if (dict.end() == fit)
        return 0;
    return &(fit->second);
}

void DocSpeller::addToIgnored(const Word& w)
{
    spellCheckerSet_.addToIgnored(w.toString());
    if (onlineSpeller_)
        onlineSpeller_->unmarkWord(w);
}

void DocSpeller::addToPersonal(const Word& w)
{
    getChecker().addToPersonal(w);
    if (onlineSpeller_)
        onlineSpeller_->unmarkWord(w);
}

void DocSpeller::addToSessionReplDict(const Word& word, const Word& repl)
{
    ustring k(word.begin(), word.end());
    ChangeDict::value_type v(k, ustring(repl.begin(), repl.end()));
    change_dict_->insert(v);
}

bool DocSpeller::isIgnored(const Word& w) const
{
    return spellCheckerSet_.isIgnored(w.toString());
}

const String& DocSpeller::querySessionDict(const Word& word) const
{
    ustring k(word.begin(), word.end());
    ChangeDict::const_iterator it(change_dict_->find(k));
    if (change_dict_->end() != it)
        return it->second;
    return String::null();
}

SpellChecker& DocSpeller::getChecker()
{
    if (!spellChecker_)
        spellChecker_ = &spellCheckerSet_.defaultChecker();
    return *spellChecker_;
}

SpellChecker& DocSpeller::defaultChecker()
{
    return spellCheckerSet_.defaultChecker();
}

void DocSpeller::reset()
{
    change_dict_->clear();
    spellCheckerSet_.clearIgnoreDict();
    spellChecker_ = 0;
}

