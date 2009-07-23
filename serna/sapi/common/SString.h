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
#ifndef SAPI_SSTRING_H_
#define SAPI_SSTRING_H_

#ifdef BUILD_SAPI
# include "common/String.h"
#endif // BUILD_SAPI

#include "sapi/sapi_defs.h"
#include "sapi/common/Char.h"
#include "sapi/common/WrappedObject.h"
#include <iosfwd>

class QString;

namespace SernaApi {

/// A wrapper for QString (QT Unicode string implementation).
class SAPI_EXPIMP SString {
public:
    typedef Char                value_type;
    typedef unsigned int        size_type;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef value_type*         iterator;
    typedef const value_type*   const_iterator;
    static  const size_type     npos = (size_type)(-1);

    /// Constructs null string
    SString();

    /// Constructs string from another string
    SString(const SString&);

    /// Construct String from Unicode buffer \a data with length \a len.
    SString(const value_type* data, size_type len);

    /// Create SString from ASCII string.
    SString(const unsigned char* data, size_type len);
    SString(const char* data);

    /// Construct SString from single Unicode character
    SString(Char c);

    /// Construct SString from QString
    SString(const QString&);

    ~SString();

    /// Resize string to the given length; pad end with c
    void            resize(size_type, value_type c = 0);

    /// ensures capacity of the string
    void            reserve(size_type new_cap = 0);

    /// Returns pointer to the start of Unicode data of the string (null terminated).
    const value_type*   c_str() const;
    /// Returns pointer to the start of Unicode data of the string.
    const value_type*   unicode() const;
    /// Returns pointer to the start of Unicode data of the string.
    const value_type*   data() const { return unicode(); }

    /// Returns length of the string
    size_type       length() const;

    /// Returns reference to the character at given position
    reference       at(size_type pos);
    /// Returns reference to the character at given position.
    const_reference at(size_type pos) const;

    reference       operator[](size_type pos) { return at(pos); }
    const_reference operator[](size_type pos) const { return at(pos); }

    /// Returns TRUE if the string is empty (has zero length)
    bool            isEmpty() const;
    /// Returns TRUE if the string is empty (has zero length)
    bool            empty() const { return isEmpty(); }

    /// Returns TRUE if the string is null. Note that null string
    /// is not the same as empty string (when S = "", S is empty, not null)
    bool            isNull() const;

    // 21.3.2 iterators:
    /// \return iterator that points to the beginning of string
    iterator begin();

    /// \return const iterator that points to the beginning of string
    const_iterator begin() const;

    /// \return iterator that points just beyond the end of string
    iterator end();

    /// \return const iterator that points just beyond the end of string
    const_iterator end() const;

    ////////////////////////////////////////////////////
    // append
    /// equivalent to the \code operator+=(\a str) \endcode
    SString&    append(const SString& str);

    /// appends \a n chars from \a str starting at \a pos
    SString&    append(const SString& str, size_type pos, size_type n);

    /// appends \a n chars from a sequence pointed to by \a s
    SString&    append(const value_type* s, size_type n);

    /// appends T::length(\a s) chars from a sequence pointed to by \a s
    SString&    append(const value_type* s);

    /// appends \a n \a c characters \code
    /// SString<char> s("ab");
    /// s.append(3, 'x'); \n s.c_str() == "abxxx"; \endcode
    SString&    append(size_type n, value_type c);

    /// appends \a c character, allows std::back_inserter<E> to be used \code
    /// std::istreambuf_iterator start(cin), end; \n SString<char> s;
    /// std::copy(start, end, back_inserter<char>(s)); \endcode
    /// inserts characters from cin into \a s until EOF is reached
    void        push_back(value_type c);

    /// appends sequence of elements designated by [\a first, \a last)
    SString&    append(const_iterator first, const_iterator last);

    //////////////////////////////////////////////////////
    // insert
    /// inserts contents of the string \a str at position \a pos
    SString& insert(size_type pos, const SString& str);

    /// \a n characters of \a str starting at \a pos2 will be inserted 
    /// at \a pos    //! \code SString<char> s("abcd"), str("1234");
    /// s.insert(1, str, 2, 2); s.c_str() == "a34bcd"; \endcode
    /// \throws std::out_of_range if \a pos1 > size() || \a pos2 > str.size()
    SString& insert(size_type pos1, const SString& str, size_type pos2,
                    size_type n);

