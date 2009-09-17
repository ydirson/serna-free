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

#include "OnlineSpeller.h"
#include "SpellChecker.h"
#include "utils.h"
#include "structeditor/StructEditor.h"
#include "editableview/EditableView.h"
#include "utils/IdleHandler.h"
#include "docview/DocumentPlugin.h"
#include "formatter/impl/TerminalFos.h"
#include "formatter/XslFoExt.h"
#include "ui/UiAction.h"
#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"
#include "common/RangeString.h"
#include "common/WordTokenizer.h"
#include "common/OwnerPtr.h"
#include "common/safecast.h"
#include "common/CommandEvent.h"
#include <QTimer>

static const int SPELLER_HOLDOFF_INTERVAL = 700;  // 700 msecs
static const int SPELLER_TICK_CHUNK       = 50;   // 50 fo's at a time
static const int MAX_MENU_SUGGESTIONS     = 7;    // suggested words in menu

using namespace Common;
using namespace GroveEditor;

class OnlineSpellerImpl;

class SpellerHoldoffTimer : public QTimer,
                            public CommandEventContext {
    Q_OBJECT
public:
    SpellerHoldoffTimer(OnlineSpellerImpl& spi);
    bool    isOn() const;

public slots:
    void timer_event();
private:
    OnlineSpellerImpl& spi_;
};

class OnlineSpellerImpl : public OnlineSpeller,
                          public IdleHandler {
public:
    OnlineSpellerImpl(StructEditor*, DocumentPlugin*,
                      bool init_state, SpellCheckerSet&);

    void         update();
    bool         isOn() const { return isOn_; }

private:
    virtual void toggleState(bool);
    virtual void recheck();
    virtual void updateMenu();
    virtual void selectSuggestion();
    virtual void replaceSuggestion();
    virtual void add(bool ignored);
    
    virtual bool processQuanta();
    bool         process_fo();
    void         start_spelling();
    void         refill_folist();
    RangeString  markedWord(GrovePos* src_pos = 0) const;
    virtual void unmarkWord(const RangeString& word);
    Formatter::Fo* first_fo() const;

    SpellCheckerSet&            spellCheckers_;
    OwnerPtr<SpellerHoldoffTimer> timer_;
    EditableView::FoModList&    textFoModList_;
    StructEditor*               se_;
    DocumentPlugin*             dp_;
    bool                        forceModified_;
    bool                        isOn_;
    ElementSkipper              elementSkipper_;
    Sui::Action*                addAction_;
    Sui::Action*                ignoreAction_;
    Sui::Action*                suggestions_;
    Sui::Action*                replaceSuggestions_;
    Sui::Action*                replaceAllMenu_;
};

inline bool SpellerHoldoffTimer::isOn() const
{
    return spi_.isOn(); 
}

SIMPLE_COMMAND_EVENT_IMPL(UpdateTextFos, SpellerHoldoffTimer)

OnlineSpellerImpl::OnlineSpellerImpl(StructEditor* se, DocumentPlugin* dp,
                                     bool isOn,
                                     SpellCheckerSet& scset)
    : spellCheckers_(scset),
      timer_(new SpellerHoldoffTimer(*this)),
      textFoModList_(se->editableView().textFoModList()),
      se_(se),
      dp_(dp),
      forceModified_(false),
      isOn_(isOn),
      elementSkipper_(se->getDsi())
{
    addAction_    = dp->findPluginUiAction(NOTR("addWordToPersonal"));
    ignoreAction_ = dp->findPluginUiAction(NOTR("addWordToIgnored"));
    suggestions_  = dp->findPluginUiAction(NOTR("suggestions"));
    replaceSuggestions_ =
        dp->findPluginUiAction(NOTR("replaceSuggestions"));
    replaceAllMenu_ = dp->findPluginUiAction(NOTR("replaceAllWords"));
    se->viewUpdate().subscribe(makeCommand<UpdateTextFos>, &*timer_);
    if (isOn_)
        start_spelling();

}

