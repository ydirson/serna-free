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
#include "structeditor/SetCursorEventData.h"
#include "structeditor/ElementList.h"
#include "structeditor/impl/debug_se.h"
#include "structeditor/impl/EditPolicyImpl.h"
#include "structeditor/impl/SelectionHistory.h"
#include "structeditor/impl/ViewParamImpl.h"
#include "structeditor/impl/PositionWatcher.h"
#include "structeditor/impl/StructAutoSave.h"
#include "structeditor/impl/SchemaCommandMaker.h"
#include "structeditor/impl/XsUtils.h"
#include "structeditor/impl/RecentElementsCache.h"

#include "utils/DocSrcInfo.h"
#include "utils/SernaMessages.h"
#include "utils/Properties.h"
#include "utils/message_utils.h"
#include "utils/AutoSaveMgr.h"
#include "utils/IdleHandler.h"
#include "utils/Version.h"
#include "common/PropertyTreeEventData.h"
#include "utils/SernaCatMgr.h"
#include "ui/UiItemSearch.h"

#include "grove/XmlNs.h"
#include "grove/Nodes.h"
#include "grove/Grove.h"
#include "grove/SectionSyncher.h"
#include "grove/IdManager.h"
#include "grove/ElementMatcher.h"

#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/PathName.h"
#include "common/StringTokenizer.h"
#include "common/StringCvt.h"
#include "common/CommandExecutor.h"
#include "common/PropertyTreeEventFactory.h"
#include "urimgr/Resource.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"

#include "editableview/EditableView.h"
#include "sceneareaset/SceneView.h"

#include "docview/SernaDoc.h"
#include "docview/MessageView.h"
#include "docview/Clipboard.h"
#include "docview/DocumentStateEventData.h"

#include "xs/Schema.h"
#include "xslt/Engine.h"
#include "xslt/Exception.h"

#include "genui/StructDocumentActions.hpp"
#include <qapplication.h>
#include <qeventloop.h>

USING_COMMON_NS

using namespace GroveLib;
using namespace GroveEditor;
using namespace Formatter;
using namespace MessageUtils;

SIMPLE_COMMAND_EVENT_IMPL(UpdateView, StructEditor)

bool UpdateView::doExecute(StructEditor* watcher, EventData*)
{
    watcher->updateView();
    return true;
}

COMMAND_EVENT_WITH_DATA_IMPL(UpdateContextHints, StructEditor)

bool UpdateContextHints::doExecute(StructEditor* watcher, EventData*)
{
    watcher->notifyChanged(StructEditor::CONTEXT_HINTS);
    return true;
}

COMMAND_EVENT_WITH_DATA_IMPL(UpdateTagColors, StructEditor)

bool UpdateTagColors::doExecute(StructEditor* watcher, EventData*)
{
    watcher->notifyChanged(StructEditor::TAG_COLORS);
    return true;
}

COMMAND_EVENT_WITH_DATA_IMPL(UpdateViewFormat, StructEditor)
bool UpdateViewFormat::doExecute(StructEditor* watcher, EventData*)
{
    watcher->notifyChanged(StructEditor::VIEW_FORMAT);
    return true;
}

///////////////////////////////////////////////////////////////////////////

