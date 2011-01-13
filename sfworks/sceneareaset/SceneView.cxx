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
#include "sceneareaset/SceneView.h"
#include "sceneareaset/PixmapImage.h"
#include "sceneareaset/GraphicAreaView.h"
#include "sceneareaset/TextAreaView.h"
#include "sceneareaset/InlineObject.h"
#include "sceneareaset/SceneAreaFactory.h"
#include "sceneareaset/Cursor.h"
#include "sceneareaset/debug.h"
#include "sceneareaset/utils.h"

#include "editableview/ToolTipEventData.h"

#include "formatter/FontMgr.h"
#include "formatter/Font.h"
#include "formatter/types.h"
#include "formatter/XslFoExt.h"
#include "formatter/impl/Areas.h"
#include "formatter/impl/ReferencedFo.h"

#include "common/safecast.h"
#include "common/Singleton.h"
#include "common/String.h"
#include "common/DiffuseSearch.h"
#include "common/timing_profiler.h"

#include <QKeyEvent>
#include <QToolTip>
#include <QApplication>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QGraphicsSimpleTextItem>
#include <QPrintDialog>
#include <QInputMethodEvent>
#include <QMatrix>

#include <iostream>

using namespace Formatter;
using namespace GroveEditor;
using namespace Common;
using namespace GroveLib;

EditableView* make_editable_view(Messenger* messenger,
                                 const GroveLib::GrovePtr& fot,
                                 EditPolicy* editPolicy)
{
    return new SceneView(messenger, fot, editPolicy);
}

SceneView::SceneView(Messenger* messenger,
                       const GroveLib::GrovePtr& fot,
                       EditPolicy* editPolicy)
    : QGraphicsView(),
      EditableView(messenger, fot, editPolicy),
      tripleClickTimer_(this),
      cursorShape_(new Cursor(&areaFactory_)),
      imageProvider_(new SceneImageProvider(&areaFactory_)),
      inlineObjFactory_(new SceneInlineObjectFactory(&areaFactory_))
{
    setName(NOTR("sceneView"));
    setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    setScene(&areaFactory_);
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
    setRenderHints(QPainter::TextAntialiasing |
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setBackgroundColor(Qt::lightGray);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);

    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_KeyCompression, true);
    setAttribute(Qt::WA_InputMethodEnabled, true);
}

void SceneView::setMicroFocusHint()
{
}

void SceneView::setWatermark(const String& watermark)
{
    watermark_ = watermark;
}

void SceneView::resetIM()
{
    QWidget::resetInputContext();
}

SceneView::~SceneView()
{
    horizontalScrollBar()->blockSignals(true);
    verticalScrollBar()->blockSignals(true);
    detachAreaTree();
}

void SceneView::setProgressStream(MessageStream* pstream)
{
    areaFactory_.setProgressStream(pstream);
}

MessageStream* SceneView::pstream() const
{
    return areaFactory_.pstream();
}

void SceneView::format(const ViewParam& viewParam)
{
    ViewParam param = viewParam;
    param.dpi_ = logicalDpiX();

    Rgb even_color = param.colorScheme_->oddContColor();
    QColor bg_color = (!param.showPaginated_) 
        ? QColor(even_color.r_, even_color.g_, even_color.b_)   
        : Qt::lightGray;
    areaFactory_.setPaginated(param.showPaginated_);
    setBackgroundColor(bg_color);

    if (formatter_) {
        formatter_->detachAreaTree();
        tagMetrixTable_.clear();
        areaFactory_.disablePerPageResizing();
    }
    EditableView::format(param);
    areaFactory_.update();  
    QApplication::syncX();
}

bool SceneView::update()
{
    bool ok = EditableView::update();
    areaFactory_.update();
    QApplication::syncX();
    return ok;
}

double SceneView::getZoom() const
{
    return matrix().m11();
}

