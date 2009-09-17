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

#include "SpellerAgent.h"
#include "DocSpeller.h"
#include "qt/SpellCheckDialog.h"

#include "utils/SernaUiItems.h"
#include "utils/tr.h"

#include "common/PropertyTree.h"
#include "common/CommandEvent.h"

using namespace Common;

SpellerAgent::SpellerAgent(DocSpeller* dsp)
    : LiquidItem(0), speller_(dsp), dlg_(0)
{
    itemProps()->makeDescendant(Sui::NAME, NOTR("SpellCheckDialog"));
    itemProps()->makeDescendant(Sui::INSCRIPTION)->setString(
        tr("Check Spelling"));
    itemProps()->makeDescendant("caption", tr("Spell Check"), false);
}

void SpellerAgent::update_dlg()
{
    dlg_->clearSuggestions();
    PropertyNode* ptn = get_prop(NOTR("suggestions"));
    for (ptn = ptn->firstChild(); 0 != ptn; ptn = ptn->nextSibling()) {
        dlg_->addToSuggestions(ptn->getString());
    }
    dlg_->setMisspell(get_prop(NOTR("misspell"))->getString());
    String lang(get_prop(NOTR("language"))->getString());
    if (!lang.empty())
        dlg_->setDict(lang);
}

bool SpellerAgent::checkResult(bool rv)
{
    if (rv)
        shutdown();
    else {
        update_dlg();
    }
    return rv;
}

QWidget* SpellerAgent::makeWidget(QWidget* parent, LiquidItem::Type type)
{
    dlg_ = makeSpellCheckDialog(parent, this, type);
    PropertyNode* ptn = get_prop(NOTR("languages"))->firstChild();
    for (; 0 != ptn; ptn = ptn->nextSibling()) {
        dlg_->addDict(ptn->getString());
    }
    update_dlg();
    return dlg_->getWidget();
}

void SpellerAgent::setSpeller(DocSpeller& dsp)
{
    speller_ = &dsp;
    if (0 != dlg_) {
        if (dlg_->getMisspell() != get_prop(NOTR("misspell"))->getString())
            update_dlg();
    }
}

void SpellerAgent::show()
{
    if (0 != dlg_)
        dlg_->exec();
}

String SpellerAgent::itemClass() const
{
    return from_latin1(Sui::SPELLER);
}

PropertyNode* SpellerAgent::get_prop(const String& pname)
{
    return getProps()->makeDescendant(pname);
}

//////////////////////////////////////////////////////////////////

bool SpellerAgent::start()
{
    show();
    return true;
}

bool SpellerAgent::shutdown()
{
    speller_->shutdown();
    remove();
    return true;
}

/////////////////////////////////////////////////////////////////

bool SpellerAgent::ignore(const RangeString& word)
{
    return checkResult(speller_->ignore(word));
}

bool SpellerAgent::ignoreAll(const RangeString& word)
{
    return checkResult(speller_->ignoreAll(word));
}

bool SpellerAgent::add(const RangeString& word)
{
    return checkResult(speller_->add(word));
}

bool SpellerAgent::change(const RangeString& word, const RangeString& repl)
{
    return checkResult(speller_->change(word, repl));
}

bool SpellerAgent::changeAll(const RangeString& word, const RangeString& repl)
{
    return checkResult(speller_->changeAll(word, repl));
}

bool SpellerAgent::skipElement()
{
    return checkResult(speller_->skipElement());
}

bool SpellerAgent::setDict(const RangeString& dict)
{
    return checkResult(speller_->setDict(dict));
}

PropertyNode* SpellerAgent::getProps() const
{
    return speller_->getProps();
}

//////////////////////////////////////////////////////////

SpellerAgent::~SpellerAgent()
{
    dlg_ = 0;
}


