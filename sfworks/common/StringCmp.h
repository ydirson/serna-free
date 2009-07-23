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

#ifndef STRING_CMP_H_
#define STRING_CMP_H_

#include "common/common_defs.h"
#include "common/StringDecl.h"
#include "common/StringUtils.h"

#include <QString>
#include <string>

COMMON_NS_BEGIN

COMMON_EXPIMP int compare(const Char*, unsigned, const char*);
COMMON_EXPIMP int compare(const Char*, unsigned, const Char*);

COMMON_EXPIMP int compare(const char*, unsigned, const Char*);
COMMON_EXPIMP int compare(const char*, unsigned, const char*);

COMMON_EXPIMP int compare(const Char*, unsigned, const char*, unsigned);
COMMON_EXPIMP int compare(const char*, unsigned, const Char*, unsigned);

COMMON_EXPIMP int compare(const Char*, unsigned, const Char*, unsigned);
COMMON_EXPIMP int compare(const char*, unsigned, const char*, unsigned);

// -------------- comparison of strings with char* types * ------------- //

template<typename E, class T, class A, class S, typename CharType> inline bool
operator== (const flex_string<E, T, A, S>& lhs, const CharType* rhs)
{
    if (is_null(lhs))
        return 0 == rhs;
    if (0 == rhs)
        return false;
    return 0 == compare(lhs.data(), lhs.length(), rhs);
}

template<typename E, class T, class A, class S, typename CharType> inline bool
operator!= (const flex_string<E, T, A, S>& lhs, const CharType* rhs)
{
    return !(lhs == rhs);
}

template<typename E, class T, class A, class S, typename CharType> inline bool
operator< (const flex_string<E, T, A, S>& lhs, const CharType* rhs)
{
    if (is_null(lhs))
        return 0 != rhs;
    if (0 == rhs)
        return true;
    return compare(lhs.data(), lhs.length(), rhs) < 0;
}

template<typename E, class T, class A, class S, typename CharType> inline bool
operator> (const flex_string<E, T, A, S>& lhs, const CharType* rhs)
{
    if (is_null(lhs))
        return 0 != rhs;
    if (0 == rhs)
        return false;
    return compare(lhs.data(), lhs.length(), rhs) > 0;
}

template<typename E, class T, class A, class S, typename CharType> inline bool
operator<= (const flex_string<E, T, A, S>& lhs, const CharType* rhs)
{
    return !(lhs > rhs);
}

template<typename E, class T, class A, class S, typename CharType> inline bool
operator>= (const flex_string<E, T, A, S>& lhs, const CharType* rhs)
{
    return !(lhs < rhs);
}

// --

template<typename E, class T, class A, class S, typename CharType> inline bool
operator== (const CharType* lhs, const flex_string<E, T, A, S>& rhs)
{
    if (0 == lhs)
        return is_null(rhs);
    if (is_null(rhs))
        return false;
    return 0 == compare(rhs.data(), rhs.length(), lhs);
}

template<typename E, class T, class A, class S, typename CharType> inline bool
operator!= (const CharType* lhs, const flex_string<E, T, A, S>& rhs)
{
    return !(lhs == rhs);
}

template<typename E, class T, class A, class S, typename CharType> inline bool
operator< (const CharType* lhs, const flex_string<E, T, A, S>& rhs)
{
    if (0 == lhs)
        return false;
    if (is_null(rhs))
        return true;
    return 0 < compare(rhs.data(), rhs.length(), lhs);
}

template<typename E, class T, class A, class S, typename CharType> inline bool
operator> (const CharType* lhs, const flex_string<E, T, A, S>& rhs)
{
    if (is_null(rhs))
        return false;
    if (0 == lhs)
        return true;
    return 0 > compare(rhs.data(), rhs.length(), lhs);
}

template<typename E, class T, class A, class S, typename CharType> inline bool
operator<= (const CharType* lhs, const flex_string<E, T, A, S>& rhs)
{
    return !(lhs > rhs);
}

template<typename E, class T, class A, class S, typename CharType> inline bool
operator>= (const CharType* lhs, const flex_string<E, T, A, S>& rhs)
{
    return !(lhs < rhs);
}

// comparison of ranges with char* types