void SceneView::setZoom(double zoom)
{
    if (0 < zoom) {
        QMatrix new_matrix;
        new_matrix.scale(zoom, zoom);
        setMatrix(new_matrix);
        if (rootArea_) {
            AreaPos pos(context_.areaPos());
            if (pos.isNull())
                return;
            QGraphicsView::centerOn(qPoint(pos.area()->
                absCursorRect(pos.pos()).origin_));
        }
    }
}

void SceneView::setContextHint(bool enabled, bool persistent,
                               bool showParent, uint delay)
{
    cursorShape_->setTips(enabled, persistent, showParent, delay);
}

void SceneView::grabFocus()
{
    if (!hasFocus())
        setFocus();
}

void SceneView::adjustView(const AreaPos& pos, bool isTop)
{
    cursorShape_->setPos(context_);
    if (pos.isNull())
        return;
    CPoint root_point = pos.area()->absCursorRect(pos.pos()).origin_;
    if (isTop)
        centerOn(root_point.x_,
            root_point.y_ - verticalScrollBar()->height() / 4);
    else
        ensureVisible(QRectF(qPoint(root_point), QSizeF(20, 40)));
    formatVisible();
}

void SceneView::startCursor()
{
    cursorShape_->startCursor();
}

void SceneView::stopCursor()
{
    cursorShape_->stopCursor();
}

bool SceneView::showCursor()
{
    if (context_.areaPos().isNull())
        return false;
    grabFocus();
    cursorShape_->show();
    return true;
}

void SceneView::hideCursor()
{
    cursorShape_->hide();
}

/////////////////////////////////////////////////////////////////////////

AreaPos SceneView::toAreaPos(const QPoint& pos) const
{
    QPointF spos(mapToScene(pos));
    return rootArea_->mapToCursorPos(CPoint(spos.x(), spos.y()));
}

void SceneView::mousePressEvent(QMouseEvent* e)
{
    dblClicked_ = false;
    AreaPos area_pos(toAreaPos(e->pos()));
    if (Qt::LeftButton == e->button() && tripleClickTimer_.isActive()) {
        tripleClickTimer_.stop();
        if (!area_pos.isNull())
            editPolicy_->mouseTripleClicked(area_pos);
    } else
        if (!area_pos.isNull())
            editPolicy_->mousePressed(area_pos, e);
}

void SceneView::mouseReleaseEvent(QMouseEvent* e)
{
    if (dblClicked_) {
        dblClicked_ = false;
    } else {
        AreaPos area_pos(toAreaPos(e->pos()));
        if (!area_pos.isNull())
            editPolicy_->mouseReleased(e);
    }
}

void SceneView::mouseDoubleClickEvent(QMouseEvent* e)
{
    dblClicked_ = true;
    AreaPos double_pos(toAreaPos(e->pos()));
    if (!double_pos.isNull())
        editPolicy_->mouseDoubleClicked(double_pos);
    tripleClickTimer_.start(qApp->doubleClickInterval(), true);
}

void SceneView::mouseMoveEvent(QMouseEvent* e)
{
    AreaPos area_pos(toAreaPos(e->pos()));
    //TODO: Make in this view map<Area::Type, QCursorShape>
    //      and use this map to keep mouse shape consistent
    //      like 'setCursor(map[area_pos.area_->type()])'
    if (!area_pos.isNull())
        editPolicy_->mouseMoved(area_pos, e);
}

bool SceneView::viewportEvent(QEvent* event)
{
    switch (event->type()) {
        case QEvent::ToolTip:
            return tooltip_event(static_cast<QHelpEvent&>(*event));
        case QEvent::KeyPress:
            return keypress_event(static_cast<QKeyEvent*>(event));
        default:
            return QGraphicsView::viewportEvent(event);
    }
}

