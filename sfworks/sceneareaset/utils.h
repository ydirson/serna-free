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
#ifndef SCENEAREASET_UTILS_H
#define SCENEAREASET_UTILS_H

#include "formatter/types.h"
#include "formatter/Area.h"
#include <QPolygonF>
#include <QColor>

class QPoint;
class QSize;
class QRect;

inline QPointF qPoint(const Formatter::CPoint& p)
{
    return QPointF(p.x_, p.y_);
}

inline QSizeF qSize(const Formatter::CRange& r)
{
    return QSizeF(r.w_, r.h_);
}

inline QRectF qRect(const Formatter::CRect& r)
{
    return QRectF(qPoint(r.origin_), qSize(r.extent_));
}

inline Formatter::CPoint cPoint(const QPointF& p)
{
    return Formatter::CPoint(p.x(), p.y());
}

inline QColor qColor(const Formatter::Rgb& color)
{
    return QColor(color.r_, color.g_, color.b_);
}

class QGraphicsScene;
class QGraphicsRectItem;

inline Formatter::CRect area_cont_rect(const Formatter::Area* area)
{
    return Formatter::CRect(area->absAllcPoint() + area->contPoint(), 
                            area->contRange());
}

Formatter::CRect area_alloc_rect(const Formatter::Area* area);

QGraphicsRectItem* make_scene_rect(QGraphicsScene* scene, 
                                   const Formatter::CRect& rect,
                                   const Formatter::Rgb& color, double z);
void update_scene_rect(QGraphicsRectItem* item, 
                       const Formatter::CRect& rect,
                       const Formatter::Rgb& color);

#endif // SCENEAREASET_UTILS_H
