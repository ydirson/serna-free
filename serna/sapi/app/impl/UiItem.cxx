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
#include "sapi/app/UiItem.h"
#include "sapi/app/SernaDoc.h"
#include "sapi/common/PropertyNode.h"
#include "sapi/common/impl/xtn_wrap_impl.h"
#include "ui/UiAction.h"
#include "ui/LiquidItem.h"
#include "ui/UiItemSearch.h"
#include "ui/UiProps.h"
#include "docview/SernaDoc.h"
#include "common/safecast.h"
#include <QPoint>

namespace SernaApi {

class LiquidItemWrap : public Sui::LiquidItem {
public:
    LiquidItemWrap(Common::PropertyNode* props,
                   UiLiquidItemBase* itemBase)
        : LiquidItem(props ? props : new Common::PropertyNode), 
          itemBase_(itemBase) 
    {
        // set default name equal to item class
        if (get(Sui::NAME).isEmpty())
            set(Sui::NAME, itemBase->itemClass());
    }
    virtual Common::String itemClass() const 
        { return itemBase_->itemClass(); }
    virtual Common::String widgetClass() const 
        { return itemBase_->widgetClass(); }
    virtual void        releaseFocus() { itemBase_->releaseFocus(); }
    virtual void        grabFocus() const { itemBase_->grabFocus(); }
    virtual QWidget*    makeWidget(QWidget* parent, Type type) 
    { return itemBase_->makeWidget(parent, (UiLiquidItemBase::Type) type); }
    virtual bool        doAttach() { return itemBase_->doAttach(); }
    /// may be reimplemented for custom detach
    virtual bool        doDetach() { return itemBase_->doDetach(); }
    /// Called when item property changes
    virtual void        propertyChanged(Common::PropertyNode* prop)
        { return itemBase_->propertyChanged(prop); }
    /// invoked when escape button is pressed
    virtual void        escapePressed() { itemBase_->escapePressed(); }
    /// May be reimplemented in custom item to indicate active focus
    virtual void        widgetFocusChanged(bool v) 
        { itemBase_->widgetFocusChanged(v); }

private:
    friend class UiLiquidItemBase;
    Common::OwnerPtr<UiLiquidItemBase> itemBase_;
};

#define SELF    static_cast<Sui::Item*>(getRep())
#define LW_IMPL static_cast<LiquidItemWrap*>(getRep())
#define LWCALL(f)  LW_IMPL->LiquidItem::f

////////////////////////////////////////////////////////////////////////

UiLiquidItemBase::UiLiquidItemBase(SernaApiBase* base)
    : SimpleWrappedObject(base)
{
}

UiLiquidItemBase::~UiLiquidItemBase()
{
}

UiItem UiLiquidItemBase::item() const
{
    return getRep();
}

SString UiLiquidItemBase::widgetClass() const
{
    if (getRep())
        return LWCALL(widgetClass());
    return SString();
}

void UiLiquidItemBase::showContextMenu(int x, int y)
{
    if (getRep())
        LWCALL(showContextMenu(QPoint(x, y)));
}

void UiLiquidItemBase::releaseFocus() const
{
    if (getRep())
        LWCALL(releaseFocus());
}
void UiLiquidItemBase::grabFocus() const
{
    if (getRep())
        LWCALL(grabFocus());
}

bool UiLiquidItemBase::doAttach()
{
    return getRep() ? LWCALL(doAttach()) : false; 
}

bool UiLiquidItemBase::doDetach()
{
    return getRep() ? LWCALL(doDetach()) : false; 
}

void UiLiquidItemBase::propertyChanged(const PropertyNode& prop)
{
    if (getRep() && prop.getRep())
        LWCALL(propertyChanged(static_cast<Common::PropertyNode*>
            (prop.getRep())));
}

void UiLiquidItemBase::escapePressed()
{
    if (getRep())
        LWCALL(escapePressed());
}

void UiLiquidItemBase::widgetFocusChanged(bool v)
{
    if (getRep())
        LWCALL(widgetFocusChanged(v));
}

////////////////////////////////////////////////////////////////////////

UiItem::UiItem(SernaApiBase* rep)
    : RefCountedWrappedObject(rep)
{
}

UiItem::~UiItem()
{
}

////////////////////////////////////////////////////////////////////////

UiItem UiItemMaker::makeLiquidItem(const PropertyNode& props,
                                   UiLiquidItemBase* base)
{
    LiquidItemWrap* wrap = new LiquidItemWrap(
        static_cast<Common::PropertyNode*>(props.getRep()), base);
    base->setRep(wrap);
    return wrap;
}

SString UiItem::translate(const char* context, const SString& str)
{
    return Sui::get_translated(context, str);
}

SString UiItem::name() const
{
    if (getRep())
        return SELF->get(Sui::NAME);
    return SString();
}

UiAction UiItem::action() const
{
    if (getRep())
        return UiAction(SELF->action());
    return 0;
}

SString UiItem::itemClass() const
{
    if (getRep())
        return SELF->itemClass();
    return SString();
}

SString UiItem::widgetClass() const
{
    if (getRep())
        return SELF->widgetClass();
    return SString();
}

QWidget* UiItem::widget() const
{
    if (getRep())
        return SELF->widget(0);
    return 0;
}

////////////////////////////////////////////////////////////////////////

PropertyNode UiItem::property(const SString& prop) const
{
    if (getRep())
        return SELF->property(prop);
    return 0;
}

PropertyNode UiItem::itemProps(bool update)
{
    if (getRep())
        return PropertyNode(SELF->itemProps(update));
    return 0;
}

PropertyNode UiItem::currActionProp() const
{
    if (getRep())
        return PropertyNode(SELF->currActionProp());
    return 0;
}

////////////////////////////////////////////////////////////////////////

SString UiItem::get(const SString& propName) const
{
    return property(propName).getString();
}

SString UiItem::getTranslated(const SString& propName) const
{
    if (getRep())
        return SELF->getTranslated(propName);
    return 0;
}

bool UiItem::getBool(const SString& propName) const
{
    return property(propName).getBool();
}

int UiItem::getInt(const SString& propName) const
{
    return property(propName).getInt();
}

double UiItem::getDouble(const SString& propName) const
{
    return property(propName).getDouble();
}

////////////////////////////////////////////////////////////////////////

void UiItem::set(const SString& propName, const SString& value)
{
    itemProps().makeDescendant(propName).setString(value);
}

void UiItem::setBool(const SString& propName, bool value)
{
    itemProps().makeDescendant(propName).setBool(value);
}

void UiItem::setInt(const SString& propName, int value)
{
    itemProps().makeDescendant(propName).setInt(value);
}

void UiItem::setDouble(const SString& propName, double value)
{
    itemProps().makeDescendant(propName).setDouble(value);
}

void UiItem::setVisible(bool v) 
{
    if (getRep())
        SELF->property(Sui::IS_VISIBLE)->setBool(v);
}

bool UiItem::isVisible() const
{
    if (getRep())
        return SELF->getBool(Sui::IS_VISIBLE);
    return false;
}

////////////////////////////////////////////////////////////////////////

void UiItem::attach(bool recursive)
{
    if (getRep())
        SELF->attach(recursive);
}

void UiItem::detach(bool recursive)
{
    if (getRep())
        SELF->detach(recursive);
}

void UiItem::dispatch()
{
    if (getRep())
        SELF->dispatch();
}

void UiItem::grabFocus() const
{
    if (getRep())
        SELF->grabFocus();
}

void UiItem::releaseFocus() const
{
    if (getRep())
        SELF->releaseFocus();
}

void UiItem::showContextMenu(int x, int y)
{
    if (getRep())
        SELF->showContextMenu(QPoint(x, y));
}

////////////////////////////////////////////////////////////////////////

UiAction UiItem::findAction(const SString& name) const
{
    if (getRep())
        return UiAction(SELF->findAction(name));
    return 0;
}

UiItem UiItem::findItemByName(const SString& name) const
{
    if (getRep())
        return UiItem(SELF->findItem(Sui::ItemName(name)));
    return 0;
}

UiItem UiItem::findItemByClass(const SString& name) const
{
    if (getRep())
        return UiItem(SELF->findItem(Sui::ItemClass(name)));
    return 0;
}

UiItem UiItem::findItemByAction(const UiAction& action) const
{
    if (getRep())
        return UiItem(SELF->findItem
            (Sui::ItemAction(static_cast<Sui::Action*>(action.getRep()))));
    return 0;
}

SernaDoc UiItem::asSernaDoc() const
{
    typedef ::SernaDoc SD;
    return dynamic_cast<SD*>(getRep());    
}
   
XTREENODE_WRAP_IMPL2(UiItem, Sui::Item, removeItem)

} // namespace SernaApi
