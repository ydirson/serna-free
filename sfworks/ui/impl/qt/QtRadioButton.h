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
#ifndef QT_RADIO_BUTTON_H_
#define QT_RADIO_BUTTON_H_

#include "ui/UiProps.h"
#include "ui/UiItem.h"
#include "ui/UiItems.h"

#include <QObject>
#include <QPointer>

class QRadioButton;
class QButtonGroup;

namespace Sui {

class QtRadioButton : public QObject, public Item {
    Q_OBJECT
public:
    QtRadioButton(Action* action, PropertyNode* properties);
    virtual ~QtRadioButton() {};

    virtual String      itemClass() const { return RADIO_BUTTON; }
    virtual String      widgetClass() const { return TERMINAL_WIDGET; }

    virtual void        setToggled(bool isOn);

    virtual QWidget*    widget(const Item* child) const;

public slots:
    void                clicked();
    void                toggled(bool);

protected:
    virtual bool        doAttach();
    virtual bool        doDetach();

    void                makeButton();
    virtual void        propertyChanged(PropertyNode* prop);

protected:
    QPointer<QRadioButton>  button_;
    QPointer<QButtonGroup>  buttonGroup_;
    bool                    noDispatch_;
};

} // namespace Sui

#endif // QT_RADIO_BUTTON_H_
