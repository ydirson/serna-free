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

#include "TableUtils.h"
#include "TablePlugin.h"
#include "plugins/tableplugin/TableBatchCommand.h"
#include "grove/Nodes.h"
#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"
#include "structeditor/StructEditor.h"
#include "structeditor/GroveCommandEventData.h"
#include "editableview/EditableView.h"
#include "formatter/impl/Areas.h"

#include <qstringlist.h>

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;

class ExecuteAndUpdate;

namespace Sui
{
// START_IGNORE_LITERALS
extern const char* const TABLE_BUTTON = "TableButton";
extern const char* const TABLE_SIZE   = "#table-size";
// STOP_IGNORE_LITERALS
}

namespace TableUtils {

typedef const char* const exported_literal;

//extern exported_literal TABLE     = "table";
//extern exported_literal TGROUP    = "tgroup";
//extern exported_literal THEAD     = "thead";
//extern exported_literal TBODY     = "tbody";
//extern exported_literal TFOOT     = "tfoot";
//extern exported_literal FRAME     = "frame";
//extern exported_literal TITLE     = "title";


// START_IGNORE_LITERALS
extern exported_literal COLS      = "cols";
extern exported_literal PARA      = "para";
extern exported_literal COLNAME   = "colname";
extern exported_literal COLNUM    = "colnum";
extern exported_literal COLWIDTH  = "colwidth";
extern exported_literal COLALIGN  = "align";
extern exported_literal COLSPEC   = "colspec";
extern exported_literal SPANNAME  = "spanname";
extern exported_literal COLSEP    = "colsep";
extern exported_literal ROWSEP    = "rowsep";
extern exported_literal NAMEST    = "namest";
extern exported_literal NAMEEND   = "nameend";
extern exported_literal ENTRY     = "entry";
// STOP_IGNORE_LITERALS


String getNameByTable(const TablePlugin* plugin,
                      const String& name, bool returnOnlyOne)
{
    String result;
    TablePlugin* tplugin = const_cast<TablePlugin*>(plugin);
    TablePlugin::TableInfoSet& dset = tplugin->infoSet();
    TablePlugin::TableInfoSet::iterator info =  dset.find(name);
    if (info != dset.end())
        result = info->second->name[tplugin->currentTable()];
    if (returnOnlyOne) {
        QStringList lst = QStringList::split(',', result);
        if (lst.size())
            result = lst[0];
    }
    if (!result.isEmpty() && 0 == name.contains("-")) {
        String ns = namespace_name(plugin);
        if (!ns.isEmpty())  {
            String prefix = tplugin->structEditor()->grove()->document()->
                     documentElement()->getPrefixByXmlNs(ns);
            if (!prefix.isEmpty())
                result = prefix + ":" + result;
        }
    }
    return result;
}

static int getLevelByTable(const TablePlugin* plugin, const String& name)
{
    int result = -1;
    TablePlugin* tplugin = const_cast<TablePlugin*>(plugin);
    TablePlugin::TableInfoSet& dset = tplugin->infoSet();
    TablePlugin::TableInfoSet::iterator info =  dset.find(name);
    if (info != dset.end())
        result = info->second->lev[tplugin->currentTable()];
    return result;
}

String row_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("row"));
}

String cell_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("entry"));
}

String cell_header_name(const TablePlugin* plugin)
{
    String cells = getNameByTable(plugin, NOTR("entry"), false);
    QStringList lst = QStringList::split(',', cells);
    return lst[lst.size()-1].stripWhiteSpace();
}

String colspec_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("colspec"));
}

String spanspec_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("spanspec"));
}

String title_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("title"));
}

String thead_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("thead"));
}

String tbody_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("tbody"));
}

String tfoot_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("tfoot"));
}

String table_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("table"));
}

