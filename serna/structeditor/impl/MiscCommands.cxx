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
#include "structeditor/LiveNodeLocator.h"
#include "structeditor/impl/XsUtils.h"
#include "structeditor/StructDocument.h"
#include "docview/MessageView.h"
#include "genui/StructDocumentActions.hpp"

#include "common/CommandEvent.h"
#include "common/Url.h"
#include "common/OwnerPtr.h"

#include "common/PropertyTreeEventData.h"
#include "utils/Config.h"
#include "utils/DocSrcInfo.h"
#include "utils/HelpAssistant.h"
#include "utils/ElementHelp.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"

#include "grove/Grove.h"
#include "grove/Nodes.h"
#include "grove/ValidationCommandsMaker.h"

#include "xs/Schema.h"

#include "editableview/EditableView.h"

#include "xslt/Engine.h"
#include "xslt/TopParamValueHolder.h"
#include "xpath/ConstValueHolder.h"

#include <qapplication.h>
#include <iostream>

using namespace Common;
using namespace GroveEditor;
using namespace Formatter;

void update_message_view(StructEditor* se)
{
    MessageView* mv =
        static_cast<StructDocument*>(se->sernaDoc())->messageView();
    if (mv)
        mv->update();
}

SIMPLE_COMMAND_EVENT_IMPL(SaveFoTree, StructEditor)

bool SaveFoTree::doExecute(StructEditor* se, EventData*)
{
    using namespace GroveLib;

    Url sysid;
    String configFoPath = config().getProperty("app/fo-path")->getString();
    if (configFoPath.isEmpty())
        sysid = se->grove()->topSysid();
    else
        sysid = configFoPath;
    sysid = sysid.combinePath2Path(Url(NOTR("serna-fot.xml")));
    se->fot()->saveAsXmlFile
        (Grove::GS_DEF_FILEFLAGS|Grove::GS_INDENT, 0, sysid);
    se->sernaDoc()->showMessageBox(SernaDoc::MB_INFO,
        tr("FO Tree Saved"),
        tr("<qt>FO Tree was saved to:<br/><nobr>%1"
           "</nobr></qt>").arg(String(sysid)), tr("&OK"));
    return true;
}

///////////////////////////////////////////////////////////////////////

static bool revalidate(StructEditor* se, bool silent = false)
{
    if (!se->schema())
        return true;
    se->messageTree()->clearMessagesFrom(6);    // validator
    GroveLib::ElementPtr top_elem = se->grove()->document()->documentElement();
    Schema::vFlags vf = se->schema()->validationFlags();
    se->schema()->setValidationFlags(vf | Schema::dontMakeElems);
    OwnerPtr<GroveLib::NullValidationCommandsMaker> ncm;
    ncm = new GroveLib::NullValidationCommandsMaker;
    se->schema()->setCommandMaker(ncm.pointer());
    bool ok = se->schema()->validate(top_elem.pointer());
    se->schema()->setValidationFlags(vf);
    se->schema()->setCommandMaker(0);
    update_message_view(se);
    if (!silent && ok)
        se->sernaDoc()->showMessageBox(
            SernaDoc::MB_INFO, "",
            qApp->translate("StructEditor", "Document is valid."), tr("&OK"));
    return true;
}


SIMPLE_COMMAND_EVENT_IMPL(Revalidate, StructEditor)

bool Revalidate::doExecute(StructEditor* se, EventData*)
{
    return revalidate(se);
}

SIMPLE_COMMAND_EVENT_IMPL(RevalidateSilent, StructEditor)

bool RevalidateSilent::doExecute(StructEditor* se, EventData*)
{
    return revalidate(se, true);
}

///////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(SwitchValidation, StructEditor)

bool SwitchValidation::doExecute(StructEditor* se, EventData*)
{
    bool wasOn = se->isValidationOn();
    se->setValidationMode();
    if (se->isValidationOn() && !wasOn)
        makeCommand<RevalidateSilent>()->execute(se);
    return true;
}

///////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(ToggleMarkup, StructEditor)

