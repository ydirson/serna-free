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
#include "structeditor/impl/debug_se.h"

#include "common/CommandEvent.h"
#include "common/PathName.h"
#include "common/PropertyTreeEventData.h"
#include "utils/GrovePosEventData.h"
#include "utils/DocSrcInfo.h"
#include "utils/file_utils.h"
#include "utils/GroveProperty.h"
#include "utils/Config.h"
#include "docview/SernaDoc.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"
#include "grove/xinclude.h"
#include "grove/SectionSyncher.h"

#include "editableview/EditableView.h"

#include <qapplication.h>

using namespace Common;
using namespace FileUtils;

//////////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(InsertXincludeXml,  StructEditor)
SIMPLE_COMMAND_EVENT_IMPL(InsertXincludeText, StructEditor)

class InsertXincludeDialog;

static bool insert_xinclude(StructEditor* se, bool isText)
{
    GroveEditor::GrovePos pos;
    if (!se->getCheckedPos(pos))
        return false;
    //! Ask user for xinclude parameters
    PropertyTreeEventData data;
    data.root()->makeDescendant("caption")->setString(
        (isText)
        ? qApp->translate("StructEditor", "XInclude Text File")
        : qApp->translate("StructEditor", "XInclude XML File"));
    if (isText)
        data.root()->makeDescendant("is-text");
    if (!makeCommand<InsertXincludeDialog>(&data)->execute(se, &data))
        return false;

    //! Get parsed document grove
    GroveProperty* grove_prop = dynamic_cast<GroveProperty*>(
        data.root()->getProperty(DocSrcInfo::TEMPLATE_DOCROOTGROVE));
    if (0 == grove_prop)
        return false;

    if (!isText)
        se->stripInfo()->strip(
            grove_prop->grove()->document()->documentElement());

    if (se->executeAndUpdate(
            se->groveEditor()->insertXinclude(
                pos, grove_prop->grove(), isText, data.root()))) {
        doTimeStamps(se->grove(),
                     se->getDsi()->makeDescendant(DocSrcInfo::MODIFIED_TIME));
        return true;
    }
    return false;
}

bool InsertXincludeXml::doExecute(StructEditor* se, EventData*)
{
    return insert_xinclude(se, false);
}

bool InsertXincludeText::doExecute(StructEditor* se, EventData*)
{
    return insert_xinclude(se, true);
}

//////////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(ConvertToXinclude,  StructEditor)

bool ConvertToXinclude::doExecute(StructEditor* se, EventData*)
{
    GroveEditor::GrovePos from, to;
    //! Check selection
    if (StructEditor::POS_OK != se->getSelection(from, to,
        StructEditor::ANY_OP|StructEditor::NOROOT_OP))
            return false;
    if (from.node() != to.node() ||
        (GroveEditor::GrovePos::TEXT_POS != from.type() &&
         (!from.before() || from.before()->nextSibling() != to.before()))) {
        se->sernaDoc()->showMessageBox(
            SernaDoc::MB_CRITICAL, tr("Cannot Convert to Xinclude"),
            tr("Select exactly one element or text"), tr("&Ok"));
        return false;
    }
    GroveLib::GrovePtr grove = se->groveEditor()->buildXincludeGrove(from, to);
    if (grove.isNull()) {
        se->sernaDoc()->showMessageBox(
            SernaDoc::MB_CRITICAL, tr("Cannot Convert to Xinclude"),
            se->groveEditor()->errorMessage(), tr("&Ok"));
        return false;
    }
    const bool is_text =
        (grove->topDecl()->dataType() == GroveLib::EntityDecl::cdata);

    //! Ask user for xinclude parameters
    PropertyTreeEventData data;
    if (is_text)
        data.root()->makeDescendant("is-text");
    data.root()->makeDescendant("is-new");
    data.root()->makeDescendant("caption")->setString(
        (is_text)
        ? tr("Convert to TEXT Xinclude") : tr("Convert to XML Xinclude"));
    if (!makeCommand<InsertXincludeDialog>(&data)->execute(se, &data))
        return false;

    if (se->executeAndUpdate(se->groveEditor()->convertToXinclude(
                                 from, to, grove, data.root()))) {
        doTimeStamps(se->grove(),
                     se->getDsi()->makeDescendant(DocSrcInfo::MODIFIED_TIME));
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////
