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

#ifndef PACKED_PTR_SET_H_
#define PACKED_PTR_SET_H_

#include "common/common_defs.h"
#include "common/common_types.h"
#include "common/asserts.h"

COMMON_NS_BEGIN

/*! A packed set of void pointers. All pointers are kept in a single
    memory chunk; end of chunk is marked as 0, and free slots has
    bit 0 set.
*/
class COMMON_EXPIMP PackedVoidPointerSet {
public:
    PackedVoidPointerSet() : v_(0) {}
    ~PackedVoidPointerSet() { clear(); }

    //! Returns true if set contains given pointer
    bool        check(const void* p) const
    {
        if (0 == v_)
            return false;
        for (const void* const* vp = v_; *vp; ++vp) {
            if (*vp == p)
                return true;
        }
        return false;
    }
    //! Inserts pointer to set
    void        insert(void* p)
    {
        RT_ASSERT(!check(p));
        add(p);
    }
    bool        add(void* p)
    {
        if (!v_) {
            prealloc(p);
            return true;
        }
        void** vp = v_;
        // look for a free slot.
        for(; *vp; ++vp) {
            if (*vp == p)
                return false;
            if (((intptr_t)(*vp)) & 01) {
                *vp = p;
                return true;
            }
        }
        expand_alloc(p, vp);
        return true;
    }

    //! Removes pointer from set
    bool        remove(void* p)
    {
        if (0 == v_)
            return false;
        for (void** vp = v_; *vp; ++vp) {
            if (*vp == p) {
                *vp = (void*)01;
                return true;
            }
        }
        return false;
    }
    //! Returns true is set is empty
    bool        isNull() const { return (0 == v_); }
    //! Removes all pointers from set
    void        clear();
    //! Returns pointer to first item of set
    void**      list() const { return v_; }
    static bool isEnd(const void* p) { return (0 == p); }
    static bool isValid(const void* p) {return (0 == ((ulong)p & 01) );}

    void        dump() const;

private:
    void        prealloc(void*);
    void        expand_alloc(void*, void**);

    void**      v_;
};

/*! A packed list of pointers of arbitrary type.
 */
template <class T> class PackedPointerSet {
public:
    bool            check(const T* p) const { return set_.check(p); }
    void            insert(T* p) { set_.insert(p); }
    void            remove(T* p) { set_.remove(p); }
    void            add(T* p) { set_.add(p); }
    bool            isNull() const { return set_.isNull(); }

    T**             list() const { return reinterpret_cast<T**>(set_.list()); }
    static bool     isEnd(const T* p) {
        return PackedVoidPointerSet::isEnd(p);
    }
    static bool     isValid(const T* p) {
        return PackedVoidPointerSet::isValid(p);
    }
    void            dump() const { set_.dump(); }
    void            clear() { set_.clear(); }

private:
    PackedVoidPointerSet    set_;
};

/*! A packed set of Ptr's.
 */
template <class T> class PackedPtrSet : public PackedPointerSet<T> {
public:
    bool    check(T* p) const { return PackedPointerSet<T>::check(p); }
    void    insert(T* p) {
        PackedPointerSet<T>::insert(p);
        p->incRefCnt();
    }
    void    remove(T* p) {
        if (PackedPointerSet<T>::remove(p))
            p->decRefCnt();
    }
    void    add(T* p) { 
        if (PackedPointerSet<T>::add(p))
            p->incRefCnt();
    }
    void    clear() {
        T** lp = PackedPointerSet<T>::list();
        if (lp) {
            for (; !isEnd(*lp); ++lp) {
                if (!isValid(*lp))
                    continue;
                (*lp)->decRefCnt();
            }
            PackedPointerSet<T>::clear();
        }
    }
    const T* const* constList() const { return PackedPointerSet<T>::list(); }
    ~PackedPtrSet() { clear(); }

    PackedPointerSet<T>::list;
    PackedPointerSet<T>::isEnd;
    PackedPointerSet<T>::isValid;
    PackedPointerSet<T>::dump;
    PackedPointerSet<T>::isNull;
};

COMMON_NS_END

#endif // PACKED_PTR_SET_H_
