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
#include "docview/SernaDoc.h"
#include "docview/EventTranslator.h"
#include "utils/SernaUiItems.h"
#include "ui/ActionSet.h"
#include "ui/UiItems.h"

using namespace Common;

UICMD_EVENT_IMPL(ToggleToolBar, SernaDoc)

bool ToggleToolBar::doExecute(SernaDoc* sernaDoc, EventData*)
{
    Sui::Action* action = activeSubAction();
    if (!action)
        return false;
    Sui::Item* item = sernaDoc->firstChild(); 
    for (; item; item = item->nextSibling()) 
        if (action->get(Sui::NAME) == item->get(Sui::NAME)) {
            item->setBool(Sui::IS_VISIBLE, action->getBool(Sui::IS_TOGGLED));
            return true;
        }
    return false;
}

SIMPLE_COMMAND_EVENT_IMPL(UpdateToolbarsMenu, SernaDoc)

bool UpdateToolbarsMenu::doExecute(SernaDoc* sernaDoc, EventData*)
{
    Sui::Action* menu_action =
        sernaDoc->actionSet()->findAction(NOTR("toolbarsMenuCmd"));
    if (!menu_action)
        return false;
    menu_action->removeAllChildren();
    Sui::Item* item = sernaDoc->firstChild();
    for (; item; item = item->nextSibling()) {
        if (item->itemClass() != Sui::TOOL_BAR)
            continue;
        PropertyNodePtr pt(new PropertyNode(Sui::ACTION));
        pt->makeDescendant(Sui::NAME, item->get(Sui::NAME), true);
        pt->makeDescendant(Sui::INSCRIPTION, 
            item->get(Sui::INSCRIPTION), true);
        pt->makeDescendant(Sui::IS_TOGGLEABLE)->setBool(true);
        pt->makeDescendant(Sui::IS_TOGGLED)->setBool(
            item->getBool(Sui::IS_VISIBLE));
        Sui::Action* action = Sui::Action::make(pt.pointer());
        action->setEnabled(true);
        menu_action->appendChild(action);
    }
    return true;
}
