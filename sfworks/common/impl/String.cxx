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

#include "common/String.h"
#include "common/StringCvt.h"
#include "common/StringUtils.h"
#include "common/Debug.h"
#include <QString>
#include <string>
#include <algorithm>
#include <string.h>

COMMON_NS_BEGIN

template<typename E> static inline const QChar*
qcharp(const E* p) { return reinterpret_cast<const QChar*>(p); }

const String& String::null()
{
    static const String null_;
    return null_;
}

String::String(const char* s, size_type len) : base_type(s, s + len) {}

String::String(const char* s) : base_type(s, s + (s ? strlen(s) : 0)) {}

String::String(size_type len, char ch) : base_type(len, value_type(ch)) {}
String::String(const QString& s) : base_type(qcharp(s.unicode()), s.length()) {}

// implementation of Qt-like interface

nstring String::local8Bit() const
{
    return COMMON_NS::local_8bit(*this);
}

nstring String::latin1() const
{
    return COMMON_NS::latin1(*this);
}

nstring String::utf8() const
{
    return COMMON_NS::utf8(*this);
}

int String::find(value_type c, int ind, bool cs) const
{
    return QSREF(*this).find(c, ind, cs);
}

int String::find(const char* s, int ind, bool cs) const
{
    return QSREF(*this).find(s, ind, cs);
}

int String::find(const String& s, int ind, bool cs) const
{
    return QSREF(*this).find(QSREF(s), ind, cs);
}

int String::findRev(value_type c, int ind, bool cs) const
{
    return QSREF(*this).findRev(c, ind, cs);
}

int String::findRev(const char* s, int ind, bool cs) const
{
    return QSREF(*this).findRev(s, ind, cs);
}

int String::findRev(const String& s, int ind, bool cs) const
{
    return QSREF(*this).findRev(QSREF(s), ind, cs);
}

static inline Qt::CaseSensitivity is_cs(bool cs)
{
    return cs ? Qt::CaseSensitive : Qt::CaseInsensitive;
}

int String::contains(value_type c, bool cs) const
{
    return true == QSREF(*this).contains(c, is_cs(cs));
}

int String::contains(const char* s, bool cs) const
{
    return true == QSREF(*this).contains(s, is_cs(cs));
}

int String::contains(const String& s, bool cs) const
{
    return true == QSREF(*this).contains(QSREF(s), is_cs(cs));
}

String String::right(size_type len) const
{
    return COMMON_NS::right(*this, len);
}

String String::mid(size_type index, size_type len) const
{
    return COMMON_NS::mid(*this, index, len);
}

String String::leftJustify(size_type width, value_type fill, bool trunc) const
{
    return left_justify(*this, width, fill, trunc);
}

String String::rightJustify(size_type width, value_type fill, bool trunc) const
{
    return right_justify(*this, width, fill, trunc);
}

String String::lower() const
{
    return to_lower<String>(*this);
}

String String::upper() const
{
    return to_upper<String>(*this);
}

String String::stripWhiteSpace() const
{
    return strip_white_space(*this);
}

String String::simplifyWhiteSpace() const
{
    return simplify_white_space(*this);
}

String& String::insert(size_type index, char c)
{
    base_type::insert(index, 1, value_type(c));
    return *this;
}

String& String::insert(size_type index, const char* s)
{
    base_type::insert(index, fromLocal8Bit(s));
    return *this;
}

String& String::insert(size_type index, value_type c)
{
    base_type::insert(index, 1, c.unicode());
    return *this;
}

String& String::insert(size_type index, const value_type* s, size_type l)
{
    base_type::insert(index, base_type(s, l));
    return *this;
}

String& String::insert(size_type index, const String& s)
{
    return insert(index, s.data(), s.length());
}

String& String::append(char c)
{
    base_type::append(1, value_type(c));
    return *this;
}

String& String::append(value_type c)
{
    base_type::append(1, c);
    return *this;
}

String& String::append(const String& s)
{
    if (s.is_null())
        return *this;
    base_type::append(s.data(), s.length());
    return *this;
}

String& String::append(const char* s)
{
    if (0 == s)
        return *this;
    const size_type sz = strlen(s);
    //! when sz == 0 QString::fromLocal8Bit returns empty string under win32 and
    //! QString::null() under unix. Hence the workaround is to append 0 chars
    if (0 == sz)
        base_type::append(0U, Char('\0'));
    else {
        QString tmp(QString::fromLocal8Bit(s, sz));
        base_type::append(tmp.unicode(), tmp.length());
    }
    return *this;
}

