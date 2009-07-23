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
#include "sceneareaset/SceneAreaView.h"
#include "sceneareaset/Tag.h"
#include "sceneareaset/TagMetrixTable.h"
#include "sceneareaset/Cursor.h"
#include "sceneareaset/utils.h"
#include "sceneareaset/debug.h"

#include "formatter/Area.h"
#include "formatter/Font.h"
#include "formatter/XslFoExt.h"
#include "formatter/impl/Areas.h"
#include "formatter/impl/TerminalFos.h"

#include "grove/Nodes.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QPen>
#include <QPolygonF>
#include <QApplication>

using namespace Common;
using namespace GroveLib;
using namespace Formatter;

SceneAreaView::~SceneAreaView()
{
}

void SceneAreaView::setCursorPos(Cursor* cursor, uint pos) const
{
    cursor->setDefaultCursorPos(area()->absCursorRect(pos));
}

/////////////////////////////////////////////////////////////////////////

ContentAreaView::ContentAreaView(const Area* area, QGraphicsScene* scene)
    : SceneAreaView(area, scene)
{
    updateGeometry();
}

ContentAreaView::~ContentAreaView()
{
}

void ContentAreaView::updateGeometry()
{
    updateContent();
}

void ContentAreaView::updateContent()
{
    const Rgb color = area()->chain()->contColor();

    bool debug_mode = false;
    DBG_IF(GV.TEST)
        debug_mode = true;

    if (debug_mode || !color.isTransparent_) {
        if (!contRect_.isNull())
            update_scene_rect(contRect_.pointer(), area_alloc_rect(area()),
                               color);
        else
            contRect_ = make_scene_rect(scene_, area_alloc_rect(area()),
                                         color, area()->chain()->level(true));
        if (debug_mode)
            contRect_->setPen(QPen(QColor(0,0,0)));
    }
    else
        contRect_ = 0;
}

/////////////////////////////////////////////////////////////////////////

TaggedAreaView::TaggedAreaView(const Area* area, QGraphicsScene* scene)
    : ContentAreaView(area, scene)
{
    updateBorders();
    updateTags();
}

TaggedAreaView::~TaggedAreaView()
{
}

void TaggedAreaView::setCursorPos(Cursor* cursor, uint area_pos) const
{
    if (emptyTag_ && 0 == area_pos)
        return emptyTag_->setCursor(cursor);
    if (startTag_ && 0 == area_pos)
        return startTag_->setCursor(cursor);
    if (endTag_ && area()->chunkPosCount() - 1 == area_pos)
        return endTag_->setCursor(cursor);
    return ContentAreaView::setCursorPos(cursor, area_pos);
}

void TaggedAreaView::updateGeometry()
{
    ContentAreaView::updateGeometry();
    updateBorders();
    updateTags();
}

void TaggedAreaView::repaintSelection()
{
    if (emptyTag_)
        emptyTag_->setSelected(selection_.extendsToLeft() ||
                               selection_.extendsToRight());
    if (startTag_)
        startTag_->setSelected(selection_.extendsToLeft());
    if (endTag_)
        endTag_->setSelected(selection_.extendsToRight());
}

