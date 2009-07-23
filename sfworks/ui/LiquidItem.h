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
#ifndef UI_LIQUID_ITEM_H_
#define UI_LIQUID_ITEM_H_

#include "ui/ui_defs.h"
#include "ui/UiProps.h"
#include "ui/UiItem.h"

#include "common/CommandEvent.h"
#include "common/PropertyTree.h"

#include <QObject>
#include <QPointer>

class QWidget;
class QMainWindow;
class QDockWidget;

namespace Sui 
{
    
extern const char* const DOCK_EDGE;
extern const char* const DOCK_TOP;
extern const char* const DOCK_BOTTOM;
extern const char* const DOCK_LEFT;
extern const char* const DOCK_RIGHT;
extern const char* const DOCK_UNDOCKED;

///////////////////////////////////////////////////////////////////////////

class DockActionMap;
class Document;

class UI_EXPIMP LiquidItem : public Item  {
public:
    typedef Item* (*restore_func)(Document*,
                    const Common::PropertyNode*, Item*);
    enum Type { WIDGET          = 0x01,
                UNDOCKED_TOOL   = 0x02,
                VERTICAL_TOOL   = 0x04,
                HORIZONTAL_TOOL = 0x08 };

    LiquidItem(PropertyNode* properties);
    virtual ~LiquidItem();

    //! Reimplemented from UiItem
    virtual String      widgetClass() const { return TERMINAL_WIDGET; }
    virtual QWidget*    widget(const Item* child) const;

    // builds generic context menu for liquid items
    void                buildChildren(restore_func);

    virtual void        setItemVisible(bool isVisible);
    virtual void        showContextMenu(const QPoint& pos);

    QMainWindow*        mainWindow() const;
    
    virtual void        escapePressed();
    virtual void        widgetFocusChanged(bool);
    
    virtual void        grabFocus() const;
    
    // notifies implementation about visibility change
    virtual void        visibilityChanged(bool) {}
    
protected:
    //! Reimplemented from UiItem
    virtual bool        doAttach();
    virtual bool        doDetach();

    virtual void        dispatch(Action* action);
    virtual QWidget*    makeWidget(QWidget* parent, Type type) = 0;
    virtual void        propertyChanged(PropertyNode* prop);

private:
    friend class LiquidItemEventFilter;
    friend class QtMainWindow;      // for preserveVisibleState

    void                floatingChanged(bool);
    void                dockLocationChanged(Qt::DockWidgetArea);
    void                changeVisibilityState(bool isVisible);
    void                remakeWidget();
    void                preserveVisibleState(bool switch_to);
    
    QPointer<QWidget>               widget_;
    QPointer<QDockWidget>           dockWidget_;
    QPointer<QObject>               eventFilter_;
    Common::OwnerPtr<DockActionMap> dockActionMap_;
};

UI_EXPIMP void disable_widget(QWidget*);
UI_EXPIMP void set_widget_focus_color(QWidget*, bool);

}

#endif // IO_LIQUID_ITEM_H_
