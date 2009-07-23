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
#include "grove/Nodes.h"

#include "formatter/area_pos_utils.h"
#include "formatter/impl/ReferencedFo.h"
#include "formatter/impl/TerminalFos.h"
#include "editableview/ToolTipEventData.h"

#include "structeditor/StructEditor.h"
#include "structeditor/InsertTextEventData.h"
#include "structeditor/impl/EditPolicyImpl.h"
#include "structeditor/impl/SelectionHistory.h"
#include "structeditor/impl/InputMethod.h"
#include "structeditor/SernaDragData.h"
#include "structeditor/impl/debug_se.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"

#include "utils/SernaMessages.h"
#include "utils/IdleHandler.h"
#include "utils/Config.h"
#include "utils/file_utils.h"
#include "utils/DocSrcInfo.h"
#include "common/PropertyTreeEventData.h"
#include "utils/GrovePosEventData.h"

#include "ui/IconProvider.h"
#include "ui/MimeHandler.h"

#include "docview/Clipboard.h"
#include "docview/SernaDoc.h"
#include "genui/StructDocumentActions.hpp"

#include "common/safecast.h"
#include "common/CommandEvent.h"
#include "common/ScopeGuard.h"

#include <QApplication>
#include <QClipboard>
#include <QRect>
#include <QDialog>
#include <QKeyEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QToolTip>
#include <QTextDocument>
#include <QVariant>
#include <Qt>

#include <iostream>

using namespace GroveLib;
using namespace Common;
using namespace FileUtils;
using namespace GroveEditor;

const int PAGE_BREAK_SPACE = 10;
const int PAGE_MARGIN = 10;

using namespace Formatter;

// Used external events:
class InsertElement;
class InsertText;
class EditPi;
class EditComment;
class SplitElement;
class AdvSplitElement;
class GoToPrevElement;
class GoToNextElement;
class GoToElementStart;
class GoToElementEnd;
class DoBackspace;
class DoDelete;
class StructCut;
class UnfoldElement;

static void reload_document(void* se)
{
    ((StructEditor*) se)->uiActions().reloadDocument()->dispatch();
}

static Node* element_node(Node* node)
{
    while (node && Node::ELEMENT_NODE != node->nodeType())
        node = node->parent();
    return node;
}

static int split_depth(StructEditor* se, const GrovePos& posBeforeSplit)
{
    if (posBeforeSplit.isNull())
        return 0;
    GrovePos curr_pos;
    if (!se->getCheckedPos(curr_pos,
        StructEditor::ANY_OP|StructEditor::SILENT_OP))
            return 0;
    Node* node = element_node(curr_pos.node());
    Node* before_split = element_node(posBeforeSplit.node());
    int i = 0;
    while (node) {
        if (before_split->parent() == node)
            return i;
        i++;
        node = node->parent();
    }
    return 0;
}

EditPolicyImpl::EditPolicyImpl(StructEditor* se)
    :  dblClicked_(false),
       enterPressCountLocked_(false),
       mousePressed_(false),
       continueText_(false),
       enterPressCount_(0),
       editableView_(0),
       structEditor_(se),
       mousePressPos_(QPoint())
{
}

class InsertElement;
class StructPaste;
class StructCopy;
class StructMousePaste;
class StructMouseCopy;
class SelectParent;

void EditPolicyImpl::lockEnterPressCount(bool isLock)
{
    enterPressCountLocked_ = isLock;
}

//// Drag and Drop actions ///////////////////////////////////////////////////

void EditPolicyImpl::contentsDragEnterEvent(QDragEnterEvent* event,
                                            const AreaPos&)
{
    DBG(SE.DRAG) << "EditPolicy: dragEnterEvent\n";
    SernaDocFragment::dragEnter(structEditor_, event);
}