    /// inserts \a n characters pointed to by \a s at position \a pos
    /// \throws std::out_of_range if \a pos > size()
    SString& insert(size_type pos, const value_type* s, size_type n);

    /// inserts T::length(\a s) characters pointed to by null-terminated
    /// sequence pointed to by \a s at position \a pos
    /// \throws std::out_of_range if \a pos > size()
    SString& insert(size_type pos, const value_type* s);

    /// inserts \a n characters \a c at position \a pos
    /// \throws std::out_of_range if \a pos > size()
    SString& insert(size_type pos, size_type n, value_type c);

    /// equivalent to \code insert(p - begin(), 1, c) \endcode
    iterator insert(iterator p, value_type c = value_type());

    /// equivalent to \code insert(p - begin(), n, c) \endcode
    void insert(iterator p, size_type n, value_type c);

    /// inserts copy of sequence designated by [\a first, \a last) at position
    /// corresponding to iterator \a p
    void insert(iterator p, const_iterator first, const_iterator last);

    //////////////////////////////////////////////////////
    // erase
    /// removes \a n characters starting at position \a pos
    /// \throws std::out_of_range if \a pos > size()
    SString& erase(size_type pos = 0, size_type n = npos);

    /// equivalent to \code erase(pos - begin(), 1); \endcode
    /// \return \a pos
    iterator erase(iterator pos);

    /// erases characters in range [\a first, \a last)
    /// \return \a first
    iterator erase(iterator first, iterator last);

    //////////////////////////////////////////////////////
    // replace
    /// replaces \a n chars starting at position \a pos by contents of \a str
    /// \throws std::out_of_range if \a pos > size()
    SString& replace(size_type pos, size_type n, const SString& str);

    /// replaces \a n1 chars starting at position \a pos1 by sequence of chars
    /// from range str[\a pos2, \a pos2 + \a n2)
    /// \throws std::out_of_range if \a pos1 > size() || \a pos2 > str.size()
    SString& replace(size_type pos1, size_type n1, const SString& str,
                         size_type pos2, size_type n2);

    /// replaces \a n1 chars starting at position \a pos by sequence of chars
    /// from range [\a s, \a s + \a n2)
    /// \throws std::out_of_range if \a pos > size()
    SString& replace(size_type pos, size_type n1, const value_type* s,
                         size_type n2);

    /// replaces \a n chars starting at position \a pos by T::length(\a s) chars    //! from null-terminated sequence pointed to by \a s
    /// \throws std::out_of_range if \a pos > size()

    SString& replace(size_type pos, size_type n, const value_type* s);
    /// replaces \a n1 chars starting at position \a pos by \a n2 chars \a c
    /// \throws std::out_of_range if \a pos > size()

    SString& replace(size_type pos, size_type n1, size_type n2,
                         value_type c);
    /// replaces sequence of chars designated by [\a i1, \a i2) by contents of
    /// the string \a str

    SString& replace(iterator i1, iterator i2, const SString& str);
    /// replaces sequence of chars designated by [\a i1, \a i2) by sequence of
    /// chars from range [\a s, \a s + \a n)

    SString& replace(iterator i1, iterator i2, const value_type* s,
                         size_type n);
    /// replaces sequence of chars designated by [\a i1, \a i2) by
    /// T::length(\a s) chars from null-terminated sequence pointed by
    /// \a s
    SString& replace(iterator i1, iterator i2, const value_type* s);

    /// replaces sequence of chars designated by [\a i1, \a i2) by
    /// \a n characters \a c
    SString& replace(iterator i1, iterator i2, size_type n, value_type c);

    /// replaces sequence of chars designated by [\a i1, \a i2) by
    /// sequence designated by [\a j1, \a j2)
    SString& replace(iterator i1, iterator i2, const_iterator j1,
                         const_iterator j2);

    /////////////////////////////////////////////////////
    // copy/swap
    /// copies \a n characters starting at position \a pos into the buffer
    /// pointed to by \a s
    size_type copy(value_type* s, size_type n, size_type pos = 0) const;

    //! swaps contents of the string with contents of \a rhs
    void swap(SString& rhs);

