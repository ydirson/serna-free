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
#ifndef GROVE_ID_MANAGER_H_
#define GROVE_ID_MANAGER_H_

#include "grove/grove_defs.h"
#include "grove/Decls.h"
#include "grove/GroveVisitor.h"
#include "common/SubscriberPtr.h"
#include "common/String.h"
#include "common/OwnerPtr.h"
#include "common/XList.h"
#include "common/SernaApiBase.h"

namespace Common {
    class PropertyNode;
}

namespace GroveLib {

////////////////////////////////////////////////////////////////////////////

class IdManager;
class IdScopeManager;
class MatchPatternFactory;
class IdScopeDef;
class IdScopeDefs;
class NodeWithNamespace;

class GROVE_EXPIMP IdTableEntry : public Common::RefCounted<>,
                                  public Common::SubscriberPtrPublisher,
                                  public Common::XListItem<IdTableEntry> {
public:
    IdTableEntry(IdScopeManager* myScope)
        : myScopeManager_(myScope) {}

    const Common::String&   name() const { return name_; }
    const Attr*             idNode() const;
    Element*                element() const;

    ///////////////////////////////////////////////////////////////////////

    class IdRefEntry : public Common::XListItem<IdRefEntry> {
    public:
        IdRefEntry(const Attr* a)
            : idRefAttr_(a) {}
        const Attr* idRefAttr() const { return idRefAttr_; }
        GROVE_OALLOC(IdRefEntry);
    private:
        const Attr* idRefAttr_;
    };
    typedef Common::XList<IdRefEntry> IdRefList;

    ///////////////////////////////////////////////////////////////////////

    class IdEntry : public Common::XListItem<IdEntry> {
    public:
        IdEntry(const Attr* a)
            : idAttr_(a) {}
        const Attr* idAttr() const { return idAttr_; }
        GROVE_OALLOC(IdEntry);
    private:
        const Attr* idAttr_;
    };
    typedef Common::XList<IdEntry> IdList;

    ///////////////////////////////////////////////////////////////////////

    IdRefList&      idRefList() { return idRefs_; }
    IdRefEntry*     findIdRefAttr(const Attr*) const;
    IdEntry*        findIdNode(const Attr*) const;
    bool            hasIdNodes() const { return !ids_.empty(); }

    /// if this ID also defines a scope
    IdScopeManager* scopeManager() const { return scopeManager_.pointer(); }
    void            setScopeManager(IdScopeManager* sm) { scopeManager_ = sm; }
    bool            isEmpty() const;
    void            removeSelf();
    void            dump() const;

    GROVE_OALLOC(IdTableEntry);
private:
    friend class IdScopeManager;
    void         checkEmpty();

    Common::String      name_;
    IdScopeManager*     myScopeManager_;
    Common::RefCntPtr<IdScopeManager> scopeManager_;
    IdRefList       idRefs_;
    IdList          ids_;
};

typedef Common::RefCntPtr<IdTableEntry> IdEntryPtr;

class GROVE_EXPIMP IdScopeManager : public Common::SernaApiRefCounted {
public: 
    IdScopeManager(IdManager* idManager, 
                   const IdScopeDef* scopeDef,
                   const Common::String& scopeName);
    ~IdScopeManager();

    const Common::String& scopeName() const { return name_; }

    /// Try to find existing entry for given ID in global table
    IdTableEntry*   findIdEntry(const Common::String& idString) const;

    /// Try to find existing entry, and if not found, create it
    IdEntryPtr      idEntry(const Common::String& idString);
    IdEntryPtr      idEntry(const Attr* attr);
    bool            isEmpty() const;

    IdManager*      idManager() const { return idManager_; }
    const IdScopeDef* scopeDef() const { return scopeDef_.pointer(); }
    void            setScopeDef(const IdScopeDef* def);

    /// Returns list of ID's
    void            getIdList(Common::PropertyNode*) const;

    void            dump() const;

    IdEntryPtr      processAttrChange(const Attr*); // process value change
    void            processAttrRemove(const Attr*); // process attr removal
    IdEntryPtr      processAttrAdd(const Attr*);    // process attr addition

private:
    IdScopeManager(const IdScopeManager&);
    IdScopeManager& operator=(const IdScopeManager&);

    friend class IdTableEntry;
    class   IdTable;
    class   OldValueTable;
    
    void    eraseEntry(const Common::String&);
    
    Common::String name_;
    Common::OwnerPtr<IdTable>       idTable_;
    IdManager*  idManager_;
    Common::ConstRefCntPtr<IdScopeDef> scopeDef_;
};

// Id Manager contains both standard (root) IdScopeManager, plus
// additonal IdScopeManager's, if needed.
class GROVE_EXPIMP IdManager : public IdScopeManager,
                               public GroveVisitor {
public:
    static const char      SCOPE_DEFS[];
    static const char      SCOPE_DEF[];
    static const char      PATTERN[];
    static const char      ATTR_NAME[];
    static const char      ID_DEF[];
    static const char      IDREF_DEF[];
    static const char      IDREF_FORMAT[];

    IdManager();
    ~IdManager();

    void            enable(GroveSectionRoot*);
    bool            isEnabled() const { return !!gsr_; }
    bool            isScopingEnabled() const { return isScopingEnabled_; }
    void            dump() const;
    
    // Set scope definitions. Disable scoped operation if scopeDesc = 0.
    void            setScopeDefs(const Common::PropertyNode* scopeDesc,
                                 const Common::PropertyNode* nsMappings = 0);
    void            setMatchPatternFactory(MatchPatternFactory*);

    // Returns appropriate scope definition, if attribute defines a scope
    const IdScopeDef* findScopeDef(const Attr* attr) const;

    IdTableEntry*   findIdrefEntry(const Common::String& idString) const;

    /// Lookup element by ID
    Element*        lookupElement(const Common::String& idString) const;
    Element*        lookupScopedElement(const Common::String& ids,
                                        const Node* context) const;

    virtual void    childInserted(const Node*);
    virtual void    attributeAdded(const Attr*);
    void            addPendingRemove(IdTableEntry*);
    void            processPendingRemove();

private:
    virtual void    nodeDestroyed(const Node*) {}
    virtual void    childRemoved (const Node*, const Node*);
    virtual void    attributeChanged(const Attr*);
    virtual void    attributeRemoved(const Element*, const Attr*);
    virtual void    textChanged(const Text*) {}
    virtual void    genericNotify(const GroveLib::Node*, void*) {}
    virtual void    nsMappingChanged(const NodeWithNamespace*,
                                     const Common::String&) {}
    IdManager(const IdScopeManager&);
    IdManager& operator=(const IdScopeManager&);

    const Common::String& oldValue(const Attr* attr, bool*) const;
    void    setOldValue(const Attr*, const Common::String&);
    void    eraseOldValue(const Attr*);
    
    friend class IdScopeManager;
    class OldValueTable;
    class ScopeManagerTable;
    typedef Common::XList<IdTableEntry> IdTableEntryList;
    
    Common::OwnerPtr<MatchPatternFactory> matchPatternFactory_;
    Common::OwnerPtr<IdScopeDefs> scopeDefs_;
    Common::OwnerPtr<OldValueTable> ovTable_;
    IdTableEntryList pendingRemove_;
    bool            hasPendingRemovals_;
    bool            isScopingEnabled_;
};

GROVE_EXPIMP Common::String make_id_string(const Node*, 
                                           const Common::String& format);

GROVE_EXPIMP Common::String get_id_scope(const Common::String&);
GROVE_EXPIMP Common::String get_idref(const Common::String&);

} // namespace GroveLib

#endif // GROVE_ID_MANAGER_H_
