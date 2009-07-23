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

#include "formatter/area_pos_utils.h"
#include "formatter/Area.h"
#include "formatter/impl/TableFo.h"

#include <iostream>

USING_COMMON_NS;

namespace Formatter 
{

AreaPos find_valid_pos(AreaPos areaPos, bool fwd)
{        
    while (!areaPos.isNull() && !areaPos.isValid()) {
        if (fwd)
            areaPos++;
        else
            areaPos--;
    }
    return areaPos;
}

AreaPos find_valid_pos(const AreaPos& areaPos, bool fwd, bool lookOpposite)
{
    if (areaPos.isNull())
        return areaPos;

    // Search in given direction
    AreaPos area_pos = find_valid_pos(areaPos, fwd);
    if (!area_pos.isNull()) 
        return area_pos;

    // Search in opposite direction
    if (lookOpposite) 
        area_pos = find_valid_pos(areaPos, !fwd);
    return area_pos;
}

bool has_ancestor_fo(AreaPos areaPos, FoType type)
{
    const Area* area = areaPos.area();
    while (area && area->chain()->type() != type)
        area = area->parent();
    return area;
}    

static const Chain* get_child_chain(ChainPos chainPos, bool findBefore)
{
    if (!findBefore) 
        return chainPos.chain()->chainAt(chainPos.pos());
    if (chainPos.pos() == 0)
        return 0;
    return chainPos.chain()->chainAt(chainPos.pos() - 1);
}

static const Chain* find_child_chain(ChainPos chainPos, bool findBefore,
                                     FoType type1, FoType type2 = UNKNOWN_FO) 
{
    while (!chainPos.isNull()) {
        const Chain* child_chain = get_child_chain(chainPos, findBefore);
        while (child_chain && child_chain->type() != type1 &&
               (type2 == UNKNOWN_FO || child_chain->type() != type2))
            child_chain = (findBefore) 
                ? child_chain->prevChain() : child_chain->nextChain();
        if (child_chain)
            return child_chain;        

        const Chain* next_chain = (findBefore) 
            ? chainPos.chain()->prevChain() : chainPos.chain()->nextChain();
        if (!next_chain)
            return 0;
        chainPos = ChainPos(next_chain, (findBefore) 
                            ? next_chain->chainPosCount() - 1 : 0);
    }
    return 0;
}

static Area* find_cell_in_row(ChainPos rowPos, bool findBefore)
{
    while (!rowPos.isNull()) {
        const Chain* cell_chain = 
            find_child_chain(rowPos, findBefore, TABLE_CELL_FO);
        
        if (cell_chain) 
            return (!findBefore) 
                ? cell_chain->firstChunk() : cell_chain->lastChunk();  

        const Chain* next_row = (findBefore) 
            ? rowPos.chain()->prevChain() : rowPos.chain()->nextChain();
        if (!next_row) {
            ChainPos pos(rowPos.chain()->parentChain(), (findBefore) 
                         ? rowPos.chain()->chainPos() 
                         : rowPos.chain()->chainPos() + 1);
            next_row = find_child_chain(pos, findBefore, TABLE_ROW_FO);
        }
        if (!next_row)
            return 0;
        rowPos = ChainPos(next_row, (findBefore) 
                          ? next_row->chainPosCount() - 1 : 0);
    }
    return 0;
}

static const Chain* find_row_in_table(AreaPos areaPos, bool findBefore)
{
    const Chain* body_chain = find_child_chain(
        areaPos, findBefore, TABLE_HEADER_FO, TABLE_BODY_FO);
    if (!body_chain) 
        return 0;
    ChainPos body_pos(body_chain, (findBefore) 
                      ? body_chain->chainPosCount() : 0);
    return find_child_chain(body_pos, findBefore, TABLE_ROW_FO);
}

static const Area* ancestor_cell_area(AreaPos areaPos)
{
    const Area* area = areaPos.area();
    while (area && area->chain()->type() != TABLE_CELL_FO)
        area = area->parent();
    return area;
}
    
AreaPos next_horizontal_cell_pos(AreaPos areaPos, bool findBefore)
{   
    Area* cell_area = 0;
    
    switch (areaPos.area()->chain()->type()) {
        case TABLE_ROW_FO: 
        case CELL_WRAPPER_FO: 
            {
                cell_area = find_cell_in_row(areaPos, findBefore);
                break;
            }
        case TABLE_HEADER_FO: 
        case TABLE_BODY_FO: 
            {
                cell_area = find_cell_in_row(
                    ChainPos(find_child_chain(areaPos, findBefore, 
                                              TABLE_ROW_FO), 0), 
                    findBefore);
                break;
            }
        case TABLE_FO: 
            {
                const Chain* row_chain = 
                    find_row_in_table(areaPos, findBefore);
                if (row_chain)
                    cell_area = find_cell_in_row(
                        ChainPos(row_chain, 0), findBefore);
            }   
            break;
        default: 
            {
                const Area* area = ancestor_cell_area(areaPos);
                if (!area)
                    return AreaPos();
                
                const Chain* cell_chain = area->chain();
                cell_area = find_cell_in_row(
                    ChainPos(cell_chain->parentChain(), (findBefore)
                             ? cell_chain->chainPos() 
                             : cell_chain->chainPos() + 1), 
                    findBefore);
            }
            break;
    }
    if (!cell_area)
        return AreaPos();
    return AreaPos(cell_area, (findBefore) 
                   ? cell_area->chunkPosCount() - 1 : 0);
}

//////////////////////////////////////////////////////////////////////////

static int persistent_column(const Area* area, CPoint persistent)
{
    while (area && TABLE_FO != area->chain()->type())
        area = area->parent();
    if (!area)
        return 0;
    const TableFo* table_fo = dynamic_cast<const TableFo*>(area->chain());
    if (!table_fo)
        return 0;
    CType x = persistent.x_ - area->absAllcPoint().x_ - area->contPoint().x_;
    for (uint col = 1; col <= table_fo->maxColumn(); col++) {
        CType w = table_fo->getColumnWidth(col);
        if (x <= w)
            return col;
        x -= w;
    }
    return table_fo->maxColumn();
}
    
static const Chain* find_cell(const Chain* rowChain, uint column)
{
    for (Chain* c = rowChain->chainAt(0); c; c = c->nextChain()) {
        const TableCellFo* cell_fo = dynamic_cast<const TableCellFo*>(c);
        if (!cell_fo)
            continue;
        if (column >= cell_fo->colNum() && 
            column < cell_fo->colNum() + cell_fo->colSpan())
            return c;
    }
    return 0;
}
    
static const Chain* next_chain(const Chain* chain, bool findBefore, 
                               FoType type)
{
    const Chain* next = (findBefore) ? chain->prevChain() : chain->nextChain();
    if (next)
        return next;
    ChainPos pos(chain->parentChain(), 
                 (findBefore) ? chain->chainPos() : chain->chainPos() + 1);
    return find_child_chain(pos, findBefore, type);
}
    
static Area* find_cell_in_column(const Chain* rowChain, int column, 
                                 bool findBefore)
{
    while (rowChain) {
        const Chain* cell_chain = find_cell(rowChain, column);
        if (cell_chain) 
            return (!findBefore) 
                ? cell_chain->firstChunk() : cell_chain->lastChunk();  
        rowChain = next_chain(rowChain, findBefore, TABLE_ROW_FO);
    }
    return 0;
}

static inline AreaPos get_valid(AreaPos areaPos, bool up, const Area* cell)
{
    if (areaPos.isNull())
        return areaPos; 
    AreaPos area_pos = areaPos;
    area_pos.findAllowed(!up, false);
    if (!area_pos.isNull() && ancestor_cell_area(area_pos) == cell) 
        return area_pos;
    area_pos = areaPos;
    area_pos.findAllowed(up, false);
    return area_pos;
}

AreaPos next_vertical_cell_pos(AreaPos areaPos, AreaPos persistentPos, 
                               bool findBefore)
{
    if (areaPos.isNull())
        return AreaPos();
    if (persistentPos.isNull())
        persistentPos = areaPos;
    
    const Area* cell_area = 0;
    const Chain* next_row = 0;
    
    CPoint persistent(persistentPos.area()->absCursorRect(
                          persistentPos.pos()).origin_);
    int persistent_col = persistent_column(areaPos.area(), persistent);
    

    switch (areaPos.area()->chain()->type()) {
        case TABLE_ROW_FO: 
        case CELL_WRAPPER_FO: 
            next_row = next_chain(areaPos.area()->chain(),
                                  findBefore, TABLE_ROW_FO);
            break;
        case TABLE_HEADER_FO: 
        case TABLE_BODY_FO: 
            next_row = find_child_chain(areaPos, findBefore, TABLE_ROW_FO);
            break;
        case TABLE_FO: 
            next_row = find_row_in_table(areaPos, findBefore);
            break;
        default: 
            {
                cell_area = ancestor_cell_area(areaPos);
                if (!cell_area)
                    return AreaPos();
                next_row = next_chain(cell_area->chain()->parentChain(), 
                                      findBefore, TABLE_ROW_FO);
            }
            break;
    }

    if (!next_row)
        return AreaPos();
    cell_area = find_cell_in_column(next_row, persistent_col, findBefore);
    if (!cell_area)
        return AreaPos();

    const Area* root_area = cell_area;
    while (root_area->parent())
        root_area = root_area->parent();

    CPoint local = cell_area->mapFrom(root_area, persistent);
    AreaPos area_pos = cell_area->mapToAreaPos(
        cell_area->mapFrom(root_area, persistent).x_, findBefore);
    return get_valid(area_pos, findBefore, cell_area);
}

} //! namespace Formatter
