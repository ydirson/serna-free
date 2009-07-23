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

#include "SpellerAgent.h"
#include "SpellerReactor.h"

#include "qt/SpellCheckDialog.h"
#include "DocSpeller.h"

#include "common/PropertyTree.h"
#include "common/CommandEvent.h"
#include "common/OwnerPtr.h"
#include "common/StringCvt.h"
#include "utils/tr.h"

#include <qwidget.h>

using namespace Common;

class SpellerAgentImpl : public SpellerAgent, private SpellerReactor {
public:
    SpellerAgentImpl(DocSpeller* dsp);
    virtual ~SpellerAgentImpl();
protected:
    //! Inherited from SpellerAgent
    virtual void doSetSpeller(DocSpeller& dsp);
    virtual void doShow();
    //! Inherited from SpellerReactor
    virtual bool ignore(const UCRange&);
    virtual bool ignoreAll(const UCRange&);
    virtual bool add(const UCRange&);
    virtual bool change(const UCRange& word, const UCRange& repl);
    virtual bool changeAll(const UCRange& word, const UCRange& repl);
    virtual bool skipElement();
    virtual bool setDict(const UCRange&);
    virtual bool start();
    virtual bool shutdown();
    virtual PropertyNode* getProps() const;
    //!
    virtual QWidget* makeWidget(QWidget* parent, LiquidItem::Type dlgtype);
    //!
private:
    template <unsigned N> PropertyNode& get_props(const char (&nm)[N])
    {
        return get_props(from_latin1(nm, str_length(nm)));
    }
    PropertyNode& get_props(const String& pname)
    {
        return *speller_->getProps()->makeDescendant(pname, String::null(),
                                                     false);
    }
    void update_dlg();
    bool checkResult(bool rv);
    //!
    DocSpeller*         speller_;
    SpellCheckDialog*   dlg_;
};

SpellerAgent* SpellerAgent::make(DocSpeller* dsp)
{
    return new SpellerAgentImpl(dsp);
}

SpellerAgentImpl::SpellerAgentImpl(DocSpeller* dsp)
 :  speller_(dsp), dlg_(0)
{
    itemProps()->makeDescendant("caption", tr("Spell Check"), false);
}

SpellerAgentImpl::~SpellerAgentImpl()
{
    dlg_ = 0;
}

void SpellerAgentImpl::update_dlg()
{
    dlg_->clearSuggestions();
    PropertyNode* ptn = &get_props(NOTR("suggestions"));
    for (ptn = ptn->firstChild(); 0 != ptn; ptn = ptn->nextSibling()) {
        dlg_->addToSuggestions(ptn->getString());
    }
    dlg_->setMisspell(get_props(NOTR("misspell")).getString());
    String lang(get_props(NOTR("language")).getString());
    if (!lang.empty())
        dlg_->setDict(lang);
}

bool SpellerAgentImpl::checkResult(bool rv)
{
    if (rv)
        shutdown();
    else {
        update_dlg();
    }
    return rv;
}

QWidget* SpellerAgentImpl::makeWidget(QWidget* parent, LiquidItem::Type type)
{
    dlg_ = makeSpellCheckDialog(parent, this, type);
    PropertyNode* ptn = get_props(NOTR("languages")).firstChild();
    for (; 0 != ptn; ptn = ptn->nextSibling()) {
        dlg_->addDict(ptn->getString());
    }
    update_dlg();
    //String caption(from_string<ustring>(dlg_->getWidget()->caption()));
    //itemProps()->makeDescendant("caption", caption, false);
    return dlg_->getWidget();
}

void SpellerAgentImpl::doSetSpeller(DocSpeller& dsp)
{
    speller_ = &dsp;
    if (0 != dlg_) {
        if (dlg_->getMisspell() != get_props(NOTR("misspell")).getString())
            update_dlg();
    }
}

void SpellerAgentImpl::doShow()
{
    if (0 != dlg_)
        dlg_->exec();
}

/////////// Implementation of the SpellerReactor interface ///////////

bool SpellerAgentImpl::start()
{
    show();
    return true;
}

bool SpellerAgentImpl::shutdown()
{
    speller_->shutdown();
    remove();
    return true;
}

bool SpellerAgentImpl::ignore(const UCRange& word)
{
    return checkResult(speller_->ignore(word));
}

bool SpellerAgentImpl::ignoreAll(const UCRange& word)
{
    return checkResult(speller_->ignoreAll(word));
}

bool SpellerAgentImpl::add(const UCRange& word)
{
    return checkResult(speller_->add(word));
}

bool SpellerAgentImpl::change(const UCRange& word, const UCRange& repl)
{
    return checkResult(speller_->change(word, repl));
}

bool SpellerAgentImpl::changeAll(const UCRange& word, const UCRange& repl)
{
    return checkResult(speller_->changeAll(word, repl));
}

bool SpellerAgentImpl::skipElement()
{
    return checkResult(speller_->skipElement());
}

bool SpellerAgentImpl::setDict(const UCRange& dict)
{
    return checkResult(speller_->setDict(dict));
}

PropertyNode* SpellerAgentImpl::getProps() const
{
    return speller_->getProps();
}

