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
#ifndef UI_PROPERTY_SYNCHER_H
#define UI_PROPERTY_SYNCHER_H

#include "ui/ui_defs.h"
#include "common/OwnerPtr.h"
#include "common/PropertyTree.h"
#include "common/Vector.h"

#include <QObject>

class QLineEdit;
class QTextEdit;
class QAbstractButton;
class QButtonGroup;
class QLabel;
class QWidget;
class QSpinBox;
class QGroupBox;

namespace Sui {

class UI_EXPIMP PropertySyncher : public QObject,
                                  public Common::RefCounted<>,
                                  protected Common::PropertyNodeWatcher {
    Q_OBJECT
public:
    PropertySyncher(Common::PropertyNode* property);
    virtual ~PropertySyncher();

    static PropertySyncher* make(Common::PropertyNode* property,
                                 QWidget* widget);

    void                    setProperty(Common::PropertyNode* property);
    virtual void            syncProperty(Common::PropertyNode* property) = 0;

signals:
    void                    propChanged(Common::PropertyNode*);

protected:
    PropertySyncher(const PropertySyncher&);
    PropertySyncher& operator=(const PropertySyncher&);

    virtual void    propertyChanged(Common::PropertyNode* property);

    Common::PropertyNode*   property_;

};

typedef Common::RefCntPtr<PropertySyncher> PropertySyncherPtr;

////////////////////////////////////////////////////////////////////////

class UI_EXPIMP PropertySyncherHolder : public Common::PropertyTree {
public:
    PropertySyncherHolder(Common::PropertyNode* rootProp = 0)
        : PropertyTree(rootProp) {}
    virtual ~PropertySyncherHolder();

    void                    addSyncher(const Common::String& property,
                                       QWidget* widget);
    void                    addSyncher(PropertySyncher* syncher);

private:
    PropertySyncherHolder(const PropertySyncherHolder&);
    PropertySyncherHolder& operator=(const PropertySyncherHolder&);

    Common::Vector<PropertySyncherPtr> synchers_;
};

////////////////////////////////////////////////////////////////////////


class UI_EXPIMP LineEditSyncher : public PropertySyncher {
    Q_OBJECT
public:
    LineEditSyncher(Common::PropertyNode* property, QLineEdit* lineEdit);
    virtual ~LineEditSyncher();

protected:
    virtual void    syncProperty(Common::PropertyNode* property);

protected slots:
    void            textChanged();

private:
    LineEditSyncher(const LineEditSyncher&);
    LineEditSyncher& operator=(const LineEditSyncher&);

    QLineEdit*      lineEdit_;
};

////////////////////////////////////////////////////////////////////////

class UI_EXPIMP TextEditSyncher : public PropertySyncher {
    Q_OBJECT
public:
    TextEditSyncher(Common::PropertyNode* property, QTextEdit* textEdit);
    virtual ~TextEditSyncher();

protected:
    virtual void    syncProperty(Common::PropertyNode* property);

protected slots:
    void            textChanged();

private:
    TextEditSyncher(const TextEditSyncher&);
    TextEditSyncher& operator=(const TextEditSyncher&);

    QTextEdit*      textEdit_;
};

////////////////////////////////////////////////////////////////////////


class UI_EXPIMP ButtonSyncher : public PropertySyncher {
    Q_OBJECT
public:
    ButtonSyncher(Common::PropertyNode* property, QAbstractButton* button);
    virtual ~ButtonSyncher();

protected:
    virtual void    syncProperty(Common::PropertyNode* property);

protected slots:
    void            toggled(bool on);

private:
    ButtonSyncher(const ButtonSyncher&);
    ButtonSyncher& operator=(const ButtonSyncher&);

    QAbstractButton* button_;
};

////////////////////////////////////////////////////////////////////////

class UI_EXPIMP GroupBoxSyncher : public PropertySyncher {
    Q_OBJECT
public:
    GroupBoxSyncher(Common::PropertyNode* property, QGroupBox*);
    virtual ~GroupBoxSyncher();

protected:
    virtual void    syncProperty(Common::PropertyNode* property);

protected slots:
    void            toggled(bool on);

private:
    GroupBoxSyncher(const GroupBoxSyncher&);
    GroupBoxSyncher& operator=(const GroupBoxSyncher&);

    QGroupBox* groupBox_;
};

////////////////////////////////////////////////////////////////////////


class UI_EXPIMP SelectorSyncher : public PropertySyncher {
    Q_OBJECT
public:
    SelectorSyncher(Common::PropertyNode* property);
    virtual ~SelectorSyncher();

    void            addSelector(const Common::String& value,
                                QAbstractButton* button);
protected:
    virtual void    syncProperty(Common::PropertyNode* property);

protected slots:
    void            toggled(bool on);

private:
    SelectorSyncher(const SelectorSyncher&);
    SelectorSyncher& operator=(const SelectorSyncher&);

    class SelectorMap;
    Common::OwnerPtr<SelectorMap>   selectorMap_;
    QButtonGroup*                   buttonGroup_;
};

////////////////////////////////////////////////////////////////////////


class UI_EXPIMP PixmapSyncher : public PropertySyncher {
    Q_OBJECT
public:
    PixmapSyncher(Common::PropertyNode* property, QLabel* label);
    PixmapSyncher(Common::PropertyNode* property, QLabel* label,
                  const Common::String& truePixmap,
                  const Common::String& trueToolTip,
                  const Common::String& falsePixmap,
                  const Common::String& falseToolTip);
    virtual ~PixmapSyncher();

    void    addState(const Common::String& value,
                     const Common::String& pixmap,
                     const Common::String& tooltip = Common::String());
    void    update(); // must be called after adding all states

    void    enableState(const Common::String& mode, bool);

protected:
    virtual void    syncProperty(Common::PropertyNode* property);
    virtual bool    eventFilter(QObject* watched, QEvent* e);

private:
    PixmapSyncher(const PixmapSyncher&);
    PixmapSyncher& operator=(const PixmapSyncher&);

    struct State {
        Common::String value_;
        Common::String pixmap_;
        Common::String tooltip_;
        bool           enabled_;
    };
    void    shiftState();
    Common::Vector<State>   states_;
    QLabel*                 label_;
    uint                    currentState_;
};

////////////////////////////////////////////////////////////////////////


class UI_EXPIMP ColorSyncher : public PropertySyncher {
public:
    ColorSyncher(Common::PropertyNode* property, QWidget* lineEdit);
    virtual ~ColorSyncher();

protected:
    virtual void    syncProperty(Common::PropertyNode* property);

private:
    ColorSyncher(const ColorSyncher&);
    ColorSyncher& operator=(const ColorSyncher&);

    QWidget*        widget_;
};

////////////////////////////////////////////////////////////////////////

class UI_EXPIMP SpinnerSyncher : public PropertySyncher {
    Q_OBJECT
public:
    SpinnerSyncher(Common::PropertyNode* property, QSpinBox* spinner);
    virtual ~SpinnerSyncher();

protected:
    virtual void    syncProperty(Common::PropertyNode* property);

protected slots:
    void            valueChanged(int value);
    virtual void    textChanged(const QString&);

private:
    SpinnerSyncher(const SpinnerSyncher&);
    SpinnerSyncher& operator=(const SpinnerSyncher&);

    QSpinBox*       spinner_;
};

} // namespace Sui

#endif  // UI_PROPERTY_SYNCHER_H
