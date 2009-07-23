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
#include "common/String.h"
#include "common/CommandEvent.h"
#include <qobject.h>

class ClipboardImpl : public QObject, public Clipboard {
    Q_OBJECT
public slots:
    void    dataChanged()      { dataChanged_ = true; }
    void    selectionChanged() { selectionChanged_ = true;  }

public:
    virtual Common::String  getText(bool isMouse,
                                    const GroveLib::StripInfo* stripInfo);
    virtual void            setText(bool isMouse, const Common::String&);

    virtual const GroveLib::DocumentFragment* getStructClipboard(bool isMouse);
    virtual void            setStructClipboard(bool isMouse,
                                    GroveLib::DocumentFragment* frag);

    virtual void    appFocusEvent(bool focusIn, const GroveLib::StripInfo*);
    virtual bool    supportsSelection() const;

    virtual Common::DynamicEventFactory& clipboardChange();

    ClipboardImpl();
    virtual ~ClipboardImpl();

private:
    bool    set_text(bool isMouse, const QString& qstr);
    GroveLib::DocumentFragment* buffer(bool isMouse) const 
        { return isMouse ? mouse_.pointer() : keyboard_.pointer(); }

    Common::RefCntPtr<GroveLib::DocumentFragment> keyboard_;
    Common::RefCntPtr<GroveLib::DocumentFragment> mouse_;

    Common::DynamicEventFactory clipboardChangeFactory_;

    QString keyboardText_;
    QString mouseText_;
    bool    dataChanged_;
    bool    selectionChanged_;
    bool    needFocusUpdate_;
    bool    initPhase_;
};
