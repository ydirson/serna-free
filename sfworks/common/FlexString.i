///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////

#include <memory>
#include "common/IterTraits.h"

namespace StringPrivate {

template<typename Ch, bool invert = false>
struct InSeq {
    InSeq(const Ch* f, unsigned l) : first_(f), len_(l) {}
    static InSeq make(const Ch* f, unsigned l) { InSeq x(f, l); return x; }
    inline operator bool() const { return 0 != first_ && 0 < len_; }
    inline bool operator()(const Ch& c) const
    {
        for (unsigned i = 0; i < len_; ++i)
            if (CHAR_TRAITS_TYPE(Ch)::eq(first_[i], c))
                return true && !invert;
        return false || invert;
    }
private:
    const Ch* first_;
    unsigned len_;
};

template<typename Ch, bool invert = false>
struct Eq {
    Eq(Ch c) : ch_(c) {}
    static Eq make(Ch c) { Eq x(c); return x; }
    inline operator bool() const { return true; }
    inline bool operator()(Ch c) const
    { return CHAR_TRAITS_TYPE(Ch)::eq(ch_, c) ? !invert : invert; }
private:
    const Ch ch_;
};

template<typename Iter, class UnaryOp> unsigned
find_first_if(Iter first, unsigned pos, unsigned len, UnaryOp op, unsigned d)
{
    if (pos < len && op)
        for (Iter i = first + pos; unsigned(i - first) != len; ++i)
            if (op(*i))
                return i - first;
    return d;
}

template<typename Iter, class UnaryOp> unsigned
find_last_if(Iter first, unsigned pos, unsigned len, UnaryOp op, unsigned d)
{
    if (len-- > 0 && op) {
        pos = pos < len ? pos : len;
        Iter i(first + pos), rend(first);
        for (--rend; i != rend; --i)
            if (op(*i))
                return i - first;
    }
    return d;
}

COMMON_EXPIMP void throw_length_error();
COMMON_EXPIMP void throw_out_of_range();

inline void check_length(bool condition)
{
    if (!condition)
        throw_length_error();
}

inline void check_range(bool condition)
{
    if (!condition)
        throw_out_of_range();
}

inline size_type min_size(size_type lhs, size_type rhs)
{
    return lhs < rhs ? lhs : rhs;
}

template<typename InIt> static
inline size_type Distance(InIt first, InIt, IntIterTag)
{
    return size_type(first);
}

template<typename InIt> static
inline size_type Distance(InIt first, InIt last, RndIterTag)
{
    return last - first;
}

template<typename InIt> static
inline size_type Distance(InIt first, InIt last)
{
    return Distance(first, last, IterTag(first));
}

template<typename InIt, typename E> static
inline E* Copy(InIt first, InIt last, E* dest, IntIterTag)
{
    const size_type n = static_cast<size_type>(first);
    std::uninitialized_fill_n(dest, n, E(last));
    return dest + n;
}

template<typename InIt, typename E> static
inline E* Copy(InIt first, InIt last, E* dest, RndIterTag)
{
    return std::uninitialized_copy(first, last, dest);
}

template<typename InIt, typename E> static
inline E* Copy(InIt first, InIt last, E* dest)
{
    return Copy(first, last, dest, IterTag(first));
}

template<typename InIt, typename E> static
inline bool Overlap(InIt first, InIt last, E* f2, E* l2, RndIterTag)
{
    const E* f1 = reinterpret_cast<const E*>(&(*first));
    const E* l1 = reinterpret_cast<const E*>(&(*last));
    return (f1 < f2 && l1 > f2) || (f1 < l2 && l1 > l2);
}

template<typename InIt, typename E> static
inline bool Overlap(InIt, InIt, E*, E*, IntIterTag)
{
    return false;
}

}

COMMON_NS_BEGIN

#if defined(__GNUG__) && (__GNUC__ < 3)
# define SPVT StringPrivate
#else
namespace SPVT = StringPrivate;
#endif

///////////////////////////////////////////////////////////////////////
// flex_string member functions implementation
///////////////////////////////////////////////////////////////////////

template<typename E, class T, class A, class S> bool
flex_string<E, T, A, S>::inv_check() const
{
    bool inv = size() <= capacity();
    return inv && 0 != begin();
}

