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

#include "FotTokenizer.h"
#include "SpellChecker.h"

#include "editableview/EditableView.h"
#include "structeditor/StructEditor.h"
#include "structeditor/InsertTextEventData.h"
#include "docview/SernaDoc.h"

#include "common/PropertyTree.h"
#include "common/StringCvt.h"

#include "proputils/PropertyAccessor.h"

#include "formatter/AreaPos.h"

#include "SpellerAgent.h"
#include "SpellerReactor.h"
#include "DocSpeller.h"

#include "utils/SernaMessages.h"
#include "utils/MsgBoxStream.h"

#include "speller_debug.h"

using namespace Common;

class StructSpeller : public DocSpeller {
public:
    StructSpeller(StructEditor* se, SpellCheckerSet& sset, OnlineSpeller* osp);

    virtual bool ignore(const Word& word);
    virtual bool ignoreAll(const Word& word);
    virtual bool add(const Word& word);
    virtual bool change(const Word& word, const Word& repl);
    virtual bool changeAll(const Word& word, const Word& repl);
    virtual bool skipElement();
    virtual bool setDict(const Word&);
    virtual bool start();
    virtual bool shutdown();
    virtual Common::PropertyNode* getProps() const { return &props_.getRoot(); }
private:
    void replace(const Word& word, const String& repl);
    //!
    DEFAULT_COPY_CTOR_DECL(StructSpeller)
    DEFAULT_ASSIGN_OP_DECL(StructSpeller)
    bool check(bool sync = true);
    //!
    StructEditor&                           structEdit_;
    FotTokenizer                            ft_;
    mutable PropUtils::PropertyAccessor     props_;
};

StructSpeller::StructSpeller(StructEditor* se, 
                             SpellCheckerSet& sset,
                             OnlineSpeller* osp)
 :  DocSpeller(sset, osp),
    structEdit_(*se),
    ft_(se->getDsi(), &se->editableView(), se->fot(), se->grove())
{
    SpellChecker::Strings dl;
    getChecker().getDictList(dl);
    if (dl.begin() != dl.end()) {
        PropertyNode& ptn(props_.getProp(NOTR("languages")));
        SpellChecker::Strings::iterator it = dl.begin();
        for (; dl.end() != it; ++it) {
            ptn.appendChild(new PropertyNode(String::null(), *it));
        }
    }
}

bool StructSpeller::ignore(const Word& word)
{
    DDBG << "StructSpeller::ignore(), word: " << sqt(range_ctor<ustring>(word))
         << std::endl;
    structEdit_.removeSelection();
    return check();
}

bool StructSpeller::ignoreAll(const Word& word)
{
    DDBG << "StructSpeller::ignoreAll(), word: "
         << sqt(range_ctor<ustring>(word)) << std::endl;
    this->addToIgnored(word);
    structEdit_.removeSelection();
    return check();
}

bool StructSpeller::add(const Word& word)
{
    DDBG << "StructSpeller::add(), word: " << sqt(range_ctor<ustring>(word))
         << std::endl;
    if (0 < word.size()) {
        try {
            this->addToPersonal(word);
        }
        catch (SpellChecker::Error& e) {
            msgbox_stream() << SernaMessages::spellCheckerError
                            << Message::L_ERROR << String(e.whatString());
            return false;
        }
    }
    structEdit_.removeSelection();
    return check();
}

using GroveEditor::GrovePos;

static bool
check_selection(GrovePos& from, GrovePos& to, const DocSpeller::Word& word)
{
    if (GrovePos::TEXT_POS != from.type() || GrovePos::TEXT_POS != to.type())
        return false;
    long fi = from.idx(), ti = to.idx();
    if (fi > ti) {
        GrovePos tmp(from);
        from = to;
        to = tmp;
        fi ^= ti; ti ^= fi; fi ^= ti;
    }
    const unsigned sz = word.size();
    if (ti != static_cast<int>(fi + sz))
        return false;
    const String& d(from.text()->data());
    if (0 != compare(d.data() + fi, sz, word.data(), sz))
        return false;
    return true;
}

class InsertText;

void StructSpeller::replace(const Word& word, const String& repl)
{
    GroveEditor::GrovePos from, to;
    if (StructEditor::POS_OK != structEdit_.getSelection(
            from, to, StructEditor::TEXT_OP))
        return;
    if (check_selection(from, to, word)) {
        InsertTextEventData ited(repl);
        makeCommand<InsertText>(&ited)->execute(&structEdit_);
        ft_.sync();
    }
}

