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

#include "speller_debug.h"
#include "SpellerAgent.h"
#include "DocSpeller.h"
#include "OnlineSpeller.h"
#include "SpellChecker.h"
#include "aspell/AspellLibrary.h"

#include "ui/UiItem.h"
#include "ui/UiItems.h"
#include "ui/UiItemSearch.h"
#include "ui/UiAction.h"

#include "utils/GrovePosEventData.h"
#include "common/PropertyTreeEventData.h"
#include "utils/SernaUiItems.h"
#include "utils/Config.h"
#include "utils/Properties.h"

#include "structeditor/StructEditor.h"
#include "structeditor/SetCursorEventData.h"
#include "editableview/EditableView.h"
#include "docview/DocumentPlugin.h"

#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"

#include "common/StrdupNew.h"
#include "common/OwnerPtr.h"

#include <stdexcept>

using namespace Common;
using namespace GroveEditor;

class StructEditor;
class DocSpeller;

class SpellerPlugin : public DocumentPlugin {
public:
    SpellerPlugin(SernaApiBase* doc, SernaApiBase* properties, char**);
    ~SpellerPlugin();

    virtual void            postInit();
    StructEditor*           structEditor() const { return se_; }
    DocSpeller*             getDocSpeller() const { return &*docSpeller_; }
    OnlineSpeller&          onlineSpeller() const { return *onlineSpeller_; }
    SpellCheckerSet&        spellerSet() const { return *spellCheckers_; }

private:
    SpellerPlugin();
    SpellerPlugin(const SpellerPlugin&);
    SpellerPlugin& operator=(const SpellerPlugin&);

    StructEditor*                   se_;
    Common::OwnerPtr<DocSpeller>    docSpeller_;
    Common::OwnerPtr<OnlineSpeller> onlineSpeller_;
    Common::OwnerPtr<SpellCheckerSet> spellCheckers_;
};

//////////////////////////////////////////////////////////////////////////

SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(SpellCheckEvent,  SpellerPlugin)
// auto spellcheck executors

SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(ToggleSpellCheck, SpellerPlugin)
SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(SelectDictionary, SpellerPlugin)
SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(UpdateSpellCheckMenu, SpellerPlugin)
SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(SelectSuggestion, SpellerPlugin)
SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(ReplaceSuggestion, SpellerPlugin)
SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(AddToPersonal, SpellerPlugin)
SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(AddToIgnored, SpellerPlugin)

SpellerPlugin::SpellerPlugin(SernaApiBase* doc, SernaApiBase* properties,
                             char** /*err*/)
 :  DocumentPlugin(doc, properties), se_(0)
{
    if (!AspellLibrary::instance().setConfig(pluginProperties())) {
        nstring errStr(utf8(AspellLibrary::instance().getLibError()));
        throw std::runtime_error(strdup_noarray(errStr.c_str()));
    }
    REGISTER_UI_EXECUTOR(SpellCheckEvent);
    REGISTER_UI_EXECUTOR(ToggleSpellCheck);
    REGISTER_UI_EXECUTOR(SelectDictionary);
    REGISTER_UI_EXECUTOR(UpdateSpellCheckMenu);
    REGISTER_UI_EXECUTOR(SelectSuggestion);
    REGISTER_UI_EXECUTOR(ReplaceSuggestion);
    REGISTER_UI_EXECUTOR(AddToPersonal);
    REGISTER_UI_EXECUTOR(AddToIgnored);
    buildPluginExecutors();
}

SpellerPlugin::~SpellerPlugin()
{
}

static const char SET_BY_USER[] = NOTR("set-by-user");