bool ToggleMarkup::doExecute(StructEditor* se, EventData*)
{
    PropertyNode* dsi = const_cast<PropertyNode*>(se->getDsi());
    bool showTags = se->uiActions().toggleMarkup()->getBool(Sui::IS_TOGGLED);
    dsi->makeDescendant(DocSrcInfo::SHOW_TAGS)->setBool(showTags);
    GroveEditor::GrovePos fo_pos = se->editViewFoPos();
    se->format();
    se->setCursor(se->toAreaPos(fo_pos), true);
    se->sernaDoc()->showStageInfo();
    update_message_view(se);
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(TogglePageMode, StructEditor)

bool TogglePageMode::doExecute(StructEditor* se, EventData*)
{
    PropertyNode* dsi = const_cast<PropertyNode*>(se->getDsi());
    bool is_paginated =
        se->uiActions().togglePageMode()->getBool(Sui::IS_TOGGLED);
    dsi->makeDescendant(DocSrcInfo::SHOW_PAGINATED)->setBool(is_paginated);
    GroveEditor::GrovePos fo_pos = se->editViewFoPos();
    se->format();
    se->setCursor(se->toAreaPos(fo_pos), true);
    se->sernaDoc()->showStageInfo();
    update_message_view(se);
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(ToggleSelectionBalancing, StructEditor)

bool ToggleSelectionBalancing::doExecute(StructEditor* se, EventData*)
{
    PropertyNode* dsi = const_cast<PropertyNode*>(se->getDsi());
    bool balanced =
        se->uiActions().balanceSelection()->getBool(Sui::IS_TOGGLED);
    dsi->makeDescendant(DocSrcInfo::SELECTION_BALANCING)->setBool(balanced);
    se->editableView().setSelectionBalancing(balanced);

    Selection selection = se->editableView().getSelection();
    const Area* root_area = se->editableView().rootArea();
    se->notifySelectionChange(
        ChainSelection(selection.tree_.start().toAreaPos(root_area),
                       selection.tree_.end().toAreaPos(root_area)),
        selection.src_);
    return true;
}

///////////////////////////////////////////////////////////////////////

// START_IGNORE_LITERALS
static const char* xpath_type_list[] = {
    "undefined", "bool", "string", "numeric", "undefined", 0
};
// STOP_IGNORE_LITERALS

PROPTREE_EVENT_IMPL(SetXsltParams, StructEditor)
REGISTER_COMMAND_EVENT_MAKER(SetXsltParams, "PropertyTree", "-")

bool SetXsltParams::doExecute(StructEditor* se, EventData*)
{
    const PropertyNode* pn = ed_->firstChild();
    Xslt::TopParamValueHolder* vh = 0;
    bool value_set = false;
    for (; pn; pn = pn->nextSibling()) {
        vh = se->xsltEngine()->getTopParamValueHolder(pn->name());
        if (0 == vh)
            continue;
        const PropertyNode* pval = pn->getProperty("value");
        Xpath::ValueHolder* new_value = 0;
        if (pval) {
            String sv = pval->getString();
            if (vh->value()->to(Xpath::Value::STRING)->getString() == sv)
                continue;   // value unchanged
            String type;
            if (pn->getProperty("type"))
                type = pn->getSafeProperty("type")->getString();
            else
                type = pn->getSafeProperty("guessed-type")->getString();
            if (type == NOTR("bool"))
                new_value = Xpath::ConstValueHolder::makeBool(sv.toInt());
            else if (type == NOTR("string"))
                new_value = Xpath::ConstValueHolder::makeString(sv);
            else if (type ==NOTR( "numeric"))
                new_value = Xpath::ConstValueHolder::makeDouble(sv.toDouble());
            else
                continue;
        }
        vh->setNewInst(new_value);
        value_set = true;
    }
    if (!value_set)
        return true;
    GroveEditor::GrovePos src_pos(se->editViewSrcPos());
    LiveNodeLocator foHint(se->editViewFoPos());
    se->removeSelection();
    se->update();
    GroveEditor::GrovePos result_pos =
        se->toResultPos(src_pos, foHint.node());
    if (!result_pos.isNull())
        se->setCursor(se->toAreaPos(result_pos), true);
    return true;
}

PROPTREE_EVENT_IMPL(GetXsltParams, StructEditor)
REGISTER_COMMAND_EVENT_MAKER(GetXsltParams, "-", "PropertyTree")

bool GetXsltParams::doExecute(StructEditor* se, EventData* ed)
{
    PropertyTreeEventData* ped = static_cast<PropertyTreeEventData*>(ed);
    Xslt::TopParamValueHolder* vh = 0;
    se->xsltEngine()->getTopParamList(ped->root());
    PropertyNode* pn = ped->root()->firstChild();
    while (pn) {
        vh = se->xsltEngine()->getTopParamValueHolder(pn->name());
        if (0 == vh) {
            PropertyNode* next = pn->nextSibling();
            pn->remove();
            pn = next;
            continue;
        }
        pn->makeDescendant("value")->
            setString(vh->value()->to(Xpath::Value::STRING)->getString());
        pn->makeDescendant("guessed-type")->
            setString(xpath_type_list[(int)vh->value()->type()]);
        pn = pn->nextSibling();
    }
    return true;
}

///////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(ElementHelp, StructEditor)

bool ElementHelp::doExecute(StructEditor* se, EventData*)
{
    const GroveLib::Element* elem = traverse_to_element(se->editViewSrcPos());
    if (0 == elem)
        return false;
    helpAssistant().showLongHelp(
        se->helpHandle()->elemHelp(elem->nodeName(), elem).pointer(),
        NOTR("&lt;") + elem->nodeName() + NOTR("&gt;"));
    return true;
}