String namespace_name(const TablePlugin* plugin)
{
    String result;
    TablePlugin* tplugin = const_cast<TablePlugin*>(plugin);
    TablePlugin::TableInfoSet& dset = tplugin->infoSet();
    TablePlugin::TableInfoSet::iterator info =  dset.find(NOTR("namespace"));
    if (info != dset.end())
        result = info->second->name[tplugin->currentTable()];
    return result;
}

String root_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("tgroup"));
}

String frame_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("table-frame"));
}

String frame_values(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("table-frame-values"), false);
}

String border_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("table-border"));
}

String border_width(const TablePlugin* plugin)
{
    return String::number(getLevelByTable(plugin, NOTR("table-border")));
}

String vspan_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("entry-rowspan"));
}

String hspan_name(const TablePlugin* plugin)
{
    return getNameByTable(plugin, NOTR("entry-colspan"));
}

bool is_genid(const TablePlugin* plugin)
{
    return 0 < getLevelByTable(plugin, NOTR("generate-id"));
}

bool is_row(const TablePlugin* plugin, const String& name)
{
    return name == getNameByTable(plugin, NOTR("row"));
}

bool is_cell(const TablePlugin* plugin, const Common::String& name)
{
    String cells = getNameByTable(plugin, NOTR("entry"), false);
    QStringList lst = QStringList::split(',', cells);
    bool result = false;
    for (int i = 0; i < (int)lst.size(); i++)
        result = (name == lst[i].stripWhiteSpace()) || result;
    return result;
}

bool is_thead(const TablePlugin* plugin, const String& name)
{
    String str = getNameByTable(plugin, NOTR("thead"));
    if (str.isEmpty())
        return false;
    return name == str;
}

bool is_tbody(const TablePlugin* plugin, const String& name)
{
    String str = getNameByTable(plugin, NOTR("tbody"));
    if (str.isEmpty())
        return false;
    return name == str;
}

bool is_tfoot(const TablePlugin* plugin, const String& name)
{
    String str = getNameByTable(plugin, NOTR("tfoot"));
    if (str.isEmpty())
        return false;
    return name == str;
}

bool vspan_counts_from_zero(const TablePlugin*  plugin)
{
    return 1 != getLevelByTable(plugin, NOTR("entry-rowspan"));
}

bool hspan_counts_from_zero(const TablePlugin*  plugin)
{
    return 1 != getLevelByTable(plugin, NOTR("entry-colspan"));
}

bool hspan_has_cals_format(const TablePlugin*  plugin)
{
    String cells = getNameByTable(plugin, NOTR("entry-colspan"), false);
    return 0 <= cells.find(',');
}

bool table_has_cols(const TablePlugin*  plugin)
{
    return !getNameByTable(plugin, NOTR("tgroup-cols")).isEmpty();
}

bool table_has_tgroup(const TablePlugin*  plugin)
{
    return root_name(plugin) != getNameByTable(plugin, NOTR("table"));
}

bool colspec_has_name(const TablePlugin*  plugin)
{
    return !getNameByTable(plugin, NOTR("colspec-name")).isEmpty();
}

int columns(TablePlugin* plugin)
{
    GroveEditor::GrovePos src_pos;
    if (!plugin->structEditor()->getCheckedPos(src_pos))
        return -1;
    Node* node = src_pos.node();
    int cols = -1;
    if (table_has_cols(plugin)) {
        while (node && root_name(plugin) != node->nodeName())
            node = node->parent();
        if (node) {
            Attr* attr_cols = static_cast<Element*>(node)->
                          attrs().getAttribute(COLS);
            if (attr_cols)
                cols = attr_cols->value().toInt();
        }
        if (0 < cols)
            return cols;
    }
  return calculated_cols(plugin);
}