bool StructSpeller::change(const Word& word, const Word& repl)
{
    DDBG << "StructSpeller::change(), word: " << sqt(range_ctor<ustring>(word))
         << ", repl: " << sqt(range_ctor<ustring>(repl)) << std::endl;
    replace(word, String(repl.begin(), repl.size()));
    return check();
}

bool StructSpeller::changeAll(const Word& word, const Word& repl)
{
    DDBG << "StructSpeller::changeAll(), word: "
         << sqt(range_ctor<ustring>(word)) << ", repl: "
         << sqt(range_ctor<ustring>(repl)) << std::endl;
    this->addToSessionReplDict(word, repl);
    return change(word, repl);
}

bool StructSpeller::skipElement()
{
    DDBG << "StructSpeller::skipElement" << std::endl;
    ft_.skipToNextElement();    return check(false);
}

bool StructSpeller::setDict(const Word& dict)
{
    DDBG << "StructSpeller::setDict(), dict: "
         << sqt(range_ctor<ustring>(dict))
         << ", was=" << getChecker().getDict()
         << std::endl;
    RangeString new_dict(dict.empty() ? defaultChecker().getDict() : dict);
    if (getChecker().getDict() != new_dict) {
        props_.getProp(NOTR("suggestions")).removeAllChildren();
        SpellChecker::Status status;
        resetDict(new_dict.data(), new_dict.size(), &status);
        if (!status.isOk()) {
            msgbox_stream() << SernaMessages::spellCheckerError
                            << Message::L_ERROR << String(status.errMsg());
            return false;
        }
        return true;
    }
    return false;
}

static void set_suggestions(PropertyNode& ptn, SpellChecker::Strings& sl)
{
    SpellChecker::Strings::iterator it = sl.begin();
    ptn.removeAllChildren();
    for (; sl.end() != it; ++it) {
        ptn.appendChild(new PropertyNode(String::null(), *it));
    }
}

bool StructSpeller::start()
{
    return check();
}

static void select_misspell(FotTokenizer& ft, StructEditor& se)
{
    GrovePos from, to;
    ft.getPosRange(from, to);

    Formatter::AreaPos start(se.toAreaPos(from));
    Formatter::AreaPos end(se.toAreaPos(to));

    se.setSelection(Formatter::ChainSelection(start, end),
                    GroveEditor::GroveSelection(EditContext::getSrcPos(start),
                                                EditContext::getSrcPos(end)));
    se.editableView().adjustView(end, true);
}

bool StructSpeller::check(bool sync)
{
    DDBG << "StructSpeller::start()" << std::endl;
    if (sync)
        ft_.sync();
    RangeString word(ft_.getWord());
    try {
        for (; !word.empty(); word = ft_.getWord()) {
            if (ft_.isLanguageChanged()) 
                setDict(ft_.getCurrentLanguage());
            SpellChecker& sc(getChecker());
            if (isIgnored(word) || sc.check(word))
                continue;
            const String& repl(querySessionDict(word));
            if (!is_null(repl)) {
                select_misspell(ft_, structEdit_);
                replace(word, repl);
                continue;
            }
            String misspell(word.data(), word.size());
            props_.getProp(NOTR("misspell")).setString(misspell);
            SpellChecker::Strings sl;
            sc.suggest(word, sl);
            set_suggestions(props_.getProp(NOTR("suggestions")), sl);
            props_.getProp(NOTR("language")).setString(sc.getDict());
            select_misspell(ft_, structEdit_);
            return false;
        }
        msgbox_stream(structEdit_.widget()) << SernaMessages::spellCheckerOk
                                            << Message::L_INFO;
    }
    catch (SpellChecker::Error& e) {
        msgbox_stream(structEdit_.widget()) << SernaMessages::spellCheckerError
                                            << Message::L_ERROR
                                            << String(e.whatString());
    }
    return true;
}

bool StructSpeller::shutdown()
{
    DDBG << "StructSpeller::shutdown()" << std::endl;
    structEdit_.removeSelection();
    reset();
    return true;
}

DocSpeller* DocSpeller::make(StructEditor* se, 
                             SpellCheckerSet& sset,
                             OnlineSpeller* osp)
{
    return new StructSpeller(se, sset, osp);
}
