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
#include "TablePlugin.h"
SIMPLE_EXECUTOR_IMPL(InsertRowAbove, TablePlugin, IS_IN_ROW);
SIMPLE_EXECUTOR_IMPL(InsertRowBelow, TablePlugin, IS_IN_ROW);
SIMPLE_EXECUTOR_IMPL(DeleteRow, TablePlugin, IS_IN_ROW);
SIMPLE_EXECUTOR_IMPL(InsertCellBefore, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(InsertCellAfter, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(DeleteCell, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(DeleteCellContent, TablePlugin, IS_IN_CELL);

#include "TableUtils.h"
#include "plugins/tableplugin/TableBatchCommand.h"
#include "common/PropertyTreeEventData.h"
#include "grove/Nodes.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveCommand.h"
#include "structeditor/StructEditor.h"
#include "structeditor/GroveCommandEventData.h"

#include "formatter/impl/Areas.h"

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;
using namespace TableUtils;

class ExecuteAndUpdate;

static void insert_cell(TablePlugin* plugin, bool prev)
{
    GroveEditor::GrovePos src_pos;
    if (!plugin->structEditor()->getCheckedPos(src_pos))
        return;
    Node* node = src_pos.node();
    while(node && !is_cell(plugin, node->nodeName()) &&
          !is_thead(plugin, node->nodeName()) &&
          !is_tbody(plugin, node->nodeName()) &&
          !is_tfoot(plugin, node->nodeName()) &&
           row_name(plugin) != node->nodeName())
        node = node->parent();
    if (!node)
        return;
    Node* parent = 0;
    Node* before = 0;
    if (is_cell(plugin, node->nodeName())) {
        if (prev)
            before = node;
        else
            before = node->nextSibling();
        parent = node->parent();
    }
    else
        parent = node;

    String cname = cell_name(plugin);
    if (node->nodeName() == cell_header_name(plugin)) 
        cname = cell_header_name(plugin);
    Element* entry = new Element(cname);
    if (plugin->isAutoEntryPara())
        entry->appendChild(new Element(PARA));
    TableBatchCommand* batch = new TableBatchCommand(plugin);
    batch->setInfo(prev ? QT_TRANSLATE_NOOP("GroveEditor",
        "Insert Table Cell Before") : QT_TRANSLATE_NOOP("GroveEditor",
        "Insert Table Cell After"));
    GroveCommandEventData gcmd(batch);
    DocumentFragmentPtr df = new DocumentFragment;
    df->appendChild(entry);
    GroveEditor::GrovePos pos(parent, before);
    CommandPtr cmd = plugin->structEditor()->
                     groveEditor()->paste(df.pointer(), pos);
    batch->executeAndAdd(cmd.pointer());
    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin->structEditor());
}

static void insert_row(TablePlugin* plugin, bool prev)
{
    GrovePos pos;
    if (!plugin->structEditor()->getCheckedPos(pos))
        return;
    int cols = columns(plugin);
    if (0 > cols)
        return;
    Node* cell = pos.node();
    Node* row = cell;
    if (!cell)
        return;
    Node* before = (GrovePos::ELEMENT_POS == pos.type()) ? pos.before() : 0;
    String sname = cell->nodeName();
    if (before && row_name(plugin) == before->nodeName())
        cell = prev ? before->firstChild()
                    : (before->prevSibling()
                        ? before->prevSibling()->firstChild()
                        : before->firstChild());
    else if (!before && (thead_name(plugin) == sname ||
        tbody_name(plugin) == sname || tfoot_name(plugin) == sname)) {
        while (cell && !is_cell(plugin, cell->nodeName()))
            cell = cell->lastChild();
    }
    else if (row_name(plugin) == sname)
        while (cell && !is_cell(plugin, cell->nodeName()))
            cell = cell->firstChild();

    while (cell && !is_cell(plugin, cell->nodeName()))
        cell = cell->parent();

    NodeList row_set, span_set;
    String cellname = cell_name(plugin);
    if (cell) {
        prev ? rowCells(plugin, cell, &row_set, &span_set, true, cols) :
               rowCells(plugin, cell, &row_set, &span_set, false, cols);
        row = cell->parent();
        if (cell->nodeName() == cell_header_name(plugin)) 
        cellname = cell_header_name(plugin);
    }
    TableBatchCommand* batch = new TableBatchCommand(plugin);
    batch->setInfo((prev ? QT_TRANSLATE_NOOP("GroveEditor",
        "Insert Table Row Above") : QT_TRANSLATE_NOOP("GroveEditor",
        "Insert Table Row Below")));
    GroveCommandEventData gcmd(batch);
    Element* row_elem = new Element(row_name(plugin));
    int i = 0;
    batch->setSuggestedPos(pos);
    for (; i < cols; ++i) {
        if (0 == row_set.size() || row_set[i] == 0)
            continue;
        if (!prev) {
            int vspan = getVSpanRange(plugin, row_set[i]);
            if (vspan) {
                if (0 == i || (0 < i && row_set[i] != row_set[i-1]))
                    span_set.push_back(row_set[i]);
                int start = i;
                int end = 0;
                getHSpanRange(plugin, row_set[i], start, end);
                if (end > start)
                    i += end - start;
                continue;
            }
        }

        Element* entry = new Element(cellname);
        if (plugin->isAutoEntryPara())
            entry->appendChild(new Element(PARA));
        int start = i;
        int end = 0;
        getHSpanRange(plugin, row_set[i], start, end);
        if (end > start) {
            copyHSpan(plugin, row_set[i], entry);
            i += end - start;
        }
        setExtraAttrs(plugin, entry);
        row_elem->appendChild(entry);
    }

    DocumentFragmentPtr df = new DocumentFragment;
    df->appendChild(row_elem);
    CommandPtr cmd = plugin->structEditor()->
                     groveEditor()->paste(df.pointer(),
                     GrovePos(row->parent(),
                     prev ? row : row->nextSibling()));
    batch->executeAndAdd(cmd.pointer());

//    insert_element(plugin, batch, prev, cell, row_elem);
    for (i = 0; i < (int)span_set.size(); ++i) {
        Node* cell = span_set[i];
        if (!cell)
            continue;
        int vspan = getVSpanRange(plugin, span_set[i]);
        if (vspan) {
            Attr* mrows = ELEMENT_CAST(cell)->
                          attrs().getAttribute(vspan_name(plugin));
            if (vspan + 1)
                batch->executeAndAdd(plugin->structEditor()->
                groveEditor()->setAttribute(mrows,
                String::number(mrows->value().toInt() + 1)).pointer());
            else
                batch->executeAndAdd(plugin->structEditor()->
                groveEditor()->removeAttribute(mrows).pointer());

        }
    }
    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin->structEditor());
}

static void delete_selected_rows(TablePlugin* plugin,TableBatchCommand* batch,
                                 Node* start_row,
                                 Node* end_row, int cols)
{
    NodeList row_set, span_set;
    int cnt = 1;
    Node* next_row = start_row;
    while (end_row && end_row != next_row) {
        next_row = next_row->nextSibling();
        cnt++;
    }
    Node* row = start_row;
    Node* row_parent = row->parent();
    for (int k = 0; row && k < cnt; k++) {
        if (row && row->firstChild())
            rowCells(plugin, row->firstChild(), &row_set,
                     &span_set, true, cols);

        NodeList next_row_set;
        if (row && row->nextSibling() &&
            row->nextSibling()->firstChild())
            rowCells(plugin, row->nextSibling()->firstChild(),
                     &next_row_set, 0, true, cols);
        int i = 0;
        for (; i < cols; ++i) {
            if (row_set[i]) {
                int vspan = getVSpanRange(plugin, row_set[i]);
                if (vspan && row->nextSibling()) {
                    Element* entry = new Element(cell_name(plugin));
                    if (plugin->isAutoEntryPara())
                        entry->appendChild(new Element(PARA));
                    int start = i;
                    int end = 0;
                    getHSpanRange(plugin, row_set[i], start, end);
                    if (end > start) {
                        copyHSpan(plugin, row_set[i], entry);
                        i += end - start;
                    }
                    copyAttributes(row_set[i], entry, vspan_name(plugin), -1);
                    Node* before = 0;
                    if (0 != next_row_set.size()) {
                        int j = i;
                        while (0 == before && j < cols) {
                            before = next_row_set[j];
                            j++;
                        }
                    }
                    DocumentFragmentPtr df = new DocumentFragment;
                    df->appendChild(entry);
                    Node* next_row = row->nextSibling();

                    GroveEditor::GrovePos pos(next_row, before);
                    batch->executeAndAdd(plugin->structEditor()->
                           groveEditor()->paste(df.pointer(), pos).pointer());
                }
            }
        }
        Node* row_to_remove = row;
        row = row->nextSibling();
        batch->executeAndAdd(plugin->structEditor()->
            groveEditor()->removeNode(row_to_remove).pointer());
        for (i = 0; i < (int)span_set.size(); ++i) {
            Node* cell = span_set[i];
            if (!cell)
                continue;
            int vspan = getVSpanRange(plugin, span_set[i]);
            if (vspan) {
                Attr* mrows = ELEMENT_CAST(cell)->
                              attrs().getAttribute(vspan_name(plugin));
                if (vspan - 1)
                    batch->executeAndAdd(plugin->structEditor()->
                    groveEditor()->setAttribute(mrows,
                    String::number(mrows->value().toInt() - 1)).pointer());
                else
                    batch->executeAndAdd(plugin->structEditor()->
                    groveEditor()->removeAttribute(mrows).pointer());
            }
        }
        row_set.clear();
        span_set.clear();
    }
    if (row_parent && !row_parent->firstChild())
        batch->executeAndAdd(plugin->structEditor()->
            groveEditor()->removeNode(row_parent).pointer());

}


static void delete_row(TablePlugin* plugin)
{
    Node* cell = 0;
    Node* end_cell = 0;
    if (!getSelectionCells(plugin, cell, end_cell)) {
        GroveEditor::GrovePos pos;
        if (!plugin->structEditor()->getCheckedPos(pos))
            return;
        cell = pos.node();
    }

    Node* row = cell;
    if (!cell)
        return;
    int cols = columns(plugin);
    if (0 > cols)
        return;
    check_support_level(plugin, cell);
    NodeList row_set, span_set;
    if (row_name(plugin) != cell->nodeName()) {
        while(cell && !is_cell(plugin, cell->nodeName()))
            cell = cell->parent();
    }
    else
        cell = cell->firstChild();

    TableBatchCommand* batch = new TableBatchCommand(plugin);
    batch->setInfo(QT_TRANSLATE_NOOP("GroveEditor", "Delete Table Row"));
    GroveCommandEventData gcmd(batch);

    if (cell)
        row = cell->parent();
    if (row->prevSibling())
        batch->setSuggestedPos(GrovePos(row->prevSibling()));
    else
        batch->setSuggestedPos(GrovePos(row->parent()));
    if (0 != end_cell) {
        if (cell->parent()->parent() != end_cell->parent()->parent()) {
            Node* scell_parent = cell->parent()->parent();
            Node* ecell_parent = end_cell->parent()->parent();
            if (thead_name(plugin) == scell_parent->nodeName()) {
                delete_selected_rows(plugin, batch, cell->parent(),
                                     scell_parent->lastChild(), cols);
                if (tfoot_name(plugin) == ecell_parent->nodeName()) {
                    delete_selected_rows(plugin, batch,
                                 scell_parent->nextSibling()->firstChild(),
                                 scell_parent->nextSibling()->lastChild(),
                                 cols);
                }
                delete_selected_rows(plugin, batch, ecell_parent->firstChild(),
                                     end_cell->parent(), cols);
            }
            else if (tbody_name(plugin) == scell_parent->nodeName()) {
                delete_selected_rows(plugin, batch, cell->parent(),
                                     scell_parent->lastChild(), cols);
                delete_selected_rows(plugin, batch, ecell_parent->firstChild(),
                                     end_cell->parent(), cols);
            }
        }
        else
            delete_selected_rows(plugin, batch, row, end_cell->parent(), cols);
    }
    else
        delete_selected_rows(plugin, batch, row, row, cols);

    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin->structEditor());
}

