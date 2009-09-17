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
    Forward declaration of String/Char
 */
#ifndef STRING_DECL_H_
#define STRING_DECL_H_

#include "common/common_defs.h"

namespace std {
template<typename T> class allocator;
template<typename T> struct char_traits;
#define CHAR_TRAITS_TYPE(T) std::char_traits<T>
};

class QString;
class QChar;

COMMON_NS_BEGIN

template<typename CharType> inline int to_int_type(const CharType& ch)
{
    return CHAR_TRAITS_TYPE(CharType)::to_int_type(ch);
}

typedef QChar Char;
class String;
class RangeString;

template <typename E, class T, class A> class FreeStorage;
template <typename E, class T, class A> class RefCntStorage;
template <typename E, class T, class A, class S> class flex_string;

typedef flex_string<Char, CHAR_TRAITS_TYPE(Char), std::allocator<Char>,
                    RefCntStorage<Char, CHAR_TRAITS_TYPE(Char),
                    std::allocator<Char> > > ustring;
typedef flex_string<char, CHAR_TRAITS_TYPE(char), std::allocator<char>,
                    RefCntStorage<char, CHAR_TRAITS_TYPE(char),
                    std::allocator<char> > > nstring;

template<typename T> class Range;
typedef Range<const Char*> UCRange;
typedef Range<const char*> NCRange;

COMMON_NS_END

namespace std {
template <> struct char_traits<COMMON_NS::Char>;
}

#endif // STRING_DECL_H_
