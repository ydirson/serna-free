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
#include "common/PropertyTree.h"
#include "common/PathName.h"
#include "common/PropertyTreeEventData.h"
#include "utils/DocSrcInfo.h"
#include "utils/Config.h"
#include "ui/ActionSet.h"

using namespace Common;

UICMD_EVENT_IMPL(OpenRecentDocument, SernaDoc)

bool OpenRecentDocument::doExecute(SernaDoc* sernaDoc, EventData*)
{
    if (activeSubAction()) {
        PropertyNode* dsi = 
            DocSrcInfo::get_dsi(activeSubAction()->getInt(Sui::NAME));
        if (!dsi)
            return false;
        PropertyTreeEventData dsi_data(dsi);
        String aname = dsi->getProperty(DocSrcInfo::IS_TEXT_DOCUMENT) ?
            NOTR("openPlainDocumentWithDsi") :
            NOTR("openStructDocumentWithDsi");
        Sui::Action* action = sernaDoc->actionSet()->findAction(aname);
        if (action) {
            EventTranslator& event_translator =
                *dynamic_cast<EventTranslator*>(sernaDoc->dispatcher());
            CommandEventPtr (*ev)(const EventData*);
            ev = event_translator.findEventMaker(action);
            if (ev)
                return (*ev)(&dsi_data)->execute(sernaDoc);
        }
    }
    return false;
}

SIMPLE_COMMAND_EVENT_IMPL(UpdateRecentFilesMenu, SernaDoc)

bool UpdateRecentFilesMenu::doExecute(SernaDoc* sernaDoc, EventData*)
{
    const PropertyNode* dsi_list =
        config().getProperty(DocSrcInfo::RECENT_LIST);
    Sui::Action* recent_menu_action =
        sernaDoc->actionSet()->findAction(NOTR("openRecentFile"));
    if (!dsi_list || !recent_menu_action)
        return false;
    recent_menu_action->removeAllChildren();
    uint num = 1;
    for (PropertyNode* i = dsi_list->firstChild(); i; i = i->nextSibling()) {
        if (DocSrcInfo::DSI != i->name())
            continue;
        PathName path(i->getSafeProperty("doc-src")->getString());
        String inscription =
            '&' + String::number(num) + "  " + String(path.filename());
        PropertyNodePtr pt(new PropertyNode(Sui::ACTION));
        pt->makeDescendant(Sui::NAME)->setInt(num); 
        pt->makeDescendant(Sui::INSCRIPTION, inscription, true);
        pt->makeDescendant(Sui::TOOLTIP, DocSrcInfo::makeDsiTooltip(i), true);
        Sui::Action* action = Sui::Action::make(pt.pointer());
        action->setEnabled(true);
        recent_menu_action->appendChild(action);
        num++;
    }
    const bool is_enabled = !!recent_menu_action->firstChild();
    recent_menu_action = 
        sernaDoc->actionSet()->findAction(NOTR("recentFilesMenu"));
    if (recent_menu_action)
        recent_menu_action->setEnabled(is_enabled);
    return true;
}
