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
#include "common/String.h"

#include "docview/SernaDoc.h"
#include "docview/qt/QtSyntaxHighlighter.h"

#include "utils/NameChecker.h"
#include "utils/HelpAssistant.h"
#include "common/PropertyTreeEventData.h"

#include "docutils/doctags.h"

#include <QApplication>
#include <QShortcut>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>

#include "structeditor/StructEditor.h"
#include "structeditor/EditCommentPIDialogBase.hpp"

using namespace Common;

////////////////////////////////////////////////////////////////////////

class EditCommentPIDialog : public QDialog,
                            protected Ui::EditCommentPIDialogBase {
    Q_OBJECT
public:
    EditCommentPIDialog(StructEditor* se, PropertyNode* properties);
    
    virtual bool    isValid() const { return true; }
    PropertyNode*   properties() const { return props_.pointer(); }
    void            hideTarget()
    {
        targetLabel_->hide();
        targetEdit_->hide();
        dataLabel_->setText(tr("Content:"));
    }
    bool            execute();
    
protected slots:
    virtual void    on_okButton__clicked();
    virtual void    on_helpButton__clicked() = 0;

protected:
    StructEditor*   se_;
    PropertyNodePtr props_;
};

////////////////////////////////////////////////////////////////////////

class EditCommentDialog : public EditCommentPIDialog {
public:
    EditCommentDialog(StructEditor* se, PropertyNode* props)
        : EditCommentPIDialog(se, props)
    {
        hideTarget();
    }

    virtual bool    isValid() const
    {
        if (dataEdit_->text().contains("--") ||
            QString('-') == dataEdit_->text().simplifyWhiteSpace().right(1)) {
            se_->sernaDoc()->showMessageBox(
                SernaDoc::MB_WARNING, tr("Invalid COMMENT contents"),
                tr("Invalid comment contents: must not contain\n"
                   "\"--\" substring or end with \"-\" character."), 
                tr("&Ok"));
            return false;
        }
        return true;
    }

    virtual void    on_helpButton__clicked() 
    {
        helpAssistant().show(DOCTAG(UG_COMM_D));
    }
};

////////////////////////////////////////////////////////////////////////

class EditPiDialog : public EditCommentPIDialog {
public:
    EditPiDialog(StructEditor* se, PropertyNode* props)
        : EditCommentPIDialog(se, props)
    {
        targetEdit_->setText(props_->getSafeProperty("target")->getString());
    }

    virtual bool    isValid() const
    {
        if (targetEdit_->text().isEmpty()) {
            se_->sernaDoc()->showMessageBox(
                SernaDoc::MB_WARNING, tr("Empty PI target"),
                tr("Please fill the PI target field!"), tr("&Ok"));
            return false;
        }
        if (!NameChecker::isValidQname(targetEdit_->text())) {
            se_->sernaDoc()->showMessageBox(
                SernaDoc::MB_WARNING, tr("Invalid PI Target"),
                tr("Invalid PI target name: must be valid XML name"), 
                tr("&Ok"));
            return false;
        }
        if (dataEdit_->text().contains("?>")) {
            se_->sernaDoc()->showMessageBox(
                SernaDoc::MB_WARNING, tr("Invalid PI contents"),
                tr("Invalid PI contents: \"?>\" is not allowed."), tr("&Ok"));
            return false;
        }
        return true;
    }

    virtual void    on_helpButton__clicked() {
        helpAssistant().show(DOCTAG(UG_PI_D));
    }
};

////////////////////////////////////////////////////////////////////////

class EditAsTextDialog : public EditCommentPIDialog { 
public:
    EditAsTextDialog(StructEditor* se, PropertyNode* props)
        : EditCommentPIDialog(se, props)
    {
        hideTarget();
        (void) new QtSyntaxHighlighter(dataEdit_->document());
    }
    virtual void    on_helpButton__clicked() {
        helpAssistant().show(DOCTAG(EDIT_XML_SOURCE));
    }
};

///////////////////////////////////////////////////////////////////////////

EditCommentPIDialog::EditCommentPIDialog(StructEditor* se, PropertyNode* props)
    : QDialog(se->widget(0)),
      se_(se), 
      props_(props)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), helpButton_),
            SIGNAL(activated()), this, SLOT(on_helpButton__clicked()));

    setCaption(props_->getSafeProperty("caption")->getString());
    dataEdit_->setPlainText(props_->getSafeProperty("data")->getString());
    if (props_->getProperty("is-readonly")) {
        dataEdit_->setReadOnly(true);
        targetEdit_->setReadOnly(true);
    } 
    else {
        readonlyLabel_->hide();
        readonlyIcon_->hide();
    }
}

void EditCommentPIDialog::on_okButton__clicked()
{
    if (dataEdit_->text().isEmpty()) {
        if (1 == se_->sernaDoc()->showMessageBox(
                SernaDoc::MB_WARNING, tr("Content field is empty"),
                tr("Content field is empty.\nIs this OK?"),
                tr("&Yes"), tr("&No")))
            return;
    }
    if (isValid()) 
        accept();
}

bool EditCommentPIDialog::execute()
{
    if (exec() != QDialog::Accepted)
        return false;
    props_->makeDescendant("data")->setString(dataEdit_->text());
    props_->makeDescendant("target")->setString(targetEdit_->text());
    return true;
}

////////////////////////////////////////////////////////////////////////

#define DECLARE_EDIT_DIALOG(commandName, dialogName) \
PROPTREE_EVENT_IMPL(commandName,  StructEditor) \
REGISTER_COMMAND_EVENT_MAKER(commandName, "PropertyTree", "PropertyTree") \
bool commandName::doExecute(StructEditor* se, EventData* ed) \
{ \
    dialogName d(se, ed_); \
    if (!d.execute()) return false; \
    static_cast<PropertyTreeEventData*>(ed)->root()->merge(d.properties(), 1);\
    return true;\
}

DECLARE_EDIT_DIALOG(ShowEditCommentDialog, EditCommentDialog)
DECLARE_EDIT_DIALOG(ShowEditPiDialog, EditPiDialog)
DECLARE_EDIT_DIALOG(ShowEditAsTextDialog, EditAsTextDialog)

////////////////////////////////////////////////////////////////////////

#include "moc/EditCommentPIDialog.moc"
