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
#include "structeditor/StructEditor.h"
#include "docview/EventTranslator.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "common/CommandExecutor.h"
#include "genui/StructDocumentActions.hpp"

using namespace Common;

static const int MAX_UNDO_REDO_DEPTH = 10;

//////////////////////////////////////////////////////////////////////////

UICMD_EVENT_IMPL(StructUndoRedo, StructEditor)

bool StructUndoRedo::doExecute(StructEditor* structEditor, EventData*)
{
    if (activeSubAction()) 
        structEditor->doUntil(activeSubAction()->getInt(Sui::NAME));
    return true;
}

/////////////////////////////////////////////////////////////////////////

void update_undo_menu(StructEditor* se, bool isUndo)
{
    Sui::Action* menu_action = isUndo ?
        se->uiActions().undoMenuCmd() : se->uiActions().redoMenuCmd();
    menu_action->removeAllChildren();
    Command *first(0), *last(0);
    if (isUndo)
        se->executor()->getUndoCommandList(last, first);
    else
        se->executor()->getRedoCommandList(first, last);
    int depth = 1;
    while (first && depth < MAX_UNDO_REDO_DEPTH) {
        QString inscription = 
            Sui::get_translated("GroveEditor", first->info(0));
        for (uint i = 1;; ++i) {
            String arg = first->info(i);
            if (arg.isNull())
                break;
            inscription = inscription.arg(arg);
        }
        if (inscription.length() > 40)
            inscription = inscription.left(40) + NOTR(" ...");
        PropertyNodePtr prop(new PropertyNode(Sui::ACTION));
        prop->makeDescendant(Sui::INSCRIPTION, inscription, true);
        prop->makeDescendant(Sui::NAME)->setInt(isUndo ? -depth : depth);
        Sui::Action* sub_action = Sui::Action::make(prop.pointer());
        sub_action->setEnabled(true);
        menu_action->appendChild(sub_action);
        if (first == last)
            break;
        first = isUndo ? first->prevSibling() : first->nextSibling();
        ++depth;
    }
    const bool is_enabled = !!menu_action->firstChild();
    (isUndo ? se->uiActions().undoMenuUpdateCmd()
        : se->uiActions().redoMenuUpdateCmd())->setEnabled(is_enabled);
}

SIMPLE_COMMAND_EVENT_IMPL(UpdateUndoMenu, StructEditor)
SIMPLE_COMMAND_EVENT_IMPL(UpdateRedoMenu, StructEditor)

bool UpdateUndoMenu::doExecute(StructEditor* se, EventData*)
{
    update_undo_menu(se, true);
    return true;
}

bool UpdateRedoMenu::doExecute(StructEditor* se, EventData*)
{
    update_undo_menu(se, false);
    return true;
}
