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
SIMPLE_EXECUTOR_IMPL(InsertColumnBefore, TablePlugin, IS_IN_ROW);
SIMPLE_EXECUTOR_IMPL(InsertColumnAfter, TablePlugin, IS_IN_ROW);
SIMPLE_EXECUTOR_IMPL(DeleteColumn, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(CopyColumn, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(CutColumn, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(PasteColumnBefore, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(PasteColumnAfter, TablePlugin, IS_IN_CELL);

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

static void insert_column_cells(TablePlugin* plugin, TableBatchCommand* batch,
     const NodeList& colSet, int curCol, bool prev, bool useClipboard = false)
{
    int i = -1;
    while(i < (int)colSet.size() - 1) {
        i++;
        Node* cell = colSet[i];
        if (cell == 0)
            continue;
        int start = i;
        int end = 0;
        getHSpanRange(plugin, cell, start, end);
        if (end > start && (prev ? start < curCol : end > curCol)) {
            if (!vspan_counts_from_zero(plugin))
                resizeHSpan(plugin, batch, cell, true, 1, curCol);
            continue;
        }
        String cname = cell_name(plugin);
        if (cell->nodeName() == cell_header_name(plugin)) {
            cname = cell_header_name(plugin);
        }
        Element* entry = new Element(cname);
        if (plugin->isAutoEntryPara())
            entry->appendChild(new Element(PARA));
        if (useClipboard && cell->parent()->parent()) {
            TablePlugin::TableClipboard& cb = plugin->clipboard();
            TablePlugin::TableClipboard::ClipContainer* current = 0;
            Node* node = cell->parent()->parent();
            if (is_thead(plugin, node->nodeName()))
                current = &cb.head;
            else if (is_tbody(plugin, node->nodeName()))
                current = &cb.body;
            else if (is_tfoot(plugin, node->nodeName()))
                current = &cb.foot;
            if (current && int(current->size()) > i && 
              !(*current)[i].isNull()) {
                DocumentFragmentPtr df = (*current)[i];
                entry = ELEMENT_CAST(df->firstChild()->cloneNode(true));
                i += getVSpanRange(plugin, entry);
                delHSpan(plugin, entry);
            }
        }
        else {
            i += getVSpanRange(plugin, cell);
            copyAttributes(cell, entry, vspan_name(plugin));
            setExtraAttrs(plugin, cell);
        }
        DocumentFragmentPtr cell_frag = new DocumentFragment;
        cell_frag->appendChild(entry);

        GroveEditor::GrovePos cell_pos(cell->parent(),
                                       prev ? cell : cell->nextSibling() );
        CommandPtr cmd = plugin->structEditor()->
                         groveEditor()->paste(cell_frag.pointer(), cell_pos);
        batch->executeAndAdd(cmd.pointer());
    }
}

static void delete_column_cells(TablePlugin* plugin,
                                TableBatchCommand* batch,
                                const NodeList& colSet,
                                int curCol)
{
    GroveEditor::Editor* editor = plugin->structEditor()->groveEditor();
    for (int i = 0; i < (int)colSet.size(); i++) {
        Node* cell = colSet[i];
        if (!cell)
            continue;
        int start = i;
        int end = 0;
        getHSpanRange(plugin, cell, start, end);
        if (end > start) {
            if (1 == end - start) {
                delHSpan(plugin, batch, cell);
            }
            else if (start == curCol) {
                resizeHSpan(plugin, batch, cell, true, -1, curCol);
            }
            else if (end == curCol) {
                resizeHSpan(plugin, batch, cell, false, -1, curCol);
            }
        }
        else
            batch->executeAndAdd(editor->removeNode(colSet[i]).pointer());
    }
}

static String make_unique_colname(const Node* spec)
{
    static String cgen(NOTR("cgen"));
    const Element* group = CONST_ELEMENT_CAST(spec->parent());
    int cgv = 0;
    for (const Node* n = group->firstChild(); n; n = n->nextSibling()) {
        if (n->nodeType() != Node::ELEMENT_NODE ||
            n->nodeName() != spec->nodeName())
            continue;
        const Attr* a = CONST_ELEMENT_CAST(n)->attrs().
            getAttribute(COLNAME);
        if (!a || a->value().left(4) != cgen)
            continue;
        bool ok = false;
        int v = a->value().mid(4).toInt(&ok);
        if (!ok)
            continue;
        if (v > cgv)
            cgv = v;    // find largest
    }
    ++cgv;
    return cgen + String::number(cgv);
}

static void insert_delete_column(TablePlugin* plugin, bool prev,
                                 bool remove = false,
                                 bool useClipboard = false)
{
    TableBatchCommand* batch = new TableBatchCommand(plugin);
    batch->setInfo(remove ? QT_TRANSLATE_NOOP("GroveEditor",
        "Delete Table Column") :
            (prev ? QT_TRANSLATE_NOOP("GroveEditor", 
                "Insert Table Column Before") :
                    QT_TRANSLATE_NOOP("GroveEditor",
                        "Insert Table Column After")));
    GroveCommandEventData gcmd(batch);

    GroveEditor::Editor* editor = plugin->structEditor()->groveEditor();
    int cur_col  = 0;
    int sel_cntr = 1;
    Node* cell = 0;
    Node* node = 0;
    Node* next_cell = 0;
    Node* end_cell = 0;
    if (remove) {
        if (getSelectionCells(plugin, cell, end_cell)) {
            if (cell && end_cell && cell->parent() != end_cell->parent())
                end_cell = cell->parent()->lastChild();
            sel_cntr = 2;
        }
    }
    if (sel_cntr == 1) {
        GroveEditor::GrovePos pos;
        if (!plugin->structEditor()->getCheckedPos(pos))
            return;
        node = pos.node();
        Node* before = (GrovePos::ELEMENT_POS == pos.type()) ? pos.before() : 0;
        String sname = node->nodeName();
        if (before && is_cell(plugin, before->nodeName()))
            node = prev ? before
                        : (before->prevSibling()
                            ? before->prevSibling() : before);
        else if (row_name(plugin) == sname || thead_name(plugin) == sname ||
             tbody_name(plugin) == sname || tfoot_name(plugin) == sname)
            while (node && !is_cell(plugin, node->nodeName()))
                node = before ? node->firstChild() : node->lastChild();

        while(node && !is_cell(plugin, node->nodeName()))
            node = node->parent();
        cell = node;
        if (!remove)
            batch->setSuggestedPos(pos);
    }
    node = cell;
    while(node && root_name(plugin) != node->nodeName())
        node = node->parent();
    if (!node)
        return;
    Element* tgroup = static_cast<Element*>(node);
    int cols_count = columns(plugin);

    while (sel_cntr && cell) {
        NodeList body_colset;
        NodeList header_colset;
        NodeList footer_colset;
        cur_col = rowCells(plugin, cell, 0, 0, true, cols_count);
        Node* cur_spec = specByNum(plugin, cur_col, tgroup);
        int count = cur_col;
        int start = cur_col;
        int end = 0;
        getHSpanRange(plugin, cell, start, end);
        if (end > start) {
            count = prev ? start : end + 1;
            cur_col = prev ? start : end;
            next_cell = cell;
        }
        else
            next_cell = cell->nextSibling();

        if (!remove || !end_cell ||
           (end_cell && end_cell == cell && next_cell != cell))
            sel_cntr = 0;

        Node* parent = 0;
        if (cell->parent() && cell->parent()->parent()) {
            parent = cell->parent()->parent();
            if (is_thead(plugin, parent->nodeName()))
                columnCells(plugin, cell, cur_col, &header_colset);
            else if (is_tfoot(plugin, parent->nodeName()))
                columnCells(plugin, cell, cur_col, &footer_colset);
            else
                columnCells(plugin, cell, cur_col, &body_colset);
        }

        for(node = tgroup->firstChild();node; node = node->nextSibling()) {
            if (node == parent)
                continue;
            if (!is_thead(plugin, node->nodeName()) &&
                !is_tfoot(plugin, node->nodeName()) &&
                !is_tbody(plugin, node->nodeName()))
                continue;
            if (node && node->firstChild() &&
                node->firstChild()->firstChild()) {
                NodeList rowset;
                rowCells(plugin, node->firstChild()->firstChild(),
                         &rowset, 0, true, cols_count);
                if (rowset[cur_col]) {
                    NodeList* extraset = 0;
                    if (is_thead(plugin, node->nodeName()))
                        extraset = &header_colset;
                    else if (is_tfoot(plugin, node->nodeName()))
                        extraset = &footer_colset;
                    else if (is_tbody(plugin, node->nodeName()))
                        extraset = &body_colset;
                    columnCells(plugin, rowset[cur_col], cur_col, extraset);
                }
            }
        }

        if (table_has_cols(plugin)) {
            Attr* attr = tgroup->attrs().getAttribute(COLS);
            int delta = remove ? -1 : 1;
            int total_cols = attr->value().toInt();
            batch->executeAndAdd(editor->setAttribute(attr,
                               String::number(total_cols + delta)).pointer());
        }
        node = tgroup->firstChild();
        Node* spec = 0;
        bool table_use_colnum = false;
        bool cur_colnum_exists = false;
        for (; node; node = node->nextSibling()) {
            if (colspec_name(plugin) != node->nodeName())
                        continue;
            Attr* attr = ELEMENT_CAST(node)->attrs().getAttribute(COLNUM);
            if (attr) {
                table_use_colnum = true;
                if (cur_col == attr->value().toInt() - 1) {
                    cur_colnum_exists = true;
                    spec = node;
                    break;
                }
            }
            if (0 == count)
                spec = node;
            count--;
        }
        if (remove) {
            if (header_colset.size())
                delete_column_cells(plugin, batch, header_colset, cur_col);
            if (body_colset.size())
                delete_column_cells(plugin, batch, body_colset, cur_col);
            if (footer_colset.size())
                delete_column_cells(plugin, batch, footer_colset, cur_col);
            if (spec && colspec_name(plugin) == spec->nodeName()) {
                if (cur_colnum_exists) {
                    int count = ELEMENT_CAST(spec)->attrs().
                        getAttribute(COLNUM)->value().toInt();
                    node = tgroup->firstChild();
                    for (; node; node = node->nextSibling()) {
                        if (colspec_name(plugin) != node->nodeName())
                            continue;
                        Attr* attr = ELEMENT_CAST(node)->
                                     attrs().getAttribute(COLNUM);
                        if (attr && count < attr->value().toInt()) {
                            batch->executeAndAdd(editor->setAttribute(attr,
                                String::number(attr->value().toInt() - 1)).
                                pointer());
                        }
                    }
                }
                batch->executeAndAdd(editor->removeNode(spec).pointer());
                cell = next_cell;
            }
            cols_count--;
        }
        else {
            if (spec && colspec_name(plugin) == spec->nodeName()) {
                ElementPtr colspec = new Element(colspec_name(plugin));
                if (colspec_has_name(plugin)) {
                    Attr* attr = new Attr(COLNAME);
                    colspec->attrs().appendChild(attr);
                    attr->setValue(make_unique_colname(spec));
                    if (table_use_colnum) {
                        Attr* cnum = new Attr(COLNUM);
                        count = prev ? cur_col + 1 : cur_col + 2;
                        cnum->setValue(String::number(count));
                        colspec->attrs().appendChild(cnum);
                        if (cur_colnum_exists) {
                            node = tgroup->firstChild();
                            for (; node; node = node->nextSibling()) {
                                if (colspec_name(plugin) != node->nodeName())
                                    continue;
                                Attr* attr = ELEMENT_CAST(node)->
                                             attrs().getAttribute(COLNUM);
                                if (attr && count <= attr->value().toInt()) {
                                    batch->executeAndAdd(editor->setAttribute(
                                        attr, String::number
                                        (attr->value().toInt() + 1)).
                                        pointer());
                                }
                            }
                        }
                    }
                    Node* spec = cur_spec;
                    if (useClipboard && !plugin->clipboard().spec.isNull())
                        spec = plugin->clipboard().spec.pointer();
                    if (spec) {
                        for(Attr* a = ELEMENT_CAST(spec)->attrs().firstChild();
                             a; a = a->nextSibling()) {
                            String name = a->nodeName();
                            if (COLNAME != name && COLNUM != name) {
                                Attr* attr = new Attr(name);
                                attr->setValue(a->value());
                                colspec->attrs().appendChild(attr);
                            }
                        }
                    }
                }
                DocumentFragmentPtr colspec_frag = new DocumentFragment;
                colspec_frag->appendChild(colspec.pointer());
                GroveEditor::GrovePos colspec_pos(tgroup,
                    prev ? spec : spec->nextSibling());
                batch->executeAndAdd(editor->paste(colspec_frag.pointer(),
                    colspec_pos).pointer());
            }
            if (header_colset.size())
                insert_column_cells(plugin, batch, header_colset,
                                    cur_col, prev, useClipboard);
            if (body_colset.size())
                insert_column_cells(plugin, batch, body_colset,
                                    cur_col, prev, useClipboard);
            if (footer_colset.size())
                insert_column_cells(plugin, batch, footer_colset,
                                    cur_col, prev, useClipboard);
        }
    }
    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin->structEditor());
}

void InsertColumnBefore::execute()
{
    insert_delete_column(plugin(), true);
}

void InsertColumnBefore::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
}

void InsertColumnAfter::execute()
{
    insert_delete_column(plugin(), false);
}

void InsertColumnAfter::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
}

void DeleteColumn::execute()
{
    insert_delete_column(plugin(), true, true);
}

static void copy_column(TablePlugin* plugin)
{
    GroveEditor::GrovePos src_pos;
    if (!plugin->structEditor()->getCheckedPos(src_pos))
        return;
    Node* cell = src_pos.node();
    while(cell && !is_cell(plugin, cell->nodeName()))
        cell = cell->parent();
    if (!cell)
        return;
    Node* tgroup = cell;
    while(tgroup && root_name(plugin) != tgroup->nodeName())
        tgroup = tgroup->parent();

    NodeList colset;
    NodeList body_colset;
    int cur_col = rowCells(plugin, cell);
    columnCells(plugin, cell, cur_col, &body_colset);
    TablePlugin::TableClipboard& cb = plugin->clipboard();
    TablePlugin::TableClipboard::ClipContainer* current = 0;
    for (Node* node = tgroup->firstChild(); node; node = node->nextSibling()) {
        if (is_thead(plugin, node->nodeName()))
            current = &cb.head;
        else if (is_tbody(plugin, node->nodeName()))
            current = &cb.body;
        else if (is_tfoot(plugin, node->nodeName()))
            current = &cb.foot;
        else
            continue;
        NodeList colset;
        if (node == cell->parent()->parent()) {
            for (uint i = 0; i < body_colset.size(); ++i)
                colset.push_back(body_colset[i]);
        }
        else if (node->firstChild() && node->firstChild()->firstChild()) {
            NodeList rowset;
            rowCells(plugin, node->firstChild()->firstChild(), &rowset);
            if (rowset[cur_col])
                columnCells(plugin, rowset[cur_col], cur_col, &colset);
        }
        current->clear();
        for (uint i = 0; i < colset.size(); ++i) {
            Node* child = colset[i];
            DocumentFragmentPtr df;
            if (child && child->firstChild())
                df = child->copyAsFragment();
            current->push_back(df);
        }
    }
    cb.spec = specByNum(plugin, cur_col, tgroup);
}

void CopyColumn::execute()
{
    copy_column(plugin());
    GroveEditor::GrovePos pos;
    if (plugin()->structEditor()->getCheckedPos(pos))
        plugin()->update(pos);
}

void CutColumn::execute()
{
    copy_column(plugin());
    insert_delete_column(plugin(), true, true);
}

void PasteColumnBefore::execute()
{
    insert_delete_column(plugin(), true, false, true);
}

void PasteColumnBefore::update(const GroveEditor::GrovePos& src)
{
    if (0 == plugin()->clipboard().body.size())
        setEnabled(false);
    else
        setEnabled(is_in(cell_name(plugin()), src.node()));
}

void PasteColumnAfter::execute()
{
    insert_delete_column(plugin(), false, false, true);
}

void PasteColumnAfter::update(const GroveEditor::GrovePos& src)
{
    if (0 == plugin()->clipboard().body.size())
        setEnabled(false);
    else
        setEnabled(is_in(cell_name(plugin()), src.node()));
}

void DeleteColumn::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
}

void CopyColumn::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
}

void CutColumn::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
}
