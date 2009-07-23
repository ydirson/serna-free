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
SIMPLE_EXECUTOR_IMPL(InsertTable, TablePlugin, OUT_OF_TABLE);
SIMPLE_EXECUTOR_IMPL(InsertQuickTable, TablePlugin, OUT_OF_TABLE);
SIMPLE_EXECUTOR_IMPL(DeleteTable, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(ColumnAttributes, TablePlugin, IS_IN_TABLE);

#include "TableUtils.h"
#include "plugins/tableplugin/TableBatchCommand.h"
#include "plugins/tableplugin/TablePlugin.h"
#include "common/PropertyTreeEventData.h"
#include "grove/Nodes.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GrovePos.h"
#include "structeditor/StructEditor.h"
#include "structeditor/ElementList.h"
#include "structeditor/GroveCommandEventData.h"
#include "xs/XsNodeExt.h"
#include <qstringlist.h>
#include <qmessagebox.h>


using namespace Common;
using namespace GroveLib;
using namespace TableUtils;

class InsertTableCmd;
class CallAttrsCmd;
class ExecuteAndUpdate;


static void insert_table(TablePlugin* plugin, PropertyNode* result)
{
    String columns = result->getSafeProperty("columns")->getString();
    bool ok;
    int cols = columns.toInt(&ok);
    if (!ok || columns.isEmpty()) {
//TODO message
        return;
    }
    String rows_str = result->getSafeProperty("rows")->getString();
    int rows = rows_str.toInt(&ok);
    if (!ok || rows_str.isEmpty()) {
//TODO message
        return;
    }
    Element* table = new Element(table_name(plugin));
    Element* tgroup = 0;
    if (!frame_name(plugin).isEmpty()) {
        String val = result->getSafeProperty("frame")->getString();
        String fvals = frame_values(plugin);
        if (!fvals.isEmpty()) {
            QStringList lst = QStringList::split(',', fvals);
            val = lst[val.toInt()].stripWhiteSpace();
        }
        if (!border_name(plugin).isEmpty()) {
            if (val != NOTR("none")) {
                Attr* border = new Attr(border_name(plugin));
                border->setValue(border_width(plugin));
                table->attrs().appendChild(border);
            }
            else
                val = "-";
        }
        if (val != "-") {
            Attr* frame = new Attr(frame_name(plugin));
            frame->setValue(val);
            table->attrs().appendChild(frame);
        }
    }
    if (result->getProperty("hasTitle") && !title_name(plugin).isEmpty())
        table->appendChild(new Element(title_name(plugin)));

    if (table_has_tgroup(plugin)) {
        tgroup = new Element(root_name(plugin));
        if (result->getProperty("hasColSeparator")) {
            Attr* colsep = new Attr(COLSEP);
            colsep->setValue("1");
            tgroup->attrs().appendChild(colsep);
        }
    }
    else
        tgroup = table;
    if (table_has_cols(plugin)) {
        Attr* attr = new Attr(COLS);
        attr->setValue(columns);
        tgroup->attrs().appendChild(attr);
    }

    int count = cols;
    if (!colspec_name(plugin).isEmpty()) {
        PropertyNode* node = result->getProperty("column_attrs");
        if (node)
            node = node->firstChild();
        for (; node && count; node = node->nextSibling()) {
            if (NOTR("attr") != node->name())
                continue;
            count--;
            Element* colspec = new Element(colspec_name(plugin));
            PropertyNode* attr = node->firstChild();
            for (; attr; attr = attr->nextSibling()) {
                String name = getNameByTable(plugin, attr->name());
                if (!name.isEmpty() && !attr->getString().isEmpty()) {
                    Attr* elem_attr = new Attr(name);
                    elem_attr->setValue(attr->getString());
                    colspec->attrs().appendChild(elem_attr);
                }
            }
            tgroup->appendChild(colspec);
        }
        int i = 0;
        for (; i < count; ++i) {
            Element* colspec = new Element(colspec_name(plugin));
            if (colspec_has_name(plugin)) {
                Attr* attr = new Attr(COLNAME);
                attr->setValue("c" + String::number(i+1));
                colspec->attrs().appendChild(attr);
            }
            tgroup->appendChild(colspec);
        }
    }
    if (result->getProperty("hasHeader") && !thead_name(plugin).isEmpty()) {
        Element* thead = new Element(thead_name(plugin));
        Element* row = new Element(row_name(plugin));
        for (int i = 0; i < cols; ++i) {
            Element* entry = new Element(cell_name(plugin));
            if (plugin->isAutoEntryPara())
                entry->appendChild(new Element(PARA));
            setExtraAttrs(plugin, entry);
            row->appendChild(entry);
        }
        thead->appendChild(row);
        tgroup->appendChild(thead);
    }
    if (result->getProperty("hasFooter") && !tfoot_name(plugin).isEmpty()) {
        Element* tfoot = new Element(tfoot_name(plugin));
        Element* row = new Element(row_name(plugin));
        for (int i = 0; i < cols; ++i) {
            Element* entry = new Element(cell_name(plugin));
            if (plugin->isAutoEntryPara())
                entry->appendChild(new Element(PARA));
            setExtraAttrs(plugin, entry);
            row->appendChild(entry);
        }
        tfoot->appendChild(row);
        tgroup->appendChild(tfoot);
    }
    Element* tbody = tgroup;
    if (!tbody_name(plugin).isEmpty())
        tbody = new Element(tbody_name(plugin));
    for (int r = 0; r < rows; ++r) {
        Element* row = new Element(row_name(plugin));
        for (int i = 0; i < cols; ++i) {
            Element* entry = new Element(cell_name(plugin));
            if (plugin->isAutoEntryPara())
                entry->appendChild(new Element(PARA));
            setExtraAttrs(plugin, entry);
            row->appendChild(entry);
        }
        tbody->appendChild(row);
    }
    if (!tbody_name(plugin).isEmpty())
        tgroup->appendChild(tbody);
    if (table_has_tgroup(plugin))
        table->appendChild(tgroup);
    if (result->getProperty("generateId")) {
        Attr* attr = new Attr(NOTR("id"));
        attr->setValue(plugin->structEditor()->generateId(NOTR("%t")));
        table->attrs().appendChild(attr);
    }
    if (result->getProperty("pgwide")) {
        Attr* attr = new Attr(NOTR("pgwide"));
        attr->setValue("1");
        table->attrs().appendChild(attr);
    }
    DocumentFragmentPtr df = new DocumentFragment;
    df->appendChild(table);
    GroveEditor::GrovePos pos;
    if (!plugin->structEditor()->getCheckedPos(pos))
        return;
    if (pos.node()->nodeType() == GroveLib::Node::CHOICE_NODE)
        pos = GroveEditor::GrovePos(pos.node()->parent(), pos.node());

    TableBatchCommand* batch = new TableBatchCommand(plugin);
    batch->setInfo(QT_TRANSLATE_NOOP("GroveEditor", "Insert Table"));
    GroveCommandEventData gcmd(batch);
    batch->executeAndAdd(plugin->structEditor()->
                         groveEditor()->paste(df.pointer(), pos).pointer());
    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin->structEditor());
}

