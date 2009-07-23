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
#ifndef GRAPHIC_AREA_VIEW_H
#define GRAPHIC_AREA_VIEW_H

#include "sceneareaset/defs.h"
#include "sceneareaset/SceneAreaView.h"
#include <QPolygonF>

class QGraphicsScene;
class PixmapItem;
class CanvasPixmapImage;

class GraphicAreaViewUtils {
public:
    GraphicAreaViewUtils(const Formatter::Area*);
    virtual ~GraphicAreaViewUtils();
    
    void        set_cursor(Cursor*, const Formatter::Area* area) const;
    void        repaint_selection(const Formatter::Area* area);
    void        update_geometry(const Formatter::Area* area);

private:
    Common::OwnerPtr<SelectionRect> selRect_;
};

class GraphicAreaView : public TaggedAreaView, public GraphicAreaViewUtils {
public:
    //CANVASAREASET_OALLOC(GraphicAreaView);

    GraphicAreaView(const Formatter::Area* area, QGraphicsScene* scene)
        : TaggedAreaView(area, scene), GraphicAreaViewUtils(area) {}
    void            updateGeometry() 
    {
        TaggedAreaView::updateGeometry();
        update_geometry(area());
    }
    virtual void repaintSelection() { repaint_selection(area()); }
    virtual void setCursorPos(Cursor* c, uint) const { set_cursor(c, area()); }
};

class SelectableAreaView : public SceneAreaView, public GraphicAreaViewUtils {
public:
    //CANVASAREASET_OALLOC(SelectableAreaView);
    
    SelectableAreaView(const Formatter::Area* area, QGraphicsScene* scene)
        : SceneAreaView(area, scene), GraphicAreaViewUtils(area) {}
    void            updateGeometry() { update_geometry(area()); }
    virtual void repaintSelection() { repaint_selection(area()); }
    virtual void setCursorPos(Cursor* c, uint) const { set_cursor(c, area()); }
};

#endif //GRAPHIC_AREA_VIEW_H
