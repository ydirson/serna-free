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

#ifndef REFCNT_STORAGE_H_
#define REFCNT_STORAGE_H_

#include "common/common_defs.h"
#include "common/RefCounted.h"
#include "common/StringDecl.h"
#include "common/CharTraits.h"

#include <string>
#include <memory>
#include <cassert>

#if defined(_MSC_VER)
# pragma warning (push)
# pragma warning (disable: 4661 4251 4231)
#endif

namespace StringPrivate {

typedef unsigned int size_type;

template<typename E> struct RefCntData : public COMMON_NS::RefCounted<> {
    size_type   size_;
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
        const size_type buf_offset((size_type)(intptr_t)&((RefCntData*)1)->buf_ - 1);
        return ::operator new(buf_offset + capacity * sizeof(E));
    }
    void operator delete(void* p, size_type)
    {
        ::operator delete(p);
    }
    inline E* buffer() { return buf_; }
    inline E* buffer() const { return buf_; }
    inline size_type size() const { return size_; }
    inline size_type capacity() const { return capacity_; }
};

// Holds the actual character string
template<typename E, class A = std::allocator<E> > struct RefCntStorageBase {
    typedef RefCntData<E> Data;
    mutable union {
        Data* pdata_;
        char  pbuf_[4];
    } pd_;
    inline E* getChar() const { return reinterpret_cast<E*>(pd_.pbuf_); }
};

}

COMMON_NS_BEGIN

template <typename E,
          class T = std::char_traits<E>,
          class A = std::allocator<E> >
class RefCntStorage : private StringPrivate::RefCntStorageBase<E, A> {
public:
    typedef RefCntStorage<E,T,A>       storage_type;
    typedef typename A::size_type               size_type;
    typedef E                                   value_type;
    typedef E*                                  iterator;
    typedef const E*                            const_iterator;
    typedef A                                   allocator_type;
    typedef T                                   traits_type;
    typedef StringPrivate::RefCntStorageBase<E, A> BaseType;
    typedef typename BaseType::Data             Data;
private:
#if defined(DEFINE_TEST_CLASS)
    DEFINE_TEST_CLASS
#endif
    static const size_type npos = static_cast<size_type>(-1);

    using BaseType::pd_;
    using BaseType::getChar;

    // string is null after init
    void init() { pd_.pdata_ = 0; }
    /**
     * Initializes pd_.pdata_, Always allocates \a capacity + 1 bytes
     * in order to accomodate trailing null character
     * does not set size
     *
     * @param capacity capacity of new storage
     */
    void alloc(size_type capacity)
    {
        ++capacity;
        pd_.pdata_ = new(capacity) Data;
        pd_.pdata_->capacity_ = capacity;
        pd_.pdata_->incRefCnt();
    }

    RefCntStorage& operator=(const RefCntStorage&);

    // Initializes storage so that it holds a copy of \a sz chars pointed to
    // by \a s. Storage capacity will be max(\a cap, \a sz) characters.
    void alloc_copy(const E* s, size_type sz, size_type cap)
    {
        assert(sz <= cap);
        alloc(cap);
        if (sz)
            traits_type::copy(buffer(), s, sz);
        set_size(sz);
    }

    void alloc_char(E ch)
    {
        alloc(1);
        new(buffer()) E(ch);
        set_size(1);
    }

    // returns the beginning of buffer of non-const string
    inline E* buffer() { return pd_.pdata_ ? pd_.pdata_->buffer() : getChar(); }
    inline const E* buffer() const
    {
        return pd_.pdata_ ? pd_.pdata_->buffer() : getChar();
    }

    void make_unique()
    {
        if (is_null() || 1 == pd_.pdata_->getRefCnt())
            return;
        RefCntStorage temp(*this, pd_.pdata_->size());
        temp.swap(*this);
    }

    void release()
    {
        if (!is_null() && 0 == pd_.pdata_->decRefCnt())
            operator delete(pd_.pdata_);
    }

protected:

    inline void set_size(size_type sz) { pd_.pdata_->size_ = sz; }
    inline bool is_null() const { return 0 == pd_.pdata_; }

    void set_null()
    {
        release();
        init();
    }

    void set_empty()
    {
        if (is_null()) {
            alloc(0);
            new(buffer()) E(0);
        }
        set_size(0);
    }

    void assign(const RefCntStorage& other)
    {
        if (pd_.pdata_ == other.pd_.pdata_)
            return;
        RefCntStorage temp(other);
        temp.swap(*this);
    }

public:

    //! Default ctor
    RefCntStorage(const A& = A()) { init(); }

    //! Copy ctor
    RefCntStorage(const RefCntStorage& other)
    {
        pd_.pdata_ = other.pd_.pdata_;
        if (pd_.pdata_)
            pd_.pdata_->incRefCnt();
    }