void TaggedAreaView::updateBorders()
{
    const  OValue<Rgb>& color = area()->chain()->bordColor();
    CPoint allc = area()->absAllcPoint();
    CRect  cont(allc + area()->contPoint(), area()->contRange());
    ORect  bord = area()->bord();
    ORect  padd = area()->padd();
    double z = area()->chain()->level(true) + 0.1;

    CType allc_w = cont.extent_.w_ + padd.left_ + bord.left_ +
        padd.right_ + bord.right_;
    CType allc_h = cont.extent_.h_ + padd.top_ + bord.top_ +
        padd.bottom_ + bord.bottom_;
    CType allc_x = cont.origin_.x_ - padd.left_ - bord.left_;
    //! Top
    if (bord.top_ && !color.top_.isTransparent_) {
        CType y = cont.origin_.y_ - padd.top_ - bord.top_;
        CRect rect(CPoint(allc_x, y), CRange(allc_w, bord.top_));
        if (border_.top_)
            update_scene_rect(border_.top_.pointer(), rect, color.top_);
        else
            border_.top_ = make_scene_rect(scene_, rect, color.top_, z);
    }
    else
        border_.top_ = 0;
    //! Bottom
    if (bord.bottom_ && !color.bottom_.isTransparent_) {
        CType y = cont.origin_.y_ + cont.extent_.h_ + padd.bottom_;
        CRect rect(CPoint(allc_x, y), CRange(allc_w, bord.bottom_));
        if (border_.bottom_)
            update_scene_rect(border_.bottom_.pointer(), rect, color.bottom_);
        else
            border_.bottom_ =
                make_scene_rect(scene_, rect, color.bottom_, z);
    }
    else
        border_.bottom_ = 0;

    //! Left
    if (bord.left_ && !color.left_.isTransparent_) {
        CType x = cont.origin_.x_ - padd.left_ - bord.left_;
        CType y = cont.origin_.y_ - padd.top_ - bord.top_;
        CRect rect(CPoint(x, y), CRange(bord.left_, allc_h));
        if (border_.left_)
            update_scene_rect(border_.left_.pointer(), rect, color.left_);
        else
            border_.left_ = make_scene_rect(scene_, rect, color.left_, z);
    }
    else
        border_.left_ = 0;
    //! Rigth
    if (bord.right_ && !color.right_.isTransparent_) {
        CType x = cont.origin_.x_ + cont.extent_.w_ + padd.right_;
        CType y = cont.origin_.y_ - padd.top_ - bord.top_;
        CRect rect(CPoint(x, y), CRange(bord.right_, allc_h));
        if (border_.right_)
            update_scene_rect(border_.right_.pointer(), rect, color.right_);
        else
            border_.right_ = make_scene_rect(scene_, rect, color.right_, z);
    }
    else
        border_.right_ = 0;
}

void TaggedAreaView::updateDecoration()
{
    const Chain* chain = area()->chain();
    const Node* origin = XslFoExt::origin(chain->headFoNode());
    const NsNode* elem = static_cast<const NsNode*>(origin);
    if (emptyTag_)
        emptyTag_->setColors(elem, chain->mediaInfo().colorScheme_);
    if (startTag_)
        startTag_->setColors(elem, chain->mediaInfo().colorScheme_);
    if (endTag_)
        endTag_->setColors(elem, chain->mediaInfo().colorScheme_);
}

void TaggedAreaView::updateTags()
{
    QRectF cont(qRect(CRect(area()->absAllcPoint() + area()->contPoint(),
                           area()->contRange())));
    ORect padd = area()->padd();
    const Chain* chain = area()->chain();
    const Node* origin = XslFoExt::origin(chain->headFoNode());

    const bool show_tags = chain->mediaInfo().isShowTags_;

    const NsNode* elem = static_cast<const NsNode*>(origin);
    String name = node_name(origin);

    Font* tag_font =
        chain->fontMgr()->getTagFont().pointer();
    SceneTagMetrix* metrix = static_cast<SceneTagMetrix*>
        (chain->tagMetrixTable().getMetrix(name, tag_font));

    double tag_z = TAG_Z;

    //! EmptyTag
    if (area()->hasDecoration(EMPTY_DECOR)) {
        QPointF p(cont.x() + 1, cont.y());
        if (emptyTag_)
            emptyTag_->move(p.x(), p.y());
        else
            emptyTag_ = new EmptyTag(p, elem, chain->mediaInfo().colorScheme_,
                                     metrix, tag_z, scene_);
    }
    else
        emptyTag_ = 0;

    //! StartTag
    if (show_tags && area()->hasDecoration(START_DECOR)) {
        QPointF p(int(cont.x() - padd.left_ + 1), int(cont.y() - padd.top_));
        if (startTag_)
            startTag_->move(p.x(), p.y());
        else
            startTag_ = new StartTag(p, elem, chain->mediaInfo().colorScheme_,
                                     metrix, tag_z, scene_);
    }
    else
        startTag_ = 0;

    //! EndTag
    if (show_tags && area()->hasDecoration(END_DECOR)) {
        QPoint p;
        if (INLINE_AREA == area()->type())
            p = QPoint(int(cont.x() + cont.width() + 1), int(cont.y()));
        else
            p = QPoint(int(cont.x() - padd.left_),
                int(cont.y() + cont.height()));
        if (endTag_)
            endTag_->move(p.x(), p.y());
        else
            endTag_ = new EndTag(p, elem, chain->mediaInfo().colorScheme_,
                                 metrix, tag_z, scene_);
    }
    else
        endTag_ = 0;
}

