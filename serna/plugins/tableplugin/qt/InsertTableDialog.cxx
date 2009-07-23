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
#include "plugins/tableplugin/TablePlugin.h"
#include "plugins/tableplugin/TableUtils.h"
#include "common/CommandEvent.h"
#include "common/PropertyTreeEventData.h"
#include "InsertTableDialogBase.hpp"
#include "ColumnAttributesDialogBase.hpp"

#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QDialog>
#include <QApplication>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

using namespace Common;
using namespace TableUtils;

class InsertTableDialog : public QDialog,
                          public Ui::InsertTableDialogBase {
    Q_OBJECT
public:
    InsertTableDialog(QWidget* parent, PropertyNode* prop);
    
    const char* frameType() const;
    
public slots:
    void on_helpButton__clicked();
    void on_attrsButton__clicked();

private:
    PropertyNode* property_;
};

class ColumnAttributesDialog : public QDialog,
                               public Ui::ColumnAttributesDialogBase {
    Q_OBJECT
public:
    ColumnAttributesDialog(QWidget* parent, PropertyNode* prop);

    QString     str_item(int, int) const;
    QString     bool_item(int, int) const;
    int         columnCount() const { return attrTable_->columnCount(); }

private:
    void    set_vertheader(int row, const QString& str);
};

static const char FRAME_TYPE_SIZE = 6;

static const char* frame_types[FRAME_TYPE_SIZE][2] = {
    { NOTR("0"),     QT_TRANSLATE_NOOP("TableFrame", "All")},
    { NOTR("1"),     QT_TRANSLATE_NOOP("TableFrame", "Bottom")}, 
    { NOTR("2"),     QT_TRANSLATE_NOOP("TableFrame", "Sides")}, 
    { NOTR("3"),     QT_TRANSLATE_NOOP("TableFrame", "Top")}, 
    { NOTR("4"),     QT_TRANSLATE_NOOP("TableFrame", "Top and Bottom")}, 
    { NOTR("5"),     QT_TRANSLATE_NOOP("TableFrame", "No Frame")}
};

InsertTableDialog::InsertTableDialog(QWidget* parent, PropertyNode* prop)
    : QDialog(parent),
      property_(prop)
{
    setupUi(this);
    for (int c = 0; c < FRAME_TYPE_SIZE; c++)
        frameCombo_->insertItem(
            qApp->translate("TableFrame", frame_types[c][1]), c);
}

const char* InsertTableDialog::frameType() const
{
    return frame_types[frameCombo_->currentItem()][0];
}

void InsertTableDialog::on_helpButton__clicked()
{
}

static bool call_attr_dialog(QWidget* parent,
                             PropertyNode* property)
{
    PropertyNode* prop = property->makeDescendant("column_attrs");
    ColumnAttributesDialog dialog(parent, prop);
    if (QDialog::Accepted != dialog.exec())
            return false;
    PropertyNode* attr = prop->firstChild();
    for (int idx = 0; idx < dialog.columnCount(); ++idx) {
        attr->makeDescendant("colspec-num")->setInt(idx + 1);
        attr->makeDescendant("colspec-name")->setString
            (dialog.str_item(0, idx));
        attr->makeDescendant("colspec-width")->setString
            (dialog.str_item(1, idx));
        attr->makeDescendant("colspec-align")->setString
            (dialog.str_item(2, idx));
        attr->makeDescendant("colspec-colsep")->setString
            (dialog.bool_item(3, idx));
        attr->makeDescendant("colspec-rowsep")->setString
            (dialog.bool_item(4, idx));
        attr = attr->nextSibling();
    }
    return true;
}

void InsertTableDialog::on_attrsButton__clicked()
{
    PropertyNode* prop = property_->makeDescendant("column_attrs");
    int cols = colsSpin_->value();
    PropertyNode* attr = prop->firstChild();
    for (int i = 1; i <= cols; i++) {
        bool to_append = false;
        if (!attr) {
            attr = new PropertyNode(NOTR("attr"));
            to_append = true;
        }
        String colnum = attr->makeDescendant("colspec-num")->getString();
        if (colnum.isEmpty())
            attr->makeDescendant("colspec-num")->setString(String::number(i));
        if (to_append)
            prop->appendChild(attr);
        attr = attr->nextSibling();
    }
    call_attr_dialog(this, property_);
}

/////////////////////////////////////////////////////////////////////////

void ColumnAttributesDialog::set_vertheader(int row, const QString& str)
{
    attrTable_->setVerticalHeaderItem(row,
        new QTableWidgetItem(str));
}

QString ColumnAttributesDialog::str_item(int row, int col) const
{
    return attrTable_->item(row, col)->text();
}

QString ColumnAttributesDialog::bool_item(int row, int col) const
{
    return (attrTable_->item(row, col)->checkState() == Qt::Checked)
        ? "" : "0"; // ask ego ...
}

