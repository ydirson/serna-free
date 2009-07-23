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
#include "structeditor/se_defs.h"
#include "structeditor/StructEditor.h"
#include "structeditor/LiveNodeLocator.h"
#include "structeditor/GroveCommandEventData.h"
#include "structeditor/impl/debug_se.h"
#include "structeditor/impl/EditPolicyImpl.h"
#include "structeditor/impl/SelectionHistory.h"
#include "structeditor/impl/ViewParamImpl.h"
#include "structeditor/impl/CommandLink.h"
#include "structeditor/impl/SchemaCommandMaker.h"
#include "structeditor/impl/StructAutoSave.h"
#include "structeditor/impl/XsUtils.h"

#include "common/STQueue.h"
#include "common/CommonMessages.h"
#include "common/timing_profiler.h"

#include "utils/Config.h"
#include "utils/DocSrcInfo.h"
#include "utils/SernaMessages.h"
#include "utils/Properties.h"
#include "common/PropertyTreeEventData.h"

#include "grove/XmlNs.h"
#include "grove/Nodes.h"
#include "grove/Grove.h"
#include "grove/udata.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"
#include "xslt/Engine.h"
#include "xslt/Result.h"

#include "docview/SernaDoc.h"

#include "xs/Schema.h"
#include "xs/XsElement.h"
#include "xs/XsNodeExt.h"

#include "ui/UiAction.h"
#include "genui/StructDocumentActions.hpp"

#include <qapplication.h>

using namespace Common;
using namespace GroveEditor;
using namespace GroveLib;

class NodePos : public Common::STQueueItem<NodePos> {
public:
    USE_SUBALLOCATOR

    NodePos(const GroveLib::Node* node, const GroveLib::Node* before)
        : node_(node), before_(before) {}

    ConstNodePtr    node_;
    ConstNodePtr    before_;
};

class LiveNodeLocator::NodePosList : public STQueue<NodePos> {};

LiveNodeLocator::LiveNodeLocator(const GrovePos& pos)
    : ancestors_(new NodePosList)
{
    const GroveLib::Node* node = (!pos.isNull()) ? pos.node() : 0;
    const GroveLib::Node* before = (!pos.isNull())
        ? ((GrovePos::ELEMENT_POS == pos.type()) ? pos.before() : 0)
        : 0;
    DBG(SE.LNL) << "LiveNodeLocator: node=" 
        << node << "before=" << before << std::endl;
    while (node) {
        ancestors_->push_front(new NodePos(node, before));
        before = node->nextSibling();
        node = node->parent();
    }
    if (!ancestors_->empty())
        ancestors_->pop_front();
}

LiveNodeLocator::~LiveNodeLocator()
{
    ancestors_->destroyAll();
}

const GroveLib::Node* LiveNodeLocator::node() const
{
    NodePos* pos = 0;
    for (NodePos* i = ancestors_->first(); i; i = i->next()) {
        if (isAlive(i->node_))
            pos = i;
        else
            break;
    }
    if (0 == pos)
        return 0;
    const GroveLib::Node* alive = 0;
    if (isAlive(pos->before_)) {
        alive = pos->before_.pointer();
        while (alive->firstChild())
            alive = alive->firstChild();
        return alive;
    }
    else {
        alive = pos->node_.pointer();
        while (alive && alive->lastChild())
            alive = alive->lastChild();
    }
    DBG_IF(SE.LNL) {
        if (alive) {
            DBG(SE.LNL) << "LiveNodeLocator: node alive" << alive << std::endl;
            GroveLib::Node::dumpSubtree(alive->parent());
        }
    }
    return alive;
}

bool LiveNodeLocator::isAlive(const ConstNodePtr& node) const
{
    return (!node.isNull() && node->parent());
}

///////////////////////////////////////////////////////////////////////////

bool StructEditor::update(bool dontFormat)
{
    TPROF_CALL(u_xslt, xsltEngine_->update());
    bool is_modified = editableView_->isModified();
    if (!dontFormat)
        TPROF_CALL(u_ev, editableView_->update());
    sernaDoc()->showStageInfo();
    TPROF_CALL(u_vuf, viewUpdateFactory_->dispatchEvent());
    if (!dontFormat)
        editableView_->setNotModified();
    return is_modified;
}

