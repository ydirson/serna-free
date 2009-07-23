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

#ifndef FORMATTER_AREA_H
#define FORMATTER_AREA_H

#include "common/RefCounted.h"
#include "common/XTreeNode.h"
#include "common/OwnerPtr.h"

#include "grove/Decls.h"

#include "formatter/decls.h"
#include "formatter/formatter_defs.h"
#include "formatter/AreaPos.h"
#include "formatter/AreaView.h"
#include "formatter/MediaInfo.h"
#include "formatter/FontMgr.h"
#include "formatter/TagMetrixTable.h"

namespace Formatter
{

enum AreaType {
    FIRST_AREA_TYPE,
    ROOT_AREA = FIRST_AREA_TYPE,
    PAGE_AREA,
    REGION_AREA,
    BLOCK_AREA,
    LINE_AREA,
    INLINE_AREA,
    TEXT_AREA,
    GRAPHIC_AREA,
    SECTION_CORNER_AREA,
    CHOICE_AREA,
    COMMENT_AREA,
    PI_AREA,
    FOLD_AREA,
    COMBO_BOX_AREA,
    LINE_EDIT_AREA,
    UNKNOWN_AREA,
    INLINE_OBJECT_AREA,
    LAST_AREA_TYPE = UNKNOWN_AREA
};

enum Decoration {
    NO_DECOR    = 0x00,
    START_DECOR = 0x01,
    EMPTY_DECOR = 0x02,
    END_DECOR   = 0x04,
    ANY_DECOR   = 0x07
};

/*! \brief Area is a base class for the formatting result tree
 */
enum PDirection {
    AMORPHOUS,
    VERTICAL,
    HORIZONTAL,
    PARALLEL
};

class Chain;

class FORMATTER_EXPIMP Area : public COMMON_NS::RefCounted<>,
                              public COMMON_NS::XTreeNode<Area,
                                     COMMON_NS::XTreeNodeRefCounted<Area> >,
                              public COMMON_NS::XListItem<Area> {
public:
    typedef COMMON_NS::XListItem<Area>              Chunk;
    typedef COMMON_NS::XTreeNode<Area,
            COMMON_NS::XTreeNodeRefCounted<Area> >  ATN;
    typedef COMMON_NS::OwnerPtr<AreaView>           AreaViewOwner;

    FORMATTER_OALLOC(Area);

    REDECLARE_XTREENODE_INTERFACE_BASE(ATN, Area);
    Area(const Chain* chain);
    virtual ~Area() {}

    //!
    virtual AreaType    type() const = 0;

//! Chunk operations
    //!
    const Chain*        chain() const;
    //!
    Area*               nextChunk() const;
    //!
    Area*               prevChunk() const;
    //!
    void                setFirstChunk(Chain* chain);
    //!
    void                insertChunkAfter(Area* area);
    //! Removes area from chain and from tree
    void                dispose();
    //!
    void                removeFromChain(bool recursively = false);
    //! Given area will be replaced with THIS area
    void                replace(const AreaPtr& area);

//! Representative operations
    //! Returns it`s view
    AreaView*           getView() const { return view_.pointer(); }
    //! Uses given factory to make it`s view
    void                makeView(const AreaViewFactory* factory);
    //!
    bool                hasDecoration(Decoration type) const;
    //! TODO: reimplement in terminal areas
    virtual bool        operator==(const Area& area) const;
    //!
    bool                operator!=(const Area& area) const;

//! Cursor positions
    //! Returns true if doesn`t allow cursor beside this area
    virtual bool        moderatesPos() const { return false; }
    //! Returns the number of possible positions in this chunk
    virtual ulong       chunkPosCount() const;
    //! Returns position of this area inside parent area
    //ulong               chunkPos() const;
    //! Returns the position of child chain inside this chunk
    AreaPos             areaPos(const Chain* chain) const;
    //! Returns the child chain at the given position inside chunk
    const Chain*        chainAt(ulong areaPos) const;
    //! Returns the child chain before the given position inside chunk
    const Chain*        chainBefore(ulong areaPos) const;
    //!
    virtual AreaPos     mapToAreaPos(const CType localX, bool up) const;
    //!
    virtual AreaPos     mapToCursorPos(const CPoint& local) const;
    //!
    bool                isProgression(PDirection direction) const;
//!
    //! Returns cursor rectangle relative to root area
    virtual CRect       absCursorRect(ulong chunkPos) const;
    //! TODO: replace with meaningful value
    virtual int         cursorHeight() const { return 20; };

//! Area geometry
    //! Returns allocation point (relative to the immediate parent)
    const CPoint&       allcPoint() const { return allcPoint_; }
    //!
    const CPoint&       absAllcPoint() const { return absAllcPoint_; }
    //! Returns the allocation rectangle width
    virtual CType       allcW() const = 0;
    //! Returns the allocation rectangle height
    virtual CType       allcH() const = 0;
    //! Upper-left corner of content rectangle
    const CPoint&       contPoint() const { return contPoint_; }
    //! The ranges of content rectangle
    const CRange&       contRange() const { return contRange_; }
    //! Returns the orientational border widths
    const ORect&        bord() const { return bord_; }
    //! Returns the orientational border widths
    CRect               bordRect() const;
    //! Returns the orientational padding widths
    const ORect&        padd() const { return padd_; }
    //! Returns baseline
    CType               base() const { return base_; }
    CType               baselineShift() const { return baseShift_; }

//! Geometry mapping
    //!
    CPoint              mapTo(const Area* ancestor,
                              const CPoint& content) const;
    //!
    CPoint              mapFrom(const Area* ancestor,
                                const CPoint& content) const;

    //! Prints the debugging info
    virtual void        dump(int indent = 0, bool recursively = true) const;
    //!
    virtual COMMON_NS::String   name() const = 0;

protected:
    //!
    void                replaceWith(Area* chunk);

    friend class Chain;
    friend class FoController;

    friend class FoImpl;
    friend class LineFo;
    friend class CellWrapperFo;

protected:
    const Chain*        chain_;
    CPoint              allcPoint_;
    CPoint              absAllcPoint_;
    CPoint              contPoint_;
    CRange              contRange_;
    ORect               bord_;
    ORect               padd_;
    CType               base_;
    CType               baseShift_;
    uint                decor_;

private:
    AreaViewOwner       view_;
};

enum FoType {
    FIRST_FO_TYPE,
    ROOT_FO = FIRST_FO_TYPE,
    PAGE_FO,
    FLOW_FO,
    BLOCK_FO,

