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
#ifndef SCENEAREASET_TAG_H
#define SCENEAREASET_TAG_H

#include "sceneareaset/defs.h"
#include "common/String.h"
#include "common/OwnerPtr.h"
#include "formatter/decls.h"

#include <QGraphicsScene>
#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QColor>
#include <QPainter>
#include <QPolygonF>

#include <list>

namespace GroveLib
{
    class Element;
    class NodeWithNamespace;
}

typedef GroveLib::NodeWithNamespace NsNode;

namespace Formatter
{
    class ColorScheme;
    class Font;
}

class SceneTagMetrix;
class Cursor;

///////////////////////////////////////////////////////////////////

class CanvasPolygon : public QGraphicsPolygonItem {
public:
    CanvasPolygon(const QPolygonF& points, QGraphicsScene* scene);

    void            setCompositionMode(QPainter::CompositionMode op) 
        { compositionMode_ = op; }

protected:
    //void            paint(QPainter*, const QStyleOptionGraphicsItem*,
    //                      QWidget* = 0); 
private:
    QPainter::CompositionMode compositionMode_;
};

///////////////////////////////////////////////////////////////////

class AreaDecoration {
public:
    AreaDecoration(const QPointF& pos, const QPolygonF& points,
                   double z, QGraphicsScene* scene);
    virtual ~AreaDecoration() {};

    virtual void        move(double x, double y);
    virtual void        show() { body_.show(); }
    virtual void        hide() { body_.hide(); }
    void                setColor(QColor bodyColor, QColor outlineColor);
    virtual void        setSelected(bool isSelected);
    virtual void        setCursor(Cursor*) const = 0;
    QGraphicsScene*     scene() const { return body_.scene(); }

protected:
    CanvasPolygon       body_;
    QColor              bodyColor_;
    QColor              outlineColor_;
    bool                isSelected_;
};

typedef Common::OwnerPtr<AreaDecoration> AreaDecorationOwner;

class  SectionCorner : public AreaDecoration {
public:
    SectionCorner(const QPointF& pos, const QPolygonF& points,
                  double z, QGraphicsScene* scene, const QColor& color, 
                  const QColor& outlineColor);
    virtual ~SectionCorner() {}
    virtual void        setCursor(Cursor*) const;
};

class NamedDecoration : public AreaDecoration {
public:
    NamedDecoration(const QPolygonF& shape, const QPointF& namePos,
                    const Common::String& name, const Formatter::FontPtr& font,
                    double z, QGraphicsScene* scene);
    virtual ~NamedDecoration() {}

    void            move(double x, double y);
    void            show() { AreaDecoration::show(); name_.show(); }
    void            hide() { AreaDecoration::hide(); name_.hide(); }
    virtual void    setCursor(Cursor*) const;

protected:
    QGraphicsSimpleTextItem name_;
    QPointF                 namePos_;
};

////////////////////////////////////////////////////////////////////

class Tag : public AreaDecoration {
public:
    Tag(const QPointF& posF, const QPolygonF& shape, const QPointF& namePos,
         const NsNode* elem, const Formatter::ColorScheme* colorScheme,
         const Formatter::FontPtr& font, double z, QGraphicsScene* scene);

    virtual ~Tag() {}

    void            setColors(const NsNode* elem, 
                              const Formatter::ColorScheme* colorScheme);
    void            move(double x, double y);
    void            show() { AreaDecoration::show(); name_.show(); }
    virtual void    setCursor(Cursor*) const;
              
protected:
    virtual qreal cursorPos() const = 0;

protected:
    QGraphicsSimpleTextItem name_;
};

////////////////////////////////////////////////////////////////////

class StartTag : public Tag {
public:
    StartTag(const QPointF& pos, const NsNode* elem,
            const Formatter::ColorScheme* colorScheme,
            SceneTagMetrix* metrix,
            double z, QGraphicsScene* scene);

    virtual ~StartTag() {}

protected:
    qreal     cursorPos() const;
};

////////////////////////////////////////////////////////////////////

class EndTag : public Tag {
public:
    EndTag(const QPointF& pos, const NsNode* elem,
            const Formatter::ColorScheme* colorScheme, 
            SceneTagMetrix* metrix,
            double z, QGraphicsScene* scene);

    virtual ~EndTag() {}

protected:
    qreal     cursorPos() const { return 0; }
};

////////////////////////////////////////////////////////////////////

class EmptyTag : public Tag {
public:
    EmptyTag(const QPointF& pos, const NsNode* elem,
             const Formatter::ColorScheme* colorScheme,
             SceneTagMetrix* metrix,
             double z, QGraphicsScene* scene);

    virtual ~EmptyTag() {}

protected:
    qreal     cursorPos() const;

private:
    qreal     cursorPos_;
};

////////////////////////////////////////////////////////////////////

class ChoiceTag : public NamedDecoration {
public:
    ChoiceTag(SceneTagMetrix* metrix, double z, QGraphicsScene* scene, 
              const Common::String& name);
    virtual ~ChoiceTag() {}

    void            move(double x, double y);
    virtual void    setCursor(Cursor*) const;
    QPolygonF       getShape() const;

protected:
    CanvasPolygon   arrow_;
};

////////////////////////////////////////////////////////////////////

class FoldTag : public NamedDecoration {
public:
    FoldTag(const QPolygonF& shape, const QPointF& namePoint,
            const Formatter::FontPtr& font, 
            const Common::String& name,
            double z, 
            QGraphicsScene* scene);

    void             move(double x, double y);
    virtual void     setCursor(Cursor*) const;

protected:
    QPolygonF        getShape() const;

protected:
    CanvasPolygon   cross_;
};

#endif // SCENEAREASET_TAG_H
