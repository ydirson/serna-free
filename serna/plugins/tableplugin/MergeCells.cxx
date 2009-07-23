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
SIMPLE_EXECUTOR_IMPL(MergeSelectedCells, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(MergeCellAbove, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(MergeCellBelow, TablePlugin, IS_IN_CELL);

#include "TableUtils.h"
#include "plugins/tableplugin/TableBatchCommand.h"
#include "common/PropertyTreeEventData.h"
#include "grove/Nodes.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveCommand.h"
#include "structeditor/StructEditor.h"
#include "structeditor/GroveCommandEventData.h"
#include "editableview/EditableView.h"
#include "formatter/impl/Areas.h"
#include <qapplication.h>

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;
using namespace TableUtils;

class ExecuteAndUpdate;

static void merge_selected(TablePlugin* plugin)
{
    if (plugin->structEditor()->editableView().getSelection().src_.isEmpty())
        return;
    GroveEditor::Editor* editor = plugin->structEditor()->groveEditor();

    Node* start_cell = 0;
    Node* end_cell = 0;
    getSelectionCells(plugin, start_cell, end_cell);
    if (!start_cell || !end_cell)
        return;

    if (start_cell->parent()->parent() != end_cell->parent()->parent()) {
        QString err(qApp->translate("TablePlugin", 
                                    "Multiple selection is not supported."));
        plugin->structEditor()->sernaDoc()->
            showMessageBox(SernaDoc::MB_CRITICAL,
            tr("Error"), err, tr("&OK"));
        return;
    }
    TableBatchCommand* batch = new TableBatchCommand(plugin);
    batch->setInfo(QT_TRANSLATE_NOOP("GroveEditor", 
        "Merge Selected Table Cells"));
    batch->setSuggestedPos(GrovePos(start_cell));
    GroveCommandEventData gcmd(batch);

    int cur_col = rowCells(plugin, start_cell);
    if (hspan_has_cals_format(plugin)) {
        Node* tgroup = start_cell;
        while(tgroup && root_name(plugin) != tgroup->nodeName())
            tgroup = tgroup->parent();
        if (0 == specByNum(plugin, cur_col, tgroup)) {
            QString err(qApp->translate("TablePlugin", 
                        "Correspondent 'colspec' declaration is not found."));
            plugin->structEditor()->sernaDoc()->
                showMessageBox(SernaDoc::MB_CRITICAL,
                tr("Error"), err, tr("&OK"));
            return;
        }
    }
    int count = 0;
    int start = cur_col;
    int end = 0;
    getHSpanRange(plugin, start_cell, start, end);
    int cur_hsize = 0;
    if (end > start)
        cur_hsize = end - start;
    count += cur_hsize;
    int vspan = getVSpanRange(plugin, start_cell);
    Node* node = start_cell->nextSibling();
    QString err(qApp->translate("TablePlugin",
               "Selected cells have different vertical span values.\n"
               "Merge operation cancelled."));
    while (node && node != end_cell->nextSibling()) {
        if (vspan != getVSpanRange(plugin, node)) {
            plugin->structEditor()->sernaDoc()->showMessageBox
                (SernaDoc::MB_CRITICAL,tr("Error"), err, tr("&OK"));
            return;
        }
        node = node->nextSibling();
    }
    node = start_cell->nextSibling();
    while (node && node != end_cell->nextSibling()) {
        int start = count;
        int end = 0;
        getHSpanRange(plugin, node, start, end);
        if (end > start)
            count += end - start;
        count++;
        batch->executeAndAdd(editor->joinElements(start_cell).pointer());
        node = node->nextSibling();
    }

    resizeHSpan(plugin, batch, start_cell, false, count - cur_hsize, cur_col);

    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin->structEditor());
    plugin->update(batch->pos()); //position have not changed
}


void MergeSelectedCells::execute()
{
    merge_selected(plugin());
}

void MergeSelectedCells::update(const GroveEditor::GrovePos&)
{
    if (plugin()->structEditor()->editableView().getSelection().src_.isEmpty()
        || hspan_name(plugin()).isEmpty()) {
        setEnabled(false);
        return;
    }
    GroveEditor::GrovePos from, to;
    Node* start_cell = 0;
    Node* end_cell = 0;
    if (!getSelectionCells(plugin(), start_cell, end_cell)) {
        setEnabled(false);
        return;
    }
    if (!start_cell || !end_cell) {
        setEnabled(false);
        return;
    }

    NodeList rowset;
    int cur_col = rowCells(plugin(), start_cell, &rowset);
    Node* node = start_cell;
    while(node && node != end_cell->nextSibling()) {
        if (cur_col >= rowset.size()) {
            setEnabled(false);
            return;
        }
        Node* row_cell = rowset[cur_col];
        if (!row_cell || row_cell->parent() != start_cell->parent()) { 
            setEnabled(false);
            return;
        }
        while (rowset[cur_col + 1] == node) {
            RT_ASSERT((cur_col + 1) < rowset.size());
            cur_col += 1;
        }
        node = node->nextSibling();
        cur_col += 1;
    }

    setEnabled(start_cell != end_cell); 
}

static void merge_vspan(TablePlugin* plugin, bool above)
{
    GroveEditor::Editor* editor = plugin->structEditor()->groveEditor();

    GroveEditor::GrovePos src_pos;
    if (!plugin->structEditor()->getCheckedPos(src_pos))
        return;
    Node* node = src_pos.node();
    if (!check_support_level(plugin, node))
        return;

    while(node && cell_name(plugin) != node->nodeName())
        node = node->parent();
    if (!node)
        return;

    NodeList columnSet;
    int cols = columns(plugin);
    int column = rowCells(plugin, node, 0, 0, true, cols);
    int row_num = columnCells(plugin, node, column, &columnSet);
    if (0 > row_num)
        return;

    if (above && row_num == 0)
        return;
    else if (!above)  {
        row_num++;
        if (!node->parent()->nextSibling())
            return;
    }

    TableBatchCommand* batch = new TableBatchCommand(plugin);
    batch->setInfo(above ? QT_TRANSLATE_NOOP("GroveEditor",
        "Merge Table Cell Above") : QT_TRANSLATE_NOOP("GroveEditor",
        "Merge Table Cell Below"));
    GroveCommandEventData gcmd(batch);

    Node* cell = columnSet[row_num - 1];
    Node* next_cell = columnSet[row_num]; //cell to delete
    while (above && cell == 0) {
        row_num--;
        cell = columnSet[row_num - 1];
    }
    while (!above && 0 == next_cell) {
        row_num++;
        next_cell = columnSet[row_num];
    }
    if (above)
        batch->setSuggestedPos(GrovePos(cell));
    else
        batch->setSuggestedPos(src_pos);
    int vspan = 0;
    bool last_row_is_empty = false;
    if (0 != next_cell) {
        int start = column, start_next = column;
        int end = 0, end_next = 0;
        getHSpanRange(plugin, cell, start, end);
        getHSpanRange(plugin, next_cell, start_next, end_next);
        if (start != start_next || end != end_next) {
            QString err(qApp->translate("TablePlugin", 
                        "Cell %1 has different horizontal span value.\n"
                        "Merge operation is cancelled."));
            plugin->structEditor()->sernaDoc()->
                showMessageBox(
                    SernaDoc::MB_CRITICAL, tr("Error"),
                    err.arg(above 
                            ? qApp->translate("TablePlugin", "above") 
                            : qApp->translate("TablePlugin", "below")), 
                    tr("&OK"));
            return;
        }
        if (next_cell->firstChild()) {
            DocumentFragmentPtr df = new DocumentFragment;
            GroveEditor::GrovePos from(next_cell, next_cell->firstChild());
            GroveEditor::GrovePos to(next_cell);
            editor->copy(from, to ,df.pointer());
            batch->executeAndAdd(editor->
                paste(df.pointer(),GrovePos(cell)).pointer());
        }
        vspan = getVSpanRange(plugin, next_cell);
        if (0 == next_cell->prevSibling() && 0 == next_cell->nextSibling()) {
            last_row_is_empty = true;
            next_cell = next_cell->parent();
        }
        batch->executeAndAdd(editor->removeNode(next_cell).pointer());
    }
    if (last_row_is_empty)
        resizeRowSpans(plugin, batch, cell, -1, cols);
    else {
        Attr* attr = static_cast<Element*>(cell)->attrs().
                     getAttribute(vspan_name(plugin));
        if (attr)
            batch->executeAndAdd(editor->setAttribute(attr,
                String::number(attr->value().toInt() + 1 + vspan)).pointer());
        else
            batch->executeAndAdd(editor->addAttribute(static_cast<Element*>(cell),
                new PropertyNode(vspan_name(plugin),
                String::number((vspan_counts_from_zero(plugin) ? 1 : 2) +
                                vspan))).pointer());
    }
    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin->structEditor());
    if (!above)
        plugin->update(src_pos); //position have not changed
}

void MergeCellAbove::execute()
{
    setEnabled(false);
    merge_vspan(plugin(), true);
}

void MergeCellAbove::update(const GroveEditor::GrovePos& src)
{
    Node* node = src.node();
    while(node && cell_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node || vspan_name(plugin()).isEmpty()) {
        setEnabled(false);
        return;
    }
    while(node && row_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node) {
        setEnabled(false);
        return;
    }
    setEnabled(0 != node->prevSibling());
}

void MergeCellBelow::execute()
{
    setEnabled(false);
    merge_vspan(plugin(), false);
}

void MergeCellBelow::update(const GroveEditor::GrovePos& src)
{
    Node* node = src.node();
    while(node && cell_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node || vspan_name(plugin()).isEmpty()) {
        setEnabled(false);
        return;
    }
    int vspan = getVSpanRange(plugin(), node);
    while(node && row_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node) {
        setEnabled(false);
        return;
    }
    for (int i = 0; node && i <= vspan; i++)
        node = node->nextSibling();
    setEnabled(0 != node);
}

