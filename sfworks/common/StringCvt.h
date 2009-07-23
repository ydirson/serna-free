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

#ifndef STRING_CVT_H_
#define STRING_CVT_H_

#include "common/common_defs.h"
#include "common/StringDecl.h"
#include "common/StringUtils.h"

#include <QString>
#include <string>

COMMON_NS_BEGIN

COMMON_EXPIMP nstring latin1(const ustring& s);
COMMON_EXPIMP nstring local_8bit(const ustring& s);
COMMON_EXPIMP nstring utf8(const ustring& s);

COMMON_EXPIMP String from_local_8bit(const char* p, unsigned len = ~0);
COMMON_EXPIMP String from_latin1(const char* p, unsigned len = ~0);
COMMON_EXPIMP String from_utf8(const char* p, unsigned len = ~0);

template<class S, typename T, unsigned N> inline S from_literal(T (&l)[N])
{
    return S(&l[0], &l[0] + str_length(l));
}
// overload with const T is for dumb compilers
template<class S, typename T, unsigned N> inline S from_literal(const T (&l)[N])
{
    return S(&l[0], &l[0] + str_length(l));
}

template<unsigned N> inline QString from_literal(char (&l)[N])
{
    return QString::fromLatin1(&l[0], str_length(l));
}

template<unsigned N> inline QString from_literal(const char (&l)[N])
{
    return QString::fromLatin1(&l[0], str_length(l));
}

// ----------------- from/to strings -------------------- //

template<class DstType, typename InpIter> inline DstType
range_ctor(const Range<InpIter>& r)
{
    if (0 == r.data())
        return DstType();
    return DstType(r.begin(), r.end());
}

template<> inline QString
range_ctor<QString, const Char*>(const Range<const Char*>& r)
{
    if (0 == r.data())
        return QString();
    return QString(r.begin(), r.size());
}

template<> inline QString
range_ctor<QString, const char*>(const Range<const char*>& r)
{
    if (0 == r.data())
        return QString();
    return QString::fromLatin1(r.data(), r.size());
}

template<class DstType, class SrcType> inline
DstType from_string(const SrcType& s)
{
    return range_ctor<DstType>(make_range(s));
}

template<class DstType, typename E, class T, class A, class S> inline
DstType to_string(const flex_string<E, T, A, S>& s)
{
    if (is_null(s))
        return DstType();
    return range_ctor<DstType>(make_range(s));
}

// -------------- from/to/set number ----------------- //

// this is a bunch of internal functions, avoid using them directly
COMMON_EXPIMP uint32 itos(char* e, char i, unsigned b);
COMMON_EXPIMP uint32 itos(char* e, short i, unsigned b);
COMMON_EXPIMP uint32 itos(char* e, int i, unsigned b);
COMMON_EXPIMP uint32 itos(char* e, long i, unsigned b);
COMMON_EXPIMP uint32 itos(char* e, int64 i, unsigned b);
COMMON_EXPIMP uint32 itos(char* e, unsigned char i, unsigned b);
COMMON_EXPIMP uint32 itos(char* e, unsigned short i, unsigned b);
COMMON_EXPIMP uint32 itos(char* e, unsigned int i, unsigned b);
COMMON_EXPIMP uint32 itos(char* e, unsigned long i, unsigned b);
COMMON_EXPIMP uint32 itos(char* e, uint64 i, unsigned b);
COMMON_EXPIMP uint32 dtos(char* buf, unsigned bufsz, double d, char flag,
                          unsigned prec);

COMMON_EXPIMP char stoi(const char* buf, char len, unsigned base, bool* ok);
COMMON_EXPIMP short stoi(const char* buf, short len, unsigned base, bool* ok);
COMMON_EXPIMP int stoi(const char* buf, int len, unsigned base, bool* ok);
COMMON_EXPIMP long stoi(const char* buf, long len, unsigned base, bool* ok);
COMMON_EXPIMP int64 stoi(const char* buf, int64 len, unsigned base, bool* ok);
COMMON_EXPIMP unsigned char stoi(const char* buf, unsigned char len,
                                 unsigned base, bool* ok);
COMMON_EXPIMP unsigned short stoi(const char* buf, unsigned short len,
                                 unsigned base, bool* ok);
