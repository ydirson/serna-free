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

#include "formatter/impl/Areas.h"

#include "formatter/impl/TableFo.h"
#include "formatter/impl/TerminalFos.h"

#include "formatter/impl/Properties.h"
#include "formatter/impl/BorderProperties.h"
#include "formatter/XslMessages.h"

USING_GROVE_NAMESPACE;
USING_COMMON_NS;

namespace Formatter {

static inline void register_row_modification(Fo* fo)
{
    while (fo) {
        if (TABLE_FO == fo->type()) {
            static_cast<TableFo*>(fo)->registerRowModification();
            return;
        }
        fo = fo->parent();
    }
}

/*
 */
TableFo::TableFo(const FoInit& init)
    : BlockLevelFo(init),
      proportionalFactor_(0),
      updateColSpecs_(false),
      defaultColWidth_(0),
      width_(0),
      maxColumn_(1)
{
}

void TableFo::sortColSpecs(const Allocation& alloc, uint maxCells)
{
    sortedSpecs_.clear();
    if (colSpecs_.empty())
        return;

    ColSpecMap sorted_tmp;
    for (ColSpecList::const_iterator i = colSpecs_.begin();
          i != colSpecs_.end(); i++) {
        (*i)->checkProperties(alloc);
        sorted_tmp[(*i)->number_] = (*i);
    }
    //! TODO: make following iterator CONST (compile error on windows)
    ColSpecMap::reverse_iterator last_spec = sorted_tmp.rbegin();
    maxColumn_ =
        (*last_spec).second->number_ + (*last_spec).second->repeats_ - 1;
    if (maxCells > maxColumn_)
        maxColumn_ = maxCells;

    ColSpecMap::const_iterator cur_spec = sorted_tmp.begin();
    ColSpecMap::const_iterator next_spec = cur_spec;
    next_spec++;
    for (uint c = 1; c <= maxColumn_; c++) {
        if (sorted_tmp.end() != next_spec &&
            c == (*next_spec).second->number_) {
            cur_spec++;
            next_spec++;
        }
        sortedSpecs_[c] = (*cur_spec).second;
    }
}

uint TableFo::maxCellsInRow(Fo* fo, const Allocation& alloc, uint cellNum)
{
    if (TABLE_ROW_FO == fo->type() || CELL_WRAPPER_FO == fo->type()) {
        CellAllocator* cell_allocator = dynamic_cast<CellAllocator*>(fo);
        RT_ASSERT(cell_allocator);
        cell_allocator->allocateCells(alloc);
        return cell_allocator->maxOccupiedColumn();
    }
    if (this != fo)
        fo->buildChildren();
    if (fo->firstChild()) {
        for(Fo* c = fo->firstChild(); c; c = c->nextSibling()) {
            uint num = maxCellsInRow(c, alloc, cellNum);
            if (cellNum < num)
                cellNum = num;
        }
    }
    return cellNum;
}

void TableFo::calcProperties(const Allocation& alloc)
{
    BlockLevelFo::calcProperties(alloc);

    uint max_cells = maxCellsInRow(this, alloc, 1);
    if (colSpecs_.empty()) {
        maxColumn_ = max_cells;
        contentWidth_ = alloc.space_.extent_.w_ - startIndent_ - endIndent_;
        defaultColWidth_ = contentWidth_ / maxColumn_;
        proportionalFactor_ = 0;
        updateColSpecs_ = false;
        return;
    }
    sortColSpecs(alloc, max_cells);

    CType col_width = 0;
    CType prop_width = 0;
    for (uint c = 1; c <= maxColumn_; c++) {
        TableColumnFo* column = sortedSpecs_[c].pointer();
        if (0 == column->proportion_ && 0 == column->width_)
            prop_width += 1;
        else {
            if (0 != column->proportion_)
                prop_width += column->proportion_;
            else
                col_width += column->width_;
        }
    }
    if (prop_width) {
        CType width =
            alloc.space_.extent_.w_ - startIndent_ - endIndent_ - col_width;
        const CType factor = width / prop_width;
        if (factor != proportionalFactor_) {
            proportionalFactor_ = factor;
            updateColSpecs_ = true;
            // Recalculating column specs
            for (ColSpecList::iterator i = colSpecs_.begin();
                 i != colSpecs_.end(); i++) {
                (*i)->notifyPropertyChanged("TableFo::proportional-factor");
                (*i)->checkProperties(alloc);
                if (0 == (*i)->width_)
                    (*i)->width_ = proportionalFactor_;
            }
        }
        if (width_ != width) {
            width_ = width;
            updateColSpecs_ = false;
            registerColSpecModification();            
        }
        contentWidth_ = 0;
    }
    else
        contentWidth_ = col_width;

    updateColSpecs_ = false;
}

bool TableFo::isReference(CRange& contRange) const
{
    contRange = CRange(contentWidth_, CTYPE_MAX);
    return true;
}

CType TableFo::getColumnIndent(uint num) const
{
    CType indent = 0;
    for (uint c = 1; c < num; c++)
        indent += getColumnWidth(c);
    return indent;
}

CType TableFo::getColumnWidth(uint num) const
{
    ColSpecMap::const_iterator i = sortedSpecs_.find(num);
    if (sortedSpecs_.end() != i)
        return (*i).second->width_;
    if (!sortedSpecs_.empty()) {
        DBG(XSL.TEST) << "Falback: last col_spec used for cell\n";
        return (*sortedSpecs_.rbegin()).second->width_;
    }
    return defaultColWidth_;
}

void TableFo::buildChildren()
{
    //DBG(XSL.FODYN) << "buildChildren:" << name() << this
    //               << " parent:" << parent() << std::endl;
    if (empty() && colSpecs_.empty()) {
        for (Node* c = node()->firstChild(); c; c = c->nextSibling()) {
            Fo* child_fo = makeChildFo(c);
            if (child_fo) {
                if (TABLE_COLUMN_FO == child_fo->type())
                    colSpecs_.push_back(static_cast<TableColumnFo*>(child_fo));
                else
                    appendChild(child_fo);
            }
        }
    }
    trackChildren_ = true;
}

void TableFo::childInserted(const Node* child)
{
    DBG(XSL.FODYN) << "TableFo child fo_node inserted:" << std::endl;
    Fo* child_fo = makeChildFo(child);
    if (child_fo) {
        DBG_IF(XSL.FODYN) child_fo->dump(4);
        if (TABLE_COLUMN_FO == child_fo->type())
            //! TODO: insert in right position && registerModification
            colSpecs_.push_back(static_cast<TableColumnFo*>(child_fo));
        else
            insertChildFo(child_fo, child);
        registerColSpecModification();
    }
}

void TableFo::childRemoved(const Node*, const Node* child)
{
    DBG(XSL.FODYN) << "TableFo child fo_node removed:" << std::endl;
    Fo* removed_fo = getFoByNode(child);
    if (removed_fo) {
        DBG_IF(XSL.FODYN) removed_fo->dump(4);
        if (TABLE_COLUMN_FO == removed_fo->type()) {
            for (ColSpecList::iterator i = colSpecs_.begin();
                 i != colSpecs_.end(); i++) {
                if (removed_fo == (*i).pointer()) {
                    colSpecs_.remove(*i);
                    break;
                }
            }
            sortedSpecs_.clear();
        }
        else
            removeChildFo(removed_fo);
        registerColSpecModification();
    }
}

void TableFo::modifyCell(Fo* fo)
{
    if (TABLE_CELL_FO == fo->type()) {
        DBG(XSL.PROPDYN) << "Table cell modified:" << fo << std::endl;
        static_cast<TableCellFo*>(fo)->PropertySet::setModified();
    }
    else {
        for(Fo* child = fo->firstChild(); child; child = child->nextSibling())
            modifyCell(child);
    }
}

void TableFo::registerColSpecModification()
{
    if (!updateColSpecs_) {
        updateColSpecs_ = true;
        PropertySet::isModified_ = true;
        registerPropertyModification();
        modifyCell(this);
    }
}

void TableFo::registerRowModification()
{
    if (!updateColSpecs_)
        registerColSpecModification();
}

Fo* TableFo::makeChildFo(const Node* foNode)
{
    switch (foName(foNode)) {
        case TABLE_COLUMN :
        case TABLE_BODY :
        case TABLE_HEADER :
        case TABLE_FOOTER :
            return ReferencedFo::makeChildFo(foNode);
            break;
        case SECTION_CORNER :
        case CHOICE :
        case COMMENT:
        case PI :
        case FOLD :
        case UNKNOWN :
            {
                Fo* fo = ReferencedFo::makeChildFo(foNode);
                SAFE_CAST(ExtensionFo*, fo)->
                    setContentType(BLOCK_CONTENT);
                return fo;
            }
            break;
        default:
            break;
    }
    data_.mstream_ << XslMessages::foNotAllowed << Message::L_WARNING
                   << foNode->nodeName() << node()->nodeName();
    return 0;
}

String TableFo::name() const
{
    return "TableFo";
}

String TableFo::areaName() const
{
    return "TableArea";
}

/*
 */
TableColumnFo::TableColumnFo(const FoInit& init)
    : TerminalFo(init),
      number_(1),
      repeats_(1),
      width_(0)
{
    buildChildren();
}

bool TableColumnFo::proportionalColumnWidth(const FunctionArgList& arglist,
                                            ValueTypePair& pair,
                                            const PropertyContext* property,
                                            const Allocation&) const
{
    DBG(XSL.PROPDYN)
        << "using TableFo::proportional-factor: " << this << std::endl;
    const TableFo* table = static_cast<const TableFo*>(parentSet_);
    if (Value::V_NUMERIC == static_cast<int>(arglist[0].type_)) {
        pair.type_ = Value::V_NUMERIC;
        pair.value_.nval_ =
            arglist[0].value_.nval_ * table->proportionalFactor();
        const_cast<TableColumnFo*>(this)->useProperty(
            "TableFo::proportional-factor",
            &static_cast<const Property&>(*property));
        return true;
    }
    else
        DBG(XSL.PROPDYN)
            << "proportionalColumnWidth: unexpected argtype: "
            << arglist[0].type_ << std::endl;
    return false;
}

void TableColumnFo::calcProperties(const Allocation& alloc)
{
    number_ = 1;
    for (const Node* fo_node = node()->prevSibling(); fo_node;
         fo_node = fo_node->prevSibling()) {
        const Fo* fo = getFoByNode(fo_node);
        if (fo && TABLE_COLUMN_FO == fo->type()) {
            const TableColumnFo* column =
                static_cast<const TableColumnFo*>(fo);
            number_ += column->number_ + column->repeats_ - 1;
            break;
        }
    }
    repeats_ = (uint)getProperty<ColumnRepeats>(alloc).value();
    width_ = getProperty<ColumnWidth>(alloc).value();
    
    proportion_ = getProperty<ColumnWidth>(alloc).proportion();
}

void TableColumnFo::registerPropertyModification()
{
    DBG(XSL.PROPDYN)
        << "TableColumnFo property modified: " << this << std::endl;
    TableFo* table = static_cast<TableFo*>(parentSet_);
    if (table)
        table->registerColSpecModification();
}

String TableColumnFo::name() const
{
    return "TableColumnFo";
}

/*
 */
TableBodyFo::TableBodyFo(const FoInit& init)
    : BlockLevelFo(init)
{
    buildChildren();
}

void TableBodyFo::calcProperties(const Allocation& alloc)
{
    if (!data_.mediaInfo_.isShowTags_)
        contColor_ = getProperty<BackgroundColor>(alloc).value();
    else
        contColor_ = Rgb(0, 0, 0, true);
    bord_ = padd_ = ORect();
    startIndent_ = endIndent_ = 0;
}

void TableBodyFo::childInserted(const Node* child)
{
    if (TABLE_CELL != foName(child)) {
        /*! TODO: elaborate pseudo-row splitting if block-fo inserted
          between cells */
        //TODO: try to reuse BlockLevelFo::childInserted(child);
        Fo* child_fo = makeChildFo(child);
        if (child_fo) {
            DBG_IF(XSL.FODYN) child_fo->dump(4);
            insertChildFo(child_fo, child);
            register_row_modification(child_fo);
        }
        return;
    }
    if (TABLE_CELL == foName(child)) {
        DBG(XSL.FODYN) << "==========before==============" << std::endl;
        DBG_IF(XSL.FODYN) dump(4);

        TableCellFo* next_cell_fo =
            find_sibling_fo<TableCellFo, TableRowFo>(child->nextSibling(),
                                                     true);
        TableCellFo* prev_cell_fo =
            find_sibling_fo<TableCellFo, TableRowFo>(child->prevSibling(),
                                                     false);
        if (next_cell_fo) {
            TableCellFo* cell_fo = new TableCellFo(FoInit(
                this, child, data_, physicalLevel_ + 1,
                logicalLevel_ + 1, false));
            next_cell_fo->insertBefore(cell_fo);
            cell_fo->registerModification(THIS_FO_MODIFIED);
            register_row_modification(cell_fo);
            DBG(XSL.FODYN) << "---------after----------------" << std::endl;
            DBG_IF(XSL.FODYN) dump(4);
            DBG(XSL.FODYN) << "==============================" << std::endl;
            return;
        }
        if (prev_cell_fo) {
            TableCellFo* cell_fo = new TableCellFo(FoInit(
                this, child, data_, physicalLevel_ + 1,
                logicalLevel_ + 1, false));
            prev_cell_fo->insertAfter(cell_fo);
            cell_fo->registerModification(THIS_FO_MODIFIED);
            register_row_modification(cell_fo);
            DBG(XSL.FODYN) << "---------after----------------" << std::endl;
            DBG_IF(XSL.FODYN) dump(4);
            DBG(XSL.FODYN) << "==============================" << std::endl;
            return;
        }
        Fo* cell_wrapper = makeChildFo(child);
        if (!cell_wrapper)
            return;
        insertChildFo(cell_wrapper, child);
        register_row_modification(cell_wrapper);
        DBG(XSL.FODYN) << "----------after---------------" << std::endl;
        DBG_IF(XSL.FODYN) dump(4);
        DBG(XSL.FODYN) << "==============================" << std::endl;
    }
}

void TableBodyFo::childRemoved(const Node* node, const Node* child)
{
    DBG(XSL.FODYN)
        << "TableBodyFo:" << this << " child fo-node removed" << std::endl;
    Fo* removed_fo = getFoByNode(child);
    if (!removed_fo)
        return;
    DBG_IF(XSL.FODYN) removed_fo->dump(4);
    if (this != removed_fo->parent()) {
        CellWrapperFo* row = SAFE_CAST(CellWrapperFo*, removed_fo->parent());
        if (!removed_fo->nextSibling() && !removed_fo->prevSibling())
            removeChildFo(row);
        else
            row->removeChildFo(removed_fo);
    }
    else
        BlockLevelFo::childRemoved(node, child);
}

Fo* TableBodyFo::makeChildFo(const Node* foNode)
{
    switch (foName(foNode)) {
        case TABLE_ROW :
            return ReferencedFo::makeChildFo(foNode);
            break;
        case TABLE_CELL :
            return new CellWrapperFo(FoInit(this, foNode, data_,
                                     physicalLevel_ + 1, 
                                     logicalLevel_ + 1, false));
            break;
        case SECTION_CORNER :
        case CHOICE :
        case COMMENT:
        case PI :
        case FOLD :
        case UNKNOWN :
            {
                Fo* fo = ReferencedFo::makeChildFo(foNode);
                SAFE_CAST(ExtensionFo*, fo)->
                    setContentType(BLOCK_CONTENT);
                return fo;
            }
            break;
        default:
            break;
    }
    data_.mstream_ << XslMessages::foNotAllowed << Message::L_WARNING
                   << foNode->nodeName() << node()->nodeName();
    return 0;
}

String TableBodyFo::name() const
{
    return "TableBodyFo";
}

String TableBodyFo::areaName() const
{
    return "TableBodyArea";
}

/*
 */
static inline TableCellFo* next_cell(Fo* fo)
{
    RT_ASSERT(fo);
    fo = fo->nextSibling();
    while (fo) {
        if (TABLE_CELL_FO == fo->type())
            return static_cast<TableCellFo*>(fo);
        fo = fo->nextSibling();
    }
    return 0;
}

void CellAllocator::allocateCells(const Allocation&)
{
    typedef std::map<uint, uint> OccupationMap;

    OccupationMap occupied;
    //! Mark columns that are occupied by previous row
    CellAllocator* prev_allocator = prevAllocator();
    if (prev_allocator) {
        const uint prev_size = prev_allocator->occupationInfo_.size();
        for (uint c = 0; c < prev_size; c++) {
            const uint span_left = prev_allocator->occupationInfo_[c];
            if (span_left > 1)
                occupied[c + 1] = span_left - 1;

        }
    }
    //! Mark columns that are occupied by cells with explicit column-number
    Allocation dummy_alloc;
    for (TableCellFo* cell = firstCell(); cell; cell = next_cell(cell)) {
        //! TODO: checkProperties
        cell->recalcProperties(dummy_alloc);
        cell->calcMainProps(dummy_alloc);
        int col_num = cell->columnNumber_;
        if (-1 != col_num) {
            //! TODO: check overlap in case of colspan
            if (occupied.end() != occupied.find(col_num)) {
                //! Message: overlapping cell - fallback
                cell->columnNumber_ = -1;
            }
            else
                occupied[col_num] = cell->rowSpan_;
        }
    }
    //! Allocate cells without explicit column-number
    uint col_num = 1;
    for (TableCellFo* cell = firstCell(); cell; cell = next_cell(cell)) {
        if (-1 == cell->columnNumber_) {
            //! TODO: check overlap in case of colspan
            while (occupied.end() != occupied.find(col_num))
                col_num++;
            cell->columnNumber_ = col_num;
            for (uint c = col_num; c < col_num + cell->colSpan_; c++)
                occupied[c] = cell->rowSpan_;
            col_num += cell->colSpan_;
        }
    }
    //! Save column allocation
    const uint max_col = (!occupied.empty())
        ? (*occupied.rbegin()).first : 0;
    occupationInfo_.resize(max_col);
    for (uint c = 0; c < max_col; c++)
        occupationInfo_[c] = 0;
    for (OccupationMap::const_iterator i = occupied.begin();
         i != occupied.end(); i++)
        occupationInfo_[(*i).first - 1] = (*i).second;
}

TableCellFo* CellAllocator::getSpannedCell(uint column) const
{
    const CellAllocator* cellAllocator = prevAllocator();
    uint row_span = 1;
    while (cellAllocator) {
        for (TableCellFo* cell = cellAllocator->firstCell();
             cell; cell = next_cell(cell)) {
            if (row_span < cell->rowSpan_ &&
                cell->colNum() <= column &&
                (cell->colNum() + cell->colSpan() - 1) >= column)
                return cell;
        }
        //! Check whether the prev row lays at the prev page
        cellAllocator = cellAllocator->prevAllocator();
        row_span++;
    }
    return 0;
}

void CellAllocator::collectSpannedCells(const CellAllocator* cellAllocator,
                                        CellList& cellList, uint rowSpan,
                                        const Area* tableBody) const
{
    while (cellAllocator) {
        const Area* row_area = cellAllocator->lastRowChunk();
        const Area* row_parent = (row_area) ? row_area->parent() : 0;
        if (row_area && tableBody != row_parent)
            return;
        for (TableCellFo* cell = cellAllocator->firstCell();
             cell; cell = next_cell(cell)) {
            Area* cell_area = cell->lastChunk();
            if (cell_area && rowSpan == cell->rowSpan_ &&
                (0 == row_area ||  row_area == cell_area->parent())) {
                static_cast<BlockArea*>(cell_area)->contRange_.h_ =
                    cell->contentHeight(cell_area);
                cellList.push_back(cell_area);
            }
        }
        //! Check whether the prev row lays at the prev page
        cellAllocator = cellAllocator->prevAllocator();
        rowSpan++;
    }
}

void CellAllocator::updateCells(CellList& cellList, CType absY) const
{
    for (CellList::iterator i = cellList.begin(); i != cellList.end(); i++) {
        if (BLOCK_AREA == (*i)->type()) {
            BlockArea* block = static_cast<BlockArea*>(*i);
            const CType decor_dy = block->allcH() - block->contRange().h_;
            block->contRange_.h_ = absY - block->absAllcPoint().y_ - decor_dy;
            block->getView()->updateGeometry();
        }
    }
}

CType CellAllocator::updateCells(const AreaPtr& area,
                                 const Allocation& alloc) const
{
    CellList cell_list;
    //! Collect all cells of given area
    for (Area* c = area->firstChild(); c; c = c->nextSibling()) {
        if (TABLE_CELL_FO == c->chain()->type()) {
            const TableCellFo* fo =static_cast<const TableCellFo*>(c->chain());
            static_cast<BlockArea*>(c)->contRange_.h_ = fo->contentHeight(c);
            if (1 == fo->rowSpan_)
                cell_list.push_back(c);
        }
    }
    //! Collect cells which are spanned until THIS row
    collectSpannedCells(prevAllocator(), cell_list, 2, alloc.area_);
    CType max_y = area->absAllcPoint().y_ + minHeight();
    //! Find tallest cell
    CellList::iterator i = cell_list.begin();
    for (; i != cell_list.end(); i++) {
        //! Calculate contentHeight of cell according to its content
        const CType y = (*i)->absAllcPoint().y_ + (*i)->allcH();
        if (y > max_y)
            max_y = y;
    }
    //! Update spanned cell`s
    updateCells(cell_list, max_y);
    updateUpperCells(area, alloc);
    checkNextRowGeometry();
    return (max_y - area->absAllcPoint().y_);
}

void CellAllocator::updateUpperCells(const AreaPtr& area,
                                     const Allocation& alloc) const
{
    CellList cell_list;
    collectSpannedCells(prevAllocator(), cell_list, 1, alloc.area_);
    updateCells(cell_list, area->absAllcPoint().y_);
}

typedef std::map<uint, CellAllocator*> AllocatorMap;
static inline CellAllocator* get_next_allocator(AllocatorMap& alloc_map,
                                                const CellAllocator* row,
                                                uint i)
{
    AllocatorMap::const_iterator iter = alloc_map.find(i);
    if (iter != alloc_map.end())
        return (*iter).second;
    CellAllocator* a = row->nextAllocator();
    for (uint idx = 1; a; a = a->nextAllocator(), idx++) {
        if (i == idx)
            break;
    }
    alloc_map[i] = a;
    return a;
}

void CellAllocator::checkNextRowGeometry() const
{
    AllocatorMap alloc_map;
    for (TableCellFo* cell = firstCell(); cell; cell = next_cell(cell)) {
        if (1 < cell->rowSpan_) {
            CellAllocator* allocator =
                get_next_allocator(alloc_map, this, cell->rowSpan_ - 1);
            const Area* row = (allocator) ? allocator->firstRowChunk() : 0;
            if (row && cell->lastChunk()) { //TODO: coredump on win without last check
                Area* cell_area = cell->lastChunk();
                const CType max_y = row->absAllcPoint().y_ + row->allcH();
                const CType cell_y =
                    cell_area->absAllcPoint().y_ + cell_area->allcH();
                if (max_y != cell_y)
                    allocator->setAllocatorModified();
            }
        }
    }
}

static inline CellAllocator* prev_allocator(const Fo* allocator)
{
    Fo* fo = allocator->prevSibling();
    while (fo) {
        if (TABLE_ROW_FO == fo->type())
            return static_cast<TableRowFo*>(fo);
        if (CELL_WRAPPER_FO == fo->type())
            return static_cast<CellWrapperFo*>(fo);
        fo = fo->prevSibling();
    }
    return 0;
}

static inline CellAllocator* next_allocator(const Fo* allocator)
{
    Fo* fo = allocator->nextSibling();
    while (fo) {
        if (TABLE_ROW_FO == fo->type())
            return static_cast<TableRowFo*>(fo);
        if (CELL_WRAPPER_FO == fo->type())
            return static_cast<CellWrapperFo*>(fo);
        fo = fo->nextSibling();
    }
    return 0;
}

/*
 */
TableRowFo::TableRowFo(const FoInit& init)
    : BlockLevelFo(init),
      height_(0)
{
    buildChildren();
}

CellAllocator* TableRowFo::prevAllocator() const
{
    return prev_allocator(this);
}

CellAllocator* TableRowFo::nextAllocator() const
{
    return next_allocator(this);
}

void TableRowFo::setAllocatorModified()
{
    registerModification(THIS_FO_MODIFIED);
}

TableCellFo* TableRowFo::firstCell() const
{
    Fo* child = firstChild();
    while (child) {
        if (TABLE_CELL_FO == child->type())
            return static_cast<TableCellFo*>(child);
        child = child->nextSibling();
    }
    return 0;
}

void TableRowFo::calcProperties(const Allocation& alloc)
{
    startIndent_ = endIndent_ = 0;
    bord_ = padd_ = ORect();
    if (!data_.mediaInfo_.isShowTags_)
        contColor_ = getProperty<BackgroundColor>(alloc).value();
    else
        contColor_ = Rgb(0, 0, 0, true);
    height_ = getProperty<Height>(alloc).value();
}

CRect TableRowFo::getSpaceAfter(const Area*, bool isFirst,
                                const Area*, const CRect& space) const
{
    CType bord = 0;
    CType padd = 0;
    CType h = (isFirst) ? getDecoration(START_DECOR, bord, padd) : 0;
    return CRect(CPoint(0, 0), CRange(space.extent_.w_, space.extent_.h_ - h));
}

bool TableRowFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    CType bord = 0;
    CType padd = 0;
    CType h = (empty()) ? getDecoration(EMPTY_DECOR, bord, padd) : 0;
    if (height_ > h)
        h = height_;
    return ((alloc.maxh_ || alloc.space_.extent_.h_ > h) &&
            (alloc.maxw_ || alloc.space_.extent_.w_ > contentWidth_));
}

