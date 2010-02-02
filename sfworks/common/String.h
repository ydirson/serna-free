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
 #ifndef STRING_H_
#define STRING_H_

#include "common/common_defs.h"
#include "common/StringDecl.h"
#include "common/FlexString.h"
#include "common/StringUtils.h"
#include "common/StringCmp.h"

#include <QString>
#include <iosfwd>
#include <string>

#define QSREF(s) QString::fromRawData((const QChar*)((s).unicode()), \
                                      (s).length())

class QRegExp;

COMMON_NS_BEGIN

class COMMON_EXPIMP String : public ustring {
    typedef ustring base_type;
public:
    // types
    typedef base_type::traits_type traits_type;
    typedef traits_type::char_type value_type;

    typedef base_type::allocator_type allocator_type;
    typedef base_type::size_type size_type;
    typedef allocator_type::difference_type difference_type;

    typedef allocator_type::reference reference;
    typedef allocator_type::const_reference const_reference;
    typedef allocator_type::pointer pointer;
    typedef allocator_type::const_pointer const_pointer;

//  ctors/dtor
    String() {}
    String(const ustring& ustr) : ustring(ustr) {}
    String(const String& s) : base_type(s) {}
    String(const QString& s);
    String(const std::string& s) : base_type(s.begin(), s.end()) {}

    String(const value_type* unicode, unsigned long len, bool = true)
        : base_type(unicode, len) {}
    String(const value_type* unicode) : base_type(unicode) {}
    String(value_type ch) : base_type(1, ch) {}

//  narrow character/string constructors
    String(const char* s, size_type len);
    String(const char* b, const char* e) : base_type(b, e) {}
    String(const char* s);
    String(size_type len, char ch);

    ~String() {}

    static const String& null();

//  public interface a-la Qt string
    bool    isEmpty() const { return size() == 0; }
    bool    isNull() const { return is_null(); }
    void    truncate(size_type pos) { resize(pos, value_type(0)); }
    void    setLength(size_type newLength) { resize(newLength); }

    String& fill(value_type c, int len = -1)
    { assign((len < 0 ? length() : len), c);  return *this; }

    int find(value_type c, int ind = 0, bool cs = true) const;
    int find(const char* s, int ind = 0, bool cs = true) const;
    int find(const String& s, int ind = 0, bool cs = true) const;

    int findRev(value_type c, int ind = -1, bool cs = true) const;
    int findRev(const char* s, int ind = -1, bool cs = true) const;
    int findRev(const String& s, int ind = -1, bool cs = true) const;

    int contains(value_type c, bool cs = true) const;
    int contains(const char* s, bool cs = true) const;
    int contains(const String& s, bool cs = true) const;

    String  left(size_type len)  const { return substr(0, len); }
    String  right(size_type len) const;
    String  mid(size_type index, size_type len = npos) const;

    String  leftJustify(size_type width, value_type fill = ' ',
                         bool trunc = false) const;
    String  rightJustify(size_type width, value_type fill = ' ',
                         bool trunc = false) const;

    String  lower() const;
    String  upper() const;

    String  stripWhiteSpace() const;
    String  simplifyWhiteSpace() const;

    String& operator+=(value_type c) { return append(c); }
    String& operator+=(const char* s) { return append(String(s)); }
    String& operator+=(const String &str) { return append(str); }
    String& operator+=(const QString &s)
    { base_type::append(s.unicode(), s.length()); return *this; }

    String& insert(size_type index, char c);
    String& insert(size_type index, const char*);
    String& insert(size_type index, value_type c);
    String& insert(size_type index, const value_type*, size_type len);
    String& insert(size_type index, const String&);

    String& append(char c);
    String& append(const char* s);
    String& append(value_type c);
    String& append(const String& s);

    String& prepend(char c) { return insert(0, value_type(c)); }
    String& prepend(value_type c) { return insert(0, c); }
    String& prepend(const String& s) { return insert(0, s); }
    String& prepend(const char* s) { return insert(0, s); }

    String& remove(size_type ind, size_type l) { erase(ind, l); return *this; }

    String& replace(size_type index, size_type len, const value_type* s,
                    size_type clen)
    { base_type::replace(index, len, s, clen); return *this; }

    String& replace(size_type index, size_type len, const String& s)
    { return replace(index, len, s.data(), s.length()); }

    String& replace(size_type index, size_type len, const char* s)
    { return replace(index, len, String(s)); }

    String& replace(const String& pat, const String& to, bool cs = true);

    String& replace(const QRegExp& qre, const String& to);

