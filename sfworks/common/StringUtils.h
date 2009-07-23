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
 #ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include "common/common_defs.h"
#include "common/common_types.h"
#include "common/StringDecl.h"
#include "common/IterTraits.h"

#include <QString>
#include <ctype.h>
#include <string>
#include <utility>

COMMON_NS_BEGIN

template<typename StrType> struct string_traits {
    typedef typename StrType::value_type        CharType;
    typedef typename StrType::iterator          IterType;
    typedef typename StrType::const_iterator    ConstIterType;
    typedef typename StrType::value_type*       PointerType;
    typedef const typename StrType::value_type* ConstPointerType;
};

template<> struct string_traits<QString> {
    typedef QChar           CharType;
    typedef QChar*          IterType;
    typedef const QChar*    ConstIterType;
    typedef const QChar*    ConstPointerType;
    typedef QChar*          PointerType;
};

template<class StrType> inline typename string_traits<StrType>::ConstPointerType
str_data(const StrType& s) { return s.data(); }

template<class StrType> inline unsigned
str_length(const StrType& s) { return s.length(); }

inline const QChar* str_data(const QString& qs) { return qs.unicode(); }

template<class StrType> inline void
str_reserve(StrType& s, unsigned n) { s.reserve(n); }

template <class StrType> inline int str_mem(const StrType& s)
    { return s.capacity() * sizeof(typename StrType::value_type) + 
        sizeof(typename StrType::size_type); }

template<typename T, unsigned N> inline const T*
str_data(T (&l)[N])
{
    return &l[0];
}

template<typename T, unsigned N> inline unsigned
str_length(T (&l)[N])
{
    return (0 == l[N - 1]) ? N - 1 : N;
}

template<typename T, unsigned N> inline unsigned
str_length(const T (&l)[N])
{
    return (0 == l[N - 1]) ? N - 1 : N;
}

inline bool is_space(Char c) { return c.isSpace(); }
inline bool is_space(char c) { return isspace(c); }

template<typename E, class T, class A, class S> bool
is_null(const flex_string<E, T, A, S>& s);

#define FSTRING(c,s) flex_string<c, CHAR_TRAITS_TYPE(c), std::allocator<c>, \
                       s<c, CHAR_TRAITS_TYPE(c), std::allocator<c> > >

template<> COMMON_EXPIMP bool is_null(const FSTRING(Char, FreeStorage)&);
template<> COMMON_EXPIMP bool is_null(const FSTRING(char, FreeStorage)&);
template<> COMMON_EXPIMP bool is_null(const FSTRING(Char, RefCntStorage)&);
template<> COMMON_EXPIMP bool is_null(const FSTRING(char, RefCntStorage)&);

#undef FSTRING

template<typename T, unsigned N> inline bool
is_null(const T (&l)[N])
{
    return false;
}

inline bool is_null(const QString& qs) { return qs.isNull(); }
inline bool is_null(const std::string&) { return false; }

//! range of input iterators
template<typename Iter> class Range : public std::pair<Iter, Iter> {
public:
    typedef std::pair<Iter, Iter> BaseType;
    using BaseType::first;
    using BaseType::second;
    //!
    typedef typename StringPrivate::InpIterTraits<Iter>::value_type value_type;
    typedef Iter iterator;
    //!
    Range(Iter f, Iter s) : std::pair<Iter, Iter>(f, s) {}
    bool empty() const { return first >= second; }
    unsigned size() const { return empty() ? 0 : second - first; }
    unsigned length() const { return size(); }
    //!
    iterator begin() { return first; }
    iterator end() { return second; }
    const value_type* data() const { return &(*first); }
    iterator begin() const { return first; }
    iterator end() const { return second; }
};

template<typename Iter> inline bool is_null(const Range<Iter>& r)
{
    return 0 == &(*r.begin());
}

template<typename Iter> struct string_traits<Range<Iter> > {
    typedef typename Range<Iter>::value_type    CharType;
    typedef const CharType*                     ConstPointerType;
    typedef CharType*                           PointerType;
    typedef Iter                                IterType;
};

