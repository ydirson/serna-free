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
#ifndef COMMON_RANGE_STRING_H_
#define COMMON_RANGE_STRING_H_

#include "common/String.h"
#include "common/StringUtils.h"

namespace Common {

// RangeString - range-based replacement for String for certain cases.
class RangeString : public Range<const Char*> {
public:
    typedef const Char* SI;
    typedef Range<SI> SR;

    RangeString(SI first, SI last)
        : SR(first, last) {}
    RangeString(SI first, const uint n)
        : SR(first, first + n) {}
    RangeString(const String& str = String::null())
        : SR(str.unicode(), str.unicode() + str.length()) {}
    
    RangeString     left(const uint n) const;
    RangeString     right(const uint n) const;
    RangeString     mid(const uint from, const int n = ~0) const;
    int             find(Char c, int index = 0) const;
    int             findRev(Char c, int index = -1) const;
    RangeString     stripTrailingWhitespace() const;

    Char            operator[](const int i) const;
    const Char*     unicode() const { return first; }
    operator String() const { return String(begin(), size(), true); }
    String          toString() const { return operator String(); }
    String          toString(int n) const { return String(first, n, true); }
    QString         toQString() const 
        { return QString::fromRawData(first, length()); }
    QString         toQString(int n) const
        { return QString::fromRawData(first, n); }
}; 

inline int RangeString::findRev(Char c, int index) const
{
    SI p = (index < 0) ? second : first + index;
    if (p >= second)
        p = second - 1;
    for (; p >= first; --p)
        if (c == *p)
            return p - first;
    return -1;
}

inline int RangeString::find(Char c, int index) const
{
    for (SI s = first + index; s < second; ++s)
        if (c == *s)
            return s - first;
    return -1;
}

inline Char RangeString::operator[](const int i) const 
{
    return ((first + i) >= second) ? Char() : *(first + i);
}

inline RangeString RangeString::left(const uint n) const
{
    return ((first + n) >= second) ? 
        RangeString(first, second) : RangeString(first, n);
}

inline RangeString RangeString::right(const uint n) const
{
    return ((second - n) < first) ?
        RangeString(first, second) : RangeString(second - n, second);
}

inline RangeString RangeString::mid(const uint from, const int n) const
{
    SI fp(first + from);
    SI ep(fp + n);
    if (fp > second)
        fp = second;
    if (n == ~0 || ep > second)
        ep = second;
    return RangeString(fp, ep);
}

inline RangeString RangeString::stripTrailingWhitespace() const
{
    SI p = second - 1;
    while (p >= first && p->isSpace())
        --p;
    return RangeString(first, ++p);
}

} // namespace Common

#endif // COMMON_RANGE_STRING_H_
