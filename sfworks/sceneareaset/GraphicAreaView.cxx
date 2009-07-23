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
#include "sceneareaset/GraphicAreaView.h"
#include "sceneareaset/PixmapImage.h"
#include "sceneareaset/Cursor.h"
#include "sceneareaset/debug.h"
#include "common/String.h"
#include "formatter/impl/Areas.h"
#include "formatter/impl/ReferencedFo.h"
#include "formatter/impl/TerminalFos.h"

#include <QPolygonF>

using namespace Common;
using namespace Formatter;

// all FO's which contain images are inherited from ImageContainer
static ScenePixmapImage* area_image(const Area* area)
{
    const ImageContainer* const img_container = 
        dynamic_cast<const ImageContainer*>(area->chain());
    RT_ASSERT(img_container);
    RT_ASSERT(dynamic_cast<ScenePixmapImage*>(img_container->image()));
    return static_cast<ScenePixmapImage*>(img_container->image());
}

GraphicAreaViewUtils::GraphicAreaViewUtils(const Area* area)
{
    update_geometry(area);
}


GraphicAreaViewUtils::~GraphicAreaViewUtils()
{
}

void GraphicAreaViewUtils::set_cursor(Cursor* cursor,
                                      const Area* area) const
{
    CRect cursor_rect = CRect(area->absAllcPoint() + area->contPoint(),
                              area->contRange());
    CType  h = cursor_rect.extent_.h_;
    CType  w = cursor_rect.extent_.w_;
    QPolygonF point_array;
    point_array << QPointF(0,   h - 4)
                << QPointF(w,   h - 4)
                << QPointF(w,   h)
                << QPointF(0,   h);
    cursor->setShapePos(qPoint(cursor_rect.origin_), point_array);
}

void GraphicAreaViewUtils::repaint_selection(const Area* area)
{
    if (area->getView()->selection().isEmpty()) {
        if (!selRect_.isNull()) {
            selRect_ = 0;
            return;
        }
    }
    else {
        CPoint p = area->absAllcPoint();
        QRectF inversed(p.x_, p.y_, area->allcW(), area->allcH());
        selRect_ = new SelectionRect(inversed, Rgb(0, 0, 0),
            Rgb(255, 255, 255), 
            static_cast<const SceneAreaView*>(area->getView())->scene(),
            area->chain()->level() + 1);
        selRect_->show();
    }
}

void GraphicAreaViewUtils::update_geometry(const Area* area)
{
    ScenePixmapImage* image = area_image(area);
    image->setOrigin(area);
    CPoint allc(area->absAllcPoint() + area->contPoint());
    DBG(GV.IMAGE) << "updateGeom: image=" << image << std::endl;
    image->setPos(allc.x_, allc.y_);
    image->setZValue(area->chain()->level(true) + 1);
    image->show();
}
