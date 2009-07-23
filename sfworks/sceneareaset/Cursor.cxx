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
#include "sceneareaset/SceneAreaView.h"
#include "sceneareaset/Cursor.h"
#include "sceneareaset/debug.h"
#include "sceneareaset/utils.h"

#include "formatter/FontMgr.h"
#include "formatter/Font.h"
#include "formatter/types.h"
#include "formatter/XslFoExt.h"
#include "formatter/Area.h"

#include "common/safecast.h"
#include "common/Singleton.h"
#include "common/String.h"
#include "common/DiffuseSearch.h"
#include "common/timing_profiler.h"

#include <QApplication>
#include <QScrollBar>
#include <QGraphicsSimpleTextItem>

using namespace Formatter;
using namespace GroveEditor;
using namespace Common;
using namespace GroveLib;

static inline Chain* get_chain(const Node* node, const Node* foHint)
{
    const Node* fo_node =
        find_diffuse<Node, XslOriginTest>(foHint, XslOriginTest(node));
    if (fo_node)
        return XslFoExt::areaChain(fo_node);
    return 0;
}

Cursor::Cursor(QGraphicsScene* scene)
    : scene_(scene),
      shape_(QPolygonF(), scene),
      isShowTips_(true),
      isPersistent_(false),
      isShowParent_(false),
      delay_(3000)
{
    shape_.setPen(QPen(QColor(0, 0, 0, 230)));
    shape_.setBrush(QBrush(QColor(0, 0, 0, 230)));
    connect(&blinkTimer_, SIGNAL(timeout()), this, SLOT(blink()));
    connect(&tipTimer_, SIGNAL(timeout()), this, SLOT(hideTips()));
}

void Cursor::setTips(bool enabled, bool persistent, bool showParent, uint delay)
{
    isShowTips_ = enabled;
    isPersistent_ = persistent;
    delay_ = delay;
    isShowParent_ = showParent;
    if (!isPersistent_)
        hideTips();
}

QRectF Cursor::rect() const
{
    return shape_.boundingRect();
}

void Cursor::setDefaultCursorPos(const CRect& r)
{
    QPolygonF shape;
    shape << QPointF(0, 0)
          << QPointF(CURSOR_WIDTH, 0)
          << QPointF(CURSOR_WIDTH, r.extent_.h_)
          << QPointF(0, r.extent_.h_);
    setShapePos(qPoint(r.origin_), shape);
}

void Cursor::setShapePos(const QPointF& pos, const QPolygonF& shape)
{
    shape_.setPos(pos);
    shape_.setPolygon(shape);
}

void Cursor::setPos(const EditContext& context)
{
    hide();
    if (context.areaPos().isNull()) {
        prevSiblingTip_ = 0;
        nextSiblingTip_ = 0;
        parentTip_ = 0;
        return;
    }
    const Area* area = context.areaPos().area();
    //! Setting cursor shape
    if (area->getView()) 
        static_cast<SceneAreaView*>(area->getView())->
            setCursorPos(this, context.areaPos().pos());
    else {
        CRect cursor_rect(area->absCursorRect(context.areaPos().pos()));
        setDefaultCursorPos(cursor_rect);
    }
    shape_.setZValue(CURSOR_Z);
    //! Setting context tips
    bool same_context = false;
    if (!src_.isNull()) {
        same_context = (!context.srcPos().isNull() &&
                        src_.type() == context.srcPos().type() &&
                        src_.node() == context.srcPos().node() &&
                        (GrovePos::TEXT_POS == src_.type() ||
                        (GrovePos::ELEMENT_POS == src_.type() &&
                            src_.before() == context.srcPos().before())));
    }
    src_ = context.srcPos();
    if (src_.node() && (!same_context || isPersistent_) && isShowTips_) {
        const Node* parent = src_.node();
        while (parent && Node::ELEMENT_NODE != parent->nodeType())
            parent = parentNode(parent);
        const Node* next = (GrovePos::TEXT_POS == src_.type()) ?
            src_.node() : src_.before();
        const Node* prev = (next) ?
            next->prevSibling() : (parent) ? parent->lastChild() : 0;
        while (next && Node::ELEMENT_NODE != next->nodeType())
            next = next->nextSibling();
        while (prev && Node::ELEMENT_NODE != prev->nodeType())
            prev = prev->prevSibling();
        const GrovePos& fo_pos = context.foPos();
        const Node* fo_hint = 0;
        if (!fo_pos.isNull()) {
            if (GrovePos::ELEMENT_POS == fo_pos.type()) {
                if (fo_pos.before())
                    fo_hint = fo_pos.before();
                else {
                    fo_hint = fo_pos.node();
                    while (fo_hint->lastChild())
                        fo_hint = fo_hint->lastChild();
                }
            }
            else
                fo_hint = fo_pos.node();
        }
        setTips(static_cast<const Element*>(parent),
                static_cast<const Element*>(prev),
                static_cast<const Element*>(next), fo_hint, area);
    }
    else {
        prevSiblingTip_ = 0;
        nextSiblingTip_ = 0;
        parentTip_ = 0;
    }
    show();
}

