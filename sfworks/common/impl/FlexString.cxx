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

#if defined(_MSC_VER) && !defined(FS_TEMPLATE_INST)
# define FS_TEMPLATE_INST
#endif

#include "common/common_defs.h"
#include "common/FlexString.h"
#include "common/String.h"
#include <QString> // get QChar from there
#include <stdexcept>
#include <iostream>
#include <string>
#include <string.h>

COMMON_NS_BEGIN

#ifdef _MSC_VER

template class COMMON_EXPIMP flex_string<Char>;
template class COMMON_EXPIMP flex_string<char>;

#endif

using namespace std;

template<class StrType> ostream& output(ostream& os, const StrType& s)
{
#if defined(_MSC_VER) && (_MSC_VER < 1300)
    string str;
    str.reserve(s.size());
    for (unsigned i = 0; i < s.size(); ++i)
        str.append(1, s[i]);
    return os << str;
#else
    return os << string(s.begin(), s.end());
#endif
}

ostream& output(ostream& os, const ustring& s)
{
    for (unsigned i = 0; i < s.size(); ++i)
        os.rdbuf()->sputc(s[i].toLatin1());
    return os;
}

template<class StrType> istream& input(istream& os, StrType& s)
{
    string istr;
    os >> istr;
    if (0 < istr.size())
        s.assign(istr.begin(), istr.end());
    return os;
}

template<class StrType> istream& get_line(istream& is, StrType& s, char delim)
{
    string istr;
    std::getline(is, istr, delim);
    if (0 < istr.size())
        s.assign(istr.begin(), istr.end());
    return is;
}

template<class StrType> istream& get_line(istream& is, StrType& s)
{
    string istr;
    std::getline(is, istr);
    if (0 < istr.size())
        s.assign(istr.begin(), istr.end());
    return is;
}

ostream&
operator << (ostream& os, const ustring& s)
{
    return output(os, s);
}

istream&
operator >> (istream& os, ustring& s)
{
    return input(os, s);
}

istream&
getline(istream& is, ustring& s, char delim)
{
    return get_line(is, s, delim);
}

istream&
getline(istream& is, ustring& s)
{
    return get_line(is, s);
}

ostream&
operator << (ostream& os, const nstring& s)
{
    return output(os, s);
}

istream&
operator >> (istream& os, nstring& s)
{
    return input(os, s);
}

istream&
getline(istream& is, nstring& s, char delim)
{
    return get_line(is, s, delim);
}

istream&
getline(istream& is, nstring& s)
{
    return get_line(is, s);
}

COMMON_NS_END

namespace StringPrivate {

void throw_length_error()
{
    throw std::length_error("String operation resulted in length error");
}

void throw_out_of_range()
{
    throw std::length_error("String operation resulted in out of range");
}

}
