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
 */
#include "SpellChecker.h"
#include "common/String.h"
#include "common/StringCvt.h"
#include "common/RefCounted.h"
#include "common/ScopeGuard.h"
#include "AspellLibrary.h"
#include "aspell.hpp"
#include "speller_debug.h"
#include <string.h>
#include <QTextCodec>

#if defined(SERNA_SYSPKG)
# ifdef FUN
#  undef FUN
# endif
# define FUN(x) x
#endif

using namespace Common;
using namespace std;

struct AspellCodecError : SpellChecker::Error {
    AspellCodecError(const nstring& err) : SpellChecker::Error(err.c_str()) {}
};

class AspellChecker : public SpellChecker {
public:
    typedef SpellChecker::Strings Strings;
    
    AspellChecker(const nstring& lang);
    virtual ~AspellChecker();
    
    virtual bool check(const RangeString&, 
                       SpellChecker::Status* = 0) const;
    virtual bool suggest(const RangeString&, Strings& si,
                         SpellChecker::Status* = 0) const;
    virtual bool addToPersonal(const RangeString&,
                               SpellChecker::Status* = 0);
    
    virtual const Common::String& getDict() const
    {
        if (dict_id_.empty()) {
            const nstring& d(AspellLibrary::instance().getDict());
            dict_id_ = from_latin1(d.data(), d.size());
        }
        return dict_id_;
    }
    //!
private:
    void set_status(SpellChecker::Status* ps = 0) const
    {
        DDBG << rbr(this) << "->set_status(): "
             << sqt(FUN(aspell_speller_error_message)(speller_)) << std::endl;
        if (0 != ps) {
            const char* errmsg = FUN(aspell_speller_error_message)(speller_);
            ps->reset(new SpellChecker::Error::Info(errmsg));
        }
    }
    nstring         actual_dict_;
    QTextCodec*     codec_;
    AspellSpeller*  speller_;
    mutable String  dict_id_;
};

AspellChecker::AspellChecker(const nstring& dict)
 :  actual_dict_(AspellLibrary::instance().findDict(dict)),
    speller_(AspellLibrary::instance().makeSpeller(actual_dict_)),
    dict_id_(actual_dict_.data(), actual_dict_.size())
{
    nstring encoding = AspellLibrary::instance().getEncoding(actual_dict_);
    codec_ = QTextCodec::codecForName(encoding.c_str());
    if (!codec_) {
        nstring err(NOTR("Unsupported local encoding: '"));
        err.append(encoding).append(1, '\'');
        throw AspellCodecError(err);
    }            
}

AspellChecker::~AspellChecker()
{
    if (0 != speller_) {
        FUN(aspell_speller_save_all_word_lists)(speller_);
        FUN(delete_aspell_speller)(speller_);
    }
}

typedef SpellChecker::Status Status;

bool AspellChecker::check(const RangeString& word, Status* ps) const
{
    QByteArray nw(codec_->fromUnicode(word.data(), word.size()));
    int rv = FUN(aspell_speller_check)(speller_, nw.data(), nw.size());
    if (1 == rv) {
        if (0 != ps)
            ps->reset();
        return true;
    }
    if (0 == rv)
        return false;
    set_status(ps);
    return false;
}

bool AspellChecker::suggest(const RangeString& word, Strings& si,
                            Status* ps) const
{
    QByteArray nw(codec_->fromUnicode(word.data(), word.size()));
    const AspellWordList* wl;
    if ((wl = FUN(aspell_speller_suggest)(speller_, nw.data(), nw.size()))) {
        if (0 != FUN(aspell_word_list_empty)(wl))
            return true;
        if (AspellStringEnumeration* ase = FUN(aspell_word_list_elements)(wl)) {
            ON_BLOCK_EXIT(FUN(delete_aspell_string_enumeration), ase);
            do
                if (const char* pw = FUN(aspell_string_enumeration_next)(ase))
                    si.push_back(codec_->toUnicode(pw, strlen(pw)));
            while (0 == FUN(aspell_string_enumeration_at_end)(ase));
        }
        else {
            set_status(ps);
            return false;
        }
    }
    return true;
}

bool SpellChecker::getDictList(Strings& si, Status* ps) 
{
    return AspellLibrary::instance().getDictList(si, ps);
}

bool AspellChecker::addToPersonal(const RangeString& word, Status* ps)
{
    QByteArray s(codec_->fromUnicode(word.data(), word.size()));
    int rv = FUN(aspell_speller_add_to_personal)(speller_, s.data(), s.size());
    if (0 == rv) {
        set_status(ps);
        return false;
    }
    return true;
}

SpellChecker* SpellChecker::make(const nstring& dict)
{
    return new AspellChecker(dict);
}