void EditPolicyImpl::contentsDragMoveEvent(QDragMoveEvent* event,
                                           const AreaPos& pos)
{
    if (pos.isNull())
        return event->ignore();
    GrovePos drop_pos = editableView_->context().getSrcPos(pos);
    if (pos != editableView_->context().areaPos()) {
        editableView_->setCursor(pos, drop_pos, false);
        editableView_->stopCursor();
    }
    SernaDocFragment* sdf = SernaDocFragment::fragment(sernaDoc());
    if (sdf)
        sdf->setDropData(drop_pos, structEditor_);
    sernaDoc()->mimeHandler().
        callback(Sui::MimeCallback::VALIDATE_CONTENT, event); 
}

void EditPolicyImpl::contentsDragLeaveEvent(QDragLeaveEvent*)
{
}

void EditPolicyImpl::contentsDropEvent(QDropEvent* event, const AreaPos& pos)
{
    DBG(SE.DRAG) << "EditPolicy: dropEvent\n";
    Sui::MimeHandler& mh = sernaDoc()->mimeHandler();
    SernaDocFragment* sdf = SernaDocFragment::fragment(sernaDoc());
    if (sdf)
        sdf->setDropData(editableView_->context().getSrcPos(pos), 
                         structEditor_);
    mh.callback(Sui::MimeCallback::VALIDATE_CONTENT, event); 
    mh.callback(Sui::MimeCallback::DROP_CONTENT, event);
    editableView_->startCursor();
    editableView_->showCursor();
}

///////////////////////////////////////////////////////////////////////////

#ifdef GUITEST
STRUCTEDITOR_EXPIMP void (*GuiTest_record_mouse_press)
    (EditableView* editableView, const AreaPos& pos, QMouseEvent* e) = 0;
#endif // GUITEST

void EditPolicyImpl::mousePressed(const AreaPos& pos, QMouseEvent* e)
{
#ifdef GUITEST
    if (GuiTest_record_mouse_press)
        (*GuiTest_record_mouse_press)(editableView_, pos, e);
#endif
    resetEnterPressCount();
    breakText();
    IdleHandler::resetIdleTimer();
    editableView_->setMicroFocusHint();
    if (Qt::LeftButton == e->button()) {
        if (e->state() & Qt::ShiftModifier) {
            sendCursorParam(pos, true, true);
            return;
        }
        Selection sel = editableView_->getSelection();
        bool is_inside_selection = false;
        if (!sel.tree_.isEmpty()) {
            ChainPos start(sel.tree_.minLoc().toChainPos(
                               editableView_->rootArea()));
            ChainPos end(sel.tree_.maxLoc().toChainPos(
                             editableView_->rootArea()));
            ChainPos chain_pos(pos);
            is_inside_selection = !(chain_pos < start) && !(chain_pos > end);
        }
        if (is_inside_selection) {
            GrovePos src_pos = editableView_->context().getSrcPos(pos);
            editableView_->setCursor(pos, src_pos, false);
            mousePressPos_ = e->pos();
            mousePressed_ = true;
        }
        else
            sendCursorParam(pos, true, false);

        const AreaPos& area_pos = editableView_->context().areaPos();
        if (area_pos.isNull())
            return;
        switch (area_pos.area()->type()) {
            case CHOICE_AREA:
                makeCommand<InsertElement>()->execute(structEditor_);
                break;
            case COMBO_BOX_AREA:
            case LINE_EDIT_AREA:
                edit_simple_form_area(structEditor_, e);
                break;
            default:
                break;
        }
        return;
    }
    if (Qt::RightButton == e->button()) {
        if (editableView_->getSelection().src_.isEmpty())
            sendCursorParam(pos, true, false);
        structEditor_->showContextMenu(e->globalPos());
    }
    else if (Qt::MidButton == e->button()) {
        //only do middle-click pasting on systems that have selections(ie. X11)
        if (serna_clipboard().supportsSelection())   {
           if (!editableView_->getSelection().tree_.isEmpty())
               sendCursorParam(pos, true, false);
           makeCommand<StructMousePaste>()->execute(structEditor_);
        }
    }
}

