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
#include "plugins/tableplugin/TablePlugin.h"
#include "plugins/tableplugin/qt/TableButton.h"
#include "plugins/tableplugin/TableUtils.h"
#include "common/PropertyTree.h"
#include "utils/GrovePosEventData.h"
#include "utils/Config.h"
#include "structeditor/StructEditor.h"
#include "structeditor/SetCursorEventData.h"
#include "proputils/PropertyTreeSaver.h"
#include "grove/Nodes.h"
#include "ui/UiItems.h"
#include "ui/UiItemSearch.h"

#include <qfileinfo.h>
#include <string>
#include <iostream>

class InsertTable;
class InsertQuickTable;
class InsertHeader;
class InsertFooter;
class InsertRowAbove;
class InsertRowBelow;
class InsertColumnBefore;
class InsertColumnAfter;
class InsertCellBefore;
class InsertCellAfter;
class MergeSelectedCells;
class MergeCellAbove;
class MergeCellBelow;
class SplitCellBefore;
class SplitCellAfter;
class SplitCellAbove;
class SplitCellBelow;
class DeleteTable;
class DeleteHeader;
class DeleteFooter;
class DeleteRow;
class DeleteColumn;
class DeleteCell;
class DeleteCellContent;
class CopyColumn;
class CutColumn;
class PasteColumnBefore;
class PasteColumnAfter;
class ColumnAttributes;
class TableColumnSeparator;
class TableRowSeparator;
class CurrentRowSeparator;
class CurrentCellRowSeparator;
class TableFrameAll;
class TableFrameBottom;
class TableFrameSides;
class TableFrameNone;
class TableFrameTop;
class TableFrameTopBot;
class TableAlignCenter;
class TableAlignChar;
class TableAlignJustify;
class TableAlignLeft;
class TableAlignRight;
class CellAlignCenter;
class CellAlignChar;
class CellAlignJustify;
class CellAlignLeft;
class CellAlignRight;



using namespace Common;
using namespace GroveLib;
using namespace TableUtils;

// START_IGNORE_LITERALS
extern const char* const IS_IN_TABLE = "0";
extern const char* const IS_IN_CELL  = "1";
extern const char* const IS_IN_ROW   = "2";
extern const char* const OUT_OF_TABLE= "3";
const char* const PARAMS_ROOT   = "tableParameters";
const char* const SUFFIX        = ".tpp";
const char* const NAME          = "name";
const char* const VALUE         = "value";
const char* const TABLE_NAME    = "table";
// STOP_IGNORE_LITERALS

/////////////////////////////////////////////////////////////////////////


GROVEPOS_EVENT_IMPL(UpdateTableMenuCmd, DocumentPlugin)

SELECTION_EVENT_IMPL(UpdateSelectedTableMenuCmd, DocumentPlugin)

bool UpdateSelectedTableMenuCmd::doExecute(DocumentPlugin* watcher,
                                           Common::EventData*)
{
    if (groveSelection_.isEmpty()) {
        TablePlugin* table_plugin = dynamic_cast<TablePlugin*>(watcher);
        if (table_plugin)
            table_plugin->update(table_plugin->
                structEditor()->editViewSrcPos());
    }
    return true;
}


bool UpdateTableMenuCmd::doExecute(DocumentPlugin* watcher, Common::EventData*)
{
    TablePlugin* table_plugin = dynamic_cast<TablePlugin*>(watcher);
    if (table_plugin)
        table_plugin->update(pos_);
    return true;
}

/////////////////////////////////////////////////////////////////////////
class TableButton;

