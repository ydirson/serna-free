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

SIMPLE_EXECUTOR_IMPL(TableColumnSeparator, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(TableRowSeparator, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(TableFrameAll, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(TableFrameBottom, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(TableFrameSides, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(TableFrameNone, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(TableFrameTop, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(TableFrameTopBot, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(CurrentRowSeparator, TablePlugin, IS_IN_ROW);
SIMPLE_EXECUTOR_IMPL(CurrentCellRowSeparator, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(TableAlignCenter, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(TableAlignChar, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(TableAlignJustify, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(TableAlignLeft, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(TableAlignRight, TablePlugin, IS_IN_TABLE);
SIMPLE_EXECUTOR_IMPL(CellAlignCenter, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(CellAlignChar, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(CellAlignJustify, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(CellAlignLeft, TablePlugin, IS_IN_CELL);
SIMPLE_EXECUTOR_IMPL(CellAlignRight, TablePlugin, IS_IN_CELL);

#include "TableUtils.h"
#include "plugins/tableplugin/TableBatchCommand.h"
#include "common/PropertyTreeEventData.h"
#include "grove/Nodes.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveCommand.h"
#include "structeditor/StructEditor.h"
#include "structeditor/GroveCommandEventData.h"
#include <qstringlist.h>

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;
using namespace TableUtils;

class ExecuteAndUpdate;

static void insert_separator(TablePlugin* plugin, const String& batchName,
                             const String& nodeName,
                             const String& attrName, const String& valueList)
{
    GroveEditor::Editor* editor = plugin->structEditor()->groveEditor();
    GroveEditor::GrovePos pos;
    if (!plugin->structEditor()->getCheckedPos(pos))
        return;
    Node* node = pos.node();
    if (!check_support_level(plugin, node))
        return;
    while(node && nodeName != node->nodeName())
        node = node->parent();
    if (!node)
        return;
    TableAttrsBatchCommand* batch = new TableAttrsBatchCommand(plugin);
    batch->setInfo(batchName);
    GroveCommandEventData gcmd(batch);
    QStringList values = QStringList::split(',', valueList);
    Element* elem = static_cast<Element*>(node);
    Attr* attr = elem->attrs().getAttribute(attrName);
    bool executed = false;
    if (!border_name(plugin).isEmpty() && elem->nodeName() == table_name(plugin)) {
        Attr* border_attr = elem->attrs().getAttribute(border_name(plugin));
        if (border_attr) 
            batch->executeAndAdd(editor->setAttribute(border_attr, 
                                 border_width(plugin)).pointer());
        else
            batch->executeAndAdd(editor->addAttribute(elem,
                new PropertyNode(border_name(plugin), border_width(plugin))).pointer());
        executed = true;
    }
    if (attr) {
        if (values[0] != "-") {
            int cur = 0;
            for (int i = 0; i < (int)values.size() - 1; ++i) {
                if (attr->value() == values[i]) {
                    cur = i + 1;
                    break;
                }
            }
            batch->executeAndAdd(editor->setAttribute(attr,values[cur]).pointer());
        }
        else
            //elem->attrs().removeAttribute(attrName);
            batch->executeAndAdd(editor->removeAttribute(attr).pointer());
            //return;
        executed = true;
    }
    else if (values[0] != "-")
        batch->executeAndAdd(editor->addAttribute(elem,
            new PropertyNode(attrName, String(values[0]))).pointer());
        executed = true; 
    if (executed) {
        batch->setSuggestedPos(pos);
        makeCommand<ExecuteAndUpdate>(&gcmd)->execute(plugin->structEditor());
    }
}

void TableColumnSeparator::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Set Table Column Separator"), root_name(plugin()), COLSEP, NOTR("0,1"));
}

void TableRowSeparator::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Set Table Row Separator"), root_name(plugin()), ROWSEP, NOTR("0,1"));
}

void CurrentRowSeparator::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Set Current Row Separator"), row_name(plugin()), ROWSEP, NOTR("0,1"));
}

void CurrentCellRowSeparator::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Set Current Cell Row Separator"), cell_name(plugin()), ROWSEP, NOTR("0,1"));
}

void TableFrameAll::execute()
{
    String val = NOTR("all");
    String fvals = frame_values(plugin());
    if (!fvals.isEmpty()) {
        QStringList lst = QStringList::split(',', fvals);
        val = lst[0].stripWhiteSpace();
    }
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Set Table Frame: All"),
        table_name(plugin()), frame_name(plugin()), val);
}

void TableFrameBottom::execute()
{
    String val = NOTR("bottom");
    String fvals = frame_values(plugin());
    if (!fvals.isEmpty()) {
        QStringList lst = QStringList::split(',', fvals);
        val = lst[1].stripWhiteSpace();
    }
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Set Table Frame: Bottom"), table_name(plugin()), 
        frame_name(plugin()), val);
}
void TableFrameNone::execute()
{
    String val = NOTR("none");
    String fvals = frame_values(plugin());
    if (!fvals.isEmpty()) {
        QStringList lst = QStringList::split(',', fvals);
        val = lst[5].stripWhiteSpace();
    }
    if (!border_name(plugin()).isEmpty()) {
        insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
            "Set Table Frame: None"), table_name(plugin()), 
            border_name(plugin()), NOTR("-"));
    }
    else
        insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
            "Set Table Frame: None"), table_name(plugin()), 
            frame_name(plugin()), val);
}
void TableFrameSides::execute()
{
    String val = NOTR("sides");
    String fvals = frame_values(plugin());
    if (!fvals.isEmpty()) {
        QStringList lst = QStringList::split(',', fvals);
        val = lst[2].stripWhiteSpace();
    }
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Set Table Frame: Sides"), table_name(plugin()),
        frame_name(plugin()), val);
}
void TableFrameTop::execute()
{
    String val = NOTR("top");
    String fvals = frame_values(plugin());
    if (!fvals.isEmpty()) {
        QStringList lst = QStringList::split(',', fvals);
        val = lst[3].stripWhiteSpace();
    }
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Set Table Frame: Top"), table_name(plugin()),
        frame_name(plugin()), val);
}
void TableFrameTopBot::execute()
{
    String val = NOTR("topbot");
    String fvals = frame_values(plugin());
    if (!fvals.isEmpty()) {
        QStringList lst = QStringList::split(',', fvals);
        val = lst[4].stripWhiteSpace();
    }
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Set Table Frame: Top and Bottom"), table_name(plugin()),
        frame_name(plugin()), val);
}

void TableAlignCenter::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Table Align: Center"), root_name(plugin()), NOTR("align"), NOTR("center"));
}

void TableAlignChar::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Table Align: Char"), root_name(plugin()), NOTR("align"), NOTR("char"));
}

void TableAlignJustify::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Table Align: Justify"), root_name(plugin()), NOTR("align"), NOTR("justify"));
}

