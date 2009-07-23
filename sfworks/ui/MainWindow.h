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
#ifndef UI_MAIN_WINDOW_H_
#define UI_MAIN_WINDOW_H_

#include "ui/ui_defs.h"
#include "ui/UiProps.h"
#include "ui/UiItems.h"
#include "ui/UiStackItem.h"

namespace Sui {

/*
 */
class  UI_EXPIMP RootItem : public Item {
public:
    RootItem();
    virtual ~RootItem() {}

    virtual String      itemClass() const { return ROOT_ITEM; }
    virtual String      widgetClass() const { return String(); }

    virtual bool        doAttach() { return true; }
    virtual bool        doDetach() { return true; }
    virtual void        setItemVisible(bool) {};

    virtual void        childInserted(Item* item);
    
    virtual RootItem*   rootItem() { return this; }
    virtual bool        mustCloseOnEscape() const { return false; }
};

/*
 */
class Document;

class  UI_EXPIMP MainWindow : public StackItem {
public:
    virtual String      itemClass() const { return MAIN_WINDOW; }
    virtual String      widgetClass() const { return MAIN_WINDOW_WIDGET; }

    static MainWindow*  make(int x = 0, int y = 0,
                             int width = 0, int height = 0,
                             const String& name = String(),
                             bool isBottomTabs = true, QWidget* parent = 0);
    virtual ~MainWindow() {}

    virtual void        setIcon(const String&) = 0;
    virtual void        setCaption(const String&) = 0;

    static const char   CLOSE_CMD_NAME[];

protected:
    virtual void        dispatch(Action* action);

protected:
    MainWindow(Action* action, PropertyNode* properties);
};

} // namespace Sui

#endif // UI_MAIN_WINDOW_H_
