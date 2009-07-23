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
#include "xsltparam.h"
#include "XsltParamsDialog.h"
#include "NewParamSetDialog.h"
#include "EditParamDialog.h"
#include "structeditor/StructEditor.h"
#include "docview/SernaDoc.h"
#include "common/PropertyTreeEventData.h"
#include "xslt/Engine.h"
#include "xslt/TopParamValueHolder.h"
#include "xpath/ConstValueHolder.h"
#include "common/PathName.h"
#include "proputils/PropertyTreeSaver.h"
#include "utils/HelpAssistant.h"
#include "docutils/doctags.h"

#include <QMessageBox>
#include <QResizeEvent>
#include <QShortcut>
#include <QHeaderView>

#include <iostream>

using namespace Common;
using namespace XsltParam;

class GetXsltParams;
class SetXsltParams;

class ParamItem : public QTreeWidgetItem,
                  public Common::PropertyNodeWatcher {
public:
    ParamItem(QTreeWidget* list, PropertyNode* property, PropertyNode* curSet)
        : QTreeWidgetItem(list), 
          property_(property),
          curSet_(curSet),
          isFromSet_(false)
    {
        setText(0, property->name());
        setText(1, property->getSafeProperty(VALUE)->getString());
        if (curSet_ && property_->parent() && property_->parent()->parent() &&
            property_->parent()->parent() == curSet_)
                isFromSet_ = true;
        PropertyNode* val = property_->getProperty(VALUE);
        if (val)
            val->addWatcher(this);
        propertyChanged(0);
    }
    bool             belongsToSet() const { return isFromSet_; }
    PropertyNode*    property() { return property_.pointer(); }

    void setProperty(PropertyNode* pn)
    {
        PropertyNode* val = property_->getProperty(VALUE);
        if (val)
            val->removeWatcher(this);
        property_ = pn;
        isFromSet_ = false;
        if (curSet_ && property_->parent() && property_->parent()->parent() &&
            property_->parent()->parent() == curSet_)
                isFromSet_ = true;
        val = property_->getProperty(VALUE);
        if (val)
            val->addWatcher(this);
        propertyChanged(0);
    }
    void propertyChanged(Common::PropertyNode*)
    {
        String val = property_->getSafeProperty(VALUE)->getString();
        if (val != text(1))
            setText(1, val);
        if (isFromSet_ != font(0).bold()) {
            QFont f = font(0);
            f.setBold(isFromSet_);
            setFont(0, f);
            setFont(1, f);
        }
        set_tooltip();
    }
    void set_tooltip()
    {
        QString ann   = property_->getSafeProperty(ANNOTATION)->getString();
        QString type  = property_->getSafeProperty(TYPE)->getString();
        if (type.isEmpty())
            type = property_->getSafeProperty(GUESSED_TYPE)->getString();
        setToolTip(0, QString(NOTR("<nobr><code>%1</pre></nobr>%2")).
            arg(property_->name()).arg(ann.isEmpty() ?
                QString() : QString(NOTR("<hr/><i>%1</i>")).arg(ann)));
        QString tip = QString(NOTR(
            "<nobr><font color='green'>%1</font> <code>%2</pre></nobr><br/>"
            "<nobr><font color='green'>%3</font> <i>%4</i></nobr>"))
                .arg(tr("Value:"))
                .arg(property_->getSafeProperty(VALUE)->getString())
                .arg(tr("Type:"))
                .arg(type);
        setToolTip(1, tip);
    }
private:
    Common::PropertyNodePtr property_;
    Common::PropertyNodePtr curSet_;
    bool                    isFromSet_;
};

XsltParamsDialog::XsltParamsDialog(StructEditor* se, PropertyNode* params,
                                   PropertyNode* defaultParams)
    : QDialog(se->sernaDoc()->widget(0)),
      se_(se), params_(params),
      paramsCopy_(params->copy(true)),
      default_(defaultParams), curSet_(0)
{
    setupUi(this);
    connect(new QShortcut(QKeySequence(Qt::Key_F1), helpButton_),
        SIGNAL(activated()), this, SLOT(on_helpButton__clicked()));
    QHeaderView& hv = *paramsListView_->header();
    hv.setStretchLastSection(true);
    hv.resizeSections(QHeaderView::ResizeToContents);

    int cur_idx = 0;
    int i = 0;
    paramsComboBox_->insertItem(tr(DEF_COMBO));
    PropertyNode* p = paramsCopy_->firstChild();
    for (; p; p = p->nextSibling(), i++) {
        String name = p->getProperty(NAME)->getString();
        if (name.isEmpty())
            continue;

        if (p->getProperty(CURRENT)) {
            curSet_ = p;
            cur_idx = i + 1;
        }
        paramsComboBox_->insertItem(name);
    }
    setModal(true);
    show();
    paramsComboBox_->setCurrentItem(cur_idx);
    paramSetChanged();
}

void XsltParamsDialog::paramSetChanged()
{
    String cur_name = paramsComboBox_->currentText();
    bool is_default = currentIsDefault();
    deleteButton_->setEnabled(!is_default);
    description_->setEnabled(!is_default);
    paramsListView_->clear();
    PropertyNode* p = paramsCopy_->firstChild();
    if (!is_default) {
        for(; p; p = p->nextSibling())
            if (cur_name == p->getProperty(NAME)->getString())
                curSet_ = p;
    }
    else
        curSet_ = 0;
    for (p = default_->copy(true)->firstChild(); p; p = p->nextSibling()) {
        PropertyNode* property = p;
        if (curSet_) {
            PropertyNode* pn = curSet_->makeDescendant(PARAMS)->firstChild();
            for (; pn && pn->name() != p->name(); pn = pn->nextSibling()) {}
            if (pn)
                property = pn;
        }
        if (!checkBox_->isChecked() && property == p &&
            p->getSafeProperty(TYPE)->getString().isEmpty())
            continue;
        new ParamItem(paramsListView_, property, curSet_);
    }
    description_->setText(curSet_ ?
        curSet_->getProperty(DESCRIPTION)->getString() : "");
    paramSelected(paramsListView_->currentItem());
}

