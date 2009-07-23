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
#ifndef GROVE_ENTITY_REFERENCE_TABLE_H_
#define GROVE_ENTITY_REFERENCE_TABLE_H_

#include "grove/grove_defs.h"
#include "grove/EntityDecl.h"
#include "common/PtrSet.h"
#include "common/RefCntPtr.h"
#include "common/Vector.h"
#include "common/asserts.h"
#include "common/SernaApiRefCounted.h"

namespace GroveLib {

class EntityReferenceStart;
class Grove;

// TODO: remove list from ERT, keep only first ERS child on corresponding level.
// this will also kill the need of table merging.

/*! This class is owned by grove and must keep synchronized
 *  lists of references to entity reference nodes.
 */
class GROVE_EXPIMP EntityReferenceTable : public Common::SernaApiRefCounted {
private:
    /*! internal-use-only class*/
    template <class V, class K> class ErtEntryKeyFunction {
    public:
        typedef K KeyType;
        static const K key(const COMMON_NS::PtrBase<V>& value)
        {
            return value->decl();
        }
        static const K key(const V* value)
        {
            return value->decl();
        }
    };
public:
    ////////////////////////////////////////////////////////////////////////

    /// Entity reference table entry.
    class GROVE_EXPIMP ErtEntry : public Common::SernaApiRefCounted {
    public:
        /// Returns pointer to the entity declaration.
        EntityDecl*             decl() const;

        /// Pointer to corresponding EntityReferenceStart nodes
        EntityReferenceStart*   node(ulong idx) const { return nodes_[idx]; }

        /// Number of EntityReferenceStart nodes for this entry
        ulong                   numOfRefs() const { return nodes_.size(); }

        friend class EntityReferenceTable;

        GROVE_OALLOC(ErtEntry);

    private:
        typedef COMMON_NS::Vector<EntityReferenceStart*> nodeList;
        nodeList nodes_;
    };
    typedef COMMON_NS::SmartPtrSet<COMMON_NS::RefCntPtr<ErtEntry>,
        ErtEntryKeyFunction<ErtEntry, const EntityDecl*> > ErtTable;

    ////////////////////////////////////////////////////////////////////////

    /// Entity reference table iterator
    typedef ErtTable::iterator iterator;

    /// Returns iterator pointing to the beginning of the table
    iterator        begin() const { return table_.begin(); }

    /// Returns iterator pointing beyond the end of the table
    iterator        end() const { return table_.end(); }

    /// Lookup single EntityReferenceTable entry
    const ErtEntry* lookup(const EntityDecl* decl) const
    {
        iterator it = table_.find(decl);
        if (it != end())
            return it->pointer();
        return 0;
    }
    /// Initialize table from ERS subtree
    void            initErt(EntityReferenceStart* ers);

    /// Add new entity reference
    void            addEntityRef(EntityReferenceStart* ref);

    /// Remove entity reference
    void            removeEntityRef(const EntityReferenceStart* ref);

    /// Merge-add another entity reference table
    void            mergeAdd(const EntityReferenceTable& t2);

    /// Remove all entity refs which are present in \a t2 from this table
    void            mergeRemove(const EntityReferenceTable& t2);

    void            dump() const;

    EntityReferenceTable();
    ~EntityReferenceTable();
    
private:
    EntityReferenceTable(const EntityReferenceTable&);
    EntityReferenceTable& operator=(const EntityReferenceTable&);
    void            init_ert(EntityReferenceStart*);

    ErtTable table_;
};

} // namespace GroveLib

#endif // GROVE_ENTITY_REFERENCE_TABLE_H_
