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

#ifndef FORMATTER_TABLE_FO_H
#define FORMATTER_TABLE_FO_H

#include "formatter/impl/ReferencedFo.h"

namespace Formatter {

/*! \brief
 */
class TableColumnFo;

class TableFo : public BlockLevelFo {
public:
    FORMATTER_OALLOC(TableFo);
    typedef COMMON_NS::RefCntPtr<TableColumnFo> ColSpecPtr;
    typedef std::list<ColSpecPtr>               ColSpecList;
    typedef std::map<int, ColSpecPtr>           ColSpecMap;

    TableFo(const FoInit& init);
    
    //!
    FoType      type() const { return TABLE_FO; }
    //!
    bool        isReference(CRange& contRange) const;
    //!
    COMMON_NS::String   name() const;
    COMMON_NS::String   areaName() const;

    //!
    void        buildChildren();
    //! Reimplemented from ReferencedFo to track TableColumnFos
    void        childInserted(const GroveLib::Node* child);
    //! Reimplemented from ReferencedFo to track TableColumnFos
    void        childRemoved (const GroveLib::Node* node,
                              const GroveLib::Node* child);

    //! NOTE: all columns count from '1'
    CType       getColumnIndent(uint num) const;
    //! NOTE: all columns count from '1'
    CType       getColumnWidth(uint num) const;
    uint        maxColumn() const { return maxColumn_; }
    CType       proportionalFactor() const { return proportionalFactor_; }
    void        registerColSpecModification();
    void        registerRowModification();

protected:
    void        modifyCell(Fo* fo);
    //!
    void        calcProperties(const Allocation& alloc);
    //!
    Fo*         makeChildFo(const GroveLib::Node* foNode);
    //!
    void        sortColSpecs(const Allocation& alloc, uint maxCells);
    //!
    uint        maxCellsInRow(Fo* fo, const Allocation& alloc, uint cellNum);

private:
    ColSpecList colSpecs_;
    ColSpecMap  sortedSpecs_;
    CType       proportionalFactor_;
    bool        updateColSpecs_;
    CType       defaultColWidth_;
    CType       width_;
    uint        maxColumn_;
};

/*! \brief
 */
class TableColumnFo : public TerminalFo {
public:
    FORMATTER_OALLOC(TableColumnFo);

    TableColumnFo(const FoInit& init);

    FoType      type() const { return TABLE_COLUMN_FO; }
    //!
    bool        isEnoughSpace(const Allocation&, const Area*) const {
        RT_ASSERT("TableColumnFos don`t produce areas");
        return false;
    }
    //!
    Area*       makeArea(const Allocation&, const Area*, bool) const {
        RT_ASSERT("TableColumnFos don`t produce areas");
        return 0;
    }
    //!
    COMMON_NS::String   name() const;

    friend class TableFo;

    bool        proportionalColumnWidth(const FunctionArgList& arglist,
                                        ValueTypePair& pair,
                                        const PropertyContext*,
                                        const Allocation& alloc) const;
protected:
    //!
    void        calcProperties(const Allocation& alloc);
    //! Registers property modification
    void        registerPropertyModification();
private:
    uint        number_;
    uint        repeats_;
    CType       width_;
    CType       proportion_;
};

/*! \brief
 */
class TableBodyFo : public BlockLevelFo {
public:
    FORMATTER_OALLOC(TableBodyFo);

    TableBodyFo(const FoInit& init);

    //!
    FoType      type() const { return TABLE_BODY_FO; }
    //!
    COMMON_NS::String   name() const;
    COMMON_NS::String   areaName() const;

    //! Reimplemented from ReferencedFo to wrap CellFos
    void        childInserted(const GroveLib::Node* child);
    //! Reimplemented from ReferencedFo to wrap CellFos
    void        childRemoved (const GroveLib::Node* node,
                              const GroveLib::Node* child);
protected:
    //!
    void        calcProperties(const Allocation& alloc);
    //!
    Fo*         makeChildFo(const GroveLib::Node* foNode);
};

/*! \brief
 */
class TableCellFo;

class CellAllocator {
public:
    typedef std::list<Area*>        CellList;
    typedef COMMON_NS::Vector<uint> UintVector;

    virtual ~CellAllocator() {};

    void        allocateCells(const Allocation& alloc);
    CType       updateCells(const AreaPtr& area,
                            const Allocation& alloc) const;
    uint        maxOccupiedColumn() const { return occupationInfo_.size(); }
    TableCellFo*    getSpannedCell(uint column) const;
protected:
    void        updateCells(CellList& cellList, CType absY) const;
    void        updateUpperCells(const AreaPtr& area,
                                 const Allocation& alloc) const;
    void        checkNextRowGeometry() const;
    void        collectSpannedCells(const CellAllocator* cellAllocator,
                                    CellList& cellList, uint rowSpan,
                                    const Area* area) const;
    virtual CType   minHeight() const { return 0; }

public:
    virtual CellAllocator*  prevAllocator() const = 0;
    virtual CellAllocator*  nextAllocator() const = 0;
    virtual TableCellFo*    firstCell() const = 0;
    virtual Area*           firstRowChunk() const = 0;
    virtual Area*           lastRowChunk() const = 0;
    virtual void            setAllocatorModified() = 0;
private:
    UintVector  occupationInfo_;
};

/*
 */
class TableRowFo : public BlockLevelFo, public CellAllocator {
public:
    //typedef COMMON_NS::Vector<uint> UintVector;
    FORMATTER_OALLOC(TableRowFo);

    TableRowFo(const FoInit& init);

