//
// Copyright(c) 2011 Syntext, Inc. All Rights Reserved.
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

#include "SpellChecker.h"
#include "HunspellLibrary.h"

using namespace Common;

class HunspellChecker : public SpellChecker {
public:
    HunspellChecker(HunHandle* handle)
        : handle_(handle) {}
    virtual ~HunspellChecker() {}
    
    virtual bool check(const RangeString&) const;
    virtual bool suggest(const RangeString&, Strings& si) const;
    virtual bool addToPersonal(const RangeString&);
    virtual const Common::String& getDict() const { return handle_->lang(); }
    virtual void  resetPwl(const WordSet&);

private:
    RefCntPtr<HunHandle> handle_;
};

#define FROM_RS(word) handle_->from_rs(word).c_str()

bool HunspellChecker::check(const RangeString& word) const
{
    return HFUN(spell)(handle_->raw(), FROM_RS(word));
}

bool HunspellChecker::suggest(const RangeString& word, Strings& si) const
{
    si.clear();
    char** sugg_lst = 0;
    int ns = HFUN(suggest)(handle_->raw(), &sugg_lst, FROM_RS(word));
    if (ns <= 0 || !sugg_lst)
        return false;
    for (int i = 0; i < ns; ++i)
        si.push_back(handle_->to_string(sugg_lst[i]));
    HFUN(free_list)(handle_->raw(), &sugg_lst, ns);
    return true;
}

void HunspellChecker::resetPwl(const WordSet& si)
{
    WordSet::const_iterator it = getPwl().begin();
    if (&si != &getPwl()) {
        for (; it != getPwl().end(); ++it)
            HFUN(remove)(handle_->raw(), FROM_RS(*it));
    }
    for (it = si.begin(); it != si.end(); ++it)
        HFUN(add)(handle_->raw(), FROM_RS(*it));
}

bool HunspellChecker::addToPersonal(const RangeString& rs)
{
    HFUN(add)(handle_->raw(), FROM_RS(rs));    
    SpellChecker::addToPersonal(rs);
    return savePwl();
}

SpellChecker* HunspellLibrary::getSpellChecker(const String& dict) 
{
    HunHandle* handle = getHandle(dict);
    if (0 == handle)
        return 0;
    if (!handle->spellChecker()) {
        handle->setSpellChecker(new HunspellChecker(handle));
        if (handle->spellChecker()->loadPwl())
            handle->spellChecker()->resetPwl(handle->spellChecker()->getPwl());
    }
    return handle->spellChecker();
}

static SpellerLibrary* hunspell_library_instance()
{
    return &HunspellLibrary::instance();
}

static SpellLibraryRegistrar aspell_reg(
    NOTR("hunspell"), hunspell_library_instance);
