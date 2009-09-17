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
#include "common/safecast.h"
#include "common/String.h"

#include "formatter/impl/Areas.h"
#include "formatter/impl/TerminalFos.h"
#include "formatter/Font.h"

#include "sceneareaset/utils.h"
#include "sceneareaset/TextAreaView.h"
#include "sceneareaset/PixmapImage.h"
#include "common/WordTokenizer.h"

#include <QApplication>
#include <QGraphicsScene>
#include <QDesktopWidget>
#include <QPainter>
#include <QComboBox>
#include <QLineEdit>
#include <QPixmap>
#include <iostream>

using namespace Common;
using namespace Formatter;

SelectionRect::SelectionRect(const QRectF& r, const Rgb& textColor,
                             const Rgb& contColor, QGraphicsScene* scene,
                             double z)
    : QGraphicsRectItem(r, 0, scene), rect_(r)
{
    bool is_cont_c = ((textColor.r_ + textColor.g_ + textColor.b_) >
                       (contColor.r_ + contColor.g_ + contColor.b_));
    color_ = is_cont_c ? contColor : textColor;
    setZValue(z + 1);
}

void SelectionRect::paint(QPainter* p, const QStyleOptionGraphicsItem*,
                          QWidget*) 
{
    QColor color(qColor(color_));
    color.setAlpha(128);
    p->fillRect(rect_, QBrush(color));
}                                        

///////////////////////////////////////////////////////////////////////////

const TextArea* TextAreaView::area() const
{
    return SAFE_CAST(const TextArea*, ContentAreaView::area());
}

const TextFo* TextAreaView::fo() const
{
    return static_cast<const TextFo*>(ContentAreaView::area()->chain());
}

static QPainterPath make_wavy_path(qreal x, qreal y, qreal minWidth)
{
    QPainterPath path;
    bool up = true;
    int i = 0;
    const qreal radius = 1.3;
    path.moveTo(x, radius + y);
    qreal xs, ys;
    do {
        xs = i * (2 * radius);
        ys = 0;
        qreal remaining = minWidth - xs;
        qreal angle = 180;
        if (remaining < 2 * radius)
            angle = 180 * remaining / (2 * radius);
        path.arcTo(xs + x, ys + y, 
            2 * radius, 2 * radius, 180, up ? angle : -angle);
        up = !up;
        ++i;
    } while (xs + 2 * radius < minWidth);
    return path;
}

TextAreaView::TextAreaView(const Area* p_area, QGraphicsScene* scene)
    : ContentAreaView(p_area, scene)
{
    CPoint allc(area()->absAllcPoint());
    setPos(allc.x_, allc.y_);
    setZValue(area()->chain()->level(true) + 1);
    scene->addItem(this);
    show();
}

void TextAreaView::updateDecoration()
{
    QGraphicsItem::update();
}

QRectF TextAreaView::boundingRect() const 
{ 
    return QRectF(0, 0, area()->allcW(), area()->allcH());
}

QPainterPath TextAreaView::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

bool TextAreaView::contains(const QPointF &point) const
{
    return boundingRect().contains(point);
}

void TextAreaView::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
                         QWidget*) 
{
    QPen pen;
    if (!fo()->textColor().isTransparent_) {
        QBrush brush(Qt::SolidPattern);
        brush.setColor(qColor(fo()->textColor()));
        pen.setBrush(brush);
    } 
    RangeString rs = area()->text().stripTrailingWhitespace();
    if (!rs.length())
        return;
    QFont adj_font(fo()->font()->qfont());
    int desktopDpi = QApplication::desktop()->logicalDpiX();
    int painterDpi = painter->device()->logicalDpiX();
    if (desktopDpi != painterDpi) 
        adj_font.setPointSizeF(adj_font.pointSizeF() * desktopDpi / painterDpi);
    painter->setFont(adj_font);
    painter->setPen(pen);
    painter->drawText(QPointF(0, area()->base()), rs.toQString());
    if (!fo()->hasMarkedWords())
        return;
    painter->setPen(Qt::red);
    QFontMetrics fm(fo()->font()->qfont());
    WordTokenizer wtok(rs);
    RangeString to_mark;
    while (wtok.next(to_mark)) {
        if (!fo()->isMarkedWord(to_mark))
            continue;
        int offset = fm.width(rs.toQString(), to_mark.begin() - rs.unicode());
        int width  = fm.width(to_mark.toQString(), to_mark.length());
        painter->drawPath(make_wavy_path(offset, area()->base() + 1, width));
    }
}

