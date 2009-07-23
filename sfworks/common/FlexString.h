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

/*
 * \file The flex_string class is a policy-based implementation of a
 * std::string interface. It's intended to be a compiler/platform independent
 * drop-in replacement for various vendor implementations. Extensions to the
 * std::string interface are implemented as external functions. flex_string
 * supports notion of an uninitialized (null) string.
 *
 * Details of the memory management and ownership are factored out into
 * Storage policy.
 */

#ifndef FLEX_STRING_H_
#define FLEX_STRING_H_

#include "common/common_defs.h"
#include "common/StringDecl.h"
#include "common/CharTraits.h"
#include "common/RefCntStorage.h"
#include "common/IterTraits.h"
#include "common/asserts.h"
#include <string>
#include <iosfwd>

COMMON_NS_BEGIN

template <typename E,
    class T = CHAR_TRAITS_TYPE(E),
    class A = std::allocator<E>,
    class Storage = RefCntStorage<E, T, A> >
class flex_string : protected Storage { // should be private

    INVARIANT_GUARD_DECLARE(flex_string);

public:
    // types
    typedef T traits_type;
    typedef typename traits_type::char_type value_type;
    typedef A allocator_type;
    typedef Storage storage_type;
    typedef unsigned int size_type;
    typedef typename A::difference_type difference_type;

    typedef typename A::reference reference;
    typedef typename A::const_reference const_reference;
    typedef typename A::pointer pointer;
    typedef typename A::const_pointer const_pointer;

    typedef E* iterator;
    typedef const E* const_iterator;
    typedef typename REVERSE_ITERATOR(iterator, value_type) reverse_iterator;
    typedef typename REVERSE_ITERATOR(const_iterator, const value_type)
            const_reverse_iterator;
    static const size_type npos;    // = size_type(-1)

private:
#if defined(DEFINE_TEST_CLASS)
    DEFINE_TEST_CLASS
#endif

    bool inv_check() const;

//private:
protected: // until String becomes typedef of ustring

    using Storage::is_null;
    using Storage::set_null;
    using Storage::set_size;

public:
    // 21.3.1 construct/copy/destroy
    //!
    //! Default constructor. Resulting string is null string
    explicit flex_string(const A& a = A());
    //! Copy constructor. Constructed string is initialized by
    //! str[\a pos, \a pos + \a n)
    flex_string(const flex_string& str, size_type pos = 0, size_type n = npos,
                const A& a = A());
    //! Constructor from null-terminated sequence \a s
    flex_string(const value_type* s, const A& a = A());
    //! Constructor from sequence [\a s, \a s + \a n)
    //! \code flex_string<char> s("abcde", 3); s.c_str() == "abc"; \endcode
    flex_string(const value_type* s, size_type n, const A& a = A());
    //! Constructor from one character repeated \a n times
    //! \code flex_string<char> s(5, 'x'); s.c_str() == "xxxxx"; \endcode
    flex_string(size_type n, value_type c, const A& a = A());
    //! Constructor from sequence designated by pair of input iterators
    //! \code vector<char> v; v.push_back('a');
    //! flex_string<char> s(v.begin(), v.end()); \endcode
    template<typename InputIterator>
    flex_string(InputIterator begin, InputIterator end,
                const A& a = storage_type::get_alloc());
    //!
    ~flex_string();
    //! Assignment operator from other string
    flex_string& operator=(const flex_string& str);
    //! Assignment operator from null-terminated sequence
    flex_string& operator=(const value_type* s);
    //! Assignment from one character
    flex_string& operator=(value_type c);

    // 21.3.2 iterators:
    //! \return iterator that points to the beginning of string
    iterator begin();
    //! \return const iterator that points to the beginning of string
    const_iterator begin() const;
    //! \return iterator that points just beyond the end of string
    iterator end();
    //! \return const iterator that points just beyond the end of string
    const_iterator end() const;
    //! \return reverse iterator that points to the end of string
    reverse_iterator rbegin();
    //! \return const reverse iterator that points to the end of string
    const_reverse_iterator rbegin() const;
    //! \return reverse iterator that points just before the beginning of string
    reverse_iterator rend();
    //! \return cosnt reverse iterator that points just before the beginning
    //! of string
    const_reverse_iterator rend() const;

