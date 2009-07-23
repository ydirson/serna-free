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
#include "sceneareaset/defs.h"
#include "sceneareaset/utils.h"
#include "formatter/Area.h"

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPainter>

#include <iostream>

using namespace Formatter;

CRect area_alloc_rect(const Area* area)
{
    CPoint cont_point(area->absAllcPoint() + area->contPoint());
    ORect bord_rect(area->padd());
    cont_point -= CPoint(bord_rect.left_, bord_rect.top_);
    return CRect(cont_point,
                 CRange(area->contRange().w_ + bord_rect.left_ +
                        bord_rect.right_,
                        area->contRange().h_ + bord_rect.top_ +
                        bord_rect.bottom_));
}

static QRectF adjusted_qrect(const CRect& rect)
{
    QRectF qrect(qRect(rect));
    qrect.setWidth(qMax(qrect.width() - 0.5, 0.));
    qrect.setHeight(qMax(qrect.height() - 0.5, 0.));
    return qrect;
}

QGraphicsRectItem* make_scene_rect(QGraphicsScene* scene, const CRect& rect,
                                   const Rgb& color, double z)
{
    QGraphicsRectItem* item = 
        new QGraphicsRectItem(adjusted_qrect(rect), 0, scene);
    if (!color.isTransparent_)
        item->setBrush(QBrush(qColor(color)));
    item->setPen(QPen(qColor(color)));
    item->setZValue(z);
    item->show();
    return item;
}

void update_scene_rect(QGraphicsRectItem* item, const CRect& rect,
                       const Rgb& color)
{
    if (item->x() != rect.origin_.x_ || item->y() != rect.origin_.y_ ||
        item->rect().width() != rect.extent_.w_ || 
        item->rect().height() != rect.extent_.h_)
            item->setRect(adjusted_qrect(rect));
    if (!color.isTransparent_) {
        QColor q_color(qColor(color));
        if (item->brush().color() != q_color) {
            item->setBrush(QBrush(q_color));
            item->setPen(QPen(q_color));
        }
    }
}