bool SceneView::keypress_event(QKeyEvent* k)
{
    if (!(k->state() & Qt::ControlButton || k->state() & Qt::AltButton)) {
        if (k->key() == Qt::Key_Backtab || (k->key() == Qt::Key_Tab &&
                                        (k->state() & Qt::ShiftButton))) {
            keyPressEvent(k);
            return (k->isAccepted());
        }
        else
            if (k->key() == Qt::Key_Tab ) {
                keyPressEvent(k);
                return (k->isAccepted());
            }
    }
    return QGraphicsView::viewportEvent(k);
}


bool SceneView::tooltip_event(const QHelpEvent& hev)
{
    const double wm = matrix().m11();
    typedef QList<QGraphicsItem*> IList;
    IList items(scene()->items(mapToScene(hev.pos())));
    IList::iterator i = items.begin();
    const TooltipView* tview = 0;
    for (; i != items.end(); ++i) {
        if ((*i)->type() != AREAVIEW_RTTI)
            continue;
        tview = dynamic_cast<const TooltipView*>(*i);
        if (tview)
            break;
    }
    if (0 == tview || 0 == tview->tooltipViewArea()) {
        QToolTip::hideText();
        return false;
    }
    GrovePos fo_pos(tview->tooltipViewArea()->chain()->headFoNode());
    QRect rect = tview->tooltipRect();
    if (!rect.height() || !rect.width())
        rect = QRect(hev.pos().x(), hev.pos().y(),
            int((*i)->boundingRect().width() * wm),
            int((*i)->boundingRect().height() * wm));
    editPolicy_->tooltipEvent(ToolTipEventData(
        fo_pos, tview->regionType(), this, rect));
    return true;
}

///////////////////////////////////////////////////////////////////////////

void SceneView::dragEnterEvent(QDragEnterEvent* event)
{
    editPolicy_->contentsDragEnterEvent(event, toAreaPos(event->pos()));
}

void SceneView::dragMoveEvent(QDragMoveEvent* event)
{
    editPolicy_->contentsDragMoveEvent(event, toAreaPos(event->pos()));
}

void SceneView::dragLeaveEvent(QDragLeaveEvent* event)
{
    editPolicy_->contentsDragLeaveEvent(event);
}

void SceneView::dropEvent(QDropEvent* event)
{
    editPolicy_->contentsDropEvent(event, toAreaPos(event->pos()));
}

///////////////////////////////////////////////////////////////////////////

void SceneView::keyPressEvent(QKeyEvent * e)
{
    editPolicy_->keyPressed(e);
}

void SceneView::keyReleaseEvent(QKeyEvent* e)
{
    editPolicy_->keyReleased(e);
}

void SceneView::focusInEvent(QFocusEvent* e)
{
    editPolicy_->focusInEvent(e->reason() == Qt::ActiveWindowFocusReason);
//    setMicroFocusHint();
}

void SceneView::focusOutEvent(QFocusEvent* e)
{
    editPolicy_->focusOutEvent(e->reason() == Qt::ActiveWindowFocusReason);
}