void Cursor::setTips(const Element* parent, const Element* prev,
                     const Element* next, const Node* foHint, const Area* area)
{
    prevSiblingTip_ = 0;
    if (prev) {
        Chain* prev_chain = get_chain(prev, foHint);
        if (prev_chain && prev_chain->firstChunk())
            prevSiblingTip_ = new ContextTip(
                prev, foHint, ContextTip::PREV_SIBLING, scene_);
    }
    nextSiblingTip_ = 0;
    if (next) {
        Chain* next_chain = get_chain(next, foHint);
        if (next_chain && next_chain->firstChunk())
            nextSiblingTip_ = new ContextTip(
                next, foHint, ContextTip::NEXT_SIBLING, scene_);
    }
    const bool show_parent = 
        isShowParent_ || (0 == prevSiblingTip_ && 0 == nextSiblingTip_);
    if (show_parent && parent && parent->firstChild()) 
        parentTip_ =
            new ContextTip(parent, foHint, ContextTip::PARENT, scene_, area);
    else
        parentTip_ = 0;
}

void Cursor::show()
{
    if (shape_.isVisible())
        return;
    shape_.show();

    if (isShowTips_) {
        if (!parentTip_.isNull())
            parentTip_->show();
        if (!prevSiblingTip_.isNull())
            prevSiblingTip_->show();
        if (!nextSiblingTip_.isNull())
            nextSiblingTip_->show();
    }
    if (isShowTips_ && !isPersistent_)
        tipTimer_.start(delay_/*timeout * 3*/);
}

void Cursor::hide()
{
    if (!shape_.isVisible())
        return;
    shape_.hide();

    if (!parentTip_.isNull())
        parentTip_->hide();
    if (!prevSiblingTip_.isNull())
        prevSiblingTip_->hide();
    if (!nextSiblingTip_.isNull())
        nextSiblingTip_->hide();
}

void Cursor::stopCursor()
{
    blinkTimer_.stop();
    show();
}

void Cursor::startCursor()
{
    int timeout = QApplication::cursorFlashTime();
    blinkTimer_.start(timeout / 2);
}

void Cursor::blink()
{
    if (QApplication::overrideCursor()) {
        hide();
        hideTips();
        return;
    }
    if (shape_.isVisible()) 
        shape_.hide();
    else
        shape_.show();
}

void Cursor::hideTips()
{
    tipTimer_.stop();
    if (!parentTip_.isNull())
        parentTip_->hide();
    if (!prevSiblingTip_.isNull())
        prevSiblingTip_->hide();
    if (!nextSiblingTip_.isNull())
        nextSiblingTip_->hide();
}

