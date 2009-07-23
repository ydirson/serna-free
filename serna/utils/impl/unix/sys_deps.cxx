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
#include "common/StringCvt.h"
#include "common/PathName.h"
#include "utils/Properties.h"
#include "utils/env_utils.h"
#include "utils/file_utils.h"
#include <stdlib.h>
#include <memory>

using namespace Common;

// START_IGNORE_LITERALS

String full_exe_path(const String& rpath)
{
    if (!PathName::isRelative(rpath))
        return rpath;

    PathName pn;
    if (rpath.contains(PathName::DIR_SEP)) {
        if (char* cwd = getcwd(0, 0)) {
            pn.assign(from_local_8bit(cwd));
            free(cwd);
            pn.append(rpath);
            pn = pn.normPath();
            if (pn.exists())
                return pn.name();
        }
    }
    ustring upath(from_local_8bit(getenv("PATH")));
    pn = PathName::searchPath(rpath, upath);
    if (pn.empty())
        return rpath;
    return pn.name();
}

bool set_env(const Common::ustring& name, const Common::ustring& value)
{
#if defined(linux)
    return 0 == setenv(local_8bit(name).c_str(), local_8bit(value).c_str(), 1);
#elif defined(sun)
    nstring nameStr(local_8bit(name)), valStr(local_8bit(value));
    const unsigned nameSize = nameStr.size(), valSize = valStr.size();
    char* env = reinterpret_cast<char*>(malloc(nameSize + valSize + 2));
    env = std::uninitialized_copy(nameStr.begin(), nameStr.end(), env);
    *env++ = '=';
    env = std::uninitialized_copy(valStr.begin(), valStr.end(), env);
    *env = '\0';
    return 0 == putenv(env - nameSize - valSize - 1);
#endif
}

bool unset_env(const Common::ustring& name)
{
#if defined(linux)
    return 0 == unsetenv(local_8bit(name).c_str());
#elif defined(sun)
    nstring nameStr(local_8bit(name));
    const unsigned nameSize = nameStr.size();
    char* env = reinterpret_cast<char*>(malloc(nameSize + 2));
    env = std::uninitialized_copy(nameStr.begin(), nameStr.end(), env);
    *env++ = '=';
    *env = '\0';
    return 0 == putenv(env - nameSize - 1);
#endif
}

namespace FileUtils {

LaunchCode os_launch_handler_app(const String&, const String&, const String&)
{
    return HANDLER_UNDEF;
}

}


