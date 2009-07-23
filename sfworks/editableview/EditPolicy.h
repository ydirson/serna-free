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
#ifndef EDIT_POLICY_H
#define EDIT_POLICY_H

#include "formatter/AreaPos.h"
#include "groveeditor/GrovePos.h"
#include <Qt>

class QMouseEvent;
class QFocusEvent;
class QKeyEvent;
class QResizeEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class QInputMethodEvent;
class ToolTipEventData;
class QVariant;

class EditPolicy {
public:
    virtual ~EditPolicy() {}

    //!
    virtual void    mousePressed(const Formatter::AreaPos& pos,
                                 QMouseEvent* e) = 0;
    //!
    virtual void    mouseDoubleClicked(const Formatter::AreaPos& pos) = 0;
    //!
    virtual void    mouseTripleClicked(const Formatter::AreaPos& pos) = 0;
    //!
    virtual void    mouseReleased(QMouseEvent* e) = 0;
    //!
    virtual void    mouseMoved(const Formatter::AreaPos& pos,
                               QMouseEvent* e) = 0;
    //!
    virtual void    focusInEvent(bool activeWin) = 0;
    //!
    virtual void    focusOutEvent(bool activeWin) = 0;
    virtual void    enterLeaveEvent(bool isEnter) = 0;
    //!
    virtual void    keyPressed(QKeyEvent*) = 0;
    //!
    virtual void    keyReleased(QKeyEvent*) = 0;
    //!
    virtual void    resetEnterPressCount() = 0;
    //!
    virtual void    resizeEvent(QResizeEvent*) = 0;

    virtual void    imEvent(QInputMethodEvent*) = 0;
    //!
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery property) = 0;

    virtual void    contentsDragEnterEvent(QDragEnterEvent* event,
                                           const Formatter::AreaPos& pos) = 0;
    virtual void    contentsDragMoveEvent(QDragMoveEvent* e,
                                          const Formatter::AreaPos& pos) = 0;
    virtual void    contentsDragLeaveEvent(QDragLeaveEvent* e) = 0;
    virtual void    contentsDropEvent(QDropEvent* event,
                                      const Formatter::AreaPos& pos) = 0;
    virtual void    tooltipEvent(const ToolTipEventData& evd) = 0;
};

#endif  // EDIT_POLICY_H