// Check if validation context "context" is present in v_context
static bool check_vc(const GroveLib::Node* my_vc, 
                     const GroveLib::Node* v_context)
{
    if (0 == v_context)
        return false;
    for (const GroveLib::Node* n = v_context; n; n = parentNode(n))
        if (n == my_vc)
            return true;
    v_context = parentNode(v_context);
    return (!v_context || 
            v_context->root()->nodeType() != GroveLib::Node::DOCUMENT_NODE);
}

static void move_old_messages(CommandLink::MessageNodeList& messageList,
                              GroveLib::Node* context,
                              MessageTreeNode* msgTree)
{
    if (0 == context)
        return;
    MessageTreeNode* top_msg = msgTree->lastChild();
    MessageTreeNode* prev;
    while (top_msg) {
        MessageTreeNode* msg_node = top_msg->lastChild();
        while (msg_node) {
            if (!msg_node->getMessage() || 
                msg_node->getMessage()->facility() != 6) {
                msg_node = msg_node->prevSibling();
                continue;
            }
            const GroveLib::CompositeOrigin* origin = msg_node->findOrigin();
            if (!origin || !origin->docOrigin() || 
                !origin->docOrigin()->node() ||
                !check_vc(context, origin->docOrigin()->node())) {

                msg_node = msg_node->prevSibling();
                continue;
            }
            messageList.insert(msg_node);
            prev = msg_node->prevSibling();
            msg_node->remove();
            msg_node = prev;
        }
        prev = top_msg->prevSibling();
        if (!top_msg->firstChild())
            top_msg->remove();
        top_msg = prev;
    }
}

static void validate_command(StructEditor* se,
                             CommandLink* cmdLink,
                             CommandContext* cmdContext,
                             MacroCommand* batchCmd,
                             GroveReplicatedCommand* cmdReplica)
{
    TPROF_GUARD(VCmd);
    GroveLib::Node* v_context = cmdContext->validationContext();
    DBG(SE.VALIDATE) << "Validation context node: "
                     << (v_context ? v_context->nodeName() : String("<none>"))
                     << std::endl;
    if (0 == v_context)
        return;
    while (v_context && 0 == XsNodeExt::xsElement(v_context))
        v_context = v_context->parent();
    if (0 == v_context)
        return;

    MessageTreeNodePtr msg_node = new MessageTreeNode;
    se->schema()->setMessenger(msg_node.pointer());
    MessageTreeNode* const msg_tree = se->messageTree();

    // check if command requires removing extra attributes
    bool rm_attrs = cmdContext->flags() & CommandContext::CF_RMATT;
    int v_flags = se->schema()->validationFlags();
    if (rm_attrs)
        se->schema()->setValidationFlags(v_flags | Schema::rmExtraAttrs);

    DBG(SE.VALIDATE) 
        << "Partial validate: " << v_context->nodeName() << std::endl;
    SchemaCommandMakerPtr cmd_maker = SchemaCommandMaker::make(se->schema());
    se->schema()->setRequiredAttrsProvider(se->requiredAttrsProvider());
    se->schema()->partialValidate(ELEMENT_CAST(v_context));
    if (cmd_maker->firstChild()) {
        DBG(SE.VALIDATE) << "** Executing v-generated commands\n";
        batchCmd->executeAndAdd(cmd_maker.pointer());
    }
    GroveLib::NullValidationCommandsMaker null_cmd_maker;
    se->schema()->setCommandMaker(&null_cmd_maker);
    se->schema()->setRequiredAttrsProvider(0);
    if (cmdReplica) {
        Command* cmd = cmdReplica->firstChild();
        if (cmd)
            cmd = cmd->nextSibling();
        for (; cmd; cmd = cmd->nextSibling()) {
            v_context = cmd->asCommandContext()->validationContext();
            if (!v_context)
                break;
            if (v_context->nodeType() != GroveLib::Node::ELEMENT_NODE)
                v_context = v_context->parent();
            while (v_context) {
                if (XsNodeExt::xsElement(v_context))
                    break;
                if (!v_context->parent() ||
                    v_context->nodeType() != GroveLib::Node::ELEMENT_NODE) {
                        v_context = 0;
                        break;
                }
                v_context = v_context->parent();
            }
            if (v_context) {
                DBG(SE.VALIDATE) << "Partial validate [REPLICATED]: "
                                 << v_context->nodeName() << std::endl;
                se->schema()->partialValidate(
                    static_cast<GroveLib::Element*>(v_context), true);
            }
        }
    }
    se->schema()->setCommandMaker(0);
    if (rm_attrs)
        se->schema()->setValidationFlags(v_flags);
    /// Update Messages
    move_old_messages(cmdLink->prevMessageNodes(), v_context, msg_tree);
    MessageTreeNode* top_msg = msg_node->firstChild();
    for (; top_msg; top_msg = top_msg->nextSibling()) {
        MessageTreeNode* msg_node = top_msg->firstChild();
        while (msg_node) {
            MessageTreeNodePtr msg_holder(msg_node);
            MessageTreeNode* next = msg_node->nextSibling();
            cmdLink->myMessageNodes().insert(msg_node);
            msg_node->remove();
            msg_tree->appendWithFacility(msg_node);
            msg_node = next;
        }
    }
}