int calculated_cols(TablePlugin* plugin)
{
    GroveEditor::GrovePos src_pos;
    if (!plugin->structEditor()->getCheckedPos(src_pos))
        return -1;
    Node* node = src_pos.node();
    int cols = -1;    
    node = src_pos.node();
    while (node &&
          !is_thead(plugin, node->nodeName()) &&
          !is_tbody(plugin, node->nodeName()) &&
          !is_tfoot(plugin, node->nodeName()) &&
          table_name(plugin) != node->nodeName())
        node = node->parent();
    if (node && node->firstChild() &&
        is_row(plugin, node->firstChild()->nodeName())) {
        Node* cell = node->firstChild()->firstChild();
        if (cell)
            cols++;
        for (; cell; cell = cell->nextSibling()) {
            cols++;
            int start = cols - 1;
            int end = 0;
            getHSpanRange(plugin, cell, start, end);
            if (end > start)
                cols += end - start;
        }
    }
    //TODO: recalculate if first row is conrefed. 
    return cols;
}

int rowCells(TablePlugin* plugin, Node* nodeCell,
             NodeList* rowSet, NodeList* vspanSet,
             bool takeTop, int size)
{
    int result = -1;
    int cols = size;
    int calc_cols = columns(plugin);
    if (0 > cols || cols < calc_cols)
        cols = calc_cols;
    for (int i = 0;  rowSet && i <= cols; i++)
        rowSet->push_back(0);
    Vector<bool>  row_plan;
    for (int i = 0;  i < cols; i++)
        row_plan.push_back(false);
    Node* cur_row = nodeCell;
    while (cur_row && !is_row(plugin, cur_row->nodeName()))
        cur_row = cur_row->parent();
    if (!cur_row)
        return -1; //TODO:warning
    if (!takeTop) {
        int vspan = getVSpanRange(plugin, nodeCell);
        if (vspan) {
            for (int i = 0; cur_row && i < vspan; ++i)
                cur_row = cur_row->nextSibling();
        }
    }
    Node* node = cur_row->prevSibling();
    int level = 0;
    for (; node; node = node->prevSibling()) {
        if (!is_row(plugin, node->nodeName()))
            continue;
        level++;
        Node* cell = node->firstChild();
        while (cell) {
            if (is_cell(plugin, cell->nodeName())) {
                int vspan = getVSpanRange(plugin, cell);
                if (vspan && vspan >= level) {
                    int cur = rowCells(plugin, cell, 0, 0, true, cols);
                    int start = cur;
                    int end = 0;
                    getHSpanRange(plugin, cell, start, end);
                    if (start >= 0 && end > start)
                        for (int i = start; i <= end; ++i)
                            row_plan[i] = true;
                    else
                        row_plan[cur] = true;
                    if (vspanSet)
                        vspanSet->push_back(cell);
                }
            }
            cell = cell->nextSibling();
        }
    }
    Node* cur_cell = nodeCell;
    while (cur_cell && !is_cell(plugin, cur_cell->nodeName()))
        cur_cell = cur_cell->parent();
    int cur = 0;
    node = cur_row->firstChild();
    for (; node; node = node->nextSibling()) {
        if (!is_cell(plugin, node->nodeName()))
            continue;
        while (cur < int(row_plan.size()) && row_plan[cur])
            cur++;
        int start = cur;
        int end = 0;
        getHSpanRange(plugin, node, start, end);
        if (start >= 0 && end > start) {
            cur += end - start;
            for (int i = start; i <= end; ++i) {
                RT_ASSERT(i < row_plan.size());
                row_plan[i] = true;
                if (rowSet && i < rowSet->size()) 
                    rowSet->operator[](i) = node;
            }
            if (node == cur_cell)
                result = start;
        }
        else {
            int sz = row_plan.size();
            if (cur > sz - 1)
                for (int i = 0; i < cur - sz + 1; ++i)
                    row_plan.push_back(false);
            RT_ASSERT(cur < row_plan.size());
            row_plan[cur] = true;
            if (rowSet && cur < rowSet->size()) 
                 rowSet->operator[](cur) = node;
            if (node == cur_cell)
                result = cur;
        }
        cur++;
    }
    return result;
}


