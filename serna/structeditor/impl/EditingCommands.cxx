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
#include "structeditor/InsertTextEventData.h"
#include "structeditor/SetCursorEventData.h"
#include "structeditor/impl/debug_se.h"
#include "structeditor/impl/XsUtils.h"
#include "xs/XsElement.h"
#include "xs/XsNodeExt.h"

#include "docview/Clipboard.h"
#include "docview/Finder.h"
#include "docview/SernaDoc.h"

#include "common/PropertyTreeEventData.h"

#include "common/CommandEvent.h"
#include "common/CommandExecutor.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"

#include "editableview/EditableView.h"

#include "genui/StructDocumentActions.hpp"

#include "formatter/impl/TerminalFos.h"
#include "formatter/impl/Areas.h"
#include <qapplication.h>

USING_COMMON_NS;
using namespace GroveEditor;
using namespace GroveLib;

class InsertText : public CommandEvent<StructEditor> {
public:
    InsertText(const InsertTextEventData& ed)
        : ed_(ed) {}

    virtual bool doExecute(StructEditor*, EventData*);

private:
    InsertTextEventData ed_;
};

COMMON_NS_BEGIN
template<> STRUCTEDITOR_EXPIMP
CommandEventPtr makeCommand<InsertText>(const EventData* d)
{
    return new InsertText(*static_cast<const InsertTextEventData*>(d));
}
COMMON_NS_END

static Node* prev_child(const GrovePos& pos)
{
    if (pos.before())
        return pos.before()->prevSibling();
    return pos.node()->lastChild();
}

static GrovePos adjust_text_pos(const GrovePos& pos)
{
    if (is_cdata_allowed(pos.node()))
        return pos;
    GrovePos text_pos = pos;
    while (!text_pos.isNull() && !is_cdata_allowed(text_pos.node()))
        text_pos = GrovePos(prev_child(text_pos), (Node*)(0));
    if (!text_pos.isNull()) {
        Node* last_child = text_pos.node()->lastChild();
        if (last_child && Node::TEXT_NODE == last_child->nodeType())
            return GrovePos(last_child, 
                            static_cast<Text*>(last_child)->data().length());
        return text_pos;
    }
    text_pos = pos;
    while (!text_pos.isNull() && !is_cdata_allowed(text_pos.node())) {
        Node* node = text_pos.before();
        text_pos = (node) ? GrovePos(node, node->firstChild()) : GrovePos();
    }
    if (!text_pos.isNull()) {
        Node* first_child = text_pos.node()->firstChild();
        if (first_child && Node::TEXT_NODE == first_child->nodeType())
            return GrovePos(first_child, (long int)(0));
    }
    return GrovePos();
}

//////////////////////////////////////////////////////////////////////

bool InsertText::doExecute(StructEditor* se, EventData*)
{
    DBG(SE.EVENT) << "InsertTextEvent (continue_text=" << ed_.continueText_
        << "): " << ed_.data_ << std::endl;
    Editor& ge = *(se->groveEditor());
    GrovePos src_pos;
    if (!se->getCheckedPos(src_pos, StructEditor::TEXT_OP) ||
        src_pos.node()->nodeType() == GroveLib::Node::CHOICE_NODE)
            return false;
    if (se->isValidationOn() > 1) {
        GrovePos text_allowed_pos = adjust_text_pos(src_pos);
        if (!text_allowed_pos.isNull())
            src_pos = text_allowed_pos;
    }
    //! If there is no selection, simply insert text
    if (se->editableView().getSelection().src_.isEmpty()) {
        if (!se->executeAndUpdate(ge.insertText(src_pos, ed_.data_)))
            return false;
        if (ed_.continueText_) {
            se->executor()->mergeLastCommand();
            se->notifyUndoStateChange();
        }
        return true;
    }
    GrovePos from, to;
    if (StructEditor::POS_OK != se->getSelection(
            from, to, StructEditor::TEXT_OP))
        return false;
    //! If selection is within single text node, replace text
    if (from.type() == GrovePos::TEXT_POS && from.node() == to.node()) 
        return se->executeAndUpdate(
            ge.replaceText(from, to.idx() - from.idx(), ed_.data_));

    //! If selection is mixed content, then do cut then insert text
    RefCntPtr<GroveBatchCommand> batch_cmd = new GroveBatchCommand;

    batch_cmd->setInfo(QT_TRANSLATE_NOOP("GroveEditor", "Cut/Insert")); 
    CommandPtr cut_cmd = ge.cut(from, to);
    if (!cut_cmd) {
        se->removeSelection();
        return se->executeAndUpdate(ge.insertText(src_pos, ed_.data_));
    }
    batch_cmd->executeAndAdd(cut_cmd.pointer());
    se->removeSelection();
    src_pos = batch_cmd->firstChild()->asCommandContext()->pos();
    batch_cmd->executeAndAdd(ge.insertText(src_pos, ed_.data_).pointer());
    return se->executeAndUpdate(batch_cmd.pointer());
}

