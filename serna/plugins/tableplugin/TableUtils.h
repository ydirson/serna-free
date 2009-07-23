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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#ifndef TABLE_UTILS_H_
#define TABLE_UTILS_H_

#include "common/Vector.h"

namespace Common {
    class String;
    class PropertyNode;
}

namespace GroveLib {
    class Node;
    class Element;
}
class TableBatchCommand;

namespace Sui
{
extern const char* const TABLE_BUTTON;
extern const char* const TABLE_SIZE;
}

class TablePlugin;

namespace TableUtils {

//extern const char* const TABLE;
//extern const char* const TGROUP;
//extern const char* const THEAD;
//extern const char* const TBODY;
//extern const char* const TFOOT;
//extern const char* const FRAME;
//extern const char* const TITLE;
extern const char* const PARA;
extern const char* const COLS;
extern const char* const COLNAME;
extern const char* const COLNUM;
extern const char* const COLWIDTH;
extern const char* const COLALIGN;
extern const char* const COLSPEC;
extern const char* const SPANNAME;
extern const char* const COLSEP;
extern const char* const ROWSEP;
extern const char* const NAMEST;
extern const char* const NAMEEND;
extern const char* const ENTRY;

Common::String getNameByTable(const TablePlugin* plugin,
                              const Common::String& name,
                              bool returnOnlyOne = true);
Common::String row_name(const TablePlugin* plugin);
Common::String cell_name(const TablePlugin* plugin);
Common::String cell_header_name(const TablePlugin* plugin);
Common::String colspec_name(const TablePlugin* plugin);
Common::String spanspec_name(const TablePlugin* plugin);
Common::String title_name(const TablePlugin* plugin);
Common::String thead_name(const TablePlugin* plugin);
Common::String tbody_name(const TablePlugin* plugin);
Common::String tfoot_name(const TablePlugin* plugin);
Common::String table_name(const TablePlugin* plugin);
Common::String namespace_name(const TablePlugin* plugin);
Common::String root_name(const TablePlugin* plugin);
Common::String frame_name(const TablePlugin* plugin);
Common::String frame_values(const TablePlugin* plugin);
Common::String border_name(const TablePlugin* plugin);
Common::String border_width(const TablePlugin* plugin);
Common::String vspan_name(const TablePlugin* plugin);
Common::String hspan_name(const TablePlugin* plugin);

bool is_genid(const TablePlugin* plugin);
bool is_row (const TablePlugin*  plugin, const Common::String& name);
bool is_cell(const TablePlugin*  plugin, const Common::String& name);
bool is_thead(const TablePlugin*  plugin, const Common::String& name);
bool is_tbody(const TablePlugin*  plugin, const Common::String& name);
bool is_tfoot(const TablePlugin*  plugin, const Common::String& name);
bool vspan_counts_from_zero(const TablePlugin*  plugin);
bool hspan_has_cals_format (const TablePlugin*  plugin);
bool hspan_counts_from_zero(const TablePlugin*  plugin);
bool table_has_cols  (const TablePlugin*  plugin);
bool table_has_tgroup(const TablePlugin*  plugin);
bool colspec_has_name(const TablePlugin*  plugin);

int  columns(TablePlugin* plugin);
int  calculated_cols(TablePlugin* plugin);

class NodeList :
    public COMMON_NS::Vector<GroveLib::Node*> {};

int   rowCells(TablePlugin* plugin, GroveLib::Node* nodeCell,
               NodeList* rowSet = 0, NodeList* vspanSet = 0,
               bool takeTop = true,
               int size = -1);

//! fill list with cells in one column, calculated by nodeCell position.
//  return current row index
int   columnCells(TablePlugin* plugin, GroveLib::Node* nodeCell,
                  int column,
                  NodeList* columnSet = 0, NodeList* hspanSet = 0);

GroveLib::Node* specByNum(TablePlugin* plugin, int count,
                GroveLib::Node* root,
                bool* tableUseColnum = 0, bool* currentColnumExist = 0);

void getHSpanRange(TablePlugin* plugin,
                   GroveLib::Node* cell, int& start, int& end);
int  getVSpanRange(TablePlugin* plugin, GroveLib::Node* cell);
Common::String copyAttributes(GroveLib::Node* from,
                              GroveLib::Element* cell,
                              const Common::String& name,
                              int increment = 0);
void setExtraAttrs(TablePlugin* plugin, GroveLib::Node* cell, 
                   const char* name = ENTRY);
void copyHSpan(TablePlugin* plugin, GroveLib::Node* from,
               GroveLib::Element* cell);
void delHSpan(TablePlugin* plugin, TableBatchCommand* batch,
              GroveLib::Node* cell);
//Delete hspan for disconnected element
void delHSpan(TablePlugin* plugin, GroveLib::Element* elem);
void resizeHSpan(TablePlugin* plugin, TableBatchCommand* batch,
              GroveLib::Node* cell, bool prev, int val, int curCol);
void resizeRowSpans(TablePlugin* plugin, TableBatchCommand* batch,
                 GroveLib::Node* cell, int value, int cols = -1);
void rebuildCalsHSpan(TablePlugin* plugin, TableBatchCommand* batch,
              GroveLib::Node* resolver, const Common::String& name,
              const Common::String& oldName);
bool getSelectionCells(TablePlugin* plugin, GroveLib::Node*& start_cell,
                                            GroveLib::Node*& end_cell);
GroveLib::Node* is_in(const Common::String& name, GroveLib::Node* node);
bool check_support_level(TablePlugin* plugin, GroveLib::Node* node);
} // TableUtils

#endif // TABLE_UTILS_H_