QVariant SceneView::inputMethodQuery(Qt::InputMethodQuery property) const
{
    QVariant value = editPolicy_->inputMethodQuery(property);
    if (value.type() == QVariant::RectF) {
        QRectF r(value.toRectF());
        QRectF cursorRect = cursorShape_->rect();
//        CRect r(pos.area()->absCursorPoint(pos.pos()));
        cursorRect.moveTo(r.x(), r.y() + r.height()/2);
        return mapFromScene(cursorRect).boundingRect();
    }
    return value;
//  return editPolicy_->inputMethodQuery(property);
    switch(property) {
        case Qt::ImMicroFocus: {
            QVariant value = editPolicy_->inputMethodQuery(property);
            if (value.type() == QVariant::RectF) {
                QRectF r(value.toRectF());
                QRectF cursorRect = cursorShape_->rect();
//                CRect r(pos.area()->absCursorPoint(pos.pos()));
                cursorRect.moveTo(r.x(), r.y() + r.height()/2);
                return mapFromScene(cursorRect).boundingRect();
            }
#if 0
            const AreaPos& pos = context_.areaPos();
            if (0 != &pos && pos.area()) {
                QRectF cursorRect = cursorShape_->rect();
                CRect r(pos.area()->absCursorPoint(pos.pos()));
                cursorRect.moveTo(r.origin_.x_, r.origin_.y_ + r.extent_.h_/2);
                return mapFromScene(cursorRect).boundingRect();
            }
#endif
            return value;
        }
        case Qt::ImFont: {
            const AreaPos& pos = context_.areaPos();
            if (0 != &pos && pos.area())
                if (const Chain* chain = pos.area()->chain())
                    if (!chain->font().isNull())
                        return QVariant(chain->font()->qfont());
        }
        case Qt::ImCursorPosition:
            return editPolicy_->inputMethodQuery(property);
        case Qt::ImSurroundingText:
            return editPolicy_->inputMethodQuery(property);
        case Qt::ImCurrentSelection:
//            const Formatter::TreeSelection& selection = getSelection().tree_;
//            if (!selection.isEmpty()) {
//
//            }
            return QString();
        default:
            return QVariant();
    }
}

void SceneView::inputMethodEvent(QInputMethodEvent* event)
{
    if (editPolicy_)
        editPolicy_->imEvent(event);
#if 0
    if (event->preeditString().isEmpty()) {
        const AreaPos& pos = context_.areaPos();
        if (pos.isNull())
            return;
        formatVisible();
        CPoint rootPoint = pos.area()->absCursorPoint(pos.pos()).origin_;
        const qreal m11 = matrix().m11();
        QPoint visible(int(rootPoint.x_ * m11), int(rootPoint.y_ * m11));
        editPolicy_->imStartEvent(event);
        setMicroFocusHint();
    }
    else if (event->commitString().isEmpty()) {
        setMicroFocusHint();
        editPolicy_->imComposeEvent(event);
    }
    else {
        editPolicy_->imEndEvent(event);
    }
#endif
}

void SceneView::resizeEvent(QResizeEvent* e)
{
    editPolicy_->resizeEvent(e);
    QGraphicsView::resizeEvent(e);
    return;
    if (rootArea_) {
        CType root_w = 21 + (rootArea_->allcW()) * matrix().m11();
        CType view_w = e->size().width();
        if (view_w > root_w) {
            qreal left_margin = view_w - root_w;
            qreal right_margin = left_margin / 2.;
            left_margin -= right_margin;
            setViewportMargins(int(left_margin), 0, int(right_margin), 0);
        }
        else
            setViewportMargins(0, 0, 0, 0);
    }
    QGraphicsView::resizeEvent(e);
}

void SceneView::scrollContentsBy(int x, int y)
{
    formatVisible();
    QGraphicsView::scrollContentsBy(x, y);
}

ImageProvider* SceneView::imageProvider()
{
    return imageProvider_.pointer();
}

InlineObjectFactory* SceneView::inlineObjFactory()
{
    return inlineObjFactory_.pointer();
}

const AreaViewFactory* SceneView::areaViewFactory() const
{
    return &areaFactory_;
}

TagMetrixTable& SceneView::tagMetrixTable()
{
    return tagMetrixTable_;
}

void SceneView::formatVisible()
{
    if (formatter_->isFormatting())
        return;
    if (!formatter_->isFinished()) {
        const Area* last_area = formatter_->lastAreaMade();
        DBG_IF(GV.TEST)
            last_area->dump();
        if (!isBelowVisibleRange(formatter_->lastAreaMade()))
            continuePostponedFormatting();
    }
}

void SceneView::updateSize(const Area* area)
{
    CType bottom = area->absAllcPoint().y_ + area->allcH();
    QRectF rect(scene()->sceneRect());
    if (bottom > rect.height()) {
        rect.setHeight(bottom);
        scene()->setSceneRect(rect);
    }
}