int columnCells(TablePlugin* plugin, Node* resolver, int column,
                NodeList* columnSet, NodeList* hspanSet)
{
    Node* root = resolver;
    while (root &&
          !is_thead(plugin, root->nodeName()) &&
          !is_tbody(plugin, root->nodeName()) &&
          !is_tfoot(plugin, root->nodeName()) &&
          table_name(plugin) != root->nodeName())
        root = root->parent();
    if (!root)
        return 0;
    Node* row = root->firstChild();
    for (; row && row_name(plugin) != row->nodeName();
        row = row->nextSibling()) {
    }
    int result = -1;
    int cur_row = 0;
    Vector<int>  row_plan;
    for (int i = 0;  i <= column; i++)
        row_plan.push_back(0);
    int in_charge = 0;
    while (row) {
        int col = 0;
        for (int i = 0;  i < column; i++)
            if (0 < row_plan[i])
                row_plan[i] -= 1;

        Node* cell = row->firstChild();
        for (; cell; cell = cell->nextSibling()) {
            if (cell->nodeType() != Node::ELEMENT_NODE)
                continue;
            if (!is_cell(plugin, cell->nodeName()))
                break;
            while (col < column && 0 < row_plan[col])
                col++;
            if (cell == resolver)
                result = cur_row;
            if (column == col)
                 break;
            int start = col;
            int end = 0;
            getHSpanRange(plugin, cell, start, end);
            if (end > start && 0 < end)
                if (column <= end) {
                    if (hspanSet)
                        hspanSet->push_back(cell);
                    break;
                }
                else
                    col = end;
            else if (column == col)
                break;
            int vspan = getVSpanRange(plugin, cell);
            if (0 < vspan) {
                Attr* mrows = ELEMENT_CAST(cell)->
                              attrs().getAttribute(vspan_name(plugin));
                if (end > start && 0 < end)
                    for (int i = 0; i <= end - start; ++i)
                        row_plan[i] = mrows->value().toInt() + 1;
                else {
                    if (0 == row_plan.size())
                        for (int i = 0; i <= col; ++i)
                            row_plan.push_back(0);
                    row_plan[col] = mrows->value().toInt() + 1;
                }
            }
            col++;
        }
        if (in_charge) {
            in_charge--;
            columnSet->push_back(0);
        }
        else {
            if (!cell) {
                QString err(tr("Inconsistent table structure.\n"
                               "Verify table cells and span attributes."));
                plugin->structEditor()->sernaDoc()->
                    showMessageBox(SernaDoc::MB_CRITICAL,
                    tr("Error"), err, tr("&OK"));
                return -1;//result; inconsistent table
            }
            if (columnSet)
                columnSet->push_back(cell);
            int vspan = getVSpanRange(plugin, cell);
            if (0 < vspan)
                in_charge = vspan;
        }

        row = row->nextSibling();
        cur_row++;
    }
    return result;
}

int  columnPosForSpec(TablePlugin* plugin, const String& name, Node* root)
{
    int count = 0;
    if (!root)
        return -1;
    Node* node = root->firstChild();
    for (; node; node = node->nextSibling()) {
        if (colspec_name(plugin) != node->nodeName())
            continue;
        count++;
        Attr* attr = static_cast<Element*>(node)->
                     attrs().getAttribute(COLNAME);
        if (attr && attr->value() == name) {
            Attr* num = ELEMENT_CAST(node)->attrs().getAttribute(COLNUM);
            if (num)
                count = num->value().toInt();
            --count;
            return (count < 0) ? 0 : count;
        }
    }
    return -1;
}

Node* specByNum(TablePlugin* plugin, int count, Node* root,
                bool* tableUseColnum, bool* currentColnumExist)
{
    if (!root)
        return 0;
    Node* node = root->firstChild();
    Node* spec = 0;
    int cur_col = count;
    for (; node; node = node->nextSibling()) {
        if (colspec_name(plugin) != node->nodeName())
                    continue;
        Attr* attr = ELEMENT_CAST(node)->attrs().getAttribute(COLNUM);
        if (attr) {
            if (tableUseColnum)
                *tableUseColnum = true;
            if (cur_col == attr->value().toInt() - 1) {
                if (currentColnumExist)
                    *currentColnumExist = true;
                spec = node;
                break;
            }
        }
        if (0 == count)
            spec = node;
        count--;
    }
    if (spec && colspec_name(plugin) != spec->nodeName())
        spec = spec->prevSibling();
    return spec;
}