template<typename E, class T, class A, class S>
flex_string<E, T, A, S>::flex_string(const A& a)
 :  storage_type(a)
{
    INVARIANT_CHECK(inv_check);
}

template<typename E, class T, class A, class S>
flex_string<E, T, A, S>::flex_string(const flex_string& str, size_type pos,
                                     size_type n, const A& a)
 :  storage_type(a)
{
    INVARIANT_GUARD(inv_check);
    const unsigned sz = str.size();
    SPVT::check_range(pos <= sz);
    if (!(str.is_null())) {
        if (0 == pos && n >= sz)
            storage_type::assign(str);
        else
            assign(str, pos, n);
    }
}

template<typename E, class T, class A, class S>
flex_string<E, T, A, S>::flex_string(const value_type* s, const A& a)
 :  storage_type(s, a)
{
    INVARIANT_CHECK(inv_check);
}

template<typename E, class T, class A, class S>
flex_string<E, T, A, S>::flex_string(const value_type* s, size_type n,
                                     const A& a)
 :  storage_type(static_cast<const E*>(s),
                 static_cast<typename A::size_type>(n),
                 static_cast<typename A::size_type>(n),
                 a)
{
    INVARIANT_CHECK(inv_check);
}

template<typename E, class T, class A, class S>
flex_string<E, T, A, S>::flex_string(size_type n, value_type c, const A& a)
 :  storage_type(n, n, c, a)
{
    INVARIANT_CHECK(inv_check);
}

template<typename E, class T, class A, class S>
template<typename InputIterator>
flex_string<E, T, A, S>::flex_string(InputIterator begin, InputIterator end,
                                     const A& a)
 :  storage_type(a)
{
    assign(begin, end);
}