static void enable_init_actions(StructEditor* se)
{
    se->uiActions().paste()->setEnabled(
        serna_clipboard().getStructClipboard(false)->firstChild());
    se->uiActions().pasteAsText()->setEnabled(
        serna_clipboard().getStructClipboard(false)->firstChild());

    se->uiActions().saveDocument()->setEnabled(se->isModified());

    // validation mode settings
    String validation_mode(NOTR("off"));
    if (se->schema()) {
        validation_mode = se->getDsi()->getSafeProperty(
            DocSrcInfo::VALIDATION_MODE)->getString();
        if (validation_mode.isEmpty())
            validation_mode = NOTR("strict");
        // if there are any validation errors on open, use validation ON
        // instead of STRICT
        if (NOTR("strict") == validation_mode) {
            MessageTreeNode* mnode = se->messageTree()->firstChild();
            for (; mnode; mnode = mnode->nextSibling()) {
                if (mnode->lastChild() && mnode->lastChild()->getMessage() &&
                    mnode->lastChild()->getMessage()->facility() == 6) {
                    validation_mode = NOTR("on");
                    PropertyNode* prop = config().root()->makeDescendant(
                        App::DONT_SHOW_VALIDATION_ON_WARNING_FLAG);
                    if (prop->getBool())
                        break;
                    prop->setBool(1 == se->sernaDoc()->showMessageBox(
                        SernaDoc::CHECKED_WARNING,
                        qApp->translate(
                            "StructEditor",
                            "Document Schema Validation Failed"),
                        qApp->translate(
                            "StructEditor",
                            "<qt><nobr>Document validation failed.</nobr><br/>"
                            "<nobr>Validation mode is now switched from "
                            "<b>Strict</b> to <b>On</b>.</nobr></qt>"),
                        tr("&Ok")));
                    break;
                }
            }
        }
    }
    const bool enable_validation = validation_mode != NOTR("off");
    se->uiActions().validationSubmenuCmd()->setEnabled(enable_validation);
    se->uiActions().revalidate()->setEnabled(enable_validation);
    se->uiActions().validationMode()->setActiveSubAction(validation_mode);
    se->setValidationMode();
    //
    se->uiActions().undo()->setEnabled(false);
    se->uiActions().redo()->setEnabled(false);

#ifdef __APPLE__
    se->uiActions().paste()->setEnabled(true);
#endif

    if (!(se->grove()->groveBuilder()->flags() &
        GroveLib::GroveBuilder::processXinclude)) {
            se->uiActions().insertXincludeXml()->setEnabled(false);
            se->uiActions().insertXincludeText()->setEnabled(false);
            // disconnect disabled ui item from action
            se->uiActions().convertToXinclude()->items().removeAllChildren();
    }
}

StructEditor::StructEditor(Sui::Action*, SernaDoc* doc)
    : Sui::Item(Sui::STRUCT_EDITOR, 0),
      SourceDocument(doc),
      editableView_(0),
      cmdExecutor_(new CommandExecutor),
      positionWatcher_(new PositionWatcher),
      elemContextChangeFactory_(new DynamicEventFactory),
      viewUpdateFactory_(new DynamicEventFactory),
      selectionChangeFactory_(new DynamicEventFactory),
      doubleClickFactory_(new DynamicEventFactory),
      tripleClickFactory_(new DynamicEventFactory),
      maybeTooltipFactory_(new DynamicEventFactory),
      lastSetPosNode_(0),
      historyChangeFactory_(new DynamicEventFactory),
      elementList_(new ElementList(this)),
      autoSaver_(make_struct_autosaver(this)),
      forceModified_(false),
      isInitialized_(false),
      changeType_(0)
{
}

// called _after_ the user interface is built.
void StructEditor::postInit()
{
    PropertyNode* balance_selection =
        getDsi()->getProperty(DocSrcInfo::SELECTION_BALANCING);
    bool balancing_on = true;
    if (balance_selection)
        balancing_on = balance_selection->getBool();
    editableView_->setSelectionBalancing(balancing_on);
    getDsi()->makeDescendant(DocSrcInfo::SELECTION_BALANCING)->
        setBool(balancing_on);
    uiActions().balanceSelection()->setToggled(balancing_on);
    uiActions().toggleMarkup()->setToggled(
        getDsi()->getSafeProperty(DocSrcInfo::SHOW_TAGS)->getBool());
    setCursorFromLineInfo();
    setCursorFromTreeloc();
    enable_init_actions(this);
    enableActions(editViewSrcPos());
    editableView_->grabFocus();

    uiActions().togglePageMode()->setToggled(
        getDsi()->getSafeProperty(DocSrcInfo::SHOW_PAGINATED)->getBool());
}

