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
#ifndef QT_LAYOUT_H_
#define QT_LAYOUT_H_

#include "ui/UiProps.h"
#include "ui/UiItem.h"
#include "ui/UiItems.h"

#include <QObject>
#include <QPointer>
#include <QGridLayout>

namespace Sui {

class QtLayout : public Item {
public:
    QtLayout(Action* action, PropertyNode* properties);
    virtual ~QtLayout() {};

    virtual String      itemClass() const { return LAYOUT; }
    virtual String      widgetClass() const { return SIMPLE_WIDGET; }

    virtual QWidget*    widget(const Item* child) const;
    QBoxLayout*         layout() const { return layout_; }

    QBoxLayout::Direction   direction() const;

protected:
    virtual bool        doAttach();
    virtual bool        doDetach();

    virtual void            setItemVisible(bool) {};
    virtual void            propertyChanged(PropertyNode* prop);

private:
    QPointer<QBoxLayout> layout_;
    QPointer<QWidget>    widget_;
};

//////////////////////////////////////////////////////////////////////////

class QtGridLayout : public Item {
public:
    QtGridLayout(Action* action, PropertyNode* properties);
    virtual ~QtGridLayout() {};

    virtual String      itemClass() const { return GRID_LAYOUT; }
    virtual String      widgetClass() const { return SIMPLE_WIDGET; }

    virtual QWidget*    widget(const Item* child) const;
    QGridLayout*        layout() const { return layout_; }

private:
    QPointer<QGridLayout>    layout_;
    QPointer<QWidget>        widget_;
};

class QtGridWidget : public Item {
public:
    QtGridWidget(Action* action, PropertyNode* properties);
    virtual ~QtGridWidget() {};

    virtual String      itemClass() const { return GRID_WIDGET; }
    virtual String      widgetClass() const { return SIMPLE_WIDGET; }
    virtual QWidget*    widget(const Item* child) const;

protected:
    virtual bool        doAttach();
    virtual bool        doDetach();

private:
    QPointer<QWidget>    widget_;
    QPointer<QGridLayout> gridLayout_;
};

//////////////////////////////////////////////////////////////////////////

class QtStretch : public Item {
public:
    QtStretch(Action* action, PropertyNode* properties);
    virtual ~QtStretch() {};

    virtual String      itemClass() const { return STRETCH; }
    virtual String      widgetClass() const { return TERMINAL_WIDGET; }

    virtual bool        doAttach();
    virtual QWidget*    widget(const Item* child) const;

private:
    int                 stretch_;
    QPointer<QWidget>   spacer_;
};

} // namespace Sui

#endif // QT_LAYOUT_H_
