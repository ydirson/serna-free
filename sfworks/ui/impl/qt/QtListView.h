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
#ifndef QT_LIST_VIEW_H_
#define QT_LIST_VIEW_H_

#include "ui/UiProps.h"
#include "ui/UiItem.h"
#include "ui/UiItems.h"

#include <QObject>
#include <QPointer>

class QTreeWidget;
class QTreeWidgetItem;

namespace Sui {

class QtListView : public QObject, public Item {
    Q_OBJECT
public:
    QtListView(Action* action, PropertyNode* properties);
    virtual ~QtListView() {};

    virtual String      itemClass() const { return LIST_VIEW; }
    virtual String      widgetClass() const { return TERMINAL_WIDGET; }
    virtual QWidget*    widget(const Item* child) const;

protected slots:
    virtual void        currentChanged(QTreeWidgetItem* item);

private:
    QPointer<QTreeWidget>  listView_;
};

///////////////////////////////////////////////////////////////////////////

class QtListViewItem : public Item {
public:
    QtListViewItem(Action* action, PropertyNode* properties);
    virtual ~QtListViewItem();

    virtual String      itemClass() const { return LIST_VIEW_ITEM; }
    virtual String      widgetClass() const { return TERMINAL_WIDGET; }

    virtual void        setItemVisible(bool isVisible);

    virtual QWidget*    widget(const Item*) const { return 0; }
    QTreeWidgetItem*      item() const;

protected:
    virtual bool        doAttach();
    virtual bool        doDetach();
    QTreeWidgetItem*      prevSiblingItem() const;
    void                setupItem();

private:
    QTreeWidgetItem*      item_;
};

} // namespace Sui

#endif // QT_LIST_VIEW_H_
