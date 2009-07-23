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
#include "common/PropertyTreeEventData.h"
#include "ui/IconProvider.h"
#include "grove/Grove.h"
#include "grove/Nodes.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityReferenceTable.h"
#include "grove/EntityDecl.h"
#include "grove/SectionNodes.h"
#include "grove/SectionSyncher.h"
#include "utils/HelpAssistant.h"
#include "docutils/doctags.h"
#include "common/PathName.h"
#include "structeditor/impl/entity_utils.h"
#include "groveeditor/GrovePos.h"

#include <QShortcut>
#include <QHeaderView>
#include "structeditor/EntitiesDialogBase.hpp"

using namespace Common;
using namespace GroveLib;

class EntitiesDialogImpl : public QDialog, 
                           private Ui::EntitiesDialogBase {
    Q_OBJECT
public:
    EntitiesDialogImpl(StructEditor* se);

    bool        getParam(Common::String& name,
                         Common::String& newName,
                         Common::String& sysd);

protected slots:
    void        on_entityListView__currentItemChanged(
        QTreeWidgetItem*, QTreeWidgetItem*) { entitySelected(); }
    void        on_buttonRemove__clicked();
    void        on_buttonHelp__clicked();
    void        on_buttonChange__clicked();

private:
    void              entitySelected();

    StructEditor*     se_;
    QTreeWidgetItem*    externalItem_;
    QTreeWidgetItem*    internalItem_;
    Common::String newName_;
    Common::String sysid_;
    bool              removed_;
};


EntitiesDialogImpl::EntitiesDialogImpl(StructEditor* se)
    : QDialog(se->widget(0)),
      se_(se), removed_(false)
{
    setupUi(this);
    hboxLayout->setStretchFactor(vboxLayout,  1);
    hboxLayout->setStretchFactor(vboxLayout1, 3);

    connect(new QShortcut(QKeySequence(Qt::Key_F1), buttonHelp_),
        SIGNAL(activated()), this, SLOT(on_buttonHelp__clicked()));

    QPixmap e_entity = Sui::icon_provider().getPixmap(NOTR("external_entity"));
    QPixmap e_i_subset =
        Sui::icon_provider().getPixmap(NOTR("external_entity_internal_subset"));
    QPixmap i_entity =
        Sui::icon_provider().getPixmap(NOTR("internal_entity"));
    QPixmap i_i_subset =
        Sui::icon_provider().getPixmap(NOTR("internal_entity_internal_subset"));

    entityListView_->header()->hide();
    entityListView_->setColumnCount(1);

    externalItem_ = new QTreeWidgetItem;
    externalItem_->setIcon(0, e_entity);
    externalItem_->setText(0, tr("External Entities"));
    entityListView_->addTopLevelItem(externalItem_);

    internalItem_ = new QTreeWidgetItem;
    internalItem_->setIcon(0, i_entity);
    internalItem_->setText(0, tr("Internal Entities"));
    entityListView_->addTopLevelItem(internalItem_);

    const EntityDeclSet* eds = get_current_grove(se)->entityDecls();
    setCaption(caption() + " (" + get_docname(se).qstring() + ")");
    EntityDeclSet::iterator iter;
    const EntityDecl* ed = 0;
    for (iter = eds->begin(); iter != eds->end(); ++iter) {
        ed = iter->pointer();
        if (ed->declOrigin() != EntityDecl::prolog)
            continue;
        if (ed->declType() == EntityDecl::internalGeneralEntity) {
            QTreeWidgetItem* item = new QTreeWidgetItem(internalItem_);
            item->setIcon(0, i_i_subset);
            item->setText(0, ed->name());
        } else if (ed->declType() == EntityDecl::externalGeneralEntity) {
            QTreeWidgetItem* item = new QTreeWidgetItem(externalItem_);
            item->setIcon(0, e_i_subset);
            item->setText(0, ed->name());
        }
    }
    //rootItem_->setOpen(true);
    //externalItem_->setOpen(false);
    //internalItem_->setOpen(false);
    entitySelected();
}

void EntitiesDialogImpl::entitySelected()
{
    QTreeWidgetItem* cur = entityListView_->currentItem();
    if (!cur || cur == externalItem_ || cur == internalItem_) {
       contentTextEdit_->setText("");
       buttonChange_->setEnabled(false);
       buttonRemove_->setEnabled(false);
       return;
    }
    Grove* grove = get_current_grove(se_);
    EntityDecl* decl = grove->entityDecls()->lookupDecl(cur->text(0));
    if (0 == decl)
        return;
    const EntityReferenceTable::ErtEntry* ert =
        grove->document()->ert()->lookup(decl);
    InternalEntityDecl* ied = decl->asInternalEntityDecl();
    if (ied) {
        if (ied->isContentModified())
            ied->rebuildContent(grove);
        contentTextEdit_->setText(ied->content());
        buttonChange_->setEnabled(true);
        buttonRemove_->setEnabled(!ert);
    }
    else if (decl->asExternalEntityDecl()) {
        contentTextEdit_->setText(decl->asExternalEntityDecl()->sysid());
        buttonChange_->setEnabled(true);
        buttonRemove_->setEnabled(!ert);
    }
}

void EntitiesDialogImpl::on_buttonRemove__clicked()
{
    removed_ = true;
    accept();
}

class ConvertToIntEntityDialogProp;
class ConvertToExtEntityDialogProp;

void EntitiesDialogImpl::on_buttonChange__clicked()
{
    String entityName, entityPath;
    QTreeWidgetItem* cur = entityListView_->currentItem();
    entityName = entityListView_->currentItem()->text(0);
    entityPath = contentTextEdit_->text();
    PropertyTreeEventData ed;
    ed.root()->makeDescendant("entityName", entityName);
    ed.root()->makeDescendant("changeEntity", "")->setBool(true);
    if (cur->parent() == internalItem_) {
        if (!makeCommand<ConvertToIntEntityDialogProp>(&ed)->execute(se_, &ed))
            return;
        newName_ = ed.root()->getSafeProperty("entityName")->getString();
        accept();
        return;
    }
    ed.root()->makeDescendant("entityPath", entityPath);
    if (!makeCommand<ConvertToExtEntityDialogProp>(&ed)->execute(se_, &ed))
        return;
    newName_ = ed.root()->getSafeProperty("entityName")->getString();
    sysid_   = ed.root()->getSafeProperty("entityPath")->getString();
    accept();
}

bool EntitiesDialogImpl::getParam(String& name,
                              String& newName, String& sysid)
{
    if (QDialog::Accepted != exec())
        return false;
    QTreeWidgetItem* cur = entityListView_->currentItem();
    name = cur->text(0);
    newName = removed_ ? String() : newName_;
    sysid = sysid_;
    return true;
}

void EntitiesDialogImpl::on_buttonHelp__clicked()
{
    helpAssistant().show(DOCTAG(UG_ENTITY_DECL_D));
}

//////////////////////////////////////////////////////////////////

SIMPLE_COMMAND_EVENT_IMPL(EntitiesDialog, StructEditor)

bool EntitiesDialog::doExecute(StructEditor* se, EventData* ed)
{
    EntitiesDialogImpl d(se);
    String name, newName, sysid;
    if (!d.getParam(name, newName, sysid))
        return false;
    PropertyNode* pn = static_cast<PropertyTreeEventData*>(ed)->root();
    pn->makeDescendant("entityName", name);
    pn->makeDescendant("entityPath", sysid);
    pn->makeDescendant("newEntityName", newName);
    return true;
}

#include "moc/EntitiesDialog.moc"