template<typename Iter, typename CharType> inline bool
operator== (const Range<Iter>& lhs, const CharType* rhs)
{
    if (is_null(lhs))
        return 0 == rhs;
    if (0 == rhs)
        return false;
    return 0 == compare(lhs.data(), lhs.length(), rhs);
}

template<typename Iter, typename CharType> inline bool
operator!= (const Range<Iter>& lhs, const CharType* rhs)
{
    return !(lhs == rhs);
}

template<typename Iter, typename CharType> inline bool
operator< (const Range<Iter>& lhs, const CharType* rhs)
{
    if (is_null(lhs))
        return 0 != rhs;
    if (0 == rhs)
        return true;
    return compare(lhs.data(), lhs.length(), rhs) < 0;
}

template<typename Iter, typename CharType> inline bool
operator> (const Range<Iter>& lhs, const CharType* rhs)
{
    if (is_null(lhs))
        return 0 != rhs;
    if (0 == rhs)
        return false;
    return compare(lhs.data(), lhs.length(), rhs) > 0;
}

template<typename Iter, typename CharType> inline bool
operator<= (const Range<Iter>& lhs, const CharType* rhs)
{
    return !(lhs > rhs);
}

template<typename Iter, typename CharType> inline bool
operator>= (const Range<Iter>& lhs, const CharType* rhs)
{
    return !(lhs < rhs);
}

// --

template<typename CharType, typename Iter> inline bool
operator== (const CharType* lhs, const Range<Iter>& rhs)
{
    if (0 == lhs)
        return is_null(rhs);
    if (is_null(rhs))
        return false;
    return 0 == compare(rhs.data(), rhs.length(), lhs);
}

template<typename CharType, typename Iter> inline bool
operator!= (const CharType* lhs, const Range<Iter>& rhs)
{
    return !(lhs == rhs);
}

template<typename CharType, typename Iter> inline bool
operator< (const CharType* lhs, const Range<Iter>& rhs)
{
    if (0 == lhs)
        return false;
    if (is_null(rhs))
        return true;
    return 0 < compare(rhs.data(), rhs.length(), lhs);
}

template<typename CharType, typename Iter> inline bool
operator> (const CharType* lhs, const Range<Iter>& rhs)
{
    if (is_null(rhs))
        return false;
    if (0 == lhs)
        return true;
    return 0 > compare(rhs.data(), rhs.length(), lhs);
}

template<typename CharType, typename Iter> inline bool
operator<= (const CharType* lhs, const Range<Iter>& rhs)
{
    return !(lhs > rhs);
}

template<typename CharType, typename Iter> inline bool
operator>= (const CharType* lhs, const Range<Iter>& rhs)
{
    return !(lhs < rhs);
}

// generic string comparison operations

template <class Lhs, class Rhs> inline bool
str_eq(const Lhs& lhs, const Rhs& rhs)
{
    const bool nullrhs = is_null(rhs);
    if (is_null(lhs))
        return nullrhs;
    if (nullrhs)
        return false;
    return 0 == compare(str_data(lhs), str_length(lhs),
                        str_data(rhs), str_length(rhs));
}

template <class Lhs, class Rhs> inline bool
str_lt(const Lhs& lhs, const Rhs& rhs)
{
    if (is_null(lhs))
        return false;
    if (is_null(rhs))
        return true;
    return compare(str_data(lhs), str_length(lhs),
                   str_data(rhs), str_length(rhs)) < 0;
}

template <class Lhs, class Rhs> inline bool
str_gt(const Lhs& lhs, const Rhs& rhs)
{
    if (is_null(rhs))
        return false;
    if (is_null(lhs))
        return true;
    return compare(str_data(lhs), str_length(lhs),
                   str_data(rhs), str_length(rhs)) > 0;
}
// same for literals
template <class Lhs, typename CharType, unsigned N> inline bool
str_eq(const Lhs& lhs, const CharType (&rhs)[N])
{
    if (is_null(lhs))
        return false;
    return 0 == compare(str_data(lhs), str_length(lhs),
                        str_data(rhs), str_length(rhs));
}

template <typename CharType, unsigned N, class Rhs> inline bool
str_eq(const CharType (&lhs)[N], const Rhs& rhs)
{
    return rhs == lhs;
}