Area* TableRowFo::makeArea(const Allocation& alloc, const Area* after,
                           bool forceToMake) const
{
    if (!forceToMake && !isEnoughSpace(alloc, after))
        return 0;

    BlockArea* block = new TableRowArea(this);
    block->allcPoint_ = alloc.space_.origin_;
    block->contRange_ = alloc.space_.extent_;

    return block;
}

void TableRowFo::updateGeometry(const AreaPtr& area, const Allocation& alloc,
                                const Area* prevChunk, bool) const
{
    area->dump();
    const CType row_height = updateCells(area, alloc);
    /*! WARNING: making changes in this code,
                 don`t forget to make it in InlineFo */
    CType space_left = alloc.space_.extent_.h_;
    bool  force_space = alloc.maxh_;

    BlockArea* block = SAFE_CAST(BlockArea*, area.pointer());
    //! Update allocation point
    block->allcPoint_ = alloc.space_.origin_;
    //! Update content range
    block->contRange_.w_ = contWidth(alloc.space_.extent_.w_);
    if (empty()) {
        settleEmptyBlock(block, prevChunk, force_space, space_left);
    }
    else {
        if (0 == row_height)
            block->contRange_.h_ = tagMetrix_->size(TagMetrix::EMPTY).h_;
        else
            block->contRange_.h_ = row_height;
        block->decor_ &= ~EMPTY_DECOR;
    }
}

