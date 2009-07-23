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
/*! \file
    UTILS definitions for foundation library
 */

#ifndef TABLE_PLUGIN_H
#define TABLE_PLUGIN_H

#include "docview/DocumentPlugin.h"
#include "common/OwnerPtr.h"
#include "utils/IdleHandler.h"
#include "common/PropertyTree.h"
#include "groveeditor/GrovePos.h"
#include "grove/Decls.h"
#include "grove/NodeVisitor.h"
#include <map>

class UiActionExecutor;
class StructEditor;
class UpdateChecker;

namespace Sui {
    class Action;
    class Item;
}

extern const char* const IS_IN_TABLE;
extern const char* const IS_IN_CELL;
extern const char* const IS_IN_ROW;
extern const char* const OUT_OF_TABLE;

class TablePlugin : public DocumentPlugin,
                    public IdleHandler,
                    public GroveLib::NodeVisitor {
public:

    void registerUpdateChecker(UpdateChecker* updateItem);

    class TableInfo {//note: used in table format support - CALS,HTML,DITA,TEI
    public:
        TableInfo(){}
        TableInfo(const Common::String& tname, int level)
        {
            name.push_back(tname);
            lev.push_back(level);
        }
        Common::Vector<Common::String>  name;
        Common::Vector<int>             lev;
    };

    class AttrInfo {//note: extra attributes defined in .tpp file 
    public:
        AttrInfo(){}
        AttrInfo(const Common::String& tname, const Common::String& val)
            :name(tname), value(val) {
        }
        Common::String  name;
        Common::String  value;
    };

    class TableInfoSet: public std::map<Common::String, TableInfo*> {};
    typedef Common::Vector<AttrInfo> AttrsContainer;
    class AttrsSet: public std::map<Common::String, AttrsContainer> {};

    virtual void  postInit();
    void          update(const GroveEditor::GrovePos& src);
    Sui::Item*     getItem(const Common::String& name);
    StructEditor* structEditor();
    bool          checkDocbookChanges();
    int           currentTable();
    TableInfoSet& infoSet()   { return TableInfoSet_;}
    AttrsSet&     attrsSet()  { return AttrsSet_;}
    bool          isAutoEntryPara() { return false; }

    class TableClipboard {  //note: used in copy&paste table columns
    public:
        TableClipboard(){}
        typedef Common::Vector<GroveLib::DocumentFragmentPtr> ClipContainer;
        ClipContainer       head;
        ClipContainer       body;
        ClipContainer       foot;
        GroveLib::NodePtr   spec;
    };
    TableClipboard& clipboard();

    TablePlugin(SernaApiBase* ssd, SernaApiBase* properties, char**);
    ~TablePlugin();
protected:
    //! Processes context change events from editor
    virtual bool    processQuanta();

    virtual void    nodeDestroyed(const GroveLib::Node*) {}
    virtual void    childInserted(const GroveLib::Node*) {}
    virtual void    childRemoved (const GroveLib::Node*,
                                  const GroveLib::Node*) {}

    virtual void    attributeChanged(const GroveLib::Attr*) { update_attrs(); }
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*) { update_attrs(); }
    virtual void    attributeAdded(const GroveLib::Attr*) { update_attrs(); }
    virtual void    textChanged(const GroveLib::Text*) {}

    void            update_attrs();
    bool            check_if_in(const Common::String&);

private:
    Common::XList<UpdateChecker>    updateFactory_;
    StructEditor*                   se_;
    GroveEditor::GrovePos           pendingPos_;
    TableInfoSet                    TableInfoSet_;
    AttrsSet                        AttrsSet_;
    Common::Vector<Common::String>  tableFormatName_;
    TableClipboard                  clipboard_;
    bool                            needAttrsUpdate_;
    Common::PropertyNodePtr         params_;
};

class UpdateChecker : public Common::XListItem<UpdateChecker> {
public:
    enum EnableWhen { IN_CELL, IN_ROW, IN_TABLE, UNKNOWN };
    UpdateChecker(Sui::Action* action, const Common::String& name);
    virtual ~UpdateChecker();

    virtual void update(const GroveEditor::GrovePos& src);
    void         updatePos(const GroveEditor::GrovePos& src);
    void         prepareEnabled(bool enable);
    void         setEnabled(bool enable);
    EnableWhen   type() { return type_; }
    Sui::Action*  action()  { return action_; }
private:
    Sui::Action*  action_;
    EnableWhen   type_;
    bool         toBeEnabled_;
};

#define SIMPLE_EXECUTOR_IMPL(ExecutorName, PluginType, UpdateName) \
class ExecutorName :  public PluginUiExecutor<PluginType>, UpdateChecker { \
public:                                           \
    ExecutorName(Sui::Action* action, DocumentPlugin* plugin) \
        : PluginUiExecutor<PluginType>(action, plugin),\
          UpdateChecker(action, UpdateName) \
     { PluginUiExecutor<PluginType>::plugin()->registerUpdateChecker(this); } \
    virtual void execute(); \
    virtual void update(const GroveEditor::GrovePos& src); \
}; \
template <> ActionExecutor* \
makeUiPluginExecutor<ExecutorName>(Sui::Action* action, DocumentPlugin* dp) \
    { return new ExecutorName(action, dp); }

#endif //TABLE_PLUGIN_H