COMMON_EXPIMP unsigned int stoi(const char* buf, unsigned int len,
                                unsigned base, bool* ok);
COMMON_EXPIMP unsigned long stoi(const char* buf, unsigned long len,
                                 unsigned base, bool* ok);
COMMON_EXPIMP uint64 stoi(const char* buf, uint64 len, unsigned base, bool* ok);
COMMON_EXPIMP double stod(const char* buf, unsigned len, bool* ok);

COMMON_EXPIMP char stoi(const Char* buf, char len, unsigned base, bool* ok);
COMMON_EXPIMP short stoi(const Char* buf, short len, unsigned base, bool* ok);
COMMON_EXPIMP int stoi(const Char* buf, int len, unsigned base, bool* ok);
COMMON_EXPIMP long stoi(const Char* buf, long len, unsigned base, bool* ok);
COMMON_EXPIMP int64 stoi(const Char* buf, int64 len, unsigned base, bool* ok);
COMMON_EXPIMP unsigned char stoi(const Char* buf, unsigned char len,
                                 unsigned base, bool* ok);
COMMON_EXPIMP unsigned short stoi(const Char* buf, unsigned short len,
                                  unsigned base, bool* ok);
COMMON_EXPIMP unsigned int stoi(const Char* buf, unsigned int len,
                                unsigned base, bool* ok);
COMMON_EXPIMP unsigned long stoi(const Char* buf, unsigned long len,
                                unsigned base, bool* ok);
COMMON_EXPIMP uint64 stoi(const Char* buf, uint64 len, unsigned base, bool* ok);
COMMON_EXPIMP double stod(const Char* buf, unsigned len, bool* ok);

template<typename IntType, class StrType> IntType
to_number(const StrType& s, unsigned base = 10, bool* ok = 0)
{
    typedef typename string_traits<StrType>::CharType CharType;
    return stoi(str_data(s), IntType(s.length()), base, ok);
}

template<typename IntType, class StrType> IntType
to_integer(const StrType& s, bool* ok = 0, unsigned base = 10)
{
    return to_number<IntType, StrType>(s, base, ok);
}

template<typename DblType, class StrType> DblType
to_number(const StrType& s, bool* ok = 0)
{
    typedef typename string_traits<StrType>::CharType CharType;
    return stod(s.c_str(), s.length(), ok);
}

template<typename DblType, class StrType> DblType
to_double(const StrType& s, bool* ok = 0)
{
    return to_number<DblType, StrType>(s, ok);
}

template<class StrType, typename IntType> StrType&
set_num(StrType& s, IntType i, unsigned int base = 10)
{
    char buf[65]; // enough to hold uint64 number in base 2 format
    char* end = buf + sizeof(buf);
    s.assign(end - itos(end, i, base), end);
    return s;
}

template<class StrType, typename IntType> StrType&
set_integer(StrType& s, IntType i, unsigned int base = 10)
{
    return set_num(s, i, base);
}

template<class StrType, typename DblType> StrType&
set_num(StrType& s, DblType d, char flag = 'g', unsigned prec = 6)
{
    char buf[384]; // enough to hold MAX_DBL (~1.8e+308) with precision 20
    s.assign(buf, buf + dtos(buf, sizeof(buf), d, flag, prec));
    return s;
}

template<class StrType, typename DblType> StrType&
set_double(StrType& s, DblType d, char flag = 'g', unsigned prec = 6)
{
    return set_num(s, d, flag, prec);
}

template<class StrType, typename IntType> StrType
from_number(IntType i, unsigned base = 10)
{
    char buf[65];
    char* end = buf + sizeof(buf);
    return StrType(end - itos(end, i, base), end);
}

template<class StrType, typename IntType> StrType
from_integer(IntType i, unsigned base = 10)
{
    return from_number<StrType, IntType>(i, base);
}

template<class StrType, typename DblType> StrType
from_number(DblType d, char flag = 'g', unsigned prec = 6)
{
    char buf[384];
    return StrType(buf, buf + dtos(buf, sizeof(buf), d, flag, prec));
}

template<class StrType, typename DblType> StrType
from_double(DblType d, char flag = 'g', unsigned prec = 6)
{
    return from_number<String, DblType>(d, flag, prec);
}

COMMON_NS_END

#endif // STRING_CVT_H_
