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
 *  Implementation of STL-compliant allocator, which uses SmallObject allocator
 *  facility. It is efficient e.g. for allocation of small tree nodes.
 */
#ifndef SMALL_STL_ALLOCATOR_H_
#define SMALL_STL_ALLOCATOR_H_

#include "common/common_defs.h"
#include "common/DefaultSmallObject.h"
#include <stddef.h>


COMMON_NS_BEGIN

/*! Allocator with STL-conforming interface which uses SmallObject allocator.
 */
template <class T, class Allocator> class SmallStlAllocator {
    typedef Allocator allocator;
public:
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T               value_type;

    template <class _Up> struct rebind {
        typedef SmallStlAllocator<_Up, Allocator> other;
    };
    pointer address(reference __x) const { return &__x; }
    const_pointer address(const_reference __x) const { return &__x; }

    T* allocate(size_type __n, const void* = 0)
    {
        if (0 == __n)
            return 0;
        return static_cast<T*> (allocator::allocate(__n * sizeof(T)));
    }
    void deallocate(pointer __p, size_type __n) throw()
    {
        allocator::deallocate(__p, __n * sizeof(T));
    }
    size_type max_size() const throw()
    {
        return size_type(-1) / sizeof(T);
    }
    void construct(pointer __p, const T& __val) { new(__p) T(__val); }
    void destroy(pointer _p) { _p->~T(); }

    SmallStlAllocator() {}

    template <class T1>
        SmallStlAllocator(const SmallStlAllocator<T1, allocator>&) {}
};

template <class _T1, class _T2, class Allocator> inline bool
operator==(const SmallStlAllocator<_T1, Allocator>&,
           const SmallStlAllocator<_T2, Allocator>&)
{
    return true;
}

template <class _T1, class _T2, class Allocator> inline bool
operator!=(const SmallStlAllocator<_T1, Allocator>&,
           const SmallStlAllocator<_T2, Allocator>&)
{
    return false;
}

COMMON_NS_END

#endif // SMALL_STL_ALLOCATOR_H_