void EditPolicyImpl::mouseReleased(QMouseEvent* e)
{
    resetEnterPressCount();
    breakText();
    if (mousePressed_) {
        structEditor_->removeSelection();
        mousePressed_ = false;
    }
    if (Qt::LeftButton == e->button() && serna_clipboard().supportsSelection())
        makeCommand<StructMouseCopy>()->execute(structEditor_);
}

void EditPolicyImpl::mouseMoved(const AreaPos& pos, QMouseEvent* e)
{
    if (mousePressed_ && (mousePressPos_ - e->pos()).manhattanLength() >
        QApplication::startDragDistance()) {
        mousePressed_ = FALSE;
        QDrag* drag = sernaDoc()->mimeHandler().
            makeDrag(structEditor_->widget(0));
        SernaDragData* sdd = new SernaDragData(structEditor_);
        drag->setPixmap(Sui::icon_provider().
            getPixmap(NOTR("insert_element")));
        sdd->startDrag(drag, sernaDoc()->mimeHandler());
        return;
    }
    if (mousePressed_)
        return;
    if (Qt::LeftButton != e->state())
        return;
    IdleHandler::resetIdleTimer();
    sendCursorParam(pos, false, true);
}

bool EditPolicyImpl::sendDoubleClickEvent()
{
    return structEditor_->doubleClick().
        dispatchEvent(0, DynamicEventFactory::And);
}

void EditPolicyImpl::mouseDoubleClicked(const AreaPos& pos)
{
    resetEnterPressCount();
    breakText();
    IdleHandler::resetIdleTimer();
    sendCursorParam(pos, true, false);
    const AreaPos& area_pos = editableView_->context().areaPos();
    if (area_pos.isNull())
        return;
    switch (area_pos.area()->type()) {
        case PI_AREA:
            if (sendDoubleClickEvent())
                makeCommand<EditPi>()->execute(structEditor_);
            break;

        case COMMENT_AREA:
            if (sendDoubleClickEvent())
                makeCommand<EditComment>()->execute(structEditor_);
            break;

        case FOLD_AREA:
            makeCommand<UnfoldElement>()->execute(structEditor_);
            break;

        case TEXT_AREA:
            if (sendDoubleClickEvent())
                setSelection(pos);
            break;

        default:
            sendDoubleClickEvent();
            break;
    }
}

bool EditPolicyImpl::sendTripleClickEvent()
{
    return structEditor_->tripleClick().
        dispatchEvent(0, DynamicEventFactory::And);
}

void EditPolicyImpl::mouseTripleClicked(const AreaPos& pos)
{
    IdleHandler::resetIdleTimer();
    breakText();
    sendCursorParam(pos, true, false);
    const AreaPos& area_pos = editableView_->context().areaPos();
    if (area_pos.isNull())
        return;
    switch (area_pos.area()->type()) {
        case PI_AREA:
        case COMMENT_AREA:
        case FOLD_AREA:
            sendTripleClickEvent();
            break;
        default:
            if (sendTripleClickEvent())
                makeCommand<SelectParent>()->execute(structEditor_);
            break;
    }
}

void EditPolicyImpl::focusInEvent(bool isActiveWindow)
{
    resetEnterPressCount();
    breakText();
    editableView_->startCursor();
    editableView_->showCursor();
    editableView_->setMicroFocusHint();
    serna_clipboard().appFocusEvent(true, structEditor_->stripInfo());
    if (!isActiveWindow)
        return;
    String need_reload = check_document_reload(structEditor_->getDsi());
    if (!need_reload.isEmpty()) {
        if (qApp->activeWindow()) {
            QObjectList qlist =
                qApp->activeWindow()->queryList(NOTR("QDialog"));
            QObjectList::iterator qit = qlist.begin();
            for (; qit != qlist.end(); ++qit)
                if (static_cast<QDialog*>(*qit)->isModal())
                    return;
        }
        if (!sernaDoc()->showMessageBox(
                SernaDoc::MB_WARNING, "",
                tr("Another application has modified "
                   "the following file(s):%1").arg(need_reload),
                tr("&Reload"), tr("&Ignore")))
            sst_delayed_call(reload_document, structEditor_);
    }
}