    /////////////////////////////////////////////////////
    // find operations
    /// finds the occurrence of \a str. search starts from position \a pos
    /// \return position where \a str occurs or \b npos if search fails
    size_type find(const SString& str, size_type pos = 0) const;

    /// finds the occurrence of sequence [\a s, \a s + \a n). search starts
    /// from position \a pos. \return position where \a str occurs or \b npos if    
    /// search fails
    size_type find(const value_type* s, size_type pos, size_type n) const;

    /// finds the occurrence of sequence [\a s, \a s + \a T::length(\a s)).
    /// search starts from position \a pos. \return position where \a str occurs    
    /// or \b npos if search fails
    size_type find(const value_type* s, size_type pos = 0) const;

    /// finds occurrence of char \a c, starting from position \a pos
    size_type find(value_type c, size_type pos = 0) const;

    /// finds backward for the occurrence of \a str. search starts from position    
    /// \a pos \return position where \a str occurs or \b npos if search fails
    size_type rfind(const SString& str, size_type pos = npos) const;

    /// finds backward the occurrence of sequence [\a s, \a s + \a n). search
    /// starts from position \a pos. \return position where \a str occurs or
    /// \b npos if search fails
    size_type rfind(const value_type* s, size_type pos, size_type n) const;

    /// finds backward the occurrence of sequence
    /// [\a s, \a s + \a T::length(\a s)). search starts from position \a pos.
    /// \return position where \a str occurs or \b npos if search fails
    size_type rfind(const value_type* s, size_type pos = npos) const;

    /// finds backward occurrence of char \a c, starting from position \a pos
    size_type rfind(value_type c, size_type pos = npos) const;

    /// finds the first occurrence of any character from \a str.
    /// search starts from position \a pos returns position where char occurs
    /// or \b npos if search fails
    size_type find_first_of(const SString& str, size_type pos = 0) const;

    /// finds the first occurrence of any character from [\a s, \a s + \a n).
    /// search starts from position \a pos. \return position where char occurs
    /// or \b npos if search fails
    size_type find_first_of(const value_type* s, size_type pos,
                            size_type n) const;
    /// finds the first occurrence of any character from sequence
    /// [\a s, \a s + \a T::length(\a s)). search starts from position \a pos.
    /// \return position where char occurs or \b npos if search fails
    size_type find_first_of(const value_type* s, size_type pos = 0) const;

    /// finds the first occurrence of char \a c, starting from \a pos
    size_type find_first_of(value_type c, size_type pos = 0) const;

    /// finds the last occurrence of any character from \a str.
    /// search starts from position \a pos returns position where char occurs
    /// or \b npos if search fails
    size_type find_last_of(const SString& str, size_type pos = npos) const;

    /// finds the last occurrence of any char from sequence [\a s, \a s + \a n).    /// search starts from position \a pos. \return position where char occurs
    /// or \b npos if search fails
    size_type find_last_of(const value_type* s, size_type pos,
                           size_type n) const;

    /// finds the last occurrence of any char from sequence
    /// [\a s, \a s + \a T::length(\a s))
    /// search starts from position \a pos. \return position where char occurs
    /// or \b npos if search fails
    size_type find_last_of(const value_type* s, size_type pos = npos) const;

    /// finds the last occurrence of char \a c, starting from \a pos
    size_type find_last_of(value_type c, size_type pos = npos) const;

    /// finds the first occurrence of char not from \a str. search starts from
    /// position \a pos \return position where char occurs or \b npos otherwise
    size_type find_first_not_of(const SString& str,
                                size_type pos = 0) const;

    /// finds the first occurrence of any char not from sequence
    /// [\a s, \a s + \a n).
    /// search starts from position \a pos. \return position where char occurs
    /// or \b npos if search fails
    size_type find_first_not_of(const value_type* s, size_type pos,
                                size_type n) const;

    /// finds the first occurrence of any character not from sequence
    /// [\a s, \a s + \a T::length(\a s)). search starts from position \a pos.
    /// \return position where char occurs or \b npos if search fails
    size_type find_first_not_of(const value_type* s, size_type pos = 0) const;

    /// finds the first occurrence of char that's not \a c, starting from \a pos    size_type find_first_not_of(value_type c, size_type pos = 0) const;
    /// finds the last occurrence of char not from \a str. search starts from
    /// position \a pos \return position where char occurs or \b npos otherwise
    size_type find_last_not_of(const SString& str,
                               size_type pos = npos) const;

