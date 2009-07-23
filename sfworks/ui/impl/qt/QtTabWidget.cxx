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
#include "ui/ui_defs.h"
#include "ui/IconProvider.h"
#include "ui/UiItem.h"
#include "ui/UiAction.h"
#include "ui/MainWindow.h"

#include "ui/impl/ui_debug.h"
#include "ui/impl/qt/QtTabWidget.h"
#include "common/ScopeGuard.h"

#include <QTabBar>
#include <QEvent>
#include <QContextMenuEvent>
#include <QApplication>
#include <QGridLayout>

using namespace Common;

Q_DECLARE_METATYPE(Sui::Item*);

namespace Sui {

COMMANDLESS_ITEM_MAKER(ToolBox, QtToolBox)

static void add_remove_watcher(const Item* item, const String& propName, 
                               PropertyNodeWatcher* watcher, bool isAdd)
{
    PropertyNode* prop = item->property(propName);
    if (prop) {
        if (isAdd)
            prop->addWatcher(watcher);
        else
            prop->removeWatcher(watcher);
    }
}

/////////////////////////////////////////////////////////////////////////////

QtToolBox::QtToolBox(PropertyNode* properties)
    : StackItem(0, properties),
      stackWidget_(0)
{
    QtToolWidget* tw = new QtToolWidget(this);
    tw->setObjectName(get(NAME));
    stackWidget_ = tw;
}

QtToolBox::~QtToolBox()
{
    delete stackWidget_;
}

///////////////////////////////////////////////////////////////////////////

QtToolWidget::QtToolWidget(StackItem* item)
    : QToolBox(0),
      StackWidget(item)
{
    QObject::connect(this, SIGNAL(currentChanged(int)),
                     SLOT(currentChanged(int)));
}

QWidget* QtToolWidget::widget(const Item* item) const
{
    ItemMap::const_iterator i = itemMap_.find(item);
    if (i != itemMap_.end())
        return (*i).second;
    return 0;
}

void QtToolWidget::insertItem(const Item* item)
{
    DBG(UI.TAB) << "ToolWidget: insert item: " << item->get(NAME) << std::endl;
    DBG_IF(UI.DUMP) item->dump(4);

    QWidget* widget = new QWidget(0, QString(item->get(NAME)));
    itemMap_[item] = widget;

    add_remove_watcher(item, INSCRIPTION, this, true);
    add_remove_watcher(item, TOOLTIP, this, true);
    add_remove_watcher(item, ICON, this, true);
    
    addItem(widget, icon_provider().getIconSet(item->get(ICON)),
            item->getTranslated(INSCRIPTION));
    setItemToolTip(indexOf(widget), item->getTranslated(TOOLTIP));
    setCurrentItem(widget);
}

void QtToolWidget::removeItem(const Item* item)
{
    DBG(UI.TAB) << "ToolWidget: remove item " << item->get(NAME) << std::endl;
    DBG_IF(UI.DUMP) item->dump(4);

    ItemMap::iterator i = itemMap_.find(item);
    if (i != itemMap_.end()) {
        add_remove_watcher(item, INSCRIPTION, this, false);
        add_remove_watcher(item, TOOLTIP, this, false);
        add_remove_watcher(item, ICON, this, false);
        QToolBox::removeItem((*i).second);
        itemMap_.erase(itemMap_.find(item));
        if (0 == count())
            StackWidget::currentChanged(0);
    }
}

void QtToolWidget::propertyChanged(PropertyNode* prop)
{
    Item* tab_item = stackItem()->currentItem();
    tab_item = (tab_item) ?  tab_item->parent() : 0;
    if (!tab_item)
        return;
    for (Item* child = tab_item->firstChild(); child; 
         child = child->nextSibling()) {

        PropertyNode* ins = child->property(INSCRIPTION);
        PropertyNode* tip = child->property(TOOLTIP);
        PropertyNode* ico = child->property(ICON);
        if (prop == ins || prop == ico || prop == tip) {
            ItemMap::iterator i = itemMap_.find(child);
            if (i == itemMap_.end()) 
                return;
            int idx = indexOf((*i).second);
            if (prop == ins && itemLabel(idx) != ins->getString())
                setItemLabel(idx, ins->getString());
            if (prop == ico)
                setItemIconSet(idx, icon_provider().getIconSet(
                                   ico->getString()));
            if (prop == tip && itemToolTip(idx) != tip->getString())
                setItemToolTip(idx, tip->getString());
            return;
        }
    }
}

void QtToolWidget::setCurrent(Item* item)
{
    setCurrentItem(itemMap_[item]);
}

Item* QtToolWidget::itemAt(QWidget* page) const
{
    ItemMap::const_iterator item = itemMap_.begin();
    for (; item != itemMap_.end(); item++)
        if (page == (*item).second)
            return const_cast<Item*>((*item).first);
    return 0;
}

void QtToolWidget::currentChanged(int idx)
{
    DBG(UI.TAB) << "ToolWidget: current item changed to " << idx << std::endl;
    StackWidget::currentChanged(itemAt(item(idx)));
}

/////////////////////////////////////////////////////////////////////////////

COMMANDLESS_ITEM_MAKER(TabItem, QtTabItem)

QtTabItem::QtTabItem(PropertyNode* properties)
    : StackItem(0, properties),
      stackWidget_(0)
{
    UI_LOCK_PROP_GUARD
    properties->makeDescendant("tabs-on-top", "false", false);
    QtTabWidget* tw = new QtTabWidget(this, properties);
    tw->setObjectName(get(NAME));
    stackWidget_ = tw;
}

QtTabItem::~QtTabItem()
{
    delete stackWidget_;
}

///////////////////////////////////////////////////////////////////////////

QtTabWidget::QtTabWidget(StackItem* item, PropertyNode* properties)
    : QTabWidget(0),
      StackWidget(item)
{
    bool tabs_on_top = properties->getSafeProperty("tabs-on-top")->getBool();
    if (tabs_on_top)
        setTabPosition(QTabWidget::North);
    else
        setTabPosition(QTabWidget::South);
    QObject::connect(this, SIGNAL(currentChanged(int)),
                     this, SLOT(currentChanged(int)));
    setContentsMargins(0, 0, 0, 0);
    hide();
    new QTabContextMenuProvider(tabBar(), this);
}

QWidget* QtTabWidget::widget(const Item* item) const
{                                   
    int index = item->siblingIndex();
    return index ? QTabWidget::widget(index - 1) : 0;
}

void QtTabWidget::insertItem(const Item* item)
{
    DBG(UI.TAB) << "TabWidget: insert tab item: "
                << item->itemClass() << "/" << item->get(NAME) << std::endl;
    DBG_IF(UI.DUMP) item->dump(4);
    
    QWidget* widget = new QWidget(0, QString(item->get(NAME)));
    QGridLayout* layout = new QGridLayout(widget);
    layout->setMargin(0);

    add_remove_watcher(item, INSCRIPTION, this, true);
    add_remove_watcher(item, TOOLTIP, this, true);
    add_remove_watcher(item, ICON, this, true);
    
    int idx = addTab(widget, icon_provider().getIconSet(item->get(ICON)),
                     item->getTranslated(INSCRIPTION));
    widget->setContentsMargins(0, 0, 0, 0);
    tabBar()->setTabData(idx, qVariantFromValue(const_cast<Item*>(item)));
    setTabToolTip(widget, item->getTranslated(TOOLTIP));
    setCurrentIndex(idx);
    if (!isVisible())
        currentChanged(idx);
}

void QtTabWidget::removeItem(const Item* item)
{
    DBG(UI.TAB) << "TabWidget: remove item " 
                << item->itemClass() << "/" << item->get(NAME)
                << std::endl;
    DBG_IF(UI.DUMP) item->dump(4);
    int index = 0;
    int cnt = QTabWidget::count();
    for (; index < cnt; index++) 
        if (itemAt(index) == item)
            break;
    add_remove_watcher(item, INSCRIPTION, this, false);
    add_remove_watcher(item, TOOLTIP, this, false);
    add_remove_watcher(item, ICON, this, false);
    if (index < cnt)
        removeTab(index);
    if (0 == cnt)
        StackWidget::currentChanged(0);
}

void QtTabWidget::itemPropertyChanged(PropertyNode*)
{
    //TODO: Elaborate
    /*
    if (prop->name() == "tabs-on-top") {
        if (prop->getBool())
            setTabPosition(Qt::DockTop);
        else
            setTabPosition(Qt::DockBottom);
    }
    */
}

void QtTabWidget::propertyChanged(PropertyNode* prop)
{
    Item* tab_item = stackItem()->currentItem();
    tab_item = (tab_item) ?  tab_item->parent() : 0;
    if (!tab_item)
        return;
    Item* child = tab_item->firstChild();
    if (child)
        child = child->nextSibling();
    for (int index = 1; child; child = child->nextSibling(), ++index) {
        PropertyNode* ins = child->property(INSCRIPTION);
        PropertyNode* tip = child->property(TOOLTIP);
        PropertyNode* ico = child->property(ICON);
        if (prop == ins || prop == ico || prop == tip) {
            if (prop == ins && tabText(index) != ins->getString())
                setTabText(index, ins->getString());
            if (prop == ico)
                setTabIcon(index, icon_provider().getIconSet(
                               ico->getString()));
            if (prop == tip && tabToolTip(index) != tip->getString())
                setTabToolTip(index, tip->getString());
            return;
        }
    }
}

void QtTabWidget::setCurrent(Item* item)
{
    if (item->siblingIndex())
        setCurrentPage(item->siblingIndex() - 1);
}

Item* QtTabWidget::itemAt(int index) const
{
    return tabBar()->tabData(index).value<Item*>();
}

void QtTabWidget::currentChanged(int index)
{
    DBG(UI.TAB) << "TabWidget: currentChanged: index: " 
        << index << ", widget=" << QTabWidget::widget(index)
        << ", item=" << itemAt(index) << std::endl;
    StackWidget::currentChanged(itemAt(index));
}

void QtTabWidget::tabRemoved(int index)
{
    DBG(UI.TAB) << "TabWidget: tabRemoved: index=" << index 
        << ", widget=" << QTabWidget::widget(index) 
        << ", count = " << count() << std::endl;
    StackWidget::currentChanged(itemAt(index));
}

void QtTabWidget::showTabContextMenu(const QPoint& pos)
{
    stackItem()->currentItem()->showContextMenu(pos);
}

//////////////////////////////////////////////////////

QTabContextMenuProvider::QTabContextMenuProvider(QTabBar* tabBar, 
                                                 QObject* parent)
    : QObject(parent), tabBar_(tabBar)
{
    connect(this, SIGNAL(showTabContextMenu(const QPoint&)), parent,
        SLOT(showTabContextMenu(const QPoint&)));
    tabBar->setContextMenuPolicy(Qt::DefaultContextMenu);
    tabBar->installEventFilter(this);
}

bool QTabContextMenuProvider::eventFilter(QObject* receiver, QEvent* event)
{
    if (receiver != tabBar_ || event->type() != QEvent::ContextMenu)
         return false;
    QPoint pos(static_cast<const QContextMenuEvent*>(event)->globalPos());
    int i = 0;
    for (; i < tabBar_->count(); ++i) {
        QRect tab_rect(tabBar_->tabRect(i));
        QPoint r(tabBar_->mapToGlobal(tab_rect.topLeft()));
        if (pos.x() > r.x() && pos.x() < (r.x() + tab_rect.width()))
            break;
    }
    if (i >= tabBar_->count())
        return false;
    if (i != tabBar_->currentIndex()) 
        tabBar_->setCurrentIndex(i);
    emit showTabContextMenu(pos);
    return true;
}

} // namespace Sui

