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
#include "structeditor/SE_Pos.h"
#include "structeditor/SetCursorEventData.h"
#include "structeditor/impl/SelectionHistory.h"
#include "structeditor/impl/debug_se.h"
#include "structeditor/impl/CommandLink.h"

#include "docutils/doctags.h"
#include "docview/SernaDoc.h"
#include "common/PropertyTreeEventData.h"

#include "common/CommandEvent.h"
#include "common/CommandExecutor.h"

#include "grove/Nodes.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityReferenceTable.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveCommands.h"
#include "groveeditor/GroveEditor.h"

#include "editableview/EditableView.h"

#include "genui/StructDocumentActions.hpp"

#include <qregexp.h>
#include <qapplication.h>

using namespace Common;
using namespace GroveLib;
using namespace Formatter;
using namespace GroveEditor;

//////////////////////////////////////////////////////////////////////

static inline ChainPos selection_pos(StructEditor* se,
                                     const TreeSelection& selection)
{
    return (!selection.isEmpty())
        ? selection.lastCommon().toAreaPos(se->editableView().rootArea())
        : se->editableView().context().areaPos();
}

static inline const Chain* normalize_chain(const Chain* chain)
{
    while (chain && chain->firstChunk() &&
           ((LINE_AREA == chain->firstChunk()->type() ||
             TEXT_AREA == chain->firstChunk()->type()) ||
            chain->hasParentOrigin()))
        chain = chain->parentChain();
    return chain;
}

SIMPLE_COMMAND_EVENT_IMPL(SelectParent, StructEditor)

bool SelectParent::doExecute(StructEditor* se, EventData*)
{
    Selection selection;
    ChainSelection chain_raw;
    const EditContext& context = se->editableView().context();

    if (se->selectionHistory().isRedoPossible()) {
        selection = se->selectionHistory().redo();
        chain_raw = ChainSelection(selection.tree_.start().toAreaPos(
                                       se->editableView().rootArea()),
                                   selection.tree_.end().toAreaPos(
                                       se->editableView().rootArea()));
    }
    else {
        selection = se->editableView().getSelection(true);
        GrovePos fo_hint = (context.areaPos().isNull())
            ? GrovePos() : EditContext::getFoPos(context.areaPos());
        const Node* common_aos = (selection.src_.isEmpty())
            ? context.srcPos().node()
            : selection.src_.start().node()->commonAos(
                selection.src_.end().node());
        while (common_aos && Node::ELEMENT_NODE != common_aos->nodeType())
            common_aos = common_aos->parent();
        if (!common_aos)
            return false;
        selection.src_ = GroveSelection(
            GrovePos(common_aos->parent(), common_aos),
            GrovePos(common_aos->parent(), common_aos->nextSibling()));

        chain_raw = get_chain_selection(selection.src_, fo_hint, se);
        selection.tree_ = TreeSelection(chain_raw.start(), chain_raw.end());
    }
    if (!selection.src_.isEmpty() &&
        selection.src_ != se->editableView().getSelection(true).src_) {
        if (!se->selectionHistory().isRedoPossible()) {
            if (!se->selectionHistory().isUndoPossible())
                se->selectionHistory().add(
                    Selection(TreeSelection(context.areaPos()),
                              GroveSelection(context.srcPos())));
            se->selectionHistory().add(selection);
        }
        se->setSelection(chain_raw, selection.src_, true, true, false);
    }
    return true;
}

//////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(DeselectParent, StructEditor)

bool DeselectParent::doExecute(StructEditor* se, EventData*)
{
    if (!se->selectionHistory().isUndoPossible())
        return false;
    Selection selection = se->selectionHistory().undo();
    ChainSelection chain_raw = ChainSelection(
        selection.tree_.start().toAreaPos(se->editableView().rootArea()),
        selection.tree_.end().toAreaPos(se->editableView().rootArea()));

    se->setSelection(chain_raw, selection.src_, true, true, false);

    return true;
}

//////////////////////////////////////////////////////////////////////

GrovePos adjust_to_text_pos(const GrovePos& srcPos)
{
    if (GrovePos::ELEMENT_POS != srcPos.type())
        return srcPos;
    if (srcPos.before() && Node::TEXT_NODE == srcPos.before()->nodeType()){
        Text* text = static_cast<Text*>(srcPos.before());
        return GrovePos(text, (int)(0));
    }
    const Node* after = (srcPos.before())
        ? srcPos.before()->prevSibling() : srcPos.node()->lastChild();

    if (after && Node::TEXT_NODE == after->nodeType()) {
        const Text* text = static_cast<const Text*>(after);
        return GrovePos(text, (int)(text->data().length()));
    }
    return srcPos;
}

SIMPLE_COMMAND_EVENT_IMPL(SelectContent, StructEditor)