    // 21.3.3 capacity:
    //! \return the string size
    size_type size() const;
    //! \return the string size
    size_type length() const;
    //! \return the maximum number of characters that a string can hold
    size_type max_size() const;
    //! resizes the string. if \a n greater than old size, the string is padded
    //! by character \a c
    void resize(size_type n, value_type c);
    //! equivalent to \code resize(n, '\0'); \endcode
    void resize(size_type n);
    //! \return size of the buffer allocated for string
    size_type capacity() const;
    //! ensures that capacity() >= \a new_cap
    void reserve(size_type new_cap = 0);
    //! ensures that size() == 0
    void clear();
    //! \return true if and only if size() == 0
    bool empty() const;

    // 21.3.4 element access:
    //! \return const reference to the pos-th element of the string \code
    //! const flex_string<char> s("abcd"); \n s[2] == *(s.begin() + 2); \endcode
    const_reference operator[](size_type pos) const;
    //! \return reference to the pos-th element of the string
    //! \code flex_string<char> s("abcd"); \n s[2] == *(s.begin() + 2); \endcode
    reference operator[](size_type pos);
    //! \return const reference to the n-th element of the string \code
    //! const flex_string<char> s("abcd"); \n s.at(2) == *(s.begin() + 2);
    //! \endcode \throws std::out_of_range if \a n > size()
    const_reference at(size_type n) const;
    //! \return reference to the n-th element of the string \code
    //! flex_string<char> s("abcd"); \n s.at(2) == *(s.begin() + 2); \endcode
    //! \throws std::out_of_range if n > size()
    reference at(size_type n);

    // 21.3.5 modifiers:
    //! appends elements contained in 'str' \code
    //! flex_string<char> s("abcd"), str("ef");
    //! s += str; s.c_str() == "abcdef"; \endcode
    flex_string& operator+=(const flex_string& str);
    //! appends elements pointed to by null-terminated sequence \a s \code
    //! flex_string<char> s("abcd"); const char* p = "ef";
    //! s += p; s.c_str() == "abcdef"; \endcode
    flex_string& operator+=(const value_type* s);
    //! appends one character \code
    //! flex_string<char> s("abcd");
    //! s += 'e'; s.c_str() == "abcde"; \endcode
    flex_string& operator+=(value_type c);

    // append
    //! equivalent to the \code operator+=(\a str) \endcode
    flex_string& append(const flex_string& str);
    //! appends \a n chars from \a str starting at \a pos
    flex_string& append(const flex_string& str, size_type pos, size_type n);
    //! appends \a n chars from a sequence pointed to by \a s
    flex_string& append(const value_type* s, size_type n);
    //! appends T::length(\a s) chars from a sequence pointed to by \a s
    flex_string& append(const value_type* s);
    //! appends \a n \a c characters \code
    //! flex_string<char> s("ab");
    //! s.append(3, 'x'); \n s.c_str() == "abxxx"; \endcode
    flex_string& append(size_type n, value_type c);
    //! appends \a c character, allows std::back_inserter<E> to be used \code
    //! std::istreambuf_iterator start(cin), end; \n flex_string<char> s;
    //! std::copy(start, end, back_inserter<char>(s)); \endcode
    //! inserts characters from cin into \a s until EOF is reached
    void push_back(E c);
    //! appends sequence of elements designated by [\a first, \a last)
    template<typename InputIterator>
    flex_string& append(InputIterator first, InputIterator last);

    // assign
    //! replaces contents of the string by contents of \a str
    flex_string& assign(const flex_string& str);
    //! replaces contents of the string by \a n chars from \a str starting at
    //! position \a pos
    flex_string& assign(const flex_string& str, size_type pos, size_type n);
    //! replaces contents of the string by \a n chars from sequence pointed to
    //! \a s
    flex_string& assign(const value_type* s, size_type n);
    //! replaces contents of the string by T::length(\a s) chars from sequence
    //! pointed to \a s
    flex_string& assign(const value_type* s);
    //! replaces contents of the string by \a n characters \a c
    flex_string& assign(size_type n, value_type c);
    //! replaces contents of the string by copy of sequence designated by
    //! pair of input iterators [\a first, \a last)
    template<typename InputIterator>
    flex_string& assign(InputIterator first, InputIterator last);

