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
#include "docview/dv_defs.h"
#include "docview/Clipboard.h"
#include "docview/qt/ClipboardImpl.h"
#include "docview/impl/debug_dv.h"
#include "common/Singleton.h"
#include "grove/Nodes.h"

#include <qapplication.h>
#include <qobject.h>
#include <qclipboard.h>
#include <iostream>

static const char* SERNA_CLIPBOARD = NOTR("SernaClipboard");

using namespace Common;

DOCVIEW_EXPIMP Clipboard& serna_clipboard()
{
    return SingletonHolder<ClipboardImpl, CreateUsingNew<ClipboardImpl>,
        NoDestroy<ClipboardImpl> >::instance();
}

ClipboardImpl::ClipboardImpl()
    : QObject(qApp, SERNA_CLIPBOARD),
      keyboard_(new GroveLib::DocumentFragment),      
      mouse_(new GroveLib::DocumentFragment),      
      dataChanged_(true),
      selectionChanged_(true),
      needFocusUpdate_(false),
#if defined(__linux__) || defined(__sun__)
      initPhase_(true)
#else // __linux__ || __sun__
      initPhase_(false)
#endif // __linux__ || __sun__
{
    connect(qApp->clipboard(), SIGNAL(dataChanged()),
            this, SLOT(dataChanged()));
    if (qApp->clipboard()->supportsSelection())
        connect(qApp->clipboard(), SIGNAL(selectionChanged()),
                this, SLOT(selectionChanged()));
    DBG(DV.CB) << "Clipboard: suppsel=" << supportsSelection() << std::endl;
    keyboard_->setUdata(1);
    mouse_->setUdata(1);
    appFocusEvent(true, 0);
}

ClipboardImpl::~ClipboardImpl()
{
}

const GroveLib::DocumentFragment* 
ClipboardImpl::getStructClipboard(bool isMouse)
{
    return buffer(isMouse);
}

void ClipboardImpl::setStructClipboard(bool isMouse,
                                       GroveLib::DocumentFragment* frag)
{
    DBG(DV.CB) << "Clipboard: set, isMouse=" << isMouse << std::endl;
    needFocusUpdate_ = true;
    initPhase_ = false;
    (isMouse ? mouse_ : keyboard_ ) = frag;
    frag->setUdata(0);
    clipboardChange().dispatchEvent();
}

static String get_str(const GroveLib::DocumentFragment* df)
{
    if (df && df->firstChild() && df->udata() &&
        df->firstChild()->nodeType() == GroveLib::Node::TEXT_NODE &&
        df->firstChild() == df->lastChild())
            return CONST_TEXT_CAST(df->firstChild())->data();
    return String(""); // empty, non-null string!
}

String ClipboardImpl::getText(bool isMouse,
                              const GroveLib::StripInfo* stripInfo)
{
    DBG(DV.CB) << "Clipboard: getText, isMouse=" << isMouse << std::endl;
    String result;
    GroveLib::DocumentFragment* frag = buffer(isMouse);
    if (frag->udata()) 
        return get_str(frag);
    frag->saveAsXmlString(result, GroveLib::Grove::GS_SAVE_CONTENT|
        (stripInfo ? GroveLib::Grove::GS_INDENT : 0), stripInfo);
    return result;
}

bool ClipboardImpl::set_text(bool isMouse, const QString& qstr)
{
    GroveLib::DocumentFragment* df = buffer(isMouse);
    String str;
    str.reserve(qstr.length());
    const Char* cp = qstr.unicode();
    const Char* ce = cp + qstr.length();
    for (; cp < ce; ++cp) 
        if (cp->isPrint() || cp->unicode() == '\n')
            str.append(*cp);
    if (df->udata() && get_str(df) == str)
        return false;
    needFocusUpdate_ = true;
    df->setUdata(1);
    df->removeAllChildren();
    df->appendChild(new GroveLib::Text(str));
    DBG(DV.CB) << "Clipboard: Text Set, isMouse=" << isMouse << ", text="
        << str << std::endl;
    return true;
}

void ClipboardImpl::setText(bool isMouse, const String& text)
{
    if (set_text(isMouse, text)) {
        initPhase_ = false;
        clipboardChange().dispatchEvent();
    }
}

void ClipboardImpl::appFocusEvent(bool focusIn, const GroveLib::StripInfo* si)
{
    QClipboard* cb = QApplication::clipboard();
    if (focusIn) {
        DBG(DV.CB) << "Clipboard: focusIn, selChanged="
            << selectionChanged_ << ", dataChanged="
            << dataChanged_ << ", initPhase=" << initPhase_ << std::endl;
        bool must_notify = false;
        if (selectionChanged_) {
            QString sel_text = cb->text(QClipboard::Selection);
            if (sel_text != mouseText_ && set_text(true, sel_text))
                must_notify = true;
            selectionChanged_ = false;
        }
        if (initPhase_ || dataChanged_) {
            QString cb_text(cb->text(QClipboard::Clipboard)); 
            if (cb_text != keyboardText_ && set_text(false, cb_text)) 
                must_notify = true;
            if (initPhase_ && !cb_text.isEmpty())
                initPhase_ = false;
            dataChanged_ = false;
        }
        keyboardText_ = mouseText_ = QString::null;
        if (must_notify)
            clipboardChange().dispatchEvent();
    } else { // focusOut
        DBG(DV.CB) << "Clipboard: focusOut, needUpdate="
            << needFocusUpdate_ << "\n";
        if (needFocusUpdate_) {
            keyboardText_ = getText(false, si);
            cb->setText(keyboardText_, QClipboard::Clipboard);
            if (supportsSelection()) {
                mouseText_ = getText(true, si);
                cb->setText(mouseText_, QClipboard::Selection);
            }
            needFocusUpdate_ = false;
        }
    }
}

DynamicEventFactory& ClipboardImpl::clipboardChange()
{
    return clipboardChangeFactory_;
}

bool ClipboardImpl::supportsSelection() const
{
    return QApplication::clipboard()->supportsSelection();
}