/////////////////////////////////////////////////////////////////////////

SectionCornerView::SectionCornerView(const Area* area, QGraphicsScene* scene)
    : SceneAreaView(area, scene)
{
    QRectF cont(qRect(CRect(area->absAllcPoint() + area->contPoint(),
                           area->contRange())));

    const Chain* chain = area->chain();
    String name = node_name(XslFoExt::origin(chain->headFoNode()));

    Font* tag_font =
        chain->fontMgr()->getTagFont().pointer();
    SceneTagMetrix* metrix = static_cast<SceneTagMetrix*>
        (chain->tagMetrixTable().getMetrix(name, tag_font));

    QPointF point(cont.x() + 2, cont.y());

    const Element* elem = static_cast<const Element*>(chain->headFoNode());
    bool is_start = false;
    const Attr* a = elem->attrs().getAttribute(NOTR("kind"));
    if (a)
        is_start = (NOTR("start") == a->value());
    a = elem->attrs().getAttribute(NOTR("type"));
    String type = a ? a->value() : String();
    
    if (NOTR("internal-entity") == type) {
        QColor color(0xff, 0x99, 0x00);
        if (is_start)
            corner_ = new SectionCorner(point, metrix->internalErs_,
                                        TAG_Z, scene_, color, Qt::black);
        else
            corner_ = new SectionCorner(point, metrix->internalEre_,
                                        TAG_Z, scene_, color, Qt::black);
    }
    else if (NOTR("external-entity") == type) {
        QColor color(0xfe, 0x75, 0xce);
        if (is_start)
            corner_ = new SectionCorner(point, metrix->externalErs_,
                                        TAG_Z, scene_, color, Qt::black);
        else
            corner_ = new SectionCorner(point, metrix->externalEre_,
                                        TAG_Z, scene_, color, Qt::black);
    }
    else if (NOTR("xinclude") == type || NOTR("xinclude-fallback") == type) {
        QColor color(0x89, 0xc0, 0xff);
        QColor outline(Qt::black);
        if (NOTR("xinclude-fallback") == type) {
            outline = color;
            color = Qt::black;
        }
        if (is_start)
            corner_ = new SectionCorner(point, metrix->xincludeStart_,
                                        TAG_Z, scene_, color, outline);
        else
            corner_ = new SectionCorner(point, metrix->xincludeEnd_,
                                        TAG_Z, scene_, color, outline);
    }
    else if (NOTR("marked-section") == type) {
        QColor color(0xfc, 0xff, 0xad);
        corner_ = new SectionCorner(point, metrix->markedSection_,
                                    TAG_Z, scene_, Qt::yellow, Qt::gray);
    }
    else { //if (NOTR("redline") == type) {
        QColor color(0xfc, 0xff, 0xad);
        if (is_start)
            corner_ = new SectionCorner(point, metrix->redlineStart_,
                                        TAG_Z, scene_, color, Qt::gray);
        else
            corner_ = new SectionCorner(point, metrix->redlineEnd_,
                                        TAG_Z, scene_, color, Qt::gray);
    }
}

