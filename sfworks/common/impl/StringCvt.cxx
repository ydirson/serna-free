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

#include "common/common_limits.h"
#include "common/common_types.h"
#include "common/math_defs.h"
#include "common/String.h"
#include "common/StringCvt.h"
#include "common/StringUtils.h"
#include "common/FlexString.h"

#include <QString>
#include <QByteArray>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#ifdef _MSC_VER
// unary minus operator applied to unsigned type, result still unsigned
# pragma warning (disable: 4146)
# define SNPRINTF _snprintf
#else
# define SNPRINTF snprintf
#endif

COMMON_NS_BEGIN

template<typename E> static inline const QChar*
qcharp(const E* p) { return reinterpret_cast<const QChar*>(p); }

nstring local_8bit(const ustring& s)
{
    QString qs(qcharp(s.data()), s.length());
    return nstring(qs.local8Bit());
}

nstring latin1(const ustring& s)
{
    return nstring(s.begin(), s.end());
}

nstring utf8(const ustring& s)
{
    QByteArray qba(QString::fromRawData(qcharp(s.data()), s.length()).toUtf8());
    return nstring(qba.data(), qba.length());
}

String
from_local_8bit(const char* p, unsigned len)
{
    if (unsigned(~0) == len)
        len = strlen(p);
    QString qs(QString::fromLocal8Bit(p, (int)len));
    return String(qcharp(str_data(qs)), qs.length());
}

String
from_latin1(const char* p, unsigned len)
{
    if (unsigned(~0) == len)
        len = strlen(p);
    return String(p, p + len);
}

String
from_utf8(const char* p, unsigned len)
{
    if (unsigned(~0) == len)
        len = strlen(p);
    QString qs(QString::fromUtf8(p, (int)len));
    return String(qcharp(str_data(qs)), qs.length());
}

#if defined(_MSC_VER) && (_MSC_VER < 1300)
std::string
copy_construct(const Char* buf, unsigned len, const std::string*)
{
    std::string tmp;
    for (tmp.reserve(len); len > 0; --len, ++buf)
        tmp.append(1, *buf);
    return tmp;
}
#endif

// ------------- number conversion utilities --------------- //

static const char cvtbuf[] = "0123456789abcdefghijklmnopqrstuvwxyz";

inline int
normbase(unsigned base)
{
    if (base < 2 || base > 36)
        return 10;
    return base;
}

template<typename CharType>
static inline CharType* ucvt(CharType* dest, uint64 v, unsigned base)
{
    do {
        new(--dest) CharType(cvtbuf[v % base]);
        v /= base;
    } while (0 != v);
    return dest;
}

template<typename CharType>
static inline unsigned utos(CharType* end, uint64 v, unsigned base)
{
    return end - ucvt(end, v, normbase(base));
}

template<typename CharType>
static unsigned itos(CharType* end, int64 i, unsigned base, uint64 all_ones)
{
    if (10 != base)
        return utos(end, i & all_ones, base);

    CharType* dest = end;
    base = normbase(base);
    bool negative = (i < 0);
    if (negative) {
        if (Min<int64>() == i) {
            int r = -(i % int(base));
            new(--dest) CharType(cvtbuf[r]);
            i /= int(base);
        }
        i = -i;
    }
    dest = ucvt(dest, i, base);
    if (negative)
        new(--dest) CharType('-');
    return end - dest;
}

uint32 itos(char* e, char i, unsigned b) { return itos(e, i, b, UCHAR_MAX); }
uint32 itos(char* e, short i, unsigned b) { return itos(e, i, b, USHRT_MAX); }
uint32 itos(char* e, int i, unsigned b) { return itos(e, i, b, UINT_MAX); }
uint32 itos(char* e, long i, unsigned b) { return itos(e, i, b, ULONG_MAX); }
uint32 itos(char* e, int64 i, unsigned b) { return itos(e, i, b, ULLONG_MAX); }

uint32 itos(char* e, unsigned char i, unsigned b) { return utos(e, i, b); }
uint32 itos(char* e, unsigned short i, unsigned b) { return utos(e, i, b); }
uint32 itos(char* e, unsigned int i, unsigned b) { return utos(e, i, b); }
uint32 itos(char* e, unsigned long i, unsigned b) { return utos(e, i, b); }
uint32 itos(char* e, uint64 i, unsigned b) { return utos(e, i, b); }