    // insert
    //! inserts contents of the string \a str at position \a pos
    flex_string& insert(size_type pos, const flex_string& str);
    //! \a n characters of \a str starting at \a pos2 will be inserted at \a pos
    //! \code flex_string<char> s("abcd"), str("1234");
    //! s.insert(1, str, 2, 2); s.c_str() == "a34bcd"; \endcode
    //! \throws std::out_of_range if \a pos1 > size() || \a pos2 > str.size()
    flex_string& insert(size_type pos1, const flex_string& str, size_type pos2,
                        size_type n);
    //! inserts \a n characters pointed to by \a s at position \a pos
    //! \throws std::out_of_range if \a pos > size()
    flex_string& insert(size_type pos, const value_type* s, size_type n);
    //! inserts T::length(\a s) characters pointed to by null-terminated
    //! sequence pointed to by \a s at position \a pos
    //! \throws std::out_of_range if \a pos > size()
    flex_string& insert(size_type pos, const value_type* s);
    //! inserts \a n characters \a c at position \a pos
    //! \throws std::out_of_range if \a pos > size()
    flex_string& insert(size_type pos, size_type n, value_type c);
    //! equivalent to \code insert(p - begin(), 1, c) \endcode
    iterator insert(iterator p, value_type c = value_type());
    //! equivalent to \code insert(p - begin(), n, c) \endcode
    void insert(iterator p, size_type n, value_type c);
    //! inserts copy of sequence designated by [\a first, \a last) at position
    //! corresponding to iterator \a p
    template<typename InputIterator>
    void insert(iterator p, InputIterator first, InputIterator last);

    // erase
    //! removes \a n characters starting at position \a pos
    //! \throws std::out_of_range if \a pos > size()
    flex_string& erase(size_type pos = 0, size_type n = npos);
    //! equivalent to \code erase(pos - begin(), 1); \endcode
    //! \return \a pos
    iterator erase(iterator pos);
    //! erases characters in range [\a first, \a last)
    //! \return \a first
    iterator erase(iterator first, iterator last);

    // replace
    //! replaces \a n chars starting at position \a pos by contents of \a str
    //! \throws std::out_of_range if \a pos > size()
    flex_string& replace(size_type pos, size_type n, const flex_string& str);
    //! replaces \a n1 chars starting at position \a pos1 by sequence of chars
    //! from range str[\a pos2, \a pos2 + \a n2)
    //! \throws std::out_of_range if \a pos1 > size() || \a pos2 > str.size()
    flex_string& replace(size_type pos1, size_type n1, const flex_string& str,
                         size_type pos2, size_type n2);
    //! replaces \a n1 chars starting at position \a pos by sequence of chars
    //! from range [\a s, \a s + \a n2)
    //! \throws std::out_of_range if \a pos > size()
    flex_string& replace(size_type pos, size_type n1, const value_type* s,
                         size_type n2);
    //! replaces \a n chars starting at position \a pos by T::length(\a s) chars
    //! from null-terminated sequence pointed to by \a s
    //! \throws std::out_of_range if \a pos > size()
    flex_string& replace(size_type pos, size_type n, const value_type* s);
    //! replaces \a n1 chars starting at position \a pos by \a n2 chars \a c
    //! \throws std::out_of_range if \a pos > size()
    flex_string& replace(size_type pos, size_type n1, size_type n2,
                         value_type c);
    //! replaces sequence of chars designated by [\a i1, \a i2) by contents of
    //! the string \a str
    flex_string& replace(iterator i1, iterator i2, const flex_string& str);
    //! replaces sequence of chars designated by [\a i1, \a i2) by sequence of
    //! chars from range [\a s, \a s + \a n)
    flex_string& replace(iterator i1, iterator i2, const value_type* s,
                         size_type n);
    //! replaces sequence of chars designated by [\a i1, \a i2) by
    //! T::length(\a s) chars from null-terminated sequence pointed by
    //! \a s
    flex_string& replace(iterator i1, iterator i2, const value_type* s);
    //! replaces sequence of chars designated by [\a i1, \a i2) by
    //! \a n characters \a c
    flex_string& replace(iterator i1, iterator i2, size_type n, value_type c);
    //! replaces sequence of chars designated by [\a i1, \a i2) by
    //! sequence designated by [\a j1, \a j2)
    template<typename InputIterator>
    flex_string& replace(iterator i1, iterator i2, InputIterator j1,
                         InputIterator j2);
    //! copies \a n characters starting at position \a pos into the buffer
    //! pointed to by \a s
    size_type copy(value_type* s, size_type n, size_type pos = 0) const;
    //! swaps contents of the string with contents of \a rhs
    void swap(flex_string& rhs);

    // 21.3.6 string operations:
    //! \return const pointer to the null-terminated sequence that is equal to
    //! the sequence controlled by string
    const value_type* c_str() const;
    //! \return const pointer to the sequence that is equal to the sequence
    //! controlled by string. sequence doesn't have to be null-terminated
    const value_type* data() const;
    //! \return the allocator object used to allocate storage for the string
    allocator_type get_allocator() const;