bool XsltParamsDialog::currentIsDefault() const
{
    return (tr(DEF_COMBO) == paramsComboBox_->currentText());
}

void XsltParamsDialog::on_description__textChanged()
{
    if (curSet_)
        curSet_->makeDescendant(DESCRIPTION)->
            setString(description_->text());
}

void XsltParamsDialog::clone(bool isNew)
{
    NewParamSetDialog dialog(this);
    String name;
    while (true) {
        if (QDialog::Accepted != dialog.exec())
            return;
        name = dialog.nameEdit_->text();
        PropertyNode* dup = paramsCopy_->firstChild();
        for(; dup && name != dup->getSafeProperty(NAME)->getString();
              dup = dup->nextSibling()) {
        }
        if (!dup && DEF_COMBO != name)
            break;
        QString err(tr("Parameter set with this name already exists."));
        QMessageBox::warning(this, tr("Warning"), err);
    }
    PropertyNodePtr pn = new PropertyNode(PARAM_SET);
    pn->makeDescendant(NAME)->setString(name);
    pn->makeDescendant(DESCRIPTION)->setString(dialog.descEdit_->text());
    if (!isNew && curSet_ && tr(DEF_COMBO) != paramsComboBox_->currentText())
        pn->appendChild(curSet_->makeDescendant(PARAMS)->copy(true));
    paramsCopy_->appendChild(pn.pointer());
    curSet_ = pn.pointer();
    paramsComboBox_->insertItem(name);
    paramsComboBox_->setCurrentItem(paramsComboBox_->count() - 1);
    paramSetChanged();
}


void XsltParamsDialog::on_cloneButton__clicked()
{
    clone();
}

void XsltParamsDialog::on_newButton__clicked()
{
    clone(true);
}

void XsltParamsDialog::on_editButton__clicked()
{
    ParamItem* item = static_cast<ParamItem*>(paramsListView_->currentItem());
    if (!editButton_->isEnabled() || !item)
        return;
    PropertyNode* prop = item->property();
    EditParamDialog dialog(prop, this);
    if (QDialog::Accepted == dialog.exec()) {
        String value = dialog.valueEdit_->text();
        PropertyNodePtr pn = prop;
        if (!item->belongsToSet()) {
            pn = prop->copy(true);
            curSet_->makeDescendant(PARAMS)->appendChild(pn.pointer());
            item->setProperty(pn.pointer());
        }
        pn->makeDescendant(VALUE)->setString(value);
    }
    paramSelected(item);
}

void XsltParamsDialog::on_setButton__clicked()
{
    ParamItem* item = static_cast<ParamItem*>(paramsListView_->currentItem());
    if (!setButton_->isEnabled() || !item || !item->belongsToSet())
        return;
    PropertyNode* prop = item->property();
    String cur_name = prop->name();
    for (PropertyNode* p = default_->firstChild(); p; p = p->nextSibling()) {
        if (cur_name == p->name()) {
            prop->remove();
            item->setProperty(p);
            break;
        }
    }
    setButton_->setEnabled(false);
}

void XsltParamsDialog::paramSelected(QTreeWidgetItem* item)
{
    setButton_->setEnabled(item &&
                           static_cast<ParamItem*>(item)->belongsToSet());
    editButton_->setEnabled(!currentIsDefault() && item);
}

void XsltParamsDialog::on_deleteButton__clicked()
{
    if (0 == se_->sernaDoc()->showMessageBox(
            SernaDoc::MB_WARNING, tr("Delete XSLT Parameter Set"),
            tr("Are you sure you want to delete XSLT Parameter Set?"),
            tr("&Delete"), tr("&Cancel"))) {
        curSet_->remove();
        paramsComboBox_->removeItem(paramsComboBox_->currentItem());
        paramSetChanged();
    }
}

void XsltParamsDialog::on_applyButton__clicked()
{
    String cur_name = curSet_ ? curSet_->makeDescendant(NAME)->getString():"";
    bool found = false;
    PropertyNode* cur = params_->firstChild();
    for (; cur && !cur->getProperty(CURRENT); cur = cur->nextSibling()) {}
    if (cur && cur_name == cur->getSafeProperty(NAME)->getString()) 
        found = true;
    params_->removeAllChildren();
    for (PropertyNode* p = paramsCopy_->firstChild(); p; p = p->nextSibling())
        params_->appendChild(p->copy(true));

    PropertyNode* set = 0;
    PropertyNodePtr clean_set = new PropertyNode(PARAMS);
    for (PropertyNode* p = default_->firstChild();p;p = p->nextSibling()){
        PropertyNode* pnode = p->copy(true);
        pnode->makeDescendant(VALUE)->remove();
        clean_set->appendChild(pnode);
    }

    if (curSet_) {
        set = curSet_->makeDescendant(PARAMS)->copy(true);
        set->merge(clean_set.pointer());
    }
    else
        set = clean_set.pointer();
    PropertyTreeEventData ed(set);
    makeCommand<SetXsltParams>(&ed)->execute(se_, &ed);
    
    QDialog::accept();
}

void XsltParamsDialog::on_helpButton__clicked()
{
    helpAssistant().show(DOCTAG(EDIT_PARAM_SET));
}