void SectionCornerView::updateGeometry()
{
    QRectF cont(qRect(CRect(area()->absAllcPoint() + area()->contPoint(),
                           area()->contRange())));
    QPointF point(cont.x() + 2, cont.y());

    corner_->move(point.x(), point.y());
}

void SectionCornerView::repaintSelection()
{
    corner_->setSelected(selection_.extendsToLeft() ||
                         selection_.extendsToRight());
}

void SectionCornerView::setCursorPos(Cursor* cursor, uint) const
{
    corner_->setCursor(cursor);
}

/////////////////////////////////////////////////////////////////////////

ChoiceAreaView::ChoiceAreaView(const Area* area, QGraphicsScene* scene)
    : SceneAreaView(area, scene)
{
    QRectF cont(qRect(CRect(area->absAllcPoint() + area->contPoint(),
                           area->contRange())));
    const Chain* chain = area->chain();

    String name = qApp->translate("ChoiceAreaView", "Choose");
    Font* tag_font =
        chain->fontMgr()->getTagFont().pointer();
    SceneTagMetrix* metrix = static_cast<SceneTagMetrix*>
        (chain->tagMetrixTable().getMetrix(name, tag_font));

    choice_ = new ChoiceTag(metrix, TAG_Z, scene_, name);
    choice_->move(cont.x(), cont.y());
    choice_->show();
}

void ChoiceAreaView::repaintSelection()
{
    choice_->setSelected(selection_.extendsToLeft() ||
                         selection_.extendsToRight());
}

void ChoiceAreaView::updateGeometry()
{
    QRectF cont(qRect(CRect(area()->absAllcPoint() + area()->contPoint(),
                           area()->contRange())));
    choice_->move(cont.x(), cont.y());
}

void ChoiceAreaView::setCursorPos(Cursor* cursor, uint) const
{
    choice_->setCursor(cursor);
}

/////////////////////////////////////////////////////////////////////////

FoldAreaView::FoldAreaView(const Area* area, QGraphicsScene* scene)
    : SceneAreaView(area, scene)
{
    const FoldFo* chain = static_cast<const FoldFo*>(area->chain());
    const Node* origin = XslFoExt::origin(chain->headFoNode());
    String name = (chain->isShowName()) ? node_name(origin) : String();

    QRectF cont(qRect(CRect(area->absAllcPoint() + area->contPoint(),
                           area->contRange())));
    Font* tag_font = chain->fontMgr()->getTagFont().pointer();
    SceneTagMetrix* metrix = static_cast<SceneTagMetrix*>
        (chain->tagMetrixTable().getMetrix(name, tag_font));

    QPointF p(cont.x() + 1, cont.y());

    if (chain->isShowName())
        fold_ = new FoldTag(metrix->fold_, metrix->foldName_, metrix->font_,
                            name, TAG_Z, scene_);
    else {
        QPolygonF shape(metrix->fold_);
        qreal h = shape[2].y();
        shape[1] = QPointF(h, 0);
        shape[2] = QPointF(h, h);
        fold_ = new FoldTag(shape, metrix->foldName_, metrix->font_,
                            name, TAG_Z, scene_);
    }

    fold_->move(cont.x(), cont.y());
    fold_->show();
}

void FoldAreaView::repaintSelection()
{
    fold_->setSelected(selection_.extendsToLeft() ||
                       selection_.extendsToRight());
}

void FoldAreaView::updateGeometry()
{
    QRectF cont(qRect(CRect(area()->absAllcPoint() + area()->contPoint(),
                           area()->contRange())));
    fold_->move(cont.x(), cont.y());
}

void FoldAreaView::setCursorPos(Cursor* cursor, uint area_pos) const
{
    if (fold_ && 0 == area_pos)
        return fold_->setCursor(cursor);
    return SceneAreaView::setCursorPos(cursor, area_pos);
}
