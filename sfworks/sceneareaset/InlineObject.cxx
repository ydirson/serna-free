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
#include "sceneareaset/InlineObject.h"
#include "sceneareaset/utils.h"
#include "formatter/InlineObject.h"
#include "formatter/impl/Areas.h"
#include "grove/Nodes.h"
#include "grove/Grove.h"
#include "common/OwnerPtr.h"

#include <QPainter>
#include <QPixmap>
#include <QGraphicsView>

#include "qtmmlwidget.h"
#include <iostream>

using namespace Formatter;

class MmlInlineObject : public SceneInlineObject,
                        public QGraphicsRectItem,
                        public QtMmlDocument {
public:
    MmlInlineObject(QGraphicsScene* scene,
                    const InlineObjectData& data)
        : QGraphicsRectItem(0, scene)
    {
        QtMmlDocument::setFontName(QtMmlWidget::NormalFont, data.fontFamily_);
        QtMmlDocument::setBaseFontPointSize(int(data.fontSize_));
        //MmlRenderer::setPaintDevice(scene->views()[0]);
        QString err;
        setContent(data.origin_, &err);
        QGraphicsRectItem::setRect(QRectF(0, 0, QtMmlDocument::size().width(),
                                          QtMmlDocument::size().height()));
        //if (!err.isEmpty())
        //    std::cerr << err.latin1() << std::endl;
    }
    virtual void move(qreal x, qreal y)
    {
        QGraphicsRectItem::setPos(x, y);
    }
    virtual void setZ(qreal z) { QGraphicsRectItem::setZValue(z); }
    virtual void show() { QGraphicsRectItem::show(); }
    virtual CRange size() const
    {
        QSize sz(QtMmlDocument::size());
        return CRange(sz.width(), sz.height());
    }
    virtual void update_view() { QGraphicsRectItem::update(); }
    virtual void paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*)
    {
        //std::cerr << "matrix scale=" << p->matrix().m11() << std::endl;
        p->setPen(QPen(QColor("#000000")));
        QtMmlDocument::paint(p, QPoint());
    }
};

MmlAreaView::MmlAreaView(const Area* area, QGraphicsScene* scene)
        : ContentAreaView(area, scene)
{
    object_ = static_cast<MmlInlineObject*>
        (static_cast<const InlineObjectArea*>(area)->inlineObject());
    updateGeometry();
}

void MmlAreaView::updateGeometry()
{
    ContentAreaView::updateGeometry();
    const CPoint allc(area()->absAllcPoint());
    object_->move(allc.x_, allc.y_);
    object_->setZ(area()->chain()->level(true) + 2);
    object_->show();
}

void MmlAreaView::repaintSelection()
{
    if (selection_.isEmpty()) {
        if (!selRect_.isNull()) {
            selRect_ = 0;
            return;
        }
    }
    else {
        CPoint allc_point = area()->absAllcPoint();
        QRectF inversed(allc_point.x_, allc_point.y_,
                        area()->contRange().w_, area()->contRange().h_);
        selRect_ = new SelectionRect(inversed, Rgb(0, 0, 0),
            Rgb(255, 255,255), scene_, 1.);
        selRect_->show();
    }
}

InlineObject*
SceneInlineObjectFactory::make(const InlineObjectData& data) const
{
    return new MmlInlineObject(scene_, data);
}
