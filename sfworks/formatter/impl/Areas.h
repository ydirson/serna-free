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
#ifndef FORMATTER_AREAS_H
#define FORMATTER_AREAS_H

#include "common/String.h"
#include "common/RangeString.h"
#include "formatter/formatter_defs.h"
#include "formatter/Area.h"

namespace Formatter
{

class Image;

typedef Common::String CString;

class RootArea : public Area {
public:
    FORMATTER_OALLOC(RootArea);

    RootArea(const Chain* traits);

    AreaType        type() const { return ROOT_AREA; }
    CType           allcW() const { return contRange().w_; }
    CType           allcH() const { return contRange().h_; }

    friend class    RootFo;

protected:
    CString          name() const;
};

////////////////////////////////////////////////////////////////////

class PageArea : public Area {
public:
    FORMATTER_OALLOC(PageArea);

    PageArea(const Chain* chain, const CRange& pageSize, uint pageNum);

    AreaType        type() const { return PAGE_AREA; }
    CType           allcW() const { return pageSize_.w_; }
    CType           allcH() const;

    bool            moderatesPos() const { return true; }
    uint            pageNum() const { return pageNum_; }

    friend class    PageFo;

protected:
    CString          name() const;
private:
    CRange          pageSize_;
    uint            pageNum_;
};

/* \brief
 */
class RegionArea : public Area {
public:
    FORMATTER_OALLOC(RegionArea);

    RegionArea(const Chain* chain);

    AreaType        type() const { return REGION_AREA; }
    CType           allcW() const { return contRange().w_; }
    CType           allcH() const;

    bool            moderatesPos() const { return true; }

    friend class    FlowFo;

protected:
    CString          name() const;
};

/*! \brief
 */
class BlockArea : public Area {
public:
    FORMATTER_OALLOC(BlockArea);

    BlockArea(const Chain* chain);

    AreaType        type() const { return BLOCK_AREA; }
    CType           allcW() const;
    CType           allcH() const;

    bool            moderatesPos() const;

    friend class    BlockLevelFo;
    friend class    CellAllocator;
    friend class    TableRowFo;
    friend class    ListItemFo;

protected:
    CString          name() const;
};

/*! \brief
 */
class TableRowArea : public BlockArea {
public:
    FORMATTER_OALLOC(TableRowArea);

    TableRowArea(const Chain* chain);
    //!
    AreaPos         mapToCursorPos(const CPoint& local) const;

protected:
    CString          name() const;
};

/* \brief
 */
class LineArea : public BlockArea {
public:
    FORMATTER_OALLOC(LineArea);

    LineArea(const Chain* chain);

    AreaType        type() const { return LINE_AREA; }
    bool            moderatesPos() const { return true; }
    int             cursorHeight() const { return (int)base_; };

    friend class    LineFo;

protected:
    CString          name() const;
};

/*! \brief
 */
class InlineArea : public Area {
public:
    FORMATTER_OALLOC(InlineArea);

    InlineArea(const Chain* chain);

    AreaType        type() const { return INLINE_AREA; }
    void            contX(CType borderLeft, CType paddingLeft);
    void            contW(CType w);
    void            contH(CType h);

    bool            moderatesPos() const;
    int             cursorHeight() const { return (int)base_; };

    CType           allcW() const;
    CType           allcH() const;

    friend class    InlineFo;

protected:
    CString          name() const;
};

/*! \brief
 */
class TextArea : public InlineArea {
public:
    FORMATTER_OALLOC(TextArea);

    TextArea(const Chain* chain, const Common::RangeString& text,
             const Common::String& hook, ulong pos);

    AreaType        type() const { return TEXT_AREA; }
    //!
    bool            moderatesPos() const;
    //!
    ulong           chunkPosCount() const;
    //!
    CRect           absCursorRect(ulong chunkPos) const;
    //!
    int             cursorHeight() const { return (int)base_; };
    //!
    AreaPos         mapToAreaPos(const CType localX, bool up) const;
    //!
    AreaPos         mapToCursorPos(const CPoint& local) const;
    //!
    const Common::RangeString& text() const { return text_; }
    //!
    ulong           pos() const { return pos_; }
    //! Reimplemented to check string equality
    bool            operator==(const Area& area) const;
    //!
    virtual void    dump(int indent = 0, bool recursively = true) const;

    friend class    TextFo;

protected:
    CString         name() const;
private:
    Common::RangeString text_;
    Common::String  textHook_;
    Rgb             textColor_;
    int             decoration_;
    ulong           pos_;
    bool            isGenerated_;
};

/*! \brief
 */
class GraphicArea : public InlineArea {
public:
    FORMATTER_OALLOC(GraphicArea);

