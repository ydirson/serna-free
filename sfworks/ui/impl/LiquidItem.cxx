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
#include "ui/UiItems.h"
#include "ui/UiItemSearch.h"

#include "ui/UiDocument.h"
#include "ui/ActionSet.h"
#include "ui/LiquidItem.h"
#include "ui/MainWindow.h"
#include "ui/impl/ui_debug.h"
#include "common/ScopeGuard.h"

#include <QDockWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QPixmap>
#include <QLabel>
#include <QContextMenuEvent>
#include <QEvent>
#include <QTimer>
#include <QDesktopWidget>
#include <QApplication>
#include <QEventLoop>

using namespace Common;

namespace Sui 
{

class LiquidItemEventFilter : public QObject {
    Q_OBJECT
public:
    LiquidItemEventFilter(LiquidItem* item)
        : item_(item),
          isFocusIn_(false),
          focusLocked_(false) {}
    
    bool isFocusIn() const { return isFocusIn_; }
    void lockFocus(bool isLocked) { focusLocked_ = isLocked; }

private slots:
    void                floatingChanged(bool v) { item_->floatingChanged(v); }
    void                dockLocationChanged(Qt::DockWidgetArea area) 
        { item_->dockLocationChanged(area); }

private:
    bool eventFilter(QObject* receiver, QEvent* e);

