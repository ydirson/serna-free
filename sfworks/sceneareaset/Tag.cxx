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

#include "formatter/Area.h"
#include "formatter/Font.h"
#include "formatter/MediaInfo.h"
#include "formatter/Font.h"

#include "grove/Nodes.h"

#include "sceneareaset/TagMetrixTable.h"
#include "sceneareaset/utils.h"
#include "sceneareaset/Tag.h"
#include "sceneareaset/Cursor.h"

#include <iostream>

using namespace Common;
using namespace GroveLib;
using namespace Formatter;

CanvasPolygon::CanvasPolygon(const QPolygonF& points, QGraphicsScene* scene)
    : QGraphicsPolygonItem(0, scene),
      compositionMode_(QPainter::CompositionMode_SourceOver)
{
    setPolygon(points);
}

/////////////////////////////////////////////////////////////////////

AreaDecoration::AreaDecoration(const QPointF& pos, const QPolygonF& points,
                               double z, QGraphicsScene* scene)
    : body_(points, scene),
      isSelected_(false)
{
    setColor(QColor(0xff, 0x99, 0x00), Qt::black);
    body_.setZValue(z);
    body_.setPos(pos.x(), pos.y());
    body_.show();
}

void AreaDecoration::move(double x, double y)
{
    body_.setPos(x, y);
}

void AreaDecoration::setColor(QColor bodyColor, QColor outlineColor)
{
    bodyColor_ = bodyColor;
    outlineColor_ = outlineColor;
    body_.setPen(QPen((isSelected_) ? bodyColor_ : outlineColor_));
    body_.setBrush(QBrush((isSelected_) ? outlineColor_ : bodyColor_));
}

void AreaDecoration::setSelected(bool isSelected)
{
    isSelected_ = isSelected;
    setColor(bodyColor_, outlineColor_);
}

/////////////////////////////////////////////////////////////////////

SectionCorner::SectionCorner(const QPointF& pos,
                             const QPolygonF& points,
                             double z, 
                             QGraphicsScene* scene,
                             const QColor& color,
                             const QColor& outlineColor)
    : AreaDecoration(pos, points, z, scene)
{
    setColor(color, outlineColor);
}

void SectionCorner::setCursor(Cursor* cursor) const
{
    cursor->setShapePos(body_.pos(), body_.polygon());
}

/////////////////////////////////////////////////////////////////////

NamedDecoration::NamedDecoration(const QPolygonF& shape,
                                 const QPointF& namePos, 
                                 const String& name,
                                 const FontPtr& font, 
                                 double z,
                                 QGraphicsScene* scene)
    : AreaDecoration(QPointF(0, 0), shape, z, scene),
      name_(0, scene),
      namePos_(namePos)
{
    name_.setText(name);
    name_.setZValue(z + TEXT_Z_SHIFT);

    if (!font.isNull())
        name_.setFont(font->qfont());

    setColor(Qt::white, Qt::black);
}

void NamedDecoration::move(double x, double y)
{
    name_.setPos(namePos_.x() + x, namePos_.y() + y);
    AreaDecoration::move(x, y);
}

void NamedDecoration::setCursor(Cursor* cursor) const
{
    cursor->setShapePos(body_.pos(), QPolygonF());
}

/////////////////////////////////////////////////////////////////////

Tag::Tag(const QPointF& pos, const QPolygonF& shape, const QPointF& namePos,
         const NsNode* elem, const ColorScheme* colorScheme,
         const FontPtr& font,  double z, QGraphicsScene* scene)
    : AreaDecoration(pos, shape, z, scene),
      name_(0, scene)
{
    String name = node_name(elem);
    String uri = (elem) ? elem->xmlNsUri() : String::null();

    Rgb body_color;
    Rgb text_color;
    colorScheme->tagColor(uri, body_color, text_color);

    name_.setText(name);
    name_.setPos(pos.x() + namePos.x(), pos.y() + namePos.y());
    name_.setZValue(z + TEXT_Z_SHIFT);
    // TODO: name_.setColor(qColor(text_color));
    if (!font.isNull())
        name_.setFont(font->qfont());
    name_.show();

    setColor(qColor(body_color), qColor(body_color).dark(150));
}

void Tag::setColors(const NsNode* elem, const ColorScheme* colorScheme)
{
    Rgb body_color;
    Rgb text_color;
    String uri = (elem) ? elem->xmlNsUri() : String::null();
    colorScheme->tagColor(uri, body_color, text_color);
    // TODO: name_.setColor(qColor(text_color));
    setColor(qColor(body_color), qColor(body_color).dark(150));
}

void Tag::move(double x, double y)
{
    QPointF name_pos(int(name_.x() - body_.x()), int(name_.y() - body_.y()));
    name_.setPos(name_pos.x() + x, name_pos.y() + y);
    AreaDecoration::move(x, y);
}

void Tag::setCursor(Cursor* cursor) const
{
    QPolygonF point_array;
    qreal c = cursorPos();
    qreal h = body_.boundingRect().height();
    point_array << QPointF(c, 0)
                << QPointF(c + CURSOR_WIDTH, 0)
                << QPointF(c + CURSOR_WIDTH, h)
                << QPointF(c, h);
    cursor->setShapePos(body_.pos(), point_array);                
}

