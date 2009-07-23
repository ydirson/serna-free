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
#include "common/String.h"
#include "sapi/common/SString.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <qstring.h>

// convert to representation of String
#define SREP(d)     (*(Common::String*)(d.getData()))

// convert to representation of ustring
#define USREP(d)    (*(Common::ustring*)(d.getData()))

// shortcuts for (this)
#define STR         SREP((*this))
#define USTR        USREP((*this))

// convert to (pointer to) Char
#define CPTR(cp)    ((Common::Char*) cp)
#define CCPTR(cp)   ((const Common::Char*) cp)
#define CT(c)       ((const Common::Char&)c)

static char char_buf[8192];

namespace SernaApi {

SString::SString()
{
    new (getData()) Common::String;
}

SString::operator Common::String() const
{
    return *((Common::String*)getData());
}

SString::SString(const Common::String& s)
{
    new (getData()) Common::String(s);
}

SString::SString(Char c)
{
    new (getData()) Common::String(CT(c));
}

SString::SString(const QString& qs)
{
    new (getData()) Common::String(qs);
}

SString::SString(const Char* data, size_type len)
{
    new (getData()) Common::String((Common::Char*) data, len);
}

SString::SString(const unsigned char* data, size_type len)
{
    new (getData()) Common::String((const char*) data, len);
}

SString::SString(const char* data)
{
    new (getData()) Common::String(data);
}

SString::SString(const SString& other)
{
    new (getData()) Common::String(SREP(other));
}

SString& SString::operator=(const SString& other)
{
    STR.~String();
    new (getData()) Common::String(SREP(other));
    return *this;
}

SString& SString::operator=(const char* data)
{
    STR.~String();
    new (getData()) Common::String(data);
    return *this;
}

void SString::resize(size_type n, value_type c)
{
    STR.resize(n, c);
}

void SString::reserve(size_type new_cap)
{
    STR.reserve(new_cap);
}

bool SString::isEmpty() const
{
    return STR.isEmpty();
}

bool SString::isNull() const
{
    return STR.isNull();
}

const Char* SString::unicode() const
{
    return (Char*) STR.unicode();
}

const Char* SString::c_str() const
{
    return (Char*) STR.c_str();
}

SString::size_type SString::length() const
{
    return STR.length();
}

SString::operator QString() const
{
    return STR.qstring();
}

char* SString::toLatin1(char* buf, size_type maxlen) const
{
    if (buf == 0) {
        buf    = char_buf;
        maxlen = sizeof(char_buf);
    }
    if (isNull()) {
        buf[0] = 0;
        return buf;
    }
    const char* to_end = buf + maxlen - 1;
    char* to = buf;
    const Char* from_end = unicode() + length();
    const Char* from = unicode();
    while (to < to_end && from < from_end)
        *to++ = (from++)->unicode() & 0377;
    *to++ = 0;
    return buf;
}

char* SString::toUtf8(char* buf, size_type maxlen) const
{
    if (buf == 0) {
        buf    = char_buf;
        maxlen = sizeof(char_buf);
    }
    if (isNull()) {
        buf[0] = 0;
        return buf;
    }
    if (maxlen > 0)
        --maxlen;
    QString qs((const QChar*) STR.unicode(), STR.length());
    QByteArray qcs(qs.utf8());
    int len = qcs.length() < (int)maxlen ? qcs.length() : maxlen;
    memcpy(buf, qcs.data(), len);
    buf[len] = 0;
    return buf;
}

char* SString::toLocal8Bit(char* buf, size_type maxlen) const
{
    if (buf == 0) {
        buf    = char_buf;
        maxlen = sizeof(char_buf);
    }
    if (isNull()) {
        buf[0] = 0;
        return buf;
    }
    if (maxlen > 0)
        --maxlen;
    QByteArray qcs =
        QString((const QChar*) STR.unicode(), STR.length()).local8Bit();
    memcpy(buf, qcs.data(), 
        size_type(qcs.length()) < maxlen ? qcs.length() : maxlen);
    buf[maxlen] = 0;
    return buf;
}

SString::~SString()
{
    STR.~String();
}

///////////////////////////////////////////////////

SString::reference SString::at(size_type pos)
{
    return (reference) STR.at(pos);
}

SString::const_reference SString::at(size_type pos) const
{
    return (reference) STR.at(pos);
}

SString::iterator SString::begin()
{
    return (iterator) STR.begin();
}

SString::const_iterator SString::begin() const
{
    return (const_iterator) STR.begin();
}

SString::iterator SString::end()
{
    return (iterator) STR.end();
}

SString::const_iterator SString::end() const
{
    return (const_iterator) STR.end();
}

//////////////////////////////////////////////////////

SString& SString::append(const SString& str)
{
    STR.append(SREP(str));
    return *this;
}

SString& SString::append(const SString& str, size_type pos, size_type n)
{
    USTR.append(USREP(str), pos, n);
    return *this;
}

SString& SString::append(const value_type* s, size_type n)
{
    USTR.append(CPTR(s), n);
    return *this;
}

SString& SString::append(const value_type* s)
{
    USTR.append(CPTR(s));
    return *this;
}

SString& SString::append(size_type n, value_type c)
{
    USTR.append(n, CT(c));
    return *this;
}

void SString::push_back(value_type c)
{
    USTR.push_back(CT(c));
}

SString& SString::append(const_iterator first, const_iterator last)
{
    USTR.append(CCPTR(first), CCPTR(last));
    return *this;
}

///////////////////////////////////////////////////////////////////

SString& SString::insert(size_type pos, const SString& str)
{
    USTR.insert(pos, USREP(str));
    return *this;
}

SString& SString::insert(size_type pos1, const SString& str, size_type pos2,
                         size_type n)
{
    USTR.insert(pos1, USREP(str), pos2, n);
    return *this;
}

SString& SString::insert(size_type pos, const value_type* s, size_type n)
{
    USTR.insert(pos, CCPTR(s), n);
    return *this;
}

SString& SString::insert(size_type pos, const value_type* s)
{
    USTR.insert(pos, CCPTR(s));
    return *this;
}

SString& SString::insert(size_type pos, size_type n, value_type c)
{
    USTR.insert(pos, n, CT(c));
    return *this;
}

SString::iterator SString::insert(iterator p, value_type c)
{
    return (iterator) USTR.insert(CPTR(p), CT(c));
}

void SString::insert(iterator p, size_type n, value_type c)
{
    USTR.insert(CPTR(p), n, CT(c));
}

void SString::insert(iterator p, const_iterator first, const_iterator last)
{
    USTR.insert(CPTR(p), CCPTR(first), CCPTR(last));
}

///////////////////////////////////////////////////////////////////

SString& SString::erase(size_type pos, size_type n)
{
    USTR.erase(pos, n);
    return *this;
}

SString::iterator SString::erase(iterator pos)
{
    return (iterator) USTR.erase(CPTR(pos));
}

SString::iterator SString::erase(iterator first, iterator last)
{
    return (iterator) USTR.erase(CPTR(first), CPTR(last));
}

///////////////////////////////////////////////////////////////////

SString& SString::replace(size_type pos, size_type n, const SString& str)
{
    USTR.replace(pos, n, USREP(str));
    return *this;
}

SString& SString::replace(size_type pos1, size_type n1, const SString& str,
                          size_type pos2, size_type n2)
{
    USTR.replace(pos1, n1, USREP(str), pos2, n2);
    return *this;
}

SString& SString::replace(size_type pos, size_type n1, const value_type* s,
                          size_type n2)
{
    USTR.replace(pos, n1, CCPTR(s), n2);
    return *this;
}

SString& SString::replace(size_type pos, size_type n, const value_type* s)
{
    USTR.replace(pos, n, CCPTR(s));
    return *this;
}

SString& SString::replace(size_type pos, size_type n1, size_type n2,
                          value_type c)
{
    USTR.replace(pos, n1, n2, CT(c));
    return *this;
}

SString& SString::replace(iterator i1, iterator i2, const SString& str)
{
    USTR.replace(CPTR(i1), CPTR(i2), USREP(str));
    return *this;
}

SString& SString::replace(iterator i1, iterator i2, const value_type* s,
                          size_type n)
{
    USTR.replace(CPTR(i1), CPTR(i2), CCPTR(s), n);
    return *this;
}

SString& SString::replace(iterator i1, iterator i2, const value_type* s)
{
    USTR.replace(CPTR(i1), CPTR(i2), CCPTR(s));
    return *this;
}

SString& SString::replace(iterator i1, iterator i2, size_type n, value_type c)
{
    USTR.replace(CPTR(i1), CPTR(i2), n, CT(c));
    return *this;
}

SString& SString::replace(iterator i1, iterator i2, const_iterator j1,
                          const_iterator j2)
{
    USTR.replace(CPTR(i1), CPTR(i2), CCPTR(j1), CCPTR(j2));
    return *this;
}

///////////////////////////////////////////////////////////////////

SString::size_type
SString::copy(value_type* s, size_type n, size_type pos) const
{
    return USTR.copy(CPTR(s), n, pos);
}

void SString::swap(SString& rhs)
{
    USTR.swap(USREP(rhs));
}

///////////////////////////////////////////////////////////////////

SString::size_type SString::find(const SString& str, size_type pos) const
{
    return USTR.find(USREP(str), pos);
}

SString::size_type
SString::find(const value_type* s, size_type pos, size_type n) const
{
    return USTR.find(CCPTR(s), pos, n);
}

SString::size_type SString::find(const value_type* s, size_type pos) const
{
    return USTR.find(CCPTR(s), pos);
}

SString::size_type SString::find(value_type c, size_type pos) const
{
    return USTR.find(CT(c), pos);
}

SString::size_type SString::rfind(const SString& str, size_type pos) const
{
    return USTR.rfind(USREP(str), pos);
}

SString::size_type
SString::rfind(const value_type* s, size_type pos, size_type n) const
{
    return USTR.rfind(CCPTR(s), pos, n);
}

SString::size_type SString::rfind(const value_type* s, size_type pos) const
{
    return USTR.rfind(CCPTR(s), pos);
}

SString::size_type SString::rfind(value_type c, size_type pos) const
{
    return USTR.rfind(CT(c), pos);
}

SString::size_type
SString::find_first_of(const SString& str, size_type pos) const
{
    return USTR.find_first_of(USREP(str), pos);
}

SString::size_type
SString::find_first_of(const value_type* s, size_type pos,
                       size_type n) const
{
    return USTR.find_first_of(CCPTR(s), pos, n);
}

SString::size_type
SString::find_first_of(const value_type* s, size_type pos) const
{
    return USTR.find_first_of(CCPTR(s), pos);
}

SString::size_type
SString::find_first_of(value_type c, size_type pos) const
{
    return USTR.find_first_of(CT(c), pos);
}

SString::size_type
SString::find_last_of(const SString& str, size_type pos) const
{
    return USTR.find_last_of(USREP(str), pos);
}

SString::size_type
SString::find_last_of(const value_type* s, size_type pos,
                      size_type n) const
{
    return USTR.find_last_of(CCPTR(s), pos, n);
}

SString::size_type
SString::find_last_of(const value_type* s, size_type pos) const
{
    return USTR.find_last_of(CCPTR(s), pos);
}

SString::size_type
SString::find_last_of(value_type c, size_type pos) const
{
    return USTR.find_last_of(CT(c), pos);
}

SString::size_type
SString::find_first_not_of(const SString& str,
                           size_type pos) const
{
    return USTR.find_first_not_of(USREP(str), pos);
}

SString::size_type
SString::find_first_not_of(const value_type* s, size_type pos,
                           size_type n) const
{
    return USTR.find_first_not_of(CCPTR(s), pos, n);
}

SString::size_type
SString::find_first_not_of(const value_type* s, size_type pos) const
{
    return USTR.find_first_not_of(CCPTR(s), pos);
}

SString::size_type
SString::find_last_not_of(const SString& str, size_type pos) const
{
    return USTR.find_last_not_of(USREP(str), pos);
}

SString::size_type
SString::find_last_not_of(const value_type* s, size_type pos,
                          size_type n) const
{
    return USTR.find_last_not_of(CCPTR(s), pos, n);
}

SString::size_type
SString::find_last_not_of(const value_type* s, size_type pos) const
{
    return USTR.find_last_not_of(CCPTR(s), pos);
}

SString::size_type
SString::find_last_not_of(value_type c, size_type pos) const
{
    return USTR.find_last_not_of(CT(c), pos);
}

SString SString::substr(size_type pos, size_type n) const
{
    return Common::String(USTR.substr(pos, n));
}

int SString::compare(const SString& str) const
{
    return USTR.compare(USREP(str));
}

int SString::compare(size_type pos, size_type n, const SString& str) const
{
    return USTR.compare(pos, n, USREP(str));
}

int SString::compare(size_type pos1, size_type n1, const value_type* s,
                     size_type n2) const
{
    return USTR.compare(pos1, n1, CCPTR(s), n2);
}

int SString::compare(size_type pos1, size_type n1, const SString& str,
                     size_type pos2, size_type n2) const
{
    return USTR.compare(pos1, n1, USREP(str), pos2, n2);
}

int SString::compare(const value_type* s) const
{
    return USTR.compare(CCPTR(s));
}

SString SString::operator+(const SString& rhs) const
{
    return Common::String(USTR.operator+(USREP(rhs)));
}

//////////////////////////////////////////////////////////////////////

short SString::toShort(bool *ok, int base) const
{
    return STR.toShort(ok, base);
}

unsigned short SString::toUShort(bool *ok, int base) const
{
    return STR.toUShort(ok, base);
}

int SString::toInt(bool *ok, int base) const
{
    return STR.toInt(ok, base);
}

unsigned int SString::toUInt(bool *ok, int base) const
{
    return STR.toLong(ok, base);
}

long SString::toLong(bool *ok, int base) const
{
    return STR.toLong(ok, base);
}

unsigned long SString::toULong(bool *ok, int base) const
{
    return STR.toULong(ok, base);
}

float SString::toFloat(bool *ok) const
{
    return STR.toFloat(ok);
}

double SString::toDouble(bool *ok) const
{
    return STR.toDouble(ok);
}

SString SString::number(long n, int base)
{
    return Common::String::number(n, base);
}

SString SString::number(unsigned long n, int base)
{
    return Common::String::number(n, base);
}

SString SString::number(int n, int base)
{
    return Common::String::number(n, base);
}

SString SString::number(size_type n, int base)
{
    return Common::String::number(n, base);
}

SString SString::number(double n, char f, int prec)
{
    return Common::String::number(n, f, prec);
}

//////////////////////////////////////////////////////////////////

SString SString::lower() const
{
    return STR.lower();
}

SString SString::upper() const
{
    return STR.upper();
}

SString SString::stripWhiteSpace() const
{
    return STR.stripWhiteSpace();
}

SString SString::simplifyWhiteSpace() const
{
    return STR.simplifyWhiteSpace();
}

SString SString::right(size_type len) const
{
    return STR.right(len);
}

SString SString::mid(size_type index, size_type len) const
{
    return STR.mid(index, len);
}

////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const SString& s)
{
    os << s.operator Common::String().local8Bit();
    return os;
}

////////////////////////////////////////////////////////////////////////

const Char Char::null = QChar::null;
const Char Char::replacement = QChar::replacement;
const Char Char::byteOrderMark = QChar::byteOrderMark;
const Char Char::byteOrderSwapped = QChar::byteOrderSwapped;
const Char Char::nbsp = QChar::nbsp;

Char::Char(const QChar& c)
    : ucs_(c.unicode())
{
}

Char::operator QChar() const
{
    return QChar(ucs_);
}

#define CHAR_CVT(type, name) \
    Char::type Char::name() const { return (Char::type) QChar(ucs_).name(); }

CHAR_CVT(Category,  category)
CHAR_CVT(Direction, direction)
CHAR_CVT(Decomposition, decompositionTag)
CHAR_CVT(Joining, joining)

unsigned char Char::combiningClass() const
{
    return QChar(ucs_).combiningClass();
}

#define CHAR_FCVT(type, func) \
    type Char::func() const { return QChar(ucs_).func(); }

CHAR_FCVT(int, digitValue)
CHAR_FCVT(Char, lower)
CHAR_FCVT(Char, upper)
CHAR_FCVT(bool, mirrored)
CHAR_FCVT(Char, mirroredChar)
CHAR_FCVT(bool, isPrint)
CHAR_FCVT(bool, isPunct)
CHAR_FCVT(bool, isSpace)
CHAR_FCVT(bool, isMark)
CHAR_FCVT(bool, isLetter)
CHAR_FCVT(bool, isNumber)
CHAR_FCVT(bool, isLetterOrNumber)
CHAR_FCVT(bool, isDigit)
CHAR_FCVT(bool, isSymbol)

} // namespace SernaApi

