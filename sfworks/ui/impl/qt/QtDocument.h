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
#ifndef UI_QT_DOCUMENT_H_
#define UI_QT_DOCUMENT_H_

#include "ui/ui_defs.h"
#include "ui/UiProps.h"
#include "ui/UiItems.h"
#include "ui/UiStackItem.h"
#include "ui/UiDocument.h"

#include <QPointer>
#include <QTabBar>
#include <QMainWindow>
#include <QShortcut>

namespace Sui {

class QtDocument : public QObject,
                   public StackWidget,
                   public Common::PropertyNodeWatcher {
    Q_OBJECT
public:
    QtDocument(Document*);
    ~QtDocument();

    virtual void        insertItem(const Item* item);
    virtual void        removeItem(const Item* item);

    virtual void        setCurrent(Item* item);
    virtual void        setItemVisible(bool isVisible);

    virtual QWidget*    widget(const Item* item) const;
    virtual StackWidget* nextStackWidget() const;

    void                saveDockingState();
    void                restoreDockingState();
    void                setGlobalAccelerators();

public slots:
    void                tabSelected(int);
    void                showTabContextMenu(const QPoint& pos);

private:
    void                ensureToolbarStates(QString&);
    void                updateTabBar(const Item*);
    virtual void        propertyChanged(Common::PropertyNode* property);
    virtual void        updateProperty(const Item* item,
                                       const Common::PropertyNode* property);
    void                add_propwatcher(const Common::String& propname);

    QPointer<QMainWindow>    mainWindow_;
    QPointer<QTabBar>        tabBar_;
    QPointer<QWidget>        widget_;
    QPointer<QWidget>        centralWidget_;
    Document*                   document_;
};

}

#endif // UI_QT_DOCUMENT_H_