///////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(DoBackspace, StructEditor)

bool DoBackspace::doExecute(StructEditor* se, EventData*)
{
    GrovePos src_pos = se->editViewSrcPos();
    if (src_pos.isNull())
        return false;
    int opClass;
    
    if (GrovePos::TEXT_POS == src_pos.type()) {
        opClass = StructEditor::TEXT_OP;
        if (0 == src_pos.idx()) {
            src_pos = GrovePos(src_pos.node()->parent(), src_pos.node());
            opClass = StructEditor::ANY_OP;
        }
    } 
    else
        opClass = StructEditor::ANY_OP;
    
    DBG(SE.EVENT) << "Backspace at:" << std::endl;
    DBG_EXEC(SE.EVENT, src_pos.dump());

    GrovePos from, to;
    if (!se->editableView().getSelection().src_.isEmpty()) {
        if (StructEditor::POS_OK == se->getSelection(from, to, opClass))
            return se->executeAndUpdate(se->groveEditor()->cut(from, to));
        return false;
    }
    if (opClass == StructEditor::TEXT_OP) {
        if (se->isEditableEntity(src_pos, opClass) != StructEditor::POS_OK)
            return false;
        Formatter::ChainPos chain_pos =
            se->editableView().context().areaPos();
        GrovePos prev_pos(EditContext::getSrcPos(Formatter::ChainPos(
            chain_pos.chain(), chain_pos.pos() - 1).toAreaPos()));
        RT_ASSERT(prev_pos.node() == src_pos.node());
        uint count = src_pos.idx() - prev_pos.idx();
        return se->executeAndUpdate(se->groveEditor()->removeText(
            GrovePos(src_pos.node(), src_pos.idx() - count), count));
    }
    else {
        Node* b = src_pos.before();
        if (!b && se->isEditableEntity(src_pos, opClass) !=
            StructEditor::POS_OK)
                return false;
        Node* ps =  b ? b->prevSibling() : 0;
        if (!ps || ps->nodeType() != Node::ENTITY_REF_END_NODE) {
            if (se->isEditableEntity(GrovePos(src_pos.node(), ps), 
                opClass) != StructEditor::POS_OK)
                    return false;
        }
    }
    return se->executeAndUpdate(se->groveEditor()->backspace(src_pos));
}

///////////////////////////////////////////////////////////

static bool ct_delete(StructEditor* se, 
                      const GrovePos& src_pos,
                      int opClass)
{
    DBG(SE.EVENT) << "Delete at:" << std::endl;
    DBG_EXEC(SE.EVENT, src_pos.dump());

    if (se->isEditableEntity(src_pos, opClass) != StructEditor::POS_OK)
        return false;
    if (opClass == StructEditor::TEXT_OP) {
        Text* text = static_cast<Text*>(src_pos.node());
        if ((int)text->data().length() > src_pos.idx()) {
            Formatter::ChainPos chain_pos =
                se->editableView().context().areaPos();
            GrovePos next_pos(EditContext::getSrcPos(
                Formatter::ChainPos(chain_pos.chain(),
                    chain_pos.pos() + 1).toAreaPos()));
            RT_ASSERT(next_pos.node() == src_pos.node());
            uint count = next_pos.idx() - src_pos.idx();
            return se->executeAndUpdate
                (se->groveEditor()->removeText(src_pos, count));
        }
    }
    return se->executeAndUpdate(se->groveEditor()->del(src_pos));
}

SIMPLE_COMMAND_EVENT_IMPL(DoDelete, StructEditor)

bool DoDelete::doExecute(StructEditor* se, EventData*)
{
    GrovePos src_pos = se->editViewSrcPos();
    if (src_pos.isNull())
        return false;

    int opClass;
    if (GrovePos::TEXT_POS == src_pos.type()) 
        opClass = StructEditor::TEXT_OP;
    else
        opClass = StructEditor::ANY_OP;

    DBG(SE.EVENT) << "DoDelete at:" << std::endl;
    DBG_EXEC(SE.EVENT, src_pos.dump());

    GrovePos from, to;
    if (se->editableView().getSelection().src_.isEmpty())
        return ct_delete(se, src_pos, opClass);
    if (StructEditor::POS_OK != se->getSelection(from, to, opClass))
        return false;
    return se->executeAndUpdate(se->groveEditor()->cut(from, to));
}

