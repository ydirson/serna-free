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

#ifndef SPELLER_HUNSPELL_LIBRARY_H_
#define SPELLER_HUNSPELL_LIBRARY_H_

#include "SpellerLibrary.h"
#include "common/PropertyTree.h"
#include "common/OwnerPtr.h"

#include "hunspelldll.h"

class QTextCodec;

class HunHandle : public Common::RefCounted<> {
public:
    HunHandle*      load();
    HunHandle(const Common::String& dic_file,
              const Common::String& aff_file,
              const Common::String& lang_code);
    ~HunHandle();

    Common::nstring from_rs(const Common::RangeString& word) const;
    Common::String  to_string(const char* s) const;
    Hunhandle*  raw() const { return handle_; }
    SpellChecker*   spellChecker() const { return spellChecker_.pointer(); }
    void            setSpellChecker(SpellChecker*); 
    const Common::String& lang() const { return lang_; }

public:
    HunHandle(const HunHandle&);
    HunHandle& operator=(const HunHandle&);

    Hunhandle*      handle_;
    bool            loaded_;
    Common::String  dic_file_, aff_file_, lang_;
    QTextCodec*     codec_;
    Common::RefCntPtr<SpellChecker> spellChecker_;
};

class HunspellLibrary : public SpellerLibrary {
public:
    static HunspellLibrary& instance();

    HunHandle*              getHandle(const Common::String& dict);
    virtual bool            getDictList(SpellChecker::Strings& si) const;
    virtual SpellChecker*   getSpellChecker(const Common::String& dict);
    virtual bool            setConfig();

    HunspellLibrary();
    ~HunspellLibrary();

private:
    class DictMap;
    mutable Common::OwnerPtr<DictMap> dictMap_;    

    DEFAULT_COPY_CTOR_DECL(HunspellLibrary)
    DEFAULT_ASSIGN_OP_DECL(HunspellLibrary)
};

#define HFUN(x) \
 DynFunctor<FunTraits<TYPEOF(Hunspell_##x)>::FunType, nm_Hunspell_##x, \
    SpellerLibraryResolver<HunspellLibrary> >()

#endif // SPELLER_HUNSPELL_LIBRARY_H_