StructEditor::~StructEditor()
{
    if (editableView_) {
        editableView_->deleteFormatter();
        delete editableView_;
    }
    DDBG << "STRUCTEDITOR::~STRUCTEDITOR\n";
}

EditableView& StructEditor::editableView() const
{
    return *editableView_;
}

class SetZoom;

bool StructEditor::doAttach()
{
    QWidget* parent_widget = parent()->widget(this);
    DBG(SE.TEST) << "StructEditor: attach" << std::endl;
    if (parent_widget != editableView_->widget()->parentWidget()) {
        DBG(SE.TEST) << "StructEditor: attach to: "
                     << parent_widget << std::endl;
        makeCommand<SetZoom>()->execute(this);
        editableView_->widget()->setParent(parent_widget);
        editableView_->widget()->setVisible(getBool(Sui::IS_VISIBLE));
        doc_->postInit();
        showContextInfo();
        editableView_->grabFocus();
    }
    return true;
}

void StructEditor::grabFocus() const
{
    Item::grabFocus();
    editableView_->grabFocus();
}

QWidget* StructEditor::widget(const Sui::Item*) const
{
    return editableView_->widget();
}

void StructEditor::showContextMenu(const QPoint& pos)
{
    Sui::Item* menu = (editableView().getSelection().src_.isEmpty())
        ? findItem(Sui::ItemName(NOTR("structEditorContextMenu")))
        : findItem(Sui::ItemName(NOTR("selectionContextMenu")));
    if (menu)
        menu->showContextMenu(pos);
}

class StructClipboardChanged;

