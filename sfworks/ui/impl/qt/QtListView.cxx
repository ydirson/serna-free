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
#include "ui/IconProvider.h"
#include "ui/UiAction.h"
#include "ui/impl/qt/QtListView.h"
#include "ui/impl/ui_debug.h"

#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QPixmap>
#include <QIcon>

using namespace Common;

static const char* CURRENT_ITEM = "#current-item";

namespace Sui {

class UiListViewItem : public QTreeWidgetItem {
public:
    UiListViewItem(Item* item, QTreeWidget* listView)
        : QTreeWidgetItem(listView),
          uiItem_(item) {};
    UiListViewItem(Item* item, QTreeWidget* listView, QTreeWidgetItem* after)
        : QTreeWidgetItem(listView, after),
          uiItem_(item) {};
    UiListViewItem(Item* item, QTreeWidgetItem* parent)
        : QTreeWidgetItem(parent, 0),
          uiItem_(item) {};
    UiListViewItem(Item* item, QTreeWidgetItem* parent, QTreeWidgetItem* after)
        : QTreeWidgetItem(parent, after),
          uiItem_(item) {};

    virtual ~UiListViewItem() {};

    Item*       uiItem() const { return uiItem_.pointer(); }

private:
    ItemPtr     uiItem_;
};

////////////////////////////////////////////////////////////////////////////

CUSTOM_ITEM_MAKER(ListView, QtListView)

/*
  ListView specific properties:

  col-specs
    column (+)  
      text
      icon
  #current-item

  acceptable children:
    ListViewItem
*/
QtListView::QtListView(Action*, PropertyNode* props)
    : Item(0, props),
      listView_(new QTreeWidget(0))
{
    PropertyNode* specs = itemProps()->getProperty("col-specs");
    if (!specs)
        return;
    QStringList header_labels;
    for (PropertyNode* c = specs->firstChild(); c; c = c->nextSibling()) {
        if ("column" != c->name())
            continue;
        QIcon icon_set;
        PropertyNode* pixmap_prop = c->getProperty("icon");
        if (pixmap_prop)
            icon_set = icon_provider().getIconSet(pixmap_prop->getString());
        header_labels.append(c->getSafeProperty("text")->getString());
    }
    listView_->setColumnCount(header_labels.count());
    listView_->setHeaderLabels(header_labels);
    
    listView_->setRootIsDecorated(true);
    //listView_->setAllColumnsShowFocus(true);
    //listView_->setResizeMode(QTreeWidget::AllColumns);
    connect(listView_, SIGNAL(currentChanged(QTreeWidgetItem*)),
            this, SLOT(currentChanged(QTreeWidgetItem*)));
    itemProps()->makeDescendant(CURRENT_ITEM)->setString(String());
}

void QtListView::currentChanged(QTreeWidgetItem* item)
{
    TreelocRep treeloc;
    UiListViewItem* ui_item = dynamic_cast<UiListViewItem*>(item);
    if (!ui_item)
        return;
    ui_item->uiItem()->treeLoc(treeloc, this);
    itemProps()->makeDescendant(CURRENT_ITEM)->setString(treeloc.toString());
}

QWidget* QtListView::widget(const Item*) const
{
    return listView_;
}

////////////////////////////////////////////////////////////////////////////

CUSTOM_ITEM_MAKER(ListViewItem, QtListViewItem)

/*
  ListViewItem specific properties:

  data
    column (+)  
      text
      icon
*/

QtListViewItem::QtListViewItem(Action*, PropertyNode* props)
    : Item(0, props),
      item_(0)
{
}

QtListViewItem::~QtListViewItem()
{
    DDBG << "~QtListViewItem: " << this << std::endl;
    delete item_;
}

QTreeWidgetItem* QtListViewItem::item() const
{
    return item_;
}

QTreeWidgetItem* QtListViewItem::prevSiblingItem() const
{
    for (Item* i = prevSibling(); i; i = i->prevSibling())
        if (LIST_VIEW_ITEM == i->itemClass())
            return static_cast<QtListViewItem*>(i)->item();
    return 0;
}

void QtListViewItem::setupItem()
{
    if (!item_)
        return;
    PropertyNode* data = itemProps()->getProperty("data");
    if (!data)
        return;
    int col_num = 0;
    for (PropertyNode* c = data->firstChild(); c; c = c->nextSibling()) {
        if ("column" != c->name())
            continue;
        item_->setText(col_num, c->getSafeProperty("text")->getString());
        PropertyNode* pixmap_prop = c->getProperty("icon");
        if (pixmap_prop)
            item_->setIcon(col_num, icon_provider().getPixmap(
                               pixmap_prop->getString()));
        col_num++;
    }
    //item_->setOpen(itemProps()->getSafeProperty("is-open")->getBool());
}

bool QtListViewItem::doAttach()
{
    if (item_) {
        delete item_;
        item_ = 0;
    }
    if (!parent())
        return false;
    QTreeWidgetItem* after = prevSiblingItem();
    if (LIST_VIEW == parent()->itemClass()) {
        QTreeWidget* view = dynamic_cast<QTreeWidget*>(parent()->widget(this));
        if (after)
            item_ = new UiListViewItem(this, view, after);
        else
            item_ = new UiListViewItem(this, view);
    }
    if (LIST_VIEW_ITEM == parent()->itemClass()) {
        QTreeWidgetItem* parent_item =
            static_cast<QtListViewItem*>(parent())->item();
        if (after)
            item_ = new UiListViewItem(this, parent_item, after);
        else
            item_ = new UiListViewItem(this, parent_item);
    }
    setupItem();
    return true;
}

bool QtListViewItem::doDetach()
{
    if (item_) {
        //item_->treeWidget()->takeTopLevelItem(item_);
    }
    return true;
}

void QtListViewItem::setItemVisible(bool isVisible)
{
    //if (item_)
    //    item_->setVisible(isVisible);
}

} // namespace Sui
