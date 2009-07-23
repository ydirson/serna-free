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
#ifndef UI_QT_TAB_WIDGET_H_
#define UI_QT_TAB_WIDGET_H_

#include "ui/ui_defs.h"
#include "ui/UiProps.h"
#include "ui/UiItems.h"
#include "ui/UiStackItem.h"

#include <QTabWidget>
#include <QTabBar>
#include <QToolBox>
#include <QPointer>
#include <QEvent>

#include <map>

namespace Sui {

class QtToolBox : public StackItem {
public:
    QtToolBox(PropertyNode* properties);
    virtual ~QtToolBox();

    virtual String      itemClass() const { return TOOL_BOX; }
    virtual String      widgetClass() const { return SIMPLE_WIDGET; }
    virtual StackWidget* stackWidget() const { return stackWidget_; }

protected:
    StackWidget* stackWidget_;
};

////////////////////////////////////////////////////////////////////////

class QtToolWidget : public QToolBox, public StackWidget, 
                     protected Common::PropertyNodeWatcher {
    Q_OBJECT
public:
    QtToolWidget(StackItem*);

    virtual void        insertItem(const Item* item);
    virtual void        removeItem(const Item* item);
    virtual void        setCurrent(Item* item);
    virtual void        setItemVisible(bool isVisible)
        { setVisible(isVisible); }
    virtual QWidget*    widget(const Item* item) const;

public slots:
    void                currentChanged(int idx);

private:
    Item*               itemAt(QWidget* page) const;
    //! Property modifications
    virtual void        propertyChanged(Common::PropertyNode* property);

private:
    typedef std::map<const Item*, QWidget*> ItemMap;
    ItemMap             itemMap_;
};

////////////////////////////////////////////////////////////////////////

class QtTabItem : public StackItem {
public:
    QtTabItem(PropertyNode* properties);
    virtual ~QtTabItem();

    virtual String          itemClass() const { return TAB_ITEM; }
    virtual String          widgetClass() const { return SIMPLE_WIDGET; }
    virtual StackWidget*    stackWidget() const { return stackWidget_; }

protected:
    StackWidget*        stackWidget_;
};

////////////////////////////////////////////////////////////////////////

class QtTabWidget : public QTabWidget, 
                    public StackWidget, 
                    protected Common::PropertyNodeWatcher {
    Q_OBJECT
public:
    QtTabWidget(StackItem*, Common::PropertyNode* properties);

    virtual void        insertItem(const Item* item);
    virtual void        removeItem(const Item* item);
    virtual void        setCurrent(Item* item);
    virtual void        setItemVisible(bool isVisible) 
        { setVisible(isVisible); }
    virtual QWidget*    widget(const Item* item) const;

public slots:
    void                currentChanged(int idx);
    void                showTabContextMenu(const QPoint& pos);

protected:
    virtual void        tabRemoved(int);
    virtual void        propertyChanged(Common::PropertyNode* property);
    virtual void        itemPropertyChanged(Common::PropertyNode* property);

private:
    Item*               itemAt(int idx) const;
};

class QTabContextMenuProvider : public QObject {
    Q_OBJECT
public:
    QTabContextMenuProvider(QTabBar* tabBar, QObject* parent);

signals:
    void                showTabContextMenu(const QPoint&);

private:
    virtual bool        eventFilter(QObject* receiver, QEvent* event);
    QPointer<QTabBar>   tabBar_;
};

} // namespace Sui

#endif // UI_QT_TAB_WIDGET_H


