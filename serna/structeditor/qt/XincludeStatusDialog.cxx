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

#include "grove/Grove.h"
#include "grove/SectionSyncher.h"
#include "grove/xinclude.h"
#include "grove/EntityDecl.h"
#include "grove/SectionNodes.h"
#include "groveeditor/GrovePos.h"

#include "utils/MessageListProperty.h"
#include "utils/HelpAssistant.h"
#include "docutils/doctags.h"

#include "common/MessageUtils.h"
#include "common/CommandEvent.h"

#include <QLabel>
#include <QListWidget>
#include <QShortcut>

#include "structeditor/XincludeStatusDialogBase.hpp"

using namespace Common;
using namespace GroveLib;

class XincludeStatusDialogImpl : public QDialog, 
                                 protected Ui::XincludeStatusDialogBase {
    Q_OBJECT
public:
    XincludeStatusDialogImpl(StructEditor* se);

public slots:
    virtual void    on_helpButton__clicked() {
        helpAssistant().show(DOCTAG(XINCLUDE_STATUS));
    }

private:
    StructEditor*   se_;
};

XincludeStatusDialogImpl::XincludeStatusDialogImpl(StructEditor* se)
    : QDialog(se->widget()), 
      se_(se)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), helpButton_),
            SIGNAL(activated()), this, SLOT(on_helpButton__clicked()));

    GroveEditor::GrovePos pos = se->editViewSrcPos();
    if (pos.isNull())
        return;
    const EntityReferenceStart* ers = pos.getErs();
    for (; ers && ers->getSectParent();
        ers = CONST_ERS_CAST(ers->getSectParent()))
            if (ers->entityDecl()->declType() == EntityDecl::xinclude)
                    break;
    if (!ers || !ers->getSectParent()) {
        messageBox_->hide();
        setFixedSize(minimumSize());
        return;
    }
    XincludeDecl* xd = static_cast<XincludeDecl*>(ers->entityDecl());
    if (xd->isFallback()) {
        if (ers->nextSibling() != ers->getSectEnd())
            statusLabel_->setText(tr("<b>Fallback</b>"));
        else
            statusLabel_->setText(tr("<b>Resource Error</b>"));
    } 
    else
        statusLabel_->setText(tr("<b>OK</b>"));
    if (!xd->url().isEmpty())
        hrefLabel_->setText(NOTR("<b>") + xd->url() + NOTR("</b>"));
    if (!xd->expr().isEmpty())
        xpointerLabel_->setText(xd->expr());
    if (xd->referenceErs() &&
        xd->referenceErs()->grove()->topDecl()->dataType() ==
            EntityDecl::cdata) {
                xpointerAnnotLabel_->hide();
                parseLabel_->setText(tr("<b>text</b>"));
    } 
    else
        parseLabel_->setText(tr("<b>xml</b>"));
    if (!xd->messageList().size()) {
        messageBox_->addItem(tr("** No error messages **"));
        return;
    }
    for (uint i = 0; i < xd->messageList().size(); ++i) {
        const Message* msg = xd->messageList()[i].pointer();
        messageBox_->addItem(
            msg->format(BuiltinMessageFetcher::instance()));
    }
}

////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(XincludeStatusDialog, StructEditor)

bool XincludeStatusDialog::doExecute(StructEditor* se, EventData*)
{
    if (se->editViewSrcPos().isNull())
        return false;
    XincludeStatusDialogImpl dialog(se);
    return dialog.exec() == QDialog::Accepted;
}

#include "moc/XincludeStatusDialog.moc"
