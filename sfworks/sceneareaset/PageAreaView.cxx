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
#include "sceneareaset/utils.h"
#include "sceneareaset/PageAreaView.h"
#include "sceneareaset/SceneView.h"
#include "common/safecast.h"
#include "common/String.h"
#include "formatter/impl/Areas.h"
#include "formatter/impl/ReferencedFo.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <iostream>

using namespace Formatter;

PageAreaView::PageAreaView(const Area* area, 
                           QGraphicsScene* scene,
                           bool showBorder)
    : SceneAreaView(area, scene)
{
    const PageArea* page = SAFE_CAST(const PageArea*, area);
    CPoint allc(area->absAllcPoint());
    CRect cont(allc, CRange(area->allcW(), area->allcH()));
    Rgb color = area->chain()->contColor();
    rect_    = make_scene_rect(scene_, cont, color, area->chain()->level());
    if (showBorder) {
        rect_->setPen(QPen(QColor(0,0,0)));
        pageNum_ = new QGraphicsSimpleTextItem(rect_.pointer());
        pageNum_->setText(QString::number(page->pageNum()));
        pageNum_->setZValue(area->chain()->level() + 1);
        pageNum_->show();
    }
    SceneView* sv = static_cast<SceneView*>(scene->views()[0]);
    QString watermark = sv->watermark();
    if (!sv->watermark().isEmpty()) {
        watermark_ = new QGraphicsSimpleTextItem(rect_.pointer());
        QFont font("Helvetica", 10);
        watermark_->setPen(QPen(QColor(Qt::lightGray)));
        watermark_->setText(sv->watermark());
        watermark_->setZValue(10000000);    // show on top of everything
        watermark_->rotate(-90);
        watermark_->show();
    }
    updateGeometry();
}

void PageAreaView::updateGeometry()
{
    const double PG_OFFSET = 30;
    CPoint allc(area()->absAllcPoint());
    update_scene_rect(rect_.pointer(),
                       CRect(allc, CRange(area()->allcW(), area()->allcH())),
                       area()->chain()->contColor());
    const double rx = rect_->rect().x();
    const double ry = rect_->rect().y();
    if (pageNum_) {
        QRectF num_rect = pageNum_->boundingRect();
        pageNum_->setPos(rx + area()->allcW() - PG_OFFSET - num_rect.width(),
            ry + area()->allcH() - PG_OFFSET - num_rect.height());
    }
    if (watermark_) {
        QRectF w_rect = watermark_->boundingRect();
        watermark_->setPos(rx + w_rect.height(), 
                           ry + 40 + w_rect.width());
    }
}