static bool update_table(TablePlugin* plugin, const GroveEditor::GrovePos&)
{
    if (plugin->structEditor()->schema()) {
        String ns = namespace_name(plugin);
        if (ns.isEmpty())
            ns = "";
        bool res = plugin->structEditor()->elementList().
               canInsert(table_name(plugin), ns);
        return res;
    }
    return true;
}

void InsertQuickTable::execute()
{
    String size = uiAction()->get(Sui::TABLE_SIZE);
    if (size.isEmpty())
        return;
    int pos = size.find('x');
    if (0 > pos)
        return;
    PropertyTreeEventData result;
    PropertyNode* prop = result.root();
    prop->makeDescendant(NOTR("columns"))->setString(size.left(pos));
    prop->makeDescendant(NOTR("rows"))->setString(size.mid(pos+1));
    prop->makeDescendant(NOTR("frame"))->setString(NOTR("all"));
    prop->makeDescendant(NOTR("hasTitle"));
    insert_table(plugin(), prop);
}

void InsertQuickTable::update(const GroveEditor::GrovePos& src)
{
    setEnabled(update_table(plugin(), src));
}

void InsertTable::execute()
{
    PropertyTreeEventData result;
    if (!makeCommand<InsertTableCmd>()->execute(plugin(), &result))
        return;
    if (plugin()->checkDocbookChanges()) { //NOTE check for docbook type
        if (!result.root()->getProperty("hasTitle"))
            plugin()->infoSet()[NOTR("table")] = 
                new TablePlugin::TableInfo(NOTR("informaltable"), 0);
        else 
            plugin()->infoSet()[NOTR("table")] = 
                new TablePlugin::TableInfo(NOTR("table"), 0);
    }
    insert_table(plugin(), result.root());
}

void InsertTable::update(const GroveEditor::GrovePos& src)
{
    setEnabled(update_table(plugin(), src));
}

void DeleteTable::execute()
{
    GroveEditor::Editor* editor = plugin()->structEditor()->groveEditor();
    GroveEditor::GrovePos src_pos;
    if (!plugin()->structEditor()->getCheckedPos(src_pos))
        return;
    Node* node = src_pos.node();
    while(node && table_name(plugin()) != node->nodeName())
        node = node->parent();
    if (node) {
        TableBatchCommand* batch = new TableBatchCommand(plugin());
        batch->setInfo(QT_TRANSLATE_NOOP("GroveEditor", "Delete Table"));
        GroveCommandEventData gcmd(batch);
        batch->executeAndAdd(editor->removeNode(node).pointer());
        makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin()->structEditor());
    }
}