bool StructEditor::executeAndUpdate(const CommandPtr& cmd)
{
    //editPolicy_->finishIM();
    if (cmd.isNull() || !cmd->isValid()) {
        showCmdExecutionError();
        return false;
    }
    Formatter::TreePos old_cursor = editableView_->context().treePos();
    const GrovePos& fo_pos = editableView_->context().foPos();
    LiveNodeLocator fo_hint(fo_pos);

    editableView_->setNotModified();

    Command* command = cmd.pointer();

    // if replicated command contains only single entry, replace it
    // with just "command"
    GroveReplicatedCommand* cmd_replica = cmd->asGroveReplicatedCommand();
    if (cmd_replica && cmd_replica->firstChild() == cmd_replica->lastChild()) {
        command = cmd_replica->firstChild();
        cmd_replica->setAutoUnexecute(false);
        cmd_replica = 0;
    }
    else {
        try {
            command->execute();
        } 
        catch (Common::Exception& exc) {
            sernaDoc()->showMessageBox(
                SernaDoc::MB_CRITICAL,
                qApp->translate("StructEditor", "Command Execution Exception"),
                exc.what(), tr("&Ok"));
            return false;
        }    
    }
    CommandLinkPtr cmd_link = new CommandLink(command);
    CommandContext* context = command->asCommandContext();

    if (isValidationOn_) {
        // message-box for single transaction
        RefCntPtr<MacroCommand> macro = new MacroCommand;
        if (context->flags() & CommandContext::CF_BATCH) {
            Command* sub_cmd = command->firstChild();
            for (; sub_cmd; sub_cmd = sub_cmd->nextSibling()) {
                context  = sub_cmd->asCommandContext();
                cmd_replica = sub_cmd->asGroveReplicatedCommand();
                validate_command(this, cmd_link.pointer(),
                                 context, macro.pointer(), cmd_replica);
            }
        } 
        else
            validate_command(this, cmd_link.pointer(), context, 
                             macro.pointer(), cmd_replica);
        if (macro->firstChild()) {
            if (macro->firstChild() == macro->lastChild()) {
                cmd_link->setSecondaryCommand(macro->firstChild());
                macro->setAutoUnexecute(false);
            } else
                cmd_link->setSecondaryCommand(macro.pointer());
        }
        schema_->setMessenger(messageTree());
    }
    cmdExecutor_->executeAndAdd(cmd_link.pointer());
    removeSelection();
    DBG_IF(SE.SRCGROVE) Node::dumpSubtree(grove()->document());
    bool res = updateView(cmd_link.pointer(), old_cursor,
        fo_hint, 0, false);
    DBG_IF(SE.TIMING) TPROF_PRINTLAST;
    return res;
}

