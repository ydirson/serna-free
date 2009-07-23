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
#ifndef STRUCTEDITOR_EDITPOLICY_IMPL_H_
#define STRUCTEDITOR_EDITPOLICY_IMPL_H_

#include <QWidget>
#include <QResizeEvent>
#include <QEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QDragMoveEvent>
#include <QKeyEvent>
#include <Qt>

#include "structeditor/se_defs.h"
#include "editableview/EditableView.h"
#include "common/OwnerPtr.h"

class StructEditor;
class ImState;
class ToolTipEventData;
class QVariant;

void edit_simple_form_area(StructEditor* structEditor, QEvent* e = 0);

/*
 */
class STRUCTEDITOR_EXPIMP EditPolicyImpl : public EditPolicy {
public:
    EditPolicyImpl(StructEditor*);
    virtual ~EditPolicyImpl();

    void        setEditableView(EditableView* editableView);
    void        lockEnterPressCount(bool isLock);
    void        resetEnterPressCount();
    void        breakText() { continueText_ = false; }
    void        finishIM();
    bool        isComposing() const { return imState_; }

    void        focusInEvent(bool);
    void        enterLeaveEvent(bool) { breakText(); }

protected:
    //!
    void        setSelection(const Formatter::AreaPos& pos);
    //!
    void        sendCursorParam(const Formatter::AreaPos& pos,
                                bool persistent, bool mark);
private:
    //!
    void        mousePressed(const Formatter::AreaPos& pos, QMouseEvent* e);
    //!
    void        mouseReleased(QMouseEvent*);
    //!
    void        mouseDoubleClicked(const Formatter::AreaPos& pos);
    //!
    void        mouseTripleClicked(const Formatter::AreaPos& pos);
    //!
    void        mouseMoved(const Formatter::AreaPos& pos, QMouseEvent* e);
    //!
    void        contentsDragEnterEvent(QDragEnterEvent* event,
                                       const Formatter::AreaPos& pos);
    void        contentsDragMoveEvent(QDragMoveEvent* e,
                                      const Formatter::AreaPos& pos);
    void        contentsDragLeaveEvent(QDragLeaveEvent* e);
    void        contentsDropEvent(QDropEvent* event,
                                  const Formatter::AreaPos& pos);
    void        tooltipEvent(const ToolTipEventData&);

    void        startDrag();

    void        keyPressed(QKeyEvent*);
    void        keyReleased(QKeyEvent*) {};
    void        resizeEvent(QResizeEvent*);
    void        focusOutEvent(bool);

    void        imEvent(QInputMethodEvent*);
    QVariant    inputMethodQuery(Qt::InputMethodQuery property);
    void        pageUp(bool isUp, bool isToCorner, bool isToSelect);

    bool        sendDoubleClickEvent();
    bool        sendTripleClickEvent();

    bool        doAdvancedSplit();

    SernaDoc*   sernaDoc() const;

private:
    EditPolicyImpl(const EditPolicyImpl&);
    EditPolicyImpl& operator=(const EditPolicyImpl&);


    bool            dblClicked_;
    bool            enterPressCountLocked_;
    bool            mousePressed_;
    bool            continueText_;
    int             enterPressCount_;
    EditableView*   editableView_;
    StructEditor*   structEditor_;
    QPoint          mousePressPos_;
    Common::OwnerPtr<ImState> imState_;
};

#endif // STRUCTEDITOR_EDITPOLICY_IMPL_H_
