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

/** \file
 */

#include "DocSpeller.h"
#include "SpellChecker.h"
#include "common/OwnerPtr.h"
#include "common/String.h"

#include <map>
#include <set>

using namespace Common;
using namespace std;

typedef set<String>         ignore_dict_type;
typedef map<String, String> change_dict_type;

class DocSpeller::Impl {
public:
    Impl() {}
    virtual ~Impl() {}

    OwnerPtr<SpellChecker>      spellChecker_;

    //!
    ignore_dict_type            ignore_dict_;
    change_dict_type            change_dict_;
};

DocSpeller::~DocSpeller()
{
    delete impl_;
}

inline DocSpeller::Impl& DocSpeller::getImpl() const
{
    if (0 == impl_)
        impl_ = new DocSpeller::Impl;
    return *impl_;
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
    getImpl().ignore_dict_.insert(ustring(w.begin(), w.end()));
}

void DocSpeller::addToSessionReplDict(const Word& word, const Word& repl)
{
    ustring k(word.begin(), word.end());
    change_dict_type::value_type v(k, ustring(repl.begin(), repl.end()));
    getImpl().change_dict_.insert(v);
}

bool DocSpeller::isIgnored(const Word& w) const
{
    return 0 < getImpl().ignore_dict_.count(ustring(w.data(), w.size()));
}

const String& DocSpeller::querySessionDict(const Word& word) const
{
    ustring k(word.begin(), word.end());
    change_dict_type::const_iterator it(getImpl().change_dict_.find(k));
    if (getImpl().change_dict_.end() != it)
        return it->second;
    return String::null();
}

SpellChecker& DocSpeller::getChecker()
{
    if (0 == getImpl().spellChecker_.pointer())
        getImpl().spellChecker_.reset(new SpellChecker());
    return *(getImpl().spellChecker_.pointer());
}

void DocSpeller::reset()
{
    Impl& impl = getImpl();

    impl.change_dict_.clear();
    impl.ignore_dict_.clear();
    impl.spellChecker_.reset(0);
}

