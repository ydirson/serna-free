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
#include "structeditor/impl/InputMethod.h"
#include "structeditor/impl/debug_se.h"
#include "structeditor/InsertTextEventData.h"

#include "docview/SernaDoc.h"
#include "editableview/EditableView.h"
#include "editableview/EditPolicy.h"
#include "sceneareaset/utils.h"

#include "groveeditor/GrovePos.h"
#include "common/CommandExecutor.h"
#include "common/ScopeGuard.h"

#include <QInputMethodEvent>
#include <QInputContext>
#include <QTextCharFormat>

using namespace Common;
using namespace GroveEditor;

class InsertText;

#if !defined(_NDEBUG)
void show_qim(StructEditor* se, QInputMethodEvent* e, const char* msg)
{
// START_IGNORE_LITERALS
    QString s = QString("%0: replStart=%1, replLen=%2, preeditLen=%3, "
                        "preeditText=<%4>, commitLen<%5>, commitText=<%6>").
        arg(msg).arg(e->replacementStart()).arg(e->replacementLength()).
        arg(e->preeditString().length()).arg(e->preeditString()).
        arg(e->commitString().size()).arg(e->commitString());

    std::cerr << String(s) << std::endl;

    const QList<QInputMethodEvent::Attribute>& attrList(e->attributes());
    QList<QInputMethodEvent::Attribute>::const_iterator it = attrList.begin();
    for (; attrList.end() != it; ++it) {
        std::cerr << "Type = " << it->type << ", start = " << it->start
                  << ", length = " << it->length << ", value = "
                  << String(it->value.toString()) << std::endl;
    }

    se->sernaDoc()->showContextInfo(s);
// STOP_IGNORE_LITERALS
}
#else
static void show_qim(StructEditor*, QInputMethodEvent*, const char*)
{
}
#endif
#if defined(QT3INPUTMETHOD)
ImState::ImState(StructEditor* se, QInputMethodEvent* e)
 :  se_(se), undoCmd_(0), imPos_(-1),
    mustFinish_(false), finished_(false), firstCompose_(true)
{
    DBG(SE.IM) << "ImState cons: " << this << "\n";
    DBG_IF(SE.IM) show_qim(se_, e, "imStart");
}
#endif
ImState::ImState(StructEditor* se)
    : se_(se), undoCmd_(0), imPos_(-1),
      mustFinish_(false) //, finished_(false), firstCompose_(true)
{
    DBG(SE.IM) << "ImState cons: " << this << std::endl;
}

static QRectF get_cursor_rectf(StructEditor* se)
{
    using namespace Formatter;
    const AreaPos& pos = se->editableView().context().areaPos();
    if (0 != &pos && pos.area())
        return qRect(pos.area()->absCursorRect(pos.pos()));
    return QRectF();
}

static int get_cursor_pos(QInputMethodEvent* e)
{
    for (int i = 0; i < e->attributes().size(); ++i) {
        const QInputMethodEvent::Attribute &a = e->attributes().at(i);
        if (a.type == QInputMethodEvent::Cursor)
            return a.length ? a.start : -1;
    }
    return -1;
}

static bool get_selection(QInputMethodEvent* e, int& start, int& length)
{
    for (int i = 0; i < e->attributes().size(); ++i) {
        const QInputMethodEvent::Attribute& attr = e->attributes().at(i);
        if (QInputMethodEvent::TextFormat == attr.type) {
            QTextCharFormat fmt;
            fmt = qvariant_cast<QTextFormat>(attr.value).toCharFormat();
            if (fmt.isValid()) {
//            if (fmt.fontUnderline()) {
                start = attr.start;
                length = attr.length;
                return true;
            }
        }
    }
    return false;
}

void ImState::handleImEvent(QInputMethodEvent* e)
{
    DBG_IF(SE.IM) show_qim(se_, e, "ImState::handleImEvent");
    DBG(SE.IM) << "mustFinish_ = '" << mustFinish_ << '\'' << std::endl;

    if (mustFinish_) {
        mustFinish_ = false;
        return e->accept();
    }

    GrovePos pos;
    if (!se_->getCheckedPos(pos, se_->TEXT_OP))
        return e->ignore();

    DBG(SE.IM) << "imString_ = '" << imString_ << '\'' << std::endl;
    EditableView& ev(se_->editableView());
    QString commitString(e->commitString());
    if (!commitString.isEmpty()) {
        if (imString_.isNull()) { // no commit pending
            InsertTextEventData ed(commitString);
            makeCommand<InsertText>(&ed)->execute(se_);
        }
        else {
            // there were earlier preedit strings => we must commit
            if (commitString != imString_)
                replaceText(commitString);
            undoCmd_ = 0;
            imString_ = QString::null;
        }
        mustFinish_ = true;
        ev.resetIM();
        mustFinish_ = false;
        se_->removeSelection();
        se_->showCursorInfo();
        if (microFocus_.isValid())
            microFocus_.clear();

        return e->accept();
    }
    // commit string is empty => handle preedit string

    DBG(SE.IM) << "preedit phase" << std::endl;
    if (!microFocus_.isValid())
        microFocus_ = get_cursor_rectf(se_);
    QString preeditString(e->preeditString());
    if (imString_ != preeditString) {
        if (!replaceText(preeditString))
            return e->ignore();
        // cursor at the end of preedit string
        if (undoCmd_)
            imPos_ = preeditString.length();
        else
            imPos_ = imString_.length();
    }

    if (!imString_.isEmpty()) {
        const int setPos = get_cursor_pos(e);
        DBG(SE.IM) << "handleImEvent, cursor pos = " << setPos
                   << ", imPos_ = " << imPos_ << std::endl;
        if (-1 != setPos) {
            if (setPos != imPos_) {
                se_->setCursor(se_->editableView().context().
                               getShiftedCursor(setPos - imPos_));
                imPos_ = setPos;
            }
            se_->removeSelection();
        }
        else {
            using namespace Formatter;

            int start = -1, length = -1;
            bool isSelected = get_selection(e, start, length);
            DBG(SE.IM) << "handleImEvent, selection range = (" << start
                       << ',' << length << ')' << std::endl;
            if (isSelected) {
                AreaPos savePos = ev.context().areaPos();
                AreaPos endPos = ev.context().getShiftedCursor(length-imPos_);
                se_->setCursor(endPos);
                AreaPos beginPos = ev.context().getShiftedCursor(-length);
                se_->extendSelectionTo(EditContext::getSrcPos(beginPos),
                                       beginPos);
                se_->setCursor(savePos);
                ev.hideCursor();
            }
        }
    }
    return e->accept();
}