bool SelectContent::doExecute(StructEditor* se, EventData*)
{
    const EditContext& context = se->editableView().context();
    GroveSelection selection = se->editableView().getSelection(true).src_;
    GrovePos fo_hint = (context.areaPos().isNull())
        ? GrovePos() : EditContext::getFoPos(context.areaPos());
    const Node* common_aos = (selection.isEmpty())
        ? context.srcPos().node()
        : selection.start().node()->commonAos(selection.end().node());
    while (common_aos && Node::ELEMENT_NODE != common_aos->nodeType())
        common_aos = common_aos->parent();
    if (!common_aos)
        return false;
    selection = GroveSelection(
        adjust_to_text_pos(GrovePos(common_aos, common_aos->firstChild())),
        adjust_to_text_pos(GrovePos(common_aos, (Node*)(0))));
    selection.dump();
    ChainSelection chain_raw = get_chain_selection(selection, fo_hint, se);
    se->setSelection(chain_raw, selection, true, true, true);
    return true;
}

///////////////////////////////////////////////////////////

static String convert_to_string(const GrovePos& from, const GrovePos& to,
                                StructEditor* se, FragmentPtr& fragment)
{
    fragment = new DocumentFragment;
    se->groveEditor()->copy(from, to, fragment);

    int flags = Grove::GS_SAVE_CONTENT;
    const StripInfo* strip_info = se->stripInfo();
    if (strip_info)
        flags |= GroveLib::Grove::GS_INDENT;
    String str;
    fragment->saveAsXmlString(str, flags, strip_info);
    return str;
}

////////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(CommentSelection, StructEditor)

static bool has_comment(const Node* n)
{
    if (n->nodeType() == Node::COMMENT_NODE)
        return true;
    for (n = n->firstChild(); n; n = n->nextSibling())
        if (has_comment(n))
            return true;
    return false;
}

bool CommentSelection::doExecute(StructEditor* se, EventData*)
{
    GrovePos from, to;
    if (StructEditor::POS_OK != se->getSelection(from, to,
        StructEditor::ANY_OP|StructEditor::NOROOT_OP))
            return false;
    GrovePos pos;
    if (!se->getCheckedPos(pos))
        return false;
    pos = pos.adjustChoice();
    RefCntPtr<GroveBatchCommand> batch_cmd = new GroveBatchCommand;
    batch_cmd->setInfo(QT_TRANSLATE_NOOP("GroveEditor",
        "Comment Out Selection"));
    batch_cmd->setUndoPos(pos);
    try {
        FragmentPtr frag;
        String comment(convert_to_string(from, to, se, frag));
        if (has_comment(frag.pointer())) {
            se->sernaDoc()->showMessageBox(SernaDoc::MB_CRITICAL,
                tr("Cannot comment out selected content"),
                tr("Cannot comment out content if it already has "
                   "comments inside"),
                tr("&Ok"));
            return false;
        }
        batch_cmd->executeAndAdd(
            se->groveEditor()->cut(from, to).pointer(), true);
        pos = cmd_pos(batch_cmd->lastChild());
        batch_cmd->executeAndAdd(
            se->groveEditor()->insertComment(pos, comment).pointer(), true);
    }
    catch (...) {
        se->showCmdExecutionError();
        return false;
    }
    return se->executeAndUpdate(batch_cmd.pointer());
}

/////////////////////////////////////////////////////////////////////

static const String ROOT_NAME = NOTR("___FRAGMENT__ROOT___");

static FragmentPtr parse_string(const String& str, Grove* grove,
                                const StripInfo* si, MessageList& msgList,
                                GrovePtr& content_grove)
{
    RefCntPtr<MessageListMessenger> mlm_ptr =
        new MessageListMessenger(msgList);

    OwnerPtr<GroveBuilder> builder (grove->groveBuilder()->copy());
    builder->setEncoding(Encodings::UTF_8);
    builder->setMessenger(mlm_ptr.pointer());
    builder->setRootGrove(grove);

    String content = "<" + ROOT_NAME  + ">" + str + "</" +ROOT_NAME  + ">";
    //std::cerr << content << std::endl;

    content_grove = builder.release()->buildGrove(content, grove->topSysid());

    if (!content_grove.isNull() && 0 == msgList.size()) {
        Node* top_element = content_grove->document()->documentElement();
        if (top_element && top_element->firstChild()) {
            //Node::dumpSubtree(top_element);
            if (si)
                si->strip(top_element);

            Node* first_child = top_element->firstChild();
            Node* last_child = top_element->lastChild();

            FragmentPtr fragment(first_child->takeAsFragment(last_child));
            fragment->setGrove(top_element->grove());

            return fragment;
        }
    }
    return 0;
}

