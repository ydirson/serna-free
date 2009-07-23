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
#ifndef STRUCTEDITOR_INSERT_ELEMENT_ITEM_H_
#define STRUCTEDITOR_INSERT_ELEMENT_ITEM_H_

#include "structeditor/se_defs.h"
#include "common/CommandEvent.h"
#include "common/OwnerPtr.h"
#include "common/PropertyTree.h"
#include "groveeditor/GrovePos.h"

#include "ui/LiquidItem.h"
#include "utils/SernaUiItems.h"
#include "utils/IdleHandler.h"

class StructEditor;
class InsertElementUtils;
class InsertElementItem;

namespace InsertElementProps
{
    extern const char* const ELEMENT_LIST;
    extern const char* const OTHER_ELEMENTS;
    extern const char* const RECENT_ELEMENTS;
    extern const char* const CAPTION;
}

class InsertElementNotifications {
public:
    InsertElementNotifications(InsertElementItem* item)
        : item_(item) {}
    virtual ~InsertElementNotifications(); 
    
    virtual void    updateSelectionState(bool sel, bool hasCdata) = 0;
    virtual void    setElementList(Common::PropertyNode*) = 0;
    virtual void    updateElementList() = 0;

protected:
    InsertElementItem* item_;
};

////////////////////////////////////////////////////////////////////////

class InsertElementItem : public Common::CommandEventContext,
                          public Sui::LiquidItem,
                          public IdleHandler {
public:
    InsertElementItem(Sui::Action*, PropertyNode* uiProps);
    virtual ~InsertElementItem();

    virtual String      itemClass() const { return Sui::INSERT_ELEMENT; }

    void                setEditorFocus();
    void                doInsert(const Common::String& elemName);

    void                notifyPositionChange(const GroveEditor::GrovePos&);
    void                setSelectionChanged(bool);
    StructEditor*       structEditor() const;

    void                updateElementList();

protected:
    virtual void        inserted();
    virtual void        removed();
    QWidget*            makeWidget(QWidget* parent, Type type);

    virtual const char* selfTypeId() const
    {
        return typeid(InsertElementItem).name();
    }
private:
    virtual bool        processQuanta(); // reimplemented from IdleHandler
    void                init(InsertElementNotifications*);
    friend class        InsertElementNotifications;

private:
    GroveEditor::GrovePos                   pendingPos_;
    Common::OwnerPtr<InsertElementUtils>    utils_;
    InsertElementNotifications*             myWidget_;
    char                                    selectionState_;
    bool                                    hasCdata_;
};

////////////////////////////////////////////////////////////////////////

class InsertElementUtils {
public:
    InsertElementUtils(StructEditor* se, bool isRename);
    virtual ~InsertElementUtils();

    bool                    doInsert(const Common::String& name);
    void                    loadElementList(Common::PropertyNode* loadTo,
                                            const GroveEditor::GrovePos& pos);
    StructEditor*           structEditor() const { return structEditor_; }

private:
    StructEditor*           structEditor_;
    bool                    rename_;
};

#endif // STRUCTEDITOR_INSERT_ELEMENT_H_