static char
fmtflag(char f)
{
    if (!(f == 'f' || f == 'e' || f == 'E' || f == 'g' || f == 'G'))
        f = 'f';
    return f;
}

static inline unsigned
fmtprec(unsigned prec) { return prec > 20 ? 20 : prec; }

uint32
dtos(char* buf, unsigned bufsz, double d, char flag, unsigned prec)
{
    char fmtstr[16];
    char* p = &fmtstr[16];
    *--p = '\0';
    *--p = fmtflag(flag);;
    *--p = 'l';
    p -= utos(p, fmtprec(prec), 10);
    *--p = '.';
    *--p = '%';
    int r = SNPRINTF(buf, bufsz, p, d);
    return r > 0 ? r : 0;
}

static const unsigned char digits_map[] = {
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 99, 99, 99, 99, 99, 99,
    99, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 99, 99, 99, 99, 99,
    99, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
};

struct okguard {
    okguard(bool* ok) : val_(false), ok_ (ok) {}
    ~okguard() { if (0 != ok_) *ok_ = val_; }
    void reset(bool v) { val_ = v; }
private:
    bool val_;
    bool* ok_;
};

// expects no whitespaces in buffer
template<typename CharType>
uint64
int_froms(const CharType* buf, unsigned len, uint64 ub, int64 lb, unsigned base,
          bool* ok)
{
    okguard okg(ok);
    IsSpace<CharType> ws_pred;
    Range<const CharType*> r(find_first_range_not_of(buf, buf + len, ws_pred));

    if (r.empty())
        return 0;

    const CharType* p = r.first;
    const CharType* last = r.second - 1;

    bool read = false;
    bool negative = ('-' == *p);
    if (negative || '+' == *p)
        ++p;

    if (base == (uint)~0 && '0' == *p && p < last) {
        ++p;
        if (('x' == *p || 'X' == *p) && p <= last) {
            ++p;
            base = 16;
        }
        else {
            base = 8;
            read = true;
        }
    }
    else
        base = normbase(base);

    if ('0' == *p) {
        read = true;
        for (; last >= p && '0' == *p; ++p);
    }

    if (last >= p) {
        if (10 == base) {
            if (negative && 0 != lb)
                ub = -lb;
        }
        else
            ub -= lb;
        uint64 val = 0;
        uint64 b = 1;
        unsigned char c;
        for (; last >= p; --last, b *= base) {
            unsigned char idx = CHAR_TRAITS_TYPE(CharType)::to_int_type(*last);
            c = digits_map[idx];
            if (base < c)
                return 0;
            const uint64 diff = b * c;
            if (val > (ub - diff))
                return 0;
            val += diff;
        }
        okg.reset(true);
        return negative ? -val : val;
    }
    okg.reset(read);
    return 0;
}

// ---------------- char stoi -------------------- //

char
stoi(const char* buf, char len, unsigned base, bool* ok)
{
    return int_froms(buf, len, SCHAR_MAX, SCHAR_MIN, base, ok);
}

short
stoi(const char* buf, short len, unsigned base, bool* ok)
{
    return int_froms(buf, len, SHRT_MAX, SHRT_MIN, base, ok);
}

int
stoi(const char* buf, int len, unsigned base, bool* ok)
{
    return int_froms(buf, len, INT_MAX, INT_MIN, base, ok);
}

long
stoi(const char* buf, long len, unsigned base, bool* ok)
{
    return int_froms(buf, len, LONG_MAX, LONG_MIN, base, ok);
}

int64
stoi(const char* buf, int64 len, unsigned base, bool* ok)
{
    return int_froms(buf, len, LLONG_MAX, LLONG_MIN, base, ok);
}

unsigned char
stoi(const char* buf, unsigned char len, unsigned base, bool* ok)
{
    return int_froms(buf, len, UCHAR_MAX, 0, base, ok);
}

unsigned short
stoi(const char* buf, unsigned short len, unsigned base, bool* ok)
{
    return int_froms(buf, len, USHRT_MAX, 0, base, ok);
}

unsigned int
stoi(const char* buf, unsigned int len, unsigned base, bool* ok)
{
    return int_froms(buf, len, UINT_MAX, 0, base, ok);
}

unsigned long
stoi(const char* buf, unsigned long len, unsigned base, bool* ok)
{
    return int_froms(buf, len, ULONG_MAX, 0, base, ok);
}

uint64
stoi(const char* buf, uint64 len, unsigned base, bool* ok)
{
    return int_froms(buf, len, ULLONG_MAX, 0, base, ok);
}

