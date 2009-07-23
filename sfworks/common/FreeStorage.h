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

#ifndef FREE_STORAGE_H_
#define FREE_STORAGE_H_

#include "common/common_defs.h"
#include "StringDecl.h"
#include "CharTraits.h"

#include <string>
#include <memory>
#include <cassert>


#if defined(_MSC_VER)
# pragma warning (push)
# pragma warning (disable: 4661 4251 4231)
#endif

namespace StringPrivate {

typedef unsigned int size_type;

template<typename E> struct Data {
    size_type   capacity_;  // length of allocated memory block
    E           buf_[1];    // pointer to the non-const string
    /**
     * allocates block for string storage
     *
     * @param cap    required string capacity
     *
     * @return pointer to the newly allocated buffer
     */
    void* operator new(std::size_t, size_type capacity)
    {
        // 1 used below to silence g++ which gives a lengthy warning
        const size_type buf_offset((size_type)&((Data*)1)->buf_ - 1);
        return ::operator new(buf_offset + capacity * sizeof(E));
    }
    void operator delete(void* p, size_type)
    {
        ::operator delete(p);
    }
    inline E* buffer() { return buf_; }
    inline size_type capacity() const { return capacity_; }
};

// Holds the actual character string
template<typename E, class A = std::allocator<E> > struct FreeStorageBase {
    typedef StringPrivate::Data<E> Data;
    // (size_ == 0) => string is empty
    // (size_ == npos && pdata_ == 0) => string is null
    // when string is null then c_str() and data() both return (const E*)&char_
    // if size_ == 0 then c_str() and data() return
    // pdata_ ? pdata_->buf_ : (const E*)&char_;
    mutable size_type   size_;
    mutable Data*       pdata_;
    inline E* getChar() const { return reinterpret_cast<E*>(&pdata_); }
};

}

COMMON_NS_BEGIN

template <typename E,
          class T = std::char_traits<E>,
          class A = std::allocator<E> >
class FreeStorage : private StringPrivate::FreeStorageBase<E, A> {
public:
    typedef FreeStorage<E,T,A>                  storage_type;
    typedef typename A::size_type               size_type;
    typedef E                                   value_type;
    typedef E*                                  iterator;
    typedef const E*                            const_iterator;
    typedef A                                   allocator_type;
    typedef T                                   traits_type;
    typedef StringPrivate::FreeStorageBase<E, A> BaseType;
    typedef typename BaseType::Data             Data;
private:
#if defined(DEFINE_TEST_CLASS)
    DEFINE_TEST_CLASS
#endif
    using BaseType::pdata_;
    using BaseType::size_;
    using BaseType::getChar;

    static const size_type npos = static_cast<size_type>(-1);
//    static const Data nullString_;

    // string is null after init
    void init() { pdata_ = 0; set_size(npos); }
    /**
     * Initializes pdata_, Always allocates \a capacity + 1 bytes
     * in order to accomodate trailing null character
     * does not set size
     *
     * @param capacity capacity of new storage
     */
    void alloc(size_type capacity)
    {
        ++capacity;
        pdata_ = new(capacity) Data;
        pdata_->capacity_ = capacity;
    }

    FreeStorage& operator=(const FreeStorage&);

    // Initializes storage so that it holds a copy of \a sz chars pointed to
    // by \a s. Storage capacity will be max(\a cap, \a sz) characters.
    void alloc_copy(const E* s, size_type sz, size_type cap)
    {
        assert(sz <= cap);
        alloc(cap);
        if (sz)
            traits_type::copy(begin(), s, sz);
        set_size(sz);
    }

    void alloc_char(E ch)
    {
        alloc(1);
        new(buffer()) E(ch);
        set_size(1);
    }

    // returns the beginning of buffer of non-const string
    inline E* buffer() { return pdata_ ? pdata_->buffer() : getChar(); }
    inline const E* buffer() const
    {
        return pdata_ ? pdata_->buffer() : getChar();
    }

protected:

    inline void set_size(size_type sz) { size_ = sz; }
    inline bool is_null() const { return npos == size_; }

    void set_null()
    {
        if (!is_null())
            operator delete(pdata_);
        init();
    }

    void set_empty()
    {
        pdata_ = 0;
        size_ = 0;
    }

    void assign(const FreeStorage& other)
    {
        FreeStorage temp(other);
        temp.swap(*this);
    }

public:

    //! Default ctor
    FreeStorage(const A& = A()) { init(); }

    //! Copy ctor
    FreeStorage(const FreeStorage& s)
    {
        if (s.is_null())
            init();
        else {
            const size_type sz = s.size();
            alloc_copy(s.begin(), sz, sz);
        }
    }

    //! Copy ctor with different capacity. Not for const strings
    //! Used for replace/append, when storage growing is needed
    // If \a cap is 0, then construct an empty string.
    FreeStorage(const FreeStorage& s, size_type cap)
    {
        if (0 == cap)
            set_empty();
        else {
            const size_type sz = s.size();
            alloc_copy(s.begin(), sz, cap < sz ? sz : cap);
        }
    }

    /**
     * Ctor from buffer pointed to by \a s of size \a len
     * if 0 == \a s then storage will be null storage
     *
     * @param s      buffer to copy from
     * @param len    length of buffer
     */
    FreeStorage(const E* s, size_type len, const A& = A())
    {
        if (0 == s)
            init();
        else
            alloc_copy(s, len, len);
    }

    /**
     * Ctor from buffer of size \a len and capacity \a cap
     * precondition: \a len <= \a cap
     * if 0 == \a s then storage will be null storage
     *
     * @param s      buffer to copy from
     * @param len    length of buffer
     * @param cap    capacity of new storage
     */
    FreeStorage(const E* s, size_type len, size_type cap, const A& = A())
    {
        if (0 == s)
            init();
        else {
            assert(len <= cap);
            // when len == 0, capacity will be >= 1, so constructing storage
            // from empty string won't result in null string and c_str
            // will work ok w/out reallocating memory
            alloc_copy(s, len, cap);
        }
    }