static FragmentPtr convert_to_fragment(const String& text,
                                       const String& caption,
                                       StructEditor* se,
                                       GrovePtr& fragmentGrove)
{
    if (text.isEmpty())
        return new DocumentFragment;
    MessageList msg_list;
    FragmentPtr fragment = parse_string(
        text, se->grove(), se->stripInfo(), msg_list, fragmentGrove);
    if (fragment.isNull()) {
        String error = qApp->translate("StructEditor",
                                       "Error parsing content. ");
        if (0 != msg_list.size()) {
            for (uint i = 0; i < msg_list.size(); ++i) {
                error += msg_list[i]->format(
                    BuiltinMessageFetcher::instance());
                if (i + 1 < msg_list.size())
                    error += '\n';
            }
        }
        se->sernaDoc()->showMessageBox(
            SernaDoc::MB_CRITICAL, caption, error, tr("&Close"));
    }
    return fragment;
}

/////////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(Uncomment, StructEditor)

bool Uncomment::doExecute(StructEditor* se, EventData*)
{
    GrovePos pos;
    if (!se->getCheckedPos(pos))
        return false;
    if (pos.node()->nodeType() != Node::COMMENT_NODE)
        return false;

    //! Parse comment data
    GrovePtr fragment_grove;
    FragmentPtr fragment = convert_to_fragment(
        static_cast<Comment*>(pos.node())->comment(),
        tr("Uncommenting error"), se, fragment_grove);
    if (fragment.isNull())
        return false;

    RefCntPtr<GroveBatchCommand> batch_cmd = new GroveBatchCommand;
    batch_cmd->setInfo(QT_TRANSLATE_NOOP("GroveEditor", "Uncomment"));
    batch_cmd->setUndoPos(pos);

    try {
        batch_cmd->executeAndAdd(
            se->groveEditor()->removeNode(pos.node()).pointer(), true);
        if (fragment->firstChild()) {
            pos = cmd_pos(batch_cmd->lastChild());
            batch_cmd->executeAndAdd(
                se->groveEditor()->paste(fragment, pos).pointer(), true);
        }
    }
    catch (...) {
        se->showCmdExecutionError();
        return false;
    }
    return se->executeAndUpdate(batch_cmd.pointer());
}

/////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(EditSelectionAsText, StructEditor)

class ShowEditAsTextDialog;

bool EditSelectionAsText::doExecute(StructEditor* se, EventData*)
{
    //! Check selection and cursor position
    GrovePos from, to;
    const StructEditor::PositionStatus pstatus =
        se->getSelection(from, to, StructEditor::ANY_OP |
                         StructEditor::ALLOW_RDONLY|StructEditor::NOROOT_OP);
    if (pstatus == StructEditor::POS_FAIL)
        return false;
    GrovePos pos(from);
    if (Node::CHOICE_NODE == pos.node()->nodeType())
        pos = GrovePos(pos.node()->parent(), pos.node());
    //! Edit xml content in dialog
    FragmentPtr frag;
    String content(convert_to_string(from, to, se, frag));
    PropertyTreeEventData ed;
    ed.root()->makeDescendant("caption", tr("Edit Selection As Text"));
    ed.root()->makeDescendant("data", content);
    if (StructEditor::POS_RDONLY == pstatus)
        ed.root()->makeDescendant("is-readonly");
    GrovePtr fragment_grove;
    FragmentPtr fragment;
    RefCntPtr<GroveBatchCommand> batch_cmd;
    do {
        batch_cmd = new GroveBatchCommand;
        for (;;) {
            if (!makeCommand<ShowEditAsTextDialog>(&ed)->execute(se, &ed))
                return false;
            if (StructEditor::POS_RDONLY == pstatus)
                return true;
            const String& edited_content =
                ed.root()->getSafeProperty("data")->getString();
            if (content == edited_content) {
                se->removeSelection();
                return false;
            }
            //! Parse edited content
            fragment = convert_to_fragment(
                edited_content, tr("Edit selection error"),
                se, fragment_grove);
            if (!fragment.isNull())
                break;
        }
        //! Replace selcted content with edited one
        batch_cmd->setInfo(QT_TRANSLATE_NOOP("GroveEditor",
            "Edit Selection As Text"));
        batch_cmd->setUndoPos(pos);
        try {
            batch_cmd->executeAndAdd(
                se->groveEditor()->cut(from, to).pointer(), true);
            if (fragment->firstChild()) {
                pos = cmd_pos(batch_cmd->lastChild());
                batch_cmd->executeAndAdd(
                    se->groveEditor()->paste(fragment, pos).pointer(), true);
            }
        }
        catch (...) {
            se->showCmdExecutionError();
            continue;
        }
    } while (!(se->executeAndUpdate(batch_cmd.pointer()) &&
             se->executor()->lastDone() &&
             static_cast<const CommandLink*>(se->executor()->lastDone())->
                getLink() == batch_cmd.pointer()));
    return true;
}
