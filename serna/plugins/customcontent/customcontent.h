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
#ifndef CUSTOM_CONTENT_H_
#define CUSTOM_CONTENT_H_

#include "ui/UiItem.h"
#include "utils/IdleHandler.h"

#include "common/Exception.h"
#include "common/PropertyTree.h"

#include "docview/dv_utils.h"
#include "docview/DocumentPlugin.h"

#include "groveeditor/GrovePos.h"
#include "CustomContentDialog.h"

#include <list>

namespace ContentProps
{

extern const char* const SCC_ROOT;
extern const char* const CONTENT_ITEM;
extern const char* const NAME;
extern const char* const INSCRIPTION;
extern const char* const ICON;
extern const char* const HOT_KEY;

extern const char* const CONTEXT_SPECS;
extern const char* const CONTENT;
extern const char* const PATTERN_STR;
extern const char* const XPATH_LOCATION;
extern const char* const REL_LOCATION;
extern const char* const TEXT;
extern const char* const MAKE_TOOL_BUTTON;

extern const char* const INSERT_BEFORE;
extern const char* const INSERT_AFTER;
extern const char* const APPEND_TO;

}

enum RelativeLocation {
    INSERT_AFTER_ = 0,
    INSERT_BEFORE_,
    APPEND_TO_,
    CURRENT_POS_
};

extern RelativeLocation location(const Common::String& str);
extern const char* location_string(RelativeLocation l);
extern const Common::String location_inscription(RelativeLocation l);

namespace Formatter
{
    class TreeSelection;
}

class StructEditor;

typedef std::list<Sui::ActionPtr>   ActionList;
typedef ActionList::const_iterator ActionIterator;

class CustomContentPlugin : public DocumentPlugin,
                            public IdleHandler,
                            public CustomNameChecker {
public:
    CustomContentPlugin(SernaApiBase* doc, SernaApiBase* properties, char**);

    virtual void            buildPluginInterface();
    virtual void            postInit();

    StructEditor*           structEditor() const { return se_; }
    virtual bool            isValidName(const Common::String& name) const;
    virtual Common::String  getValidName(const Common::String& name) const;
    const ActionList&       actionList() { return actionList_; }
    void                    makeContentItems(Sui::Item* toolbar,
                                            Sui::Item* menu,
                                            const Sui::Action* action);
    void                    makeContentItems(const Sui::Action* action);
    //! Creates action and its item(s)
    Sui::Action*             addContent(Common::PropertyNode* content);
    void                    deleteContent(Sui::Action* action);

    virtual void            saveProperties(const Common::String& path);

    // used internally
    Common::String          substitute_file_mark(const String& str,
                                                 const String& docPath);
    void                    substitute_marks(GroveLib::Node* node, 
                                             const Common::String& docPath,
                                             const GroveEditor::GrovePos& pos);
protected:
    //! Processes context change events from editor
    virtual bool            processQuanta();

private:
    ActionList              actionList_;
    Sui::ItemPtr             toolSeparator_;
    Sui::ItemPtr             menuSeparator_;
    GroveEditor::GrovePos   pendingPos_;
    StructEditor*           se_;
    Common::PropertyNodePtr sccRoot_;
};

////////////////////////////////////////////////////////////////////////////

class CustomContentPropMaker : public PropertyMaker {
public:
    CustomContentPropMaker(const ActionList& actionList)
        : actionList_(actionList) {}
    virtual Common::PropertyNode*   makeProperty();
private:
    const ActionList&               actionList_;
};

////////////////////////////////////////////////////////////////////////////

class CustomContentException : public COMMON_NS::Exception {
public:
    CustomContentException(const COMMON_NS::String& s)
        : COMMON_NS::Exception(s) {}
    CustomContentException(const char *s)
        : COMMON_NS::Exception(s) {}
};

#endif // CUSTOM_CONTENT_H_