    // find operations
    //! finds the occurrence of \a str. search starts from position \a pos
    //! \return position where \a str occurs or \b npos if search fails
    size_type find(const flex_string& str, size_type pos = 0) const;
    //! finds the occurrence of sequence [\a s, \a s + \a n). search starts
    //! from position \a pos. \return position where \a str occurs or \b npos if
    //! search fails
    size_type find(const value_type* s, size_type pos, size_type n) const;
    //! finds the occurrence of sequence [\a s, \a s + \a T::length(\a s)).
    //! search starts from position \a pos. \return position where \a str occurs
    //! or \b npos if search fails
    size_type find(const value_type* s, size_type pos = 0) const;
    //! finds occurrence of char \a c, starting from position \a pos
    size_type find(value_type c, size_type pos = 0) const;
    //! finds backward for the occurrence of \a str. search starts from position
    //! \a pos \return position where \a str occurs or \b npos if search fails
    size_type rfind(const flex_string& str, size_type pos = npos) const;
    //! finds backward the occurrence of sequence [\a s, \a s + \a n). search
    //! starts from position \a pos. \return position where \a str occurs or
    //! \b npos if search fails
    size_type rfind(const value_type* s, size_type pos, size_type n) const;
    //! finds backward the occurrence of sequence
    //! [\a s, \a s + \a T::length(\a s)). search starts from position \a pos.
    //! \return position where \a str occurs or \b npos if search fails
    size_type rfind(const value_type* s, size_type pos = npos) const;
    //! finds backward occurrence of char \a c, starting from position \a pos
    size_type rfind(value_type c, size_type pos = npos) const;
    //! finds the first occurrence of any character from \a str.
    //! search starts from position \a pos returns position where char occurs
    //! or \b npos if search fails
    size_type find_first_of(const flex_string& str, size_type pos = 0) const;
    //! finds the first occurrence of any character from [\a s, \a s + \a n).
    //! search starts from position \a pos. \return position where char occurs
    //! or \b npos if search fails
    size_type find_first_of(const value_type* s, size_type pos,
                            size_type n) const;
    //! finds the first occurrence of any character from sequence
    //! [\a s, \a s + \a T::length(\a s)). search starts from position \a pos.
    //! \return position where char occurs or \b npos if search fails
    size_type find_first_of(const value_type* s, size_type pos = 0) const;
    //! finds the first occurrence of char \a c, starting from \a pos
    size_type find_first_of(value_type c, size_type pos = 0) const;
    //! finds the last occurrence of any char from \a str
    //! search starts from position \a pos \return position where char occurs
    //! or \b npos if search fails
    size_type find_last_of(const flex_string& str, size_type pos = npos) const;
    //! finds the last occurrence of any char from sequence [\a s, \a s + \a n).
    //! search starts from position \a pos. \return position where char occurs
    //! or \b npos if search fails
    size_type find_last_of(const value_type* s, size_type pos,
                           size_type n) const;
    //! finds the last occurrence of any char from sequence
    //! [\a s, \a s + \a T::length(\a s))
    //! search starts from position \a pos. \return position where char occurs
    //! or \b npos if search fails
    size_type find_last_of(const value_type* s, size_type pos = npos) const;
    //! finds the last occurrence of char \a c, starting from \a pos
    size_type find_last_of(value_type c, size_type pos = npos) const;
    //! finds the first occurrence of char not from \a str. search starts from
    //! position \a pos \return position where char occurs or \b npos otherwise
    size_type find_first_not_of(const flex_string& str,
                                size_type pos = 0) const;
    //! finds the first occurrence of any char not from sequence
    //! [\a s, \a s + \a n).
    //! search starts from position \a pos. \return position where char occurs
    //! or \b npos if search fails
    size_type find_first_not_of(const value_type* s, size_type pos,
                                size_type n) const;
    //! finds the first occurrence of any character not from sequence
    //! [\a s, \a s + \a T::length(\a s)). search starts from position \a pos.
    //! \return position where char occurs or \b npos if search fails
    size_type find_first_not_of(const value_type* s, size_type pos = 0) const;
    //! finds the first occurrence of char that's not \a c, starting from \a pos
    size_type find_first_not_of(value_type c, size_type pos = 0) const;
    //! finds the last occurrence of char not from \a str. search starts from
    //! position \a pos \return position where char occurs or \b npos otherwise
    size_type find_last_not_of(const flex_string& str,
                               size_type pos = npos) const;
    //! finds the last occurrence of any char not from sequence
    //! [\a s, \a s + \a n).
    //! search starts from position \a pos \return position where char occurs
    //! or \b npos if search fails
    size_type find_last_not_of(const value_type* s, size_type pos,
                               size_type n) const;
    //! finds the last occurrence of any character not from sequence
    //! [\a s, \a s + \a T::length(\a s)). search starts from position \a pos.
    //! \return position where char occurs or \b npos if search fails
    size_type find_last_not_of(const value_type* s, size_type pos = npos) const;
    //! finds the last occurrence of char that's not \a c, starting from \a pos
    size_type find_last_not_of(value_type c, size_type pos = npos) const;
    //! returns copy of \a n characters starting from position \a pos
    //! \throws std::out_of_range if \a pos > size()
    flex_string substr(size_type pos = 0, size_type n = npos) const;