void OnlineSpellerImpl::start_spelling()
{
    // process some FO's right away
    for (int i = 0; i < SPELLER_TICK_CHUNK && process_fo(); ++i)
        ;
    registerHandler(this);
}

bool UpdateTextFos::doExecute(SpellerHoldoffTimer* watcher, EventData*)
{
    if (watcher->isOn())
        watcher->start(SPELLER_HOLDOFF_INTERVAL);
    return true;
}

void OnlineSpellerImpl::update()
{
    if (textFoModList_.isEmpty())
        return;
    forceModified_ = true;
    registerHandler(this);
}

bool OnlineSpellerImpl::processQuanta()
{
    for (int i = 0; i < SPELLER_TICK_CHUNK; ++i) {
        if (process_fo()) 
            continue;
        deregisterHandler(this);
        return false;
    }
    return true;
}

static void update_fo_decoration(Formatter::TextFo* text_fo)
{
    Formatter::Area* area = text_fo->firstChunk();
    for (; area; area = area->Chunk::nextSibling())
        area->getView()->updateDecoration();
}

bool OnlineSpellerImpl::process_fo()
{
    Formatter::TextFo* text_fo = textFoModList_.pop_front();
    if (!text_fo) {
        deregisterHandler(this);
        return false;
    }
    RangeString fotext(text_fo->strippedText());
    WordTokenizer wtok(fotext);
    RangeString word;
    bool changed = false;
    if (text_fo->hasMarkedWords()) {
        text_fo->clearMarkedWords();
        changed = true;
    }
    const GroveLib::Node* fo_node = text_fo->headFoNode();
    if (!elementSkipper_.mustSkip(fo_node)) {
        while (wtok.next(word)) {
            if (spellCheckers_.isIgnored(word))
                continue;
            if (spellCheckers_.getChecker(get_lang(fo_node)).check(word))
                continue;
            changed = true;
            text_fo->addMarkedWord(word.toString());
        }
    }
    if (forceModified_ && changed) 
        update_fo_decoration(text_fo);
    return true;
}

static void traverse_text_fos(Formatter::Fo* fo, EditableView::FoModList& ml)
{
    if (fo->type() == Formatter::TEXT_FO) {
        ml.push_back(static_cast<Formatter::TextFo*>(fo));
        return;
    }
    for (fo = fo->firstChild(); fo; fo = fo->nextSibling())
        traverse_text_fos(fo, ml);
}

void OnlineSpellerImpl::refill_folist()
{
    textFoModList_.clear();
    for (Formatter::Fo* fo = first_fo(); fo; fo = fo->nextSibling())
        traverse_text_fos(fo, textFoModList_);
}

void OnlineSpellerImpl::toggleState(bool new_state)
{
    if (new_state == isOn_)
        return;
    isOn_ = new_state;
    refill_folist();
    if (new_state) { // switch ON
        forceModified_ = true;
        start_spelling();
        return;
    }
    timer_->stop();
    deregisterHandler(this);
    EditableView::FoModList::iterator it = textFoModList_.begin();
    for (; it != textFoModList_.end(); ++it) {
        if (!it->hasMarkedWords())
            continue;
        it->clearMarkedWords();
        update_fo_decoration(it);
    }
}

void OnlineSpellerImpl::recheck()
{
    if (!isOn_)
        return;
    toggleState(false);
    toggleState(true);
}

static const Formatter::TextFo* get_fo(const StructEditor* se)
{
    const GrovePos& gp = se->editViewFoPos();
    if (gp.isNull() || gp.type() != GrovePos::TEXT_POS)
        return 0;
    const Formatter::Chain* chain = Formatter::XslFoExt::areaChain(gp.node());
    if (chain->type() != Formatter::TEXT_FO)
        return 0;
    const Formatter::TextFo* text_fo = 
        static_cast<const Formatter::TextFo*>(chain);
    if (!text_fo->hasMarkedWords())
        return 0;
    return text_fo;
}