CType TableRowFo::childrenHeight(const Area*) const
{
    return 0;
}

void TableRowFo::childInserted(const Node* child)
{
    BlockLevelFo::childInserted(child);
    register_row_modification(this);
}

void TableRowFo::childRemoved(const Node* parent, const Node* child)
{
    BlockLevelFo::childRemoved(parent, child);
    register_row_modification(this);
}

Fo* TableRowFo::makeChildFo(const Node* foNode)
{
    switch (foName(foNode)) {
        case TABLE_CELL :
            return ReferencedFo::makeChildFo(foNode);
            break;
        case SECTION_CORNER:
        case CHOICE:
        case COMMENT:
        case PI :
        case FOLD :
        case UNKNOWN:
            {
                Fo* fo = ReferencedFo::makeChildFo(foNode);
                SAFE_CAST(ExtensionFo*, fo)->
                    setContentType(BLOCK_CONTENT);
                return fo;
            }
            break;
        default:
            break;
    }
    data_.mstream_ << XslMessages::foNotAllowed << Message::L_WARNING
                   << foNode->nodeName() << node()->nodeName();
    DDBG << "Unexpected FO node in table-row: ";
    DBG_IF(XSL.TEST) foNode->dump();
    return 0;
}

String TableRowFo::name() const
{
    return "TableRowFo";
}