void InsertRowAbove::execute()
{
    insert_row(plugin(), true);
}

void InsertRowAbove::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
}

void InsertRowBelow::execute()
{
    insert_row(plugin(), false);
}

void InsertRowBelow::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
}

void InsertCellBefore::execute()
{
    insert_cell(plugin(), true);
}

void InsertCellAfter::execute()
{
    insert_cell(plugin(), false);
}

void DeleteRow::execute()
{
    delete_row(plugin());
}

void DeleteCell::execute()
{
    GroveEditor::Editor* editor = plugin()->structEditor()->groveEditor();
    GroveEditor::GrovePos src_pos;
    if (!plugin()->structEditor()->getCheckedPos(src_pos))
        return;
    Node* node = src_pos.node();
    while(node && !is_cell(plugin(), node->nodeName()))
        node = node->parent();
//TODO warning
    if (!node)
        return;
    TableBatchCommand* batch = new TableBatchCommand(plugin());
    batch->setInfo(QT_TRANSLATE_NOOP("GroveEditor", "Delete Table Cell"));
    GroveCommandEventData gcmd(batch);

    Node* cell = 0;
    Node* end_cell = 0;
    if (getSelectionCells(plugin(), cell, end_cell)) {
        if (!cell)
            return;
        Node* row_start = cell->parent();
        Node* row_end = end_cell ? end_cell->parent() : row_start;
        if (end_cell)
            end_cell = end_cell->nextSibling();
        do {
            while (cell && cell != end_cell) {
                node = cell->nextSibling();
                batch->executeAndAdd(editor->removeNode(cell).pointer());
                cell = node;
            }
            if (row_start == row_end)
                break;
            if (!row_start->nextSibling() &&
                 row_start->parent() != row_end->parent() &&
                 row_start->parent()->nextSibling()->firstChild()) {
                row_start = row_start->parent()->nextSibling()->firstChild();
            }
            else
                row_start = row_start->nextSibling();
            cell = row_start ? row_start->firstChild() : 0;
        } while (row_start);

    }
    else
        batch->executeAndAdd(editor->removeNode(node).pointer());
    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin()->structEditor());
}