template<typename E, class T, class A, class S>
flex_string<E, T, A, S>::~flex_string()
{}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::operator=(const flex_string& str)
{
    if (&str != this)
        storage_type::assign(str);
    return *this;
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::operator=(const value_type* s)
{
    flex_string(s).swap(*this);
    return *this;
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::operator=(value_type c)
{
    INVARIANT_GUARD(inv_check);
    assign(&c, 1);
    return *this;
}

// 21.3.2 iterators:
template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::iterator
flex_string<E, T, A, S>::begin()
{
    return storage_type::begin();
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::const_iterator
flex_string<E, T, A, S>::begin() const
{
    return storage_type::begin();
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::iterator
flex_string<E, T, A, S>::end()
{
    return storage_type::end();
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::const_iterator
flex_string<E, T, A, S>::end() const
{
    return storage_type::end();
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::reverse_iterator
flex_string<E, T, A, S>::rbegin()
{
    return reverse_iterator(end());
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::const_reverse_iterator
flex_string<E, T, A, S>::rbegin() const
{
    return const_reverse_iterator(end());
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::reverse_iterator
flex_string<E, T, A, S>::rend()
{
    return reverse_iterator(begin());
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::const_reverse_iterator
flex_string<E, T, A, S>::rend() const
{
    return const_reverse_iterator(begin());
}

// 21.3.3 capacity:
template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::size() const
{
    return storage_type::size();
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::length() const
{
    return size();
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::max_size() const
{
    return storage_type::max_size();
}

template<typename E, class T, class A, class S> void
flex_string<E, T, A, S>::resize(size_type n, value_type c)
{
    storage_type::resize(n, c);
}

template<typename E, class T, class A, class S> void
flex_string<E, T, A, S>::resize(size_type n)
{
    resize(n, E(0));
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::capacity() const
{
    return storage_type::capacity();
}

template<typename E, class T, class A, class S> void
flex_string<E, T, A, S>::reserve(size_type res_arg)
{
    SPVT::check_length(res_arg <= max_size());
    storage_type::reserve(res_arg);
}

template<typename E, class T, class A, class S> void
flex_string<E, T, A, S>::clear()
{
    resize(0);
}

template<typename E, class T, class A, class S> bool
flex_string<E, T, A, S>::empty() const
{
    return size() == 0;
}

// 21.3.4 element access:
template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::const_reference
flex_string<E, T, A, S>::operator[](size_type pos) const
{
    return *(begin() + pos);
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::reference
flex_string<E, T, A, S>::operator[](size_type pos)
{
    return *(begin() + pos);
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::const_reference
flex_string<E, T, A, S>::at(size_type n) const
{
    SPVT::check_range(n < size());
    return this->operator[](n);
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::reference
flex_string<E, T, A, S>::at(size_type n)
{
    SPVT::check_range(n < size());
    return this->operator[](n);
}

// 21.3.5 modifiers:
// operator +=
template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::operator+=(const flex_string& str)
{
    return append(str);
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::operator+=(const value_type* s)
{
    return append(s);
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::operator+=(value_type c)
{
    storage_type::append(c);
    return *this;
}

// append
template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::append(const flex_string& str)
{
    if (str.is_null())
        return *this;
    return append(str.data(), str.size());
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::append(const flex_string& str, size_type pos,
                                size_type n)
{
    if (str.is_null())
        return *this;
    const size_type sz = str.size();
    SPVT::check_range(pos <= sz);
    const int n1 = sz - pos;
    return append(str.data() + pos, SPVT::min_size(n, n1));
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::append(const value_type* s, size_type n)
{
    INVARIANT_GUARD(inv_check);
    if (0 == s)
        return *this;
    storage_type::append(s, n);
    return *this;
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::append(const value_type* s)
{
    if (0 == s)
        return *this;
    return append(s, traits_type::length(s));
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::append(size_type n, value_type c)
{
    storage_type::append(n, c);
    return *this;
}

template<typename E, class T, class A, class S> void
flex_string<E, T, A, S>::push_back(E c)
{
    storage_type::append(c);
}

template<typename E, class T, class A, class S>
template<typename InputIterator> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::append(InputIterator first, InputIterator last)
{
    Append(first, last, SPVT::IterTag(first));
    return *this;
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::assign(const flex_string<E, T, A, S>& str)
{
    if (str.is_null()) {
        set_null();
        return *this;
    }
    return assign(str.data(), str.size());
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::assign(const flex_string<E, T, A, S>& str,
                                size_type pos, size_type n)
{
    const size_type sz = str.size();
    SPVT::check_range(pos <= sz);
    const int n1 = sz - pos;
    return assign(str.data() + pos, SPVT::min_size(n, n1));
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::assign(const value_type* s, size_type n)
{
    if (0 == s)
        set_null();
    else
        storage_type::assign(n, s, n);
    return *this;
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::assign(const value_type* s)
{
    if (0 == s) {
        set_null();
        return *this;
    }
    else
        return assign(s, traits_type::length(s));
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::assign(size_type n, value_type c)
{
    storage_type::assign(n, c);
    return *this;
}

template<typename E, class T, class A, class S>
template<typename InputIterator> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::assign(InputIterator first, InputIterator last)
{
    Assign(first, last, SPVT::IterTag(first));
    return *this;
}

// insert
template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::insert(size_type pos,
                                const flex_string<E, T, A, S>& str)
{
    return insert(pos, str, 0, npos);
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::insert(size_type pos1,
                                const flex_string<E, T, A, S>& str,
                                size_type pos2, size_type n)
{
    const size_type sz = str.size();
    SPVT::check_range(pos2 <= sz);
    return insert(pos1, str.data() + pos2, SPVT::min_size(n, sz - pos2));
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::insert(size_type pos, const value_type* s, size_type n)
{
    if (n > 0) {
        const size_type sz = size();
        SPVT::check_range(pos <= sz);
        replace(pos, 0, s, n);
    }
    return *this;
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::insert(size_type pos, const value_type* s)
{
    return insert(pos, s, traits_type::length(s));
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::insert(size_type pos, size_type n, value_type c)
{
    return replace(pos, 0, n, c);
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::iterator
flex_string<E, T, A, S>::insert(iterator p, value_type c)
{
    const size_type pos = p - begin();
    insert(pos, &c, 1);
    return begin() + pos;
}

template<typename E, class T, class A, class S> void
flex_string<E, T, A, S>::insert(iterator p, size_type n, value_type c)
{
    insert(p - begin(), n, c);
}

template<typename E, class T, class A, class S>
template<typename InputIterator> void
flex_string<E, T, A, S>::insert(iterator p, InputIterator first,
                                InputIterator last)
{
    replace(p, p, first, last);
}

// erase
template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::erase(size_type pos, size_type n)
{
    if (0 == n)
        return *this;
    const size_type sz = size();
    SPVT::check_range(pos <= sz);
    const size_type shrink = SPVT::min_size(sz - pos, n);
    const size_type rem_pos = pos + shrink;
    if (rem_pos < sz) {
        iterator erp = begin() + pos;
        traits_type::move(erp, erp + shrink, sz - rem_pos);
    }
    resize(sz - shrink);
    return *this;
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::iterator
flex_string<E, T, A, S>::erase(iterator position)
{
    const size_type pos(position - begin());
    erase(pos, 1);
    return begin() + pos;
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::iterator
flex_string<E, T, A, S>::erase(iterator first, iterator last)
{
    const size_type pos(first - begin());
    erase(pos, last - first);
    return begin() + pos;
}

// replace
template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::replace(size_type pos1, size_type n1,
                                 const flex_string<E, T, A, S>& str)
{
    return replace(pos1, n1, str.data(), str.size());
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::replace(size_type pos1, size_type n1,
                                 const flex_string<E, T, A, S>& str,
                                 size_type pos2, size_type n2)
{
    SPVT::check_range(pos2 <= str.length());
    return replace(pos1, n1, str.data() + pos2,
                   SPVT::min_size(n2, str.size() - pos2));
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::replace(size_type pos, size_type n1,
                                 const value_type* s, size_type n2)
{
    Replace(pos, n1, s, s + n2);
    return *this;
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::replace(size_type pos, size_type n,
                                 const value_type* s)
{
    return replace(pos, n, s, traits_type::length(s));
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::replace(size_type pos, size_type n1, size_type n2,
                                 value_type c)
{
    Replace(pos, n1, n2, size_type(T::to_int_type(c)));
    return *this;
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::replace(iterator i1, iterator i2,
                                 const flex_string<E, T, A, S>& str)
{
    return replace(i1 - begin(), i2 - i1, str.data(), str.size());
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::replace(iterator i1, iterator i2, const value_type* s,
                                 size_type n)
{
    return replace(i1 - begin(), i2 - i1, s, n);
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::replace(iterator i1, iterator i2, const value_type* s)
{
    return replace(i1 - begin(), i2 - i1, s, traits_type::length(s));
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::replace(iterator i1, iterator i2,
                                 size_type n, value_type c)
{
    return replace(i1 - begin(), i2 - i1, n, c);
}

template<typename E, class T, class A, class S>
template<typename InputIterator> flex_string<E, T, A, S>&
flex_string<E, T, A, S>::replace(iterator i1, iterator i2,
                                 InputIterator j1, InputIterator j2)
{
    Replace(i1, i2, j1, j2, SPVT::IterTag(j1));
    return *this;
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::copy(value_type* s, size_type n,
                              size_type pos) const
{
    SPVT::check_range(pos <= size());
    n = SPVT::min_size(n, size() - pos);
    std::copy(begin() + pos, begin() + pos + n, s);
    return n;
}

template<typename E, class T, class A, class S> void
flex_string<E, T, A, S>::swap(flex_string<E, T, A, S>& rhs)
{
    storage_type& srhs = rhs;
    this->storage_type::swap(srhs);
}

// 21.3.6 string operations:
template<typename E, class T, class A, class S>
const typename flex_string<E, T, A, S>::value_type*
flex_string<E, T, A, S>::c_str() const
{
    return storage_type::c_str();
}

template<typename E, class T, class A, class S>
const typename flex_string<E, T, A, S>::value_type*
flex_string<E, T, A, S>::data() const
{
    return storage_type::data();
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::allocator_type
flex_string<E, T, A, S>::get_allocator() const
{
    return storage_type::get_allocator();
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find(const flex_string<E, T, A, S>& str,
                              size_type pos) const
{
    return find(str.data(), pos, str.length());
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find(const value_type* s, size_type pos,
                              size_type n) const
{
    const size_type sz = size();
    if (sz >= n) {
        if (0 == n)
            return (0 == sz) && (0 == pos) ? 0 : npos;
        if (sz - n >= pos) {
            const value_type* p = data();
            do
                if (0 == traits_type::compare(p + pos, s, n))
                    return pos;
            while (sz - n >= ++pos);
        }
    }
    return npos;
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find(const value_type* s, size_type pos) const
{
    return find(s, pos, traits_type::length(s));
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find(value_type c, size_type pos) const
{
    using namespace SPVT;
    return find_first_if(data(), pos, size(), Eq<E>::make(c), npos);
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::rfind(const flex_string<E, T, A, S>& str,
                               size_type pos) const
{
    return rfind(str.data(), pos, str.length());
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::rfind(const value_type* s, size_type pos,
                               size_type n) const
{
    const size_type sz = size();
    if (n > sz)
        return npos;
    pos = SPVT::min_size(pos, sz - n);
    if (n == 0)
        return pos;

    const_iterator rend = begin() - 1;
    for (const_iterator i = begin() + pos; i != rend; --i)
        if (traits_type::eq(*i, *s) && traits_type::compare(&*i, s, n) == 0)
            return i - begin();
    return npos;
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::rfind(const value_type* s, size_type pos) const
{
    return rfind(s, pos, traits_type::length(s));
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::rfind(value_type c, size_type pos) const
{
    using namespace SPVT;
    return find_last_if(data(), pos, size(), Eq<E>::make(c), npos);
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_first_of(const flex_string<E, T, A, S>& str,
                                       size_type pos) const
{
    return find_first_of(str.data(), pos, str.size());
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_first_of(const value_type* s, size_type pos,
                                       size_type n) const
{
    using namespace SPVT;
    return find_first_if(data(), pos, size(), InSeq<E>::make(s, n), npos);
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_first_of(const value_type* s,
                                       size_type pos) const
{
    return find_first_of(s, pos, traits_type::length(s));
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_first_of(value_type c, size_type pos) const
{
    return find(c, pos);
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_last_of(const flex_string<E, T, A, S>& str,
                                      size_type pos) const
{
    return find_last_of(str.data(), pos, str.length());
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_last_of(const value_type* s, size_type pos,
                                      size_type n) const
{
    using namespace SPVT;
    return find_last_if(data(), pos, size(), InSeq<E>::make(s, n), npos);
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_last_of(const value_type* s,
                                      size_type pos) const
{
    return find_last_of(s, pos, traits_type::length(s));
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_last_of(value_type c, size_type pos) const
{
    return rfind(c, pos);
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_first_not_of(const flex_string<E, T, A, S>& str,
                                           size_type pos) const
{
    return find_first_not_of(str.data(), pos, str.length());
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_first_not_of(const value_type* s, size_type pos,
                                           size_type n) const
{
    using namespace SPVT;
    const size_type sz = size();
    return find_first_if(data(), pos, sz, InSeq<E, true>::make(s, n), npos);
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_first_not_of(const value_type* s,
                                           size_type pos) const
{
    return find_first_not_of(s, pos, traits_type::length(s));
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_first_not_of(value_type c,
                                           size_type pos) const
{
    return find_first_not_of(&c, pos, 1);
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_last_not_of(const flex_string<E, T, A, S>& str,
                                          size_type pos) const
{
    return find_last_not_of(str.data(), pos, str.length());
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_last_not_of(const value_type* s, size_type pos,
                                          size_type n) const
{
    using namespace SPVT;
    const size_type sz = size();
    return find_last_if(data(), pos, sz, InSeq<E, true>::make(s, n), npos);
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_last_not_of(const value_type* s,
                                          size_type pos) const
{
    return find_last_not_of(s, pos, traits_type::length(s));
}

template<typename E, class T, class A, class S>
typename flex_string<E, T, A, S>::size_type
flex_string<E, T, A, S>::find_last_not_of(value_type c, size_type pos) const
{
    using namespace SPVT;
    return find_last_if(data(), pos, size(), Eq<E, true>::make(c), npos);
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>
flex_string<E, T, A, S>::substr(size_type pos, size_type n) const
{
    SPVT::check_range(pos <= size());
    return flex_string(data() + pos, SPVT::min_size(n, size() - pos));
}

template<typename E, class T, class A, class S> int
flex_string<E, T, A, S>::compare(const flex_string& str) const
{
    if (this->is_null())
        return str.is_null() ? 0 : 1;
    if (str.is_null())
        return -1;
    return Compare(data(), size(), str.data(), str.size());
}

template<typename E, class T, class A, class S> int
flex_string<E, T, A, S>::compare(size_type pos1, size_type n1,
                                 const flex_string& str) const
{
    if (this->is_null())
        return str.is_null() ? 0 : 1;
    if (str.is_null())
        return -1;
    return compare(pos1, n1, str.data(), str.size());
}

template<typename E, class T, class A, class S> int
flex_string<E, T, A, S>::compare(size_type pos1, size_type n1,
                                 const value_type* s, size_type n2) const
{
    if (this->is_null())
        return -1;
    const size_type sz = size();
    SPVT::check_range(pos1 <= sz);
    n1 = SPVT::min_size(sz - pos1, n1);
    return Compare(data() + pos1, n1, s, n2);
}

template<typename E, class T, class A, class S> int
flex_string<E, T, A, S>::compare(size_type pos1, size_type n1,
                                 const flex_string& str, size_type pos2,
                                 size_type n2) const
{
    if (this->is_null())
        return str.is_null() ? 0 : -1;
    if (str.is_null())
        return 1;
    const size_type sz = str.size();
    SPVT::check_range(pos2 <= sz);
    return compare(pos1, n1, str.data() + pos2, SPVT::min_size(n2, sz - pos2));
}

template<typename E, class T, class A, class S> int
flex_string<E, T, A, S>::compare(const value_type* s) const
{
    if (this->is_null())
        return -1;
    return Compare(data(), size(), s, traits_type::length(s));
}

template<typename E, class T, class A, class S> flex_string<E, T, A, S>
flex_string<E, T, A, S>::operator+ (const flex_string<E, T, A, S>& rhs) const
{
    if (is_null() && rhs.is_null())
        return flex_string();
    return flex_string(data(), size(), rhs.data(), rhs.size());
}

template<typename E, class T, class A, class S>
flex_string<E, T, A, S>::flex_string(const_pointer p1, size_type s1,
                                     const_pointer p2, size_type s2)
 :  storage_type(p1, s1, p2, s2)
{
}

template<typename E, class T, class A, class S>
flex_string<E, T, A, S>::flex_string(value_type c, const_pointer p2,
                                     size_type s2)
 :  storage_type(c, p2, s2)
{
}

template<typename E, class T, class A, class S>
flex_string<E, T, A, S>::flex_string(const_pointer p2, size_type s2,
                                     value_type c)
 :  storage_type(p2, s2, c)
{
}

///////////////////////////////////////////////////////////////////////
// flex_string private member functions implementation
///////////////////////////////////////////////////////////////////////

template<typename E, class T, class A, class S> template<typename InpIt>
inline void
flex_string<E, T, A, S>::Assign(InpIt first, InpIt last, SPVT::InpIterTag)
{
    storage_type tmp;
    tmp.reserve(4); // magic constant
    for (; first != last; ++first)
        tmp.append(*first);;
    tmp.swap(*this);
}

template<typename E, class T, class A, class S> template<typename InpIt>
inline void
flex_string<E, T, A, S>::Assign(InpIt first, InpIt last, SPVT::RndIterTag)
{
    const size_type new_sz = last - first;
    storage_type::resize(new_sz);
    typedef typename StringPrivate::InpIterTraits<InpIt>::value_type InCharType;
    typedef typename CHAR_TRAITS_TYPE(InCharType) InCharTraits;
    for (iterator it = begin(); first != last; ++first, ++it)
        *it = static_cast<int>(InCharTraits::to_int_type(*first));
//    std::uninitialized_copy(first, last, begin());
}

template<typename E, class T, class A, class S> template<typename InpIt>
inline void
flex_string<E, T, A, S>::Assign(InpIt first, InpIt last, SPVT::MBNIterTag)
{
    if (0 != first)
        Assign(first, last, SPVT::RndIterTag());
}

template<typename E, class T, class A, class S> template<typename InpIt>
inline void
flex_string<E, T, A, S>::Append(InpIt first, InpIt last, SPVT::RndIterTag)
{
    const size_type cnt = last - first;
    const size_type newsz = size() + cnt;
    if (capacity() < newsz) {
        storage_type tmp(*this, newsz);
        std::uninitialized_copy(first, last, tmp.end());
        tmp.swap(*this);
    }
    else
        std::uninitialized_copy(first, last, end());
    set_size(newsz);
}

template<typename E, class T, class A, class S> template<typename InpIt>
inline void
flex_string<E, T, A, S>::Append(InpIt first, InpIt last, SPVT::MBNIterTag)
{
    if (0 != first)
        Append(first, last, SPVT::RndIterTag());
}

template<typename E, class T, class A, class S> template<typename InpIt>
inline void
flex_string<E, T, A, S>::Append(InpIt first, InpIt last, SPVT::InpIterTag)
{
    append(flex_string(first, last));
}


/**
 * Replace \a n1 chars starting at \a pos with copy of range designated by
 * random-access iterators [\a first, \a last)
 *
 * @param pos
 * @param n1
 * @param first
 * @param last
 */
template<typename E, class T, class A, class S> template<typename RndIt>
inline void
flex_string<E, T, A, S>::Replace(size_type pos, size_type n1,
                                 RndIt first, RndIt last)
{
    const size_type sz = size();
    n1 = SPVT::min_size(sz - pos, n1);
    const size_type n2 = SPVT::Distance(first, last);
    if (0 == n2) {
        erase(pos, n1);
        return;
    }

    E* beg = begin();
    const size_type newsz = sz - n1 + n2;
    const size_type tailsz = sz - pos - n1; // sizeof the remaining tail
    E* repl = beg + pos;                    // where to place [first, last)
    E* tail = repl + n1;
    if (n2 <= n1) {                         // will not grow in size
        SPVT::Copy(first, last, repl);
        if (n2 < n1 && 0 < tailsz)          // shrink
            traits_type::copy(repl + n2, tail, tailsz);
    }
    else {
        bool overlap = SPVT::Overlap(first, last, repl, tail,
                                     SPVT::IterTag(first));
        if (newsz >= capacity() || overlap) {
            // copy pos chars to tmp
            // those typecasts is for stupid g++ 3.3
            storage_type tmp((const E*)beg,
                             (typename A::size_type)pos,
                             (typename A::size_type)newsz, get_allocator());
            E* p = tmp.begin() + pos;
            p = SPVT::Copy(first, last, p);
            traits_type::copy(p, tail, tailsz);
            tmp.swap(*this);
        }
        else {
            T::move(repl + n2, tail, tailsz);
            SPVT::Copy(first, last, repl);
        }
    }
    set_size(newsz);
}

template<typename E, class T, class A, class S> template<typename InpIt>
inline void
flex_string<E, T, A, S>::Replace(iterator i1, iterator i2,  InpIt j1, InpIt j2,
                                 SPVT::InpIterTag)
{
    replace(i1, i2, flex_string(j1, j2));
}

template<typename E, class T, class A, class S> template<typename InpIt>
inline void
flex_string<E, T, A, S>::Replace(iterator i1, iterator i2, InpIt j1, InpIt j2,
                                 SPVT::RndIterTag)
{
    Replace(i1 - begin(), i2 - i1, j1, j2);
}

template<typename E, class T, class A, class S> template<typename InpIt>
inline void
flex_string<E, T, A, S>::Replace(iterator i1, iterator i2, InpIt j1, InpIt j2,
                                 SPVT::IntIterTag)
{
    Replace(i1 - begin(), i2 - i1, j1, j2);
}

template<typename E, class T, class A, class S> inline int
flex_string<E, T, A, S>::Compare(const value_type* s1, size_type n1,
                                 const value_type* s2, size_type n2)
{
    int res = traits_type::compare(s1, s2, SPVT::min_size(n1, n2));
    if (0 == res)
        return n1 - n2;
    return res;
}

COMMON_NS_END

#ifdef SPVT
//# undef SPVT
#endif