    //! Copy ctor with different capacity. Not for const strings
    //! Used for replace/append, when storage growing is needed
    // If \a cap is 0, then construct an empty string.
    RefCntStorage(const RefCntStorage& s, size_type cap)
    {
        if (0 == cap) {
            init();
            set_empty();
        }
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
    RefCntStorage(const E* s, size_type len, const A& = A())
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
    RefCntStorage(const E* s, size_type len, size_type cap, const A& = A())
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
    RefCntStorage(const E* s, const A& = A())
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
    RefCntStorage(const E* s1, size_type n1, const E* s2, size_type n2,
                const A& = A())
    {
        const unsigned sz = n1 + n2;
        if (0 == sz) {
            init();
            return;
        }
        alloc(sz);
        if (!s1 || !n1) {
            alloc_copy(s2, n2, n2);
            return;
        }
        if (!s2 || !n2) {
            alloc_copy(s1, n1, n1);
            return;
        }
        E* const begp = buffer();
        traits_type::copy(begp, s1, n1);
        traits_type::copy(begp + n1, s2, n2);
        set_size(sz);
    }

    RefCntStorage(E c, const E* s, size_type n, const A& = A())
    {
        if (0 == s)
            alloc_char(c);
        else {
            const unsigned sz = n + 1;
            alloc(sz);
            E* begp = buffer();
            new (begp) E(c);
            traits_type::copy(++begp, s, n);
            set_size(sz);
        }
    }

    RefCntStorage(const E* s, size_type n, E c, const A& = A())
    {
        if (0 == s)
            alloc_char(c);
        else {
            const unsigned sz = n + 1;
            alloc(sz);
            E* const begp = buffer();
            traits_type::copy(begp, s, n);
            new (begp + n) E(c);
            set_size(sz);
        }
    }

    RefCntStorage(size_type len, size_type cap, E c, const A& = A())
    {
        assert(len <= cap);
        if (0 == cap) {
            init();
            set_empty();
        }
        else {
            alloc(cap);
            traits_type::assign(buffer(), len, c);
            set_size(len);
        }
    }

    ~RefCntStorage()
    {
        set_null();
    }

    iterator begin() { make_unique(); return buffer(); }
    iterator end() { make_unique(); return begin() + size(); }

    const_iterator begin() const { return buffer(); }
    const_iterator end() const { return begin() + size(); }

    size_type size() const { return is_null() ? 0 : pd_.pdata_->size(); }
    // reserve one char for trailing '\0'
    size_type max_size() const { return size_type(-1) - 2; }
    size_type capacity() const { return pd_.pdata_ ? pd_.pdata_->capacity() : 0; }

    //! grows or shrinks Storage
    void adjust(size_type new_cap)
    {
        if (new_cap != capacity()) {
            const size_type sz(size()), new_sz(new_cap < sz ? new_cap : sz);
            RefCntStorage temp(buffer(), new_sz, new_cap);
            temp.swap(*this);
        }
    }

    //! ensure that capacity is at least new_sz
    void reserve(size_type new_sz)
    {
        if (is_null() || new_sz > capacity() || pd_.pdata_->getRefCnt() > 1) {
            RefCntStorage temp(*this, new_sz);
            temp.swap(*this);
        }
    }
    //! reserve using the size of nearest power of 2
    void reserve2(size_type new_sz)
    {
        if (is_null() || new_sz > capacity() || pd_.pdata_->getRefCnt() > 1) {
            if (new_sz & (new_sz - 1)) {
                --new_sz;
                for (int i = 1; i < 32; ++i)
                    new_sz |= (new_sz >> i);
                ++new_sz;
            } else
                new_sz <<= 1;
            RefCntStorage temp(*this, new_sz);
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
        reserve2(sz + 1);
        new(buffer() + sz) E(c);
        set_size(sz + 1);
    }

    void append(size_type n, E c)
    {
        const size_type new_sz = n + size();
        reserve2(new_sz);
        if (n) {
            traits_type::assign(end(), n, c);
            set_size(new_sz);
        }
    }

    void append(const E* s, size_type n)
    {
        if (0 != s) {
            const size_type new_sz(n + size());
            reserve2(new_sz);
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

    void swap(RefCntStorage& rhs)
    {
        std::swap(pd_.pdata_, rhs.pd_.pdata_);
    }

    const E* c_str() const
    {
        if (pd_.pdata_) {
            const size_type sz(size());
            if (pd_.pdata_->capacity_ <= sz) {
                RefCntStorage temp(*this, sz + 1);
                std::swap(pd_.pdata_, temp.pd_.pdata_);
            }
            E* const begp(pd_.pdata_->buffer());
            new(begp + sz) E(0);
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

#if defined(_MSC_VER)
# if !defined(SS_TEMPLATE_INST)
# include <QString>

extern template class COMMON_IMPORT RefCntStorage<Char>;
extern template class COMMON_IMPORT RefCntStorage<char>;
# endif
# pragma warning (pop)
#endif

COMMON_NS_END

#endif // REFCNT_STORAGE_H_
