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

#ifndef CHAR_TRAITS_H_
#define CHAR_TRAITS_H_

#include "StringDecl.h"

#include <cstring>
#include <cstddef>
#include <QString>

namespace std {

template <> struct char_traits<COMMON_NS::Char> {
    typedef COMMON_NS::Char   Char;
    typedef Char              char_type;
    typedef int               int_type;

    static const char_type* move(char_type* dst, const char_type* src, size_t n)
    {
        return static_cast<Char*>(memmove(dst, src, n * sizeof(Char)));
    }

    static const char_type* copy(char_type* dst, const char_type* src, size_t n)
    {
        return (char_type*)memcpy(dst, src, n * sizeof(char_type));
    }

    static bool eq(const char_type& c1, const char_type& c2)
    {
        return c1 == c2;
    }

    static int compare(const char_type* dst, const char_type* src, size_t n)
    {
        while (n--) {
            if (int diff = (dst->unicode() - src->unicode()))
                return diff;
            ++src;
            ++dst;
        }
        return 0;
    }

    static size_t length(const char_type* s)
    {
        const char_type* t = s;
        while (0 != s->unicode())
            ++s;
        return s - t;
    }

    static char_type* assign(char_type* dst, size_t n, char_type c)
    {
        char_type* s = dst;
        while (n--)
            *s++ = c;
        return dst;
    }

    static int_type to_int_type(const char_type& c) { return c.unicode(); }
    static Char to_char_type(const int_type& code) { return Char(code); }
};

}

#endif // CHAR_TRAITS_H_
