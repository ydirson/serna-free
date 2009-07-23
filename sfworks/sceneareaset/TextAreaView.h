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
#ifndef TEXT_AREA_VIEW_H
#define TEXT_AREA_VIEW_H

#include "sceneareaset/GraphicAreaView.h"
#include "editableview/TooltipView.h"
#include "formatter/types.h"

#include <QGraphicsView>
#include <QPixmap>

namespace Formatter {
    class TextArea;
    class TextFo;
}

class TextAreaView : public ContentAreaView,
#ifdef NEW_TDRAW
                     public QGraphicsItem,
#else
                     public QGraphicsSimpleTextItem,
#endif
                     public TooltipView {
public:
    //CANVASAREASET_OALLOC(TextAreaView);
    
    TextAreaView(const Formatter::Area* area,
                 QGraphicsScene* scene);

    void    updateGeometry();
    void    repaintSelection();

    const Formatter::TextArea*  area() const;
    const Formatter::TextFo*    fo() const;

    virtual const Formatter::Area* tooltipViewArea() const
        { return ContentAreaView::area(); }
    virtual RegionType  regionType() const { return CONTENT_AREA_REGION; }
    virtual int         type() const { return AREAVIEW_RTTI; } 

#ifdef NEW_TDRAW
    // reimplemented from GraphicsItem
    virtual void        paint(QPainter* p,
                              const QStyleOptionGraphicsItem*, QWidget*);
    virtual QRectF      boundingRect() const;
    virtual QPainterPath shape() const;
    virtual bool        contains(const QPointF &point) const; 
#endif // NEW_TDRAW
private:
    Common::OwnerPtr<SelectionRect> selRect_;
};

class WidgetAreaView : public ContentAreaView,
                       public QGraphicsRectItem {
public:
    WidgetAreaView(const Formatter::Area* area, QGraphicsScene* scene);
    
    virtual QPixmap     grabWidget() const = 0;
    virtual void        paint(QPainter*, const QStyleOptionGraphicsItem*,
                              QWidget*); 
    
    virtual void        updateGeometry();
    virtual void        repaintSelection();
    
    virtual ~WidgetAreaView();

protected:
    WidgetAreaView(const WidgetAreaView&);
    WidgetAreaView& operator=(const WidgetAreaView&);
    QPixmap             do_grab(QWidget* widget) const;

    Common::OwnerPtr<SelectionRect>     selRect_;
    QPixmap                             pixmap_;
};

class ComboBoxAreaView : public WidgetAreaView {
public:
    ComboBoxAreaView(const Formatter::Area* area, QGraphicsScene* scene)
        : WidgetAreaView(area, scene) { updateGeometry(); }
    virtual QPixmap     grabWidget() const;
};

class LineEditAreaView : public WidgetAreaView {
public:
    LineEditAreaView(const Formatter::Area* area, QGraphicsScene* scene)
        : WidgetAreaView(area, scene) { updateGeometry(); }
    virtual QPixmap     grabWidget() const;
};

#endif //TEXT_AREA_VIEW_H