void StructEditor::doUntil(int op_depth, bool dontFormat)
{
    const bool is_undo = (0 > op_depth);
    int depth = abs(op_depth);

    Formatter::TreePos old_cursor = editableView_->context().treePos();
    const GrovePos& fo_pos = editableView_->context().foPos();
    LiveNodeLocator fo_hint(fo_pos);

    removeSelection();
    //editPolicy_->finishIM();

    Command* done = 0;
    while (depth--) {
        done = is_undo ? cmdExecutor_->undo() : cmdExecutor_->redo();
        if (!done)
            break;
        CommandLink* cmd_link = static_cast<CommandLink*>(done);
        // prev msgs.
        MessageTreeNode** msg_node = cmd_link->prevMessageNodes().list(); 
        if (is_undo) {
            for (; msg_node && *msg_node; ++msg_node) {
                if (!CommandLink::MessageNodeList::isValid(*msg_node))
                    continue;
                messageTree()->appendWithFacility(*msg_node);
            }
            for (msg_node = cmd_link->myMessageNodes().list(); 
                 msg_node && *msg_node; ++msg_node) {
                if (!CommandLink::MessageNodeList::isValid(*msg_node))
                    continue;
                (*msg_node)->remove();
            }
        }
        else {
            for (; msg_node && *msg_node; ++msg_node) {
                if (!CommandLink::MessageNodeList::isValid(*msg_node))
                    continue;
                (*msg_node)->remove();
            }
            for (msg_node = cmd_link->myMessageNodes().list(); 
                 msg_node && *msg_node; ++msg_node) {
                if (!CommandLink::MessageNodeList::isValid(*msg_node))
                    continue;
                messageTree()->appendWithFacility(*msg_node);
            }
        }
        messageTree()->cleanEmpty();
    }
    if (done)
        updateView(done, old_cursor, fo_hint, op_depth, dontFormat);
    editPolicy_->resetEnterPressCount();
    DBG_IF(SE.SRCGROVE) Node::dumpSubtree(grove()->document());
}

static void undoMessage(CommandLink* cmd, MessageTreeNode* msgTree)
{
    MessageTreeNode** msg_node = cmd->prevMessageNodes().list();
    for (; msg_node && *msg_node; ++msg_node) {
        if (!CommandLink::MessageNodeList::isValid(*msg_node))
            continue;
        msgTree->appendWithFacility(*msg_node);
    }
    for (msg_node = cmd->myMessageNodes().list(); 
         msg_node && *msg_node; ++msg_node) {
        if (!CommandLink::MessageNodeList::isValid(*msg_node))
            continue;
        (*msg_node)->remove();
    }
    msgTree->cleanEmpty();
}

static inline void set_treepos_cursor(StructEditor* se, 
                                      const Formatter::TreePos& cursor) 
{
    Formatter::AreaPos area_pos = 
        cursor.toAreaPos(se->editableView().rootArea());
    se->setCursor(area_pos, false);
}