    short toShort (bool *ok = 0, int base = 10) const;
    unsigned short toUShort(bool *ok = 0, int base = 10) const;
    int toInt (bool *ok = 0, int base = 10) const;
    unsigned int toUInt (bool *ok = 0, int base = 10) const;
    long toLong (bool *ok = 0, int base = 10) const;
    void* toPtr(bool *ok = 0, int base = 10) const;
    unsigned long toULong (bool *ok = 0, int base = 10) const;
    float toFloat (bool *ok = 0) const;
    double toDouble(bool *ok = 0) const;

    String& setNum(long, int base=10);
    String& setNum(unsigned long, int base=10);
    String& setNum(double, char f = 'g', int prec = 6);
    String& setNum(short n, int base=10) { return setNum((long)n, base); }
    String& setNum(unsigned short n, int base=10) { return setNum((unsigned long)n, base); }
    String& setNum(int n, int base=10) { return setNum((long)n, base); }
    String& setNum(unsigned int n, int base=10) { return setNum((unsigned long)n, base); }
    String& setNum(float n, char f = 'g', int prec = 6)
    {
        return setNum((double)n, f, prec);
    }
    static String number(long, int base = 10);
    static String number(unsigned long, int base = 10);
    static String number(int, int base = 10);
    static String number(size_type, int base = 10);
    static String number(void*, int base = 10);
    static String number(double, char f = 'g', int prec = 6);

    static int compare(const String& s1, const String& s2);
    int compare(const String& s) const
    {
        return this->base_type::compare(s);
    }
    int compare(const char* s) const;

//    value_type& at(size_type i) const;
    using base_type::at;
    using base_type::operator[];
    // non-checking accessor
//    const value_type& operator[](size_type i) const { return this->base_type::operator[](i); }
//    value_type& operator[](size_type i) { return this->base_type::operator[](i); }
    value_type& ref(size_type i) { return this->at(i); }
    const value_type* unicode() const { return data(); }
    using base_type::length;
    using base_type::data;
/*
    bool operator==(const String& other)
    {
        return static_cast<const base_type&>(*this) ==
               static_cast<const base_type&>(other);
    }
*/
    using base_type::erase;
    using base_type::assign;
    String& append(size_type l, Char c)
    {
        base_type::append(l, c);
        return *this;
    }
    String& append(const Char* p, size_type l)
    {
        base_type::append(p, l);
        return *this;
    }
//    using base_type::append;

    static String fromLatin1(const char*, int len = -1);
    static String fromLocal8Bit(const char*, int len = -1);

    nstring local8Bit() const;
    nstring utf8() const;
    nstring latin1() const;

    operator QString() const;
    String& operator=(const QString&);
//    String& operator=(const String& s) { assign(s); return *this; }
    String& operator=(const char* s)
    {
        const size_type sz = strlen(s);
        resize(sz);
        std::copy(s, s + sz, begin());
        return *this;
    }
    String& operator=(value_type c) { assign(1, c); return *this; }
    String& operator=(char c)       { assign(1, c); return *this; }

    QString qstring() const { return operator QString(); }
    String& setUnicode(const value_type* unicode, uint len);
    String& setLatin1(const char*, int len = -1);
};

COMMON_EXPIMP std::ostream& operator<<(std::ostream& os, const String& s);
COMMON_EXPIMP std::istream& operator>>(std::istream& os, const String& s);

inline bool operator< (const String& s1, const String& s2)
{
    return 0 > String::compare(s1, s2);
}

inline bool operator== (const String& s1, const String& s2)
{
    return 0 == String::compare(s1, s2);
}

inline bool operator!= (const String &s1, const String &s2)
{
    return !(s1 == s2);
}

inline bool operator<= (const String &s1, const String &s2)
{
    return String::compare(s1, s2) <= 0;
}

inline bool operator> (const String &s1, const String &s2)
{
    return String::compare(s1, s2) > 0;
}

inline bool operator>= (const String &s1, const String &s2)
{
    return String::compare(s1, s2) >= 0;
}

/////

inline bool operator==(const String &s1, const char *s2)
{
    return static_cast<const ustring&>(s1) == s2;
}

inline bool operator!=(const String &s1, const char *s2)
{
    return static_cast<const ustring&>(s1) != s2;
}

inline bool operator<(const String &s1, const char *s2)
{
    return static_cast<const ustring&>(s1) < s2;
}

inline bool operator<=(const String &s1, const char *s2)
{
    return static_cast<const ustring&>(s1) <= s2;
}

inline bool operator>(const String &s1, const char *s2)
{
    return static_cast<const ustring&>(s1) > s2;
}

inline bool operator>=(const String& s1, const char *s2)
{
    return static_cast<const ustring&>(s1) >= s2;
}

inline bool operator!=(const char* s1, const String &s2)
{
    return !(s2 == s1);
}

inline bool operator==(const char* s1, const String &s2)
{
    return (s2 == s1);
}

inline bool operator<(const char* s1, const String &s2)
{
    return s2 > s1;
}

