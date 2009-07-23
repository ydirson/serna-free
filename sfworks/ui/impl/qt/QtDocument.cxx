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
#include "ui/ActionSet.h"
#include "ui/MainWindow.h"
#include "ui/MimeHandler.h"
#include "ui/impl/ui_debug.h"
#include "ui/impl/qt/QtDocument.h"
#include "ui/impl/qt/QtTabWidget.h"
#include "common/safecast.h"
#include "common/XList.h"
#include "common/StringTokenizer.h"

#include <QLayout>
#include <QTabBar>
#include <QEvent>
#include <QMainWindow>
#include <QApplication>
#include <QDropEvent>
#include <QIconSet>
#include <QUrl>
#include <QToolBar>
#include <QTextStream>
#include <QShortcut>

using namespace Common;

Q_DECLARE_METATYPE(Sui::Item*);

namespace Sui {

void QtDocument::add_propwatcher(const String& propname)
{
    PropertyNode* prop = document_->property(propname);
    if (prop)
        prop->addWatcher(this);
}

class DndTabBar : public QTabBar {
public:
    DndTabBar(QWidget* parent, QtDocument* qtdoc)
        : QTabBar(parent),
          qtdoc_(qtdoc) { setAcceptDrops(true); }
    virtual void dragEnterEvent(QDragEnterEvent* event) { event->accept(); }
    virtual void dragMoveEvent(QDragMoveEvent* event);

private:
    QtDocument* qtdoc_;
};

class MainWindowImpl : public QMainWindow {
public:
    MainWindowImpl(QWidget* parent, Document* doc);

    void    dragEnterEvent(QDragEnterEvent* event);
    void    dropEvent(QDropEvent*); 

private:
    Document* document_;
};

QtDocument::QtDocument(Document* doc)
    : StackWidget(SAFE_CAST(StackItem*, doc->Item::parent())),
      document_(doc)
{
    mainWindow_ = new MainWindowImpl(document_->parent()->widget(0), doc);
    mainWindow_->setWindowFlags(0);
    mainWindow_->setObjectName(NOTR("QMainWindow"));
    mainWindow_->setAcceptDrops(TRUE);
    const bool isHidden = stackItem()->itemProps()->
        getSafeProperty(NOTR("hidden"))->getBool();
    if (!isHidden && !mainWindow_->parentWidget()->isVisible())
        mainWindow_->parentWidget()->setVisible(true);
    mainWindow_->setVisible(isHidden);
    if (!doc->isVisual())
        return;
    centralWidget_ = new QWidget(mainWindow_, NOTR("MWCentralWidget"));
    mainWindow_->setCentralWidget(centralWidget_);
    mainWindow_->setDockOptions(mainWindow_->dockOptions() |
        QMainWindow::VerticalTabs);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget_);
    layout->setMargin(0);
    widget_ = new QWidget(centralWidget_, NOTR("MWDocPlaceWidget"));
    (new QHBoxLayout(widget_))->setMargin(0);
    layout->addWidget(widget_);

    tabBar_ = new DndTabBar(centralWidget_, this);
    tabBar_->setUsesScrollButtons(true);
    tabBar_->setElideMode(Qt::ElideRight);
    tabBar_->setFocusPolicy(Qt::NoFocus);
    QFont tab_font(tabBar_->font());
    tab_font.setBold(true);
    tabBar_->setFont(tab_font);
    tabBar_->setDrawBase(true);
    new QTabContextMenuProvider(tabBar_, this);
    tabBar_->setName(NOTR("DocumentTabBar"));
    updateTabBar(0);
    tabBar_->setShape(QTabBar::RoundedSouth);
    tabBar_->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
        QSizePolicy::Fixed));
    QHBoxLayout* hb_layout = new QHBoxLayout;
    layout->addLayout(hb_layout);
    hb_layout->setMargin(0);
    hb_layout->addWidget(tabBar_);
    hb_layout->addStretch(0);

    centralWidget_->setShown(true);
    connect(tabBar_, SIGNAL(currentChanged(int)),
        this, SLOT(tabSelected(int)));
    add_propwatcher(INSCRIPTION);
    add_propwatcher(TOOLTIP);
    add_propwatcher(ICON);
}