// todo: for undo/redo only: if operation has no visual effect,
// check for folded ancestors, and unfold them, if any.
bool StructEditor::updateView(Command* command,
                              const Formatter::TreePos& old_cursor,
                              const LiveNodeLocator& foHint,
                              int depth,
                              bool dontFormat)
{
    CommandLink* cmd = static_cast<CommandLink*>(command);
    const CommandContext* context = cmd->getLink()->asCommandContext();
    if (0 == context)
        return false;
    if (!cmd->myMessageNodes().isNull() && depth >= 0 && isValidationOn() > 1) {
            doc_->showMessageBox(
                SernaDoc::MB_CRITICAL,
                tr("Operation Not Valid in Context"),
                tr("<qt><nobr><b>This operation is not allowed "
                   "by the current XML Schema.</b></nobr><br/>"
                   "<nobr>To override this, turn off "
                   "Strict Validation Mode (Edit->Validation->On)."
                   "</nobr></qt>"), tr("&Ok"));
        //! Unexecute command
        cmdExecutor_->undo();
        undoMessage(cmd, messageTree());
        update();
        if (!old_cursor.isNull()) 
            set_treepos_cursor(this, old_cursor);
        notifyUndoStateChange();
        return false;
    }
    if (!update(dontFormat)) {
        if (context->flags() & CommandContext::CF_NOVISUAL) {
            notifyUndoStateChange();
            if (!old_cursor.isNull())
                set_treepos_cursor(this, old_cursor);
            return true;
        }
        // now, allow contentmap operations in folded content 
        // (not only for undo/redo)
        for (Node* c = context->pos().node(); c; c = c->parent()) {
            if (is_node_folded(c)) {
                notifyUndoStateChange();
                return true; // collapsed element found - skip cursor set
            }
        }
        if (depth) {
            if (!old_cursor.isNull())
                set_treepos_cursor(this, old_cursor);
            notifyUndoStateChange();
            return true;
        }
        if (!doc_->showMessageBox(
                SernaDoc::MB_WARNING,
                tr("Operation has no visual effect"),
                tr("<qt><nobr><b>Operation has no visual effect</b></nobr>"
                   "<br/> (probably stylesheet does not reflect the change "
                   "made).<br/><nobr>To preserve document consistency, it "
                   "is recommended to cancel this operation.</nobr>"
                   "</nobr></qt>"), tr("&Cancel"), tr("&Proceed"))) {
            cmdExecutor_->undo();
            cmdExecutor_->removeLastCommand();
            undoMessage(cmd, messageTree());
            notifyUndoStateChange();
            viewUpdateFactory_->dispatchEvent();
            if (!old_cursor.isNull())
                set_treepos_cursor(this, old_cursor);
            return true;
        }
    }
    GrovePos src_pos;
    const SchemaCommandMaker* cmd_maker =
        dynamic_cast<const SchemaCommandMaker*>(cmd->getSecondaryCommand());
    // for batch command, try to use explicit src_pos
    if (cmd_maker) {
        GroveLib::Node* pos_hint = cmd_maker->posHint();
        DBG_IF(SE.CURSOR) {
            if (pos_hint) {
                DBG(SE.CURSOR) << "SCMD pos-hint node: "; pos_hint->dump();
                DBG(SE.CURSOR) 
                    << "SCMD-originated pos hint: " << (pos_hint->parent()
                         ? pos_hint->parent()->nodeName() : String(""))
                     << "/" << pos_hint->nodeName() << std::endl;
            }
            else
                DBG(SE.CURSOR) << "No pos hint\n";
        }
        if (pos_hint) {
            if (context->pos().node() && 
                (((context->flags() & CommandContext::CF_BATCH) &&
                    context->pos().node()->getGSR() &&
                    context->pos().node()->getGSR()->nodeType() ==
                    GroveLib::Node::DOCUMENT_NODE) ||
                (pos_hint->parent() == context->pos().node()->parent()))) 
                    src_pos = context->pos();
            else 
                src_pos = GrovePos(pos_hint);
        } else
            src_pos = GrovePos(context->pos());
    }
    else
        src_pos = GrovePos(context->pos());

    DBG(SE.CURSOR) << "Command suggested position:\n";
    DBG_EXEC(SE.CURSOR, src_pos.dump());

    GrovePos result_pos;
    while (!src_pos.isNull()) {
        result_pos = toResultPos(src_pos, foHint.node());
        if (!result_pos.isNull())
            break;
        src_pos = GrovePos(src_pos.node()->parent(), src_pos.node());
    }    
    if (src_pos.isNull() || result_pos.isNull()) {
        if (!(context->flags() & CommandContext::CF_OLDPOS) &&
            !depth && !doc_->showMessageBox(
            SernaDoc::MB_WARNING,
            tr("Cursor positioning failed"),
            tr("<qt><nobr><b>Cursor positioning failed</b>"
               " (probably because of stylesheet error).</nobr><br/><nobr>"
               "This can happen if stylesheet does not reflect the results "
               "of the last operation.</nobr><br/>"
               "<nobr>To preserve document consistency, it is recommended"
               " to cancel this operation.</nobr>"),
            tr("&Cancel"), tr("&Proceed"))) {
                cmdExecutor_->undo();
                cmdExecutor_->removeLastCommand();
                undoMessage(cmd, messageTree());
        }
        update();
        if (!old_cursor.isNull())
            set_treepos_cursor(this, old_cursor);
        notifyUndoStateChange();
        return true;
    }
    //! Setting cursor to position suggested bu executed command
    editPolicy_->lockEnterPressCount(true);
    Formatter::AreaPos area_pos = toAreaPos(result_pos, ADJUST_FORWARD);
    if (src_pos != EditContext::getSrcPos(area_pos))
        area_pos = toAreaPos(result_pos, ADJUST_BACKWARD);
    bool cursor_set = setCursor(src_pos, area_pos, false);
    editPolicy_->lockEnterPressCount(false);
    if (!cursor_set) {
        DBG(SE.CURSOR) << "Cursor placement may be inaccurate\n";
        if (!old_cursor.isNull())
            set_treepos_cursor(this, old_cursor);
    }
    notifyUndoStateChange();
    return true;
}