template <class Lhs, typename CharType, unsigned N> inline bool
str_lt(const Lhs& lhs, const CharType (&rhs)[N])
{
    if (is_null(lhs))
        return false;
    return compare(str_data(lhs), str_length(lhs),
                   str_data(rhs), str_length(rhs)) < 0;
}

template <typename CharType, unsigned N, class Rhs> inline bool
str_lt(const CharType (&lhs)[N], const Rhs& rhs)
{
    if (is_null(rhs))
        return true;
    return compare(str_data(lhs), str_length(lhs),
                   str_data(rhs), str_length(rhs)) < 0;
}

template <class Lhs, typename CharType, unsigned N> inline bool
str_gt(const Lhs& lhs, const CharType (&rhs)[N])
{
    if (is_null(lhs))
        return true;
    return compare(str_data(lhs), str_length(lhs),
                   str_data(rhs), str_length(rhs)) > 0;
}

template <typename CharType, unsigned N, class Rhs> inline bool
str_gt(const CharType (&lhs)[N], const Rhs& rhs)
{
    if (is_null(rhs))
        return false;
    return compare(str_data(lhs), str_length(lhs),
                   str_data(rhs), str_length(rhs)) > 0;
}

// -------------- comparisons of flex_string with QString --------------- //

template<typename E, class T, class A, class S> inline bool
operator== (const flex_string<E, T, A, S>& lhs, const QString& rhs)
{
    return str_eq(lhs, rhs);
}

template<typename E, class T, class A, class S> inline bool
operator!= (const flex_string<E, T, A, S>& lhs, const QString& rhs)
{
    return !(lhs == rhs);
}

template<typename E, class T, class A, class S> inline bool
operator< (const flex_string<E, T, A, S>& lhs, const QString& rhs)
{
    return str_lt(lhs, rhs);
}

template<typename E, class T, class A, class S> inline bool
operator> (const flex_string<E, T, A, S>& lhs, const QString& rhs)
{
    return str_gt(lhs, rhs);
}

template<typename E, class T, class A, class S> inline bool
operator<= (const flex_string<E, T, A, S>& lhs, const QString& rhs)
{
    return !(lhs > rhs);
}

template<typename E, class T, class A, class S> inline bool
operator>= (const flex_string<E, T, A, S>& lhs, const QString& rhs)
{
    return !(lhs < rhs);
}

// --

template<typename E, class T, class A, class S> inline bool
operator== (const QString& lhs, const flex_string<E, T, A, S>& rhs)
{
    return str_eq(lhs, rhs);
}

template<typename E, class T, class A, class S> inline bool
operator!= (const QString& lhs, const flex_string<E, T, A, S>& rhs)
{
    return !(lhs == rhs);
}

template<typename E, class T, class A, class S> inline bool
operator< (const QString& lhs, const flex_string<E, T, A, S>& rhs)
{
    return str_lt(lhs, rhs);
}

template<typename E, class T, class A, class S> inline bool
operator> (const QString& lhs, const flex_string<E, T, A, S>& rhs)
{
    return str_gt(lhs, rhs);
}

template<typename E, class T, class A, class S> inline bool
operator<= (const QString& lhs, const flex_string<E, T, A, S>& rhs)
{
    return !(lhs > rhs);
}

template<typename E, class T, class A, class S> inline bool
operator>= (const QString& lhs, const flex_string<E, T, A, S>& rhs)
{
    return !(lhs < rhs);
}

// -------------- comparisons of flex_string with std::string ------------ //

template<typename E, class T, class A, class S> inline bool
operator== (const flex_string<E, T, A, S>& lhs, const std::string& rhs)
{
    return str_eq(lhs, rhs);
}

template<typename E, class T, class A, class S> inline bool
operator!= (const flex_string<E, T, A, S>& lhs, const std::string& rhs)
{
    return !(lhs == rhs);
}

template<typename E, class T, class A, class S> inline bool
operator< (const flex_string<E, T, A, S>& lhs, const std::string& rhs)
{
    return str_lt(lhs, rhs);
}