template<typename RndIter> Range<RndIter>
make_range(RndIter first, RndIter last)
{
    return Range<RndIter>(first, last);
}

template<typename RndIter> Range<RndIter>
make_range(RndIter first, unsigned distance)
{
    return Range<RndIter>(first, first + distance);
}

template<class StrType>
inline Range<const typename string_traits<StrType>::CharType*>
make_range(const StrType& s)
{
    return make_range(str_data(s), s.length());
}

template<typename T, unsigned N> inline Range<T*>
make_range(T (&l)[N])
{
    return Range<T*>(&l[0], &l[0] + str_length(l));
}

template<typename T, unsigned N> inline Range<const T*>
make_range(const T (&l)[N])
{
    return Range<const T*>(&l[0], &l[0] + str_length(l));
}

inline Range<const Char*> make_range(const QString& qs)
{
    if (qs.isNull())
        return Range<const Char*>(0, 0);
    return Range<const Char*>(qs.unicode(), qs.unicode() + qs.length());
}

template<typename CharType> struct IsSpace {
    bool operator()(CharType c) { return is_space(c); }
};

template<typename CharType> struct IsChar {
    IsChar(CharType c) : char_(c) {}
    IsChar() : char_(CharType(0)) {}
    bool operator()(CharType c) const
    {
        return CHAR_TRAITS_TYPE(CharType)::eq(char_, c);
    }
private:
    CharType char_;
};

template <typename CharType, typename SetType = char> struct InChars {
    typedef SetType CharSetType;
    //!
    InChars() : set_(0), setSize_(0) {}
    InChars(const CharSetType* set, unsigned len)
     :  set_(set), setSize_(len) {}
    template <unsigned N> InChars(const CharSetType (&lit)[N])
     :  set_(&lit[0]), setSize_(0 == lit[N - 1] ? N - 1 : N) {}
    bool operator()(CharType c)
    {
        if (0 == setSize_)
            return CharType(0) == c;
        for (unsigned i = 0; i < setSize_; ++i)
            if (CHAR_TRAITS_TYPE(CharType)::eq(set_[i], c))
                return true;
        return false;
    }
private:
    const CharSetType*  set_;
    unsigned            setSize_;
};

template<typename RndIter, class Pr> inline Range<RndIter>
find_longest_range_between(RndIter f, RndIter l, Pr pr)
{
    RndIter b = f, e = l;
    for (; b < l && pr(*b); ++b) ;
    for (--l; e > b && pr(*l); e = l--) ;
    return Range<RndIter>(b, e);
}

template<typename RndIter, class Pr> inline Range<RndIter>
find_longest_range_between(const Range<RndIter>& r, Pr pr)
{
    return find_longest_range_between(r.first, r.second, pr);
}

template<typename RndIter, class Pr> inline Range<RndIter>
find_first_range_not_of(RndIter f, RndIter l, Pr pr)
{
    RndIter b, e;
    for (b = f; b < l && pr(*b); ++b) ;
    for (e = b; e < l && !pr(*e); ++e) ;
    return Range<RndIter>(b, e);
}

template<typename RndIter, class Pr> inline Range<RndIter>
find_first_range_not_of(const Range<RndIter>& r, Pr pr)
{
    return find_first_range_not_of(r.first, r.second, pr);
}

template<typename RndIter, class Pr> inline Range<RndIter>
find_first_range_between(RndIter f, RndIter l, Pr pr)
{
    RndIter b = f;
    for (; f < l && !pr(*f); ++f) ;
    return Range<RndIter>(b, f);
}

template<typename RndIter, class Pr> inline Range<RndIter>
find_first_range_between(const Range<RndIter>& r, Pr pr)
{
    return find_first_range_between(r.first, r.second, pr);
}

template<class StrType> StrType
strip_white_space(const StrType& s)
{
    if (is_null(s))
        return s;

    typedef typename string_traits<StrType>::ConstIterType CIter;
    typedef typename string_traits<StrType>::CharType CharType;
    IsSpace<CharType> ws_pred;
    Range<CIter> r(find_longest_range_between(make_range(s), ws_pred));

    if (r.empty())
        return StrType(r.first, 0U);
    return StrType(r.first, r.size());
}