// ---------------- Char stoi -------------------- //

char
stoi(const Char* buf, char len, unsigned base, bool* ok)
{
    return int_froms(buf, len, SCHAR_MAX, SCHAR_MIN, base, ok);
}

short
stoi(const Char* buf, short len, unsigned base, bool* ok)
{
    return int_froms(buf, len, SHRT_MAX, SHRT_MIN, base, ok);
}

int
stoi(const Char* buf, int len, unsigned base, bool* ok)
{
    return int_froms(buf, len, INT_MAX, INT_MIN, base, ok);
}

long
stoi(const Char* buf, long len, unsigned base, bool* ok)
{
    return int_froms(buf, len, LONG_MAX, LONG_MIN, base, ok);
}

int64
stoi(const Char* buf, int64 len, unsigned base, bool* ok)
{
    return int_froms(buf, len, LLONG_MAX, LLONG_MIN, base, ok);
}

unsigned char
stoi(const Char* buf, unsigned char len, unsigned base, bool* ok)
{
    return int_froms(buf, len, UCHAR_MAX, 0, base, ok);
}

unsigned short
stoi(const Char* buf, unsigned short len, unsigned base, bool* ok)
{
    return int_froms(buf, len, USHRT_MAX, 0, base, ok);
}

unsigned int
stoi(const Char* buf, unsigned int len, unsigned base, bool* ok)
{
    return int_froms(buf, len, UINT_MAX, 0, base, ok);
}

unsigned long
stoi(const Char* buf, unsigned long len, unsigned base, bool* ok)
{
    return int_froms(buf, len, ULONG_MAX, 0, base, ok);
}

uint64
stoi(const Char* buf, uint64 len, unsigned base, bool* ok)
{
    return int_froms(buf, len, ULLONG_MAX, 0, base, ok);
}

double
stod(const char* buf, unsigned len, okguard& okg)
{
    char* p = 0;
//    buf[len] = '\0';
    double result = strtod(buf, &p);
    if (buf + len == p)
        okg.reset(true);
    else
        result = NAN;
    return result;
}

//! generic CharType* to double conversion
template<typename CharType> inline double
stod(const CharType* begin, const CharType *end, bool* ok)
{
    okguard okg(ok);
    typedef Range<const CharType*> CRange;
    IsSpace<CharType> ws_pred;
    CRange r(find_longest_range_between(begin, end, ws_pred));
    const unsigned sz = r.size();
    const unsigned BUFSZ = 385; // magic
    if (0 != sz && sz < BUFSZ) {
        char buf[BUFSZ];
        for (char* p = &buf[0]; r.first != r.second; ++r.first)
            *p++ = CHAR_TRAITS_TYPE(CharType)::to_int_type(*r.first);
//        std::uninitialized_copy(r.first, r.second, &buf[0]);
        buf[sz] = '\0';
        return stod(buf, sz, okg);
    }
    return NAN;
}

double
stod(const Char* buf, unsigned len, bool* ok)
{
    return stod(buf, buf + len, ok);
}

double
stod(const char* buf, unsigned len, bool* ok)
{
    okguard okg(ok);

    IsSpace<char> ws_pred;
    Range<const char*> r(find_longest_range_between(buf, buf + len, ws_pred));

    if (!r.empty())
        return stod(r.first, r.size(), okg);
    return NAN;
}

COMMON_NS_END
// this is for the little stupid g++ 2.95.x which can't correctly handle
// function declarations in function definitions
#if 0 // defined(__GNUG__) && (__GNUG__ < 3) && (__GNUC_MINOR__ < 96)
USING_COMMON_NS

uint32 itos(char* e, char i, unsigned b) { return COMMON_NS::itos(e, i, b); }
uint32 itos(char* e, short i, unsigned b) { return COMMON_NS::itos(e, i, b); }
uint32 itos(char* e, int i, unsigned b) { return COMMON_NS::itos(e, i, b); }
uint32 itos(char* e, long i, unsigned b) { return COMMON_NS::itos(e, i, b); }
uint32 itos(char* e, int64 i, unsigned b) { return COMMON_NS::itos(e, i, b); }

