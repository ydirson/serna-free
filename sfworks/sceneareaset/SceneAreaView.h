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
#ifndef SCENE_AREA_VIEW_H
#define SCENE_AREA_VIEW_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPolygonF>

#include "common/OwnerPtr.h"
#include "formatter/AreaView.h"
#include "sceneareaset/Tag.h"

class QGraphicsScene;

const int AREAVIEW_RTTI = QGraphicsItem::UserType + 10;

class SelectionRect: public QGraphicsRectItem {
public:
    SelectionRect(const QRectF& r, const Formatter::Rgb& textColor,
                  const Formatter::Rgb& contColor,
                  QGraphicsScene* scene, double z);
protected:
    virtual void        paint(QPainter*, const QStyleOptionGraphicsItem*,
                              QWidget*); 
private:
    QRectF              rect_;
    Formatter::Rgb      color_;
};

/////////////////////////////////////////////////////////////////

class SceneAreaView : public Formatter::AreaView {
public:
    SceneAreaView(const Formatter::Area* area, QGraphicsScene* scene)
        : AreaView(area), scene_(scene) {}

    virtual void                updateGeometry() = 0;
    virtual void                setCursorPos(Cursor*, uint pos) const;
    virtual ~SceneAreaView();
    
    QGraphicsScene*             scene() const { return scene_; }

protected:
    QGraphicsScene*            scene_;
};

/////////////////////////////////////////////////////////////////

class ContentAreaView : public SceneAreaView {
public:
    ContentAreaView(const Formatter::Area* area, QGraphicsScene* scene);
    virtual ~ContentAreaView();

    virtual void    updateGeometry();

protected:
    void            updateContent();

protected:
    COMMON_NS::OwnerPtr<QGraphicsRectItem> contRect_;
};

/////////////////////////////////////////////////////////////////

class TaggedAreaView : public ContentAreaView {
public:
    typedef COMMON_NS::OwnerPtr<QGraphicsRectItem>   BorderOwner;

    TaggedAreaView(const Formatter::Area* area, QGraphicsScene* scene);

    virtual void                setCursorPos(Cursor*, uint pos) const;
    void        updateGeometry();
    void        updateDecoration();
    void        repaintSelection();
    virtual     ~TaggedAreaView();

protected:
    void        updateBorders();
    void        updateTags();

private:
    Formatter::OValue<BorderOwner>  border_;
    Common::OwnerPtr<Tag>           startTag_;
    Common::OwnerPtr<Tag>           endTag_;
    Common::OwnerPtr<Tag>           emptyTag_;
};

/////////////////////////////////////////////////////////////////

class SectionCornerView : public SceneAreaView {
public:
    SectionCornerView(const Formatter::Area* area, QGraphicsScene* scene);

    void            updateGeometry();
    virtual void    setCursorPos(Cursor*, uint pos) const;

    void            repaintSelection();

private:
    Common::OwnerPtr<AreaDecoration> corner_;
};

/////////////////////////////////////////////////////////////////

class ChoiceAreaView : public SceneAreaView {
public:
    ChoiceAreaView(const Formatter::Area* area, QGraphicsScene* scene);

    void            updateGeometry();
    void            repaintSelection();
    virtual void    setCursorPos(Cursor*, uint pos) const;

private:
    Common::OwnerPtr<AreaDecoration> choice_;
};

/////////////////////////////////////////////////////////////////

class FoldAreaView : public SceneAreaView {
public:
    FoldAreaView(const Formatter::Area* area, QGraphicsScene* scene);

    void            updateGeometry();
    void            repaintSelection();
    virtual void    setCursorPos(Cursor*, uint pos) const;

private:
    Common::OwnerPtr<AreaDecoration> fold_;
};

#endif      // SCENE_AREA_VIEW_H
