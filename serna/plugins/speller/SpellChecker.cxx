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
#include "SpellerLibrary.h"
#include "common/String.h"
#include "common/StringCvt.h"
#include "common/RefCounted.h"
#include "common/Singleton.h"
#include "common/PropertyTree.h"
#include "utils/SernaMessages.h"
#include "utils/Config.h"
#include <list>
#include <iostream>

using namespace Common;
using namespace std;

const char SPELL_CFG_VAR[]      = "speller";
const char SPELL_PWSDIR_VAR[]   = "pws-dir";
const char SPELL_DICT_VAR[]     = "default-dict";
const char SPELL_USE_VAR[]      = "use";
const char SPELL_DICT[]         = "en";

class NullChecker : public SpellChecker {
public:
    NullChecker(const String& dict)
        : dict_(dict) {}
    virtual const Common::String& getDict() const { return dict_; }
    virtual bool check(const Common::RangeString&) const { return true; }
    virtual bool suggest(const Common::RangeString&, Strings&) const 
        { return false; }
    virtual bool addToPersonal(const Common::RangeString&) { return false; }
private:
    String dict_;
};

SpellChecker* SpellChecker::make(const nstring& dict)
{
    SpellerLibrary* lib = SpellerLibrary::instance();
    SpellChecker* chk = lib ? lib->makeSpellChecker(dict) : 0;
    return chk? chk : new NullChecker(from_latin1(dict.c_str()));
}

bool SpellChecker::getDictList(Strings& si) 
{
    SpellerLibrary* lib = SpellerLibrary::instance();
    return lib ? lib->getDictList(si) : false;
}

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

SpellChecker& SpellCheckerSet::getChecker(const Common::String& lang) 
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
    OwnerPtr<SpellChecker> new_checker(SpellChecker::make(lang.utf8()));
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