uint32 itos(char* e, unsigned char i, unsigned b)
{
    return COMMON_NS::utos(e, i, b);
}
uint32 itos(char* e, unsigned short i, unsigned b)
{
    return COMMON_NS::utos(e, i, b);
}
uint32 itos(char* e, unsigned int i, unsigned b)
{
    return COMMON_NS::utos(e, i, b);
}
uint32 itos(char* e, unsigned long i, unsigned b)
{
    return COMMON_NS::utos(e, i, b);
}
uint32 itos(char* e, uint64 i, unsigned b) { return COMMON_NS::utos(e, i, b); }

uint32 dtos(char* buf, unsigned bufsz, double d, char flag, unsigned prec)
{
    return COMMON_NS::dtos(buf, bufsz, d, flag, prec);
}

char
stoi(const char* buf, char len, unsigned base, bool* ok)
{
    return int_froms(buf, len, SCHAR_MAX, SCHAR_MIN, base, ok);
}

short
stoi(const char* buf, short len, unsigned base, bool* ok)
{
    return int_froms(buf, len, SHRT_MAX, SHRT_MIN, base, ok);
}

int
stoi(const char* buf, int len, unsigned base, bool* ok)
{
    return int_froms(buf, len, INT_MAX, INT_MIN, base, ok);
}

long
stoi(const char* buf, long len, unsigned base, bool* ok)
{
    return int_froms(buf, len, LONG_MAX, LONG_MIN, base, ok);
}

int64
stoi(const char* buf, int64 len, unsigned base, bool* ok)
{
    return int_froms(buf, len, LLONG_MAX, LLONG_MIN, base, ok);
}

unsigned char
stoi(const char* buf, unsigned char len, unsigned base, bool* ok)
{
    return int_froms(buf, len, UCHAR_MAX, 0, base, ok);
}

unsigned short
stoi(const char* buf, unsigned short len, unsigned base, bool* ok)
{
    return int_froms(buf, len, USHRT_MAX, 0, base, ok);
}

unsigned int
stoi(const char* buf, unsigned int len, unsigned base, bool* ok)
{
    return int_froms(buf, len, UINT_MAX, 0, base, ok);
}

unsigned long
stoi(const char* buf, unsigned long len, unsigned base, bool* ok)
{
    return int_froms(buf, len, ULONG_MAX, 0, base, ok);
}

uint64
stoi(const char* buf, uint64 len, unsigned base, bool* ok)
{
    return int_froms(buf, len, ULLONG_MAX, 0, base, ok);
}

// ---------------- Char stoi -------------------- //

char
stoi(const Char* buf, char len, unsigned base, bool* ok)
{
    return int_froms(buf, len, SCHAR_MAX, SCHAR_MIN, base, ok);
}

short
stoi(const Char* buf, short len, unsigned base, bool* ok)
{
    return int_froms(buf, len, SHRT_MAX, SHRT_MIN, base, ok);
}

int
stoi(const Char* buf, int len, unsigned base, bool* ok)
{
    return int_froms(buf, len, INT_MAX, INT_MIN, base, ok);
}

long
stoi(const Char* buf, long len, unsigned base, bool* ok)
{
    return int_froms(buf, len, LONG_MAX, LONG_MIN, base, ok);
}

int64
stoi(const Char* buf, int64 len, unsigned base, bool* ok)
{
    return int_froms(buf, len, LLONG_MAX, LLONG_MIN, base, ok);
}

unsigned char
stoi(const Char* buf, unsigned char len, unsigned base, bool* ok)
{
    return int_froms(buf, len, UCHAR_MAX, 0, base, ok);
}

unsigned short
stoi(const Char* buf, unsigned short len, unsigned base, bool* ok)
{
    return int_froms(buf, len, USHRT_MAX, 0, base, ok);
}

unsigned int
stoi(const Char* buf, unsigned int len, unsigned base, bool* ok)
{
    return int_froms(buf, len, UINT_MAX, 0, base, ok);
}

unsigned long
stoi(const Char* buf, unsigned long len, unsigned base, bool* ok)
{
    return int_froms(buf, len, ULONG_MAX, 0, base, ok);
}

uint64
stoi(const Char* buf, uint64 len, unsigned base, bool* ok)
{
    return int_froms(buf, len, ULLONG_MAX, 0, base, ok);
}

double
stod(const char* buf, unsigned len, okguard& okg)
{
    return COMMON_NS::stod(buf, len, okg);
}

double
stod(const Char* buf, unsigned len, bool* ok)
{
    return COMMON_NS::stod(buf, buf + len, ok);
}

double
stod(const char* buf, unsigned len, bool* ok)
{
    return COMMON_NS::stod(buf, buf + len, ok);
}

#endif