RangeString OnlineSpellerImpl::markedWord(GrovePos* src_pos) const
{
    const Formatter::TextFo* text_fo = get_fo(se_);
    if (!text_fo)
        return RangeString();
    const GrovePos& gp = se_->editViewFoPos();
    const Char* start = gp.text()->data().unicode();
    const Char* cp = start + text_fo->convertPos(gp.idx(), false);
    while (cp > start && WordTokenizer::tokenChar(*cp)) 
        cp--;
    WordTokenizer wt(cp, start + gp.text()->data().length());
    RangeString result;
    if (!wt.next(result) || spellCheckers_.isIgnored(result) ||
        !text_fo->isMarkedWord(result))
            return RangeString();
    // note: position is already converted to unstripped
    const GrovePos& src_gp = se_->editViewSrcPos();
    if (src_pos && src_gp.type() == GrovePos::TEXT_POS)
        *src_pos = GrovePos(src_gp.text(), 
            result.begin() - gp.text()->data().unicode());
    return result;
}

void OnlineSpellerImpl::updateMenu()
{
    RangeString word = markedWord();
    if (addAction_)
        addAction_->setBool(Sui::IS_ENABLED, !word.empty());
    if (ignoreAction_)
        ignoreAction_->setBool(Sui::IS_ENABLED, !word.empty());
    if (replaceAllMenu_)
        replaceAllMenu_->setBool(Sui::IS_ENABLED, false);
    if (suggestions_)
        suggestions_->removeAllChildren();
    if (replaceSuggestions_)
        replaceSuggestions_->removeAllChildren();
    if (word.empty())
        return;
    SpellChecker::Strings slist;
    spellCheckers_.getChecker(get_lang(se_->editViewFoPos().text())).
        suggest(word, slist);
    SpellChecker::Strings::iterator si = slist.begin();
    for (int i = 0; i < MAX_MENU_SUGGESTIONS && si != slist.end(); ++i, ++si) {
        PropertyNodePtr prop(new PropertyNode(Sui::ACTION));
        prop->makeDescendant(Sui::INSCRIPTION, *si, true);
        prop->makeDescendant(Sui::NAME, *si, true);
        prop->makeDescendant(Sui::TOOLTIP, 
            Sui::get_translated(NOTR("SpellerPlugin"), 
                    "Replace with: ") + *si, true);
        RefCntPtr<Sui::Action> sub_action = Sui::Action::make(&*prop);
        sub_action->setEnabled(true);
        if (suggestions_)
            suggestions_->appendChild(&*sub_action);
        sub_action = Sui::Action::make(&*prop);
        sub_action->setEnabled(true);
        if (replaceSuggestions_)
            replaceSuggestions_->appendChild(&*sub_action);
    }
    if (replaceAllMenu_)
        replaceAllMenu_->setBool(Sui::IS_ENABLED,
            !!replaceSuggestions_->firstChild());
}

void OnlineSpellerImpl::selectSuggestion()
{
    Sui::Action* act = suggestions_->activeSubAction();
    GrovePos wpos;
    RangeString word = markedWord(&wpos);
    if (!act || word.empty() || wpos.isNull())
        return;
    Editor::CommandPtr cmd = se_->groveEditor()->
        replaceText(wpos, word.length(), act->get(Sui::NAME));
    if (!cmd.isNull())
        se_->executeAndUpdate(&*cmd);
}

Formatter::Fo* OnlineSpellerImpl::first_fo() const
{
    const Formatter::Fo* rootfo = static_cast<const Formatter::Fo*>
        (se_->editableView().rootArea()->chain());
    return rootfo->firstChild(); 
}

typedef std::set<GroveLib::Text*> ReplaceNodeSet;