void getHSpanRange(TablePlugin* plugin, Node* cell, int& start, int& end)
{
    if (hspan_has_cals_format(plugin)) {
        Node* root = cell;
        while (root && root_name(plugin) != root->nodeName())
            root = root->parent();
        if (0 == root || cell->nodeType() != Node::ELEMENT_NODE) {
            start = end = 0;
            return;
        }
        Attr* namest   = ELEMENT_CAST(cell)->attrs().getAttribute(NAMEST);
        Attr* nameend  = ELEMENT_CAST(cell)->attrs().getAttribute(NAMEEND);
        Attr* spanname = ELEMENT_CAST(cell)->attrs().getAttribute(SPANNAME);
        if (spanname) {
            Node* node = root->firstChild();
            for (; node; node = node->nextSibling()) {
                if (node->nodeType() != Node::ELEMENT_NODE)
                    continue;
                if (spanspec_name(plugin) != node->nodeName())
                                    continue;
                Attr* spname =
                    ELEMENT_CAST(node)->attrs().getAttribute(SPANNAME);
                if (spname && spanname->value() == spname->value()) {
                    namest   = ELEMENT_CAST(node)->attrs().getAttribute(NAMEST);
                    nameend  = ELEMENT_CAST(node)->
                        attrs().getAttribute(NAMEEND);
                    break;
                }
            }
        }
        if (namest)
            start = columnPosForSpec(plugin, namest->value(), root);
        if (nameend)
            end = columnPosForSpec(plugin, nameend->value(), root);
    }
    else {
        Attr* attr = ELEMENT_CAST(cell)->attrs().
                     getAttribute(hspan_name(plugin));
        if (attr)
            end = start + attr->value().toInt() -
                  (hspan_counts_from_zero(plugin) ? 0 : 1);
    }
}


int getVSpanRange(TablePlugin* plugin, Node* cell)
{
    if (cell->nodeType() != Node::ELEMENT_NODE)
        return 0;
    Attr* mrows = ELEMENT_CAST(cell)->attrs().getAttribute(vspan_name(plugin));
    if (mrows) {
        bool ok;
        int result = mrows->value().toInt(&ok);
        if (ok)
            return vspan_counts_from_zero(plugin) ? result : result - 1;
    }
    return 0;
}

String copyAttributes(Node* from, Element* cell,
                      const String& name, int increment)
{
    String result;
    if (!from || from->nodeType() != Node::ELEMENT_NODE)
        return result;
    Element* elem = ELEMENT_CAST(from);
    Attr* attr = elem->attrs().getAttribute(name);
    if (attr) {
        result = attr->value();
        if (0 != increment)
            result = String::number(result.toInt() + increment);
        Attr* copy = new Attr(name);
        copy->setValue(result);
        cell->attrs().appendChild(copy);
    }
    return result;
}

void setExtraAttrs(TablePlugin* plugin, Node* cell,
                   const char* name)
{
    if (cell->nodeType() != Node::ELEMENT_NODE)
        return;
    Element* elem = ELEMENT_CAST(cell);
    TablePlugin::AttrsSet& aset = plugin->attrsSet();
    TablePlugin::AttrsSet::iterator attrs = aset.find(name);
    if (attrs != aset.end()) {
        const TablePlugin::AttrsContainer& container = attrs->second;
        for (int i = 0; i < container.size(); i++) {
            const TablePlugin::AttrInfo& attr_info = container[i];
            Attr* attr = new Attr(attr_info.name);
            attr->setValue(attr_info.value);
            elem->attrs().appendChild(attr);
        }
    }
}