String TableRowFo::areaName() const
{
    return "TableRowArea";
}

/*
 */
CellWrapperFo::CellWrapperFo(const FoInit& init)
    : FoImpl(init.parentset_, false, init.data_),
      physicalLevel_(init.physicalLevel_),
      logicalLevel_(init.logicalLevel_)
{
    for (const Node* cell = init.foNode_; cell; cell = cell->nextSibling()) {
        if (Node::TEXT_NODE == cell->nodeType())
            continue;
        if (TABLE_CELL != foName(cell))
            break;
        TableCellFo* cell_fo =
            new TableCellFo(FoInit(parentSet_, cell, data_,
                            physicalLevel_ + 1, logicalLevel_ + 1, false));
        if (cell != init.foNode_ && cell_fo->startsRow()) {
            delete cell_fo;
            break;
        }
        appendChild(cell_fo);
        if (cell_fo->endsRow())
            break;
    }
}

CellAllocator* CellWrapperFo::prevAllocator() const
{
    return prev_allocator(this);
}

CellAllocator* CellWrapperFo::nextAllocator() const
{
    return next_allocator(this);
}

void CellWrapperFo::setAllocatorModified()
{
    firstChild()->registerModification(THIS_FO_MODIFIED);
}

TableCellFo* CellWrapperFo::firstCell() const
{
    Fo* child = firstChild();
    while (child) {
        if (TABLE_CELL_FO == child->type())
            return static_cast<TableCellFo*>(child);
        child = child->nextSibling();
    }
    return 0;
}

