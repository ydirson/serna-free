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
#ifndef SCENEAREASET_CURSOR_H
#define SCENEAREASET_CURSOR_H

#include "sceneareaset/SceneView.h"
#include "sceneareaset/Tag.h"

class ContextTip
{
public:
    enum Type {
        PARENT,
        PREV_SIBLING,
        NEXT_SIBLING
    };
    typedef Common::OwnerPtr<QGraphicsLineItem> CanvasLineOwner;
    typedef Formatter::OValue<CanvasLineOwner>  OLineOwner;

    ContextTip(const GroveLib::Element* elem,
               const GroveLib::Node* foHint,
               Type type, QGraphicsScene* scene,
               const Formatter::Area* descendant = 0);

    void        show();
    void        hide();

protected:
    void        makeBorder(const Formatter::Area* area,
                           QGraphicsScene* scene, bool forAllChunks);
private:
    OLineOwner                      border_;
    AreaDecorationOwner             label_;
    Formatter::FontPtr              font_;
};

class Cursor : public QObject {
    Q_OBJECT
public:
    typedef Common::OwnerPtr<ContextTip> TipOwner;

    Cursor(QGraphicsScene* scene);

    void    setPos(const EditContext& context);
    void    show();
    void    hide();
    void    startCursor();
    void    stopCursor();
    void    setTipsVisible(bool isVisible) { isShowTips_ = isVisible; }
    void    setTips(bool enabled, bool persistent, bool showParent, uint delay);
    QRectF  rect() const;

    void    setShapePos(const QPointF&, const QPolygonF&);
    void    setDefaultCursorPos(const Formatter::CRect&);

protected:
    void    setTips(const GroveLib::Element* parent,
                    const GroveLib::Element* prev,
                    const GroveLib::Element* next,
                    const GroveLib::Node* foHint,
                    const Formatter::Area* descendant = 0);
private slots:
    //! Makes the cursor blinking
    void    blink();
    //!
    void    hideTips();

private:
    QGraphicsScene*             scene_;
    CanvasPolygon               shape_;

    QTimer                      blinkTimer_;
    QTimer                      tipTimer_;

    GroveEditor::GrovePos       src_;

    TipOwner                    parentTip_;
    TipOwner                    prevSiblingTip_;
    TipOwner                    nextSiblingTip_;

    bool                        isShowTips_;
    bool                        isPersistent_;
    bool                        isShowParent_;
    uint                        delay_;
};

#endif // SCENEAREASET_CURSOR_H