TablePlugin::TablePlugin(SernaApiBase* ssd, SernaApiBase* properties, char**)
        : DocumentPlugin(ssd, properties),
          needAttrsUpdate_(false)
{
#ifndef NDEBUG
    //The following lines are needed to use table button in development cases
    sernaDoc()->addItemMaker(
        Sui::TABLE_BUTTON,
        new Sui::CustomItemMaker(Sui::makeUiItem<TableButton>), 
        Sui::TOOL_BAR_WIDGET);
    REGISTER_UI_EXECUTOR(InsertQuickTable);
#endif

    REGISTER_UI_EXECUTOR(InsertTable);
    REGISTER_UI_EXECUTOR(InsertHeader);
    REGISTER_UI_EXECUTOR(InsertFooter);
    REGISTER_UI_EXECUTOR(InsertRowAbove);
    REGISTER_UI_EXECUTOR(InsertRowBelow);
    REGISTER_UI_EXECUTOR(InsertColumnBefore);
    REGISTER_UI_EXECUTOR(InsertColumnAfter);
    REGISTER_UI_EXECUTOR(InsertCellBefore);
    REGISTER_UI_EXECUTOR(InsertCellAfter);
    REGISTER_UI_EXECUTOR(MergeSelectedCells);
    REGISTER_UI_EXECUTOR(MergeCellAbove);
    REGISTER_UI_EXECUTOR(MergeCellBelow);
    REGISTER_UI_EXECUTOR(SplitCellBefore);
    REGISTER_UI_EXECUTOR(SplitCellAfter);
    REGISTER_UI_EXECUTOR(SplitCellAbove);
    REGISTER_UI_EXECUTOR(SplitCellBelow);
    REGISTER_UI_EXECUTOR(DeleteTable);
    REGISTER_UI_EXECUTOR(DeleteHeader);
    REGISTER_UI_EXECUTOR(DeleteFooter);
    REGISTER_UI_EXECUTOR(DeleteRow);
    REGISTER_UI_EXECUTOR(DeleteColumn);
    REGISTER_UI_EXECUTOR(DeleteCell);
    REGISTER_UI_EXECUTOR(DeleteCellContent);
    REGISTER_UI_EXECUTOR(CopyColumn);
    REGISTER_UI_EXECUTOR(CutColumn);
    REGISTER_UI_EXECUTOR(PasteColumnBefore);
    REGISTER_UI_EXECUTOR(PasteColumnAfter);
    REGISTER_UI_EXECUTOR(ColumnAttributes);
    REGISTER_UI_EXECUTOR(TableColumnSeparator);
    REGISTER_UI_EXECUTOR(TableRowSeparator);
    REGISTER_UI_EXECUTOR(CurrentRowSeparator);
    REGISTER_UI_EXECUTOR(CurrentCellRowSeparator);
    REGISTER_UI_EXECUTOR(TableFrameAll);
    REGISTER_UI_EXECUTOR(TableFrameBottom);
    REGISTER_UI_EXECUTOR(TableFrameSides);
    REGISTER_UI_EXECUTOR(TableFrameNone);
    REGISTER_UI_EXECUTOR(TableFrameTop);
    REGISTER_UI_EXECUTOR(TableFrameTopBot);
    REGISTER_UI_EXECUTOR(TableAlignCenter);
    REGISTER_UI_EXECUTOR(TableAlignChar);
    REGISTER_UI_EXECUTOR(TableAlignJustify);
    REGISTER_UI_EXECUTOR(TableAlignLeft);
    REGISTER_UI_EXECUTOR(TableAlignRight);
    REGISTER_UI_EXECUTOR(CellAlignCenter);
    REGISTER_UI_EXECUTOR(CellAlignChar);
    REGISTER_UI_EXECUTOR(CellAlignJustify);
    REGISTER_UI_EXECUTOR(CellAlignLeft);
    REGISTER_UI_EXECUTOR(CellAlignRight);

    se_ = dynamic_cast<StructEditor*>(
        sernaDoc()->findItem(Sui::ItemClass(Sui::STRUCT_EDITOR)));
    if (!se_)
        return;
    se_->elementContextChange().subscribe(
         makeCommand<UpdateTableMenuCmd>, this);
    se_->selectionChange().subscribe(
         makeCommand<UpdateSelectedTableMenuCmd>, this);
    buildPluginExecutors();
    params_ = registerPropTree(SUFFIX, PARAMS_ROOT);
    PropertyNode* tbls = params_->firstChild();
    if (tbls && tbls->name()==TABLE_NAME) {
        for (; tbls ; tbls = tbls->nextSibling()) {
            PropertyNode* p = tbls->firstChild();
            String format = p->getSafeProperty(NAME)->getString();
            tableFormatName_.push_back(format);
            p = p->nextSibling();
            for (; p ; p = p->nextSibling()) {
                String name = p->getSafeProperty(NAME)->getString();
                if (0 < p->name().find(NOTR("-extra-attr"))) {
                    String val = p->getSafeProperty(VALUE)->getString();
                    String elem = p->name().left(
                                  p->name().find(NOTR("-extra-attr")));
                    AttrsSet::iterator attrs = attrsSet().find(elem);
                    if (attrs != attrsSet().end())
                        AttrsSet_[elem].push_back(AttrInfo(name, val));
                    else {
                        AttrsContainer container;
                        container.push_back(AttrInfo(name, val));
                        AttrsSet_[elem] = container;
                    }
                    continue;
                }
                int value = p->getSafeProperty(VALUE)->getInt();
                TableInfoSet::iterator info =  infoSet().find(p->name());
                if (info != infoSet().end()) {
                    info->second->name.push_back(name);
                    info->second->lev.push_back(value);
                }
                else 
                    TableInfoSet_[p->name()]= new TableInfo(name, value);
            }
       }
   }
   else {
// START_IGNORE_LITERALS
        tableFormatName_.push_back("CALS");
        TableInfoSet_["table"]  = new TableInfo("table", 0);
        TableInfoSet_["tgroup"] = new TableInfo("tgroup",1);
        TableInfoSet_["title"]  = new TableInfo("title",1);
        TableInfoSet_["thead"]  = new TableInfo("thead", 2);
        TableInfoSet_["tbody"]  = new TableInfo("tbody", 2);
        TableInfoSet_["tfoot"]  = new TableInfo("tfoot", 2);
        TableInfoSet_["row"]    = new TableInfo("row",   3);
        TableInfoSet_["entry"]  = new TableInfo("entry", 4);
        TableInfoSet_["colspec"]       = new TableInfo("colspec",  2);
        TableInfoSet_["spanspec"]       = new TableInfo("spanspec",  2);
        TableInfoSet_["entry-rowspan"] = new TableInfo("morerows", 0);
        TableInfoSet_["entry-colspan"] = new TableInfo("namest, nameend", -1);
        TableInfoSet_["colspec-num"]   = new TableInfo("colnum",  -1);
        TableInfoSet_["colspec-name"]  = new TableInfo("colname", -1);
        TableInfoSet_["colspec-width"] = new TableInfo("colwidth",-1);
        TableInfoSet_["colspec-align"] = new TableInfo("align",   -1);
        TableInfoSet_["colspec-colsep"]= new TableInfo("colsep",  -1);
        TableInfoSet_["colspec-rowsep"]= new TableInfo("rowsep",  -1);
        TableInfoSet_["table-border"]= new TableInfo("",  -1);
        TableInfoSet_["table-frame"]= new TableInfo("frame",  -1);
        TableInfoSet_["table-frame-values"]= new TableInfo("all,bottom,sides,top,topbot,none",  -1);
        TableInfoSet_["tgroup-cols"]= new TableInfo("cols",  -1);
        TableInfoSet_["generate-id"]= new TableInfo("id",  -1);
        TableInfoSet_["namespace"]= new TableInfo("",  -1);
// STOP_IGNORE_LITERALS
   }
}