bool SceneView::isBelowVisibleRange(const Area* area)
{
    CType area_bottom = area->absAllcPoint().y_ + area->allcH();
    area_bottom *= matrix().m11();
    CType visible_bottom = verticalScrollBar()->value() +
        verticalScrollBar()->height() * 1.5;

    //CType visible_bottom = contentsY() + visibleHeight()*1.5;
    DBG(GV.TEST)
        << "isBelowVisibleRange:" << area
        << " top/bottom:" << area->absAllcPoint().y_ << '/' << area_bottom
        << " visible_bottom:" << visible_bottom << std::endl;
    return (area_bottom > visible_bottom);
}

void SceneView::scrollByPage(bool isUp)
{
    int h = maximumViewportSize().height();
    scrollContentsBy(0, (isUp) ? -h : h);
}

// map from area coordinates as visible on editableview into global Qt pos
QPoint SceneView::mapToGlobal(const CPoint& cp) const
{
    return viewport()->mapToGlobal(mapFromScene(QPointF(cp.x_, cp.y_))); 
}

static int get_page_num(const Area* pos)
{
    int num = -1;
    const Area* child = pos;
    while (PAGE_AREA != child->type())
        child = child->parent();
    const PageArea* page = SAFE_CAST(const PageArea*, child);
    if (page)
        num = page->pageNum();
    return num;
}

void SceneView::print()
{
    typedef QAbstractPrintDialog PD;
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog pdialog(&printer, qApp->activeWindow());
    if (!formatter_->isFinished())
        formatter_->format(false);

    pdialog.setEnabledOptions(PD::PrintToFile|PD::PrintPageRange);
    pdialog.setMinMax(1, rootArea_->countChildren());
    if (!context_.areaPos().isNull()) {
        int num = get_page_num(context_.areaPos().area());
        if (0 < num)
            pdialog.setFromTo(1, num);
    }
    int sel_from = 0;
    int sel_to   = 0;
    if (!selection_.tree_.isEmpty()) {
        AreaPos pos = selection_.tree_.start().toAreaPos(rootArea());
        if (!pos.isNull())
            sel_from = get_page_num(pos.area());
        pos = selection_.tree_.end().toAreaPos(rootArea());
        if (!pos.isNull())
            sel_to   = get_page_num(pos.area());
    }
    if (0 < sel_from && 0 < sel_to) {
        pdialog.addEnabledOption(PD::PrintSelection);
        pdialog.setFromTo(sel_from, sel_to);
        pdialog.setPrintRange(PD::Selection);
    } else
        pdialog.setPrintRange(PD::AllPages);
    printer.setFullPage(TRUE);
    if (pdialog.exec() == QDialog::Accepted) {
        int from =  printer.fromPage();
        int to =  printer.toPage();
        if (QPrinter::PageRange == printer.printRange()) {
            if (0 == from)
                from++;
            if (0 == to)
                to++;
            if(from > to)
                to = from;
        }
        else if (QPrinter::Selection == printer.printRange()) {
            from = sel_from;
            to   = sel_to;
        }
        else if (QPrinter::AllPages == printer.printRange()) {
            from = 1;
            to = printer.maxPage();
        }
        QPainter p(&printer);
        Area* child = rootArea_->firstChild();
        int i = 1;
        hideCursor();
        removeSelection();
        for(; child; child = child->nextSibling(), i++) {
            if (PAGE_AREA != child->type() ||
                i < from )
                continue;
            if (i != from)
                printer.newPage();
            const PageArea* page = SAFE_CAST(const PageArea*, child);

            areaFactory_.render(&p, QRectF(),
                QRectF(qPoint(page->absAllcPoint()),
                       QSizeF(page->allcW(), page->allcH())));
            if (i == to)
                break;
        }
        showCursor();
    }
}