String& String::replace(const String& pat, const String& to, bool cs)
{
    if (empty())
        return *this;

    size_type plen = pat.length();
    size_type tlen = to.length();
    const value_type* tdata = to.data();

    for (int p = 0; ; p += tlen) {
        p = find(pat, p, cs);
        if (0 > p)
            return *this;
        replace(p, plen, tdata, tlen);
    }
}

String& String::replace(const QRegExp& qre, const String& to)
{
    QString qs(data(), length());
    qs.replace(qre, QSREF(to));
    assign(qs.unicode(), qs.length());
    return *this;
}

short String::toShort (bool *ok, int base) const
{
    return to_number<short>(*this, base, ok);
}

unsigned short String::toUShort(bool *ok, int base) const
{
    return to_number<unsigned short>(*this, base, ok);
}

int String::toInt (bool *ok, int base) const
{
    return to_number<int>(*this, base, ok);
}

unsigned int String::toUInt (bool *ok, int base) const
{
    return to_number<unsigned>(*this, base, ok);
}

long String::toLong (bool *ok, int base) const
{
    return to_number<long>(*this, base, ok);
}

unsigned long String::toULong (bool *ok, int base) const
{
    return to_number<unsigned long>(*this, base, ok);
}

float String::toFloat (bool *ok) const
{
    return to_number<float>(*this, ok);
}

double String::toDouble (bool *ok) const
{
    return to_number<double>(*this, ok);
}

String& String::setNum(long l, int base)
{
    set_num(*this, l, unsigned(base));
    return *this;
}

String& String::setNum(unsigned long l, int base)
{
    set_num(*this, l, unsigned(base));
    return *this;
}

String& String::setNum(double d, char f, int prec)
{
    set_num(*this, d, f, unsigned(prec));
    return *this;
}

String String::number(long l, int base)
{
    return from_number<String>(l, unsigned(base));
}

String String::number(unsigned long l, int base)
{
    return from_number<String>(l, unsigned(base));
}

String String::number(int i, int base)
{
    return from_number<String>(i, unsigned(base));
}

String String::number(unsigned int i, int base)
{
    return from_number<String>(i, unsigned(base));
}

String String::number(double d, char f, int prec)
{
    return from_number<String>(d, f, unsigned(prec));
}

String String::fromLocal8Bit(const char* s, int len)
{
    QString tmp(QString::fromLocal8Bit(s, len));
    return String(qcharp(tmp.unicode()), tmp.length());
}

String String::fromLatin1(const char* s, int len)
{
    QString tmp(QString::fromLatin1(s, len));
    return String(qcharp(tmp.unicode()), tmp.length());
}

String::operator QString() const
{
    return QString(reinterpret_cast<const QChar*>(data()), length());
}

String& String::operator=(const QString& qs)
{
    assign(reinterpret_cast<const value_type*>(qs.unicode()), qs.length());
    return *this;
}

String& String::setUnicode(const value_type* unicode, uint len)
{
    assign(unicode, len);
    return *this;
}

String& String::setLatin1(const char* s, int len)
{
    assign(fromLatin1(s, len));
    return *this;
}

int String::compare(const String& s1, const String& s2)
{
    return s1.compare(s2);
}

std::ostream& operator<<(std::ostream& os, const String& s)
{
    return os << local_8bit(s).c_str();
}

COMMON_NS_END

#if !defined(NDEBUG) && (defined(DEBUG) || defined(_DEBUG))

COMMON_NS_BEGIN

const DebugOutStream& operator << (const DebugOutStream& ds, const String& s)
{
    ds.notify();
    ds.get_os() << s;
    return ds;
}

COMMON_NS_END

#if !defined(_WIN32)

const char* as_cstr(const Common::String& str)
{
    using namespace Common;
    static char DBGBUF[1024];
    const Char* p = str.data();
    const int sz = std::min(str.size(),
                            static_cast<String::size_type>(sizeof(DBGBUF)));
    for (int i = 0; i < sz; ++i)
        DBGBUF[i] = p[i].latin1();
    DBGBUF[sz] = '\0';
    return DBGBUF;
}

#endif

#endif