TablePlugin::~TablePlugin()
{
}

void TablePlugin::postInit()
{
    update(structEditor()->editViewSrcPos());
}

void TablePlugin::update(const GroveEditor::GrovePos& src)
{
    pendingPos_ = src;
    IdleHandler::registerHandler(this);
}

void TablePlugin::registerUpdateChecker(UpdateChecker* updateItem)
{
    updateFactory_.appendChild(updateItem);
}

bool TablePlugin::checkDocbookChanges()
{
    String doctype = structEditor()->getDsi()->
                     getSafeProperty("category")->getString();
    return (0 <= doctype.find(NOTR("docbook"), 0, false));
}

int TablePlugin::currentTable()    
{ 
//    String doctype = structEditor()->getDsi()->
//                     getSafeProperty("category")->getString();
//    for (int i = 0; i < (int)tableFormatName_.size(); ++i) {
//        if (doctype == tableFormatName_[i])
//            return i;
//    }
    return 0; 
}

void TablePlugin::update_attrs()
{
    if (needAttrsUpdate_)
        return;
    IdleHandler::registerHandler(this);
}

bool TablePlugin::check_if_in(const String& name)
{
    Node* in_node = is_in(name, pendingPos_.node());
    if (0 == in_node)
        return false;
    in_node->registerNodeVisitor(this,
        NodeVisitor::NOTIFY_ATTRIBUTE_ADDED|
        NodeVisitor::NOTIFY_ATTRIBUTE_REMOVED|
        NodeVisitor::NOTIFY_ATTRIBUTE_CHANGED);
    return true;
}