void EditPolicyImpl::focusOutEvent(bool /*isActiveWindow*/)
{
    resetEnterPressCount();
    breakText();
    editableView_->stopCursor();
    serna_clipboard().appFocusEvent(false, structEditor_->stripInfo());
}

QVariant EditPolicyImpl::inputMethodQuery(Qt::InputMethodQuery property)
{
    breakText();
    if (0 == &*imState_)
        return QVariant();
    return imState_->inputMethodQuery(property);
}

void EditPolicyImpl::imEvent(QInputMethodEvent* e)
{
    if (0 == &*imState_)
        imState_ = new ImState(structEditor_);
    imState_->handleImEvent(e);
}

static void send_cursor_param(StructEditor* structEditor,
                              const AreaPos& areaPos,
                              bool persistent, bool mark)
{
    structEditor->setLastPosNode(0);
    structEditor->selectionHistory().clear();
    if (!mark && !structEditor->editableView().getSelection().src_.isEmpty())
        structEditor->removeSelection();
    if (areaPos.isNull())
        return;
    if (mark)
        structEditor->extendSelectionTo(
            EditContext::getSrcPos(areaPos), areaPos);
    structEditor->editableView().context().lockPersistentPos(!persistent);
    structEditor->setCursor(areaPos);
    structEditor->editableView().context().lockPersistentPos(false);
    structEditor->editableView().grabFocus();
}

static void go_next_hor_pos(StructEditor* structEditor, bool navTable,
                            bool isLeft, bool mark)
{
    AreaPos area_pos = structEditor->editableView().context().areaPos();
    if (navTable && has_ancestor_fo(area_pos, TABLE_FO)) {
        area_pos = next_horizontal_cell_pos(area_pos, isLeft);
        if (!area_pos.isNull())
            send_cursor_param(structEditor, area_pos, true, false);
    }
    else {
        area_pos = structEditor->editableView().context().getShiftedCursor(
            (isLeft) ? -1 : 1);
        send_cursor_param(structEditor, area_pos, true, mark);
    }
}

static void go_next_vert_pos(StructEditor* structEditor, bool navTable,
                             bool isUp, bool mark)
{
    AreaPos area_pos = structEditor->editableView().context().areaPos();
    if (navTable && has_ancestor_fo(area_pos, TABLE_FO))
        area_pos = next_vertical_cell_pos(
            area_pos, structEditor->editableView().context().persistent(),
            isUp);
    else
        area_pos = structEditor->editableView().getUpCursor(isUp);
    send_cursor_param(structEditor, area_pos, false, mark);
}

void EditPolicyImpl::pageUp(bool isUp, bool isToCorner, bool isToSelect)
{
    const bool is_paginated = structEditor_->getDsi()->getSafeProperty(
        DocSrcInfo::SHOW_PAGINATED)->getBool();    
    if (!is_paginated) 
        editableView_->scrollByPage(isUp);
    else
        sendCursorParam(editableView_->getPageCursor(isUp, isToCorner),
            false, isToSelect);
}

