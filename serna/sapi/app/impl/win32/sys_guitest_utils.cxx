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
#include "sapi/app/guitest_utils.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>

using namespace SernaApi;
using namespace std;

SString GuiTestUtils::getShortPathName(const SString& longName)
{
    const int len = longName.length();
    vector<wchar_t> buf(len);
    DWORD rc = GetShortPathNameW((LPWSTR)longName.c_str(),
                                 (LPWSTR)&buf[0], len);
    if (0 == rc)
        return SString();
        
    return SString((SString::value_type*)&buf[0], rc);
}

SString GuiTestUtils::getLongPathName(const SString& shortName)
{
    vector<wchar_t> buf;
    DWORD rc = 0;
    do {
        buf.resize(buf.size() + MAX_PATH);
        DWORD rc = GetLongPathNameW((LPWSTR)shortName.c_str(),
                                    (LPWSTR)&buf[0], buf.size());
    } while (0 != rc && rc > buf.size());
    
    if (0 == rc)
        return shortName;

    return SString((SString::value_type*)&buf[0], rc);
}
