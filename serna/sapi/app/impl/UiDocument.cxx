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
#include "sapi/app/UiDocument.h"
#include "sapi/common/PropertyNode.h"
#include "ui/UiDocument.h"
#include "ui/UiAction.h"
#include "ui/ActionSet.h"
#include "common/PropertyTree.h"

namespace SernaApi {

extern void sapi_add_item_maker(UiDocument*, const SString&, UiItemMaker*);

#define UD_IMPL static_cast<Sui::Document*>(getRep())
#define CS_IMPL static_cast<Sui::ActionSet*>(getRep())

UiActionSet UiDocument::actionSet()
{
    return UiActionSet(UD_IMPL->actionSet());
}

UiDocument::UiDocument(SernaApiBase* rep)
    : UiItem(rep)
{
}

UiItem UiDocument::makeItem(const SString& itemClass,
                            const PropertyNode& properties) const
{
    return UD_IMPL->makeItem(itemClass,
        static_cast<Common::PropertyNode*>(properties.getRep()));
}

PropertyNode UiDocument::addItemMaker(const SString& itemClass,
                                      UiItemMaker*  itemMaker,
                                      const SString& acceptableIn)
{
    if (!getRep() || !itemMaker)
        return 0;
    Common::PropertyNodePtr pn = UD_IMPL->addItemMaker(itemClass,
        static_cast<Sui::ItemMaker*>(itemMaker->getRep()), acceptableIn);
    sapi_add_item_maker(this, itemClass, itemMaker);
    return pn.pointer();
}

void UiDocument::makeAndShow(const SString& itemClass,
                             const PropertyNode& properties)
{
    UiItem item = makeItem(itemClass, properties);
    if (!item.getRep())
        return;
    Sui::Item* sui_item = static_cast<Sui::Item*>(item.getRep());
    appendChild(sui_item);
    sui_item->attach();
    sui_item->setBool(Sui::IS_VISIBLE, true);
}

UiDocument::~UiDocument()
{
}

//////////////////////////////////////////////////////////

class UiItemMakerProxy : public Sui::ItemMaker {
public:
    UiItemMakerProxy(UiItemMaker& maker)
        : maker_(maker) {}
     virtual Sui::Item* makeItem(Sui::Action* action, 
                                 Common::PropertyNode* properties)
     {
         UiItem new_item = maker_.makeItem(action, properties);
         // at this point, we have refcounted ui item; we need to
         // release holder (UiItem) without deleting object
         SernaApiBase* item_ptr = new_item.getRep();
         new_item.decRepRefCnt();
         new_item.releaseRep();
         return static_cast<Sui::Item*>(item_ptr);
     }
private:
    UiItemMaker& maker_;
};

UiItemMaker::UiItemMaker()
    : RefCountedWrappedObject(new UiItemMakerProxy(*this))
{
}

UiItemMaker::~UiItemMaker()
{
}

//////////////////////////////////////////////////////////

UiActionSet::UiActionSet(SernaApiBase* rep)
    : SimpleWrappedObject(rep)
{
}

UiAction UiActionSet::makeAction(const SString& name,
                            const SString& inscription,
                            const SString& icon,
                            const SString& tooltip,
                            const SString& whatsThis,
                            const SString& accel,
                            bool isTogglable)
{
    return UiAction(CS_IMPL->makeAction(name,
        inscription, icon, tooltip, whatsThis, accel, isTogglable));
}

UiAction UiActionSet::makeAction(const PropertyNode& pn)
{
    return UiAction(CS_IMPL->makeAction
        (static_cast<Common::PropertyNode*>(pn.getRep())));
}

UiAction UiActionSet::findAction(const SString& action) const
{
    return UiAction(CS_IMPL->findAction(action));
}

void UiActionSet::removeAction(const UiAction& action)
{
    CS_IMPL->removeAction(static_cast<Sui::Action*>(action.getRep()));
}

UiActionSet::~UiActionSet()
{
}

} // namespace SernaApi