void DndTabBar::dragMoveEvent(QDragMoveEvent* event)
{
    event->ignore();
    int tab_index = tabAt(event->pos());
    if (tab_index < 0 || tab_index == currentIndex())
        return;
    setCurrentIndex(tab_index);
}

void QtDocument::setItemVisible(bool isVisible)
{
    mainWindow_->setShown(isVisible);
}

StackWidget* QtDocument::nextStackWidget() const
{
    Item* item = document_->Item::nextSibling();
    for (; item; item = item->Item::nextSibling()) {
        Document* d = dynamic_cast<Document*>(item);
        if (d)
            return d->stackWidget();
    }
    return 0;
}

QWidget* QtDocument::widget(const Item* item) const
{
    if (0 == item)
        return mainWindow_;
    return widget_;
}

QWidget* Document::widget(const Item* item) const
{
    return stackWidget() ? stackWidget()->widget(item) : 0;
}

void QtDocument::tabSelected(int index)
{
    StackWidget::currentChanged(tabBar_->tabData(index).value<Item*>());
}

void QtDocument::setCurrent(Item* item)
{
    if (!tabBar_)
        return;
    tabBar_->blockSignals(true);
    for (int i = 0; i < tabBar_->count(); ++i) {
        if (tabBar_->tabData(i).value<Item*>() == item) {
            tabBar_->setCurrentIndex(i);
            break;
        }
    }
    tabBar_->blockSignals(false);
}

void QtDocument::updateTabBar(const Item* newTab)
{
    if (!tabBar_)
        return;
    tabBar_->blockSignals(true);
    while (tabBar_->count())
        tabBar_->removeTab(0);
    Item* item = document_->Item::parent()->firstChild();
    int tab_index = 0;
    for (; item; item = item->nextSibling()) {
        Document* d = dynamic_cast<Document*>(item);
        if (0 == d || !d->isVisual())
            continue;
        tabBar_->addTab(icon_provider().getIconSet(d->get(ICON)),
            d->getTranslated(INSCRIPTION));
        tabBar_->setTabData(tab_index, qVariantFromValue(item));
        tabBar_->setTabToolTip(tab_index, d->getTranslated(TOOLTIP));
        ++tab_index;
    }
    if (newTab)
        setCurrent(const_cast<Item*>(newTab));
    tabBar_->blockSignals(false);
    tabBar_->updateGeometry();
}

void QtDocument::propertyChanged(PropertyNode* prop)
{
    DBG(UI.WIDGET) << "QtDocument: propChanged=" << prop->name() << std::endl;
    stackItem()->updateWidgetProperties(document_, prop);
}

void QtDocument::updateProperty(const Item* item,
                                const PropertyNode* prop)
{
    if (!tabBar_)
        return;
    int tab_index = -1;
    for (int i = 0; i < tabBar_->count(); ++i)
        if (tabBar_->tabData(i).value<Item*>() == item) {
            tab_index = i;
            break;
    }
    if (tab_index < 0)
        return;
    if (INSCRIPTION == prop->name())
        tabBar_->setTabText(tab_index, item->getTranslated(INSCRIPTION));
    else if (ICON == prop->name())
        tabBar_->setTabIcon(tab_index,
            icon_provider().getIconSet(item->get(ICON)));
    else if (TOOLTIP == prop->name())
        tabBar_->setTabToolTip(tab_index, item->get(TOOLTIP));
}

void QtDocument::insertItem(const Item* newTab)
{
    updateTabBar(newTab);
}

void QtDocument::removeItem(const Item* newTab)
{
    updateTabBar(newTab);
}

