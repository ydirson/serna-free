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
#include "sapi/app/UiAction.h"
#include "sapi/common/PropertyNode.h"
#include "sapi/common/impl/xtn_wrap_impl.h"

#include "common/PropertyTree.h"
#include "common/String.h"

#include "ui/UiProps.h"
#include "ui/UiAction.h"

#define TO_PTN(x) static_cast<Common::PropertyNode*>((x).getRep())
#define SELF   static_cast<Sui::Action*>(getRep())

namespace SernaApi {

////////////////////////////////////////////////////////////////////////////

UiAction::UiAction(SernaApiBase* rep)
    : RefCountedWrappedObject(rep)
{
}

UiAction::~UiAction()
{
}

////////////////////////////////////////////////////////////////////////////

SString UiAction::name() const
{
    if (getRep())
        return SELF->get(Sui::NAME);
    return SString();
}

PropertyNode UiAction::property(const SString& prop) const
{
    if (getRep())
        return SELF->property(prop);
    return 0;
}

PropertyNode UiAction::properties() const
{
    if (getRep())
        return SELF->properties();
    return 0;
}

////////////////////////////////////////////////////////////////////////////

SString UiAction::get(const SString& propName) const
{
    return property(propName).getString();
}

SString UiAction::getTranslated(const SString& propName) const
{
    if (getRep())
        return SELF->getTranslated(propName);
    return 0;
}

bool UiAction::getBool(const SString& propName) const
{
    return property(propName).getBool();
}

int UiAction::getInt(const SString& propName) const
{
    return property(propName).getInt();
}

double UiAction::getDouble(const SString& propName) const
{
    return property(propName).getDouble();
}

////////////////////////////////////////////////////////////////////////////

void UiAction::set(const SString& propName, const SString& value)
{
    properties().makeDescendant(propName).setString(value);
}

void UiAction::setBool(const SString& propName, bool value)
{
    properties().makeDescendant(propName).setBool(value);
}

void UiAction::setInt(const SString& propName, int value)
{
    properties().makeDescendant(propName).setInt(value);
}

void UiAction::setDouble(const SString& propName, double value)
{
    properties().makeDescendant(propName).setDouble(value);
}

////////////////////////////////////////////////////////////////////////////

bool UiAction::isEnabled() const
{
    return getRep() ? SELF->getBool(Sui::IS_ENABLED) : false;
}

void UiAction::setEnabled(bool v)
{
    if (getRep())
        SELF->setEnabled(v);
}

bool UiAction::isToggleable() const
{
    return getRep() ? SELF->getBool(Sui::IS_TOGGLEABLE) : false;
}

bool UiAction::isToggledOn() const
{
    return getRep() ? SELF->getBool(Sui::IS_TOGGLED) : false;
}

void UiAction::setToggled(bool v)
{
    if (getRep())
        SELF->setToggled(v);
}

////////////////////////////////////////////////////////////////////////////

void UiAction::dispatch() const
{
    if (getRep())
        SELF->dispatch();
}

void UiAction::dump() const
{
    if (getRep())
        SELF->dump(0);
}

XTREENODE_WRAP_IMPL2(UiAction, Sui::Action, removeAction)

} // namespace SernaApi