    GraphicArea(const Chain* chain, const Image* image);

    AreaType        type() const { return GRAPHIC_AREA; }
    //!
    ulong           chunkPosCount() const { return 1; }
    //!
    //! Reimplemented to check string equality
    bool            operator==(const Area& area) const;

    friend class    GraphicFo;

protected:
    CString          name() const;
    const Image*     image_;
};

/*! \brief
 */
class SectionCornerArea : public InlineArea {
public:
    FORMATTER_OALLOC(SectionCornerArea);

    SectionCornerArea(const Chain* chain);

    AreaType        type() const { return SECTION_CORNER_AREA; }

    friend class    SectionCornerFo;

protected:
    CString          name() const;
};

/*! \brief
 */
class ChoiceArea : public InlineArea {
public:
    FORMATTER_OALLOC(ChoiceArea);

    ChoiceArea(const Chain* chain);

    AreaType        type() const { return CHOICE_AREA; }

    friend class    ChoiceFo;

protected:
    CString          name() const;
};

/*! \brief
 */
class CommentArea : public InlineArea {
public:
    FORMATTER_OALLOC(CommentArea);

    CommentArea(const Chain* chain);

    AreaType        type() const { return COMMENT_AREA; }

    friend class    CommentFo;

protected:
    CString          name() const;
};

/*! \brief
 */
class PiArea : public InlineArea {
public:
    FORMATTER_OALLOC(PiArea);

    PiArea(const Chain* chain);

    AreaType        type() const { return PI_AREA; }

    friend class    PiFo;

protected:
    CString          name() const;
};

/*! \brief
 */
class FoldArea : public InlineArea {
public:
    FORMATTER_OALLOC(FoldArea);

    FoldArea(const Chain* chain);

    AreaType        type() const { return FOLD_AREA; }

    friend class    FoldFo;

protected:
    CString          name() const;
};

/*! \brief
 */
class ComboBoxArea : public InlineArea {
public:
    FORMATTER_OALLOC(ComboBoxArea);
    typedef Common::Vector<CString> StringVector;
    
    ComboBoxArea(const Chain* chain, const CString& text,
                 const StringVector& valueList, bool isEditable, 
                 bool isEnabled)
        : InlineArea(chain),
          text_(text),
          valueList_(valueList),
          isEditable_(isEditable), 
          isEnabled_(isEnabled) {}

    AreaType        type() const { return COMBO_BOX_AREA; }
    //!
    const CString&   text() const { return text_; }
    const StringVector& valueList() const { return valueList_; }
    bool            isEditable() const { return isEditable_; }
    bool            isEnabled() const { return isEnabled_; }
    //!
    CRect           absCursorRect(uint chunkPos) const;
    //!
    int             cursorHeight() const;

    friend class    ComboBoxFo;

    //! Reimplemented to check string equality
    bool            operator==(const Area& area) const;

protected:
    CString          name() const;
    CString          text_;
    const StringVector& valueList_;
    bool            isEditable_;
    bool            isEnabled_;
};

/*! \brief
 */
class LineEditArea : public InlineArea {
public:
    FORMATTER_OALLOC(LineEditArea);
    typedef Common::Vector<CString> StringVector;
    
    LineEditArea(const Chain* chain, const CString& text, bool isEnabled)
        : InlineArea(chain),
          text_(text),
          isEnabled_(isEnabled) {}

    AreaType        type() const { return LINE_EDIT_AREA; }
    //!
    const CString&   text() const { return text_; }
    bool            isEnabled() const { return isEnabled_; }
    //!
    CRect           absCursorRect(uint chunkPos) const;
    //!
    int             cursorHeight() const;

    friend class    LineEditFo;

    //! Reimplemented to check string equality
    bool            operator==(const Area& area) const;

protected:
    CString          name() const;
    CString          text_;
    bool            isEnabled_;
};

/*! \brief
 */
class UnknownArea : public InlineArea {
public:
    FORMATTER_OALLOC(UnknownArea);

    UnknownArea(const Chain* chain);

    AreaType        type() const { return UNKNOWN_AREA; }

    friend class    UnknownFo;

protected:
    CString          name() const;
};


class InlineObject;

class FORMATTER_EXPIMP InlineObjectArea : public InlineArea {
public:
    FORMATTER_OALLOC(InlineObjectArea);

    InlineObjectArea(const Chain* chain);

    AreaType        type() const { return INLINE_OBJECT_AREA; }
    InlineObject*   inlineObject() const;

    friend class    InlineObjectFo;

protected:
    CString          name() const;
};

}

#endif // FORMATTER_AREAS_H
