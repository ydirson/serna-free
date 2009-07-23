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
#include "SpellerPlugin.h"
#include "speller_debug.h"
#include "SpellerAgent.h"
#include "DocSpeller.h"
#include "SpellChecker.h"
#include "aspell/AspellLibrary.h"

#include "ui/UiItem.h"
#include "ui/UiItems.h"
#include "ui/UiItemSearch.h"

#include "utils/GrovePosEventData.h"
#include "common/PropertyTreeEventData.h"
#include "utils/SernaUiItems.h"
#include "utils/Config.h"
#include "utils/Properties.h"

#include "structeditor/StructEditor.h"
#include "structeditor/SetCursorEventData.h"
#include "editableview/EditableView.h"

#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"

#include "common/StrdupNew.h"

#include <stdexcept>

using namespace Common;

//////////////////////////////////////////////////////////////////////////

SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(SpellCheckEvent, SpellerPlugin)

SpellerPlugin::SpellerPlugin(SernaApiBase* doc, SernaApiBase* properties,
                             char** err)
 :  DocumentPlugin(doc, properties), se_(0)
{
    if (!AspellLibrary::instance().setConfig(pluginProperties())) {
        nstring errStr(utf8(AspellLibrary::instance().getLibError()));
        throw std::runtime_error(strdup_noarray(errStr.c_str()));
    }
    REGISTER_UI_EXECUTOR(SpellCheckEvent);
    buildPluginExecutors();
}

SpellerPlugin::~SpellerPlugin()
{
}

void SpellerPlugin::postInit()
{
    se_ = dynamic_cast<StructEditor*>(
        sernaDoc()->findItem(Sui::ItemClass(Sui::STRUCT_EDITOR)));
    if (pluginProperties()->getProperty("ui")) {
        docSpeller_ = DocSpeller::make(se_);
        return;
    }
    PropertyNode* languages = config().root()->makeDescendant(
        Speller::SPELLER)->makeDescendant(Speller::SPELLER_LANGUAGES);
    if (!languages->getString().isEmpty())
        return;
    try {
        SpellChecker::Status status;
        typedef SpellChecker::Strings Strings;
        Strings dlist(SpellChecker::getDictList(&status));
        Strings::iterator it = dlist.begin();
        for (; it != dlist.end(); ++it) {
            if (!languages->getString().isEmpty())
                languages->setString(languages->getString() + ' ');
            languages->setString(languages->getString() + *it);
        }
    }
    catch (...) {}
}

static SpellerAgent* find_speller_item(SernaDoc* sdoc)
{
    Sui::Item* sp_item = sdoc->findItem(Sui::ItemClass(Sui::SPELLER));
    return dynamic_cast<SpellerAgent*>(sp_item);
}

static SpellerAgent* make_speller_item(SernaDoc* doc, DocSpeller* speller)
{
    SpellerAgent* agent = SpellerAgent::make(speller);
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

DEFINE_PLUGIN_CLASS(SpellerPlugin)