void TextAreaView::repaintSelection()
{
    if (selection_.isEmpty()) {
        if (!selRect_.isNull()) {
            selRect_ = 0;
            return;
        }
    }
    else {
        CRect from = area()->absCursorRect(selection_.from());
        CRect to = area()->absCursorRect(selection_.to());
        CType sel_l = from.origin_.x_;
        CType sel_r = to.origin_.x_;
        CType y = from.origin_.y_;
        CType dy = area()->contRange().h_;
        QRectF inversed(sel_l, y, sel_r - sel_l, dy);
        selRect_ = new SelectionRect(inversed,
            fo()->textColor(), fo()->contColor(), scene_, zValue());
        selRect_->show();
    }
}


void TextAreaView::updateGeometry()
{
    ContentAreaView::updateGeometry();
    CPoint allc(area()->absAllcPoint());
    setPos(allc.x_, allc.y_);
}

///////////////////////////////////////////////////////////////////////////

WidgetAreaView::WidgetAreaView(const Area* area, QGraphicsScene* scene)
    : ContentAreaView(area, scene)
{
    scene->addItem(this);
}

void WidgetAreaView::repaintSelection()
{
    if (selection_.isEmpty()) {
        if (!selRect_.isNull()) {
            selRect_ = 0;
            return;
        }
    }
    else {
        CPoint allc_point = area()->absAllcPoint();
        QRectF inversed(QPointF(allc_point.x_, allc_point.y_), 
                        QSizeF(area()->contRange().w_, 
                               area()->contRange().h_));
        selRect_ = new SelectionRect(inversed, Rgb(0, 0, 0), 
                                     Rgb(255, 255,255), scene_, zValue());
        selRect_->show();
    }
}

void WidgetAreaView::updateGeometry()
{
    ContentAreaView::updateGeometry();
    const CPoint allc(area()->absAllcPoint());
    QRectF new_rect(allc.x_, allc.y_, 
        area()->contRange().w_, area()->contRange().h_);
    if (new_rect != rect())
        setRect(new_rect);
    setZValue(area()->chain()->level(true));
    pixmap_ = grabWidget();
    show();
}

void WidgetAreaView::paint(QPainter* p, const QStyleOptionGraphicsItem*,
                           QWidget*)
{
    p->drawPixmap(QPointF(rect().x(), rect().y()), pixmap_);
}

// a workaround for qt 4.4 bug - can be replaced with just grabWidget() later
QPixmap WidgetAreaView::do_grab(QWidget* widget) const
{
    QGraphicsView* view = QGraphicsItem::scene()->views()[0];
    const CPoint allcPoint(area()->absAllcPoint());
    CRange range(area()->contRange());
    QRect geometry = QRect(view->viewport()->mapToGlobal
        (view->mapFromScene(QPointF(allcPoint.x_, allcPoint.y_))), 
            QSize(int(range.w_), int(range.h_)));
    widget->setFont(area()->chain()->font()->qfont());
    widget->setFixedSize(geometry.size());
#ifdef _WIN32
    widget->reparent(view, Qt::WType_Popup, geometry.topLeft());
    widget->show();
    QPixmap pixmap = QPixmap::grabWidget(widget);
    widget->hide();
    return pixmap;
#else // _WIN32
    return QPixmap::grabWidget(widget);
#endif // _WIN32
}

WidgetAreaView::~WidgetAreaView()
{
}

///////////////////////////////////////////////////////////////////////////

QPixmap ComboBoxAreaView::grabWidget() const
{    
    const ComboBoxArea* a = static_cast<const ComboBoxArea*>(area());
    QComboBox combo;
    combo.insertItem(a->text());
    combo.setEditable(a->isEditable());
    combo.setEnabled(a->isEnabled());
    return do_grab(&combo);
}

///////////////////////////////////////////////////////////////////////////

QPixmap LineEditAreaView::grabWidget() const
{    
    const LineEditArea* a = static_cast<const LineEditArea*>(area());
    QLineEdit line_edit(a->text());
    line_edit.setEnabled(a->isEnabled());
    line_edit.setCursorPosition(line_edit.text().length());
    return do_grab(&line_edit);
}