//////////////////////////////////////////////////////////////////////////

static const int TIP_BORD  = 2;
static const int TIP_SPACE = 2;

static const int SCROLL_MARGIN_X = 20; // pixels
static const int SCROLL_MARGIN_Y = 40;

static const QColor PARENT_COLOR = QColor(NOTR("#aaffa3"));
//static const QColor PREV_SIBLING_COLOR = QColor("#a5f0ff");
static const QColor PREV_SIBLING_COLOR = QColor(NOTR("#fffaa3"));
static const QColor NEXT_SIBLING_COLOR = QColor(NOTR("#fffaa3"));

ContextTip::ContextTip(const Element* elem, const Node* foHint,
                       Type type, QGraphicsScene* scene, const Area* descendant)
    : font_(Font::make(QApplication::font().toString()))
{
    Chain* chain = get_chain(elem, foHint);
    const Area* area = 0;
    if (chain) {
        while (chain->parentChain() && chain->firstChunk() &&
               (chain->hasParentOrigin() ||
                LINE_AREA == chain->firstChunk()->type()))
            chain = chain->parentChain();
        if (PARENT == type || NEXT_SIBLING == type)
            area = chain->firstChunk();
        else
            area = chain->lastChunk();
        if (PARENT == type && area && descendant) {
            while (descendant && area->chain() != descendant->chain())
                descendant = descendant->parent();
            if (descendant)
                area = descendant;
        }
    }
    if (area)
        makeBorder(area, scene, !area->parent()->isProgression(HORIZONTAL));
    //! Setting tip position
    QGraphicsSimpleTextItem name_text(elem->nodeName(), 0, scene);
    name_text.setFont(font_->qfont());
    QRectF name_rect(name_text.boundingRect());
    CRange tip_range(name_rect.width() + TIP_BORD*2,
                     name_rect.height() + TIP_BORD*2 - 1);
    bool is_vertical = (chain && chain->parentChain() &&
                        VERTICAL == chain->parentChain()->progression());
    QPointF tip_point;
    if (is_vertical) {
        if (!border_.left_.isNull()) {
            QPointF point = border_.left_->line().p1();
            CType h = border_.left_->line().y2() - point.y();
            tip_point.setX(point.x() - tip_range.w_);
            if (PREV_SIBLING == type)
                tip_point.setY(point.y() + h - tip_range.h_);
            else
                if (NEXT_SIBLING == type)
                    tip_point.setY(point.y() - 1);
                else
                    tip_point.setY(point.y() + (h - tip_range.h_) / 2.0 - 1);
        }
    }
    else {
        if (!border_.top_.isNull()) {
            QPointF point = border_.top_->line().p1();
            CType w = border_.top_->line().x2() - point.x();
            tip_point.setY(point.y() - tip_range.h_);
            if (PREV_SIBLING == type)
                tip_point.setX(point.x() + w - tip_range.w_);
            else
                if (NEXT_SIBLING == type) {
                    CPoint top_left(area->absAllcPoint() + area->contPoint());
                    tip_point.setX(top_left.x_);
                }
                else
                    tip_point.setX(point.x() + (w - tip_range.w_) / 2.0);
        }
    }
    QPolygonF shape;
    const qreal w =  tip_range.w_;
    const qreal hh = tip_range.h_ / 2.;
    const qreal h = hh * 2.;
    shape << QPointF(0, 0) << QPointF(w, 0);
    if (PARENT == type)
        shape << QPointF(w, h) << QPointF(0, h);
    else
        if (PREV_SIBLING == type)
            shape << QPointF(w, h) << QPointF(0, h) << QPointF(-hh, hh);
        else
            if (NEXT_SIBLING == type)
                shape << QPointF(w+hh, hh) << QPointF(w, h) << QPointF(0, h);
    double z = CURSOR_TIP_LABEL_Z;
    if (PARENT != type)
        z += 1;
    label_ = new NamedDecoration(shape, QPointF(TIP_BORD, TIP_BORD - 1),
                                 elem->nodeName(), font_, z, scene);
    if (NEXT_SIBLING == type && is_vertical)
        label_->move(tip_point.x() - hh, tip_point.y());
    else
        label_->move(tip_point.x(), tip_point.y());

    QColor color = (PARENT == type)
        ? PARENT_COLOR
        : (PREV_SIBLING == type) ? PREV_SIBLING_COLOR : NEXT_SIBLING_COLOR;
    label_->setColor(color, color.dark(150));
}

