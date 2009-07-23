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
SIMPLE_EXECUTOR_IMPL(SplitCellBefore, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(SplitCellAfter, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(SplitCellAbove, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(SplitCellBelow, TablePlugin, IS_IN_CELL);

#include "TableUtils.h"
#include "plugins/tableplugin/TableBatchCommand.h"
#include "common/PropertyTreeEventData.h"
#include "grove/Nodes.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveCommand.h"
#include "structeditor/StructEditor.h"
#include "structeditor/GroveCommandEventData.h"

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;
using namespace TableUtils;

class ExecuteAndUpdate;

static void hsplit(TablePlugin* plugin,  bool prev)
{
    GroveEditor::Editor* editor = plugin->structEditor()->groveEditor();

    GroveEditor::GrovePos src_pos;
    if (!plugin->structEditor()->getCheckedPos(src_pos))
        return;
    Node* start_cell = src_pos.node();
    if (!check_support_level(plugin, start_cell))
        return;

    while(start_cell && cell_name(plugin) != start_cell->nodeName())
        start_cell = start_cell->parent();
    if (!start_cell)
        return;
    int start = 1;
    int end = 1;
    getHSpanRange(plugin, start_cell, start, end);
    if (end <= start)
        return;

    TableBatchCommand* batch = new TableBatchCommand(plugin);
    batch->setInfo(prev ? QT_TRANSLATE_NOOP("GroveEditor",
        "Split Table Cell Before") : QT_TRANSLATE_NOOP("GroveEditor",
        "Split Table Cell After"));
    batch->setSuggestedPos(src_pos);
    GroveCommandEventData gcmd(batch);

    if (1 == end - start)
        delHSpan(plugin, batch, start_cell);
    else {
        resizeHSpan(plugin, batch, start_cell, prev, -1, start);
    }
    Element* entry = new Element(cell_name(plugin));
    if (plugin->isAutoEntryPara())
        entry->appendChild(new Element(PARA));
    if (getVSpanRange(plugin, start_cell))
        copyAttributes(start_cell, entry, vspan_name(plugin));
    setExtraAttrs(plugin, entry);

    DocumentFragmentPtr df = new DocumentFragment;
    df->appendChild(entry);

    GroveEditor::GrovePos pos(start_cell->parent(),
        prev ? start_cell : start_cell->nextSibling());
    batch->executeAndAdd(editor->paste(df.pointer(), pos).pointer());
    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin->structEditor());
    plugin->update(src_pos); //position have not changed
}


void SplitCellBefore::execute()
{
    hsplit(plugin(), true);
}

void SplitCellBefore::update(const GroveEditor::GrovePos& src)
{
    Node* node = src.node();
    while(node && cell_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node) {
        setEnabled(false);
        return;
    }
    int start = 1;
    int end = 1;
    getHSpanRange(plugin(), node, start, end);
    setEnabled(start < end);
}

void SplitCellAfter::execute()
{
    hsplit(plugin(), false);
}

void SplitCellAfter::update(const GroveEditor::GrovePos& src)
{
    Node* node = src.node();
    while(node && cell_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node) {
        setEnabled(false);
        return;
    }
    int start = 1;
    int end = 1;
    getHSpanRange(plugin(), node, start, end);
    setEnabled(start < end);
}

static void vsplit(TablePlugin* plugin, bool above)
{
    GroveEditor::Editor* editor = plugin->structEditor()->groveEditor();

    GroveEditor::GrovePos src_pos;
    if (!plugin->structEditor()->getCheckedPos(src_pos))
        return;
    Node* cell = src_pos.node();
    if (!check_support_level(plugin, cell))
        return;
    while(cell && cell_name(plugin) != cell->nodeName())
        cell = cell->parent();
    if (!cell)
        return;
    int cur_col = rowCells(plugin, cell);
    int value = getVSpanRange(plugin, cell);
    Attr* attr = ELEMENT_CAST(cell)->attrs().getAttribute(vspan_name(plugin));

    Node* row = cell->parent();
    TableBatchCommand* batch = new TableBatchCommand(plugin);
    batch->setInfo(above ? QT_TRANSLATE_NOOP("GroveEditor",
        "Split Table Cell Above") : QT_TRANSLATE_NOOP("GroveEditor",
        "Split Table Cell Below"));
    batch->setSuggestedPos(src_pos);
    GroveCommandEventData gcmd(batch);
/*  Following code is for vertical split of not merged cells.
    Uncomment when SRS specify this behavior.
    if (0 == value) {
        row = row->nextSibling();
        resizeRowSpans(plugin, batch, cell, 1);
        Element* gen_row = new Element(row_name(plugin));
        Element* entry = new Element(cell_name(plugin));
        if (plugin->isAutoEntryPara())
            entry->appendChild(new Element(PARA));
        gen_row->appendChild(entry);
        copyHSpan(plugin, cell, entry);
        DocumentFragmentPtr df = new DocumentFragment;
        df->appendChild(gen_row);
        GrovePos pos(cell->parent()->parent(), row);
        batch->executeAndAdd(editor->paste(df.pointer(), pos).pointer());
        makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin->structEditor());
        plugin->update(src_pos);
        return;
    }
*/
    if (above)
        row = row->nextSibling();
    else
        for (int i = 0; i < value; i++)
            row = row->nextSibling();
    if (!row || !attr)
        return;
    Node* next_cell = 0;
    if (row->firstChild()) {
        NodeList rowset;
        rowCells(plugin, row->firstChild(), &rowset);
        int col = cur_col;
        while (col < (int)rowset.size() && 0 == rowset[col])
            col++;
        if (col < (int)rowset.size())
            next_cell = rowset[col];
    }

    Element* entry = new Element(cell_name(plugin));
    if (plugin->isAutoEntryPara())
        entry->appendChild(new Element(PARA));
    setExtraAttrs(plugin, entry);

    value--;
    if (0 == value || above) {
        batch->executeAndAdd(editor->removeAttribute(attr).pointer());
        if (value) {
            Attr* attr = new Attr(vspan_name(plugin));
            attr->setValue(String::number(
                          vspan_counts_from_zero(plugin) ? value : value + 1));
            entry->attrs().appendChild(attr);
        }
    }
    else if (!above)
        batch->executeAndAdd(editor->
            setAttribute(attr,String::number(
            vspan_counts_from_zero(plugin) ? value : value + 1)).pointer());
    copyHSpan(plugin, cell, entry);
    DocumentFragmentPtr df = new DocumentFragment;
    df->appendChild(entry);
    batch->executeAndAdd(editor->
        paste(df.pointer(),GrovePos(row, next_cell)).pointer());
    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin->structEditor());
    plugin->update(src_pos); //position have not changed
}

void SplitCellAbove::execute()
{
    vsplit(plugin(), true);
}

void SplitCellAbove::update(const GroveEditor::GrovePos& src)
{
    Node* node = src.node();
    while(node && cell_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node) {
        setEnabled(false);
        return;
    }
    setEnabled(0 < getVSpanRange(plugin(), node));
}

void SplitCellBelow::execute()
{
    vsplit(plugin(), false);
}

void SplitCellBelow::update(const GroveEditor::GrovePos& src)
{
    Node* node = src.node();
    while(node && cell_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node) {
        setEnabled(false);
        return;
    }
    setEnabled(0 < getVSpanRange(plugin(), node));
}

