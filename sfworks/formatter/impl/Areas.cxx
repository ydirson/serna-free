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
/*! \file
 */

#include "common/safecast.h"

#include "formatter/impl/debug.h"

#include "formatter/XslFoExt.h"
#include "formatter/impl/Areas.h"
#include "formatter/impl/formatter_utils.h"
#include "formatter/impl/ReferencedFo.h"
#include "formatter/impl/TerminalFos.h"
#include "formatter/impl/TableFo.h"
#include "formatter/impl/PageMaster.h"

USING_COMMON_NS;
USING_GROVE_NAMESPACE;

namespace Formatter
{

/*
 */
RootArea::RootArea(const Chain* chain)
    : Area(chain)
{
}

String RootArea::name() const
{
    return "RootArea";
}

/*
 */
PageArea::PageArea(const Chain* chain, const CRange& pageSize, uint pageNum)
    : Area(chain),
      pageSize_(pageSize),
      pageNum_(pageNum)
{
}

CType PageArea::allcH() const
{
    const PageFo* page_fo = static_cast<const PageFo*>(chain());
    if (page_fo->data().mediaInfo_.isPaginated_)
        return pageSize_.h_;

    const PageSpecs& specs = *page_fo->pageSpecs();
    return (specs.margin().top_ + specs.margin().bottom_ + contRange_.h_);
}

String PageArea::name() const
{
    return "PageArea";
}

/*
 */
RegionArea::RegionArea(const Chain* chain)
    : Area(chain)
{
}

String RegionArea::name() const
{
    return "RegionArea";
}

CType RegionArea::allcH() const
{ 
    const FlowFo* flow_fo = static_cast<const FlowFo*>(chain());
    const RegionSpecs* region_specs = flow_fo->regionSpecs();

    return (region_specs->margin().top_ + region_specs->margin().bottom_ + 
            contRange_.h_);
//    return contRange().h_; 
}

/*
 */
BlockArea::BlockArea(const Chain* chain)
    : Area(chain)
{
}

bool BlockArea::moderatesPos() const
{
    return (0 == dynamic_cast<BlockArea*>(parent()) ||
            chain_->hasParentOrigin());
}

CType BlockArea::allcW() const
{
    CRange range;
    for (const Chain* c = chain_; c; c = c->parentChain()) {
        if (c->isReference(range))
            return range.w_;
    }
    return 0;
}

CType BlockArea::allcH() const
{
    return bord().top_ + bord().bottom_ + padd().top_ +
           padd().bottom_ + contRange().h_;
}

String BlockArea::name() const
{
    const BlockLevelFo* fo = dynamic_cast<const BlockLevelFo*>(chain_);
    if (fo)
        return fo->areaName();
    return "UnknownBlockArea";
}

/*
 */
TableRowArea::TableRowArea(const Chain* chain)
    : BlockArea(chain)
{
}

String TableRowArea::name() const
{
    return "TableRowArea";
}

AreaPos TableRowArea::mapToCursorPos(const CPoint& local) const
{
    const TableFo* table_fo = ancestor_table(static_cast<const Fo*>(chain_));
    if (table_fo) {
        CType x = local.x_;
        for (uint col = 1; col <= table_fo->maxColumn(); col++) {
            x -= table_fo->getColumnWidth(col);
            if (x <= 0) {
                const CellAllocator* allocator =
                    dynamic_cast<const CellAllocator*>(chain_);
                const Fo* cell_fo = allocator->getSpannedCell(col);
                if (0 == cell_fo)
                    break;
                for (Area* cell = cell_fo->firstChunk(); cell;
                     cell = cell->nextChunk()) {
                    CPoint cell_local(local - cell->allcPoint() -
                                      cell->contPoint());
                    CType dy = absAllcPoint().y_ - cell->absAllcPoint().y_;
                    cell_local.y_ += dy;
                    CRect  cell_bord_rect(cell->bordRect());
                    CPoint bord_top_left = cell_bord_rect.origin_;
                    CPoint bord_bott_right = cell_bord_rect.bottomRight();

                    if ((cell_local.y_ >= bord_top_left.y_) &&
                        (cell_local.y_ <= bord_bott_right.y_))
                        return cell->mapToCursorPos(cell_local);
                }
                break;
            }
        }
    }
    return BlockArea::mapToCursorPos(local);
}

/*
 */
LineArea::LineArea(const Chain* chain)
    : BlockArea(chain)
{
}

String LineArea::name() const
{
    return "LineArea";
}

/*
 */
InlineArea::InlineArea(const Chain* chain)
    : Area(chain)
{
    //!TODO: recover: rContPoint().y_ = 0;
}

bool InlineArea::moderatesPos() const
{
    return (chain_->hasParentOrigin());
}

CType InlineArea::allcW() const
{
    return bord().left_ + bord().right_ +
           padd().left_ + padd().right_ + contRange().w_;
}

CType InlineArea::allcH() const
{
    //! Using small allocation rectangle
    return contRange().h_;
}

String InlineArea::name() const
{
    return "InlineArea";
}

/*
 */
TextArea::TextArea(const Chain* chain, const RangeString& text, 
                   const String& hook, ulong pos)
    : InlineArea(chain),
      text_(text),
      textHook_(hook),
      textColor_(chain->textColor()),
      decoration_(SAFE_CAST(const TextFo*, chain)->textDecoration()),
      pos_(pos),
      isGenerated_(0 == XslFoExt::origin(chain->headFoNode()))
{
}

AreaPos TextArea::mapToAreaPos(const CType localX, bool) const
{
    const TextFo* fo = SAFE_CAST(const TextFo*, chain());
    TextChunk t_chunk = get_max_text_chunk(text_, localX, fo->font());
    if (t_chunk.text_.length() < text_.length()) {
        RangeString txt = text_.left(t_chunk.text_.length() + 1);
        CType w = fo->font()->width(txt);
        if (localX - t_chunk.width_ > w - localX) 
            return AreaPos(this, txt.length());
    }
    return AreaPos(this, t_chunk.text_.length());
}

AreaPos TextArea::mapToCursorPos(const CPoint& local) const
{
    AreaPos pos(mapToAreaPos(local.x_, true));
    if (text_.length() == pos.pos())
        pos.findAllowed(false, true);
    else
        pos.findAllowed(true, true);
    return pos;
}

String TextArea::name() const
{
    return "TextArea";
}

bool TextArea::operator==(const Area& area) const
{
    if (!Area::operator==(area))
        return false;
    const TextArea* text = SAFE_CAST(const TextArea*, &area);
    return (text->text_ == text_ &&
            text->pos_ == pos_ && 
            text->textColor_ == textColor_ && 
            text->decoration_ == decoration_);
}

ulong TextArea::chunkPosCount() const
{
    return (text_.length() + 1);
}

bool TextArea::moderatesPos() const
{
    return !isGenerated_;
}

CRect TextArea::absCursorRect(ulong chunkPos) const
{
    const TextFo* fo = SAFE_CAST(const TextFo*, chain());
    CType pos = fo->font()->width(text_.left(chunkPos));
    return CRect(absAllcPoint_ + contPoint_ + CPoint(pos, 0),
                 CRange(0, cursorHeight()));
}

void TextArea::dump(int indent, bool) const
{
    Area::dump(indent);
    DINDENT(indent);
    DINDENT(name().length());
    DBG(XSL.AREA) << " <" << text_.toString() << ">" << std::endl;
}

/////////////////////////////////////////////////////////////////////////////

CRect ComboBoxArea::absCursorRect(uint) const
{
    return CRect(absAllcPoint_ + contPoint_ + CPoint(2, 2),
                 CRange(2, cursorHeight()));
}

int ComboBoxArea::cursorHeight() const
{
    return int(contRange_.h_ - 4);
}

bool ComboBoxArea::operator==(const Area& area) const
{
    if (!Area::operator==(area))
        return false;
    const ComboBoxArea* text = SAFE_CAST(const ComboBoxArea*, &area);
    return (text->text_ == text_);
}

String ComboBoxArea::name() const
{
    return "ComboBoxArea";
}

/////////////////////////////////////////////////////////////////////////////

CRect LineEditArea::absCursorRect(uint) const
{
    return CRect(absAllcPoint_ + contPoint_ + CPoint(2, 2),
                 CRange(2, cursorHeight()));
}

int LineEditArea::cursorHeight() const
{
    return int(contRange_.h_ - 4);
}

bool LineEditArea::operator==(const Area& area) const
{
    if (!Area::operator==(area))
        return false;
    const LineEditArea* text = SAFE_CAST(const LineEditArea*, &area);
    return (text->text_ == text_);
}

String LineEditArea::name() const
{
    return "LineEditArea";
}

/*
 */
GraphicArea::GraphicArea(const Chain* chain,
                         const Image* image)
    : InlineArea(chain),
      image_(image) 
{
}

String GraphicArea::name() const
{
    return "GraphicArea";
}

bool GraphicArea::operator==(const Area& area) const
{
    if (!InlineArea::operator==(area))
        return false;
    const GraphicArea* graphic = SAFE_CAST(const GraphicArea*, &area);
    return (graphic->image_ && image_ && graphic->image_->isEqual(image_));
}

/*
 */
SectionCornerArea::SectionCornerArea(const Chain* chain)
    : InlineArea(chain)
{
}

String SectionCornerArea::name() const
{
    return "SectionCornerArea";
}

/*
 */
ChoiceArea::ChoiceArea(const Chain* chain)
    : InlineArea(chain)
{
}

String ChoiceArea::name() const
{
    return "ChoiceArea";
}

/*
 */
CommentArea::CommentArea(const Chain* chain)
    : InlineArea(chain)
{
}

String CommentArea::name() const
{
    return "CommentArea";
}

/*
 */
PiArea::PiArea(const Chain* chain)
    : InlineArea(chain)
{
}

String PiArea::name() const
{
    return "PiArea";
}

/*
 */
FoldArea::FoldArea(const Chain* chain)
    : InlineArea(chain)
{
}

String FoldArea::name() const
{
    return "FoldArea";
}

/*
 */
UnknownArea::UnknownArea(const Chain* chain)
    : InlineArea(chain)
{
}

String UnknownArea::name() const
{
    return "UnknownArea";
}

/////////////////////////////////////////////////////////////////////

InlineObjectArea::InlineObjectArea(const Chain* chain)
    : InlineArea(chain)
{
}

InlineObject* InlineObjectArea::inlineObject() const
{
    return static_cast<const InlineObjectFo*>(chain())->inlineObject();
}

String InlineObjectArea::name() const
{
    return "InlineObjectArea";
}

} // namespace Formatter
