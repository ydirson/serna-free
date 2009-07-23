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
#include "common/XList.h"
#include "common/StringTokenizer.h"

#include "ui/ui_defs.h"
#include "ui/IconProvider.h"
#include "ui/UiDocument.h"
#include "ui/LiquidItem.h"

#include "ui/impl/ui_debug.h"
#include "ui/impl/qt/QtTabWidget.h"
#include "ui/impl/qt/QtMainWindow.h"

#include <QPixmap>
#include <QApplication>
#include <QEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDesktopWidget>
#include <QUrl>
#include <QStatusBar>
#include <QDockWidget>
#include <QStackedWidget>
#include <QList>

using namespace Common;

namespace Sui {
/*
class QtStackedWidget : public QStackedWidget, public QAxBindable {
    Q_OBJECT
public:
    QtStackedWidget(QWidget* parent = 0) : QStackedWidget(parent) {}
};
*/
const char MainWindow::CLOSE_CMD_NAME[]    = "closeWindow";

MainWindow* MainWindow::make(int x, int y, int width, int height,
                             const String& name, bool isBottomTabs,
                             QWidget* parent)
{
    PropertyNodePtr props = new PropertyNode("properties");
    props->makeDescendant(NAME)->setString(name);
    props->makeDescendant("tab-position")->setString(
        (isBottomTabs) ? "bottom" : "top");
    return new QtMainWindow(x, y, width, height, props.pointer(), parent);
}

QtMainWindow::QtMainWindow(int x, int y,
                           int width, int height,
                           PropertyNode* properties, QWidget* parent)
    : MainWindow(0, properties),
      widgetStack_(0)
{
    static int wstack_seq = 0;
    widgetStack_ = new QStackedWidget(parent);
    widgetStack_->setName(make_seqid(wstack_seq, NOTR("MainWidgetStack")));
    if (!parent  && (0 == width || 0 == height)) {
        QRect geom = QApplication::desktop()->screenGeometry();
        width = int(geom.width() * 0.8);
        height = int(geom.height() * 0.8);
        x = geom.topLeft().x() + int(width * 0.1);
        y = geom.topLeft().y() + int(height * 0.1);
    }
    widgetStack_->move(x, y);
    widgetStack_->resize(width, height);
    widgetStack_->installEventFilter(this);
}

QtMainWindow::~QtMainWindow()
{
    if (widgetStack_)
        delete widgetStack_;
}

QWidget* QtMainWindow::widget(const Item* child) const
{
    if (child) {
        const Item* parent = child->parent();
        if (parent == this)
            return StackItem::widget(child);
    }
    return widgetStack_;
}

StackWidget* QtMainWindow::stackWidget() const
{
    Item* item = firstChild();
    for (; item; item = item->nextSibling()) {
        Document* d = dynamic_cast<Document*>(item);
        if (d)
            return d->stackWidget();
    }
    return 0;
}

void QtMainWindow::setIcon(const String& icon)
{
    widgetStack_->setIcon(icon_provider().getPixmap(icon));
}

void QtMainWindow::setCaption(const String& caption)
{
    widgetStack_->setCaption(caption);
    widgetStack_->setWindowTitle(caption);
}

void QtMainWindow::updateWidgetProperties(const Item* item,
                                          const PropertyNode* prop)
{
    if (INSCRIPTION == prop->name())
        set_caption(item);
    StackItem::updateWidgetProperties(item, prop);
}

bool QtMainWindow::eventFilter(QObject* watched, QEvent* e)
{
    if (widgetStack_ != watched || e->type() != QEvent::Close)
        return false;
    if (QApplication::overrideCursor())
        return false;
    e->ignore();  // needed for qt4 to avoid getting this twice
    if (firstChild()) {
        Action* action = firstChild()->findAction(CLOSE_CMD_NAME);
        if (action)
            dispatch(action);
    }
    return true;
}

void QtMainWindow::set_caption(const Item* item)
{
    String orig_caption = get("#caption");
    if (orig_caption.isEmpty() || !item)
        return;
    String inscription = item->get(INSCRIPTION);
    if (inscription.isEmpty())
        setCaption(orig_caption);
    else
        setCaption(orig_caption + NOTR(" - ") + inscription);
}

#ifdef __APPLE__
// on Mac, need to reattach menubar to update 'top' screen menu
static void update_menubar(Item* item, bool attach)
{
    extern bool is_mac_app_menu;
    if (!is_mac_app_menu)
        return;
    for (item = item->firstChild(); item; item = item->nextSibling())
        if (item->itemClass() == MAIN_MENU)
            break;
    if (0 == item)
        return;
    if (attach) {
	item->detach(false);
        item->attach(true);
    } else
        item->detach(false);
}
#endif // __APPLE__

void QtMainWindow::currentChanged(Item* prev, Item* curr)
{
    DBG_IF(UI.WINDOW) {
        DBG(UI.WINDOW) << "MainWindow: current document changed: ";
        if (prev)
            DBG(UI.WINDOW) << prev << "(" << prev->get(INSCRIPTION)
                << ") -> ";
        if (curr)
            DBG(UI.WINDOW) << curr << "(" << curr->get(INSCRIPTION)
                << ")\n";
    }
    Document* doc = dynamic_cast<Document*>(curr);
    if (!doc)
        return;
#ifdef __APPLE__
    if (prev && curr) {
        update_menubar(prev, false);
        update_menubar(curr, true);
    }
#endif
    if (doc->getBool(NOTR("#just-inserted"))) {
    	doc->attach(true);
        doc->restoreDockingState();
        doc->setBool("#just-inserted", false);
    }
    QWidget* current_widget = doc->stackWidget()->widget(0);
    setCaption(current_widget->caption());
    widgetStack_->setCurrentWidget(current_widget);
    if (prev) {
        Item* ci = 0;
        for (ci = prev->firstChild(); ci; ci = ci->nextSibling()) {
            LiquidItem* li = dynamic_cast<LiquidItem*>(ci);
            if (li)
                li->preserveVisibleState(false);
        }
        for (ci = curr->firstChild(); ci; ci = ci->nextSibling()) {
             LiquidItem* li = dynamic_cast<LiquidItem*>(ci);
            if (li)
                li->preserveVisibleState(true);
        }
    }
    set_caption(curr);
}

void QtMainWindow::childInserted(Item* item)
{
    DBG(UI.WINDOW) << "QtMainWindow:: childInserted: " << item->itemClass()
        << "(" << item << "/" << currentItem() << ")\n";
    Document* doc = dynamic_cast<Document*>(item);
    if (0 == doc) {
        DBG(UI.WINDOW)
            << "MainWindow: only document items are allowed" << std::endl;
        item->removeItem();
        return;
    }
    doc->makeStackWidget();
    QWidget* widget = doc->stackWidget()->widget(0);
    if (!item->prevSibling()) {
        doc->attach(true);
        doc->restoreDockingState();
    } else
        doc->setBool("#just-inserted", true);
    widgetStack_->addWidget(widget);
    StackItem::childInserted(doc);
}

void QtMainWindow::childRemoved(Item* item)
{
    DBG(UI.WINDOW) << "QtMainWindow:: childRemoved: " << item->itemClass()
        << std::endl;
    if (1 == countChildren())
        setCaption(get(NOTR("#caption"))); // reset original caption
    Document* doc = dynamic_cast<Document*>(item);
    RT_ASSERT(doc);
    if (!doc)
        return;
    doc->detach(true);
    widgetStack_->removeWidget(doc->stackWidget()->widget(0));
    StackItem::childRemoved(item);
}

} // namepsace