template<class StrType> StrType
simplify_white_space(const StrType& s)
{
    if (is_null(s))
        return s;

    typedef typename string_traits<StrType>::ConstIterType CIter;
    typedef typename string_traits<StrType>::CharType CharType;
    IsSpace<CharType> ws_pred;
    Range<CIter> r(find_longest_range_between(make_range(s), ws_pred));

    StrType tmp(r.first, 0U); // empty string
    if (!r.empty()) {
        tmp.reserve(r.size());
        for (CIter src = r.first;;) {
            tmp += *r.first;
            for (src = ++r.first; !r.empty() && is_space(*r.first); ++r.first) ;
            if (r.empty())
                break;
            if (src < r.first)
                tmp += ' ';
        }
    }
    return tmp;
}

template<class StrType> StrType
right_justify(const StrType& s, unsigned width,
              typename StrType::value_type fill = ' ', bool trunc = false)
{
    const typename StrType::value_type* data = str_data(s);
    const unsigned l = s.length();
    if (l < width) {
        StrType res;
        res.reserve(width);
        res.append(width - l, fill).append(data, l);
        return res;
    }
    return trunc ? StrType(data, width) : StrType(data, l);
}

template<class StrType> StrType
left_justify(const StrType& s, unsigned width,
             typename StrType::value_type fill = ' ', bool trunc = false)
{
    const typename StrType::value_type* data = str_data(s);
    const unsigned l = s.length();
    if (l < width) {
        StrType res;
        res.reserve(width);
        res.append(data, l).append(width - l, fill);
        return res;
    }
    return trunc ? StrType(data, width) : StrType(data, l);
}

template<class StrType> StrType
mid(const StrType& s, unsigned pos, unsigned len)
{
    unsigned sz = s.size();
    if (pos > sz || 0 == sz)
        return StrType();
    return s.substr(pos, len);
}

template<class StrType> StrType
left(const StrType& s, unsigned len)
{
    return s.substr(0, len);
}

template<class StrType> StrType
right(const StrType& s, unsigned len)
{
    unsigned sz = s.size();
    if (sz < len)
        return s;
    return s.substr(sz - len, len);
}

template<typename E, class T, class A, class S, typename Ch> bool
starts_with(const flex_string<E, T, A, S>& s, const Ch* p)
{
    int diffpos = compare(s.data(), s.length(), p);
    return diffpos > 0 ? '\0' == p[diffpos - 1] : 0 == diffpos;
}

template<typename E, class T, class A, class S, class StrType> bool
starts_with_aux(const flex_string<E, T, A, S>& s, const StrType& pfx)
{
    if (is_null(pfx))
        return true;
    unsigned pl = pfx.length(), sl = s.length();
    if (pl <= sl)
        return 0 == compare(s.data(), pl, str_data(pfx), pl);
    return false;
}

template<typename E, class T, class A, class S> inline bool
starts_with(const flex_string<E, T, A, S>& s, const QString& qs)
{
    return starts_with_aux(s, qs);
}

template<typename E, class T, class A, class S> inline bool
starts_with(const flex_string<E, T, A, S>& s, const ustring& us)
{
    return starts_with_aux(s, us);
}

template<typename E, class T, class A, class S> inline bool
starts_with(const flex_string<E, T, A, S>& s, const nstring& ns)
{
    return starts_with_aux(s, ns);
}

template<typename E, class T, class A, class S, typename Ch> bool
ends_with(const flex_string<E, T, A, S>& s, const Ch* sfx)
{
    unsigned sfxl = CHAR_TRAITS_TYPE(Ch)::length(sfx), sl = s.length();
    if (sfxl <= sl)
        return 0 == compare(s.data() + sl - sfxl, sfxl, sfx, sfxl);
    return false;
}