void EditPolicyImpl::keyPressed(QKeyEvent* e)
{
    IdleHandler::resetIdleTimer();
    ScopeGuard enter_flag_guard(
        makeObjGuard(*this, &EditPolicyImpl::resetEnterPressCount));
    ScopeGuard break_text_guard(
        makeObjGuard(*this, &EditPolicyImpl::breakText));
    const AreaPos& area_pos = editableView_->context().areaPos();
    if ((e->state() & Qt::MetaButton) || area_pos.isNull()) {
        e->ignore();
        return;
    }
    EventData ed;
    e->accept();
    if (e->state() & Qt::ControlButton) {
        switch (e->key()) {
            case Qt::Key_A:
                sendCursorParam(editableView_->context().getLineCursor(true),
                                true, (e->state() & Qt::ShiftButton));
                return;
            case Qt::Key_E:
                sendCursorParam(editableView_->context().getLineCursor(false),
                                true, (e->state() & Qt::ShiftButton));
                return;
            case Qt::Key_Left:
                sendCursorParam(editableView_->context().getWordCursor(true),
                                true, (e->state() & Qt::ShiftButton));
                return;
            case Qt::Key_Right:
                sendCursorParam(editableView_->context().getWordCursor(false),
                                true, (e->state() & Qt::ShiftButton));
                return;
            case Qt::Key_PageUp: {
                pageUp(true, true, (e->state() & Qt::ShiftButton));
                return;
            } 
            case Qt::Key_PageDown:
                pageUp(false, true, (e->state() & Qt::ShiftButton));
                return;
            case Qt::Key_Up:
                makeCommand<GoToPrevElement>((e->state() & Qt::ShiftButton)
                    ? &ed : 0)->execute(structEditor_);
                return;
            case Qt::Key_Down:
                makeCommand<GoToNextElement>((e->state() & Qt::ShiftButton)
                    ? &ed : 0)->execute(structEditor_);
                return;
            case Qt::Key_Home:
                makeCommand<GoToElementStart>((e->state() & Qt::ShiftButton)
                    ? &ed : 0)->execute(structEditor_);
                return;
            case Qt::Key_End:
                makeCommand<GoToElementEnd>
                    ((e->state() & Qt::ShiftButton) ? &ed : 0);
                return;
            case Qt::Key_Insert:
                makeCommand<StructCopy>()->execute(structEditor_);
                return;
            case ' ':
                if (e->state() & Qt::ShiftButton) {
                    InsertTextEventData ed2(QChar(Char::nbsp), continueText_);
                    makeCommand<InsertText>(&ed2)->execute(structEditor_);
                    break_text_guard.dismiss();
                    continueText_ = true;
                }
                return;
            default:
                if (!(e->state() & Qt::AltButton)) {
                    e->ignore();
                    return;
                }
                break;
        }
    }
    switch (e->key()) {
        case Qt::Key_Tab: {
            go_next_hor_pos(structEditor_, true, false, false);
            return;
        }
        case Qt::Key_BackTab: {
            go_next_hor_pos(structEditor_, true, true, false);
            return;
        }
        case Qt::Key_Left:
            go_next_hor_pos(structEditor_, e->state() & Qt::AltButton, true,
                            (e->state() & Qt::ShiftButton));
            return;
        case Qt::Key_Right:
            go_next_hor_pos(structEditor_, e->state() & Qt::AltButton, false,
                            (e->state() & Qt::ShiftButton));
            return;
        case Qt::Key_Up: {
            go_next_vert_pos(structEditor_, e->state() & Qt::AltButton,
                             true, (e->state() & Qt::ShiftButton));
            return;
        }
        case Qt::Key_Down: {
            go_next_vert_pos(structEditor_, e->state() & Qt::AltButton,
                             false, (e->state() & Qt::ShiftButton));
            return;
        }
        case Qt::Key_PageUp:
            pageUp(true, false, (e->state() & Qt::ShiftButton));
            return;
        case Qt::Key_PageDown:
            pageUp(false, false, (e->state() & Qt::ShiftButton));
            return;
        case Qt::Key_Backspace:
            makeCommand<DoBackspace>()->execute(structEditor_);
            return;
        case Qt::Key_Home:
            sendCursorParam(editableView_->context().getLineCursor(true),
                            false, (e->state() & Qt::ShiftButton));
            return;
        case Qt::Key_End:
            sendCursorParam(editableView_->context().getLineCursor(false),
                            false, (e->state() & Qt::ShiftButton));
            return;
        case Qt::Key_Delete:
            if (e->state() & Qt::ShiftButton)
                makeCommand<StructCut>()->execute(structEditor_);
            else
                makeCommand<DoDelete>()->execute(structEditor_);
            return;
        case Qt::Key_Escape:
            structEditor_->removeSelection();
            return;
        case Qt::Key_Insert:
            if (e->state() & Qt::ShiftButton)
                makeCommand<StructPaste>()->execute(structEditor_);
            return;
        case Qt::Key_Return: {
            if (CHOICE_AREA == area_pos.area()->type()) {
                makeCommand<InsertElement>()->
                    execute(structEditor_);
                return;
            }
            if (area_pos.area()->chain()->isPreserveLinefeed()) {
                InsertTextEventData ed("\n", continueText_);
                makeCommand<InsertText>(&ed)->execute(structEditor_);
                continueText_ = true;
                break_text_guard.dismiss();
                return;
            }
            if (PI_AREA == area_pos.area()->type())
                makeCommand<EditPi>()->execute(structEditor_);
            else if (COMMENT_AREA == area_pos.area()->type())
                makeCommand<EditComment>()->execute(structEditor_);
            else if (FOLD_AREA == area_pos.area()->type())
                makeCommand<UnfoldElement>()->execute(structEditor_);
            else if (COMBO_BOX_AREA == area_pos.area()->type() ||
                     LINE_EDIT_AREA == area_pos.area()->type())
                edit_simple_form_area(structEditor_);
            else {
                enter_flag_guard.dismiss();
                if (0 < enterPressCount_) {
                    if (!doAdvancedSplit())
                        resetEnterPressCount();
                }
                else {
                    GrovePos pos(structEditor_->editViewSrcPos());
                    if (makeCommand<SplitElement>()->execute(
                            structEditor_))
                        enterPressCount_ = split_depth(structEditor_, pos);
                }
            }
            return;
        }
        case Qt::Key_Menu: 
            if (editableView_->getSelection().src_.isEmpty()) 
                sendCursorParam(area_pos, true, false);
            if (area_pos.area()) {
                CRect crect = area_pos.area()->absCursorRect(area_pos.pos());
                structEditor_->showContextMenu(editableView_->mapToGlobal(
                    CPoint(crect.origin_)));
            }
            return;
        default:
            break;
    }
    if (e->text().isEmpty()) {
        e->ignore();
        break_text_guard.dismiss();
        return;
    }
    switch (area_pos.area()->type()) {
        case COMBO_BOX_AREA:
        case LINE_EDIT_AREA:
            edit_simple_form_area(structEditor_, e);
            return;
        default:
            break;
    }
    InsertTextEventData ed2(e->text(), continueText_);
    makeCommand<InsertText>(&ed2)->execute(structEditor_);
    continueText_ = true;
    break_text_guard.dismiss();
}