void copyHSpan(TablePlugin* plugin, Node* from, Element* cell)
{
    if (hspan_has_cals_format(plugin)) {
        copyAttributes(from, cell, SPANNAME);
        copyAttributes(from, cell, NAMEST);
        copyAttributes(from, cell, NAMEEND);
    }
    else
        copyAttributes(from, cell, hspan_name(plugin));
}

void delHSpan(TablePlugin* plugin, TableBatchCommand* batch, Node* cell)
{
    if (!batch || !cell || cell->nodeType() != Node::ELEMENT_NODE)
        return;
    Element* elem = ELEMENT_CAST(cell);
    GroveEditor::Editor* editor = plugin->structEditor()->groveEditor();
    if (hspan_has_cals_format(plugin)) {
        Attr* namest = elem->attrs().getAttribute(NAMEST);
        Attr* nameend = elem->attrs().getAttribute(NAMEEND);
        Attr* spanname = elem->attrs().getAttribute(SPANNAME);
        if (spanname)
            batch->executeAndAdd(editor->removeAttribute(spanname).pointer());
        if (namest)
            batch->executeAndAdd(editor->removeAttribute(namest).pointer());
        if (nameend)
            batch->executeAndAdd(editor->removeAttribute(nameend).pointer());
    }
    else {
        Attr* attr = elem->attrs().getAttribute(hspan_name(plugin));
        if (attr)
            batch->executeAndAdd(editor->removeAttribute(attr).pointer());

    }
}

void delHSpan(TablePlugin* plugin, Element* elem)
{
    if (!elem)
        return;
    if (hspan_has_cals_format(plugin)) {
        Attr* namest = elem->attrs().getAttribute(NAMEST);
        Attr* nameend = elem->attrs().getAttribute(NAMEEND);
        Attr* spanname = elem->attrs().getAttribute(SPANNAME);
        if (spanname)
            spanname->remove();
        if (namest)
            namest->remove();
        if (nameend)
            nameend->remove();
    }
    else {
        Attr* attr = elem->attrs().getAttribute(hspan_name(plugin));
        if (attr)
            attr->remove();
    }
}

