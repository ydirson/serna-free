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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#ifndef QT_CONTENT_MAP_H_
#define QT_CONTENT_MAP_H_

#include "csl/Engine.h"
#include "csl/Instance.h"
#include "ContentMap.h"

#include "common/String.h"
#include "grove/Node.h"
#include "groveeditor/GrovePos.h"

#include <QFont>
#include <QTimer>
#include <q3listview.h>
#include <QPixmap>

#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QDragEnterEvent>

///////////////////////////////////////////////////////////////////////////

class QToolTip;
class ContentMapListItem;
class ProfileMap;
class ContentMapWidget;
class QToolButton;
class QTabBar;

namespace Sui {
    class MimeHandler;
}

class ProfileSelector : public QWidget {
    Q_OBJECT
public:
    ProfileSelector(QWidget* parent, ContentMapWidget* contentMapWidget, 
                    const Csl::Stylesheet* stylesheet);
    virtual ~ProfileSelector();

    void            languageChanged();
    Common::String  currentProfile() const;

protected slots:
    void    selectProfile(int id);
    void    setStrongFocus(bool);
    
signals:
    void    profileSelected(const Common::String& profileName);
    
protected:
    const Csl::Stylesheet*          stylesheet_;
    Common::OwnerPtr<ProfileMap>    profileMap_;
    ContentMap*                     contentMap_;
    Sui::Action*                     policyAction_;
    QToolButton*                    focusPolicyButton_;
    QTabBar*                        tabBar_;
};

///////////////////////////////////////////////////////////////////////////

class ContentMapWidget : public Q3ListView, 
                         public Csl::InstanceWatcher {
    Q_OBJECT
public:
    typedef GroveEditor::GrovePos GrovePos;
    
    ContentMapWidget(QWidget* parent, ContentMap* contentMap);
    virtual ~ContentMapWidget();

    void                    ensureSelectionVisible(const Q3ListViewItem* from,
                                                   const Q3ListViewItem* to,
                                                   const Q3ListViewItem* curr);
    void                    setCurrentItem(const GrovePos& src);
    void                    showSelection(const GrovePos& from,
                                          const GrovePos& to);
    Common::String          contextMenuAt(const QPoint& pos);

    virtual void            notifyInstanceInserted(Csl::Instance*);
    virtual void            notifyInstanceRemoved(Csl::Instance* parent,
                                                  Csl::Instance* child);
    void                    removeListItems(Csl::Instance* instance);
    ContentMap*             contentMap() const { return contentMap_; }
    StructEditor*           structEditor() const;
    Sui::MimeHandler&       mimeHandler() const;

protected:
    //! reimplemented to catch system font change
    virtual bool            event(QEvent* e);
    void                    ensureItemVisible2(const Q3ListViewItem* item);

    Q3ListViewItem*          listViewItem(const GrovePos& srcPos);
    void                    setHighlighted(ContentMapListItem* item);
    void                    extendSelectionTo(const GrovePos& pos);

    virtual void            contentsMouseMoveEvent(QMouseEvent* e);
    virtual void            contentsMousePressEvent(QMouseEvent* e);
    bool                    eventFilter(QObject* o, QEvent* e);
    void                    keyPressEvent(QKeyEvent* e);

    void                    contentsDragEnterEvent(QDragEnterEvent* event);
    void                    contentsDragLeaveEvent(QDragLeaveEvent* event);
    void                    contentsDragMoveEvent(QDragMoveEvent* e);
    void                    contentsDropEvent(QDropEvent* event);
    void                    startDrag(ContentMapListItem* item);
    void                    focusStateChanged(bool v);
    
public slots:
    void                    selectProfile(const Common::String& profile);
    void                    selectItem(Q3ListViewItem* item);
    virtual void            grabFocus() {}

private slots:
    void                    itemClicked(Q3ListViewItem* item);
    void                    itemSelected(Q3ListViewItem* item);
    void                    sendSrcPos(Q3ListViewItem* item);
    void                    openItem();
    
private:
    ContentMap*             contentMap_;
    ContentMapListItem*     highlighted_;
    ContentMapListItem*     selectedParent_;
    QObject*                tipManager_;
    QToolTip*               toolTip_;

    Qt::ButtonState         buttonState_;
    Q3ListViewItem*          prevItem_;
    
    int                     dropSerialNumber_;
    bool                    focusState_;
    bool                    isStartDrag_;
    bool                    pendingContextMenu_;
    QPoint                  mousePressPos_;
    QTimer                  autoOpenTimer_;
};

///////////////////////////////////////////////////////////////////////////

Csl::Instance* make_qt_csl_instance(const Csl::InstanceInit& data);

#endif // QT_CONTENT_MAP_H_
