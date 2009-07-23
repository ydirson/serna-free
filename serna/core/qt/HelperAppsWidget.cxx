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

#include "core/qt/FileHandlerDialog.h"
#include "core/HelperAppsWidgetBase.hpp"
#include "core/Preferences.h"
#include "utils/Properties.h"
#include "common/PathName.h"

#include <QFileInfo>
#include <QMessageBox>

using namespace Common;

class HelperAppsWidget : public QWidget,
                         public Ui::HelperAppsWidgetBase {
    Q_OBJECT
public:
    HelperAppsWidget(Common::PropertyNode* prop);

    virtual void     editFileHandler(QTreeWidgetItem* item);

protected slots:
    virtual void     on_addExtensionButton__clicked() { addFileHandlerHelper();}
    virtual void     on_editExtensionButton__clicked();
    virtual void     on_removeExtensionButton__clicked();
    virtual void     on_appListView__itemDoubleClicked(QTreeWidgetItem*, int);
    virtual void     on_appListView__itemSelectionChanged();

private:
    void addFileHandlerHelper(Common::PropertyNode* node = 0);
    Common::PropertyNode*   handlers_;
};

REGISTER_BUILTIN_PREFERENCES_TAB(HelperApps, 4, true)

class FileHandlerItem : public QTreeWidgetItem {
public:
    FileHandlerItem(PropertyNode* handler, QTreeWidget* listView)
        : handler_(handler) 
    {
        setHandler(handler);
        listView->addTopLevelItem(this);
    }
    void setHandler(PropertyNode* handler)
    {
        handler_ = handler;
        setText(0, handler->makeDescendant(FileHandler::APP_EXT)->getString());
        setText(1, handler->makeDescendant(FileHandler::APP_PATH)->getString());
    }
public:
    PropertyNode*   handler_;
};

//////////////////////////////////////////////////////////////////////////

HelperAppsWidget::HelperAppsWidget(PropertyNode* props)
    : handlers_(props)
{
    setupUi(this);
    PropertyNode* handler = props->makeDescendant(FileHandler::HANDLER_LIST);
    for (PropertyNode* n = handler->firstChild(); n; n = n->nextSibling())
        if (FileHandler::APP == n->name())
            new FileHandlerItem(n, appListView_);
    handler = handlers_->makeDescendant(FileHandler::HANDLER_LIST_TEMP);
    for (PropertyNode* n = handler->firstChild(); n; n = n->nextSibling())
        if (FileHandler::APP == n->name() && !appListView_->findItems(
            n->getSafeProperty(FileHandler::APP_EXT)->getString(), 
            Qt::MatchExactly).size())
            new FileHandlerItem(n, appListView_);

    if (appListView_->topLevelItemCount()) {
        appListView_->setItemSelected(appListView_->topLevelItem(0), true);
        appListView_->setCurrentItem(appListView_->topLevelItem(0));
    }
    on_appListView__itemSelectionChanged();
}

void HelperAppsWidget::on_appListView__itemSelectionChanged()
{
    QTreeWidgetItem* item = appListView_->currentItem();
    removeExtensionButton_->setEnabled(item);
    editExtensionButton_->setEnabled(item);
}

void HelperAppsWidget::addFileHandlerHelper(PropertyNode* node)
{
    PropertyNodePtr filehandler;
    if (node)
        filehandler = node;
    else
        filehandler = new PropertyNode(FileHandler::APP);
    FileHandlerDialog dialog(filehandler.pointer(), this);
    if (dialog.exec() == QDialog::Accepted) {
        String name =
               filehandler->makeDescendant(FileHandler::APP_EXT)->getString();
        while ('.' == name[0])
            name = name.mid(1);
        if (appListView_->findItems(name, Qt::MatchExactly).size()) {
            QMessageBox::warning(this, tr("Warning"),
                         tr("Map for this extension already exists"));
            addFileHandlerHelper(filehandler.pointer());
            return;
        }
        PropertyNode* hnd  = new PropertyNode(FileHandler::APP);
        hnd->appendChild(new PropertyNode(FileHandler::APP_EXT,name));
        hnd->appendChild(new PropertyNode(FileHandler::APP_PATH,
            filehandler->makeDescendant(FileHandler::APP_PATH)->getString()));
        handlers_->makeDescendant(FileHandler::HANDLER_LIST)->appendChild(hnd);
        FileHandlerItem* item = new FileHandlerItem(hnd, appListView_);
        QList<QTreeWidgetItem*> sel_items = appListView_->selectedItems();
        if (sel_items.size())
            appListView_->setItemSelected(sel_items[0], false);
        appListView_->setItemSelected(item, true);
        appListView_->setCurrentItem(item);
        on_appListView__itemSelectionChanged();
    }
}

void HelperAppsWidget::on_removeExtensionButton__clicked()
{
    if (appListView_->currentItem()) {
        FileHandlerItem* item =
            static_cast<FileHandlerItem*>(appListView_->currentItem());
        item->handler_->remove();
        delete item;
    }
}

void 
HelperAppsWidget::on_appListView__itemDoubleClicked(QTreeWidgetItem* item, int)
{
    editFileHandler(item);
}

void HelperAppsWidget::on_editExtensionButton__clicked()
{
    if (appListView_->currentItem()) 
        editFileHandler(appListView_->currentItem());
}

void HelperAppsWidget::editFileHandler(QTreeWidgetItem* item)
{
    if (!item)
        return;
    FileHandlerItem* hnd_item = static_cast<FileHandlerItem*>(item);
    PropertyNode* filehandler = hnd_item->handler_;
    PropertyNodePtr filehandler_copy = filehandler->copy(true);
    FileHandlerDialog dialog(filehandler_copy.pointer(), this);
    if (dialog.exec() == QDialog::Accepted) {
        String name = filehandler_copy->
                      makeDescendant(FileHandler::APP_EXT)->getString();
        while ('.' == name[0])
            name = name.mid(1);
        QList<QTreeWidgetItem*> items =
            appListView_->findItems(name, Qt::MatchExactly);
        QTreeWidgetItem* new_item = items.size() ? items[0] : 0;
        if (0 != new_item && item != new_item) {
            QMessageBox::warning(this, tr("Warning"),
                         tr("Map for this extension already exists"));
            editFileHandler(item);
            return;
        }
        PropertyNode* hnd  = new PropertyNode(FileHandler::APP);
        hnd->appendChild(new PropertyNode(FileHandler::APP_EXT,name));
        hnd->appendChild(new PropertyNode(FileHandler::APP_PATH,
                         filehandler_copy->makeDescendant(
                         FileHandler::APP_PATH)->getString()));
        filehandler->remove();
        handlers_->makeDescendant(FileHandler::HANDLER_LIST)->appendChild(hnd);
        hnd_item->setHandler(hnd);
    }
}

#include "moc/HelperAppsWidget.moc"
