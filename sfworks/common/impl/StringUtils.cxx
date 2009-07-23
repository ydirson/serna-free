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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#include "common/common_defs.h"
#include "common/String.h"
#include "common/RefCntStorage.h"
#include "common/FreeStorage.h"
#include "common/StrdupNew.h"
#include "common/CharTraits.h"

#include <QString>
#include <string.h>

COMMON_NS_BEGIN

template<typename CharType> inline typename CHAR_TRAITS_TYPE(CharType)::int_type
ch2int_type(const CharType& ch)
{
    return CHAR_TRAITS_TYPE(CharType)::to_int_type(ch);
}

template<typename T1, typename T2> struct CharCmp {
    bool operator()(const T1& lhs, const T2& rhs)
    {
        return ch2int_type(lhs) == ch2int_type(rhs);
    }
};

template<typename T1, typename T2> CharCmp<T1, T2>
make_char_cmp(const T1&, const T2&)
{
    CharCmp<T1, T2> cmp;
    return cmp;
}

template<typename T1, typename T2> struct CharNCaseCmp {
    bool operator()(const T1& lhs, const T2& rhs)
    {
        return ch2int_type(to_lower(lhs)) == ch2int_type(to_lower(lhs));
    }
};

template<typename T1, typename T2> CharCmp<T1, T2>
make_char_ncase_cmp(const T1&, const T2&)
{
    CharNCaseCmp<T1, T2> cmp;
    return cmp;
}

/**
 * Compares two buffers of CharType1 and CharType2
 *
 * The \a lhs string has length \a lhs_len, \a rhs has an arbitrary length
 *
 * @return 0 if buffers match,
 *         dist + 1 if \a lhs[dist] > \a rhs[dist]
 *         -(dist + 1) if \a lhs[dist] < \a rhs[dist]
 */
template<typename Char1, typename Char2, class Cmp> inline int
compare_aux(const Char1* lhs, unsigned lhs_len, const Char2* rhs, Cmp cmp)
{
    const Char1* e = lhs + lhs_len;
    while (lhs < e && cmp(*lhs, *rhs)) { ++lhs; ++rhs; }
    if (lhs == e)
        return 0 != ch2int_type(*rhs) ? -(lhs_len + 1) : 0;
    const int dist = lhs_len - (e - lhs) + 1;
    return ch2int_type(*lhs) > ch2int_type(*rhs) ? dist : -dist;
}

template<typename CharType1, typename CharType2> inline int
str_cmp(const CharType1* lhs, unsigned lhs_len, const CharType2* rhs)
{
    return compare_aux(lhs, lhs_len, rhs, make_char_cmp(*lhs, *rhs));
}

template<typename CharType1, typename CharType2> inline int
str_icmp(const CharType1* lhs, unsigned lhs_len, const CharType2* rhs)
{
    return compare_aux(lhs, lhs_len, rhs, make_char_ncase_cmp(*lhs, *rhs));
}

int compare(const Char* s, unsigned l, const char* p)
{
    return str_cmp(s, l, p);
}

int compare(const char* s, unsigned l, const char* p)
{
    return str_cmp(s, l, p);
}

int compare(const char* s, unsigned l, const Char* p)
{
    return str_cmp(s, l, p);
}

int compare(const Char* s, unsigned l, const Char* p)
{
    return str_cmp(s, l, p);
}

template<typename Char1, typename Char2, class Cmp> inline int
compare_aux2(const Char1* lhs, unsigned lhs_len,
             const Char2* rhs, unsigned rhs_len, Cmp cmp)
{
    unsigned i = 0, min_len = lhs_len < rhs_len ? lhs_len : rhs_len;
    while (i < min_len && cmp(lhs[i], rhs[i]))
        ++i;
    if (i == min_len)
        return lhs_len - rhs_len;
    int equal = ch2int_type(lhs[i]) - ch2int_type(rhs[i]);
    ++i;
    return equal < 0 ? -i : +i;
}

int compare(const Char* s, unsigned sn, const char* p, unsigned pn)
{
    return compare_aux2(s, sn, p, pn, make_char_cmp(*s, *p));
}

int compare(const char* s, unsigned sn, const Char* p, unsigned pn)
{
    return - compare(p, pn, s, sn);
}

int compare(const Char* s, unsigned sn, const Char* p, unsigned pn)
{
    return compare_aux2(s, sn, p, pn, make_char_cmp(*s, *p));
}

int compare(const char* s, unsigned sn, const char* p, unsigned pn)
{
    return compare_aux2(s, sn, p, pn, make_char_cmp(*s, *p));
}

COMMON_NS_END

USING_COMMON_NS

COMMON_EXPIMP char* strdup_new(const char* src)
{
    if (0 == src)
        return 0;
    size_t sz = strlen(src);
    char* os = new char[++sz];
    return strcpy(os, src);
}

/**
 * Intended for duplication of const char* strings
 * does not use new char[], so it's safe to be used
 * to initialize auto_ptrs and OwnerPtrs
 *
 * @param src    char sequence to be duplicated
 *
 * @return newly duplicated sequence
 */
COMMON_EXPIMP char* strdup_noarray(const char* src)
{
    if (0 == src)
        return 0;
    size_t sz = strlen(src);
    char* os = static_cast<char*>(::operator new(++sz));
    return strcpy(os, src);
}