    /// finds the last occurrence of any char not from sequence
    /// [\a s, \a s + \a n).
    /// search starts from position \a pos \return position where char occurs
    /// or \b npos if search fails
    size_type find_last_not_of(const value_type* s, size_type pos,
                               size_type n) const;

    /// finds the last occurrence of any character not from sequence
    /// [\a s, \a s + \a T::length(\a s)). search starts from position \a pos.
    /// \return position where char occurs or \b npos if search fails
    size_type find_last_not_of(const value_type* s, size_type pos = npos) const;

    /// finds the last occurrence of char that's not \a c, starting from \a pos
    size_type find_last_not_of(value_type c, size_type pos = npos) const;

    /// returns copy of \a n characters starting from position \a pos
    /// \throws std::out_of_range if \a pos > size()
    SString substr(size_type pos = 0, size_type n = npos) const;

    // comparison operations
    /// \return x such that x < 0 if \a str is greater than *this,
    /// x == 0 if \a str is equal to *this, and x > 0 if \a str is less than
    /// *this
    int compare(const SString& str) const;

    /// equivalent to
    /// \code SString(*this, pos, n).compare(str); \endcode
    int compare(size_type pos, size_type n, const SString& str) const;

    /// equivalent to \code
    /// SString(*this, pos1, n1).compare(SString(s, n2));
    /// \endcode
    int compare(size_type pos1, size_type n1, const value_type* s,
                size_type n2) const;

    /// equivalent to \code
    /// SString(*this, pos1, n1).compare(SString(str, pos2, n2));
    /// \endcode
    int compare(size_type pos1, size_type n1, const SString& str,
                size_type pos2, size_type n2) const;

    /// equivalent to \code compare(SString(s)); \endcode
    int compare(const value_type* s) const;

    /// returns string that's concatenation of *this and \a rhs
    SString     operator+(const SString& rhs) const;
    /// returns string that's concatenation of *this and \a s
    SString&    operator+=(const SString& s) { return append(s); }
    /// returns string that's concatenation of *this and \a s
    SString&    operator+=(const char* s) { return append(s); }
    /// returns string that's concatenation of *this and \a c
    SString&    operator+=(const value_type& c) { push_back(c); return *this; }

    /////////////////////////////////////////////////////////////////
    // Conversions

    /// Translate string to latin_1, with max size maxlen.
    /// This function always returns \a buf. If buf = 0, then internal
    /// static buffer is used (use with caution!)
    char*           toLatin1(char* buf = 0, size_type maxlen = 0) const;
    /// Translate string to utf8.
    char*           toUtf8(char* buf = 0, size_type maxlen = 0) const;
    /// Translate string to local8bit.
    char*           toLocal8Bit(char* buf = 0, size_type maxlen = 0) const;

    /// converts string to numbers
    short           toShort(bool *ok = 0, int base = 10) const;
    /// converts string to numbers
    unsigned short  toUShort(bool *ok = 0, int base = 10) const;
    /// converts string to numbers
    int             toInt(bool *ok = 0, int base = 10) const;
    /// converts string to numbers
    unsigned int    toUInt(bool *ok = 0, int base = 10) const;
    /// converts string to numbers
    long            toLong(bool *ok = 0, int base = 10) const;
    /// converts string to numbers
    unsigned long   toULong(bool *ok = 0, int base = 10) const;
    /// converts string to numbers
    float           toFloat(bool *ok = 0) const;
    /// converts string to numbers
    double          toDouble(bool *ok = 0) const;

    // converts number to string
    static SString  number(long, int base = 10);
    // converts number to string
    static SString  number(unsigned long, int base = 10);
    // converts number to string
    static SString  number(int, int base = 10);
    // converts number to string
    static SString  number(size_type, int base = 10);
    // converts number to string
    static SString  number(double, char f = 'g', int prec = 6);

    //////////////////////////////////////////////////////////////////
    /*!
        Returns a lowercase copy of the string.      
    */
    SString         lower() const;
    /// Returns an uppercase copy of the string.
    SString         upper() const;
    /*!
        Returns a string that has whitespace removed from the start and
        the end.
    
        Whitespace means any character for which Char::isSpace() returns
        TRUE. This includes Unicode characters with decimal values 9
        (TAB), 10 (LF), 11 (VT), 12 (FF), 13 (CR) and 32 (Space), and may
        also include other Unicode characters.
    
        See also: simplifyWhiteSpace()
    */
    
