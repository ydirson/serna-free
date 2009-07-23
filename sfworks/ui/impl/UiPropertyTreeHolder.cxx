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
#include "ui/UiAction.h"
#include "ui/UiPropSyncher.h"
#include "ui/UiItemSearch.h"
#include "ui/UiDocument.h"
#include "ui/impl/ui_debug.h"

#include <QApplication>

using namespace Common;

namespace Sui {

//////////////////////////////////////////////////////////////////////////

class PropertyTreeHolder::SyncherList : 
        public std::list<RefCntPtr<PropSyncher> > {};

PropertyTreeHolder::PropertyTreeHolder(PropertyNode* root)
    : properties_(root)
{
}
    
PropertyTreeHolder::~PropertyTreeHolder()
{
}

PropertyNode* PropertyTreeHolder::property(const String& name) const 
{
    PropertyNode* pn = properties();
    return (pn->name() == name) ? pn : pn->getProperty(name);
}

const String& PropertyTreeHolder::get(const String& name) const
{
    PropertyNode* prop = property(name);
    return (prop) ? prop->getString() : String::null();
}

bool PropertyTreeHolder::getBool(const String& name) const
{                                                
    PropertyNode* prop = property(name);
    return (prop) ? prop->getBool() : false;
}

int PropertyTreeHolder::getInt(const String& name) const
{                                                
    PropertyNode* prop = property(name);
    return (prop) ? prop->getInt() : 0;
}

double PropertyTreeHolder::getDouble(const String& name) const
{                                                
    PropertyNode* prop = property(name);
    return (prop) ? prop->getDouble() : 0;
}

////////////

void PropertyTreeHolder::set(const String& name, const String& value)
{
    properties()->makeDescendant(name)->setString(value);
}

void PropertyTreeHolder::setBool(const String& name, bool value)
{
    properties()->makeDescendant(name)->setBool(value);
}

void PropertyTreeHolder::setInt(const String& name, int value)
{
    properties()->makeDescendant(name)->setInt(value);
}

void PropertyTreeHolder::setDouble(const String& name, double value)
{
    properties()->makeDescendant(name)->setDouble(value);
}

////////////

void PropertyTreeHolder::installSyncher(PropertyNode* prop)
{
    if ("sync-property" == prop->name()) {
        if (syncherList_.isNull())
            syncherList_ = new SyncherList;
        syncherList_->push_back(new PropSyncher(prop, this));
        return;
    }
    for (PropertyNode* i = prop->firstChild(); i; i = i->nextSibling()) 
        installSyncher(i);
}
    
void PropertyTreeHolder::setupSynchers(Item* item)
{
    if (syncherList_.isNull())
        return;
    for (SyncherList::iterator i = syncherList_->begin(); 
         i != syncherList_->end(); ++i)
        (*i)->subscribe(item);
}

//////////////////////////////////////////////////////////////////////////

PropSyncher::PropSyncher(PropertyNode* spec, PropertyTreeHolder* holder)
    : spec_(spec),
      doNotify_(true),
      holder_(holder),
      from_(0)
{
    if (!spec_.isNull() && holder) {
        if (spec_->getProperty("item")) {
            Item* item = dynamic_cast<Item*>(holder);
            if (item)
                item->addWatcher(this);
        }
        else {
            from_ = holder;
            subscribe();
        }
    }
}

void PropSyncher::subscribe(Item* item)
{
    if (item && spec_->getProperty("item") && item->action() == holder_) 
        item->addWatcher(this);
}

void PropSyncher::subscribe()
{
    String prop_name = spec_->getSafeProperty("from")->getString();
    fromProp_ = (prop_name.isEmpty()) ? 0 : 
        from_->properties()->makeDescendant(prop_name);

    prop_name = spec_->getSafeProperty("to")->getString();
    toProp_ = (prop_name.isEmpty()) ? 0 : 
        holder_->properties()->makeDescendant(prop_name);

    if (fromProp_.isNull() || toProp_.isNull()) 
        return;
    fromProp_->addWatcher(this);
    if ("from" != spec_->getSafeProperty("direction")->getString())
        toProp_->addWatcher(this);
    propertyChanged(fromProp_.pointer());
}

void PropSyncher::childInserted(Item* item)
{
    if (0 == from_ && item->get(NAME) == 
        spec_->getSafeProperty("item")->getString()) {

        from_ = item;
        subscribe();
    }
}

void PropSyncher::childRemoved(Item* /*parent*/, Item* /*item*/)
{
}

//! Notifications about "holder_" dynamics
void PropSyncher::inserted(Item* item)
{
    if (item != holder_ && item->action() != holder_) 
        return;
    Document* doc = item->documentItem();
    if (doc)
        from_ = doc->findItem(
            ItemName(spec_->getSafeProperty("item")->getString()));
    if (from_) {
        subscribe();
        return;
    }
    if (doc) 
        doc->addWatcher(this, true);
}

void PropSyncher::removed(Item* /*item*/)
{
}

///////////////

String PropSyncher::masterValue(const String& toValue) const
{
    PropertyNode* map = spec_->getProperty("value-map");
    if (toValue.isNull()) {
        if (map) 
            map = map->getProperty(fromProp_->getString());
        return (map) ? map->getString() : fromProp_->getString();
    }
    if (map)
        for (map = map->firstChild(); map; map = map->nextSibling())
            if (map->getString() == toValue)
                return map->name();
    return toValue;
}

void PropSyncher::propertyChanged(PropertyNode* prop) 
{
    if (!doNotify_ || toProp_.isNull())
        return;
    doNotify_ = false;
    if (prop == fromProp_)
        toProp_->setString(masterValue());
    else 
        fromProp_->setString(masterValue(toProp_->getString()));
    doNotify_ = true;
}

///////////////////////////////////////////////////////////

UI_EXPIMP String get_translated(const char* context, const String& value)
{
    nstring to_translate;
    to_translate.reserve(value.length() + 1);
    const Char* cp = value.unicode();
    const Char* ce = cp + value.length();
    for (; cp < ce; ++cp) {
        if (cp->unicode() & 0xFF80)
            return value;   // do not translate if we have non-ascii chars
        to_translate += cp->unicode() & 0x7F;
    }
    return qApp->translate(context, to_translate.c_str());
}

static String do_translate(const PropertyTreeHolder* holder,
                        const String& value)
{
    nstring tr_context(holder->translationContext().latin1());
    if (tr_context.empty())
        tr_context = DEFAULT_TRANSLATION_CONTEXT;
    return get_translated(tr_context.c_str(), value);
}
    
String PropertyTreeHolder::getTranslated(const String& name) const
{
    return do_translate(this, get(name));
}

String PropertyTreeHolder::getTranslated(const PropertyNode* prop) const
{
    return prop ? do_translate(this, prop->getString()) : String();
}

} // namespace