inline bool operator>(const char* s1, const String &s2)
{
    return s2 < s1;
}

inline bool operator<=(const char* s1, const String &s2)
{
    return s2 >= s1;
}

inline bool operator>=(const char* s1, const String &s2)
{
    return s2 <= s1;
}

/////

inline bool operator== (const String& lhs, const QString& rhs)
{
    return str_eq(lhs, rhs);
}

inline bool operator!= (const String& lhs, const QString& rhs)
{
    return !(lhs == rhs);
}

inline bool operator< (const String& lhs, const QString& rhs)
{
    return str_lt(lhs, rhs);
}

inline bool operator> (const String& lhs, const QString& rhs)
{
    return str_gt(lhs, rhs);
}

inline bool operator<= (const String& lhs, const QString& rhs)
{
    return !(lhs > rhs);
}

inline bool operator>= (const String& lhs, const QString& rhs)
{
    return !(lhs < rhs);
}

// --

inline bool operator== (const QString& lhs, const String& rhs)
{
    return str_eq(lhs, rhs);
}

inline bool operator!= (const QString& lhs, const String& rhs)
{
    return !(lhs == rhs);
}

inline bool operator< (const QString& lhs, const String& rhs)
{
    return str_lt(lhs, rhs);
}

inline bool operator> (const QString& lhs, const String& rhs)
{
    return str_gt(lhs, rhs);
}

inline bool operator<= (const QString& lhs, const String& rhs)
{
    return !(lhs > rhs);
}

inline bool operator>= (const QString& lhs, const String& rhs)
{
    return !(lhs < rhs);
}

/////

inline String operator+(const String &s1, const String &s2)
{
    return String(s1) += s2;
}

inline String operator+(const String &s1, const char *s2)
{
    String tmp(s1); tmp += String(s2); return tmp;
}

inline String operator+(const char *s1, const String &s2)
{
    String tmp = String(s1); tmp += s2; return tmp;
}

inline String operator+(const String &s1, Char c2)
{
    String tmp(s1); tmp += c2; return tmp;
}

inline String operator+(const String &s1, char c2)
{
    String tmp(s1); tmp += c2; return tmp;
}

inline String operator+(Char c1, const String &s2)
{
    String tmp; tmp += c1; tmp += s2; return tmp;
}

inline String operator+(char c1, const String &s2)
{
    String tmp(c1); return tmp += s2;
}

inline bool is_null(const String& s)
{
    return is_null(static_cast<const ustring&>(s));
}

#if defined(_MSC_VER) && (_MSC_VER < 1400)

///// Comparison operators for Char and char types

namespace CharPrivate {

template<typename CharType> typename CHAR_TRAITS_TYPE(CharType)::int_type
to_int(CharType c)
{
    return CHAR_TRAITS_TYPE(CharType)::to_int_type(c);
}

}

inline bool operator==(Char lhs, char rhs)
{
    using namespace CharPrivate;
    return to_int(lhs) == to_int(rhs);
}

inline bool operator!=(Char lhs, char rhs)
{
    using namespace CharPrivate;
    return to_int(lhs) != to_int(rhs);
}

inline bool operator<(Char lhs, char rhs)
{
    using namespace CharPrivate;
    return to_int(lhs) < to_int(rhs);
}

inline bool operator<=(Char lhs, char rhs)
{
    using namespace CharPrivate;
    return to_int(lhs) <= to_int(rhs);
}

inline bool operator>(Char lhs, char rhs)
{
    using namespace CharPrivate;
    return to_int(lhs) > to_int(rhs);
}

inline bool operator>=(Char lhs, char rhs)
{
    using namespace CharPrivate;
    return to_int(lhs) >= to_int(rhs);
}

//------

inline bool operator==(char lhs, Char rhs)
{
    using namespace CharPrivate;
    return to_int(lhs) == to_int(rhs);
}

inline bool operator!=(char lhs, Char rhs)
{
    using namespace CharPrivate;
    return to_int(lhs) != to_int(rhs);
}

inline bool operator<(char lhs, Char rhs)
{
    using namespace CharPrivate;
    return to_int(lhs) < to_int(rhs);
}

inline bool operator<=(char lhs, Char rhs)
{
    using namespace CharPrivate;
    return to_int(lhs) <= to_int(rhs);
}

inline bool operator>(char lhs, Char rhs)
{
    using namespace CharPrivate;
    return to_int(lhs) > to_int(rhs);
}

inline bool operator>=(char lhs, Char rhs)
{
    using namespace CharPrivate;
    return to_int(lhs) >= to_int(rhs);
}

/////

#endif

class DebugOutStream;

COMMON_EXPIMP const DebugOutStream&
operator << (const DebugOutStream&, const String&);

COMMON_NS_END

#endif