bool CellWrapperFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    return ((alloc.maxh_ || alloc.space_.extent_.h_ > 0) &&
            (alloc.maxw_ ||
             alloc.space_.extent_.w_ > startIndent_ + endIndent_));
}

Area* CellWrapperFo::makeArea(const Allocation& alloc, const Area* after,
                              bool forceToMake) const
{
    if (!forceToMake && !isEnoughSpace(alloc, after))
            return 0;
    BlockArea* block = new TableRowArea(this);
    block->allcPoint_ = alloc.space_.origin_;
    block->contRange_ = alloc.space_.extent_;
    return block;
}

CPoint CellWrapperFo::getAllcPoint(const CPoint& spaceAllc) const
{
    return spaceAllc;
}

CRect CellWrapperFo::getSpaceAfter(const Area*, bool, const Area* child,
                                   const CRect& space) const
{
    CType bottom_y = (child) ? child->allcPoint().y_ + child->allcH() : 0;
    return CRect(CPoint(0, bottom_y),
                 CRange(space.extent_.w_, space.extent_.h_ - bottom_y));
}

CType CellWrapperFo::childrenHeight(const Area*) const
{
    return 0;
}

void CellWrapperFo::updateGeometry(const AreaPtr& area,
                                   const Allocation& alloc,
                                   const Area*, bool) const
{
    const CType row_height = updateCells(area, alloc);

    CType space_left = alloc.space_.extent_.h_;
    BlockArea* block = SAFE_CAST(BlockArea*, area.pointer());
    //! Update allocation point
    block->allcPoint_ = alloc.space_.origin_;
    //! Update content range
    RT_MSG_ASSERT(!empty(), "CellWrapper should not be empty");
    block->contRange_.h_ = row_height;
    space_left -= block->contRange_.h_;
}