SIMPLE_COMMAND_EVENT_IMPL(RemoveElement, StructEditor)

bool RemoveElement::doExecute(StructEditor* se, EventData*)
{
    GrovePos from, to;
    if (StructEditor::POS_OK == se->getSelection(
            from, to, StructEditor::STRUCT_OP))
        return se->executeAndUpdate(se->groveEditor()->cut(from, to));
    if (!se->getCheckedPos(from, 
            StructEditor::STRUCT_OP|StructEditor::PARENT_OP))
        return false;
    Node* node = (from.type() == GrovePos::TEXT_POS) ?
        from.node()->parent() : from.node();
    if (from.node()->grove()->document()->documentElement() == node)
        return false;
    return ct_delete(se, GrovePos(node->parent(), node), 
        StructEditor::STRUCT_OP);
}

///////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(StructUndo, StructEditor)

bool StructUndo::doExecute(StructEditor* se, EventData*)
{
    se->doUntil(-1);
    return true;
}

///////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(StructRedo, StructEditor)

bool StructRedo::doExecute(StructEditor* se, EventData*)
{
    se->doUntil(1);
    return true;
}

///////////////////////////////////////////////////////////

static bool struct_copy(StructEditor* se, bool isMouse)
{
    try {
        GrovePos from, to;
        if (StructEditor::POS_FAIL == se->getSelection(from, to, 
            StructEditor::SILENT_OP | StructEditor::ALLOW_RDONLY))
                return false;
        GroveEditor::Editor::FragmentPtr fragment = new DocumentFragment;
        if (!se->groveEditor()->copy(from, to, fragment)) {
            if (!isMouse && !se->groveEditor()->errorMessage().isEmpty())
                se->sernaDoc()->showMessageBox(
                    SernaDoc::MB_CRITICAL, 
                    qApp->translate("StructEditor", 
                                    "Command Execution Failed"),
                    se->groveEditor()->errorMessage(), tr("&Ok"));
            return false;
        }
        if (!isMouse)
            se->removeSelection();
        serna_clipboard().setStructClipboard(isMouse, fragment.pointer());
        return true;
    }
    catch (Common::Exception& e) {
        se->sernaDoc()->showMessageBox(
            SernaDoc::MB_CRITICAL, 
            qApp->translate("StructEditor", "Clipboard Error"),
            qApp->translate("StructEditor", 
                            "<qt><p>Cannot copy the selection</p><p>Reason:"
                            "</p><pre>%1</pre>").arg(QString(e.what())), 
            tr("&OK"));
        return false;
    }
}

SIMPLE_COMMAND_EVENT_IMPL(StructCopy, StructEditor)

bool StructCopy::doExecute(StructEditor* se, EventData*)
{
    return struct_copy(se, false);
}

SIMPLE_COMMAND_EVENT_IMPL(StructMouseCopy, StructEditor)

bool StructMouseCopy::doExecute(StructEditor* se, EventData*)
{
    return struct_copy(se, true);
}


static bool selection_paste(StructEditor* se,
                            const GrovePos& from,
                            const GrovePos& to,
                            DocumentFragment* df)
{
    RefCntPtr<GroveBatchCommand> batch_cmd = new GroveBatchCommand;
    batch_cmd->setInfo(QT_TRANSLATE_NOOP("GroveEditor", "Cut and Paste"));
    CommandPtr cut_cmd = se->groveEditor()->cut(from, to);
    if (0 == cut_cmd) 
        return false; 
    batch_cmd->executeAndAdd(cut_cmd.pointer());
    se->removeSelection();
    GrovePos src_pos = batch_cmd->firstChild()->asCommandContext()->pos();
    batch_cmd->executeAndAdd(
        se->groveEditor()->paste(df, src_pos, 0).pointer());
    return se->executeAndUpdate(batch_cmd.pointer());
}

static bool struct_paste(StructEditor* se, bool isMouse)
{   
    DocumentFragment* df = const_cast<DocumentFragment*>
         (serna_clipboard().getStructClipboard(isMouse));
    if (!df->firstChild())
        return false;
    int opClass = StructEditor::STRUCT_OP;
    if (df->firstChild() == df->lastChild() &&
        df->firstChild()->nodeType() == GroveLib::Node::TEXT_NODE) {
            opClass = StructEditor::TEXT_OP;
            if (CONST_TEXT_CAST(df->firstChild())->data().isEmpty())
                return false;
    }
    try {
        GrovePos from, to;
        if (!se->editableView().getSelection().src_.isEmpty()) {
            if (StructEditor::POS_OK == se->getSelection(from, to, opClass)) 
                return selection_paste(se, from, to, df);
            return false;
        }
        if (!se->getCheckedPos(from, opClass))
            return false;
        return se->executeAndUpdate(se->groveEditor()->paste(df, from, 0));
    }
    catch (Common::Exception& e) {
        se->sernaDoc()->showMessageBox(
            SernaDoc::MB_WARNING, 
            qApp->translate("StructEditor", "Clipboard Error"),
            qApp->translate("StructEditor", 
                            "<qt><p>Cannot paste clipboard content</p><p>"
                            "Reason:</p><pre>%1</pre>").arg(QString(e.what())),
            tr("&OK"));
        return false;
    }
}