void StructEditor::init()
{
    DBG(SE.TEST) << "StructEditor: init" << std::endl;
    String style_path = getDsi()->getSafeProperty(
        DocSrcInfo::RESOLVED_STYLE_PATH)->getString();
    MessageTreeNodePtr xsltMessageTree = new MessageTreeNode;

    xsltEngine_ = Xslt::Engine::makeEngine(xsltMessageTree.pointer(),
                                           sernaDoc()->progressStream(),
                                           uriMapper()->catalogManager());

    DBG(SE.TEST) << "Using Stylesheet: " << style_path << std::endl;
    xsltEngine_->setStyle(style_path);
    MessageTreeNode* mtn = xsltMessageTree->firstChild();
    if (mtn)
        mtn = mtn->firstChild();
    if (mtn) {
        String msg = tr("Stylesheet parse error:\n");
        for (; mtn; mtn = mtn->nextSibling()) {
            if (0 == mtn->getMessage())
                continue;
            msg += msg_str(mtn->getMessage()) + "\n";
        }
        throw StructEditorException(msg);
    }
    xsltEngine_->setMessenger(messageTree());

    DocumentStateEventData evd(DocumentStateEventData::BEFORE_TRANSFORM);
    sernaDoc()->stateChangeFactory().dispatchEvent(&evd);
    doc_->showStageInfo(tr("XSLT: Transforming Document. Please Wait..."));
    fot_ = xsltEngine_->transform(grove_);

    if (fot_.isNull() || !fot_->document()->documentElement())
        throw StructEditorException(
              String(tr("XSLT transform produced empty FO tree")));

    //! Formatting editable view
    getDsi()->makeDescendant(DocSrcInfo::SHOW_PAGINATED, "true", false);
    uiActions().togglePageMode()->setToggled(
        getDsi()->getSafeProperty(DocSrcInfo::SHOW_PAGINATED)->getBool());

    editPolicy_ = new EditPolicyImpl(this);
    editableView_ = make_editable_view(messageTree(), fot_,
                                       editPolicy_.pointer());

    editableView_->setProgressStream(doc_->progressStream());
    editPolicy_->setEditableView(editableView_);
    format();

    selectionHistory_ = new SelectionHistory();
    uiActions().deselectParent()->setEnabled(
        selectionHistory().isUndoPossible());

    groveEditor_ = new GroveEditor::Editor;
    serna_clipboard().clipboardChange().subscribe
        (makeCommand<StructClipboardChanged>, this);

    executor()->setDepthLimit(config().getProperty(App::APP)->
        getSafeProperty(App::MAX_UNDO_DEPTH)->getInt());

    //! TODO: remove factory update after config notification bugfix
    config().eventFactory().update();
    config().updateFinishFactory().subscribe(makeCommand<UpdateView>, this);
    config().eventFactory().subscribe(DocLook::HINTS_ENABLED,
                                      makeCommand<UpdateContextHints>, this);
    config().eventFactory().subscribe(DocLook::HINTS_PERSISTENT,
                                      makeCommand<UpdateContextHints>, this);
    config().eventFactory().subscribe(DocLook::HINTS_SHOW_PARENT,
                                      makeCommand<UpdateContextHints>, this);
    config().eventFactory().subscribe(DocLook::HINTS_DELAY,
                                      makeCommand<UpdateContextHints>, this);

    config().eventFactory().subscribe(DocLook::INDENT,
                                      makeCommand<UpdateViewFormat>, this);
    config().eventFactory().subscribe(DocLook::TAG_FONT,
                                      makeCommand<UpdateViewFormat>, this);
    config().eventFactory().subscribe(DocLook::EVEN_COLOR,
                                      makeCommand<UpdateViewFormat>, this);
    config().eventFactory().subscribe(DocLook::ODD_COLOR,
                                      makeCommand<UpdateViewFormat>, this);

    config().eventFactory().subscribe(String(DocLook::NS_COLOR_MAP),
                                      makeCommand<UpdateTagColors>, this);
    config().eventFactory().subscribe(DocLook::NS_COLOR_MAP_ENTRY,
                                      makeCommand<UpdateTagColors>, this);
    config().eventFactory().subscribe(DocLook::NS_URI,
                                      makeCommand<UpdateTagColors>, this);
    config().eventFactory().subscribe(DocLook::NS_TAG_COLOR,
                                      makeCommand<UpdateTagColors>, this);
    config().eventFactory().subscribe(DocLook::NS_TEXT_COLOR,
                                      makeCommand<UpdateTagColors>, this);
    isInitialized_ = true;
}

StructDocumentActions& StructEditor::uiActions()
{
    return static_cast<StructDocumentActions&>(sernaDoc()->uiActions());
}

StructDocumentActionGroups& StructEditor::actionGroups()
{
    return static_cast<StructDocumentActionGroups&>
        (sernaDoc()->actionGroups());
}

Common::DynamicEventFactory& StructEditor::elementContextChange()
{
    return *elemContextChangeFactory_;
}

Common::DynamicEventFactory& StructEditor::selectionChange()
{
    return *selectionChangeFactory_;
}

Common::DynamicEventFactory& StructEditor::historyChange()
{
    return *historyChangeFactory_;
}

Common::DynamicEventFactory& StructEditor::viewUpdate()
{
    return *viewUpdateFactory_;
}

Common::DynamicEventFactory& StructEditor::doubleClick()
{
    return *doubleClickFactory_;
}

Common::DynamicEventFactory& StructEditor::tripleClick()
{
    return *tripleClickFactory_;
}

Common::DynamicEventFactory& StructEditor::maybeTooltip()
{
    return *maybeTooltipFactory_;
}

void StructEditor::notifyUndoStateChange()
{
    StructDocumentActions& sdc = uiActions();
    sdc.undo()->setEnabled(cmdExecutor_->isUndoPossible());
    sdc.redo()->setEnabled(cmdExecutor_->isRedoPossible());
    sdc.saveDocument()->setEnabled(isModified());
    if (historyChange().hasSubscribers())
        historyChange().dispatchEvent();
}

