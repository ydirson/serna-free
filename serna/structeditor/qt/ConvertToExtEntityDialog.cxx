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
#include "grove/Grove.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityDecl.h"
#include "grove/PrologNodes.h"
#include "groveeditor/GrovePos.h"
#include "utils/HelpAssistant.h"
#include "common/Url.h"
#include "docutils/doctags.h"

#include "ConvertToExtEntityDialogBase.hpp"
#include "docview/qt/BrowseButtonMenu.h"
#include <QShortcut>
#include <QMessageBox>

using namespace Common;

class ConvertToExtEntityDialogImpl : public QDialog,
                                     private Ui::ConvertToExtEntityDialogBase {
    Q_OBJECT
public:
    ConvertToExtEntityDialogImpl(StructEditor* se,
                                 const Common::String& entityName,
                                 const Common::String& entityPath,
                                 bool changeEntity = false);

    bool        getParam(Common::String& entityName,
                         Common::String& entityPath);

protected slots:
    void    on_buttonHelp__clicked();
    void    on_entityNameEdit__textChanged(const QString&) { dataChanged(); }
    void    on_fileNameEdit__textChanged(const QString&) { dataChanged(); }

    void    selectUrl(const Common::String& url);

private:
    void        dataChanged();
    
    StructEditor*                   se_;
    bool                            changeEntity_;
    QPointer<BrowseButtonMenu>      browseMenu_;
    QString                         oldPath_;
    QString                         oldName_;
};

ConvertToExtEntityDialogImpl::ConvertToExtEntityDialogImpl(
    StructEditor* se, const String& entityName,
    const String& entityPath, bool changeEntity)
    : QDialog(se->widget(0)),
      se_(se), changeEntity_(changeEntity),
      oldPath_(entityPath),
      oldName_(entityName)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), buttonHelp_),
        SIGNAL(activated()), this, SLOT(on_buttonHelp__clicked()));

    browseMenu_ = new BrowseButtonMenu(
        se_->sernaDoc(), buttonBrowse_, get_current_entity_url(se), true);
    connect(browseMenu_, SIGNAL(urlSelected(const Common::String&)),
            this, SLOT(selectUrl(const Common::String&)));

    QString docname(get_docname(se));
    docname = " (" + docname + ")";

    if (changeEntity)
        setCaption(tr("Change External Entity") + docname);
    else
        setCaption(caption() + docname);
    entityNameEdit_->setText(entityName);
    fileNameEdit_->setText(entityPath);
}

bool ConvertToExtEntityDialogImpl::getParam(String& entityName,
                                            String& entityPath)
{
    for (;;) {
        if (exec() != QDialog::Accepted)
            return false;
        entityName = entityNameEdit_->text();
        entityPath = fileNameEdit_->text();
        if ((changeEntity_ && entityPath != oldPath_ && 
            entityName == oldName_) || NameChecker::checkEntity(entityName,
                *get_current_grove(se_)->entityDecls()))
                    return true;
    }
}

void ConvertToExtEntityDialogImpl::dataChanged()
{
    buttonOk_->setEnabled(!entityNameEdit_->text().isEmpty() &&
                          !fileNameEdit_->text().isEmpty());
}

void ConvertToExtEntityDialogImpl::selectUrl(const String& url)
{
    String rel_url = 
        Url(se_->editViewSrcPos().getErs()->xmlBase()).relativePath(url);
    fileNameEdit_->setText(rel_url);
}

void ConvertToExtEntityDialogImpl::on_buttonHelp__clicked()
{
    helpAssistant().show(changeEntity_ ? DOCTAG(UG_CH_EXT_ENT_D) :
                                         DOCTAG(UG_CONVERT_EXT_ENT_D));
}

/////////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(ConvertToExtEntityDialog, StructEditor)
PROPTREE_EVENT_IMPL(ConvertToExtEntityDialogProp,   StructEditor)

bool ConvertToExtEntityDialogProp::doExecute(StructEditor* se, EventData* ed)
{
    String entityName, entityPath;
    bool changeEntity = false;
    if (ed_) {
        entityName = ed_->getSafeProperty("entityName")->getString();
        entityPath = ed_->getSafeProperty("entityPath")->getString();
        changeEntity = ed_->getSafeProperty("changeEntity")->getBool();
    }
    ConvertToExtEntityDialogImpl d(se, entityName, entityPath, changeEntity);
    if (!d.getParam(entityName, entityPath))
        return false;
    PropertyNode* pn = static_cast<PropertyTreeEventData*>(ed)->root();
    pn->makeDescendant("entityName", entityName, true);
    pn->makeDescendant("entityPath", entityPath, true);
    return true;
}

bool ConvertToExtEntityDialog::doExecute(StructEditor* se, EventData* ed)
{
    return makeCommand<ConvertToExtEntityDialogProp>(0)->execute(se, ed);
}

#include "moc/ConvertToExtEntityDialog.moc"