void resizeHSpan(TablePlugin* plugin, TableBatchCommand* batch,
                 Node* cell, bool prev, int val, int curCol)
{
    GroveEditor::Editor* editor = plugin->structEditor()->groveEditor();
    if (!batch || !cell || cell->nodeType() != Node::ELEMENT_NODE)
        return;
    Element* elem = ELEMENT_CAST(cell);
    if (!hspan_has_cals_format(plugin)) {
        Attr* attr = elem->attrs().getAttribute(hspan_name(plugin));
        int result = val;
        if (attr)
            batch->executeAndAdd(editor->
                setAttribute(attr,
                String::number(result + attr->value().toInt())).pointer());
        else
            batch->executeAndAdd(editor->addAttribute(elem,
                new PropertyNode(hspan_name(plugin),
                String::number(result + 1))).pointer());
        return;
    }
    Node* root = cell;
    while(root && root_name(plugin) != root->nodeName())
        root = root->parent();
    if (!root || root->nodeType() != Node::ELEMENT_NODE)
        return;
    Attr* spanname = elem->attrs().getAttribute(SPANNAME);
    if (spanname) {
        Node* node = root->firstChild();
        Attr* namest = 0;
        Attr* nameend = 0;
        for (; node; node = node->nextSibling()) {
            if (spanspec_name(plugin) != node->nodeName())
                            continue;
            Attr* spname = ELEMENT_CAST(node)->attrs().getAttribute(SPANNAME);
            if (spname && spanname->value() == spname->value()) {
                namest   = ELEMENT_CAST(node)->attrs().getAttribute(NAMEST);
                nameend  = ELEMENT_CAST(node)->attrs().getAttribute(NAMEEND);
                break;
            }
        }
        if (namest && nameend) {
            batch->executeAndAdd(editor->removeAttribute(spanname).pointer());
            batch->executeAndAdd(editor->addAttribute(elem,
                new PropertyNode(NAMEST, namest->value())).pointer());
            batch->executeAndAdd(editor->addAttribute(elem,
                new PropertyNode(NAMEEND, nameend->value())).pointer());
        }
    }
    String name = prev ? NAMEST : NAMEEND;
    Attr* name_attr = elem->attrs().getAttribute(name);

    int count = curCol;
    int start = curCol;
    int end = curCol;
    getHSpanRange(plugin, cell, start, end);

    Element* tgroup = static_cast<Element*>(root);

    count = (prev ? start - val : end + val) ;
    if (count < 0)
        count = 0;
    Node* spec = specByNum(plugin, count, tgroup);
    if (0 == spec) {
        QString err(tr("Correspondent 'colspec' declaration is not found.\n"
                       "Undo the operation if the result is unexpected."));
        plugin->structEditor()->sernaDoc()->
            showMessageBox(SernaDoc::MB_CRITICAL,
            tr("Error"), err, tr("&OK"));
        return;
    }
    Attr* colname = ELEMENT_CAST(spec)->attrs().getAttribute(COLNAME);
    if (0 == colname)
        return;
    if (name_attr)
        batch->executeAndAdd(editor->
            setAttribute(name_attr,colname->value()).pointer());
    else {
        String second_name = prev ? NAMEEND : NAMEST;
        Attr* second_attr = elem->attrs().getAttribute(second_name);
        if (!second_attr) {
            count = QMAX((prev ? end :  start), 0);
            Node* sspec = specByNum(plugin, count, tgroup);
            Attr* sname = ELEMENT_CAST(sspec)->attrs().getAttribute(COLNAME);
            if (0 == sname)
                return;
            batch->executeAndAdd(editor->addAttribute(elem,
                new PropertyNode(second_name, sname->value())).pointer());
        }
        batch->executeAndAdd(editor->addAttribute(elem,
            new PropertyNode(name, colname->value())).pointer());
    }
}

void resizeRowSpans(TablePlugin* plugin, TableBatchCommand* batch,
                 GroveLib::Node* cell, int value, int cols)
{
    GroveEditor::Editor* editor = plugin->structEditor()->groveEditor();
    if (0 > cols)
        cols = columns(plugin);
    Node* node = 0;
    NodeList row_set;
    rowCells(plugin, cell, &row_set, &row_set, true, cols);
    for (int i = 0; i < (int)row_set.size(); ++i) {
        if (node == row_set[i])
            continue;
        node = row_set[i];
        if (!node || cell == node)
            continue;
        int vspan = getVSpanRange(plugin, node);
        Attr* mrows = ELEMENT_CAST(node)->
                      attrs().getAttribute(vspan_name(plugin));
        int delta = vspan + value;
        if (0 >= delta && mrows)
            batch->executeAndAdd(editor->
                                 removeAttribute(mrows).pointer());
        else if (mrows)
            batch->executeAndAdd(editor->setAttribute(mrows,
                String::number(mrows->value().toInt() + value)).pointer());
        else
            batch->executeAndAdd(editor->addAttribute
                (ELEMENT_CAST(node), new PropertyNode(vspan_name(plugin),
                String::number((vspan_counts_from_zero(plugin) ? 0 : 1) +
                value))).pointer());
    }
}

bool check_support_level(TablePlugin*, Node*)
{
/*    while(node && root_name(plugin) != node->nodeName())
        node = node->parent();
    if (!node)
        return false;
    for(node = node->firstChild(); node; node = node->nextSibling()) {
        if (SPANSPEC == node->nodeName()) {
            QString err(tr("Plugin doesn't support 'spanspec' declarations."));
            plugin->structEditor()->sernaDoc()->
                showMessageBox(SernaDoc::MB_CRITICAL,
                tr("Error"), err, tr("&OK"));
            return false;
        }
    }
*/
    return true;
}