/////////////////////////////////////////////////////////////////////

StartTag::StartTag(const QPointF& pos, const NsNode* elem,
                   const ColorScheme* colorScheme,
                   SceneTagMetrix* metrix, double z, QGraphicsScene* scene)
    : Tag(pos, metrix->start_, metrix->startName_,
          elem, colorScheme, metrix->font_, z, scene)
{
}

qreal StartTag::cursorPos() const
{
    return body_.boundingRect().width();
}

/////////////////////////////////////////////////////////////////////

EndTag::EndTag(const QPointF& pos, const NsNode* elem,
               const ColorScheme* colorScheme,
               SceneTagMetrix* metrix, double z, QGraphicsScene* scene)
    : Tag(pos, metrix->end_, metrix->endName_,
          elem, colorScheme, metrix->font_, z, scene)
{
}

/////////////////////////////////////////////////////////////////////

EmptyTag::EmptyTag(const QPointF& pos, const NsNode* elem,
                   const ColorScheme* colorScheme,
                   SceneTagMetrix* metrix, double z, QGraphicsScene* scene)
    : Tag(pos, metrix->empty_, metrix->emptyName_,
          elem, colorScheme, metrix->font_, z, scene),
      cursorPos_(metrix->empty_[2].x())
{
}

qreal EmptyTag::cursorPos() const
{
    return cursorPos_;
}

/////////////////////////////////////////////////////////////////////

ChoiceTag::ChoiceTag(SceneTagMetrix* metrix, double z, 
                     QGraphicsScene* scene, const String& name)
    : NamedDecoration(metrix->choice_, metrix->choiceName_, name,
                      metrix->font_, z, scene),
      arrow_(getShape(), scene)
{
    QColor color(NOTR("#adffaa"));
    setColor(color, color.dark(200));

    arrow_.setBrush(QBrush(QColor(NOTR("#8ddf8a"))));
    arrow_.setZValue(z + TEXT_Z_SHIFT);
    arrow_.show();
}

void ChoiceTag::move(double x, double y)
{
    arrow_.setPos(x, y);
    NamedDecoration::move(x, y);
}

QPolygonF ChoiceTag::getShape() const
{
    QRectF rect(body_.boundingRect());
    qreal d = rect.height() / 2;
    qreal x = rect.width() - (rect.height() - d)/2;
    qreal y = (rect.height() - d)/2;
    QPolygonF point_array;
    point_array << QPointF(x, y)
                << QPointF(x - d, y) 
                << QPointF(x - d/2, y + d);
    return point_array;
}

void ChoiceTag::setCursor(Cursor* cursor) const
{
    cursor->setShapePos(body_.pos(), getShape());
}

/////////////////////////////////////////////////////////////////////

FoldTag::FoldTag(const QPolygonF& shape, const QPointF& namePoint,
                 const FontPtr& font, const String& name,
                 double z, QGraphicsScene* scene)
    : NamedDecoration(shape, namePoint, name, font, z, scene),
      cross_(getShape(), scene)
{
    setColor(QColor(NOTR("#fffec9")), QColor(NOTR("#6b9393")));

    cross_.setPen(QPen(QColor(NOTR("#6b9393"))));
    cross_.setBrush(QBrush(QColor((NOTR("#6b9393")))));
    cross_.setZValue(z + TEXT_Z_SHIFT);
    cross_.show();
}

void FoldTag::move(double x, double y)
{
    cross_.setPos(x, y);
    NamedDecoration::move(x, y);
}

QPolygonF FoldTag::getShape() const
{
    qreal h = body_.boundingRect().height();
    qreal dx = h*1/4;

    qreal x1 = h/4;
    qreal y1 = h/4;

    qreal x2 = x1 + dx;
    qreal y2 = y1 + dx;

    qreal w = 1;
    qreal x3 = x2 + w + dx;
    qreal y3 = y2 + w + dx;

    QPolygonF point_array;
    point_array << QPointF(x1,      y2)
                << QPointF(x2,      y2)
                << QPointF(x2,      y1)
                << QPointF(x2 + w,  y1)
                << QPointF(x2 + w,  y2)
                << QPointF(x3,      y2)
                << QPointF(x3,      y2 + w)
                << QPointF(x2 + w,  y2 + w)
                << QPointF(x2 + w,  y3)
                << QPointF(x2,      y3)
                << QPointF(x2,      y2 + w)
                << QPointF(x1,      y2 + w);
    return point_array;
}

void FoldTag::setCursor(Cursor* cursor) const
{
    QRectF rect(body_.boundingRect());
    qreal h  = rect.height()/2;
    qreal x1 = h/2;
    qreal y1 = h/2;
    QPolygonF point_array;
    point_array << QPointF(x1,           y1)
                << QPointF(x1 + h + 1,   y1)
                << QPointF(x1 + h + 1,   y1 + h + 1)
                << QPointF(x1,           y1 + h + 1);
    cursor->setShapePos(body_.pos(), point_array);
}
