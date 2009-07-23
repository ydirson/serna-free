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

#ifndef GROVE_ENTITY_DECL_SET_H_
#define GROVE_ENTITY_DECL_SET_H_

#include "grove/grove_exports.h"
#include "grove/grove_defs.h"
#include "grove/EntityDecl.h"
#include "common/String.h"
#include "common/PtrSet.h"
#include "common/SernaApiRefCounted.h"

namespace GroveLib {

class EntityDecl;

/*! This class represents set of entities used in a document. Because
 *  it is possible to define entities in different ways (in document
 *  prolog entities and in schemas), we keep complete document entity
 *  set as a several linked entity sets.
 */
class GROVE_EXPIMP EntityDeclSet : public Common::SernaApiRefCounted {
public:
    typedef COMMON_NS::RefCntPtr<EntityDecl> DeclPtr;
    template <class V, class K> class EntityKeyFunction {
    public:
        typedef K KeyType;
        static const K& key(const COMMON_NS::PtrBase<V>& value) { return value->name(); }
        static const K& key(const V* value) { return value->name(); }
    };
    typedef COMMON_NS::SmartPtrSet<DeclPtr,
        EntityKeyFunction<EntityDecl, COMMON_NS::String> > DeclSet;
    typedef DeclSet::iterator iterator;

    /// Returns iterator pointing to the beginning of entity decl table
    iterator begin() const { return table_.begin(); }

    /// Returns iterator pointing beyond the end of entity decl table
    iterator end() const { return table_.end(); }

    /// Lookup single entity declaration with given name
    EntityDecl* lookupDecl(const COMMON_NS::String& name) const
    {
        iterator it = table_.find(name);
        if (it != end())
            return it->pointer();
        return 0;
    }

    /// Remove entity declaration from entity declaration table
    void        removeDecl(const COMMON_NS::String& name);

    /*! Insert new entity declaration into this entity declaration set.
     *  If \a replace is true, then old declaration with the same name
     *  will be replaced with a new one.
     */
    void        insertDecl(EntityDecl* d, bool replace = true);

    /// Rebulds content() of modified internal entities
    void        rebuildInternalEntityContents(Grove* g);

    /// Debug-print whole entity declaration set
    void        dump() const;

    EntityDeclSet() {}

private:
    EntityDeclSet(EntityDeclSet&);
    EntityDeclSet& operator=(EntityDeclSet&);

    DeclSet     table_;
};

} // namespace

#endif // GROVE_ENTITY_DECL_SET_H_