const Node* CellWrapperFo::headFoNode() const
{
    return firstChild()->headFoNode();
}

const Node* CellWrapperFo::tailFoNode() const
{
    return lastChild()->tailFoNode();
}

String CellWrapperFo::name() const
{
    return "CellWrapperFo";
}

String CellWrapperFo::areaName() const
{
    return "CellWrapperArea";
}

void CellWrapperFo::calcProperties(const Allocation&)
{
    bord_ = padd_ = ORect();
    contColor_ = Rgb(0, 0, 0, true);
}

void CellWrapperFo::dump(int indent) const
{
    DINDENT(indent);
    DBG(XSL.CHAIN) << "CellWrapperFo:" << this << std::endl;
    DBG_IF(XSL.CHAIN) Chain::dump(indent);

    for (Fo* child = firstChild(); child; child = child->nextSibling())
        DBG_IF(XSL.CHAIN) child->dump(indent);
}

/*
 */
TableCellFo::TableCellFo(const FoInit& init)
    : BlockFo(init),
      columnNumber_(-1),
      colSpan_(1),
      rowSpan_(1),
      startsRow_(false),
      endsRow_(false)
{
    Allocation alloc;
    calcMainProps(alloc);
    buildChildren();
}

CType TableCellFo::contentHeight(const Area* area) const
{
    if (area->hasDecoration(EMPTY_DECOR)) {
        CType bord = 0;
        CType padd = 0;
        return getDecoration(EMPTY_DECOR, bord, padd);
    }
    return childrenHeight(area);
}

