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
#ifndef SCENEARESET_VIEW_H
#define SCENEARESET_VIEW_H

#include "editableview/EditableView.h"

#include "sceneareaset/defs.h"
#include "sceneareaset/SceneAreaFactory.h"
#include "sceneareaset/TagMetrixTable.h"
#include "sceneareaset/Tag.h"

#include <QDragMoveEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QTimer>
#include <QPrinter>
#include <QGraphicsView>
#include <QGraphicsLineItem>

class QInputMethodEvent;
class QKeyEvent;
class QHelpEvent;
class Cursor;

namespace Formatter
{
    class Area;
    class AreaPos;
    class FontMgr;
    class ImageProvider;
    class InlineObjectFactory;
}

//////////////////////////////////////////////////////////////////////

// TODO: need to separate QGraphicsView and EditableView, because:
//  1. there can be many views for single scene
//  2. editableView represents common, shared interface for all these views,
//     therefore items should know about (my_scene + editable_view).

class SceneView : public QGraphicsView, public EditableView {
    Q_OBJECT
public:
    SceneView(Common::Messenger* messenger,
              const GroveLib::GrovePtr& fot,
              EditPolicy* editPolicy = 0);
    virtual ~SceneView();

    //!
    void        format(const ViewParam& viewParam);
    //!
    bool        update();
    //!
    double      getZoom() const;
    QPoint      mapToGlobal(const Formatter::CPoint&) const;
    //!
    void        setZoom(double zoom);
    //!
    void        setContextHint(bool enabled, bool persistent,
                               bool showParent, uint delay);
    //!
    bool        showCursor();
    //!
    void        hideCursor();
    //!
    void        startCursor();
    //!
    void        stopCursor();
    //!
    void        grabFocus();
    //!
    void        print();
    //
    void        setMicroFocusHint();
    void        resetIM();
    //
    QWidget*    widget() { return this; }
    void        setParentWidget(QWidget* p) { QWidget::setParent(p); }
    //!
    void        setProgressStream(COMMON_NS::MessageStream* pstream);
    void        updateSize(const Formatter::Area* area);
    virtual void scrollByPage(bool isUp);

    static  Formatter::FontMgr* fontManager();

    virtual void setWatermark(const Common::String&);
    const Common::String& watermark() const { return watermark_; }

protected:
    Formatter::AreaPos toAreaPos(const QPoint&) const;

    //! Reimplemented from SceneView (QWidget)
    virtual void    mousePressEvent(QMouseEvent* e);
    virtual void    mouseReleaseEvent(QMouseEvent* e);
    virtual void    mouseDoubleClickEvent(QMouseEvent* e);
    virtual void    mouseMoveEvent(QMouseEvent* e);
    virtual void    keyPressEvent(QKeyEvent* e);
    virtual void    keyReleaseEvent(QKeyEvent* e);
    virtual void    focusInEvent(QFocusEvent* e);
    virtual void    focusOutEvent(QFocusEvent* e);
    virtual void    resizeEvent(QResizeEvent* e);
    virtual void    inputMethodEvent(QInputMethodEvent* event);

    virtual QVariant    inputMethodQuery(Qt::InputMethodQuery property) const;

    virtual void    dragEnterEvent(QDragEnterEvent* event);
    virtual void    dragMoveEvent(QDragMoveEvent* e);
    virtual void    dragLeaveEvent(QDragLeaveEvent* e);
    virtual void    dropEvent(QDropEvent* event);
    virtual bool    viewportEvent(QEvent*);

    // --------------------------
    // reimplemented from QAbstractScrollArea
    virtual void    scrollContentsBy(int, int);

    //!
    void        adjustView(const Formatter::AreaPos& pos, bool isTop);
    //!

    virtual Formatter::ImageProvider*           imageProvider();
    virtual Formatter::InlineObjectFactory*     inlineObjFactory();
    //!
    virtual const Formatter::AreaViewFactory* areaViewFactory() const;
    //!
    virtual Formatter::TagMetrixTable&  tagMetrixTable();
    //!
    bool        isBelowVisibleRange(const Formatter::Area* area);

    void        repaintView() { QGraphicsView::repaint(); }

    virtual Common::MessageStream* pstream() const;
private:
    SceneView(const SceneView&);
    SceneView& operator=(const SceneView&);

    void                            formatVisible();

    bool                            tooltip_event(const QHelpEvent&);
    bool                            keypress_event(QKeyEvent*);

    QTimer                          tripleClickTimer_;
    SceneAreaFactory                areaFactory_;
    SceneTagMetrixTable             tagMetrixTable_;
    bool                            dblClicked_;
    QTimer                          blinkTimer_;
    Common::OwnerPtr<Cursor>        cursorShape_;
    Common::OwnerPtr<Formatter::ImageProvider> imageProvider_;
    Common::OwnerPtr<Formatter::InlineObjectFactory> inlineObjFactory_;
    Common::String                  watermark_;
};

class EditPolicy;

namespace Common {
class Messenger;
}

SCENEAREASET_EXPIMP EditableView*
make_editable_view(Common::Messenger* messenger, const GroveLib::GrovePtr& fot,
                   EditPolicy* editPolicy);

#endif //SCENEARESET_VIEW_H
