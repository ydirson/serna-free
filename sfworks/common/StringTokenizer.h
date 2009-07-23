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
#ifndef COMMON_STRING_TOKENIZER_H_
#define COMMON_STRING_TOKENIZER_H_

#include "common/common_defs.h"
#include "common/String.h"


namespace COMMON_NS {

/*! A class which does tokenization of a string, provided a list of
 *  ASCII token separators. It is intended as a replacement for a
 *  QStringList.
 *
 *  Usage:
 *
 *   for (StringTokenizer t(str); t; )
 *      do_something(t.next());
 */
class StringTokenizer {
public:
    /// Note that constructor does not copy the string.
    StringTokenizer(const String& s,
                    const char* tokenList = " \t\r\n")
        : cp_(s.unicode()),
          ce_(s.unicode() + s.length()),
          tlist_(tokenList) {}

    operator bool() const { return cp_ < ce_; }
    String   next()
    {
        while (cp_ < ce_ && sep(*cp_))
            ++cp_;
        if (cp_ >= ce_)
            return String();
        const Char* pcp = cp_;
        while (cp_ < ce_ && !sep(*cp_))
            ++cp_;
        if (cp_ == pcp)
            return String();
        return String(pcp, cp_ - pcp);
    }

private:
    bool    sep(const Char& c)
    {
        for (const char* tp = tlist_; *tp; ++tp)
            if (c.unicode() == *tp)
                return true;
        return false;
    }
    const Char* cp_;
    const Char* ce_;
    const char* tlist_;
};

} // end of namespace COMMON_NS

#endif // COMMON_STRING_TOKENIZER_H_
