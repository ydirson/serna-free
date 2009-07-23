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

#include "sys_deps.h"
#include "common/String.h"
#include "common/winlean.h"

#include "utils/env_utils.h"
#include "utils/file_utils.h"
#include "impl/file_utils_pvt.h"

#include <windows.h>
#include <shlobj.h>
#include <stdlib.h>
#include <vector>

using namespace Common;

// START_IGNORE_LITERALS

String full_exe_path(const String& rpath)
{
    std::vector<WCHAR> buf;
    DWORD nchars = 0;
    do {
        buf.resize(nchars += 128);
        nchars = GetModuleFileNameW(0, &buf[0], buf.size());
    } while(buf.size() <= nchars);
    return String(reinterpret_cast<Char*>(&buf[0]), nchars);
}

static inline LPCWSTR lpcwstr(const String& str)
{
    return reinterpret_cast<LPCWSTR>(str.c_str());
}

bool set_env(const Common::ustring& name, const Common::ustring& value)
{
    return 0 != SetEnvironmentVariableW(lpcwstr(name), lpcwstr(value));
}

bool unset_env(const Common::ustring& name)
{
    return 0 != SetEnvironmentVariableW(lpcwstr(name), NULL);
}

namespace FileUtils {

static bool shell_execute(const String& url, LPCWSTR verb, LPCWSTR progid = 0)
{
    SHELLEXECUTEINFOW ei = {0};
    ei.cbSize = sizeof(ei);
    ei.fMask = SEE_MASK_FLAG_NO_UI;
    ei.hwnd = HWND_DESKTOP;
    ei.lpVerb = verb;
    ei.lpFile = lpcwstr(url);
    ei.nShow = SW_SHOWNORMAL;
    if (progid) {
        HKEY key;
        LONG err = RegOpenKeyExW(HKEY_CLASSES_ROOT,  // handle to open key
                                 progid,             // subkey name
                                 0,                  // reserved
                                 KEY_READ,           // desired access
                                 &key);              // pointer to key
        if (err != ERROR_SUCCESS)
            return false;
        ei.fMask |= SEE_MASK_CLASSKEY;
        ei.hkeyClass = key;
    }
    return ShellExecuteExW(&ei);
}

LaunchCode os_launch_handler_app(const String& url, const String& type,
                                 const String& action)
{
    if (strip_white_space(url).empty())
        return HANDLER_BADURL;

    nstring scheme(to_lower<nstring>(url.substr(0, sizeof("http://") - 1)));
    if (scheme != "http://") {
        scheme = to_lower<nstring>(url.substr(0, sizeof("ftp://") - 1));
        if (scheme != "ftp://") {
            if (!action.empty() && shell_execute(url, lpcwstr(action)))
                return HANDLER_OK;
            if (shell_execute(url, L"opennew"))
                return HANDLER_OK;
            if (shell_execute(url, L"open"))
                return HANDLER_OK;
            return HANDLER_ERROR;
        }
    }

    if (!action.empty() && shell_execute(url, lpcwstr(action), L"htmlfile"))
        return HANDLER_OK;

    if (shell_execute(url, L"opennew", L"htmlfile"))
        return HANDLER_OK;

    if (shell_execute(url, L"open", L"htmlfile"))
        return HANDLER_OK;

    return HANDLER_ERROR;
}

String get_default_doc_dir()
{
    String path;
    path.resize(MAX_PATH);
    bool rv = SHGetSpecialFolderPathW(0,                // hwndOwner
                                      (LPWSTR)&path[0], // dst buffer
                                      CSIDL_PERSONAL,   // My Documents CSIDL
                                      false);           // create if not exist
    if (rv) {
        int end = path.find(QChar('\0'));
        if (-1 != end)
            return String(&path[0], end + 1);
    }
    return String();
}

}