void EditPolicyImpl::resetEnterPressCount()
{
    if (!enterPressCountLocked_)
        enterPressCount_ = 0;
}

bool EditPolicyImpl::doAdvancedSplit()
{
    GrovePos pos;
    if (!structEditor_->getCheckedPos(pos))
        return false;
    //! Adjust position to first element ancestor
    while (!pos.isNull() && pos.type() != GrovePos::ELEMENT_POS)
        pos = GrovePos(pos.node()->parent(), pos.node());
    for (int c = enterPressCount_; !pos.isNull() && 0 != c; --c)
        pos = GrovePos(pos.node()->parent(), pos.node());
    if (pos.isNull())
        return false;
    GrovePosEventData pos_data(pos);
    if (makeCommand<AdvSplitElement>(&pos_data)->execute(structEditor_)) {
        enterPressCount_ = split_depth(structEditor_, pos);
        return true;
    }
    return false;
}

void EditPolicyImpl::sendCursorParam(const AreaPos& areaPos,
                                     bool persistent, bool mark)
{
    send_cursor_param(structEditor_, areaPos, persistent, mark);
}

static inline bool is_alphanumeric(const Char& ch)
{
    return ('\'' == ch || ch.isLetterOrNumber());
}

static inline bool is_either(const Char& ch, bool searchSpace,
                             bool searchAlphanumeric)
{
    if (!ch.isSpace() && searchAlphanumeric == is_alphanumeric(ch))
        return true;
    if (searchSpace == (' ' == ch))
        return true;
    return false;
}

