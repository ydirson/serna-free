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
#include "plaineditor/PlainDocument.h"
#include "plaineditor/PlainEditor.h"
#include "plaineditor/impl/debug_pe.h"

#include "docview/Clipboard.h"
#include "docview/Finder.h"
#include "docview/GoToOriginEventData.h"
#include "docview/EventTranslator.h"
#include "common/PropertyTreeEventData.h"
#include "utils/DocSrcInfo.h"
#include "grove/Nodes.h"

#include <QFileDialog>

#include "genui/PlainDocumentActions.hpp"
using namespace Common;

/////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(PlainClipboardChanged, PlainEditor)

bool PlainClipboardChanged::doExecute(PlainEditor* pe, EventData*)
{
    String text = serna_clipboard().getText(false, 0);  // TODO: corr. wstrip
    pe->uiActions().paste()->setEnabled(!text.isEmpty());
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(PlainCut, PlainEditor)

bool PlainCut::doExecute(PlainEditor* pe, EventData*)
{
    serna_clipboard().setText(false, pe->selectedText());
    serna_clipboard().appFocusEvent(false, 0);
    pe->cut();
    return true;
}
SIMPLE_COMMAND_EVENT_IMPL(PlainCopy, PlainEditor)

bool PlainCopy::doExecute(PlainEditor* pe, EventData*)
{
    serna_clipboard().setText(false, pe->selectedText());
    serna_clipboard().appFocusEvent(false, 0);
    pe->copy();
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(PlainPaste, PlainEditor)

bool PlainPaste::doExecute(PlainEditor* pe, EventData*)
{
    pe->paste(serna_clipboard().getText(false, 0));
    return true;
}

UICMD_EVENT_IMPL(SwitchToEntity, PlainEditor)

bool SwitchToEntity::doExecute(PlainEditor* pe, EventData*)
{
    if (activeSubAction())
        pe->switchToEntity(activeSubAction()->get(Sui::INSCRIPTION));
    return true;
}

/////////////////////////////////////////////////////////////////////

PlainEditor::PlainEditor(PlainDocument* doc, PropertyNode* dsi)
    : Sui::Item(Sui::PLAIN_EDITOR, 0),
      doc_(doc)
{
    DBG(PE.TEST) << "PlainEditor cons: " << this << ", plaindoc=" 
        << doc_ << std::endl;
    doc->setDsi(dsi);
    serna_clipboard().clipboardChange().subscribe(
        makeCommand<PlainClipboardChanged>, this);
    uiActions().find()->setEnabled(true);
    uiActions().findReplace()->setEnabled(true);
    uiActions().paste()->setEnabled(serna_clipboard().getStructClipboard(0));
}

PlainEditor::~PlainEditor()
{
    DBG(PE.TEST) << "~PlainEditor: " << this << std::endl;
}

PlainDocumentActions& PlainEditor::uiActions()
{
    return static_cast<PlainDocumentActions&>(plainDoc()->uiActions());
}

////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(PlainUndo, PlainEditor)

bool PlainUndo::doExecute(PlainEditor* pe, EventData*)
{
    pe->undo();
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(PlainRedo, PlainEditor)

bool PlainRedo::doExecute(PlainEditor* pe, EventData*)
{
    pe->redo();
    return true;
}

////////////////////////////////////////////////////////////////

GOTO_ORIGIN_EVENT_IMPL(PlainGoToMessageOrigin, PlainEditor)

bool PlainGoToMessageOrigin::doExecute(PlainEditor* pe, EventData*)
{
    const GroveLib::OriginBase* origin = origin_.docOrigin();
    if (!origin)
        return false;
    pe->openSingleFile(origin->fileName());
    pe->setCursor(origin->line() - 1, origin->column() - 1);
    return true;
}
//////////////////////////////////////////////////////////////

PROPTREE_EVENT_IMPL(DoOpenPlainDocument, PlainEditor)

bool DoOpenPlainDocument::doExecute(PlainEditor* pe, EventData*)
{
    String file = ed_->getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
    if (pe->openDocument(file)) {
        pe->plainDoc()->getDsi()->makeDescendant(DocSrcInfo::IS_TEXT_DOCUMENT);
        return true;
    } 
    return false;
}

PROPTREE_EVENT_IMPL(DoSavePlainDocWithDsi, PlainEditor)
REGISTER_COMMAND_EVENT_MAKER(DoSavePlainDocWithDsi, "PropertyTree", "-")

bool DoSavePlainDocWithDsi::doExecute(PlainEditor* pe, EventData*)
{
    String path;
    if (ed_)
        path = ed_->getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
    else
        path = pe->plainDoc()->getDsi()->
            getSafeProperty(DocSrcInfo::DOC_PATH)->getString();
    if (!pe->saveDocument(path))
        return false;
    pe->plainDoc()->getDsi()->makeDescendant(DocSrcInfo::DOC_PATH, path, true);
    pe->plainDoc()->updateTooltip();
    return true;
}

/////////////////////////////////////////////////////////////////////

/// BELOW ARE KEPT FOR COMPATIBILITY, AND TO BE REMOVED IN SERNA4

class SaveDocument;
class SaveDocumentAs;

SIMPLE_COMMAND_EVENT_IMPL(SavePlainDocument, PlainEditor)

bool SavePlainDocument::doExecute(PlainEditor* pe, EventData*)
{
    return makeCommand<SaveDocument>()->execute(pe->plainDoc());
}

SIMPLE_COMMAND_EVENT_IMPL(SaveAsPlainDocument, PlainEditor)

bool SaveAsPlainDocument::doExecute(PlainEditor* pe, EventData*)
{
    return makeCommand<SaveDocumentAs>()->execute(pe->plainDoc());
}