void rebuildCalsHSpan(TablePlugin* plugin, TableBatchCommand* batch,
              GroveLib::Node* resolver, const Common::String& name,
              const Common::String& oldName)
{
    GroveEditor::Editor* editor = plugin->structEditor()->groveEditor();
    Node* root = resolver;
    while (root && root_name(plugin) != root->nodeName())
        root = root->parent();
    if (0 == root || root->nodeType() != Node::ELEMENT_NODE)
        return;
    Node* node = root->firstChild();
    for (; node; node = node->nextSibling()) {
        Node* row = node;
        if (is_thead(plugin, node->nodeName()) ||
            is_tbody(plugin, node->nodeName()) ||
            is_tfoot(plugin, node->nodeName()))
            row = node->firstChild();
        for (; row; row = row->nextSibling()) {
            Node* cell = 0;
            if (spanspec_name(plugin) != row->nodeName())
                cell = row;
            else if (row_name(plugin) != row->nodeName())
                cell = row->firstChild();

            for (; cell; cell = cell->nextSibling()) {
                if (!is_cell(plugin, cell->nodeName()) &&
                    spanspec_name(plugin) != cell->nodeName())
                    continue;
                Attr* namest = ELEMENT_CAST(cell)->attrs().getAttribute(NAMEST);
                Attr* nameend = ELEMENT_CAST(cell)->attrs().getAttribute(NAMEEND);
                Attr* colname = ELEMENT_CAST(cell)->attrs().getAttribute(COLNAME);
                if (namest && oldName == namest->value())
                    batch->executeAndAdd(editor->
                                         setAttribute(namest, name).pointer());
                if (nameend && oldName == nameend->value())
                    batch->executeAndAdd(editor->
                                         setAttribute(nameend, name).pointer());
                if (colname && oldName == colname->value())
                    batch->executeAndAdd(editor->
                                         setAttribute(colname, name).pointer());
            }

        }
    }
}

bool getSelectionCells(TablePlugin* plugin, Node*& start_cell, Node*& end_cell)
{
    if (!plugin->structEditor()->editableView().getSelection().src_.isEmpty()){
        GrovePos from, to;
        if (StructEditor::POS_OK != plugin->structEditor()->getSelection(from,
            to, StructEditor::ANY_OP | StructEditor::SILENT_OP))
                return false;
        start_cell = (GrovePos::ELEMENT_POS == from.type()) ?
                      (from.before() ? from.before() : from.node()) :
                      from.node();
        String sname = start_cell->nodeName();
        if (table_name(plugin) == sname || root_name(plugin)  == sname ||
            row_name(plugin)   == sname || thead_name(plugin) == sname ||
            tbody_name(plugin) == sname || tfoot_name(plugin) == sname ) {
            while (start_cell && !is_cell(plugin, start_cell->nodeName()))
                start_cell = start_cell->firstChild();
        }
        while (start_cell && !is_cell(plugin, start_cell->nodeName()))
            start_cell = start_cell->parent();

        end_cell = (GrovePos::ELEMENT_POS == to.type()) ?
                   (to.before() && to.before()->prevSibling() ?
                    to.before()->prevSibling(): to.node()) : to.node();

        String ename = end_cell->nodeName();
        if (table_name(plugin) == ename || root_name(plugin) == ename ||
            row_name(plugin) == ename || thead_name(plugin) == ename ||
            tbody_name(plugin) == ename || tfoot_name(plugin) == ename ) {
            while (end_cell && !is_cell(plugin, end_cell->nodeName()))
                end_cell = end_cell->lastChild();
        }

        while (end_cell && !is_cell(plugin, end_cell->nodeName()))
            end_cell = end_cell->parent();

        if (start_cell && end_cell &&
            Common::LESS == end_cell->comparePos(start_cell)) {
            Node* swap = start_cell;
            start_cell = end_cell;
            end_cell = swap;
        }
        return true;
    }
    return false;
}


GroveLib::Node* is_in(const String& name, Node* node)
{
    while (node && name != node->nodeName())
        node = parentNode(node);
    return node;
}

} //namespace