void StructEditor::notifySelectionChange(const ChainSelection& chainSel,
                                         const GroveSelection& srcSel)
{
    const bool is_empty = srcSel.isEmpty();
    actionGroups().selectionOnActions().setEnabled(!is_empty);
    uiActions().editSelectionAsText()->setEnabled(!is_empty);
    uiActions().commentSelection()->setEnabled(!is_empty);
    if (selectionChange().hasSubscribers()) {
        SelectionEventData ed(chainSel, srcSel);
        selectionChange().dispatchEvent(&ed);
    }
    if (is_empty) {
        const Node* n = editViewSrcPos().node();
        if (n)
            n = parentNode(n);
        uiActions().deleteElement()->setEnabled
            (n && n->nodeType() == Node::ELEMENT_NODE);
    } else
        uiActions().deleteElement()->setEnabled(false);
    uiActions().deselectParent()->setEnabled(
        selectionHistory().isUndoPossible());
}

void StructEditor::enableActions(const GrovePos& pos)
{
    bool valid_pos       = false;
    bool can_split       = false;
    bool can_join        = false;
    bool can_navigate    = false;
    bool within_entity   = false;
    bool edit_pi         = false;
    bool edit_comment    = false;
    bool is_choice       = false;
    const EntityReferenceStart* ers = 0;

    if (!pos.isNull()) {
        can_navigate = true;
        ers = pos.getErs();
        if (ers->getSectParent())
            within_entity = true;
        Node* node = 0;
        if (pos.node()->nodeType() == GroveLib::Node::CHOICE_NODE)
            is_choice = true;
        else
            node = pos.node();
        if (node) {
            valid_pos = true;
            if (pos.type() == GrovePos::ELEMENT_POS) {
                Node* before = pos.before();
                if (before) {
                    Node* prev = before->prevSibling();
                    if (prev && prev->nodeType() == Node::ELEMENT_NODE &&
                        before->nodeType() == Node::ELEMENT_NODE &&
                        before->nodeName() == prev->nodeName())
                            can_join = true;
                }
            }
            switch (node->nodeType()) {
                case GroveLib::Node::PI_NODE:
                    edit_pi = true;
                    break;

                case GroveLib::Node::COMMENT_NODE:
                    edit_comment = true;
                    break;

                case GroveLib::Node::TEXT_NODE:
                    node = node->parent();
                    /* FALL THRU */
                default:
                    if (node && node->parent() && node->parent()->parent())
                        can_split = true;
                break;
            }
        }
    }
    StructDocumentActions& action_set = uiActions();
    action_set.splitElement()->setEnabled(can_split);
    action_set.joinElements()->setEnabled(can_join);
    action_set.convertFromEntity()->setEnabled(within_entity);
    action_set.xincludeStatus()->setEnabled(
        within_entity &&
        ers->entityDecl()->declType() == EntityDecl::xinclude);
    action_set.editPi()->setEnabled(edit_pi);
    action_set.editComment()->setEnabled(edit_comment);

    action_set.uncomment()->setEnabled(edit_comment);

    actionGroups().validPosGroup().setEnabled(valid_pos);
    actionGroups().navigationActionGroup().setEnabled(can_navigate);

    if (is_choice)
        action_set.insertElement()->setEnabled(true);
    action_set.insertPi()->setEnabled(!(edit_pi || edit_comment));
    action_set.insertComment()->setEnabled(!(edit_pi || edit_comment));
    action_set.deleteElement()->setEnabled((can_split|edit_comment|edit_pi)
        && editableView().getSelection().src_.isEmpty());
    action_set.foldElement()->setEnabled(can_split);
    action_set.untagElement()->setEnabled(can_split);
    if (edit_pi || edit_comment) {
        action_set.insertInternalEntity()->setEnabled(false);
        action_set.insertExternalEntity()->setEnabled(false);
        action_set.insertXincludeXml()->setEnabled(false);
        action_set.insertXincludeText()->setEnabled(false);
    }
}

