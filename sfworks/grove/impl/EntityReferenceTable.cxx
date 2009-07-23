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
#include "grove/grove_defs.h"
#include "grove/EntityDecl.h"
#include "grove/EntityDeclSet.h"
#include "grove/EntityReferenceTable.h"
#include "grove/Nodes.h"
#include "grove/grove_trace.h"
#include "grove/RedlineUtils.h"
#include <algorithm>

namespace GroveLib {

EntityReferenceTable::EntityReferenceTable() 
{
}

EntityReferenceTable::~EntityReferenceTable() 
{
}

void EntityReferenceTable::initErt(EntityReferenceStart* ersRoot)
{
    GroveSectionStart* gss = ersRoot->getFirstSect();
    for (; gss; gss = gss->getNextSect())
        init_ert(static_cast<EntityReferenceStart*>(gss));
}

void EntityReferenceTable::init_ert(EntityReferenceStart* ers)
{
    addEntityRef(ers);
    GroveSectionStart* gss = ers->getFirstSect();
    for (; gss; gss = gss->getNextSect())
        init_ert(static_cast<EntityReferenceStart*>(gss));
}

EntityDecl* EntityReferenceTable::ErtEntry::decl() const
{
    return nodes_[0]->entityDecl();
}

void EntityReferenceTable::addEntityRef(EntityReferenceStart* ers)
{
    if (ers->entityDecl()->declType() == EntityDecl::document)
        return;
    iterator it = table_.find(ers->entityDecl());
    if (it == table_.end()) {
        ErtEntry* ee = new ErtEntry;
        ee->nodes_.push_back(ers);
        table_.insert(ee);
        return;
    }
    ErtEntry::nodeList& nl = (*it)->nodes_;
    ErtEntry::nodeList::iterator it2 =
        std::lower_bound(nl.begin(), nl.end(), ers);
    nl.insert(it2, ers);
}

void EntityReferenceTable::removeEntityRef(const EntityReferenceStart* ers)
{
    iterator it = table_.find(ers->entityDecl());
    if (it == table_.end())
        return;
    ErtEntry::nodeList& nl = (*it)->nodes_;
    ErtEntry::nodeList::iterator it2 =
        std::lower_bound(nl.begin(), nl.end(), ers);
    if (it2 == nl.end() || *it2 != ers)
        return;
    nl.erase(it2, it2 + 1);
    if (0 == nl.size())
        table_.erase(it);
}

void EntityReferenceTable::mergeAdd(const EntityReferenceTable& t2)
{
    for (iterator it = t2.begin(); it != t2.end(); ++it) {
        iterator it2 = table_.find((*it)->decl());
        if (it2 == table_.end()) {
            ErtEntry* ee = new ErtEntry;
            ee->nodes_ = (*it)->nodes_;
            table_.insert(ee);
            continue;
        }
        ErtEntry::nodeList& nl1 = (*it)->nodes_;
        ErtEntry::nodeList& nl2 = (*it2)->nodes_;
        ErtEntry::nodeList onl(nl1.size() + nl2.size());
        ErtEntry::nodeList::iterator oi =
            std::set_union(nl1.begin(), nl1.end(),
                nl2.begin(), nl2.end(), onl.begin());
        onl.resize(oi - onl.begin());
        nl2.swap(onl);
    }
}

void EntityReferenceTable::mergeRemove(const EntityReferenceTable& t2)
{
    for (iterator it = t2.begin(); it != t2.end(); ++it) {
        iterator it2 = table_.find((*it)->decl());
        if (it2 == table_.end())
            continue;
        ErtEntry::nodeList& nl1 = (*it)->nodes_;
        ErtEntry::nodeList& nl2 = (*it2)->nodes_;
        ErtEntry::nodeList onl(nl2.size());
        ErtEntry::nodeList::iterator oi =
            std::set_difference(nl2.begin(), nl2.end(),
                nl1.begin(), nl1.end(), onl.begin());
        onl.resize(oi - onl.begin());
        if (onl.size())
            nl2.swap(onl);
        else
            table_.erase(it2);
    }
}

#ifdef GROVE_DEBUG
void EntityReferenceTable::dump() const
{
    using std::endl;
    DDINDENT;

    DDBG << "EntityReferenceTable:" << this << endl;
    DDINDENT;
    for (iterator i = table_.begin(); i != table_.end(); ++i) {
        DDBG << "entity:" << (*i)->decl() << " <" << (*i)->decl()->name() 
             << ">  refs:" << (*i)->numOfRefs() << endl;
        (*i)->decl()->dump();
        for (ulong ref = 0;  ref < (*i)->numOfRefs(); ++ref)
            if ((*i)->node(ref)->getSectParent())
                (*i)->node(ref)->dump();
    }
}
#else
void EntityReferenceTable::dump() const
{
}
#endif // GROVE_DEBUG

} // namespace GroveLib
