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
#include "structeditor/impl/entity_utils.h"
#include "utils/NameChecker.h"
#include "common/PropertyTreeEventData.h"
#include "utils/HelpAssistant.h"
#include "grove/Grove.h"
#include "grove/EntityDeclSet.h"
#include "docutils/doctags.h"

#include <QShortcut>
#include "ConvertToIntEntityDialogBase.hpp"

using namespace Common;

class ConvertToIntEntityDialogImpl : public QDialog,
                                     private Ui::ConvertToIntEntityDialogBase {
    Q_OBJECT
public:
    ConvertToIntEntityDialogImpl(StructEditor* se,
                                 bool changeEntityName = false);

    bool        getName(COMMON_NS::String&);
    void        setName(const COMMON_NS::String&);

protected slots:
    void        on_buttonHelp__clicked();
    void        on_entityNameEdit__textChanged(const QString&);

private:
    StructEditor*   se_;
    bool            changeEntityName_;
};

ConvertToIntEntityDialogImpl::ConvertToIntEntityDialogImpl(StructEditor* se,
                                                           bool changeName)
    : QDialog(se->widget(0)),
      se_(se), changeEntityName_(changeName)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), buttonHelp_),
        SIGNAL(activated()), this, SLOT(on_buttonHelp__clicked()));

    QString docname = QString(NOTR(" (%1)")).arg(get_docname(se));

    if (changeEntityName_)
        setCaption(tr("Change Internal Entity Name") + docname);
    else
        setCaption(caption() + docname);
}

void 
ConvertToIntEntityDialogImpl::on_entityNameEdit__textChanged(const QString&)
{
    buttonOk_->setEnabled(!entityNameEdit_->text().isEmpty());
}

bool ConvertToIntEntityDialogImpl::getName(String& entityName)
{
    for (;;) {
        if (QDialog::Accepted != exec())
            return false;
        entityName = entityNameEdit_->text();
        if (NameChecker::checkEntity(entityName,
                *get_current_grove(se_)->entityDecls()))
            return true;
    }
}

void ConvertToIntEntityDialogImpl::setName(const String& entityName)
{
    entityNameEdit_->validateAndSet(entityName, 0, 0, 0);
}

void ConvertToIntEntityDialogImpl::on_buttonHelp__clicked()
{
    helpAssistant().show(changeEntityName_ ? DOCTAG(UG_CONVERT_INT_ENT_D) :
                                             DOCTAG(UG_CONVERT_INT_ENT_D));
}

//////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(ConvertToIntEntityDialog, StructEditor)
PROPTREE_EVENT_IMPL(ConvertToIntEntityDialogProp, StructEditor)

bool ConvertToIntEntityDialogProp::doExecute(StructEditor* se, EventData* ed)
{
    String entityName;
    bool changeEntity = false;
    if (ed_) {
        entityName = ed_->getSafeProperty("entityName")->getString();
        changeEntity = ed_->getSafeProperty("changeEntity")->getBool();
    }
    ConvertToIntEntityDialogImpl d(se, changeEntity);
    if (ed_) 
        d.setName(entityName);
    if (!d.getName(entityName))
        return false;
    static_cast<PropertyTreeEventData*>(ed)->root()->
        makeDescendant("entityName", entityName, true);
    return true;
}

bool ConvertToIntEntityDialog::doExecute(StructEditor* se, EventData* ed)
{
    return makeCommand<ConvertToIntEntityDialogProp>(0)->execute(se, ed);
}

#include "moc/ConvertToIntEntityDialog.moc"