    // comparison operations
    //! \return x such that x < 0 if \a str is greater than *this,
    //! x == 0 if \a str is equal to *this, and x > 0 if \a str is less than
    //! *this
    int compare(const flex_string& str) const;
    //! equivalent to
    //! \code flex_string(*this, pos, n).compare(str); \endcode
    int compare(size_type pos, size_type n, const flex_string& str) const;
    //! equivalent to \code
    //! flex_string(*this, pos1, n1).compare(flex_string(s, n2));
    //! \endcode
    int compare(size_type pos1, size_type n1, const value_type* s,
                size_type n2) const;
    //! equivalent to \code
    //! flex_string(*this, pos1, n1).compare(flex_string(str, pos2, n2));
    //! \endcode
    int compare(size_type pos1, size_type n1, const flex_string& str,
                size_type pos2, size_type n2) const;
    //! equivalent to \code compare(flex_string(s)); \endcode
    int compare(const value_type* s) const;

    //
    //! returns string that's concatenation of *this and \a rhs
    flex_string operator+ (const flex_string& rhs) const;

//  the following is public because g++ 2.95.3 has a hard time figuring out
//  correct friend template declarations
public:
//private:
    // constructors for RVO-capable operator+
    //!
    flex_string(const_pointer p1, size_type s1, const_pointer p2, size_type s2);
    //!
    flex_string(value_type c, const_pointer p2, size_type s2);
    //!
    flex_string(const_pointer p2, size_type s2, value_type c);

private:
    // implementation details

    template<typename InpIter> void
    Assign(InpIter first, InpIter last, StringPrivate::InpIterTag);

    template<typename InpIter> void
    Assign(InpIter first, InpIter last, StringPrivate::RndIterTag);

    template<typename InpIter> void
    Assign(InpIter first, InpIter last, StringPrivate::MBNIterTag);

    template<typename InpIter> void
    Append(InpIter first, InpIter last, StringPrivate::RndIterTag);

    template<typename InpIter> void
    Append(InpIter first, InpIter last, StringPrivate::MBNIterTag);

    template<typename InpIter> void
    Append(InpIter first, InpIter last, StringPrivate::InpIterTag);

    template<typename RndIt> void
    Replace(size_type pos, size_type n, RndIt begin, RndIt end);

    template<typename InpIter> void
    Replace(iterator i1, iterator i2, InpIter j1, InpIter j2,
            StringPrivate::InpIterTag);

    template<typename InpIter> void
    Replace(iterator i1, iterator i2, InpIter j1, InpIter j2,
            StringPrivate::RndIterTag);

    template<typename InpIter> void
    Replace(iterator i1, iterator i2, InpIter j1, InpIter j2,
            StringPrivate::IntIterTag);

    static int Compare(const value_type* s1, size_type n1,
                       const value_type* s2, size_type n2);

};