    FoType      type() const { return TABLE_ROW_FO; }
    //!
    bool        isParallel() const { return true; }
    //!
    CRect       getSpaceAfter(const Area* area, bool isFirst,
                              const Area* child, const CRect& space) const;
    //!
    bool        isEnoughSpace(const Allocation&, const Area*) const;
    //!
    Area*       makeArea(const Allocation& alloc, const Area* after,
                         bool forceToMake) const;
    //!
    void        updateGeometry(const AreaPtr& area, const Allocation& alloc,
                               const Area* prevChunk, bool isLast) const;
    //!
    PDirection  progression() const { return PARALLEL; }
    //!
    COMMON_NS::String   name() const;
    COMMON_NS::String   areaName() const;

    //!
    void        childInserted(const GroveLib::Node* child);
    //!
    void        childRemoved (const GroveLib::Node* node,
                              const GroveLib::Node* child);

protected:
    CellAllocator*  prevAllocator() const;
    CellAllocator*  nextAllocator() const;
    TableCellFo*    firstCell() const;
    Area*           firstRowChunk() const { return firstChunk(); }
    Area*           lastRowChunk() const { return lastChunk(); }
    void            setAllocatorModified();
    CType           minHeight() const { return height_; }

protected:
    //!
    void        calcProperties(const Allocation& alloc);
    //!
    CType       childrenHeight(const Area* area) const;
    //!
    Fo*         makeChildFo(const GroveLib::Node* foNode);

    friend class TableFo;
private:
    CType       height_;
};

/*! \brief
 */
class CellWrapperFo : public FoImpl, public CellAllocator {
public:
    FORMATTER_OALLOC(CellWrapperFo);

    CellWrapperFo(const FoInit& init);

    FoType      type() const { return CELL_WRAPPER_FO; }
    //!
    bool        isParallel() const { return true; }
    //!
    bool        isReference(CRange&) const { return false; }
    //!
    bool        isEnoughSpace(const Allocation& alloc,
                              const Area* after) const;
    //!
    Area*       makeArea(const Allocation& alloc, const Area* after,
                         bool forceToMake) const;
    //!
    CPoint      getAllcPoint(const CPoint& spaceAllc) const;
    //!
    CRect       getSpaceAfter(const Area* area, bool isFirst,
                              const Area* child, const CRect& space) const;
    //!
    void        updateGeometry(const AreaPtr& area, const Allocation& alloc,
                               const Area* prevChunk, bool isLast) const;
    //!
    uint        level(bool physical = false) const {
        return (physical) ? physicalLevel_ : logicalLevel_;
    }
    //! CellWrapperFo belongs to nodes of it`s child fos
    const GroveLib::Node*   headFoNode() const;
    //! CellWrapperFo belongs to nodes of it`s child fos
    const GroveLib::Node*   tailFoNode() const;

    //! CellWrapperFo buids it`s subtree in construction
    void        buildChildren() {};
    //!
    PDirection  progression() const { return PARALLEL; }
    //! Prints the debugging info
    void        dump(int indent) const;
    //! Uses parent fo to obtain properties
    template <class T> T&   getProperty(const Allocation& alloc,
                                        const CType percentBase = -1) {
        return parentSet_->getProperty<T>(alloc, percentBase);
    }
    //!
    COMMON_NS::String   name() const;
    COMMON_NS::String   areaName() const;

protected:
    CellAllocator*  prevAllocator() const;
    CellAllocator*  nextAllocator() const;
    TableCellFo*    firstCell() const;
    Area*           firstRowChunk() const { return firstChunk(); }
    Area*           lastRowChunk() const { return lastChunk(); }
    void            setAllocatorModified();

protected:
    //!
    void        calcProperties(const Allocation& alloc);
    //! NOTE: is copied from TableRowFo TODO: male it better
    CType       childrenHeight(const Area* area) const;
    //!
    bool        needsDecoration(Decoration) const { return false; }
private:
    //! LineFo has no origin node
    const GroveLib::Node*   node() const { return 0; }
private:
    CType       startIndent_;
    CType       endIndent_;
    uint        physicalLevel_;
    uint        logicalLevel_;
};

/*! \brief
 */
class TableCellFo : public BlockFo {
public:
    FORMATTER_OALLOC(TableCellFo);

    TableCellFo(const FoInit& init);

    CType       contentHeight(const Area* area) const;
    //!
    FoType      type() const { return TABLE_CELL_FO; }
    //!
    bool        isReference(CRange& contRange) const;
    //!
    COMMON_NS::String   name() const;
    COMMON_NS::String   areaName() const;
    //!
    bool        startsRow() const { return startsRow_; }
    bool        endsRow() const { return endsRow_; }
    //!
    uint        colNum() const { return columnNumber_; }
    uint        colSpan() const { return colSpan_; }

    friend class TableFo;
    friend class TableRowFo;
    friend class CellAllocator;

protected:
    //!
    void        calcProperties(const Allocation& alloc);
    void        calcMainProps(const Allocation& alloc);
    //! Registers property modification
    void        registerPropertyModification();

private:
    int         columnNumber_;
    uint        colSpan_;
    uint        rowSpan_;
    bool        startsRow_;
    bool        endsRow_;
};

inline const TableFo* ancestor_table(const Fo* fo)
{
    while (fo) {
        if (TABLE_FO == fo->type())
            return static_cast<const TableFo*>(fo);
        fo = fo->parent();
    }
    return 0;
}

} // namespace Formatter

#endif // FORMATTER_REFERENCED_FO_H
