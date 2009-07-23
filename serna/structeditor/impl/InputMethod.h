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
#ifndef STRUCTEDITOR_INPUT_METHOD_H_
#define STRUCTEDITOR_INPUT_METHOD_H_

#include "structeditor/se_defs.h"
#include "common/Command.h"
#include <QString>
#include <QVariant>
#include <Qt>

class QInputMethodEvent;
class QVariant;
class StructEditor;

class ImState {
public:
//    ImState(StructEditor* se, QInputMethodEvent*);
    ImState(StructEditor* se);
    ~ImState();

    void        handleImEvent(QInputMethodEvent*);
    // returns true if composing is OK, false if it must be aborted
//    bool    compose(QInputMethodEvent*);
    bool        replaceText(const QString&);
    QVariant    inputMethodQuery(Qt::InputMethodQuery property);

    // called on IMEnd event
//    void    finish(QInputMethodEvent*);

    // tells whether pending finishIM() must be done
//    bool    mustFinish() const { return mustFinish_; }

private:
    ImState(const ImState&);
    ImState& operator=(const ImState&);

    StructEditor*           se_;
    const Common::Command*  undoCmd_;
    QString                 imString_;
    int                     imPos_;
    bool                    mustFinish_;
    QVariant                microFocus_;
//    bool                    finished_;
//    bool                    firstCompose_;
};

#endif // STRUCTEDITOR_INPUT_METHOD_H_