template<typename E, class T, class A, class S> inline bool
operator> (const flex_string<E, T, A, S>& lhs, const std::string& rhs)
{
    return str_gt(lhs, rhs);
}

template<typename E, class T, class A, class S> inline bool
operator<= (const flex_string<E, T, A, S>& lhs, const std::string& rhs)
{
    return !(lhs > rhs);
}

template<typename E, class T, class A, class S> inline bool
operator>= (const flex_string<E, T, A, S>& lhs, const std::string& rhs)
{
    return !(lhs < rhs);
}

// --

template<typename E, class T, class A, class S> inline bool
operator== (const std::string& lhs, const flex_string<E, T, A, S>& rhs)
{
    return str_eq(lhs, rhs);
}

template<typename E, class T, class A, class S> inline bool
operator!= (const std::string& lhs, const flex_string<E, T, A, S>& rhs)
{
    return !(lhs == rhs);
}

template<typename E, class T, class A, class S> inline bool
operator< (const std::string& lhs, const flex_string<E, T, A, S>& rhs)
{
    return str_lt(lhs, rhs);
}

template<typename E, class T, class A, class S> inline bool
operator> (const std::string& lhs, const flex_string<E, T, A, S>& rhs)
{
    return str_gt(lhs, rhs);
}

template<typename E, class T, class A, class S> inline bool
operator<= (const std::string& lhs, const flex_string<E, T, A, S>& rhs)
{
    return !(lhs > rhs);
}

template<typename E, class T, class A, class S> inline bool
operator>= (const std::string& lhs, const flex_string<E, T, A, S>& rhs)
{
    return !(lhs < rhs);
}

// ----------- comparisons of flex_string with ranges of Iter --------- //

template<typename E, class T, class A, class S, typename Iter> inline bool
operator== (const flex_string<E, T, A, S>& lhs, const Range<Iter>& rhs)
{
    return str_eq(lhs, rhs);
}

template<typename E, class T, class A, class S, typename Iter> inline bool
operator!= (const flex_string<E, T, A, S>& lhs, const Range<Iter>& rhs)
{
    return !(lhs == rhs);
}

template<typename E, class T, class A, class S, typename Iter> inline bool
operator< (const flex_string<E, T, A, S>& lhs, const Range<Iter>& rhs)
{
    return str_lt(lhs, rhs);
}

template<typename E, class T, class A, class S, typename Iter> inline bool
operator> (const flex_string<E, T, A, S>& lhs, const Range<Iter>& rhs)
{
    return str_gt(lhs, rhs);
}

template<typename E, class T, class A, class S, typename Iter> inline bool
operator<= (const flex_string<E, T, A, S>& lhs, const Range<Iter>& rhs)
{
    return !(lhs > rhs);
}

template<typename E, class T, class A, class S, typename Iter> inline bool
operator>= (const flex_string<E, T, A, S>& lhs, const Range<Iter>& rhs)
{
    return !(lhs < rhs);
}

// --

template<typename E, class T, class A, class S, typename Iter> inline bool
operator== (const Range<Iter>& lhs, const flex_string<E, T, A, S>& rhs)
{
    return str_eq(lhs, rhs);
}

template<typename E, class T, class A, class S, typename Iter> inline bool
operator!= (const Range<Iter>& lhs, const flex_string<E, T, A, S>& rhs)
{
    return !(lhs == rhs);
}

template<typename E, class T, class A, class S, typename Iter> inline bool
operator< (const Range<Iter>& lhs, const flex_string<E, T, A, S>& rhs)
{
    return str_lt(lhs, rhs);
}

template<typename E, class T, class A, class S, typename Iter> inline bool
operator> (const Range<Iter>& lhs, const flex_string<E, T, A, S>& rhs)
{
    return str_gt(lhs, rhs);
}

template<typename E, class T, class A, class S, typename Iter> inline bool
operator<= (const Range<Iter>& lhs, const flex_string<E, T, A, S>& rhs)
{
    return !(lhs > rhs);
}

template<typename E, class T, class A, class S, typename Iter> inline bool
operator>= (const Range<Iter>& lhs, const flex_string<E, T, A, S>& rhs)
{
    return !(lhs < rhs);
}

COMMON_NS_END

#endif // STRING_CMP_H_