void ContextTip::makeBorder(const Area* area, QGraphicsScene* scene,
                            bool forAllChunks)
{
    QColor bord_color(Qt::black);

    CRect rect(area->absAllcPoint() + area->contPoint(), area->contRange());
    if (forAllChunks) {
        CPoint top_left = rect.origin_;
        CPoint bott_right = rect.bottomRight();
        for (const Area* c = area->chain()->firstChunk();
             c; c = c->nextChunk()) {
            CRect c_rect(c->absAllcPoint() + c->contPoint(), c->contRange());
            CPoint c_top_left = c_rect.origin_;
            CPoint c_bott_right = c_rect.bottomRight();
            if (c_top_left.x_ < top_left.x_)
                top_left.x_ = c_top_left.x_;
            if (c_top_left.y_ < top_left.y_)
                top_left.y_ = c_top_left.y_;
            if (c_bott_right.x_ > bott_right.x_)
                bott_right.x_ = c_bott_right.x_;
            if (c_bott_right.y_ > bott_right.y_)
                bott_right.y_ = c_bott_right.y_;
        }
        rect = CRect(top_left, bott_right);
    }

    CPoint top_left(rect.origin_);
    CRange extent(rect.extent_);

    border_.top_ = new QGraphicsLineItem(0, scene);
    border_.top_->setLine(top_left.x_, top_left.y_,
                          top_left.x_ + extent.w_, top_left.y_);
    QPen pen(bord_color);
    border_.top_->setPen(pen);
    border_.top_->setZValue(CURSOR_TIP_RECT_Z);

    border_.bottom_ = new QGraphicsLineItem(0, scene);
    border_.bottom_->setPen(pen);
    border_.bottom_->setLine(top_left.x_, top_left.y_ + extent.h_,
                             top_left.x_ + extent.w_, top_left.y_ + extent.h_);
    border_.bottom_->setZValue(CURSOR_TIP_RECT_Z);

    border_.left_ = new QGraphicsLineItem(0, scene);
    border_.left_->setPen(pen);
    border_.left_->setLine(top_left.x_, top_left.y_,
                           top_left.x_, top_left.y_ + extent.h_);
    border_.left_->setZValue(CURSOR_TIP_RECT_Z);

    border_.right_ = new QGraphicsLineItem(0, scene);
    border_.right_->setPen(pen);
    border_.right_->setLine(top_left.x_ + extent.w_, top_left.y_,
                            top_left.x_ + extent.w_,
                            top_left.y_ + extent.h_);
    border_.right_->setZValue(CURSOR_TIP_RECT_Z);
}

void ContextTip::show()
{
    if (!border_.top_.isNull())
        border_.top_->show();
    if (!border_.bottom_.isNull())
        border_.bottom_->show();
    if (!border_.left_.isNull())
        border_.left_->show();
    if (!border_.right_.isNull())
        border_.right_->show();
    label_->show();
}

void ContextTip::hide()
{
    if (!border_.top_.isNull())
        border_.top_->hide();
    if (!border_.bottom_.isNull())
        border_.bottom_->hide();
    if (!border_.left_.isNull())
        border_.left_->hide();
    if (!border_.right_.isNull())
        border_.right_->hide();
    label_->hide();
    label_->scene()->update(); // hack - why it isn't updated normally?
}