const GroveLib::StripInfo* StructEditor::stripInfo() const
{
    return xsltEngine_->stripInfo();
}

void StructEditor::setNotModified()
{
    cmdExecutor_->setNotModified();
    forceModified_ = false;
    notifyUndoStateChange();
}

bool StructEditor::isModified() const
{
    return forceModified_ || cmdExecutor_->isModified();
}

String StructEditor::generateId(const String& fmt) const
{
    const GroveLib::Node* node = editViewSrcPos().node();
    if (0 == node)
        node = grove()->document()->documentElement();
    const PropertyNode* pn =
        getDsi()->getProperty(DocSrcInfo::ELEMENT_ID_FORMAT);
    String id_format(fmt.isNull() ?
        (pn ? pn->getString() : String(NOTR("%l_%n"))) : fmt);
    return GroveLib::make_id_string(node, id_format);
}

/////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(SetFocusToEditWindow, StructEditor)

bool SetFocusToEditWindow::doExecute(StructEditor* se, EventData*)
{
    se->grabFocus();
    return true;
}

/////////////////////////////////////////////////////////////////////

static void remove_doc_inst(void* sd)
{
    ((SernaDoc*) sd)->removeItem();
}

void StructEditor::abortSession(const COMMON_NS::String& srcWhat)
{
    qt_clean_sst();
    if (editableView_)
        editableView_->EditableView::setCursor(AreaPos(), GrovePos(), true);
    QString what = srcWhat;
    what.replace(NOTR("<"), NOTR("&lt;"));
    if (!isInitialized_) {
        doc_->showMessageBox(
            SernaDoc::MB_CRITICAL, tr("Critical error"),
            tr("<qt><pre>%0</pre><nobr><b>Document cannot be opened."
                "</b></nobr></qt>").arg(what), tr("&OK"));
        return;
    }
    if (cmdExecutor_.isNull() || !cmdExecutor_->isModified()) {
        doc_->showMessageBox(
            SernaDoc::MB_CRITICAL, tr("Critical error"),
            tr("<qt><pre>%0</pre>"
            "<br/>Critical error has occured. Please try to re-open "
            " the document.</br>Document will be closed now: "
            "No changes were made since last save.").arg(what), tr("&OK"));
    }
    else
        if (doc_->showMessageBox(
            SernaDoc::MB_CRITICAL, tr("Critical command execution error"),
            tr("<qt><pre>%0</pre>"
            "<br/><nobr><b>Critical error has occured. Please try to re-open "
            " the document.<br/>Document will be closed now. "
            "<b>Save your changes?</b></nobr></qt>").arg(what),
            tr("&Yes"), tr("&No")) == 0)
                saveDocument(0);
    // delay call because Qt event loop is not yet empty
    sst_delayed_call(remove_doc_inst, sernaDoc());
    return;
}

bool StructEditor::executeEvent(CommandEventBase* event, EventData* ed)
{
    try {
        return CommandEventContext::executeEvent(event, ed);
    }
    catch (Command::Exception& e) {
        doc_->showMessageBox(
            SernaDoc::MB_WARNING, tr("Command execution error"),
            tr("<qt><pre>%1</pre><nobr><b>Last user command not done."
               "</b></nobr>").arg(e.whatString().qstring()), tr("&OK"));
        return false;
    }
    catch (Xslt::Exception& e) {
        Uri::uriManager().releaseResource(NOTR("xslt"), xsltEngine_->getStyle());
        abortSession(e.whatString());
        return false;
    }
    catch (Common::Exception& e) {
        abortSession(e.whatString());
        return false;
    }
    catch (std::exception& e) {
        doc_->showMessageBox(
            SernaDoc::MB_CRITICAL, tr("Unknown exception"),
            tr("<qt><nobr><b>Internal error: Caught unhandled exception: ") +
            QString(typeid(e).name()) + NOTR("</b></nobr></qt>"), tr("&OK"));
        abortSession(e.what());
        return false;
    }
}