static void collect_replace(Formatter::Fo* fo, 
                            const RangeString& word,
                            ReplaceNodeSet& rs)
{
    if (fo->type() == Formatter::TEXT_FO) {
        Formatter::TextFo* text_fo = 
            static_cast<Formatter::TextFo*>(fo);
        if (text_fo->hasMarkedWords() && text_fo->isMarkedWord(word)) {
            GroveLib::Node* n = Xslt::resultOrigin(text_fo->headFoNode());
            if (n && n->nodeType() == GroveLib::Node::TEXT_NODE)
                rs.insert(TEXT_CAST(n));
        }
        return;
    }
    for (fo = fo->firstChild(); fo; fo = fo->nextSibling())
        collect_replace(fo, word, rs);
}

void OnlineSpellerImpl::replaceSuggestion()
{
    Sui::Action* act = replaceSuggestions_->activeSubAction();
    RangeString word = markedWord();
    if (!act || word.empty())
        return;
    String word_str(word.toString().lower());
    ReplaceNodeSet rset;
    for (Formatter::Fo* fo = first_fo(); fo; fo = fo->nextSibling())
        collect_replace(fo, word, rset);
    String replace_to(act->get(Sui::NAME));
    ReplaceNodeSet::iterator it = rset.begin();
    RefCntPtr<GroveBatchCommand> batch_cmd = new GroveBatchCommand;
    batch_cmd->setInfo(QString(Sui::get_translated(NOTR("SpellerPlugin"), 
        "Correct All %0 -> %1")).arg(word.toString()).arg(replace_to));
    for (; it != rset.end(); ++it) {
        GroveLib::Text* text_node = *it;
        int offset = 0;
        for (;;) {
            RangeString data(text_node->data());
            data.first += offset;
            WordTokenizer wt(data);
            RangeString rs;
            if (!wt.next(rs)) 
                break;
            if (rs.toString().lower() != word_str) {
                offset = rs.second - text_node->data().unicode();
                continue;
            }
            int pos = rs.first - text_node->data().unicode();
            CommandPtr cmd = se_->groveEditor()->replaceText(
                GrovePos(text_node, pos), word.length(), replace_to);
            offset = pos + replace_to.length();
            if (cmd.isNull())
                break;
            batch_cmd->executeAndAdd(&*cmd);
        }
    }
    if (batch_cmd->firstChild())
        se_->executeAndUpdate(&*batch_cmd);
}

void OnlineSpellerImpl::add(bool ignored)
{
    RangeString word = markedWord();
    if (word.empty())
        return;
    if (ignored) 
        spellCheckers_.addToIgnored(word);
    else {
        try {
        spellCheckers_.getChecker(get_lang(se_->editViewFoPos().text())).
            addToPersonal(word);
        } catch(...) {
            return;
        }
    }
    unmarkWord(word);
}

static void unmark_word(Formatter::Fo* fo, 
                        const RangeString& word)
{
    if (fo->type() == Formatter::TEXT_FO) {
        Formatter::TextFo* text_fo = 
            static_cast<Formatter::TextFo*>(fo);
        if (text_fo->hasMarkedWords() && text_fo->isMarkedWord(word)) {
            text_fo->unmarkWord(word);
            update_fo_decoration(text_fo);
        }
        return;
    }
    for (fo = fo->firstChild(); fo; fo = fo->nextSibling())
        unmark_word(fo, word);
}

void OnlineSpellerImpl::unmarkWord(const RangeString& word)
{
    for (Formatter::Fo* fo = first_fo(); fo; fo = fo->nextSibling())
        unmark_word(fo, word);
}

OnlineSpeller* OnlineSpeller::make(StructEditor* se, 
                                   DocumentPlugin* dp,
                                   bool init_state,
                                   SpellCheckerSet& scset)
{
    return new OnlineSpellerImpl(se, dp, init_state, scset);
}

///////////////////////////////////////////////////////

SpellerHoldoffTimer::SpellerHoldoffTimer(OnlineSpellerImpl& spi)
    : spi_(spi)
{
    connect(this, SIGNAL(timeout()), this, SLOT(timer_event()));
}

void SpellerHoldoffTimer::timer_event()
{
    spi_.update();
}

#include "moc/OnlineSpeller.moc"