void TableCellFo::registerPropertyModification()
{
    DBG(XSL.PROPDYN)
        << "TableCellFo property modified: " << this << std::endl;
    register_row_modification(this);
    BlockFo::registerPropertyModification();
}

void TableCellFo::calcMainProps(const Allocation& alloc)
{
    startsRow_ = getProperty<StartsRow>(alloc).value();
    endsRow_ = getProperty<EndsRow>(alloc).value();
    colSpan_ = (uint)getProperty<ColumnsSpanned>(alloc).value();
    rowSpan_ = (uint)getProperty<RowsSpanned>(alloc).value();
    columnNumber_ = (int)getProperty<ColumnNumber>(alloc).value();
}

void TableCellFo::calcProperties(const Allocation& alloc)
{
    RT_ASSERT(trackChildren_);

    BlockFo::calcProperties(alloc);
    getProperty<StartIndent>(alloc, -1, true).setValue(0);
    getProperty<EndIndent>(alloc, -1, true).setValue(0);
    startIndent_ = 0;
    endIndent_ = 0;

    colSpan_ = (uint)getProperty<ColumnsSpanned>(alloc).value();

    const TableFo* table = ancestor_table(parent());
    if (table) {
        RT_ASSERT((0 < columnNumber_));

        CType left = bord_.left_ + padd_.left_;
        CType right = bord_.right_ + padd_.right_;

        startIndent_ = table->getColumnIndent(columnNumber_) + left;
        contentWidth_ = - left - right;
        for (uint c = 0; c < colSpan_; c++)
            contentWidth_ += table->getColumnWidth(columnNumber_ + c);
    }
    else
        RT_MSG_ASSERT(false, "No table fo found");
}

bool TableCellFo::isReference(CRange& contRange) const
{
    contRange = CRange(contentWidth_, CTYPE_MAX);
    return true;
}

String TableCellFo::name() const
{
    return "TableCellFo";
}

String TableCellFo::areaName() const
{
    return "TableCellArea";
}

} // namespace Formatter
