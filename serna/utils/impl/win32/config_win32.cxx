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

#if defined(_WIN32)

#include "common/winlean.h"
#include "common/String.h"
#include "common/FlexString.h"
#include "common/PathName.h"
#include "config_win32.h"
#include "utils_debug.h"
#include "utils/appver.hpp"
#include "reg_utils.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <windows.h>
#include <shellapi.h>

#ifdef DBG_DEFAULT_TAG
# undef DBG_DEFAULT_TAG
#endif

#define DBG_DEFAULT_TAG UTILS.CONFIG

using namespace std;

#if !defined(SERNA_NAME)
# define SERNA_NAME "Serna Free"
#endif

static const TCHAR keyname[] = "Software\\Syntext\\" SERNA_NAME "\\" SERNA_VERSION_ID;

static bool
get_startupdir(TCharBuf& ddir)
{
    LONG sz = 128;
    LONG r;
    do {
        sz += 64;
        ddir.resize(sz);
        r = GetModuleFileName(0, &ddir[0], sz);
    } while (0 == r && ERROR_MORE_DATA == GetLastError());
    ddir.resize(sz);
    if (!ddir.empty()) {
        TCharBuf::reverse_iterator rb = ddir.rbegin(), re = ddir.rend(),
                                   rit = find(rb, re, '\\');
        if (re != rit && re != ++rit)
            if (re != (rit = find(rit, re, '\\')))
                ddir.erase((++rit).base(), ddir.end());
    }
    DDBG << "StartupDir == " << ddir << endl;
    return 0 < ddir.size();
}


static void query_instdir(const TCharBuf& nm, TCharBuf& dst)
{
    TCharBuf val;
    bool ok = reg_query_value(HKEY_LOCAL_MACHINE, keyname, &nm[0], val);
    DDBG << "Instdir query: '" << val << '\'' << endl;
    if (ok)
        val.swap(dst);
}

namespace cfg {

USING_COMMON_NS

void set_instdir(const TCharBuf& nm, const TCharBuf& val)
{
    if (val.empty() || nm.empty())
        return;

    HKEY key;
    LONG err = RegCreateKeyEx(HKEY_LOCAL_MACHINE,   // handle to open key
                              keyname,              // subkey name
                              0,                    // reserved
                              0,                    // class string
                              REG_OPTION_NON_VOLATILE,
                              KEY_SET_VALUE,        // desired access
                              0,                    // security attributes
                              &key,                 // pointer to key
                              0);
    if (err == ERROR_SUCCESS)
        RegSetValueEx(key, &nm[0], 0, REG_EXPAND_SZ, (CONST BYTE*)&val[0],
                      val.size() * sizeof(val[0]));
    RegCloseKey(key);
    DDBG << "Set instdir = " << (err == ERROR_SUCCESS)
         << " value set == " << val << endl;
}

PathName
get_win32_instdir(const char* varname)
{
    unsigned varlen;
    PathName rv;
    if (0 == varname || 1 == (varlen = strlen(varname) + 1))
        return rv;

    TCharBuf datadir, nm;
    nm.reserve(varlen);
    copy(varname, varname + varlen, back_inserter(nm));
    query_instdir(nm, datadir);
    if (datadir.empty()) {
        if (get_startupdir(datadir)) {
            if ('\0' != *(datadir.rbegin()))
                datadir.push_back('\0');
            set_instdir(nm, datadir);
        }
    }
    PathName::base_type tmp;
    tmp.reserve(datadir.size());
    for (TCharBuf::const_iterator it = datadir.begin(); '\0' != *it; ++it)
        tmp.append(1, *it);
    DDBG << "get_win32_instdir returned '" << tmp << '\'' << endl;
    return rv.assign(tmp);
}

static const TCHAR browser_key[] = "http\\shell\\open\\command";

PathName get_win32_browser()
{
    TCharBuf bp;
    PathName::base_type result;

    bool ok = reg_query_value(HKEY_CLASSES_ROOT, browser_key, 0, bp);
    DDBG << "Browser query: '" << bp << '\'' << endl;
    if (!ok)
        return result;

    const TCHAR EXE[] = ".exe";
    unsigned const bp_sz = bp.size();
    if (0 < bp_sz) {
        TCharBuf::iterator first = bp.begin();
        if ('"' == *first)
            ++first;
        TCharBuf::iterator bp_end = bp.end();
        TCharBuf::iterator last = find_end(first, bp_end,
                                           EXE, EXE + sizeof(EXE) - 1);
        if (bp_end != last)
            last += sizeof(EXE) - 1;
        else
            if ('"' != *(--last))
                ++last;
        result.assign(first, last);
    }
    return result;
}

static const TCHAR key_sec_a[] = "SOFTWARE\\Classes\\";
static const TCHAR key_sec_b[] = "\\shell\\open\\command";

PathName get_registered_viewer_path(const char* ext)
{
    PathName::base_type rv;
    TCharBuf ext_key(key_sec_a, key_sec_a + sizeof key_sec_a - 1), value;
    ext_key.insert(ext_key.end(), ext, ext + strlen(ext) + 1);
    if (!reg_query_value(HKEY_LOCAL_MACHINE, &ext_key[0], 0, value))
        return rv;

    TCharBuf app_key(key_sec_a, key_sec_a + sizeof key_sec_a - 1);
    app_key.insert(app_key.end(), value.begin(), value.end());
    app_key.insert(app_key.end() - 1, key_sec_b, key_sec_b + sizeof key_sec_b);

    if (!reg_query_value(HKEY_LOCAL_MACHINE, &app_key[0], 0, value))
        return rv;
    if ('"' == value[0]) {
        TCharBuf::iterator begin(value.begin() + 1), end(value.end());
        TCharBuf::iterator it = find(begin, end, '"');
        if (value.end() == it)
            it = end;
        rv.assign(begin, it);
    }
    else {
        TCharBuf::iterator begin(value.begin()), end(value.end());
        TCharBuf::iterator it = find(begin, end, ' ');
        if (value.end() == it)
            it = end;
        rv.assign(begin, it);
    }

    String path(rv);
    int per_pos = path.find("%");
    while (0 <= per_pos) {
        int pos = path.find("%", per_pos + 1);
        if (0 <= pos) {
            String env = path.mid(per_pos + 1, pos - per_pos - 1);
            pos++;
            String str = getenv(env.local8Bit().c_str());
            if (!str.isEmpty()) {
                path = path.replace("%" + env + "%", str);
                pos = 0;
            }
        }
        else
            pos = per_pos + 1;
        per_pos = path.find("%", pos);
    }
    return PathName(path);
}

}

#endif
