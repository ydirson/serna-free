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
#include "ui/UiPropertySyncher.h"
#include "ui/IconProvider.h"
#include "ui/impl/ui_debug.h"
#include "common/StringTokenizer.h"

#include <map>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QToolTip>
#include <QMouseEvent>
#include <QEvent>
#include <QButtonGroup>
#include <QTextEdit>
#include <QGroupBox>

using namespace Common;

//////////////////////////////////////////////////////////////////////

namespace Sui {

PropertySyncher::PropertySyncher(PropertyNode* property)
    : property_(property)
{
    property_->addWatcher(this);
}

void PropertySyncher::setProperty(PropertyNode* property)
{
    if (!property)
        return;
    property_ = property;
    propertyChanged(property_);
}

PropertySyncher* PropertySyncher::make(PropertyNode* property, QWidget* widget)
{
    if (widget->inherits("QLineEdit"))
        return new LineEditSyncher(property, static_cast<QLineEdit*>(widget));
    if (widget->inherits("QAbstractButton"))
        return new ButtonSyncher(property,
            static_cast<QAbstractButton*>(widget));
    DBG(UI.TEST)
        << "no PropertySyncher for " << widget->className() << std::endl;
    return 0;
}

void PropertySyncher::propertyChanged(Common::PropertyNode* property)
{
    syncProperty(property);
    emit propChanged(property);
}

PropertySyncher::~PropertySyncher()
{
}

//////////////////////////////////////////////////////////////////////

void PropertySyncherHolder::addSyncher(const String& propName, QWidget* widget)
{
    if (0 == root())
        return;
    PropertyNode* property = root()->makeDescendant(propName);
    PropertySyncher* syncher = PropertySyncher::make(property, widget);
    if (syncher)
        addSyncher(syncher);
}

void PropertySyncherHolder::addSyncher(PropertySyncher* syncher)
{
    synchers_.push_back(syncher);
}

PropertySyncherHolder::~PropertySyncherHolder()
{
}

//////////////////////////////////////////////////////////////////////

LineEditSyncher::LineEditSyncher(PropertyNode* property, QLineEdit* lineEdit)
    : PropertySyncher(property),
      lineEdit_(lineEdit)
{
    propertyChanged(property_);
    connect(lineEdit_, SIGNAL(textChanged(const QString&)),
            this, SLOT(textChanged()));
}

void LineEditSyncher::textChanged()
{
    if (property_->getString() != lineEdit_->text())
        property_->setString(lineEdit_->text());
}

void LineEditSyncher::syncProperty(PropertyNode*)
{
    if (lineEdit_->text() != property_->getString())
        lineEdit_->setText(property_->getString());
}

LineEditSyncher::~LineEditSyncher()
{
}

//////////////////////////////////////////////////////////////////////

TextEditSyncher::TextEditSyncher(PropertyNode* property, QTextEdit* textEdit)
    : PropertySyncher(property),
      textEdit_(textEdit)
{
    propertyChanged(property_);
    connect(textEdit_, SIGNAL(textChanged()),
            this, SLOT(textChanged()));
}

void TextEditSyncher::textChanged()
{
    if (property_->getString() != textEdit_->text())
        property_->setString(textEdit_->text());
}

void TextEditSyncher::syncProperty(PropertyNode*)
{
    if (textEdit_->text() != property_->getString())
        textEdit_->setText(property_->getString());
}

TextEditSyncher::~TextEditSyncher()
{
}

//////////////////////////////////////////////////////////////////////

ButtonSyncher::ButtonSyncher(PropertyNode* property, QAbstractButton* button)
    : PropertySyncher(property),
      button_(button)
{
    propertyChanged(property_);
    connect(button_, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
}

void ButtonSyncher::toggled(bool)
{
    if (property_->getBool() != button_->isOn())
        property_->setBool(button_->isOn());
}

void ButtonSyncher::syncProperty(PropertyNode*)
{
    if (button_->isOn() != property_->getBool())
        button_->toggle();
}

ButtonSyncher::~ButtonSyncher()
{
}

//////////////////////////////////////////////////////////////////////

GroupBoxSyncher::GroupBoxSyncher(PropertyNode* property, QGroupBox* gbox)
    : PropertySyncher(property),
      groupBox_(gbox)
{
    propertyChanged(property_);
    connect(groupBox_, SIGNAL(toggled(bool)),
        this, SLOT(toggled(bool)));
}

void GroupBoxSyncher::toggled(bool)
{
    if (property_->getBool() != groupBox_->isChecked())
        property_->setBool(groupBox_->isChecked());
}

void GroupBoxSyncher::syncProperty(PropertyNode*)
{
    if (groupBox_->isChecked() != property_->getBool())
        groupBox_->setChecked(property_->getBool());
}

GroupBoxSyncher::~GroupBoxSyncher()
{
}

//////////////////////////////////////////////////////////////////////

class SelectorSyncher::SelectorMap :
    public std::map<String, QAbstractButton*> {};

SelectorSyncher::SelectorSyncher(PropertyNode* property)
    : PropertySyncher(property),
      selectorMap_(new SelectorMap),
      buttonGroup_(new QButtonGroup(this))
{
    buttonGroup_->setExclusive(true);
}

void SelectorSyncher::addSelector(const String& value, QAbstractButton* button)
{
    if (selectorMap_->end() == selectorMap_->find(value)) {
        (*selectorMap_)[value] = button;
        buttonGroup_->addButton(button);
        if (button->isOn() != (property_->getString() == value))
            button->toggle();
        connect(button, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
    }
}

void SelectorSyncher::toggled(bool on)
{
    if (!on)
        return;
    for (SelectorMap::const_iterator i = selectorMap_->begin();
         i != selectorMap_->end(); i++) {
        if ((*i).second->isOn()) {
            if (property_->getString() != (*i).first)
                property_->setString((*i).first);
            break;
        }
    }
}

void SelectorSyncher::syncProperty(PropertyNode*)
{
    SelectorMap::const_iterator i = selectorMap_->find(property_->getString());
    if (i != selectorMap_->end() && !(*i).second->isOn())
        (*i).second->toggle();
}

SelectorSyncher::~SelectorSyncher()
{
}

//////////////////////////////////////////////////////////////////////

PixmapSyncher::PixmapSyncher(Common::PropertyNode* property,
                             QLabel* label,
                             const Common::String& truePixmap,
                             const Common::String& trueToolTip,
                             const Common::String& falsePixmap,
                             const Common::String& falseToolTip)
    : PropertySyncher(property),
      label_(label),
      currentState_(0)
{
    label_->installEventFilter(this);
    addState("false", falsePixmap, falseToolTip);
    addState("true",  truePixmap,  trueToolTip);
    update();
}

void PixmapSyncher::update()
{
    propertyChanged(property_);
}

PixmapSyncher::PixmapSyncher(PropertyNode* property, QLabel* label)
    : PropertySyncher(property),
      label_(label),
      currentState_(0)

{
    label_->installEventFilter(this);
}

void PixmapSyncher::addState(const Common::String& value,
                             const Common::String& pixmap,
                             const Common::String& tooltip)
{
    State state;
    state.value_   = value;
    state.pixmap_  = pixmap;
    state.tooltip_ = tooltip;
    state.enabled_ = true;
    states_.push_back(state);
}

bool PixmapSyncher::eventFilter(QObject* obj, QEvent* e)
{
    if (obj != label_ || !states_.size())
        return false;
    if (e->type() == QEvent::MouseButtonPress ||
        e->type() == QEvent::MouseButtonDblClick) {
        DBG(UI.TEST) << "mouse pressed " << std::endl;
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(e);
        if (Qt::LeftButton == mouse_event->button()) {
            uint prevState = currentState_;
            do {
                ++currentState_;
                if (currentState_ >= states_.size())
                    currentState_ = 0;
                if (currentState_ == prevState) // loop
                    return true;
            } while (!states_[currentState_].enabled_);
            property_->setString(states_[currentState_].value_);
            return true;
        }
    }
    return false;
}

void PixmapSyncher::enableState(const String& state, bool enable = true)
{
    for (uint i = 0; i < states_.size(); ++i) {
        if (states_[i].value_ == state) {
            states_[i].enabled_ = enable;
            return;
        }
    }
}

void PixmapSyncher::syncProperty(PropertyNode*)
{
    for (uint i = 0; i < states_.size(); ++i) {
        if (states_[i].value_ != property_->getString())
            continue;
        currentState_ = i;
        label_->setPixmap(Sui::icon_provider().getPixmap(states_[i].pixmap_));
        if (!states_[i].tooltip_.isEmpty())
            QToolTip::add(label_, states_[i].tooltip_);
    }
}

PixmapSyncher::~PixmapSyncher()
{
}

//////////////////////////////////////////////////////////////////////

ColorSyncher::ColorSyncher(PropertyNode* property, QWidget* widget)
    : PropertySyncher(property),
      widget_(widget)
{
    propertyChanged(property_);
    widget_->setAutoFillBackground(true);
}

void ColorSyncher::syncProperty(PropertyNode*)
{
    Common::StringTokenizer st(property_->getString(), ",");
    uint rgb[3] = { 0U }, *p = &rgb[0];
    for (bool ok = true; ok && p < &rgb[3] && st; *p++ = st.next().toUInt(&ok));
    if (p != &rgb[3])
        return;
    QColor color(rgb[0], rgb[1], rgb[2]);
    QPalette palette(widget_->palette());
    palette.setColor(QPalette::Window, color);
    widget_->setPalette(palette);
}

ColorSyncher::~ColorSyncher()
{
}

//////////////////////////////////////////////////////////////////////

SpinnerSyncher::SpinnerSyncher(PropertyNode* property, QSpinBox* spinner)
    : PropertySyncher(property),
      spinner_(spinner)
{
    propertyChanged(property_);
    connect(spinner_, SIGNAL(valueChanged(int)),
                this, SLOT(valueChanged(int)));
}

void SpinnerSyncher::valueChanged(int)
{
    if (property_->getInt() != spinner_->value())
        property_->setInt(spinner_->value());
}

void SpinnerSyncher::textChanged(const QString& text)
{
    valueChanged(text.toInt());
}

void SpinnerSyncher::syncProperty(PropertyNode*)
{
    if (spinner_->value() != property_->getInt()) {
        int value = property_->getInt();
        if (spinner_->minValue() > value)
            value = spinner_->minValue();
        if (spinner_->maxValue() < value)
            value = spinner_->maxValue();
        spinner_->setValue(value);
    }
}

SpinnerSyncher::~SpinnerSyncher()
{
}

} // namespace Sui