void DeleteCellContent::execute()
{
    GroveEditor::Editor* editor = plugin()->structEditor()->groveEditor();
    GroveEditor::GrovePos src_pos;
    if (!plugin()->structEditor()->getCheckedPos(src_pos))
        return;
    Node* node = src_pos.node();
    while(node && !is_cell(plugin(), node->nodeName()))
        node = node->parent();
    if (!node)
        return;
    TableBatchCommand* batch = new TableBatchCommand(plugin());
    batch->setInfo(QT_TRANSLATE_NOOP("GroveEditor", 
        "Delete Table Cell Content"));
    GroveCommandEventData gcmd(batch);


    Node* cell = 0;
    Node* end_cell = 0;
    if (getSelectionCells(plugin(), cell, end_cell)) {
        if (!cell)
            return;
        Node* row_start = cell->parent();
        Node* row_end = end_cell ? end_cell->parent() : row_start;
        if (end_cell)
            end_cell = end_cell->nextSibling();
        do {
            while (cell && cell != end_cell) {
                node = cell->nextSibling();
                Node* content_node = cell->firstChild();
                while(content_node) {
                    Node* next = content_node->nextSibling();
                    batch->executeAndAdd(editor->removeNode(content_node).pointer());
                    content_node = next;
                }
                cell = node;
            }
            if (row_start == row_end)
                break;
            if (!row_start->nextSibling() &&
                 row_start->parent() != row_end->parent() &&
                 row_start->parent()->nextSibling()->firstChild()) {
                row_start = row_start->parent()->nextSibling()->firstChild();
            }
            else
                row_start = row_start->nextSibling();
            cell = row_start ? row_start->firstChild() : 0;
        } while (row_start);

    }
    else {
        node = node->firstChild();
        while(node) {
            Node* next = node->nextSibling();
            batch->executeAndAdd(editor->removeNode(node).pointer());
            node = next;
        }
    }
    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin()->structEditor());
}

void DeleteCellContent::update(const GroveEditor::GrovePos& src)
{
    Node* node = src.node();
    while(node && !is_cell(plugin(), node->nodeName()))
        node = node->parent();
    if (!node) {
        setEnabled(false);
        return;
    }
    setEnabled(node->firstChild());
}

void DeleteRow::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
}

void DeleteCell::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
}

void InsertCellBefore::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
}

void InsertCellAfter::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
}

