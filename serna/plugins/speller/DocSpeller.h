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

#ifndef DOC_SPELLER_H_
#define DOC_SPELLER_H_

#include "SpellerReactor.h"
#include "SpellChecker.h"

#include "common/String.h"
#include "common/OwnerPtr.h"

class StructEditor;
class OnlineSpeller;

class DocSpeller : public SpellerReactor {
public:
    typedef Common::RangeString Word;
    virtual ~DocSpeller();

    static DocSpeller* make(StructEditor*, SpellCheckerSet&, OnlineSpeller*);

    SpellChecker&   getChecker();
    SpellChecker&   defaultChecker();

protected:
    DocSpeller(SpellCheckerSet&, OnlineSpeller*); 

    void            addToIgnored(const Word& word);
    void            addToPersonal(const Word& word);
    bool            isIgnored(const Word& word) const;
    void            addToSessionReplDict(const Word& word, const Word& repl);
    const Common::String& 
                    querySessionDict(const Word& word) const;

    void            resetDict(const Common::Char* did, 
                              unsigned dlen, SpellChecker::Status* ps); 
    void            reset();

private:
    DocSpeller(const DocSpeller&);
    DocSpeller& operator=(const DocSpeller&);

    class IgnoreDict;
    class ChangeDict;

    Common::OwnerPtr<ChangeDict>    change_dict_;
    SpellChecker*                   spellChecker_;
    SpellCheckerSet&                spellCheckerSet_;
    OnlineSpeller*                  onlineSpeller_;
};

#endif // DOC_SPELLER_H_