template<typename E, class T, class A, class S, class StrType> bool
ends_with_aux(const flex_string<E, T, A, S>& s, const StrType& sfx)
{
    if (is_null(sfx))
        return true;
    unsigned sfxl = sfx.length(), sl = s.length();
    if (sfxl <= sl)
        return 0 == compare(s.data() + sl - sfxl, sfxl, str_data(sfx), sfxl);
    return false;
}

template<typename E, class T, class A, class S> inline bool
ends_with(const flex_string<E, T, A, S>& s, const QString& sfx)
{
    return ends_with_aux(s, sfx);
}

template<typename E, class T, class A, class S> inline bool
ends_with(const flex_string<E, T, A, S>& s, const ustring& sfx)
{
    return ends_with_aux(s, sfx);
}

template<typename E, class T, class A, class S> inline bool
ends_with(const flex_string<E, T, A, S>& s, const nstring& sfx)
{
    return ends_with_aux(s, sfx);
}

inline Char to_lower(Char c) { return c.toLower(); }
inline char to_lower(char c) { return tolower(c); }

inline Char to_upper(Char c) { return c.toUpper(); }
inline char to_upper(char c) { return toupper(c); }

template<typename E> struct ToLower {
    E operator()(E c) { return to_lower(c); }
};

template<typename E> struct ToUpper {
    E operator()(E c) { return to_upper(c); }
};

template<class StrDst, typename RndIt, class UnaryOp> StrDst
str_transform(RndIt first, RndIt last, UnaryOp op)
{
    StrDst dst;
    str_reserve(dst, last - first);
    typedef typename string_traits<StrDst>::CharType CharType;
    for (; first < last; ++first)
        dst += CharType(to_int_type(op(*first)));
    return dst;
}

template<class StrDst, class StrSrc, class UnaryOp> StrDst
str_transform(const StrSrc& s, UnaryOp op)
{
    const typename string_traits<StrSrc>::CharType* it = str_data(s);
    return str_transform<StrDst>(it, it + s.size(), op);
}

template<class StrDst, class StrSrc> inline StrDst
to_lower(const StrSrc& s)
{
    typedef typename string_traits<StrSrc>::CharType CharType;
    ToLower<CharType> pr;
    return str_transform<StrDst>(s, pr);
}

template<class StrDst, class StrSrc> inline StrDst
to_upper(const StrSrc& s)
{
    typedef typename string_traits<StrSrc>::CharType CharType;
    ToUpper<CharType> pr;
    return str_transform<StrDst>(s, pr);
}

template<class S, typename It> inline S&
str_assign(S& s, const Range<It>& r)
{
    return is_null(r) ? s : s.assign(r.begin(), r.end());
}

template<class S, typename T, unsigned N> inline S&
str_assign(S& s, T (&l)[N])
{
    return str_assign(s, make_range<T, N>(l));
}
// overload with const T is for dumb compilers
template<class S, typename T, unsigned N> inline S&
str_assign(S& s, const T (&l)[N])
{
//    const Range<const T*> r();
    return str_assign(s, make_range<const T, N>(l));
}

template<class S, typename It> inline S&
str_append(S& s, const Range<It>& r)
{
    return is_null(r) ? s : s.append(r.begin(), r.end());
}

template<class S, typename T, unsigned N> inline S&
str_append(S& s, T (&l)[N])
{
    return str_append(s, make_range<T, N>(l));
}

template<class S, typename T, unsigned N> inline S&
str_append(S& s, const T (&l)[N])
{
    return str_append(s, make_range<const T, N>(l));
}

template<class S, typename It> inline S&
str_insert(S& s, unsigned pos, const Range<It>& r)
{
    return is_null(r) ? s : (s.insert(s.begin() + pos, r.begin(), r.end()), s);
}

template<class S, typename T, unsigned N> inline S&
str_insert(S& s, unsigned pos, T (&l)[N])
{
    return str_insert(s, pos, make_range<T, N>(l));
}

template<class S, typename T, unsigned N> inline S&
str_insert(S& s, unsigned pos, const T (&l)[N])
{
    return str_insert(s, pos, make_range<const T, N>(l));
}

COMMON_NS_END

#endif
