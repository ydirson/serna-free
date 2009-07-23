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
// Copyright (c) 2002 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#ifndef STRING_STORAGE_H_
#define STRING_STORAGE_H_

#include "common/common_defs.h"
#include "StringDecl.h"
#include "FreeStorage.h"

COMMON_NS_BEGIN

template <typename E, class A = std::allocator<E> >
class DefaultStorage : public FreeStorage<E, A> {

private:

    typedef typename A::size_type   size_type;
    typedef E*                      iterator;
    typedef const E*                const_iterator;
    typedef A                       allocator_type;
    typedef FreeStorage<E, A>       Base;

protected:

    inline size_type const_size() const { return Base::const_size(); }
    inline size_type nonconst_size() { return Base::nonconst_size(); }

    inline void set_size(size_type sz) { Base::set_size(sz); }
    inline bool is_const() const { return Base::is_const(); }
    inline bool is_null() const { return Base::is_null(); }

    inline void set_null() { Base::set_null(); }

    // Construct Storage to hold info about const char* pointer
    DefaultStorage(size_type len, const E* s) : Base(len, s) {}

public:

    //! Default ctor
    DefaultStorage(const A& a = A()) : Base(a) {}
    //! Copy ctor
    DefaultStorage(const DefaultStorage& s) : Base(s) {}
    //! Copy ctor with different capacity. Not for const strings
    DefaultStorage(const DefaultStorage& s, size_type cap) : Base(s, cap) {}
    //! Ctor from buffer of size 'len', and particular capacity
    DefaultStorage(const E* s, size_type l, size_type cap = 0, const A& a = A())
    : Base(s, l, cap, a) {}
    //! Constructors for optimized operator+
    DefaultStorage(const E* s1, size_type n1, const E* s2, size_type n2)
    : Base(s1, n1, s2, n2) {}
    DefaultStorage(E c, const E* s, size_type n) : Base(c, s, n) {}
    DefaultStorage(const E* s, size_type n, E c) : Base(s, n, c) {}

    DefaultStorage(size_type len, E c, size_type cap = 0, const A& a = A(),
                   int d = 0) : Base(len, c, cap, a, d) {}
    ~DefaultStorage() {}

    inline iterator begin() { return Base::begin(); }
    inline iterator end() { return Base::end(); }

    inline const_iterator begin() const { return Base::begin(); }
    inline const_iterator end() const { return Base::end(); }

    inline size_type size() const { return Base::size(); }
    inline size_type max_size() const { return Base::max_size(); }
    inline size_type capacity() const { return Base::capacity(); }

    //! resize operations
    inline void resize(size_type n) { return Base::resize(n); }
    inline void resize(size_type n, E c) { return Base::resize(n, c); }
/*
    //! reserves n chars and appends sn chars from s
    void resize(size_type n, const_iterator s, size_type sn)
    {
        Base::resize(n, s, sn);
    }
*/
    //! append operations
    inline void append(E c) { return Base::append(c); }
    inline void append(size_type n, E c) { return Base::append(n, c); }
    inline void append(const E* s, size_type n) { return Base::append(s, n); }

    //! assign operations
    inline void assign(size_type n, E c) { return Base::assign(n, c); }
    //! assigns sn chars from s reserving n characters
    void assign(size_type n, const_iterator s, size_type sn)
    {
        Base::assign(n, s, sn);
    }
//! ensure that capacity is at least new_sz
    inline void reserve(size_type new_sz) { Base::reserve(new_sz); }

    //! grows or shrinks Storage
    inline void adjust(size_type new_sz) { Base::adjust(new_sz); }

    inline void swap(DefaultStorage& rhs) { Base::swap(rhs); }

    inline const E* c_str() const { return Base::c_str(); }

    //! return pointer to buffer NOT appending trailing NULL
    inline const E* data() const { return Base::data(); }

    allocator_type get_allocator() const { return Base::get_allocator(); }

};

#if defined(_MSC_VER) && !defined(SS_TEMPLATE_INST)
# pragma warning (push)
# pragma warning (disable: 4231 4251)
extern template class COMMON_IMPORT DefaultStorage<Char>;
extern template class COMMON_IMPORT DefaultStorage<char>;
# pragma warning (pop)
#endif

COMMON_NS_END

#endif