static void set_context_hints(EditableView* editableView,  bool isShowTags)
{
    PropertyNode* context_hints = config().root()->makeDescendant(
        String(DocLook::DOC_LOOK) + '/' + String(DocLook::CONTEXT_HINTS));
    bool hints_enabled =
        context_hints->makeDescendant(DocLook::HINTS_ENABLED)->getBool();
    bool hints_persistent =
        context_hints->makeDescendant(DocLook::HINTS_PERSISTENT)->getBool();
    bool hints_showparent =
        context_hints->makeDescendant(DocLook::HINTS_SHOW_PARENT)->getBool();
    int  hints_delay =
        context_hints->makeDescendant(DocLook::HINTS_DELAY)->getInt();
    editableView->setContextHint(hints_enabled && !isShowTags,
                                 hints_persistent, 
                                 hints_showparent,
                                 hints_delay);
}

static void update_area_view(const Formatter::Area* area)
{
    Formatter::AreaView* area_view = area->getView();
    if (area_view)
        area_view->updateDecoration();
    for (Formatter::Area* c = area->firstChild(); c; c = c->nextSibling())
        update_area_view(c);
}

void StructEditor::updateView()
{
    if (VIEW_FORMAT & changeType_) {
        format();
        sernaDoc()->showStageInfo();
    }
    else {
        if (CONTEXT_HINTS & changeType_)
            set_context_hints(editableView_, viewParam_->showTags_);
        if (TAG_COLORS & changeType_) {
            colorSchemeImpl_->setColors();
            update_area_view(editableView_->rootArea());
        }
    }
    changeType_ = 0;
}

class DumpFoTree;

void StructEditor::format()
{
    doc_->showStageInfo(tr("Formatting"));
    const Formatter::TreePos& cursor = editableView_->context().treePos();
    GrovePos src_pos = editViewSrcPos();
    initViewParam();
    //editPolicy_->finishIM();
    //DBG_IF(XSL.TYPES) cursor.dump();
    try {
        editableView_->detachAreaTree();
        set_context_hints(editableView_, viewParam_->showTags_);
        editableView_->format(*viewParam_);
    }
    catch (Exception& e) {
        makeCommand<DumpFoTree>()->execute(this);
        throw StructEditorException(e.what());
    }
    Formatter::AreaPos area_pos;
    if (!cursor.isNull()) 
        area_pos = cursor.toAreaPos(editableView_->rootArea());
    if (area_pos.isNull()) {
        area_pos = editableView_->getInitialCursor();
        src_pos = EditContext::getSrcPos(area_pos);
    }
    editableView_->EditableView::setCursor(area_pos, src_pos, true);
}

class RevalidateSilent;

void StructEditor::setValidationMode()
{
    isValidationOn_ = 0;
    const Sui::Action* a = uiActions().validationMode()->activeSubAction();
    if (!a || !a->parent()->getBool(Sui::IS_ENABLED))
        return;
    String vmode = a->get(Sui::NAME);
    uiActions().revalidate()->setEnabled(vmode != NOTR("off"));
    if (vmode != NOTR("off")) {
        isValidationOn_ = 1;
        if (vmode == NOTR("strict"))
            isValidationOn_ = 2;
    } 
    getDsi()->makeDescendant(DocSrcInfo::VALIDATION_MODE)->setString(vmode);
}

void StructEditor::showCmdExecutionError() const
{
    if (groveEditor_->errorMessage().isEmpty())
        return;
    sernaDoc()->showMessageBox(SernaDoc::MB_CRITICAL,
                               tr("Command Execution Failed"),
                               groveEditor_->errorMessage(), tr("&Ok"));
}

////////////////////////////////////////////////////////////////////////

class ExecuteAndUpdate : public Common::CommandEvent<StructEditor> {
public:
    ExecuteAndUpdate(const Common::CommandPtr& context)
        : context_(context) {}
    virtual bool doExecute(StructEditor* se, Common::EventData*)
    {
        return se->executeAndUpdate(context_.pointer());
    }
private:
    Common::CommandPtr context_;
};

COMMON_NS_BEGIN
template<> COMMON_EXPORT Common::CommandEventPtr
makeCommand<ExecuteAndUpdate>(const Common::EventData* ed)
{
    return new ExecuteAndUpdate
        (static_cast<const GroveCommandEventData*>(ed)->gc_);
}
COMMON_NS_END