void DeleteTable::update(const GroveEditor::GrovePos& src)
{
    Node* node = src.node();
    while(node && table_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node) {
        setEnabled(false);
        return;
    }
    setEnabled(true);
}

void ColumnAttributes::execute()
{
    if (colspec_name(plugin()).isEmpty())
        return;
    GroveEditor::GrovePos pos;
    if (!plugin()->structEditor()->getCheckedPos(pos))
        return;
    int cur_col = 0;
    Node* node = pos.node();
    if (table_name(plugin()) == node->nodeName())
        node = node->firstChild();
    while(node && cell_name(plugin()) != node->nodeName()
               && root_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node)
        return;
    if (cell_name(plugin()) == node->nodeName())
        cur_col = rowCells(plugin(), node);
    while(node && root_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node)
        return;
    GroveEditor::Editor* editor = plugin()->structEditor()->groveEditor();
    Node* tgroup = node;
    PropertyTreeEventData result;
    PropertyNode* cattrs = result.root()->makeDescendant("column_attrs");
    node = tgroup->firstChild();
    int i = 0;
    for (; node; node = node->nextSibling()) {
        if (colspec_name(plugin()) != node->nodeName())
            continue;
        PropertyNode* pnode = new PropertyNode(NOTR("attr"));
        TablePlugin::TableInfoSet& dset = plugin()->infoSet();
        TablePlugin::TableInfoSet::iterator it =  dset.begin();
        for  (;it != dset.end(); ++it) {
            String nm = it->first;
            if (!starts_with(nm, NOTR("colspec-")))
                continue;
            String name = getNameByTable(plugin(), nm);
            if (!name.isEmpty()) {
                Attr* attr = ELEMENT_CAST(node)->attrs().getAttribute(name);
                if (attr)
                    pnode->makeDescendant(nm)->setString(attr->value());
            }
        }
        if (i == cur_col)
            pnode->makeDescendant("current");
        cattrs->appendChild(pnode);
        i++;
    }
    PropertyNode* backup = cattrs->copy(true);
    if (!makeCommand<CallAttrsCmd>()->execute(plugin(), &result))
        return;

    TableBatchCommand* batch = new TableBatchCommand(plugin());
    batch->setInfo(QT_TRANSLATE_NOOP("GroveEditor",
        "Change Table Column Attributes"));
    batch->setSuggestedPos(pos);
    GroveCommandEventData gcmd(batch);
    Vector<String> new_names;
    Vector<String> old_names;
    node = tgroup->firstChild();
    i = 1;
    for (; node; node = node->nextSibling(), i++) {
        if (colspec_name(plugin()) != node->nodeName())
                    continue;
        PropertyNode* pnode = result.root()->makeDescendant("column_attrs")->
                              firstChild();
        PropertyNode* bnode = backup->firstChild();
        for (; pnode && bnode; pnode = pnode->nextSibling(),
             bnode = bnode->nextSibling()) {
            int cur = pnode->getSafeProperty("colspec-num")->
                      getString().toInt();
            if (cur != i)
                continue;
            new_names.push_back(pnode->getSafeProperty("colspec-name")->getString());
            old_names.push_back(bnode->getSafeProperty("colspec-name")->getString());
            PropertyNode* pattr = pnode->firstChild();
            for (; pattr; pattr = pattr->nextSibling()) {
                String name = getNameByTable(plugin(), pattr->name());
                if (!name.isEmpty()) {
                    Attr* attr = ELEMENT_CAST(node)->attrs().getAttribute(name);
                    if (attr)  {
                        if (pattr->getString().isEmpty())
                            batch->executeAndAdd(editor->
                                removeAttribute(attr).pointer());
                        else
                            batch->executeAndAdd(editor->setAttribute(attr,
                               pattr->getString()).pointer());
                    }
                    else if (!pattr->getString().isEmpty())
                        batch->executeAndAdd(editor->addAttribute(
                            ELEMENT_CAST(node), new PropertyNode(name,
                            pattr->getString())).pointer());
                }
            }
        }
    }
    for (i = 0; i < (int)new_names.size(); ++i) {
        if (new_names[i] != old_names[i]) {
            String nn = new_names[i];
            for (int j = i + 1; j < (int)old_names.size(); ++j) {
                if (i != j && nn == old_names[j] &&
                    new_names[j] != old_names[j]) {
                    rebuildCalsHSpan(plugin(), batch, tgroup,
                                     new_names[j], old_names[j]);
                    new_names.erase(j);
                    old_names.erase(j);
                }
            }
            rebuildCalsHSpan(plugin(), batch, tgroup, new_names[i], old_names[i]);
        }
    }
    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin()->structEditor());
}

void ColumnAttributes::update(const GroveEditor::GrovePos& src)
{
    Node* node = src.node();
    while(node && table_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node) {
        setEnabled(false);
        return;
    }
    setEnabled(true);
}
