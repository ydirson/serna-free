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

#include "grove/grove_defs.h"
#include "grove/GroveUtils.h"
#include "grove/EntityReferenceTable.h"
#include "grove/Nodes.h"
#include "grove/SectionNodes.h"
#include "grove/Grove.h"
#include "common/String.h"

USING_COMMON_NS

namespace GroveLib {

/*! Quote string
 */
String quoteString(const String& s, bool escapeAll)
{
    String os;
    char rc = '"';
    const char* rcn = "&quot;";
    if (escapeAll) {
        os += rc;
        ulong sl = s.length();
        for (ulong i = 0; i < sl; ++i) {
            Char c = s[i];
            if (c == '\'')
                os += "&apos;";
            else if (c == '"')
                os += "&quot;";
            else
                os += c;
        }
        os += rc;
        return os;
    }
    ulong napos = s.contains('\'');
    ulong nquot = s.contains('"');
    if (0 == nquot) {
        os += '"';  os += s; os += '"';
        return os;
    }
    if (0 == napos) {
        os += '\''; os += s; os += '\'';
        return os;
    }
    if (nquot > napos) {
        rc  = '\'';
        rcn = "&apos;";
    }
    os += rc;
    ulong sl = s.length();
    for (ulong i = 0; i < sl; ++i) {
        if (s.at(i) == rc)
            os += rcn;
        else
            os += s.at(i);
    }
    os += rc;
    return os;
}

//////////////////////////////////////////////////////////////////

#ifndef _NDEBUG

GROVE_EXPIMP void check_grove_ert(Grove* g)
{
    EntityReferenceTable::ErtTable::iterator ertIter =
        g->document()->ert()->begin();
    for (; ertIter != g->document()->ert()->end(); ++ertIter) {
        EntityReferenceTable::ErtEntry& ere = *(*ertIter);
        for (uint i = 0; i < ere.numOfRefs(); ++i) {
            EntityReferenceStart* ers = ere.node(i);
            RT_ASSERT(ers);
            RT_ASSERT(ers->getRefCnt() && ers->getRefCnt() < 1000);
            if (ers->getGSR() != g->document()) {
                ers->dump();
                ers->getGSR()->dump();
                g->document()->dump();
            }
            RT_ASSERT(ers->getGSR() == g->document());
            RT_ASSERT(ere.decl());
            RT_ASSERT(ers->entityDecl());
        }
    }
}

#endif // _NDEBUG

} // namespace GroveLib