bool TablePlugin::processQuanta()
{
    IdleHandler::deregisterHandler(this);
    deregisterFromAllNodes();
    Node* node = pendingPos_.node();
    if (checkDocbookChanges()) { //NOTE check for docbook type
        while (node && NOTR("table") != node->nodeName() &&
              NOTR("informaltable") != node->nodeName())
            node = parentNode(node);
        if (node)
            TableInfoSet_[NOTR("table")] = 
                new TablePlugin::TableInfo(node->nodeName(), 0);
    }
//*************************************************
    bool in_table = check_if_in(table_name(this));
    bool in_cell  = check_if_in(cell_name(this));
    bool in_row   = check_if_in(row_name(this));
    check_if_in(root_name(this));
    UpdateChecker* uc = updateFactory_.firstChild();
    for (; uc; uc = uc->nextSibling()) {
        switch (uc->type()) {
            case UpdateChecker::IN_TABLE:
                uc->prepareEnabled(in_table);
                break;
            case UpdateChecker::IN_CELL:
                uc->prepareEnabled(in_cell);
                break;
            case UpdateChecker::IN_ROW:
                uc->prepareEnabled(in_row);
                break;
            case UpdateChecker::UNKNOWN:
                uc->prepareEnabled(true);
                break;
            default:
                break;
        }
        try {
            uc->updatePos(pendingPos_);
        }
        catch (...) {
            String err = "Table Plugin Error ";
            if (uc->action())
                err += uc->action()->get(Sui::NAME);
            std::cerr << err << std::endl;
        }
    }
    return false;
}

StructEditor* TablePlugin::structEditor()
{
    return se_;
}

TablePlugin::TableClipboard& TablePlugin::clipboard()
{
    return clipboard_;
}


UpdateChecker::UpdateChecker(Sui::Action* action, const String& name)
    : action_(action), toBeEnabled_(false)
{
    if (IS_IN_TABLE == name)
        type_ = UpdateChecker::IN_TABLE;
    else if (IS_IN_CELL == name)
        type_ = UpdateChecker::IN_CELL;
    else if (IS_IN_ROW == name)
        type_ = UpdateChecker::IN_ROW;
    else
        type_ = UpdateChecker::UNKNOWN;

}

UpdateChecker::~UpdateChecker()
{
}

void UpdateChecker::prepareEnabled(bool enable)
{
    toBeEnabled_ = enable;
}

void UpdateChecker::updatePos(const GroveEditor::GrovePos& pos) 
{
    if (toBeEnabled_)
        update(pos);
    else
        setEnabled(toBeEnabled_);
}

void UpdateChecker::update(const GroveEditor::GrovePos&)
{
    setEnabled(toBeEnabled_);
}

void UpdateChecker::setEnabled(bool enable)
{
    action_->setEnabled(enable);
}

DEFINE_PLUGIN_CLASS(TablePlugin)

