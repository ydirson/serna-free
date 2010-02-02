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
#include "grove/IdManager.h"
#include "grove/Nodes.h"
#include "grove/grove_trace.h"
#include "grove/SectionSyncher.h"
#include "grove/GroveSectionRoot.h"
#include "grove/MatchPattern.h"
#include "grove/udata.h"

#include "common/Url.h"
#include "common/PtrSet.h"
#include "common/PropertyTree.h"

#include <map>

#include "stdio.h"

#if defined(_WIN32)
# include <windows.h>
#else
# include <sys/time.h>
#endif

//#define IDM_IDREF

// TODO: add stripping part before '#' for SCOPED_IDREF's

using namespace Common;

namespace GroveLib {

// an example of scopes/id definition
// <id-scope-defs>
//   <id-scope-def>
//     <pattern>*[@class="- topic/topic "]</pattern>
//     <attr-name>id</attr-name>
//     <id-def>
//       <pattern>*</pattern>
//       <attr-name>id</attr-name>
//     </id-def>
//     <idref-def>
//       <pattern>xref</pattern>
//       <attr-name>href|conref</attr-name>
//     </idref-def>
//     <idref-def>
//       <pattern>@conref</pattern>
//       <attr-name>conref</attr-name>
//     </idref-def>
//   </id-scope-def> 

const char IdManager::SCOPE_DEFS[]    = "id-scope-defs";
const char IdManager::SCOPE_DEF[]     = "id-scope-def";
const char IdManager::PATTERN[]       = "pattern";
const char IdManager::ATTR_NAME[]     = "attr-name";
const char IdManager::ID_DEF[]        = "id-def";
const char IdManager::IDREF_DEF[]     = "idref-def";
const char IdManager::IDREF_FORMAT[]  = "idref-format";

template <class V, class K> class IdTableKeyFunction {
public:
    typedef K KeyType;
    static const K& key(const COMMON_NS::PtrBase<V>& value)
                       { return value->name(); }
    static const K& key(const V* value) { return value->name(); }
};

class IdScopeManager::IdTable : public SmartPtrSet<RefCntPtr<IdTableEntry>,
    IdTableKeyFunction<IdTableEntry, String> > {};

class IdManager::OldValueTable : public std::map<const Attr*, String> {};

class IdScopeDef : public RefCounted<>,
                   public XListItem<IdScopeDef, 
                    XTreeNodeRefCounted<IdScopeDef> > {
public:
    bool    init(const PropertyNode* scopeDef, 
                 const NodeWithNamespace* nsNode,
                 const MatchPatternFactory&);

    struct Def { MatchPatternPtr pattern_; String idAttr_, format_; };
    typedef Common::Vector<Def> IdDefs;
    typedef Common::Vector<Def> IdrefDefs;

    MatchPatternPtr scopePattern() const { return scopePattern_; }
    const String&   scopeIdAttr() const { return scopeIdAttr_; }

    const IdDefs&   idDefs() const { return idDefs_; }
    const IdrefDefs& idrefDefs() const { return idrefDefs_; }

private:
    MatchPatternPtr scopePattern_;
    String          scopeIdAttr_;
    IdDefs          idDefs_;
    IdrefDefs       idrefDefs_;
};

class IdScopeDefs : public XList<IdScopeDef, 
    XTreeNodeRefCounted<IdScopeDef> > {};

// get ID part from the scoped IDREF string
GROVE_EXPIMP String get_idref(const String& idref)
{
    const Char* cp = idref.unicode();
    const Char* ce = cp + idref.length();
    int i = idref.rfind('#');
    if (i >= 0)
        cp += i + 1;
    while (cp < ce && *cp != '/')
        ++cp;
    if (cp >= ce || ++cp >= ce)
        return String::null();
    return String(cp, ce - cp);
}

// get scope part from the scoped IDREF string
GROVE_EXPIMP String get_id_scope(const String& idref)
{
    const Char* uc = idref.unicode();
    const Char* ce = uc + idref.length();
    int i = idref.rfind('#');
    if (i >= 0)
        uc += i + 1;
    const Char* cp = uc;
    while (cp < ce && *cp != '/')
        ++cp;
    if (cp >= ce) // no slash found - this is global id or scope
        return (uc == cp) ? idref : String(uc, ce - uc); 
    return String(uc, cp - uc);
}

static inline IdScopeManager* get_scope(const IdManager* idm, const Node* n)
{
    const Element* elem = CONST_ELEMENT_CAST(n);            
    const Attr* attr = elem->attrs().firstChild();
    for (; attr; attr = attr->nextSibling()) {
        if (attr->idSubClass() == Attr::ID_SCOPE_DEF &&
            attr->idClass() == Attr::IS_ID) {
                IdTableEntry* ite = idm->findIdEntry(attr->value());
                if (!ite)
                    continue;
                if (ite->scopeManager())
                    return ite->scopeManager();
                const IdScopeDef* scope_def = idm->findScopeDef(attr);
                if (!scope_def)
                    continue;
                ite->setScopeManager(new IdScopeManager
                     (const_cast<IdManager*>(idm), scope_def, attr->value()));
                return ite->scopeManager();
        }
    }
    return 0;
}

static inline IdScopeManager* find_scope(const IdManager* idm, const Node* n)
{
    for (; n; n = n->parent()) {
        if (is_id_scope_ext(n)) {
            IdScopeManager* sm = get_scope(idm, n);
            if (sm)
                return sm;
        }
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////

IdScopeManager::IdScopeManager(IdManager* idm, 
                               const IdScopeDef* scopeDef,
                               const String& name)
    : name_(name),
      idTable_(new IdTable), 
      idManager_(idm),
      scopeDef_(scopeDef)
{
}

IdEntryPtr IdScopeManager::idEntry(const String& ids)
{
    IdTable::iterator it = idTable_->find(ids);
    IdTableEntry* e = (it == idTable_->end()) ? 0 : it->pointer();
    if (0 == e) {
        DBG(GROVE.IDM) << "IdScopeManager(" << this 
            << "): created id table entry for <" << ids << ">\n";
        e = new IdTableEntry(this);
        e->name_ = ids;
        idTable_->insert(e);
    }
    return e;
}

IdEntryPtr IdScopeManager::idEntry(const Attr* attr)
{
    return (attr->idSubClass() != Attr::SCOPED_IDREF) 
        ? idEntry(attr->value()) : idEntry(get_idref(attr->value()));
}

IdTableEntry* IdScopeManager::findIdEntry(const String& ids) const
{
    IdTable::iterator it = idTable_->find(ids);
    if (it == idTable_->end())
        return 0;
    return it->pointer();
}

void IdScopeManager::getIdList(Common::PropertyNode* idl) const
{
    IdTable::const_iterator it = idTable_->begin();
    for (; it != idTable_->end(); ++it)
        if ((*it)->element())
            idl->appendChild(new PropertyNode((*it)->name()));
}

IdEntryPtr IdScopeManager::processAttrAdd(const Attr* attr)
{
    DBG(GROVE.IDM) << "IdScopeManager(" << name_ << "): add <"
        << attr->value() << "> idc:" << attr->idClass()
        << " subc:" << attr->idSubClass() << std::endl;
    IdEntryPtr entry = idEntry(attr);
    if (attr->idClass() == Attr::IS_ID || 
        attr->idSubClass() == Attr::SCOPED_ID) {
        if (entry->findIdNode(attr))
            return entry.pointer();
        const_cast<Attr*>(attr)->setDuplicateId(entry->ids_.firstChild());
        entry->ids_.appendChild(new IdTableEntry::IdEntry(attr));
        if (entry->parent())
            entry->remove();    // remove from pending-del list
    } else {
#ifdef IDM_IDREF
        if (entry->findIdrefAttr(attr))
            return entry.pointer();
        entry->idRefs_.appendChild(new IdTableEntry::IdRefEntry(attr));
        if (entry->parent())
            entry->remove();    // remove from pending-del list
#endif // IDM_IDREF
    }
    idManager()->setOldValue(attr, entry->name());
    entry->notifyChanged();
    return entry;
}

void IdScopeManager::processAttrRemove(const Attr* attr)
{
    DBG(GROVE.IDM) << "IdScopeManager(" << name_ << "): remove <"
        << attr->value() << "> idc:" << attr->idClass()
        << " subc:" << attr->idSubClass() << std::endl;
    IdEntryPtr entry = idEntry(attr);
#ifdef IDM_IDREF
    if (attr->idClass() == Attr::IS_IDREF || 
        attr->idSubClass() == Attr::SCOPED_IDREF) {
            IdTableEntry::IdRefEntry* ire = entry->findIdRefAttr(attr);
            if (ire)
                ire->remove();
    } else 
#endif // IDM_IDREF
    {
        IdTableEntry::IdEntry* ide = entry->findIdNode(attr);
        if (ide)
            ide->remove();
        if (entry->idNode())
            const_cast<Attr*>(entry->idNode())->setDuplicateId(false);
    }
    idManager()->eraseOldValue(attr);
    entry->checkEmpty();
    entry->notifyChanged();
}

// idea: to have search by attr pointer, to the old entry

IdEntryPtr IdScopeManager::processAttrChange(const Attr* attr)
{
    DBG(GROVE.IDM) << "IdScopeManager(" << name_ << "): change <"
        << attr->value() << "> idc:" << attr->idClass()
        << " subc:" << attr->idSubClass() << std::endl;
    bool found_oval = false;
    String ovalue(idManager()->oldValue(attr, &found_oval));
    IdTableEntry* oldEntry = 0;
    if (found_oval) {
        IdTable::iterator idit = idTable_->find(ovalue);
        if (idit != idTable_->end())
            oldEntry = idit->pointer();
    }
    if (attr->idClass() == Attr::IS_ID ||
        attr->idSubClass() == Attr::SCOPED_ID) {
        if (oldEntry) {
            IdTableEntry::IdEntry* ide = oldEntry->findIdNode(attr);
            if (ide) {
                const_cast<Attr*>(ide->idAttr())->setDuplicateId(false);
                ide->remove();
            }
            if (oldEntry->idNode())
                const_cast<Attr*>(oldEntry->idNode())->setDuplicateId(false);
            oldEntry->notifyChanged();
        }
        return processAttrAdd(attr);
    }
#ifdef IDM_IDREF
    // idref change
    IdEntryPtr entry = idEntry(attr);
    IdTableEntry::IdRefEntry* ire  = entry->findIdRefAttr(attr);
    if (ire)
        return entry; // already there
    if (oldEntry) {
        ire = oldEntry->findIdRefAttr(attr);
        if (ire)
            ire->remove();
        oldEntry->notifyChanged();
    }
#endif // IDM_IDREF
    return processAttrAdd(attr);
}

void IdScopeManager::eraseEntry(const Common::String& id)
{
    IdTable::iterator it = idTable_->find(id);
    if (it != idTable_->end())
        idTable_->erase(it);
}

void IdScopeManager::setScopeDef(const IdScopeDef* def)
{
    scopeDef_ = def;
}

bool IdScopeManager::isEmpty() const
{
    return idTable_->begin() == idTable_->end();
}

void IdScopeManager::dump() const
{
#ifndef NDEBUG
    DBG(GROVE.IDM) << "ID Manager Table(" << name_ << "):" << std::endl;
    IdTable::const_iterator it = idTable_->begin();
    for (; it != idTable_->end(); ++it)
        (*it)->dump();
#endif
}

IdScopeManager::~IdScopeManager()
{
    DBG(GROVE.IDM) << "~IdScopeManager(" << scopeName() << "): "
        << this << std::endl;
}

//////////////////////////////////////////////////////////////

IdManager::IdManager()
    : IdScopeManager(this, 0, String::null()), 
      GroveVisitor(0),
      scopeDefs_(new IdScopeDefs),
      ovTable_(new OldValueTable),
      hasPendingRemovals_(false),
      isScopingEnabled_(false)
{
}
    
void IdManager::setScopeDefs(const PropertyNode* scopeDesc,
                             const PropertyNode* nsprops)
{
    isScopingEnabled_ = false;
    DBG(GROVE.IDM) << "IdManager: setting scope-defs, mpf="
        << matchPatternFactory_.pointer() << std::endl;
    if (matchPatternFactory_.isNull())
        return;
    ElementPtr nsNode(new Element(NOTR("#ns-map")));
    nsNode->copyNsFromProps(nsprops);
    const PropertyNode* pn = scopeDesc->firstChild();
    for (; pn; pn = pn->nextSibling()) {
        if (pn->name() != SCOPE_DEF)
            continue;
        RefCntPtr<IdScopeDef> scope_def(new IdScopeDef);
        if (!scope_def->init(pn, nsNode.pointer(), *matchPatternFactory_))
            return;
        DBG(GROVE.IDM) << "adding scope def: ";
        DBG_IF(GROVE.IDM) pn->dump();
        scopeDefs_->appendChild(scope_def.pointer());
    }
    isScopingEnabled_ = !!scopeDefs_->firstChild();
}

void IdManager::enable(GroveSectionRoot* gsr)
{
    if (gsr_)
        gsr_->deregisterVisitor(this);
    gsr_ = gsr;
    if (gsr_) {
        DBG(GROVE.IDM) << "IDM: enable for " 
            << gsr->grove()->topSysid() << std::endl;
        gsr_->registerVisitor(this);
        for (const Node* n = gsr_->firstChild(); n; n = n->nextSibling()) {
            if (n->nodeType() == Node::ELEMENT_NODE) {
                childInserted(n);
                break;
            }
        }
    }
}
Element* IdManager::lookupElement(const String& ids) const
{
    IdTableEntry* ite = findIdrefEntry(ids);
    if (0 == ite || 0 == ite->idNode())
        return 0;
    return ite->idNode()->element();
}

Element* 
IdManager::lookupScopedElement(const String& ids, const Node* context) const
{
    DBG(GROVE.IDM) << "IDM: lookupScopedElement: ref=<" 
        << ids << ">, context=" << context->nodeName() << "(" 
        << context << ")\n";
    if (!scopeDefs_->firstChild())
        return lookupElement(ids);
    IdScopeManager* idm = find_scope(this, context);
    if (!idm)
        return lookupElement(ids);
    IdTableEntry* ite = idm->findIdEntry(ids);
    if (0 == ite || 0 == ite->idNode())
        return 0;
    return ite->idNode()->element();
}

IdTableEntry* IdManager::findIdrefEntry(const Common::String& idref) const
{
    DBG(GROVE.IDM) << "IDM: findIdrefEntry: gsid=" 
        << gsr_->grove()->topSysid() << std::endl;
    DBG(GROVE.IDM) << "IDM: findIdrefEntry: idref=" << idref << std::endl;
    const Char* uc = idref.unicode();
    const Char* ce = uc + idref.length();
    int i = idref.rfind('#');
    if (i >= 0)
        uc += i + 1;
    const Char* scope_tag = uc;
    const Char* cp = uc;
    for (; cp < ce && *cp != '/'; ++cp)
        ;
    IdTableEntry* ite;
    if (cp >= ce) {
        ite = findIdEntry((idref.unicode() == uc) ? 
            idref : String(uc, ce - uc));
        DBG(GROVE.IDM) << "IDM: findIdrefEntry: unscoped ID, ite="
            << ite << std::endl;
        return ite;
    }
    scope_tag = cp++;
    ite = findIdEntry(String(uc, scope_tag - uc));
    if (0 == ite) {
        DBG(GROVE.IDM) << "IDM: findIdrefEntry: scope not found\n";
        return 0;
    }
    if (cp >= ce)
        return ite;
    if (!ite->scopeManager()) {
        DBG(GROVE.IDM) << "IDM: findIdrefEntry: no SM\n";
        return 0;
    }
    ite = ite->scopeManager()->findIdEntry(String(cp, ce - cp));
    DBG(GROVE.IDM) << "IDM: findIdrefEntry: found ite=" << ite << std::endl;
    return ite;
}

/////////////////////////////////////////////////////////////

static inline void set_id_subclass(const Attr* attr, int sc)
{
    const_cast<Attr*>(attr)->setIdSubClass((Attr::IdSubClass)sc);
}

static IdScopeManager* find_sm(IdManager* idm, 
                               const String& idref,
                               bool create = false)
{
    String id_scope = get_id_scope(idref);
    IdTableEntry* ite = idm->findIdEntry(id_scope);
    if (!create)
        return ite ? ite->scopeManager() : 0;
    IdEntryPtr ite_holder;
    if (!ite) {
        ite_holder = idm->idEntry(id_scope);
        ite = ite_holder.pointer();
    }
    if (!ite->scopeManager()) 
        ite->setScopeManager(new IdScopeManager(idm, 0, id_scope));    
    return ite->scopeManager();
}

// set scopes recursively, stop on scope nodes
static void set_scopes(IdScopeManager* sm, 
                       IdScopeManager* old_sm, 
                       Node* n)
{
    if (n->nodeType() != Node::ELEMENT_NODE || is_id_scope_ext(n))
        return;
    Attr* attr = ELEMENT_CAST(n)->attrs().firstChild();
    for (; attr; attr = attr->nextSibling()) {
        if (attr->idSubClass() == Attr::SCOPED_ID) {
            if (old_sm) 
                old_sm->processAttrRemove(attr);    // remove from old scope    
            if (sm)
                sm->processAttrAdd(attr);               // add to new scope
            continue;
        }
#ifdef IDM_IDREF
        if (attr->idSubClass() == Attr::SCOPED_IDREF) {
            String sn = get_id_scope(attr->value());
            if (sm && sn == sm->scopeName()) {
                // 1) idref was undefined, new scope fits
                // 2) idref was defined, new scope redefines it
                IdScopeManager* osm = find_sm(sm->idManager(), attr->value());
                if (osm)
                    osm->processAttrRemove(attr);
                sm->processAttrAdd(attr);
            }
            continue;
        }
#endif // IDM_IDREF
    }
    for (n = n->firstChild(); n; n = n->nextSibling())
        set_scopes(sm, old_sm, n);
}

static void set_child_scopes(IdScopeManager* sm, 
                             IdScopeManager* old_sm, 
                             Node* n)
{
    for (n = n->firstChild(); n; n = n->nextSibling())
        set_scopes(sm, old_sm, n);
}

/////////////////////////////////////////////////////////////////////////

static const IdScopeDef::Def* find_id_def(const IdScopeManager* sm,
                                          const Attr* attr,
                                          bool* isIdref)
{
    if (!sm->scopeDef())
        return 0;
    const IdScopeDef& scope_def = *sm->scopeDef();
    *isIdref = false;
    for (uint i = 0; i < scope_def.idDefs().size(); ++i) {
        const IdScopeDef::Def* def = &scope_def.idDefs()[i];
        if (attr->nodeName() == def->idAttr_ &&
            def->pattern_->matched(attr->element()))
                return def;
    }
    *isIdref = true;
    for (uint i = 0; i < scope_def.idrefDefs().size(); ++i) {
        const IdScopeDef::Def* def = &scope_def.idrefDefs()[i];
        if (attr->nodeName() == def->idAttr_ &&
            def->pattern_->matched(attr->element()))
                return def;
    }
    return 0;
}

static void process_new_scope(IdManager* mgr, 
                              IdScopeManager* osm,
                              const Attr* attr)
{
    if (attr->idClass() != Attr::IS_ID)
        return;
    DBG(GROVE.IDM) << "Process new scope: " << attr->nodeName()
        << "/" << attr->value() << std::endl;
    IdEntryPtr ite = mgr->processAttrAdd(attr);
    const IdScopeDef* scope_def = mgr->findScopeDef(attr);
    if (!scope_def) 
        return;
    DBG(GROVE.IDM) << "AttributeAdded: newScope, id=" 
        << attr->value() << std::endl;
    if (!ite->scopeManager()) 
        ite->setScopeManager(new IdScopeManager
            (mgr, scope_def, attr->value()));
    else if (!ite->scopeManager()->scopeDef())
        ite->scopeManager()->setScopeDef(scope_def);
    set_id_subclass(attr, Attr::ID_SCOPE_DEF);
    set_child_scopes(ite->scopeManager(), osm, attr->element());
    set_id_scope_ext(attr->element(), true);
}

static void process_scoped_id(IdScopeManager* sm, const Attr* attr)
{
    DBG(GROVE.IDM) << "Process scoped id=" << attr->nodeName()
        << "/" << attr->value() << ", sm=" << sm << std::endl;
    if (0 == sm) {
        DBG(GROVE.IDM) << "process_scoped_id: no IDM\n";
        return;
    }
    bool is_idref = false;
    const IdScopeDef::Def* id_def = find_id_def(sm, attr, &is_idref);
    if (!id_def) {
        DBG(GROVE.IDM) << "process_scoped_id: no def\n";
        return;
    }
    if (is_idref) {
        DBG(GROVE.IDM) << "attrAdded: scoped_idref, scope=" << sm
            << ", name=" << attr->nodeName() << std::endl;
        // filter out non-local references
        String ids = attr->value();
        if (ids.rfind('#') <= 0) {
            set_id_subclass(attr, Attr::SCOPED_IDREF);
#ifdef IDM_IDREF
            sm = find_sm(sm->idManager(), attr->value(), true);
            sm->processAttrAdd(attr); 
#endif // IDM_IDREF
        }
    } else {
        DBG(GROVE.IDM) << "attrAdded: scoped_id, scope=" << sm
            << ", name=" << attr->nodeName() << std::endl;
        set_id_subclass(attr, Attr::SCOPED_ID);
        sm->processAttrAdd(attr);
    }
}

void IdManager::attributeAdded(const Attr* attr)
{
    if (scopeDefs_->firstChild()) {
        IdScopeManager* sm = find_scope(this, attr->element());
        if (attr->idClass() != Attr::NOT_ID) 
            process_new_scope(this, sm, attr);
        else
            process_scoped_id(sm, attr);
    } else if (attr->idClass() != Attr::NOT_ID)
        process_new_scope(this, 0, attr);
}

void IdManager::attributeRemoved(const Element* elem, const Attr* attr)
{
    if (attr->idClass() != Attr::NOT_ID) {
        if (attr->idSubClass() == Attr::ID_SCOPE_DEF) {
            IdEntryPtr ite = idEntry(attr->value());
            if (ite && ite->scopeManager()) { 
                DBG(GROVE.IDM) << "IDM: attrRemoved: Removing scope <"
                    << ite->scopeManager()->scopeName() << "\n";
                IdScopeManager* psm = find_scope(this, elem);
                DBG(GROVE.IDM) << "IDM: parentScope="
                    << (psm ? psm->scopeName() : String()) << std::endl;
                set_child_scopes(psm, ite->scopeManager(), 
                    const_cast<Element*>(elem));
            }
        }            
        processAttrRemove(attr);            
    } else {
        switch (attr->idSubClass()) {
            case Attr::SCOPED_ID: {
                IdScopeManager* psm = find_scope(this, elem);
                if (psm)
                    psm->processAttrRemove(attr);    
                break;
            }
#ifdef IDM_IDREF
            case Attr::SCOPED_IDREF: {
                IdScopeManager* ism = find_sm(this, attr->value());
                if (ism)
                    ism->processAttrRemove(attr);
                break;
            }
#endif // IDM_IDREF
            default:
                break;
        }
    }
}

void IdManager::attributeChanged(const Attr* attr)
{
    if (attr->idClass() != Attr::NOT_ID) {
        RefCntPtr<IdScopeManager> osm;
        String ovalue;
        if (attr->idSubClass() == Attr::ID_SCOPE_DEF) {
            bool found_ov;
            ovalue = oldValue(attr, &found_ov);
            osm = find_sm(this, ovalue);
        }
        DBG(GROVE.IDM) << "IDM: attrChange: scope change, ov=" 
            << ovalue << ", nv=" << attr->value() << std::endl;
        IdEntryPtr ite = processAttrChange(attr);
        if (!ite->scopeManager()) {
            const IdScopeDef* scope_def = findScopeDef(attr);
            if (scope_def) {
                DBG(GROVE.IDM) << "AttributeChanged: newScope, id=" 
                    << attr->value() << std::endl;
                IdScopeManager* nsm = new 
                    IdScopeManager(this, scope_def, attr->value());
                ite->setScopeManager(nsm);
                set_id_subclass(attr, Attr::ID_SCOPE_DEF);
                DBG(GROVE.IDM) << "set_scopes: nsm=" << nsm->scopeName()
                    << ", osm=" << (osm ? osm->scopeName() : String()) 
                    << std::endl;
                set_child_scopes(nsm, osm.pointer(), attr->element());
                set_id_scope_ext(attr->element(), true);
            }
        } else 
            set_child_scopes(ite->scopeManager(),
                osm.pointer(), attr->element());
        return;
    }
    switch (attr->idSubClass()) {
        case Attr::SCOPED_ID: {
            IdScopeManager* psm = find_scope(this, attr->element());
            if (psm)
                psm->processAttrChange(attr);    
            break;
        }
#ifdef IDM_IDREF
        case Attr::SCOPED_IDREF: {
            bool found_ovalue;
            String ovalue = oldValue(attr, &found_ovalue);
            IdScopeManager* osm = find_sm(this, ovalue);
            IdScopeManager* ism = find_sm(this, attr->value());
            if (osm != ism) {
                if (osm)
                    osm->processAttrRemove(attr);
                ism->processAttrAdd(attr);
            } else 
                if (osm)
                    osm->processAttrChange(attr);
            break;
        }
#endif // IDM_IDREF
        default:
            break;
    }
}

const IdScopeDef* IdManager::findScopeDef(const Attr* attr) const
{
    const IdScopeDef* sd = scopeDefs_->firstChild();
    for (; sd; sd = sd->nextSibling()) {
        RT_ASSERT(sd->scopePattern());
        RT_ASSERT(attr && attr->element());
        if (sd->scopeIdAttr() != attr->nodeName())
            continue;
        if (sd->scopePattern()->matched(attr->element()))
            return sd;
    }
    return 0;
}

static void remove_ids(IdManager* mgr, const Node* n, IdScopeManager* sm)
{
    if (n->nodeType() != Node::ELEMENT_NODE)
        return;
    Attr* attr = static_cast<const Element*>(n)->attrs().firstChild();
    for (; attr; attr = attr->nextSibling()) {
        if (attr->idClass() != Attr::NOT_ID) {
            mgr->processAttrRemove(attr);
            sm = 0;
            continue;
        }
        switch (attr->idSubClass()) {
            case Attr::SCOPED_ID: {
                if (sm)
                    sm->processAttrRemove(attr);
                break;
            }
#ifdef IDM_IDREF            
            case Attr::SCOPED_IDREF: {
                IdScopeManager* ism = find_sm(mgr, attr->value());
                if (ism)
                    ism->processAttrRemove(attr);
                break;
            }
#endif // IDM_IDREF
            default:
                break;
        }
    }
    for (n = n->firstChild(); n; n = n->nextSibling())
        if (n->nodeType() == Node::ELEMENT_NODE)
            remove_ids(mgr, n, sm);
}

void IdManager::childRemoved(const Node* p, const Node* n)
{
    if (n->nodeType() != Node::ELEMENT_NODE)
        return;
    remove_ids(this, n, isScopingEnabled() ? find_scope(this, p) : 0);
}

//////////////////////////////

static void add_scopes(IdManager* mgr, const Node* n)
{
    Attr* attr = static_cast<const Element*>(n)->attrs().firstChild();
    for (; attr; attr = attr->nextSibling()) {
        if (attr->idClass() != Attr::IS_ID) 
            continue;
        process_new_scope(mgr, 0, attr);
    }
    for (n = n->firstChild(); n; n = n->nextSibling())
        if (n->nodeType() == Node::ELEMENT_NODE)
            add_scopes(mgr, n);
}
static void add_ids(IdManager* mgr, const Node* n, IdScopeManager* sm)
{
    Attr* attr = static_cast<const Element*>(n)->attrs().firstChild();
    if (is_id_scope_ext(n)) 
        sm = get_scope(mgr, n);
    for (; attr; attr = attr->nextSibling()) 
        if (attr->idClass() != Attr::IS_ID)
            process_scoped_id(sm, attr);
    for (n = n->firstChild(); n; n = n->nextSibling())
        if (n->nodeType() == Node::ELEMENT_NODE)
            add_ids(mgr, n, sm);
}

void IdManager::childInserted(const Node* n)
{
    if (n->nodeType() != Node::ELEMENT_NODE)
        return;
    add_scopes(this, n);
    if (isScopingEnabled())
        add_ids(this, n, find_scope(this, n));
}

//////////////////////////////

void IdManager::setMatchPatternFactory(MatchPatternFactory* fact)
{
    matchPatternFactory_ = fact;
}

/////////////////////////////////////////////////////////////////

const String& IdManager::oldValue(const Attr* attr, bool* found) const
{
    OldValueTable::iterator ovit = ovTable_->find(attr);
    if (ovit != ovTable_->end()) {
        *found = true;
        return ovit->second;         
    }
    *found = false;
    return String::null();
}

void IdManager::eraseOldValue(const Attr* attr) 
{
    OldValueTable::iterator ovit = ovTable_->find(attr);
    if (ovit != ovTable_->end())
        ovTable_->erase(ovit);
}

void IdManager::setOldValue(const Attr* attr, const String& v)
{
    (*ovTable_)[attr] = v;
}

void IdManager::dump() const
{
    IdScopeManager::dump();
}

IdManager::~IdManager()
{
    //DBG(GROVE.IDM) << "~IdManager: " << this << std::endl;
}

////////////////////////////////////////////////////////////////////

void IdTableEntry::checkEmpty()
{
    IdManager& idm = *myScopeManager_->idManager();
    if (!ids_.firstChild() && !idRefs_.firstChild())
        idm.addPendingRemove(this);
    idm.processPendingRemove();
}

inline void IdTableEntry::removeSelf()
{
    DBG(GROVE.IDM) << "IDM: self-removing<" << name_ 
        << "> from IDM<" << myScopeManager_->scopeName() << ">\n";
    myScopeManager_->eraseEntry(name_);
    IdManager* idm = myScopeManager_->idManager();
    if (idm == myScopeManager_)
        return;
    if (!myScopeManager_->isEmpty())
        return;
    IdTableEntry* ite = idm->findIdEntry(myScopeManager_->scopeName());
    DBG(GROVE.IDM) << "ite = " << ite << std::endl;
    if (!ite || ite->idNode())
        return;
    idm->eraseEntry(myScopeManager_->scopeName());
}

inline bool IdTableEntry::isEmpty() const
{
    return ids_.empty() && idRefs_.empty() && !hasSubscribers();
}

const Attr* IdTableEntry::idNode() const
{
    if (ids_.firstChild())
        return ids_.firstChild()->idAttr();
    return 0;
}

Element* IdTableEntry::element() const
{
    if (0 == idNode())
        return 0;
    return idNode()->element();
}

IdTableEntry::IdRefEntry* IdTableEntry::findIdRefAttr(const Attr* attr) const
{
    IdRefEntry* e = idRefs_.firstChild();
    for (; e; e = e->nextSibling())
        if (e->idRefAttr() == attr)
            return e;
    return 0;
}

IdTableEntry::IdEntry* IdTableEntry::findIdNode(const Attr* attr) const
{
    IdEntry* e = ids_.firstChild();
    for (; e; e = e->nextSibling())
        if (e->idAttr() == attr)
            return e;
    return 0;
}

void IdManager::addPendingRemove(IdTableEntry* entry)
{
    IdTableEntry* e = pendingRemove_.firstChild();
    for (; e; e = e->nextSibling())
        if (entry == e)
            return;     // already in the list
    pendingRemove_.appendChild(entry);
    hasPendingRemovals_ = true;
    DBG(GROVE.IDM) << "IDM: added pending remove for: " << entry->name()
        << std::endl;
}

void IdManager::processPendingRemove()
{
    if (!hasPendingRemovals_)
        return;
    IdTableEntry* e = pendingRemove_.firstChild();
    IdTableEntry* next_e;
    while (e) {
        next_e = e->nextSibling();
        if (e->isEmpty()) 
            e->removeSelf();
        e = next_e;
    }
    hasPendingRemovals_ = !pendingRemove_.empty();
}

void IdTableEntry::dump() const
{
#ifndef NDEBUG
    int scount = 0;
    IdTableEntry* self = const_cast<IdTableEntry*>(this);
    SubscriberList::const_iterator sbi = self->subscriberList().begin();
    for (; sbi != self->subscriberList().end(); ++sbi)
        ++scount;
    DBG(GROVE.IDM) << "IDV:<" << name() << "> IDN:" << idNode();
    if (element())
        DBG(GROVE.IDM) << '/' << element()->nodeName();
    DBG(GROVE.IDM) << " NSUB:" << scount << std::endl;
    const IdRefEntry* e = idRefs_.firstChild();
    if (e) {
        DDINDENT;
        for (; e; e = e->nextSibling()) {
            DBG(GROVE.IDM) << "IDREF: " << e->idRefAttr() << " " 
                << e->idRefAttr()->value();
            if (e->idRefAttr()->element())
                DBG(GROVE.IDM) << " <" 
                    << e->idRefAttr()->element()->nodeName() << ">";
            DBG(GROVE.IDM) << std::endl;
        }
    }
    if (scopeManager()) {
        DBG(GROVE.IDM) << "Dumping sub-scope-mgr: " 
            << scopeManager()->scopeName() << std::endl;
        DDINDENT;
        scopeManager()->dump();
    }
#endif // NDEBUG
}

bool IdScopeDef::init(const PropertyNode* scopeDef, 
                      const NodeWithNamespace* nsNode,
                      const MatchPatternFactory& mpf)
{
    typedef IdManager I;
    try {
        const PropertyNode* pn = scopeDef->firstChild();
        for (; pn; pn = pn->nextSibling()) {
            if (pn->name() == I::PATTERN) {
                scopePattern_ = mpf.make(pn->getString(), nsNode);
                continue;
            }
            if (pn->name() == I::ATTR_NAME) {
                scopeIdAttr_ = pn->getString();
                continue;
            }
            bool is_def = pn->name() == I::ID_DEF;
            if (is_def || pn->name() == I::IDREF_DEF) {
                Def def;
                def.pattern_ = mpf.make(pn->getString(I::PATTERN), nsNode);
                def.idAttr_ = pn->getString(I::ATTR_NAME);
                if (def.idAttr_.isEmpty())
                    def.idAttr_ = NOTR("id");
                (is_def ? idDefs_ : idrefDefs_).push_back(def);
            }
        }
    } catch(...) {
        return false;
    }
    if (scopeIdAttr_.isEmpty())
        scopeIdAttr_ = NOTR("id");
    return true;
}

///////////////////////////////////////////////////////////////////////////

static bool is_unique_id(const Grove* grove, const String& id)
{
    const IdManager* idm = grove->idManager();
    if (0 == idm)
        return false;
    return !idm->lookupElement(id);
}

static bool is_unique_id(const Node* node, const String& id)
{
    const Grove* grove = node->grove()->root();
    if (!is_unique_id(grove, id))
        return false;
    for (grove = grove->firstChild(); grove; grove = grove->nextSibling())
        if (!is_unique_id(grove, id))
            return false;
    return true;
}

GROVE_EXPIMP String make_id_string(const Node* node, const String& fmt)
{
    if (node->nodeType() != GroveLib::Node::ELEMENT_NODE)
        node = parentNode(node);
    if (0 == node)
        return String::null();
    String temp_res, result;
    temp_res.reserve(64);
    const Char* cp = fmt.unicode();
    const Char* ce = cp + fmt.length();
    int  idx_pos = -1;
    bool force_index = false;
    for (; cp < ce; ++cp) {
        if (cp->unicode() != '%') {
            temp_res.append(*cp);
            continue;
        }
        if (++cp >= ce)
            break;
        switch (cp->unicode()) {
            case 'l': { // localname
                int idx = node->nodeName().find(':');
                temp_res += (idx > 0) 
                    ? node->nodeName().mid(idx + 1) : node->nodeName();
                continue;
            }
            case 'q': { // qname
                temp_res += node->nodeName();
                continue;
            }
            case 't': { // timestamp
                char buf[32];
#if defined(_WIN32)
                (void) _snprintf(buf, sizeof(buf), "%08X", 
                    GetTickCount() & 0xFFFFFFFF);
#else // _WIN32
                struct timeval tv;
                (void) gettimeofday(&tv, 0);
                (void) snprintf(buf, sizeof(buf), "%06X%02X", 
                    uint(tv.tv_sec & 0xFFFFFF), 
                    uint((tv.tv_usec >> 10) & 0xFF));
#endif // _WIN32
                temp_res += buf;
                continue;
            }
            case 'N':
            case 'n': { // index
                idx_pos = temp_res.length();
                force_index = (cp->unicode() == 'N');
                continue;
            }
            case 'f': { // filename of the current entity
                const EntityReferenceStart* ers = get_ers(node);
                Url url(ers ? ers->xmlBase() : node->grove()->topSysid());
                String path = url[Url::FILENAME];
                const Char* cp = path.unicode();
                const Char* ce = cp + path.length();
                for (; cp < ce; ++cp) {
                    if (!cp->isLetterOrNumber() && cp->unicode() != '$'
                        && cp->unicode() != '-' && cp->unicode() != '_')
                            temp_res += '_';
                    else
                        temp_res += *cp;
                }
                continue;
            }
            case 'e': { // entity name
                const EntityReferenceStart* ers = get_ers(node);
                if (ers && ers->getSectParent() && ers->entityDecl())
                    temp_res += ers->entityDecl()->name();
                continue;
            }
            default:
                temp_res.append(*cp);
                continue;
        }
    }
    if (idx_pos < 0)
        idx_pos = temp_res.length();
    for (uint index = 1; ; ++ index) {
        result = temp_res;
        if (force_index || index > 1)
            result.insert(idx_pos, String::number(index));
        if (result.length() && result[0].isDigit())
            result.prepend('S');
        if (is_unique_id(node, result))
            break;
    }
    return result;
}

} // namespace GroveLib