    SString         stripWhiteSpace() const;
    /*!
        Returns a string that has whitespace removed from the start and
        the end, and which has each sequence of internal whitespace
        replaced with a single space.
    
        Whitespace means any character for which Char::isSpace() returns
        TRUE. This includes Unicode characters with decimal values 9
        (TAB), 10 (LF), 11 (VT), 12 (FF), 13 (CR), and 32 (Space).
    
        See also: stripWhiteSpace()
    */
    SString         simplifyWhiteSpace() const;
    /*!
        Returns a substring that contains the \a len leftmost characters
        of the string.
    
        The whole string is returned if \a len exceeds the length of the
        string.
        See also: right(), mid(), isEmpty()
    */
    
    SString         left(size_type len)  const { return substr(0, len); }
    /*!
        Returns a string that contains the \a len rightmost characters of
        the string.
    
        If \a len is greater than the length of the string then the whole
        string is returned.
    
        \sa left(), mid(), isEmpty()
    */
    SString         right(size_type len) const;
    /*!
        Returns a string that contains the \a len characters of this
        string, starting at position \a index.
    
        Returns a null string if the string is empty or \a index is out of
        range. Returns the whole string from \a index if \a index + \a len
        exceeds the length of the string.
    
        \sa left(), right()
    */
    SString         mid(size_type index, size_type len = npos) const;
    
    operator QString() const;

#ifdef BUILD_SAPI
    operator Common::String() const;
    SString(const Common::String&);
#endif // BUILD_SAPI

    SString& operator=(const SString&);
    SString& operator=(const char*);

private:
    SAPI_PVT_CONTAINER(1)
};

///////////////////////////////////////////////////////////////////////

SAPI_EXPIMP std::ostream& operator<<(std::ostream& os, const SString& s);

inline bool operator< (const SString& s1, const SString& s2)
{
    return 0 > s1.compare(s2);
}

inline bool operator== (const SString& s1, const SString& s2)
{
    return 0 == s1.compare(s2);
}

inline bool operator!= (const SString &s1, const SString &s2)
{
    return !(s1 == s2);
}

inline bool operator<= (const SString &s1, const SString &s2)
{
    return s1.compare(s2) <= 0;
}

inline bool operator> (const SString &s1, const SString &s2)
{
    return s1.compare(s2) > 0;
}

inline bool operator>= (const SString &s1, const SString &s2)
{
    return s1.compare(s2) >= 0;
}

inline bool operator< (const SString& s1, const char* s2)
{
    return 0 > s1.compare(SString(s2));
}

inline bool operator== (const SString& s1, const char* s2)
{
    return 0 == s1.compare(SString(s2));
}

inline bool operator!= (const SString &s1, const char* s2)
{
    return !(s1 == s2);
}

inline bool operator<= (const SString &s1, const char* s2)
{
    return s1.compare(SString(s2)) <= 0;
}

inline bool operator> (const SString &s1, const char* s2)
{
    return s1.compare(SString(s2)) > 0;
}

inline bool operator>= (const SString &s1, const char* s2)
{
    return s1.compare(SString(s2)) >= 0;
}

/////////////////////////////////////////////////////////////////////////

inline SString operator+(const SString &s1, const char *s2)
{
    SString tmp(s1); tmp += SString(s2); return tmp;
}

inline SString operator+(const char *s1, const SString &s2)
{
    SString tmp = SString(s1); tmp += s2; return tmp;
}

inline SString operator+(const SString &s1, Char c2)
{
    SString tmp(s1); tmp += c2; return tmp;
}

inline SString operator+(const SString &s1, char c2)
{
    SString tmp(s1); tmp += c2; return tmp;
}

inline SString operator+(Char c1, const SString &s2)
{
    SString tmp; tmp += c1; tmp += s2; return tmp;
}

inline SString operator+(char c1, const SString &s2)
{
    SString tmp(c1); return tmp += s2;
}

inline bool is_null(const SString& s)
{
    return s.isNull();
}

} // namespace SernaApi

#endif // SAPI_SSTRING_H_