    //! If receiver is descendant of qobject_ or is qobject_ itself
    bool isDescendant(QObject* receiver) const
    {
        QObject* widget = item_->widget_;
        for (QObject* obj = receiver; obj; obj = obj->parent())
            if (obj == widget)
                return true;
        return false;
    }
private:
    LiquidItem*         item_;
    bool                isFocusIn_;
    bool                focusLocked_;
};

bool LiquidItemEventFilter::eventFilter(QObject* receiver, QEvent* e)
{
    if (!item_->widget_)
        return false;
    switch (e->type()) {
        case QEvent::FocusIn:
            if (isDescendant(receiver)) {
                DBG(UI.WIDGET) << "LiquidItem<" << item_->get(NAME) 
                    << ">: focusIn\n";
                item_->widgetFocusChanged(true);
            }
            return false;
        case QEvent::FocusOut:
            if (isDescendant(receiver)) {
                DBG(UI.WIDGET) << "LiquidItem<" << item_->get(NAME) 
                    << ">: focusOut\n";
                item_->widgetFocusChanged(false);
            }
            return false;
        case QEvent::KeyPress: {
            QKeyEvent* ke = static_cast<QKeyEvent*>(e);
            if (ke->key() == Qt::Key_Escape && isDescendant(receiver)) {
                DBG(UI.WIDGET) << "LiquidItem: escapePressed\n";
                item_->escapePressed();
                return true;
            }
            return false;
        }
        case QEvent::Hide: {
            if (receiver == item_->dockWidget_) 
                item_->changeVisibilityState(false);
            return false;
        }
        case QEvent::Show: {
            if (receiver == item_->dockWidget_) 
                item_->changeVisibilityState(true);
            return false;
        }
        case QEvent::ContextMenu: {
            if (receiver != item_->widget_)
                return false;
            item_->showContextMenu(static_cast<QContextMenuEvent*>
                (e)->globalPos());
            return true;
        }
        default:
            return false;
    }
}

typedef const char* const exported_literal;

// START_IGNORE_LITERALS
exported_literal DOCK_EDGE      = "dock-edge";
exported_literal DOCK_TOP       = "dock-top";
exported_literal DOCK_BOTTOM    = "dock-bottom";
exported_literal DOCK_LEFT      = "dock-left";
exported_literal DOCK_RIGHT     = "dock-right";
exported_literal DOCK_UNDOCKED  = "undocked";
exported_literal DOCK_GEOMETRY  = "dock-geometry";
exported_literal UNDOCKED_POS   = "undocked-pos";
// STOP_IGNORE_LITERALS

//////////////////////////////////////////////////////////////////////

static exported_literal dock_edge(Qt::DockWidgetArea area)
{
    switch (area) {
        case Qt::TopDockWidgetArea:
            return DOCK_TOP;
        case Qt::BottomDockWidgetArea:
            return DOCK_BOTTOM;
        case Qt::LeftDockWidgetArea:
            return DOCK_LEFT;
        case Qt::RightDockWidgetArea:
            return DOCK_RIGHT;
        default:
            return DOCK_UNDOCKED;
    };
    return DOCK_UNDOCKED;
}

static Qt::DockWidgetArea dock_area(const String& dockEdge)
{
    if (DOCK_BOTTOM == dockEdge)
        return Qt::BottomDockWidgetArea;
    if (DOCK_LEFT == dockEdge)
        return Qt::LeftDockWidgetArea;
    if (DOCK_RIGHT == dockEdge)
        return Qt::RightDockWidgetArea;
    if (DOCK_TOP == dockEdge)
        return Qt::TopDockWidgetArea;
    return Qt::NoDockWidgetArea;
}

static Qt::DockWidgetArea dock_area(LiquidItem* item)
{
    PropertyNode* dock_edge = item->itemProps()->getProperty(DOCK_EDGE);
    if (0 == dock_edge)
        return Qt::DockWidgetArea(0);
    return dock_area(dock_edge->getString());
}

static LiquidItem::Type tool_type(LiquidItem* item)
{
    PropertyNode* dock_edge = item->itemProps()->getProperty(DOCK_EDGE);
    if (0 == dock_edge)
        return LiquidItem::UNDOCKED_TOOL;
    
    if (DOCK_TOP == dock_edge->getString() ||
        DOCK_BOTTOM == dock_edge->getString())
        return LiquidItem::HORIZONTAL_TOOL;
    if (DOCK_LEFT == dock_edge->getString() ||
        DOCK_RIGHT == dock_edge->getString())
        return LiquidItem::VERTICAL_TOOL;

    return LiquidItem::UNDOCKED_TOOL;
}

static bool is_horizontal(const String& s)
{
    return s == DOCK_TOP || s == DOCK_BOTTOM;
}

static bool is_undocked(const String& s)
{
    return s == DOCK_UNDOCKED;
}

bool is_same_orientation(const String& oldEdge, const String& newEdge)
{
    if (newEdge == oldEdge)
        return true;
    if (is_horizontal(oldEdge) == is_horizontal(newEdge) &&
        is_undocked(oldEdge) == is_undocked(newEdge))
        return true;
    return false;
}

static void set_dock_title(const Item* item, QDockWidget* dw)
{
    String title = item->getTranslated(INSCRIPTION);
    if (title.isEmpty())
        title = QObject::tr("Serna");
    dw->setWindowTitle(title);
}

//////////////////////////////////////////////////////////////////////

class DockActionMap : public std::map<String, String> {};

LiquidItem::LiquidItem(PropertyNode* properties)
    : Item(0, properties),
      dockWidget_(0),
      dockActionMap_(new DockActionMap)
{
    UI_LOCK_PROP_GUARD

    // START_IGNORE_LITERALS
    (*dockActionMap_)[DOCK_TOP]      = "dockTop";
    (*dockActionMap_)[DOCK_BOTTOM]   = "dockBottom";
    (*dockActionMap_)[DOCK_LEFT]     = "dockLeft";
    (*dockActionMap_)[DOCK_RIGHT]    = "dockRight";
    (*dockActionMap_)[DOCK_UNDOCKED] = "undock";
    // STOP_IGNORE_LITERALS

    itemProps()->makeDescendant(DOCK_EDGE, DOCK_UNDOCKED, false);
    PropertyNode* dock_edge_specs = itemProps()->makeDescendant(
        ITEM_PROP_SPECS + String('/') + DOCK_EDGE);
    dock_edge_specs->appendChild(new PropertyNode(DOCK_TOP));
    dock_edge_specs->appendChild(new PropertyNode(DOCK_BOTTOM));
    dock_edge_specs->appendChild(new PropertyNode(DOCK_LEFT));
    dock_edge_specs->appendChild(new PropertyNode(DOCK_RIGHT));
    dock_edge_specs->appendChild(new PropertyNode(DOCK_UNDOCKED));

    eventFilter_ = new LiquidItemEventFilter(this);
    qApp->installEventFilter(eventFilter_);
}

LiquidItem::~LiquidItem()
{
    delete eventFilter_;
    if (!widget_.isNull()) {
        widget_->blockSignals(true);
        widget_->deleteLater();
    }
    if (!dockWidget_.isNull()) {
        if (mainWindow())
            mainWindow()->removeDockWidget(dockWidget_);
        dockWidget_->deleteLater();
    }
}

QWidget* LiquidItem::widget(const Item*) const
{
    return widget_;
}

void LiquidItem::grabFocus() const
{
    if (!widget_)
        return;
#if defined(__linux__)
    static const int force_cnt = 300;
#else // __linux__
    static const int force_cnt = 2;
#endif // __linux__
    QWidget* w = dockWidget_;
    if (!w)
        w = widget_;
    w->show();
    w->raise();
    for (int i = 0; i < force_cnt; ++i) {
        w->activateWindow();
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    QMetaObject::invokeMethod(widget_, NOTR("grabFocus"), Qt::DirectConnection);
    DBG(UI.WIDGET) << "LiquidItem: grabFocus\n";
}

void LiquidItem::changeVisibilityState(bool isVisible)
{
    UI_LOCK_PROP_GUARD
    DBG(UI.WIDGET) << "LiquidItem<" << get(NAME) << ">: changeVisibility="
        << isVisible << std::endl;
    setBool(IS_VISIBLE, isVisible);
    visibilityChanged(isVisible);
    if (!isVisible)
        releaseFocus();
}

void LiquidItem::propertyChanged(PropertyNode* prop)
{
    if (isLockedNotify()) 
        return;
    DBG(UI.WIDGET) << "LiquidItem<" << get(NAME) 
        << ">: propChanged=" << prop->name() << std::endl;
    Item::propertyChanged(prop);
    if (dockWidget_.isNull() || !prop)
        return;
    if (INSCRIPTION == prop->name()) {
        set_dock_title(this, dockWidget_);
        return;
    }
    if (DOCK_EDGE != prop->name())
        return;
    String edge = prop->getString();
    if (edge.isEmpty())
        return;
    remakeWidget();
}

void LiquidItem::remakeWidget()
{             
    DBG(UI.WIDGET) << "LiquidItem<" << get(NAME) << ">: remake, was="
        << widget_ << ", new edge=" << get(DOCK_EDGE) << std::endl;
    dockWidget_->setWidget(0);
    delete widget_;
    widget_ = makeWidget(dockWidget_, tool_type(this));
    dockWidget_->setWidget(widget_);
    if (UNDOCKED_TOOL == tool_type(this))
        dockWidget_->resize(widget_->sizeHint());
    
    widget_->installEventFilter(eventFilter_);
    widget_->show();
}

QMainWindow* LiquidItem::mainWindow() const
{
    Sui::Item* item = Item::parent();
    for (; item; item = item->Item::parent()) {
        Sui::Document* doc = dynamic_cast<Document*>(item);
        if (doc) 
            return dynamic_cast<QMainWindow*>(doc->widget(0));
    }
    return 0;
}

static QPoint widget_pos(LiquidItem* item, QWidget* widget)
{
    PropertyNode* pn = item->itemProps()->makeDescendant(UNDOCKED_POS);
    QWidget& aw = *item->mainWindow();
    QPoint dp = aw.mapToGlobal(aw.pos());
    if (pn->getProperty("x")) 
        dp = QPoint(pn->getSafeProperty("x")->getInt(),
                    pn->getSafeProperty("y")->getInt());
    else {
        dp = QPoint(dp.x() + aw.width()/2 - widget->width()/2, 
                    dp.y() + aw.height()/2 - widget->height()/2);
        pn->makeDescendant("x")->setInt(dp.x());
        pn->makeDescendant("y")->setInt(dp.y());
    }
    DBG(UI.WIDGET) << "dp.y = " << dp.y() << ", aw.h=" << aw.height()
        << ", dockw.h=" << widget->height() << ", result=" 
        << (dp.y() + aw.height()/2 - widget->height()/2) << std::endl;
    return dp;
}

bool LiquidItem::doAttach()
{    
    Sui::Document* doc = dynamic_cast<Document*>(Item::parent());
    QWidget* parent_widget = Item::parent()->widget(doc ? 0 : this);
    if (0 == parent_widget)
        return false;
    bool is_visible = getBool(IS_VISIBLE);
    Qt::DockWidgetArea area = dock_area(this);
    DBG(UI.WIDGET) << "LiquidItem: doAttach<" << get(NAME) << ">, doc=" << doc 
        << ", visible=" << is_visible << std::endl;
    if (doc) {
        dockWidget_ = new QDockWidget(get(NOTR("caption")), mainWindow());
        set_dock_title(this, dockWidget_);
        dockWidget_->setObjectName(dockWidget_->windowTitle());
        widget_ = makeWidget(dockWidget_, tool_type(this));
        if (!widget_)
            return false;
        dockWidget_->setWidget(widget_);
        dockWidget_->resize(widget_->sizeHint());
        // the following is needed to initialize default docking position
        // and area. Because of this we need to set docking area even for
        // floating dockwidgets.
        if (area == Qt::NoDockWidgetArea) {
            mainWindow()->addDockWidget(Qt::RightDockWidgetArea, dockWidget_);
            dockWidget_->move(widget_pos(this, dockWidget_));
            dockWidget_->setFloating(true);
            if (!is_visible) 
                dockWidget_->hide();
            // trick is to process events before event filter install
            for (int i = 0; i < 5; ++i)
                qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 1);
            releaseFocus();
        } else
            mainWindow()->addDockWidget(area, dockWidget_);
        DBG(UI.WIDGET) << "LiquidItem: makeDockWidget, floating="
            << dockWidget_->isFloating() << std::endl;
    }
    else { 
        widget_ = makeWidget(parent_widget, WIDGET);
        widget_->move(widget_pos(this, widget_));
        widget_->setVisible(is_visible);
    }
    if (dockWidget_) {
        dockWidget_->setVisible(is_visible);
        QObject::connect(dockWidget_, SIGNAL(topLevelChanged(bool)), 
            eventFilter_, SLOT(floatingChanged(bool)));
        QObject::connect(dockWidget_, 
            SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            eventFilter_, 
            SLOT(dockLocationChanged(Qt::DockWidgetArea)));
        dockWidget_->installEventFilter(eventFilter_);
    }
    widget_->installEventFilter(eventFilter_);
    return true;
}

bool LiquidItem::doDetach()
{
    if (!dockWidget_.isNull() && mainWindow()) {
        mainWindow()->removeDockWidget(dockWidget_);
        dockWidget_->deleteLater();
        dockWidget_ = 0;
    }
    if (!widget_.isNull()) {
        widget_->blockSignals(true);
        widget_->deleteLater();
        widget_ = 0;
    }
    return true;
}

// this function is used to build generic context menu for liquid items
void LiquidItem::buildChildren(restore_func restoreUiItem)
{
    Document* doc = static_cast<Document*>(documentItem());
    if (0 == doc)
        return;
    PropertyNode* dialogs =
        doc->itemProps()->getProperty(LIQUID_DIALOGS);
    if (0 == dialogs)
        return;
    PropertyNode* property = dialogs->getProperty(itemClass());
    if (0 == property)
        property = dialogs->getProperty("GenericDialog");
    if (0 == property)
        return;
    (*restoreUiItem)(doc, property, this);
}

void LiquidItem::showContextMenu(const QPoint& pos)
{
    Document* doc = static_cast<Document*>(documentItem());
    if (0 == doc)
        return;
    String dock_edge = (*dockActionMap_)[get(DOCK_EDGE)];
    DockActionMap::iterator i = dockActionMap_->begin();
    for (; i != dockActionMap_->end(); i++) {
        Action* action = doc->actionSet()->findAction((*i).second);
        if (0 == action)
            continue;
        action->setEnabled(dock_edge != action->get(NAME));
    }
    Item* menu = findItem(ItemName(NOTR("liquidDialogContextMenu")));
    if (menu)
        menu->showContextMenu(pos);
}

void LiquidItem::dispatch(Action* action)
{
    DBG(UI.TEST) << "LiquidItem: dispatch: " 
                 << action->get(NAME) << std::endl;
    String actionName = action->get(NAME);
    if (actionName == NOTR("closeDialogWindow")) {
        Item::remove();
        return;
    }
    if (actionName == NOTR("hideDialogWindow")) {
        setBool(IS_VISIBLE, false);
        return;
    }
    if (actionName == NOTR("helpOnLiquidItem") && widget_) {
        QMetaObject::invokeMethod(widget_, NOTR("help"),
            Qt::QueuedConnection);
        return;
    }
    DockActionMap::const_iterator i = dockActionMap_->begin();
    for (; i != dockActionMap_->end(); i++) {
        if ((*i).second == action->get(NAME)) {
            const String old_edge = get(DOCK_EDGE);
            const String new_edge = i->first;
            if (old_edge == new_edge)
                return;
            if (documentItem() && documentItem() != Item::parent()) {
                Item* document_item = documentItem();
                detach(true);
                itemProps()->makeDescendant(DOCK_EDGE)->setString(new_edge);
                ItemPtr item_holder(this);
                Item::remove();
                document_item->appendChild(this);
                attach(true);
            }
            if (dockWidget_.isNull())
                return;
            if (mainWindow())
                mainWindow()->addDockWidget(dock_area(new_edge), dockWidget_);
            dockWidget_->setFloating(new_edge == DOCK_UNDOCKED);
            itemProps()->makeDescendant(DOCK_EDGE)->setString(new_edge);
            return;
        }
    }
    Item::dispatch(action);
    DBG(UI.TEST) << "end of LiquidItem: dispatch: " << std::endl;
}

void LiquidItem::setItemVisible(bool is_visible)
{
    DBG(UI.WIDGET) << "LiquidItem: setItemVisible=" << is_visible << std::endl;
    if (dockWidget_) {
        dockWidget_->setVisible(is_visible);
        if (is_visible) {
            dockWidget_->raise();
            DBG_IF(UI.WIDGET) {
                DBG(UI.WIDGET) << "LiquidItem: size=" 
                    << dockWidget_->width() << "/" 
                    << dockWidget_->height() << ", pos="
                    << dockWidget_->x() << "/"
                    << dockWidget_->y() << std::endl;
            }
        }
    }
    if (!widget_)
        return;
    if (is_visible) {
        widget_->show();
        widget_->activateWindow();
        widget_->raise();
        widget_->setFocus();
    } else if (!dockWidget_)
        widget_->hide();
}

void LiquidItem::floatingChanged(bool v)
{
    if (!mainWindow() || isLockedNotify())
        return;
    DBG(UI.WIDGET) << "LiquidItem: dock floating changed, new=" << v << "\n";
    if (v) {
        PropertyNode* gp = itemProps()->makeDescendant(DOCK_GEOMETRY)->
            makeDescendant(itemProps()->
                getSafeProperty(DOCK_EDGE)->getString());
        gp->makeDescendant(NOTR("width"), String::number(widget_->width()));
        gp->makeDescendant(NOTR("height"), String::number(widget_->height()));
        itemProps()->makeDescendant(DOCK_EDGE)->setString(DOCK_UNDOCKED);
    }
}

void LiquidItem::dockLocationChanged(Qt::DockWidgetArea area)
{
    if (isLockedNotify())
        return;
    DBG(UI.WIDGET) << "DockAreaChanged, to=" 
        << dock_edge(area) << std::endl;
    itemProps()->makeDescendant(DOCK_EDGE)->setString(dock_edge(area));
}

void LiquidItem::widgetFocusChanged(bool state)
{
    if (!widget_)
        return;
    QMetaObject::invokeMethod(widget_, NOTR("focusStateChanged"),
        Qt::DirectConnection, Q_ARG(bool, state));
}

void LiquidItem::escapePressed()
{
    if (!dockWidget_)
        return;      
    const RootItem* root = rootItem();
    if (dockWidget_->isFloating() && root && root->mustCloseOnEscape())
        setBool(IS_VISIBLE, false);
    else
        releaseFocus();
}

void LiquidItem::preserveVisibleState(bool switch_to)
{
    static const char* vprop = NOTR("#must-be-visible");
    if (!dockWidget_ || !dockWidget_->isFloating())
        return;
    if (switch_to) {
        if (getBool(vprop)) {
            widget_->show();
            dockWidget_->show();
            dockWidget_->raise();
        }
    } else {
        bool visible = getBool(IS_VISIBLE);
        setBool(vprop, visible);
        if (visible)
            dockWidget_->hide();
    }
}

UI_EXPIMP void disable_widget(QWidget* widget)
{
    if (0 == widget)
        return;
    const QObjectList list = widget->children();
    for (QObjectList::const_iterator i = list.begin();
         i != list.end(); ++i) {
        if ((*i)->isWidgetType())
            static_cast<QWidget*>(*i)->setEnabled(false);
    }
    QObject* close_button = widget->child(NOTR("closeButton_"));
    if (close_button)
        static_cast<QWidget*>(close_button)->setEnabled(true);
}

UI_EXPIMP void set_widget_focus_color(QWidget* widget, bool state)
{
    if (!widget)
        return;
    widget->setAutoFillBackground(true);
    if (state)
        widget->setPaletteBackgroundColor(QColor(NOTR("#c5e8e7")));
    else
        widget->setPaletteBackgroundColor
            (QApplication::palette().inactive().background());
}

}

#include "moc/LiquidItem.moc"