void SpellerPlugin::postInit()
{
    se_ = dynamic_cast<StructEditor*>(
        sernaDoc()->findItem(Sui::ItemClass(Sui::STRUCT_EDITOR)));
    SpellChecker::Status status;
    typedef SpellChecker::Strings Strings;
    Strings dlist;
    Strings::iterator it;
    try {
        SpellChecker::getDictList(dlist, &status);
    } catch (...) {}
    if (pluginProperties()->getProperty("ui")) {
        spellCheckers_ = new SpellCheckerSet;
        Sui::Action* toggle_act = 
            findPluginUiAction(NOTR("toggleSpellCheck"));
        if (toggle_act && !toggle_act->getBool(SET_BY_USER)) {
            PropertyNode* init_sprop = config().root()->
                makeDescendant(Speller::SPELLER)->
                makeDescendant(Speller::SPELLER_AUTO, "true", false);
            toggle_act->setToggled(init_sprop && init_sprop->getBool());
        }
        onlineSpeller_ = OnlineSpeller::make(se_, this,
            toggle_act ? toggle_act->getBool(Sui::IS_TOGGLED) : false,
            *spellCheckers_);
        docSpeller_ = DocSpeller::make(se_, *spellCheckers_, &*onlineSpeller_);
        Sui::Action* lang_act = findPluginUiAction(NOTR("dictionaries"));
        if (!lang_act)
            return;
        lang_act->removeAllChildren();
        for (it = dlist.begin(); it != dlist.end(); ++it) {
            PropertyNodePtr prop(new PropertyNode(Sui::ACTION));
            prop->makeDescendant(Sui::INSCRIPTION, *it, true);
            prop->makeDescendant(Sui::NAME, *it, true);
            prop->makeDescendant(Sui::TOOLTIP, 
                Sui::get_translated(NOTR("SpellerPlugin"), 
                    "Dictionary: ") + *it, true);
            Sui::Action* sub_action = Sui::Action::make(&*prop);
            sub_action->setEnabled(true);
            lang_act->appendChild(sub_action);
        }
        lang_act->setActiveSubAction(
            spellCheckers_->defaultChecker().getDict());
        return;
    }
    PropertyNode* languages = config().root()->makeDescendant(
        Speller::SPELLER)->makeDescendant(Speller::SPELLER_LANGUAGES);
    if (!languages->getString().isEmpty())
        return;
    for (it = dlist.begin(); it != dlist.end(); ++it) {
        if (!languages->getString().isEmpty())
            languages->setString(languages->getString() + ' ');
        languages->setString(languages->getString() + *it);
    }
}

static SpellerAgent* find_speller_item(SernaDoc* sdoc)
{
    Sui::Item* sp_item = sdoc->findItem(Sui::ItemClass(Sui::SPELLER));
    return dynamic_cast<SpellerAgent*>(sp_item);
}

static SpellerAgent* make_speller_item(SernaDoc* doc, DocSpeller* speller)
{
    SpellerAgent* agent = new SpellerAgent(speller);
    doc->appendChild(agent);
    agent->attach();
    return agent;
}

void SpellCheckEvent::execute()
{
    DDBG << "SpellCheckEvent::doExecute(), this:" << abr(this) << std::endl;

    DocSpeller* speller = plugin()->getDocSpeller();
    if (speller && !speller->start()) {
        SpellerAgent* agent = find_speller_item(plugin()->sernaDoc());
        if (0 == agent)
            agent = make_speller_item(plugin()->sernaDoc(), speller);
        else
            agent->setSpeller(*speller);
        agent->setBool(Sui::IS_VISIBLE, true);
        agent->show();
    }
}

//////////////////////////////////////////////////////////////////////
//
// online spell-check actions
//

void UpdateSpellCheckMenu::execute()
{
    plugin()->onlineSpeller().updateMenu();        
}

void SelectSuggestion::execute()
{
    plugin()->onlineSpeller().selectSuggestion();
}

void ReplaceSuggestion::execute()
{
    plugin()->onlineSpeller().replaceSuggestion();
}

void AddToPersonal::execute()
{
    plugin()->onlineSpeller().add(false);
}

void AddToIgnored::execute()
{
    plugin()->onlineSpeller().add(true);
}

void ToggleSpellCheck::execute()
{
    plugin()->onlineSpeller().toggleState(
        uiAction()->getBool(Sui::IS_TOGGLED));
    uiAction()->setBool(SET_BY_USER, true);
}

void SelectDictionary::execute()
{
    const Sui::Action* act = uiAction()->activeSubAction();
    if (act) {
        plugin()->spellerSet().setDict(act->get(Sui::NAME));
        plugin()->onlineSpeller().recheck();
    }
}

//////////////////////////////////////////////////////////////////////

DEFINE_PLUGIN_CLASS(SpellerPlugin)

