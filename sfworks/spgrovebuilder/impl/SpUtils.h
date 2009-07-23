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
/*! \file
 */

#ifndef SP_UTILS_H_
#define SP_UTILS_H_

#include "spgrovebuilder/spgrovebuilder_defs.h"
#include "sp/config.h"
#include "sp/StringC.h"
#include "grove/grove_defs.h"
#include "common/String.h"
#include "grove/Origin.h"

namespace Sp {

class Location;
class Message;

} // end of namespace Sp

GROVE_NAMESPACE_BEGIN

/*! Miscellaneous common functions for providing interface to Sp.
 */
class SpUtils {
public:
    static const Sp::Char SPCR = '\015';
    static inline void assignString(COMMON_NS::String& s, const Sp::Char* base,
                                    ulong length)
    {
        s.reserve(length);
        const Sp::Char* it = base, *end = it + length;
        for (; it != end; ++it) {
            if (SPCR == *it)
                continue;
            if ('\t' == *it)
                s.push_back(' ');
            else
                s.push_back(QChar(*it));
        }
    }
    //!
    static inline COMMON_NS::String makeString(const Sp::StringC& s1)
    {
        if (unsigned const sz = s1.size()) {
            COMMON_NS::String str;
            assignString(str, s1.data(), sz);
            return str;
        }
        return COMMON_NS::String::null();
    }

    static PlainOrigin extractSourceLocation(const Sp::Location& loc);
    static COMMON_NS::String extractFormattedMessage(const Sp::Message& msg);
};

GROVE_NAMESPACE_END

#endif // SP_UTILS_H_
