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
#include "proputils/VariableResolver.h"
#include "common/PropertyTree.h"
#include "common/PathName.h"
#include "common/Vector.h"
#include "common/StringTokenizer.h"

USING_COMMON_NS;

namespace PropUtils {

static const char path_sep[] = { PathName::PATH_SEP, 0 };

bool resolveVariable(Vector<String>& result, const PropertyNode* ptree,
                     const String& is, const String& prefix)
{
    if (is.empty()) {
        result.push_back(is);
        return false;
    }
    String pref = prefix.lower();
    const Char* cp = is.data();
    const Char* ce = cp + is.length();
    String first, varname, second;
    for (;;) {
        for (; cp < ce && *cp != '$'; ++cp)
            first += *cp;
        if (cp >= ce) { // no variables
            result.push_back(is);
            return false;
        }
        const Char* ocp = cp++;
        while (cp < ce && !PathName::isDirsep(*cp))
            varname += to_lower(*cp++);
        if (varname.left(pref.length()) != pref) {
            first += String(ocp, cp - ocp);
            varname = String();
            continue;
        }
        while (cp < ce)
            second += *cp++;
        break;
    }
    if (varname.isEmpty()) {
        result.push_back(is);
        return false;
    }
    varname = varname.right(varname.length() - pref.length());
    // tokenize varname contents
    const PropertyNode* prop = ptree->getProperty(varname);
    String varval;
    if (prop)
        varval = prop->getString();
    else {
        result.push_back(first + second);
        return true;
    }
    for (StringTokenizer st(varval, path_sep); st; )
        result.push_back(first + st.next() + second);
    return true;
}

} // namespace PropUtils