void EditPolicyImpl::setSelection(const AreaPos& areaPos)
{
    const Chain* chain = areaPos.area()->chain();
    switch (areaPos.area()->type()) {
        case TEXT_AREA :
            {
                const TextFo* text_fo = static_cast<const TextFo*>(chain);
                String text = text_fo->strippedText();
                ChainPos chain_pos(areaPos);
                bool search_space = !(' ' == text[chain_pos.pos()]);
                bool search_alphanumeric =
                    !is_alphanumeric(text[chain_pos.pos()]);

                int from = chain_pos.pos();
                for (; from > 0; --from) {
                    if (is_either(text[from], search_space,
                                  search_alphanumeric)) {
                        ++from;
                        break;
                    }
                }
                int to = chain_pos.pos();
                for (; (uint)to < text.length(); ++to) {
                    if (is_either(text[to], search_space,
                                  search_alphanumeric))
                        break;
                }
                if (from != to) {
                    ChainPos start(chain, from);
                    ChainPos end(chain, to);
                    structEditor_->setSelection(
                        ChainSelection(start, end),
                        GroveSelection(EditContext::getSrcPos(
                                           start.toAreaPos()),
                                       EditContext::getSrcPos(
                                           end.toAreaPos())));
                    serna_clipboard().setText(true,
                        text.mid(from, to - from));
                }
            }
            break;
        default:
            {
                const Chain* parent = chain->parentChain();
                if (!parent)
                    return;
                ChainPos start(parent, chain->chainPos());
                ChainPos end(parent, chain->chainPos() + 1);
                structEditor_->setSelection(
                    ChainSelection(start, end),
                    GroveSelection(EditContext::getSrcPos(start.toAreaPos()),
                                   EditContext::getSrcPos(end.toAreaPos())));
            }
            break;
    }
}

void EditPolicyImpl::setEditableView(EditableView* editableView)
{
    editableView_ = editableView;
}

class SetZoom;

void EditPolicyImpl::resizeEvent(QResizeEvent*)
{
    makeCommand<SetZoom>()->execute(structEditor_);
}
#if defined(QT3INPUTMETHOD)
void EditPolicyImpl::finishIM()
{
    DBG(SE.IM) << "FinishIM: IMS=" << !imState_.isNull()
        << ", mustFinish=" << (imState_ ? imState_->mustFinish() : 0)
        << std::endl;
//    if (imState_ && imState_->mustFinish())
//        imState_ = 0;
}
#endif
static QString entity_tip(const GroveLib::EntityReferenceStart* ers)
{
    QString tip;

    const GroveLib::EntityDecl* decl = ers->entityDecl();

    switch (decl->declType()) {
        case GroveLib::EntityDecl::internalGeneralEntity: {
            tip = QCoreApplication::translate(
                "SpecialNodeTooltip", 
                "Internal Entity: <b>name</b>='%0'").arg(decl->name());
            break;
        }
        case GroveLib::EntityDecl::externalGeneralEntity: {
            const GroveLib::ExternalEntityDecl* external_decl =
                static_cast<const GroveLib::ExternalEntityDecl*>(decl);
            tip = QCoreApplication::translate(
                "SpecialNodeTooltip", "External Entity<hr/><b>name</b>='%0'"
                "<br/><b>sysid</b>='%1'")
                .arg(decl->name()).arg(external_decl->sysid());
            break;
        }
        case GroveLib::EntityDecl::xinclude: {
            const GroveLib::XincludeDecl* xinclude =
                static_cast<const XincludeDecl*>(decl);
            tip = QCoreApplication::translate(
                "SpecialNodeTooltip", "XInclude<hr/><b>href</b>='%0'")
                .arg(xinclude->url());

            if (!xinclude->expr().isEmpty())
                tip += QCoreApplication::translate(
                    "SpecialNodeTooltip", "<br/><b>xpointer</b>='%0'")
                    .arg(xinclude->expr());
            if (xinclude->isFallback())
                tip += QString(NOTR(
                    "<br/><font color='#990000'>%0</font>")).
                    arg(QCoreApplication::translate(
                            "SpecialNodeTooltip", "This is fallback"));
            break;
        }
        default:
            break;
    }
    if (!tip.isEmpty())
        tip = NOTR("<qt>") + tip + NOTR("</qt>");
    return tip;
}

