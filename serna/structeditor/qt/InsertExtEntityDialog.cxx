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
#include "docview/SernaDoc.h"
#include "docview/qt/BrowseButtonMenu.h"

#include "structeditor/StructEditor.h"
#include "structeditor/impl/entity_utils.h"

#include "ui/IconProvider.h"

#include "utils/HelpAssistant.h"
#include "utils/GrovePosEventData.h"
#include "utils/NameChecker.h"
#include "common/PropertyTreeEventData.h"

#include "docutils/doctags.h"

#include "grove/Grove.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityDecl.h"
#include "grove/SectionSyncher.h"
#include "grove/EntityReferenceTable.h"

#include "common/Url.h"
#include "groveeditor/GrovePos.h"

#include <QShortcut>
#include <QDir>
#include <QHeaderView>
#include <QPixmap>

#include "structeditor/InsertExtEntityDialogBase.hpp"

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;

class InsertExtEntityDialogImpl : public QDialog,
                                  protected Ui::InsertExtEntityDialogBase {
    Q_OBJECT
public:
    InsertExtEntityDialogImpl(StructEditor* se, const GrovePos& gp,
                              const String& baseUrl);

    bool        getParam(String& entityName, String& entityPath);

protected slots:
    void        on_entityNameLineEdit__textChanged() { dataChanged(); }
    void        on_fileNameLineEdit__textChanged() { dataChanged(); }
    
    void        on_fileRadioButton__clicked()
    {
        switchOption(false);
        dataChanged();
    }
    void        on_declaredRadioButton__clicked()
    {
        switchOption(true);
        okButton_->setEnabled(0 != entityListView_->topLevelItemCount());
    }
    void        on_entityListView__currentItemChanged(QTreeWidgetItem*,
                                                      QTreeWidgetItem*)
    {
        okButton_->setEnabled(true);
    }
    void        selectUrl(const Common::String& url)
    {
        fileNameLineEdit_->setText(String(Url(get_current_entity_url(se_)).
                                          relativePath(url)).qstring());
    }
    void        on_helpButton__clicked() 
    {
        helpAssistant().show(DOCTAG(UG_INSERT_EXT_ENT_D));
    }
private:
    void        dataChanged();
    void        switchOption(bool sw);

protected:
    StructEditor*               se_;
    GrovePos                    pos_;
    QPointer<BrowseButtonMenu>  browseMenu_;
};

InsertExtEntityDialogImpl::InsertExtEntityDialogImpl(
    StructEditor* se, const GrovePos& pos, const String& baseUrl)
    : QDialog(se->widget()),
      se_(se),
      pos_(pos)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), helpButton_),
            SIGNAL(activated()), this, SLOT(on_helpButton__clicked()));
    
    browseMenu_ = new BrowseButtonMenu(se_->sernaDoc(), browseButton_, 
                                       baseUrl);
    connect(browseMenu_, SIGNAL(urlSelected(const Common::String&)),
            this, SLOT(selectUrl(const Common::String&)));

    QPixmap e_subset = Sui::icon_provider().getPixmap(
        NOTR("external_entity_external_subset"));
    QPixmap i_subset = Sui::icon_provider().getPixmap(
        NOTR("external_entity_internal_subset"));

    const EntityDeclSet* eds = get_current_grove(se)->entityDecls();
    setCaption(caption() + " (" + get_docname(se).qstring() + ")");

    const EntityReferenceStart* ers = pos.getErs();
    EntityDeclSet::iterator iter = eds->begin();
    for (; iter != eds->end(); ++iter) {
        if (has_entity_loop(ers, (*iter)->name()))
            continue;
        if ((*iter)->asExternalEntityDecl() &&
            (*iter)->declType() != EntityDecl::document) {
            EntityDecl::DeclOrigin dor = (*iter)->declOrigin();
            QTreeWidgetItem* i = new QTreeWidgetItem(entityListView_);
            i->setText(0, (*iter)->name());
            i->setText(1, (*iter)->asExternalEntityDecl()->sysid());
            if (EntityDecl::dtd == dor)
                i->setIcon(0, i_subset);
            else
                i->setIcon(0, e_subset);
        }
    }
    entityListView_->header()->hide();
    switchOption(true);
}

bool InsertExtEntityDialogImpl::getParam(String& entityName, 
                                         String& entityPath)
{
    if (QDialog::Accepted != exec())
        return false;
    if (fileRadioButton_->isChecked()) {
        entityName = entityNameLineEdit_->text();
        entityPath = fileNameLineEdit_->text();
        return true;
    }
    QTreeWidgetItem* cur = entityListView_->currentItem();
    if (cur) {
        entityName = cur->text(0);
        return true;
    }
    return false;
}

void InsertExtEntityDialogImpl::dataChanged()
{
    okButton_->setEnabled(!entityNameLineEdit_->text().isEmpty() &&
        !fileNameLineEdit_->text().isEmpty() &&
        NameChecker::isValidLocalName(entityNameLineEdit_->text()));
}

void InsertExtEntityDialogImpl::switchOption(bool sw)
{
    declaredRadioButton_->setChecked(sw);
    fileRadioButton_->setChecked(!sw);
    entityListView_->setEnabled(sw);
    entityNameLabel_->setEnabled(!sw);
    fileNameLabel_->setEnabled(!sw);
    entityNameLineEdit_->setEnabled(!sw);
    fileNameLineEdit_->setEnabled(!sw);
    browseButton_->setEnabled(!sw);
}

/////////////////////////////////////////////////////////////////

GROVEPOS_EVENT_IMPL(InsertExtEntityDialog, StructEditor)

bool InsertExtEntityDialog::doExecute(StructEditor* se, EventData* ed)
{
    InsertExtEntityDialogImpl d(se, pos_,
        ed_->getSafeProperty("browseDir")->getString());
    String entityName, entityPath;
    if (!d.getParam(entityName, entityPath))
        return false;
    PropertyNode* pn = static_cast<PropertyTreeEventData*>(ed)->root();
    pn->makeDescendant("entityName", entityName);
    pn->makeDescendant("entityPath", entityPath);
    return true;
}

#include "moc/InsertExtEntityDialog.moc"
