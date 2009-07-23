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
SIMPLE_EXECUTOR_IMPL(InsertHeader, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(InsertFooter, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(DeleteHeader, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(DeleteFooter, TablePlugin, IS_IN_TABLE);

#include "TableUtils.h"
#include "plugins/tableplugin/TableBatchCommand.h"
#include "common/PropertyTreeEventData.h"
#include "grove/Nodes.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GrovePos.h"
#include "structeditor/StructEditor.h"
#include "structeditor/GroveCommandEventData.h"

using namespace Common;
using namespace GroveLib;
using namespace TableUtils;

class ExecuteAndUpdate;

static void insert_header_or_footer(TablePlugin* plugin, const String name) {
    GroveEditor::GrovePos src_pos;
    if (!plugin->structEditor()->getCheckedPos(src_pos))
        return;
    Node* node = src_pos.node();
    while(node && root_name(plugin) != node->nodeName())
        node = node->parent();
    if (!node)
        return;
    bool is_header = (name == thead_name(plugin));
    Node* tgroup = node;
    Node* before = 0;
    for(node = tgroup->firstChild(); node; node = node->nextSibling()) {
        if (name == node->nodeName())
            return; //thead already exist
        if (tbody_name(plugin) == node->nodeName() ||
             row_name(plugin) == node->nodeName()) {
            before = node;
            break;
        }
    }

    int cols = columns(plugin);
    if (0 >= cols)
        cols = 1;
    Element* telem = new Element(name);
    Element* row = new Element(row_name(plugin));
    for (int i = 0; i < cols; ++i) {
        Element* entry = new Element(cell_header_name(plugin));
        if (plugin->isAutoEntryPara())
            entry->appendChild(new Element(PARA));
        setExtraAttrs(plugin, entry);
        row->appendChild(entry);
    }
    telem->appendChild(row);
    DocumentFragmentPtr df = new DocumentFragment;
    df->appendChild(telem);

    GroveEditor::GrovePos pos(tgroup, before);
    TableBatchCommand* batch = new TableBatchCommand(plugin);
    batch->setInfo(is_header ? QT_TRANSLATE_NOOP("GroveEditor", 
        "Insert Table Header") : QT_TRANSLATE_NOOP("GroveEditor",
        "Insert Table Footer"));
    GroveCommandEventData gcmd(batch);
    batch->executeAndAdd(plugin->structEditor()->
                         groveEditor()->paste(df.pointer(), pos).pointer());
    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin->structEditor());
}


void InsertHeader::execute()
{
    if (!thead_name(plugin()).isEmpty())
        insert_header_or_footer(plugin(), thead_name(plugin()));
}


void InsertHeader::update(const GroveEditor::GrovePos& src)
{
    Node* node = src.node();
    while(node && root_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node || thead_name(plugin()).isEmpty()) {
        setEnabled(false);
        return;
    }
    Node* tgroup = node;
    for(node = tgroup->firstChild(); node; node = node->nextSibling()) {
        if (thead_name(plugin()) == node->nodeName()) {
           setEnabled(false);
           return;
        }
    }
    setEnabled(true);
}

void InsertFooter::execute()
{
    if (!tfoot_name(plugin()).isEmpty())
        insert_header_or_footer(plugin(), tfoot_name(plugin()));
}

void InsertFooter::update(const GroveEditor::GrovePos& src)
{
    Node* node = src.node();
    while(node && root_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node || tfoot_name(plugin()).isEmpty()) {
        setEnabled(false);
        return;
    }
    Node* tgroup = node;
    for(node = tgroup->firstChild(); node; node = node->nextSibling()) {
        if (tfoot_name(plugin()) == node->nodeName()) {
           setEnabled(false);
           return;
        }
    }
    setEnabled(true);
}

void DeleteHeader::execute()
{
    GroveEditor::Editor* editor = plugin()->structEditor()->groveEditor();
    GroveEditor::GrovePos src_pos;
    if (!plugin()->structEditor()->getCheckedPos(src_pos))
        return;
    Node* node = src_pos.node();
    while(node && root_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node)
        return;
    TableBatchCommand* batch = new TableBatchCommand(plugin());
    batch->setInfo(QT_TRANSLATE_NOOP("GroveEditor", "Delete Table Header"));
    GroveCommandEventData gcmd(batch);
    for( node = node->firstChild();node; node = node->nextSibling()) {
        if (is_thead(plugin(), node->nodeName())) {
            batch->executeAndAdd(editor->removeNode(node).pointer());
            break;
        }
    }
    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin()->structEditor());
}

void DeleteHeader::update(const GroveEditor::GrovePos& src)
{
    Node* node = src.node();
    while(node && root_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node || thead_name(plugin()).isEmpty()) {
        setEnabled(false);
        return;
    }
    Node* tgroup = node;
    for(node = tgroup->firstChild(); node; node = node->nextSibling()) {
        if (thead_name(plugin()) == node->nodeName()) {
           setEnabled(true);
           return;
        }
    }
    setEnabled(false);
}

void DeleteFooter::execute()
{
    GroveEditor::Editor* editor = plugin()->structEditor()->groveEditor();
    GroveEditor::GrovePos src_pos;
    if (!plugin()->structEditor()->getCheckedPos(src_pos))
        return;
    Node* node = src_pos.node();
    while(node && root_name(plugin()) != node->nodeName())
        node = node->parent();
//TODO warning
    if (!node)
        return;
    TableBatchCommand* batch = new TableBatchCommand(plugin());
    batch->setInfo(QT_TRANSLATE_NOOP("GroveEditor", "Delete Table Footer"));
    GroveCommandEventData gcmd(batch);
    for( node = node->firstChild();node; node = node->nextSibling()) {
        if (tfoot_name(plugin()) == node->nodeName()) {
            batch->executeAndAdd(editor->removeNode(node).pointer());
            break;
        }
    }
    makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin()->structEditor());
}

void DeleteFooter::update(const GroveEditor::GrovePos& src)
{
    Node* node = src.node();
    while(node && root_name(plugin()) != node->nodeName())
        node = node->parent();
    if (!node || tfoot_name(plugin()).isEmpty()) {
        setEnabled(false);
        return;
    }
    Node* tgroup = node;
    for(node = tgroup->firstChild(); node; node = node->nextSibling()) {
        if (tfoot_name(plugin()) == node->nodeName()) {
           setEnabled(true);
           return;
        }
    }
    setEnabled(false);
}