void TableAlignLeft::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Table Align: Left"), root_name(plugin()), NOTR("align"), NOTR("left"));
}

void TableAlignRight::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Table Align: Right"), root_name(plugin()), NOTR("align"), NOTR("right"));
}

void CellAlignCenter::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Table Cell Align: Center"), cell_name(plugin()), NOTR("align"), NOTR("center"));
}

void CellAlignChar::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Table Cell Align: Char"), cell_name(plugin()), NOTR("align"), NOTR("char"));
}

void CellAlignJustify::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Table Cell Align: Justify"), cell_name(plugin()), NOTR("align"), NOTR("justify"));
}

void CellAlignLeft::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Table Cell Align: Left"), cell_name(plugin()), NOTR("align"), NOTR("left"));
}

void CellAlignRight::execute()
{
    insert_separator(plugin(), QT_TRANSLATE_NOOP("GroveEditor",
        "Table Cell Align: Right"), cell_name(plugin()), NOTR("align"), NOTR("right"));
}

static void check_toggle_state(ActionExecutor* executor, Node* node,
                               const String& nodeName,
                               const String& attrName,
                               const String& value)
{
    if (!executor->uiAction()->getBool(Sui::IS_ENABLED)) {
        executor->uiAction()->setToggled(false);
        return;
    }
    while (node && nodeName != node->nodeName())
        node = parentNode(node);
    if (!node || node->nodeType() != Node::ELEMENT_NODE)
        return;
    Attr* attr = ELEMENT_CAST(node)->attrs().getAttribute(attrName);
    executor->uiAction()->setToggled((attr && attr->value() == value));
}

void TableFrameAll::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), table_name(plugin()),
                       frame_name(plugin()), NOTR("all"));
}

void TableFrameBottom::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), table_name(plugin()),
                       frame_name(plugin()), NOTR("bottom"));
}

void TableFrameNone::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), table_name(plugin()),
                       frame_name(plugin()), NOTR("none"));
}

void TableFrameSides::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), table_name(plugin()),
                       frame_name(plugin()), NOTR("sides"));
}

void TableFrameTop::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), table_name(plugin()),
                       frame_name(plugin()), NOTR("top"));
}

void TableFrameTopBot::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), table_name(plugin()),
                       frame_name(plugin()), NOTR("topbot"));
}

void TableAlignCenter::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), root_name(plugin()),
                       NOTR("align"), NOTR("center"));
}

void TableAlignChar::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), root_name(plugin()),NOTR("align"), NOTR("char"));
}

void TableAlignJustify::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), root_name(plugin()),
                       NOTR("align"), NOTR("justify"));
}

void TableAlignLeft::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), root_name(plugin()),
                       NOTR("align"), NOTR("left"));
}

void TableAlignRight::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), root_name(plugin()),
                       NOTR("align"), NOTR("right"));
}

void CellAlignCenter::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), cell_name(plugin()), NOTR("align"), NOTR("center"));
}

void CellAlignChar::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), cell_name(plugin()),NOTR("align"), NOTR("char"));
}

void CellAlignJustify::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(),cell_name(plugin()),NOTR("align"), NOTR("justify"));
}

void CellAlignLeft::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), cell_name(plugin()),NOTR("align"), NOTR("left"));
}

void CellAlignRight::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), cell_name(plugin()),NOTR("align"), NOTR("right"));
}

void TableColumnSeparator::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), root_name(plugin()), COLSEP, "0");
    if (uiAction()->getBool(Sui::IS_ENABLED))
        uiAction()->setToggled(!uiAction()->getBool(Sui::IS_TOGGLED));
}

void TableRowSeparator::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), root_name(plugin()), ROWSEP, "0");
    if (uiAction()->getBool(Sui::IS_ENABLED)) {
        bool rev = !uiAction()->getBool(Sui::IS_TOGGLED);
        uiAction()->setToggled(rev);
    }
}

void CurrentRowSeparator::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), row_name(plugin()), ROWSEP, "0");
    if (uiAction()->getBool(Sui::IS_ENABLED))
        uiAction()->setToggled(!uiAction()->getBool(Sui::IS_TOGGLED));
}

void CurrentCellRowSeparator::update(const GroveEditor::GrovePos& src)
{
    UpdateChecker::update(src);
    check_toggle_state(this, src.node(), cell_name(plugin()), ROWSEP, "0");
    if (uiAction()->getBool(Sui::IS_ENABLED))
        uiAction()->setToggled(!uiAction()->getBool(Sui::IS_TOGGLED));
}