    /**
     * Ctor from buffer pointed to by \a s
     * if 0 == \a s then storage will be null storage
     *
     * @param s      buffer to copy from
     */
    FreeStorage(const E* s, const A& = A())
    {
        if (0 == s)
            init();
        else {
            const size_type sz = traits_type::length(s);
            alloc_copy(s, sz, sz);
        }
    }

//! Constructors for optimized operator+

    //! precondition: s1 != 0, n1 != 0, s2 != 0, n2 != 0
    FreeStorage(const E* s1, size_type n1, const E* s2, size_type n2,
                const A& = A())
    {
        assert(s1 && s2 && n1 && n2);
        set_size(n1 + n2);
        alloc(size());
        E* const begp = begin();
        traits_type::copy(begp, s1, n1);
        traits_type::copy(begp + n1, s2, n2);
    }

    FreeStorage(E c, const E* s, size_type n, const A& = A())
    {
        if (0 == s)
            alloc_char(c);
        else {
            set_size(n + 1);
            alloc(size());
            E* begp = begin();
            new (begp) E(c);
            traits_type::copy(++begp, s, n);
        }
    }

    FreeStorage(const E* s, size_type n, E c, const A& = A())
    {
        if (0 == s)
            alloc_char(c);
        else {
            set_size(n + 1);
            alloc(size());
            E* const begp = begin();
            traits_type::copy(begp, s, n);
            new (begp + n) E(c);
        }
    }

    FreeStorage(size_type len, size_type cap, E c, const A& = A())
    {
        assert(len <= cap);
        if (0 == cap)
            set_empty();
        else {
            alloc(cap);
            traits_type::assign(buffer(), len, c);
            set_size(len);
        }
    }

    ~FreeStorage()
    {
        if (pdata_)
            operator delete(pdata_);
    }

    iterator begin() { return buffer(); }
    iterator end() { return begin() + size(); }

    const_iterator begin() const { return buffer(); }
    const_iterator end() const { return begin() + size(); }

    size_type size() const { return is_null() ? 0 : size_; }
    // reserve one char for trailing '\0'
    size_type max_size() const { return size_type(-1) - 2; }
    size_type capacity() const { return pdata_ ? pdata_->capacity() : 0; }

    //! grows or shrinks Storage
    void adjust(size_type new_cap)
    {
        if (new_cap != capacity()) {
            const size_type sz(size()), new_sz(new_cap < sz ? new_cap : sz);
            FreeStorage temp(buffer(), new_sz, new_cap);
            temp.swap(*this);
        }
    }

    //! ensure that capacity is at least new_sz
    void reserve(size_type new_sz)// const
    {
        if (new_sz > capacity() || is_null()) {
            FreeStorage temp(*this, new_sz);
            temp.swap(*this);
        }
    }

    //! resize operations
    void resize(size_type n)
    {
        reserve(n);
        set_size(n);
    }

    void resize(size_type n, E c)
    {
        const size_type sz(size());
        reserve(n);
        if (n > sz)
            traits_type::assign(end(), n - sz, c);
        set_size(n);
    }

    //! append operations
    void append(E c)
    {
        const size_type sz = size();
        reserve(sz + 1);
        new(buffer() + sz) E(c);
        set_size(sz + 1);
    }

    void append(size_type n, E c)
    {
        const size_type new_sz = n + size();
        reserve(new_sz);
        if (n) {
            traits_type::assign(end(), n, c);
            set_size(new_sz);
        }
    }

    void append(const E* s, size_type n)
    {
        if (0 != s) {
            const size_type new_sz(n + size());
            reserve(new_sz);
            if (n > 0) {
                traits_type::move(end(), s, n);
                set_size(new_sz);
            }
        }
    }

    //! assign operations
    void assign(size_type n, E c)
    {
        reserve(n);
        traits_type::assign(begin(), n, c);
        set_size(n);
    }

    //! assigns sn chars from s reserving n characters
    //! pre: sn <= n
    void assign(size_type n, const_iterator s, size_type sn)
    {
        assert(sn <= n);
        reserve(n);
        traits_type::move(begin(), s, sn);
        set_size(n);
    }

    void swap(FreeStorage& rhs)
    {
        std::swap(pdata_, rhs.pdata_);
        std::swap(size_, rhs.size_);
    }

    const E* c_str() const
    {
        if (pdata_) {
            const size_type sz(size());
            // can't use reserve because it's const member
            if (pdata_->capacity_ <= sz) {
                FreeStorage temp(*this, sz + 1);
                std::swap(pdata_, temp.pdata_);
                std::swap(size_, temp.size_);
            }
            E* const begp(pdata_->buf_), *const endp(begp + size());
            new(endp) E(0);
            return begp;
        }
        return getChar();
    }

    //! return pointer to buffer NOT appending trailing NULL
    const E* data() const { return begin(); }

    A get_allocator() const { return A(); }
protected:
    static A get_alloc() { return A(); } // workaround for MSVC 7.1 C2064
};

/*
template <typename E, class T, class A>
const typename FreeStorage<E, T, A>::Data
FreeStorage<E, T, A>::nullString_ = { 0, { '\0' } };
*/
#if defined(_MSC_VER)
# if !defined(SS_TEMPLATE_INST)
#  include <QString>

extern template class COMMON_IMPORT FreeStorage<Char>;
extern template class COMMON_IMPORT FreeStorage<char>;
# endif
# pragma warning (pop)
#endif

COMMON_NS_END

#endif // FREE_STORAGE_H_
