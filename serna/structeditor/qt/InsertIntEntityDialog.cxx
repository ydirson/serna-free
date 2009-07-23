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

#include "utils/GrovePosEventData.h"
#include "utils/HelpAssistant.h"
#include "docutils/doctags.h"

#include "grove/Grove.h"
#include "grove/EntityDeclSet.h"
#include "groveeditor/GrovePos.h"

#include "ui/IconProvider.h"

#include <QHeaderView>
#include <QShortcut>
#include <QPixmap>

#include "structeditor/InsertIntEntityDialogBase.hpp"

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;

class InsertIntEntityDialogImpl : public QDialog,
                                  protected Ui::InsertIntEntityDialogBase {
    Q_OBJECT
public:
    InsertIntEntityDialogImpl(StructEditor* se, const GrovePos& grovePos);

    bool        getParam(String& name);

protected slots:
    void        on_entityListView__currentItemChanged(QTreeWidgetItem*, 
        QTreeWidgetItem*);
    void        on_entityListView__itemDoubleClicked(QTreeWidgetItem*, int) {
        if (okButton_->isEnabled())
            accept();
    }

    void        on_helpButton__clicked() {
        helpAssistant().show(DOCTAG(UG_INSERT_INT_ENT_D));
    }

private:
    StructEditor*       se_;
    const GrovePos&     gpos_;
    QTreeWidgetItem*    rootItem_;
    QTreeWidgetItem*    externalItem_;
    QTreeWidgetItem*    internalItem_;
    QTreeWidgetItem*    symbolItem_;
};

InsertIntEntityDialogImpl::InsertIntEntityDialogImpl(StructEditor* se,
                                                     const GrovePos& gpos)
    : QDialog(se->widget()),
      se_(se), 
      gpos_(gpos)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), helpButton_),
            SIGNAL(activated()), this, SLOT(on_helpButton__clicked()));

    entityListView_->header()->setVisible(false);
    entityListView_->setSortingEnabled(false);

    QPixmap e_subset = Sui::icon_provider().getPixmap(
        NOTR("internal_entity_external_subset"));
    QPixmap i_subset = Sui::icon_provider().getPixmap(
        NOTR("internal_entity_internal_subset"));
    QPixmap sym_subset = Sui::icon_provider().getPixmap(NOTR("insert_symbol"));

    rootItem_ = new QTreeWidgetItem(entityListView_);
    rootItem_->setText(0, tr("Internal Entities"));
    rootItem_->setIcon(0, Sui::icon_provider().getPixmap(
                           NOTR("internal_entity")));

    externalItem_ = new QTreeWidgetItem(rootItem_);
    externalItem_->setText(0, tr("External DTD Subset"));
    externalItem_->setIcon(0, e_subset);

    internalItem_ = new QTreeWidgetItem(rootItem_);
    internalItem_->setText(0, tr("Internal DTD Subset"));
    internalItem_->setIcon(0, i_subset);
    
    symbolItem_ = new QTreeWidgetItem(rootItem_);
    symbolItem_->setText(0, tr("Symbols (External DTD Subset)"));
    symbolItem_->setIcon(0, sym_subset);

    const EntityDeclSet* eds = get_current_grove(se)->entityDecls();
    setCaption(caption() + " (" + get_docname(se).qstring() + ")");

    EntityDeclSet::iterator iter;
    const EntityReferenceStart* ers = gpos.getErs();
    for (iter = eds->begin(); iter != eds->end(); ++iter) {
        String name = (*iter)->name();
        if (has_entity_loop(ers, name))
            continue;
        if ((*iter)->isSingleCharEntity()) {
            QTreeWidgetItem* i = new QTreeWidgetItem(symbolItem_);
            i->setText(0, name);
            i->setIcon(0, sym_subset);
            continue;
        }
        if ((*iter)->asInternalEntityDecl()) {
            EntityDecl::DeclOrigin dor = (*iter)->declOrigin();
            if (EntityDecl::dtd == dor) {
                QTreeWidgetItem* i = new QTreeWidgetItem(externalItem_);
                i->setText(0, name);
                i->setIcon(0, e_subset);
            }
            else
                if (EntityDecl::prolog == dor) {
                    QTreeWidgetItem* i = new QTreeWidgetItem(internalItem_);
                    i->setText(0, name);
                    i->setIcon(0, i_subset);
                }
        }
    }
    entityListView_->expandItem(rootItem_);
    entityListView_->collapseItem(externalItem_);
    entityListView_->collapseItem(internalItem_);
    entityListView_->collapseItem(symbolItem_);
    entityListView_->sortItems(0, Qt::AscendingOrder);
    on_entityListView__currentItemChanged(entityListView_->topLevelItem(0), 0);
}

void InsertIntEntityDialogImpl::on_entityListView__currentItemChanged
    (QTreeWidgetItem* cur, QTreeWidgetItem*)
{
    if (!cur || cur == rootItem_ || cur == externalItem_ ||
         cur == internalItem_) {
       contentTextEdit_->setText("");
       okButton_->setEnabled(false);
       return;
    }
    Grove* grove = get_current_grove(se_);
    const EntityDeclSet* eds = grove->entityDecls();
    EntityDecl* decl = eds->lookupDecl(cur->text(0));
    if (decl && decl->asInternalEntityDecl()) { 
        if (decl->isContentModified()) 
            decl->asInternalEntityDecl()->rebuildContent(grove);
        contentTextEdit_->setText(decl->asInternalEntityDecl()->content());
        okButton_->setEnabled(true);
    } 
    else {
       contentTextEdit_->setText("");
       okButton_->setEnabled(false);
    }
}

bool InsertIntEntityDialogImpl::getParam(String& name)
{
    if (QDialog::Accepted != exec())
        return false;
    QTreeWidgetItem* cur = entityListView_->currentItem();
    if (cur != rootItem_ || cur != externalItem_ || cur != internalItem_)
        name = cur->text(0);
    return true;
}

//////////////////////////////////////////////////////////////////////

GROVEPOS_EVENT_IMPL(InsertIntEntityDialog, StructEditor)

bool InsertIntEntityDialog::doExecute(StructEditor* se, EventData* ed)
{
    InsertIntEntityDialogImpl d(se, pos_);
    String entityName;
    if (!d.getParam(entityName))
        return false;
    static_cast<PropertyTreeEventData*>(ed)->root()->
        makeDescendant("entityName", entityName);
    return true;
}

#include "moc/InsertIntEntityDialog.moc"
