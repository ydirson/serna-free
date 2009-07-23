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
/*! \file
 */

#include "xs/xs_defs.h"
#include "xs/ComponentSpace.h"
#include "xs/Component.h"
#include "xs/NcnCred.h"
#include "xs/Schema.h"
#include "xs/XsDataImpl.h"
#include "xs/XsAttribute.h"
#include "xs/XsElement.h"
#include "xs/XsNodeExt.h"
#include "xs/components/AttributeSet.h"
#include "xs/components/XsElementImpl.h"
#include "grove/ChoiceNode.h"
#include "grove/Nodes.h"
#include "grove/XmlName.h"
#include "grove/XmlPredefNs.h"
#include "common/PtrSet.h"
#include "common/RefCounted.h"
#include "common/PropertyTree.h"
#include "xs/XsMessages.h"
#include <iostream>

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

template <class V, class K> class ComponentKeyFunction {
public:
    typedef K KeyType;
    static const K& key(const PtrBase<V>& c) { return c->constCred()->name();}
    static const K& key(const V* c) { return c->constCred()->name(); }
};

typedef SmartPtrSet<RefCntPtr<Component>,
    ComponentKeyFunction<Component, String> > NsSet;

class ComponentSpaceBase::NsTableItem : public RefCounted<> {
public:
    String      ns_;
    NsSet       nsTable_;

    NsTableItem(const String& nsuri)
        : ns_(nsuri) {}
    ~NsTableItem() {}
};


ComponentSpaceBase::ComponentSpaceBase()
    : lns_("")
{
}

void ComponentSpaceBase::setLns(const String& lns)
{
    lns_ = lns;
}

ComponentSpaceBase::NsTableItem*
 ComponentSpaceBase::getTable(const String& xns) const
{
    String ns = xns.isNull() ? lns_ : xns;
    for(uint i = 0; i < table_.size(); ++i) {
        if (ns == table_[i]->ns_)
            return table_[i].pointer();
    }
    return 0;
}

void ComponentSpaceBase::processRedefine(Schema* s)
{
    NcnCred pvt_ns(s->xsi()->makeCred(String(), true));
    NsTableItem* nstable = getTable(pvt_ns.xmlns());
    if (0 == nstable)
        return;
    Vector<Component*> toRedefine;
    toRedefine.reserve(8192);
    NsSet::iterator it = nstable->nsTable_.begin();
    for (; it != nstable->nsTable_.end(); ++it)
        toRedefine.push_back(it->pointer());
    for (uint i = 0; i < toRedefine.size(); ++i) {
        Component* rc = toRedefine[i];
        Component* origComponent = lookupBase(rc->constCred()->name(), lns_);
        if (0 == origComponent) {
            s->mstream() << XsMessages::redefinitionOfUndefined
                << Message::L_ERROR << rc->constCred()->name()
                << XSP_ORIGIN(rc);
            continue;
        }
        rc->processRedefine(s, origComponent);
    }
}

Component* ComponentSpaceBase::getFirst(const String& xmlns) const
{
    NsTableItem* t = getTable(xmlns);
    if (0 == t)
        return 0;
    NsSet::iterator it = t->nsTable_.first();
    if (t->nsTable_.end() != it)
        return it->pointer();
    return 0;
}


Component* ComponentSpaceBase::lookupBase(const String& name,
                                          const String& xmlns) const
{
    NsTableItem* t = getTable(xmlns);
    if (0 == t)
        return 0;
    NsSet::iterator it = t->nsTable_.find(name);
    if (t->nsTable_.end() != it)
        return it->pointer();
    return 0;
}

Component*
 ComponentSpaceBase::lookupBase(const XS_NAMESPACE::NcnCred& cred) const
{
    NsTableItem* t = getTable(cred.xmlns());
    if (0 == t)
        return 0;
    NsSet::iterator it = t->nsTable_.find(cred.name());
    if (t->nsTable_.end() != it)
        return it->pointer();
    return 0;
}

void ComponentSpaceBase::erase(const XS_NAMESPACE::Component* comp)
{
    NsTableItem* t = getTable(comp->constCred()->xmlns());
    if (0 == t)
        return;
    NsSet::iterator it = t->nsTable_.find(comp->constCred()->name());
    if (t->nsTable_.end() != it)
        t->nsTable_.erase(it);
}

void ComponentSpaceBase::insert(XS_NAMESPACE::Component* comp, bool override)
{
    NsTableItem* t = getTable(comp->constCred()->xmlns());
    if (0 == t) {
        t = new NsTableItem(comp->constCred()->xmlns());
        table_.push_back(t);
    }
    t->nsTable_.insert(comp, override);
}

void ComponentSpaceBase::merge(Schema* s,
                               XS_NAMESPACE::ComponentSpaceBase* sp2)
{
    String w3ns = s->xsd()->schemaNs();
    for(uint i = 0; i < sp2->table_.size(); ++i) {
        NsTableItem* t = sp2->table_[i].pointer();
        if (t->ns_ == w3ns)          // do not copy built-in types
            continue;
        NsSet::iterator it;
        for(it = t->nsTable_.begin(); it != t->nsTable_.end(); ++it)
            insert(it->pointer(), false);
    }
}

void ComponentSpaceBase::getAllComponents(Common::Vector<Component*>& v) const
{
    for (uint i = 0; i < table_.size(); ++i) {
        const NsTableItem* t = table_[i].pointer();
        NsSet::const_iterator it;
        for (it = t->nsTable_.begin(); it != t->nsTable_.end(); ++it)
            v.push_back(it->pointer());
    }
}

ComponentSpaceBase::~ComponentSpaceBase()
{
}

void ComponentSpaceBase::dump() const
{
#ifdef XS_DEBUG
    std::cerr << NOTR("ComponentSpace(") << this << NOTR("):");
    for (uint i = 0; i < table_.size(); ++i) {
        NsTableItem* t = table_[i].pointer();
        NsSet::iterator it;
        for (it = t->nsTable_.begin(); it != t->nsTable_.end(); ++it) {
            it->pointer()->constCred()->dump();
            std::cerr << std::endl;
        }
    }
    std::cerr << std::endl;
#endif // XS_DEBUG
}

XS_NAMESPACE_END