STRUCTEDITOR_EXPIMP QString special_node_tooltip(const GroveLib::Node* node)
{
    switch (node->nodeType()) {
        case GroveLib::Node::COMMENT_NODE: {
            return QCoreApplication::translate(
                "SpecialNodeTooltip", "<qt>Comment<hr/>%0</qt>")
                //QString(NOTR("<font color='green'>&lt;--</font>"
                //"%1 <font color='green'>-->"))
                .arg(Qt::escape(CONST_COMMENT_CAST(node)->comment()));
        }
        case GroveLib::Node::PI_NODE: {
            const GroveLib::ProcessingInstruction* pi =
                CONST_PI_CAST(node);
            return QCoreApplication::translate(
                "SpecialNodeTooltip",
                "<qt>Processing Instruction<hr/><b>target</b>='%0'<br/>"
                "<b>data</b>='%1'</qt>")
                .arg(pi->target())
                .arg(Qt::escape(pi->data()));

            //return QString(NOTR(
            //    "<nobr><font color='green'>&lt;?</font><font color='blue'>"
            //    "%1</font> %2<font color='green'>?&gt;</font></nobr>"))
            //    .arg(pi->target())
            //    .arg(Qt::escape(pi->data()));
        }
        case GroveLib::Node::ENTITY_REF_END_NODE: {
            const GroveLib::EntityReferenceStart* ers =
                CONST_ERS_CAST(CONST_ERE_CAST(node)->getSectStart());
            if (ers->entityDecl()->declType() == EntityDecl::xinclude) {
                const GroveLib::XincludeDecl* xd = static_cast
                    <const GroveLib::XincludeDecl*>(ers->entityDecl());
                return QCoreApplication::translate(
                    "SpecialNodeTooltip", "end of XInclude:</i> <b>%0</b>").
                    arg(xd->url());
            }
            else
                return QCoreApplication::translate(
                    "SpecialNodeTooltip", "end of entity:</i> <b>%0</b>").
                    arg(ers->entityDecl()->name());
        }
        case GroveLib::Node::ENTITY_REF_START_NODE:
            return entity_tip(CONST_ERS_CAST(node));
        default:
            return QString();
    }
}

void EditPolicyImpl::tooltipEvent(const ToolTipEventData& evd)
{
    const QRect& r = evd.rect();
    DBG_IF(SE.TOOLTIP) {
        DBG(SE.TOOLTIP) << "TooltipEvent: rect=(" << r.x() << "," << r.y()
            << "," << r.width() << "," << r.height() << ") foNodeName="
            << evd.foPos().node()->nodeName()
            << ", widget=" << evd.widget() << std::endl;
    }
    if (structEditor_->maybeTooltip().
        dispatchEvent(&evd, DynamicEventFactory::Or))
            return;
    const GroveLib::Node* node = Xslt::resultOrigin(evd.foPos().node());
    if (0 == node)
        return;
    DBG_IF(SE.TOOLTIP) node->dump();
    QString tip(special_node_tooltip(node));
    if (tip.isEmpty())
        QToolTip::hideText();
    else
        QToolTip::showText(evd.widget()->mapToGlobal(QPoint(r.x(), r.y())),
            tip, evd.widget(), r);
}

SernaDoc* EditPolicyImpl::sernaDoc() const
{
    return structEditor_->sernaDoc();
}

EditPolicyImpl::~EditPolicyImpl()
{
}

