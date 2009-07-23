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
// Copyright (c) 2007 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#include "common/String.h"
#include "common/FlexString.h"
#include "common/StringCvt.h"
#include "common/OsEnv.h"

#include <stdlib.h>
#include <vector>

#if defined(_WIN32)
# define VC_EXTRALEAN
# include <windows.h>
#endif

namespace Common {

namespace os_env_private {

using namespace Common;

template<> String get_env<sizeof(Char)>(const void* name, unsigned len)
{
    ustring vName(reinterpret_cast<const Char*>(name), len);
#if defined(_WIN32)
    std::vector<WCHAR> buf;

    DWORD n = 0;
    do {
        buf.resize(n += 128);
        n = GetEnvironmentVariableW(reinterpret_cast<LPCWSTR>(vName.c_str()),
                                    &buf[0], buf.size());
    } while (n && buf.size() <= n);
    return String(reinterpret_cast<const Char*>(&buf[0]), n);
#else
    return getenv(local_8bit(vName).c_str());
#endif
}

template<> String get_env<sizeof(char)>(const void* name, unsigned len)
{
    nstring vName(reinterpret_cast<const char*>(name), len);
    return getenv(vName.c_str());
}

}

}