    LIST_BLOCK_FO,
    LIST_ITEM_FO,
    LIST_ITEM_LABEL_FO,
    LIST_ITEM_BODY_FO,

    TABLE_FO,
    TABLE_CAPTION_FO,
    TABLE_COLUMN_FO,
    TABLE_HEADER_FO,
    TABLE_BODY_FO,
    TABLE_ROW_FO,
    CELL_WRAPPER_FO,
    TABLE_CELL_FO,

    LINE_FO,
    INLINE_FO,
    TEXT_FO,
    GRAPHIC_FO,
    SECTION_CORNER_FO,
    CHOICE_FO,
    COMMENT_FO,
    PI_FO,
    FOLD_FO,
    COMBO_BOX_FO,
    LINE_EDIT_FO,
    INLINE_OBJECT_FO,
    UNKNOWN_FO,
    LAST_FO_TYPE = UNKNOWN_FO
};

/*! \brief Chain keeps together areas made by single Fo

  Also Chain holds CommonTraits as described in section 4.2.2  of XSL specs.
 */
class FORMATTER_EXPIMP Chain : protected COMMON_NS::XList<Area> {
public:
    FORMATTER_OALLOC(Chain);

    Chain(bool hasParentOrigin)
        : hasParentOrigin_(hasParentOrigin),
          isGeometryModified_(false) {};
    virtual ~Chain() {};

    //!
    virtual FoType      type() const = 0;

    //!
    virtual bool        isModified() const = 0;
    //! Returns true if first area geometry modified during formatting
    bool                isGeometryModified() const;
    //!
    void                setGeometryModified();

//! Traverse in chunks
    //!
    Area*               firstChunk() const { return firstChild(); }
    //!
    Area*               lastChunk() const { return lastChild(); }
    //!
    void                setFirstChunk(Area* chunk);
    //!
    void                removeAllChunks(bool isToDelete);

//! Chain positions for AreaPos
    //!
    virtual bool        hasChildChains() const = 0;
    //!
    virtual Chain*      nextChain() const = 0;
    //!
    virtual Chain*      prevChain() const = 0;
    //!
    virtual Chain*      parentChain() const = 0;
    //!
    virtual Chain*      commonAos(const Chain* chain) const = 0;
    //! Returns the chain by it`s sequential number
    virtual Chain*      chainAt(ulong chainPos) const = 0;
    //! Returns the count of available positions
    virtual ulong       chainPosCount() const = 0;
    //! Returns position of this chain inside parent one
    virtual ulong       chainPos() const = 0;
    //!
    virtual void        treeloc(COMMON_NS::TreelocRep& tloc) const = 0;
    //!
    virtual Chain*      byTreeloc(COMMON_NS::TreelocRep& tloc) const = 0;

    //! See section 5.6 of XSL specs.
    virtual bool        isReference(CRange& contRange) const = 0;
    //!
    bool                hasParentOrigin() const { return hasParentOrigin_; }
    //! Returns progression for stacking of child areas
    virtual PDirection  progression() const = 0;

//! Chain traits
    //!
    virtual bool        isPreserveLinefeed() const { return false; }
    //! Returns the colors for all borders - top, bottom, left and right
    const OValue<Rgb>&  bordColor() const { return bordColor_; }
    //! Returns the color of the area content
    const Rgb&          contColor() const { return contColor_; }
    //! Returns the color of the area content
    const Rgb&          textColor() const { return textColor_; }
    //!
    virtual uint        level(bool physical = false) const = 0;
    //!
    virtual const MediaInfo&        mediaInfo() const = 0;
    //!
    virtual FontMgr*                fontMgr() const = 0;
    //!
    virtual TagMetrixTable&         tagMetrixTable() const = 0;

    //! Returns the first foNode to which this Fo belongs
    virtual const GroveLib::Node*   headFoNode() const = 0;
    //!Returns the last foNode to which this Fo belongs
    virtual const GroveLib::Node*   tailFoNode() const = 0;

    //!
    virtual COMMON_NS::String       name() const = 0;
    //!
    const FontPtr&      font() const { return font_; }
    //! Prints the debugging info
    virtual void        dump(int indent = 0) const;

protected:
    const bool          hasParentOrigin_;
    bool                isGeometryModified_;
    ORect               bord_;
    ORect               padd_;
    OValue<Rgb>         bordColor_;
    Rgb                 contColor_;
    Rgb                 textColor_;
    FontPtr             font_;
};

FORMATTER_EXPIMP Common::String node_name(const GroveLib::Node* foNode);
}

#endif // FORMATTER_AREA_H
