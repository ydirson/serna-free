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
#include "common/Url.h"
#include "common/PathName.h"

#include "structeditor/StructEditor.h"
#include "structeditor/impl/debug_se.h"

#include "utils/DocSrcInfo.h"
#include "common/PropertyTreeEventData.h"
#include "utils/Config.h"
#include "common/PropertyTreeEventFactory.h"
#include "genui/StructDocumentActions.hpp"

#include "docview/SernaDoc.h"
#include "editableview/EditableView.h"

USING_COMMON_NS;

PROPTREE_EVENT_IMPL(OpenStructDocument,   StructEditor)
PROPTREE_EVENT_IMPL(NewStructDocument,    StructEditor)
PROPTREE_EVENT_IMPL(DoSaveStructDocWithDsi, StructEditor)

class CannotSaveWarningDialog;

// For these, dsi_ is provided by OpenDocument/NewDocument implementation

bool OpenStructDocument::doExecute(StructEditor* se, EventData*)
{
    return se->openDocument(ed_);
}

bool NewStructDocument::doExecute(StructEditor* se, EventData*)
{
    return se->newDocument(ed_);
}


bool DoSaveStructDocWithDsi::doExecute(StructEditor* se, EventData*)
{
    return se->saveDocument(ed_);
}

SIMPLE_COMMAND_EVENT_IMPL(PrintStructDocument,   StructEditor)

bool PrintStructDocument::doExecute(StructEditor* se, EventData*)
{
    bool is_paginated = 
        se->uiActions().togglePageMode()->getBool(Sui::IS_TOGGLED);
    if (!is_paginated) {
        int result = se->sernaDoc()->showMessageBox(
            SernaDoc::MB_WARNING,
            tr("Document Cannot Be Printed In Normal View"),
            tr("<qt><nobr>Document cannot be printed in <i>Normal View</i>. "
               "</nobr><br/><nobr>Would you like to switch to <i>Paginated "
               "View</i> and print the document?</nobr></qt>"), 
            tr("&Switch && Print"), tr("&Cancel"));
        if (0 != result)
            return true;
        se->uiActions().togglePageMode()->setToggled(true);
        se->uiActions().togglePageMode()->dispatch();
    }
    
    se->editableView().print();
    return true;
}

/////////////////////////////////////////////////////////////////////

class PublishDialog;

SIMPLE_COMMAND_EVENT_IMPL(PublishDocument,   StructEditor)

bool PublishDocument::doExecute(StructEditor* se, EventData*)
{
    PropertyTreeEventData pinfo(se->getDsi()->makeDescendant("publish-info"));
    makeCommand<PublishDialog>(&pinfo)->execute(se, &pinfo);
    return true;
}

/////////////////////////////////////////////////////////////////////

// THESE ARE KEPT FOR COMPATIBILITY WITH OLDER VERSIONS - TO BE REMOVED
// IN SERNA 4 (apg)

PROPTREE_EVENT_IMPL(SaveAsStructDocWithDsi, StructEditor)
REGISTER_COMMAND_EVENT_MAKER(SaveAsStructDocWithDsi, "PropertyTree", "-")

bool SaveAsStructDocWithDsi::doExecute(StructEditor* se, EventData*)
{
    return se->sernaDoc()->saveDocument(ed_);
}

SIMPLE_COMMAND_EVENT_IMPL(SaveAsStructDocument, StructEditor)

class SaveDocument;
class SaveDocumentAs;

bool SaveAsStructDocument::doExecute(StructEditor* se, EventData*)
{
    return makeCommand<SaveDocumentAs>()->execute(se->sernaDoc());
}

SIMPLE_COMMAND_EVENT_IMPL(SaveStructDocument,   StructEditor)

bool SaveStructDocument::doExecute(StructEditor* se, EventData*)
{
    return makeCommand<SaveDocument>()->execute(se->sernaDoc());
}

/////////////////////////////////////////////////////////////////////