// non-member functions
//! equivalent to flex_string(lhs) + rhs
template <typename E, class T, class A, class S>
flex_string<E, T, A, S>
operator+(const typename flex_string<E, T, A, S>::value_type* lhs,
          const flex_string<E, T, A, S>& rhs)
{
    if (0 == lhs)
        return rhs;
    const unsigned lhs_sz = flex_string<E, T, A, S>::traits_type::length(lhs);
    if (0 == rhs.size())
        return flex_string<E, T, A, S>(lhs, lhs_sz);
    return flex_string<E, T, A, S>(lhs, lhs_sz, rhs.data(), rhs.size());
}
//! equivalent to flex_string(1, lhs) + rhs
template <typename E, class T, class A, class S>
flex_string<E, T, A, S>
operator+(typename flex_string<E, T, A, S>::value_type lhs,
          const flex_string<E, T, A, S>& rhs)
{
    return flex_string<E, T, A, S>(lhs, rhs.data(), rhs.size());
}
//! equivalent to lhs + flex_string(rhs)
template <typename E, class T, class A, class S>
flex_string<E, T, A, S>
operator+(const flex_string<E, T, A, S>& lhs,
          const typename flex_string<E, T, A, S>::value_type* rhs)
{
    if (0 == rhs)
        return lhs;
    const unsigned rhs_sz = flex_string<E, T, A, S>::traits_type::length(rhs);
    if (0 == lhs.size())
        return flex_string<E, T, A, S>(rhs, rhs_sz);
    return flex_string<E, T, A, S>(lhs.data(), lhs.size(), rhs, rhs_sz);
}
//! equivalent to lhs + flex_string(1, rhs)
template <typename E, class T, class A, class S>
flex_string<E, T, A, S>
operator+(const flex_string<E, T, A, S>& lhs,
          typename flex_string<E, T, A, S>::value_type rhs)
{
    return flex_string<E, T, A, S>(lhs.data(), lhs.size(), rhs);
}

// -- comparison operators -- //

template<typename E, class T, class A, class S> bool
operator== (const flex_string<E, T, A, S>& lhs,
            const flex_string<E, T, A, S>& rhs)
{
    return 0 == lhs.compare(rhs);
}

template<typename E, class T, class A, class S> bool
operator!= (const flex_string<E, T, A, S>& lhs,
            const flex_string<E, T, A, S>& rhs)
{
    return 0 != lhs.compare(rhs);
}

template<typename E, class T, class A, class S> bool
operator> (const flex_string<E, T, A, S>& lhs,
           const flex_string<E, T, A, S>& rhs)
{
    return lhs.compare(rhs) > 0;
}

template<typename E, class T, class A, class S> bool
operator>= (const flex_string<E, T, A, S>& lhs,
            const flex_string<E, T, A, S>& rhs)
{
    return lhs.compare(rhs) >= 0;
}

template<typename E, class T, class A, class S> bool
operator< (const flex_string<E, T, A, S>& lhs,
           const flex_string<E, T, A, S>& rhs)
{
    return lhs.compare(rhs) < 0;
}

template<typename E, class T, class A, class S> bool
operator<= (const flex_string<E, T, A, S>& lhs,
            const flex_string<E, T, A, S>& rhs)
{
    return lhs.compare(rhs) <= 0;
}

// subclause 21.3.7.8:
template <typename E, class T, class A, class S>
void swap(flex_string<E, T, A, S>& lhs, flex_string<E, T, A, S>& rhs)
{
    lhs.swap(rhs);
}

COMMON_EXPIMP std::ostream& operator << (std::ostream& os, const ustring& s);
COMMON_EXPIMP std::istream& operator >> (std::istream& os, ustring& s);
//! reads characters from \a is into \a s until \a delim is encountered
//! \a delim is read but is not stored into \a s
COMMON_EXPIMP std::istream& getline(std::istream& is, ustring& s, char delim);
//! equivalent to getline(\a is, \a s, '\n')
COMMON_EXPIMP std::istream& getline(std::istream& is, ustring& s);

COMMON_EXPIMP std::ostream& operator << (std::ostream& os, const nstring& s);
COMMON_EXPIMP std::istream& operator >> (std::istream& os, nstring& s);
//! reads characters from \a is into \a s until \a delim is encountered
//! \a delim is read but is not stored into \a s
COMMON_EXPIMP std::istream& getline(std::istream& is, nstring& s, char delim);
//! equivalent to getline(\a is, \a s, '\n')
COMMON_EXPIMP std::istream& getline(std::istream& is, nstring& s);

template <typename E, class T, class A, class S>
const typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::npos = (typename flex_string<E, T, A, S>::size_type)(-1);

#if defined(_MSC_VER) && !defined(FS_TEMPLATE_INST)
#pragma warning (push)
# pragma warning (disable: 4231)
extern template class COMMON_IMPORT flex_string<Char>;
extern template class COMMON_IMPORT flex_string<char>;
#pragma warning (pop)
#endif

COMMON_NS_END

#include "FlexString.i"

#undef REVERSE_ITERATOR
#ifdef NO_ITERATOR_TRAITS
# undef NO_ITERATOR_TRAITS
#endif

#endif // _FLEX_STRING_H