bool ImState::replaceText(const QString& new_text)
{
    DBG(SE.IM) << "ImState::replaceText(" << new_text << ')' << std::endl;
    if (new_text != imString_) {
        if (undoCmd_) {
            if (se_->executor()->lastDone() != undoCmd_)
                return false;
            se_->doUntil(-1, !new_text.isEmpty());
            se_->editableView().setMicroFocusHint();
            DBG(SE.IM) << "Undoing last cmd\n";
            undoCmd_ = 0;
        }
        if (new_text.length()) {
#if defined(QT3INPUTMETHOD)
            if (firstCompose_)
                firstCompose_ = false;
            else
                se_->removeSelection();
#endif
            InsertTextEventData ed2(new_text);
            if (makeCommand<InsertText>(&ed2)->execute(se_))
                undoCmd_ = se_->executor()->lastDone();
            else
                undoCmd_ = 0;
            DBG(SE.IM) << "Inserted text, undoCmd=" << undoCmd_ << "\n";
            se_->editableView().setMicroFocusHint();
            if (undoCmd_)
                imString_ = new_text;
            else
                imString_ = QString::null;
        } else {
            undoCmd_ = 0;
            imString_ = QString::null;
        }
    }
    return true;
}
#if defined(QT3INPUTMETHOD)
bool ImState::compose(QInputMethodEvent* e)
{
    DBG_IF(SE.IM) show_qim(se_, e, "imCompose");

    mustFinish_ = false;
    QString composeString(e->preeditString());
    const int pos = e->replacementStart();
    if (composeString != imString_) {
        if (!replaceText(composeString))
            return false;
        if (undoCmd_)
            imPos_ = (pos < 0) ? -1 : composeString.length();
        else
            imPos_ = -1;
    }
    if (imPos_ < 0) {
        mustFinish_ = true;
        return true;
    }
    if (pos != imPos_) {
        se_->setCursor(se_->editableView().context().
                       getShiftedCursor(pos - imPos_));
        imPos_ = pos;
    }
    if (const int length = e->replacementLength()) {
        Formatter::AreaPos old_pos = se_->editableView().context().areaPos();
        Formatter::AreaPos apos = se_->editableView().context().
            getShiftedCursor(length);
        se_->extendSelectionTo(EditContext::getSrcPos(apos), apos);
        se_->setCursor(old_pos);
    }
    se_->editableView().setMicroFocusHint();
    mustFinish_ = true;
    return true;
}

void ImState::finish(QInputMethodEvent* e)
{
    if (finished_)
        return;
    DBG_IF(SE.IM) show_qim(se_, e, "imEnd");
    mustFinish_ = false;
    const QString text(e->commitString());
    if (text != imString_)
        replaceText(text);
    se_->removeSelection();
    finished_ = true;
}
#endif

QVariant ImState::inputMethodQuery(Qt::InputMethodQuery property)
{
    switch(property) {
        case Qt::ImMicroFocus: {
            if (microFocus_.type() == QVariant::RectF)
                return microFocus_;
            return get_cursor_rectf(se_);
        }
        case Qt::ImFont: {
            using namespace Formatter;
            const AreaPos& pos = se_->editableView().context().areaPos();
            if (0 != &pos && pos.area())
                if (const Chain* chain = pos.area()->chain())
                    if (!chain->font().isNull())
                        return QVariant(chain->font()->qfont());
            break;
        }
        case Qt::ImCursorPosition:
            break;
        case Qt::ImSurroundingText:
            break;
        case Qt::ImCurrentSelection:
            break;
        default:
            break;
    }
    return QVariant();
}

ImState::~ImState()
{
    DBG(SE.IM) << "ImState Deleted:" << this << "\n";
#if defined(QT3INPUTMETHOD)
    mustFinish_ = false;
    se_->showCursorInfo();
    if (finished_)
        return;
    se_->removeSelection();
    se_->editableView().resetIM();
#endif
}

