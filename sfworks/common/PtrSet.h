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
    Set of wrappers for stl::set, which keep objects by pointer but sort them by value.
    It also uses specialized fast allocator for RB-tree nodes to minimize fragmentation.
 */
#ifndef PTR_SET_H_
#define PTR_SET_H_

#include "common/common_defs.h"
#include "common/DefaultSmallObject.h"
#include "common/RefCntPtr.h"
#include "common/RbTree.h"


COMMON_NS_BEGIN
# ifdef COMMON_PRAGMA_BEGIN
# pragma COMMON_PRAGMA_BEGIN
# endif // - COMMON_PRAGMA_BEGIN

template <class V, class K = V> class DefaultPtrSetKeyFunction {
public:
    typedef K KeyType;
    static const K& key(const PtrBase<V>& value) { return *value; }
    static const K& key(const V* value) { return *value; }
};

/*! Generic template for creating custom sets of SmartPointers.
    T must be a smart pointer class.
    For example, set of locked externally-refcounted objects:

        SmartPtrSet<Locked<RefCntPtr<Foo, ExternalRefCnt> > > set;

    By default this set uses T as a key type; this may be overriden by
    specifying your own key function class which defines it's own KeyType
    (see DefaultPtrSetKeyFunction template as an example - probably
     you won't need a template, just concrete class).
 */
template <class SPTR,
          class KF = DefaultPtrSetKeyFunction<typename SPTR::OriginalType> >
  class SmartPtrSet : public RbTree<SPTR, typename KF::KeyType,
                                    KF, DefaultSmallObject> {
public:
    typedef typename SPTR::OriginalType OriginalType;
};

/*! A owner set which keeps copiable objects (with CopyOwner interface).
    Objects are deleted only when set itself is destroyed (e.g.
    set.erase(foo) won't delete foo*).
 */
template <class T, class KF = DefaultPtrSetKeyFunction<T> >
  class CopyOwnerPtrSet : public RbTree<T*, typename KF::KeyType,
                                        KF, DefaultSmallObject> {
    typedef RbTree<T*, typename KF::KeyType, KF, DefaultSmallObject> BaseType;
    using BaseType::first;
    using BaseType::clear;
    
public:
    typedef T OriginalType;
    typedef typename BaseType::iterator iterator;
    typedef typename BaseType::const_iterator const_iterator;

    /*! Deletes whole contents of the set */
    void deleteAll()
    {
        for (iterator it = first(); (bool)it; ++it)
            delete *it;
        clear();
    }
    CopyOwnerPtrSet(const CopyOwnerPtrSet& other) { copy(other); }
    CopyOwnerPtrSet& operator=(const CopyOwnerPtrSet& other)
    {
        deleteAll(); copy(other);
    }
    ~CopyOwnerPtrSet() { deleteAll(); }
    CopyOwnerPtrSet() {}

private:
    void copy(const CopyOwnerPtrSet& other)
    {
        for (const_iterator it = other.first(); it; ++it)
            insert((*it)->copy());
    }
};

/*! Set which owns all kept objects. Objects are deleted only when set
    itself is destroyed (e.g. set.erase(foo) won't delete foo*).
 */
template <class T, class KF = DefaultPtrSetKeyFunction<T> >
  class OwnerPtrSet : public CopyOwnerPtrSet<T, KF> {
public:
    OwnerPtrSet(const CopyOwnerPtrSet<T, KF>& other)
      : CopyOwnerPtrSet<T, KF>(other) {}
    OwnerPtrSet& operator=(const CopyOwnerPtrSet<T, KF>& other) {
        CopyOwnerPtrSet<T, KF>::operator=(other);
    }
    OwnerPtrSet() {}
private:
    OwnerPtrSet(const OwnerPtrSet& other);
    OwnerPtrSet& operator=(const OwnerPtrSet& other);
};

# ifdef COMMON_PRAGMA_END
# pragma COMMON_PRAGMA_END
# endif // - COMMON_PRAGMA_END
COMMON_NS_END

#endif // PTR_SET_H_