void QtDocument::showTabContextMenu(const QPoint& pos)
{
    stackItem()->currentItem()->showContextMenu(pos);
}
void QtDocument::restoreDockingState()
{
    if (document_->isLockedNotify())
        return;
    PropertyNode* dstate_prop =
        document_->itemProps()->getProperty("docking_state");
    if (0 == dstate_prop)
        return;
    QByteArray state;
    QDataStream stream(&state, QIODevice::WriteOnly);
    StringTokenizer st_data(dstate_prop->getSafeProperty("data")->getString());
    while (st_data)
        stream << (uchar) st_data.next().toUInt();
    mainWindow_->restoreState(state, 0);
}

void QtDocument::saveDockingState()
{
    QByteArray state = mainWindow_->saveState(0);
    if (state.isEmpty())
        return;
    QString dock_state;
    QTextStream stream(&dock_state, QIODevice::WriteOnly);
    const unsigned int stateSize = state.size() - 1;
    dock_state.reserve(3 * stateSize);
    for (unsigned int i = 0; i < stateSize; ++i)
        stream << uchar(state.at(i)) << ' ';
    stream << uchar(state.at(stateSize));
    bool notify_lock = document_->isLockedNotify();
    document_->setLockedNotify(true);
    PropertyNode* save_prop =
        document_->itemProps()->makeDescendant("docking_state");
    save_prop->makeDescendant("data", dock_state, true);
    document_->setLockedNotify(notify_lock);
}

void MainWindowImpl::dragEnterEvent(QDragEnterEvent* e)
{
    document_->mimeHandler().dragEnter(e);
    document_->mimeHandler().callback(MimeCallback::VALIDATE_CONTENT, e);
}

void MainWindowImpl::dropEvent(QDropEvent* e)
{
    if (document_->mimeHandler().callback(MimeCallback::VALIDATE_CONTENT, e))
        document_->mimeHandler().callback(MimeCallback::DROP_CONTENT, e); 
}

static const char* URLLIST_MT = NOTR("text/uri-list");

MIMETYPE_CALLBACK_DECL(UiDocValidate, VALIDATE_CONTENT, URLLIST_MT, /**/)

bool UiDocValidate::execute(MimeHandler& mh, QDropEvent*) 
{
    return mh.mimeData()->hasUrls();
}

MIMETYPE_CALLBACK_DECL(UiDocDrop, DROP_CONTENT, URLLIST_MT, Document* doc_;)

bool UiDocDrop::execute(MimeHandler& mh, QDropEvent*) 
{
    QList<QUrl> url_list = mh.mimeData()->urls();
    DBG(UI.DRAG) << "DND: UiDoc drop: nUrls=" << url_list.count() << std::endl;
    for (int i = 0; i < url_list.size(); ++i) {
        QString url = url_list[i].toString();
        if (url.startsWith("/\\\\"))
            url = url.mid(1);
        SAFE_CAST(Document*, doc_->parent()->firstChild())->openUrl(url);
    }
    return true;
}

MainWindowImpl::MainWindowImpl(QWidget* parent, Document* doc)
    : QMainWindow(parent), document_(doc)
{
    doc->mimeHandler().registerCallback(new UiDocValidate, 90);
    UiDocDrop* udd = new UiDocDrop;
    udd->doc_ = doc;
    doc->mimeHandler().registerCallback(udd, 90);
}

//////////////////////////////////////////////////////////////////

QtDocument::~QtDocument()
{
    delete mainWindow_;
}

void Document::saveDockingState()
{
    static_cast<QtDocument*>(stackWidget_.pointer())->saveDockingState();
}

void Document::restoreDockingState()
{
    static_cast<QtDocument*>(stackWidget_.pointer())->restoreDockingState();
}

void Document::makeStackWidget()
{
    stackWidget_ = new QtDocument(this);
}

void Document::setStackWidget(StackWidget* stackWidget)
{
    stackWidget_ = stackWidget;
}

} // namespace
