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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "core/FileTypeWidgetBase.hpp"
#include "core/qt/FileTypeDialog.h"
#include "core/Preferences.h"

#include "utils/Properties.h"

#include "common/StringCmp.h"
#include "common/StringCvt.h"
#include "common/PropertyTree.h"

#include <QHeaderView>
#include <QMessageBox>
#include <algorithm>

using namespace Common;
using namespace SernaFileType;

class FileTypeWidget : public QWidget,
                       public Ui::FileTypeWidgetBase {
    Q_OBJECT
public:
    FileTypeWidget(Common::PropertyNode* props);

protected:
    void            editFileType(QTreeWidgetItem*);

protected slots:
    virtual void on_insertBeforeButton__clicked() { addFileType(true); }
    virtual void on_insertAfterButton__clicked()  { addFileType(false);}
    virtual void on_delButton__clicked();
    virtual void on_editButton__clicked();

    virtual void on_fileTypeListView__itemSelectionChanged();
    virtual void on_fileTypeListView__itemDoubleClicked(QTreeWidgetItem*, int);

private:
    void                    addFileType(bool before);
    Common::PropertyNode*   fileTypes_;
};

REGISTER_BUILTIN_PREFERENCES_TAB(FileType, 2, true)

class FileTypeItem : public QTreeWidgetItem {
public:
    FileTypeItem(PropertyNode* fileType)
    {
        setItem(fileType);
    }
    void setItem(PropertyNode* fileType)
    {
        fileType_ = fileType;
        setText(0,
            fileType->makeDescendant(SernaFileType::PATTERN)->getString());
        setText(1,
            fileType->makeDescendant(DESCRIPTION)->getString());
    }
    PropertyNode* property() { return fileType_; }

public:
    PropertyNode*   fileType_;
};

//////////////////////////////////////////////////////////////////////////

FileTypeWidget::FileTypeWidget(PropertyNode* props)
    : fileTypes_(props->makeDescendant(TYPE_LIST))
{
    setupUi(this);
    fileTypeListView_->header()->setResizeMode(QHeaderView::Stretch);
    for (PropertyNode* n = fileTypes_->firstChild(); n; n = n->nextSibling()) {
        if (TYPE == n->name()) 
            fileTypeListView_->addTopLevelItem(new FileTypeItem(n));
    }
    if (fileTypeListView_->topLevelItemCount()) {
        QTreeWidgetItem* item = fileTypeListView_->topLevelItem(0);
        fileTypeListView_->setItemSelected(item, true);
        fileTypeListView_->setCurrentItem(item);
    }
    on_fileTypeListView__itemSelectionChanged();
}

void FileTypeWidget::on_fileTypeListView__itemSelectionChanged()
{
    QTreeWidgetItem* item = fileTypeListView_->currentItem();
    delButton_->setEnabled(item);
    editButton_->setEnabled(item);
}

void FileTypeWidget::addFileType(bool before)
{
    PropertyNodePtr filetype = new PropertyNode(TYPE);
    FileTypeDialog dialog(filetype.pointer(), this);
    if (dialog.exec() != QDialog::Accepted) 
        return;
    FileTypeItem* cur =
        dynamic_cast<FileTypeItem*>(fileTypeListView_->currentItem());
    if (cur && cur->property())
        before ? cur->property()->insertBefore(filetype.pointer())
               : (cur->property()->nextSibling()
                     ? cur->property()->nextSibling()->
                       insertBefore(filetype.pointer())
                     : fileTypes_->appendChild(filetype.pointer()));
    else
        fileTypes_->appendChild(filetype.pointer());
    FileTypeItem* item = new FileTypeItem(filetype.pointer());
    int curIndex = fileTypeListView_->indexOfTopLevelItem(cur);
    fileTypeListView_->setItemSelected(cur, false);
    if (cur) {
        if (before) 
            fileTypeListView_->insertTopLevelItem(curIndex, item);
        else {
            if (curIndex >= fileTypeListView_->topLevelItemCount())
                fileTypeListView_->addTopLevelItem(item);
            else
                fileTypeListView_->insertTopLevelItem(curIndex + 1, item);
        }
    } else
        fileTypeListView_->addTopLevelItem(item);
    fileTypeListView_->setItemSelected(item, true);
    fileTypeListView_->setCurrentItem(item);
    on_fileTypeListView__itemSelectionChanged();
}

void FileTypeWidget::on_delButton__clicked()
{
    if (fileTypeListView_->currentItem()) {
        FileTypeItem* item =
            static_cast<FileTypeItem*>(fileTypeListView_->currentItem());
        item->property()->remove();
        delete item;
    }
}

void FileTypeWidget::on_editButton__clicked()
{
    editFileType(fileTypeListView_->currentItem());
}

void 
FileTypeWidget::on_fileTypeListView__itemDoubleClicked(QTreeWidgetItem* item,
                                                       int)
{
    editFileType(item);
}

void FileTypeWidget::editFileType(QTreeWidgetItem* item)
{
    if (!item)
        return;
    PropertyNode* filetype = static_cast<FileTypeItem*>(item)->property();
    PropertyNodePtr filetype_copy = filetype->copy(true);
    FileTypeDialog dialog(filetype_copy.pointer(), this);
    if (dialog.exec() == QDialog::Accepted) {
        String name = filetype_copy->
                      makeDescendant(SernaFileType::PATTERN)->getString();
        QList<QTreeWidgetItem*> items = 
            fileTypeListView_->findItems(name, Qt::MatchExactly, 0);
        QTreeWidgetItem* new_item = items.size() ? items[0] : 0;
        if (0 != new_item && item != new_item) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Map for this pattern already exists"));
            editFileType(item);
            return;
        }
        filetype->makeDescendant(SernaFileType::PATTERN)->setString(
            filetype_copy->makeDescendant(SernaFileType::PATTERN)->
            getString());
        filetype->makeDescendant(DESCRIPTION)->setString(
            filetype_copy->makeDescendant(DESCRIPTION)->getString());
        static_cast<FileTypeItem*>(item)->setItem(filetype);
    }
}

#include "moc/FileTypeWidget.moc"