SIMPLE_COMMAND_EVENT_IMPL(StructPaste, StructEditor)

bool StructPaste::doExecute(StructEditor* se, EventData*)
{
    return struct_paste(se, false);
}

SIMPLE_COMMAND_EVENT_IMPL(StructMousePaste, StructEditor)

bool StructMousePaste::doExecute(StructEditor* se, EventData*)
{
    return struct_paste(se, true);
}

SIMPLE_COMMAND_EVENT_IMPL(StructCopyAsText, StructEditor)

class SelectionTextCollectorOp : public EditableView::SelectionOp {
public:
    String text;
    Formatter::CPoint curPos;
    Formatter::CRange curRange;
    virtual void operator()(const Formatter::Area* area, long from, long to)
    {
        if (area && Formatter::TEXT_AREA == area->type()) {
            int font_height = area->cursorHeight();
            Formatter::CPoint pos = area->absAllcPoint();
            int delta_y = int(pos.y_ - curPos.y_);
            int delta_x = int(pos.x_ - (curPos.x_ + curRange.w_));
            if (0 <= delta_y && font_height > delta_y) {
                for (int i = 0; i < delta_x / font_height; i++)
                    text += " ";
            }
            else if (0 < delta_y) {
                for (int i = 0; i < delta_y / font_height; i++)
                    text += "\n";
                for (int i = 0; i < pos.x_ / font_height; i++)
                    text += " ";
            }
            if (0 > from)
                from = 0;
            text += static_cast<const Formatter::TextArea*>(area)->
                          text().mid(from, to - from);
            curPos = pos;
            curRange = Formatter::CRange(area->allcW(), area->allcH());
        }
    }
};

bool StructCopyAsText::doExecute(StructEditor* se, EventData*)
{
    SelectionTextCollectorOp collect_text;
    se->editableView().inSelectionDo(
        se->editableView().getSelection().tree_, collect_text);
    se->removeSelection();
    if (collect_text.text.isEmpty())
        return true;
    collect_text.text = collect_text.text.stripWhiteSpace();
    serna_clipboard().setText(false, collect_text.text);
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(StructPasteAsText, StructEditor)

bool StructPasteAsText::doExecute(StructEditor* se, EventData*)
{
    GrovePos pos, from, to;
    if (!se->getCheckedPos(pos, StructEditor::TEXT_OP))
        return false;
    RefCntPtr<DocumentFragment> new_df = new DocumentFragment;
    new_df->appendChild(new GroveLib::Text(serna_clipboard().
        getText(false, se->stripInfo())));
    if (se->editableView().getSelection().src_.isEmpty())
        return se->executeAndUpdate(se->groveEditor()->paste
            (new_df.pointer(), pos, 0));
    if (StructEditor::POS_OK != se->getSelection(
            from, to, StructEditor::TEXT_OP))
        return false;
    return selection_paste(se, from, to, new_df.pointer());
}

///////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(StructCut, StructEditor)

bool StructCut::doExecute(StructEditor* se, EventData*)
{
    GrovePos from, to;
    // treat as text-op because there wouldn't be any elements in text
    // content anyway
    if (StructEditor::POS_OK != se->getSelection(
            from, to, StructEditor::TEXT_OP))
        return false;
    se->removeSelection();
    GroveEditor::Editor::FragmentPtr fragment = new DocumentFragment;
    se->groveEditor()->copy(from, to, fragment);
    serna_clipboard().setStructClipboard(false, fragment.pointer());
    return se->executeAndUpdate(se->groveEditor()->cut(from, to));
}

///////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(StructClipboardChanged, StructEditor)

bool StructClipboardChanged::doExecute(StructEditor* se, EventData*)
{
    se->uiActions().paste()->setEnabled
        (serna_clipboard().getStructClipboard(false)->firstChild());
    se->uiActions().pasteAsText()->setEnabled
        (serna_clipboard().getStructClipboard(false)->firstChild());
    return true;
}