static void set_check_state(QTableWidgetItem* item,
                            PropertyNode* pn,
                            const char* propname)
{
    String str = pn->getSafeProperty(propname)->getString();
    item->setCheckState((str == "0") ? Qt::Unchecked : Qt::Checked);
}

ColumnAttributesDialog::ColumnAttributesDialog(QWidget* parent,
                                               PropertyNode* prop)
    : QDialog(parent)
{
    setupUi(this);
    attrTable_->clear();
    attrTable_->setRowCount(5);

    int count = 0;
    PropertyNode* node = prop->firstChild();
    for (; node; node = node->nextSibling(), count++) 
        if (NOTR("attr") != node->name())
            continue;
    attrTable_->setColumnCount(count);
    set_vertheader(0, tr("Name"));
    set_vertheader(1, tr("Width"));
    set_vertheader(2, tr("Align"));
    set_vertheader(3, tr("Column Separator"));
    set_vertheader(4, tr("Row Separator"));
    
    QString str;
    node = prop->firstChild();
    for (count = 0; node; node = node->nextSibling(), count++) {
        if (NOTR("attr") != node->name())
            continue;
        str = node->getSafeProperty("colspec-num")->getString();
        if (str.isEmpty())
            str = QString::number(count + 1);
        attrTable_->setHorizontalHeaderItem(count, new QTableWidgetItem(str));
        
        str = node->getSafeProperty("colspec-name")->getString();
        if (str.isEmpty())
            str = QString(NOTR("c%1")).arg(count);
        QTableWidgetItem* item = new QTableWidgetItem(str);
        attrTable_->setItem(0, count, item);

        item = new QTableWidgetItem(
            node->getSafeProperty("colspec-width")->getString());
        attrTable_->setItem(1, count, item);

        item = new QTableWidgetItem(
            node->getSafeProperty("colspec-align")->getString());
        attrTable_->setItem(2, count, item);
        
        item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
        set_check_state(item, node, NOTR("colspec-colsep"));
        attrTable_->setItem(3, count, item);

        item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
        set_check_state(item, node, NOTR("colspec-rowsep"));
        attrTable_->setItem(4, count, item);

        if (node->getProperty("current"))
            attrTable_->setCurrentItem(item);
    }
    attrTable_->horizontalHeader()->
        resizeSections(QHeaderView::ResizeToContents);
    attrTable_->horizontalHeader()->
        setResizeMode(QHeaderView::Stretch);
    attrTable_->verticalHeader()->
        resizeSections(QHeaderView::ResizeToContents);
    attrTable_->verticalHeader()->
        setResizeMode(QHeaderView::Stretch);
    attrTable_->setSelectionMode(QTableView::NoSelection);
    attrTable_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    attrTable_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}                                              

PROPTREE_EVENT_IMPL(InsertTableCmd, DocumentPlugin)

bool InsertTableCmd::doExecute(DocumentPlugin* plugin, EventData* result)
{
    PropertyNode* prop = static_cast<PropertyTreeEventData*>(result)->root();
    TablePlugin* table_plugin = dynamic_cast<TablePlugin*>(plugin);
    InsertTableDialog dialog(table_plugin->sernaDoc()->widget(0), prop);
    if (table_plugin->checkDocbookChanges()) 
        dialog.titleCheck_->setText(
            tr("Has Title (if not checked it makes 'informaltable')"));
    if (is_genid(table_plugin))
        dialog.genIdCheck_->setChecked(true);
    if (QDialog::Accepted != dialog.exec())
        return false;
    prop->makeDescendant("columns")->setString(
        String::number(dialog.colsSpin_->value()));
    prop->makeDescendant("rows")->setString(
        String::number(dialog.rowsSpin_->value()));
    prop->makeDescendant("frame")->setString(dialog.frameType());

    if (dialog.titleCheck_->isChecked())
        prop->makeDescendant("hasTitle");
    if (dialog.headerCheck_->isChecked())
        prop->makeDescendant("hasHeader");
    if (dialog.footerCheck_->isChecked())
        prop->makeDescendant("hasFooter");
    if (dialog.separatorsCheck_->isChecked())
        prop->makeDescendant("hasColSeparator");
    if (dialog.spansCheck_->isChecked())
        prop->makeDescendant("pgwide");
    if (dialog.genIdCheck_->isChecked())
        prop->makeDescendant("generateId");
    return true;
}


PROPTREE_EVENT_IMPL(CallAttrsCmd, DocumentPlugin)

bool CallAttrsCmd::doExecute(DocumentPlugin* plugin, EventData* result)
{
    PropertyNode* prop = static_cast<PropertyTreeEventData*>(result)->root();
    TablePlugin* table_plugin = dynamic_cast<TablePlugin*>(plugin);
    return call_attr_dialog(table_plugin->sernaDoc()->widget(0), prop);
}

#include "moc/InsertTableDialog.moc"
