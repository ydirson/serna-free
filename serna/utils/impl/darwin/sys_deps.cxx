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
#include "common/Url.h"
#include "common/ScopeGuard.h"

#include "utils/Properties.h"
#include "utils/env_utils.h"
#include "utils/file_utils.h"
#include <stdlib.h>

#include <qfileinfo.h>
#include <vector>

#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>

using namespace Common;

namespace Common {
class PropertyNode;
}

// START_IGNORE_LITERALS

String full_exe_path(const String&)
{
    CFURLRef exe_ref = CFBundleCopyExecutableURL(CFBundleGetMainBundle());
    CFStringRef path = CFURLCopyFileSystemPath(exe_ref, kCFURLPOSIXPathStyle);
    QFileInfo fi;
    if (const UniChar* local_path = CFStringGetCharactersPtr(path))
        fi.setFile(QString::fromUcs2(local_path));
    else {
        const unsigned len = CFStringGetLength(path);
        std::vector<UniChar> buf(len);
        CFStringGetCharacters(path, CFRangeMake(0, len), &buf[0]);
        buf.push_back(0);
        fi.setFile(QString::fromUcs2(&buf[0]));
    }
    return fi.absFilePath();
}

bool set_env(const Common::ustring& name, const Common::ustring& value)
{
    return 0 == setenv(local_8bit(name).c_str(), local_8bit(value).c_str(), 1);
}

bool unset_env(const Common::ustring& name)
{
#if defined(__DARWIN_UNIX_03)
    return 0 == unsetenv(local_8bit(name).c_str());
#else
    unsetenv(local_8bit(name).c_str());
    return true;
#endif
}

namespace FileUtils {

LaunchCode os_launch_handler_app(const String& url, const String&,
                                 const String&)
{
    Url absUrl(Url(url).absolute());
    CFURLRef cfUrl;
    String tmp(absUrl);
    const UniChar* unidata = reinterpret_cast<const UniChar*>(tmp.data());
    CFStringRef cfUrlString = CFStringCreateWithCharacters(NULL, unidata,
                                                           tmp.size());
    ON_BLOCK_EXIT(CFRelease, cfUrlString);
    if (absUrl.isLocal())
        cfUrl = CFURLCreateWithFileSystemPath(NULL, cfUrlString,
                                              kCFURLPOSIXPathStyle, false);
    else
        cfUrl = CFURLCreateWithString(NULL, cfUrlString, NULL);
    ON_BLOCK_EXIT(CFRelease, cfUrl);

    CFURLRef urls[] = { cfUrl };
    CFArrayRef cfUrls = CFArrayCreate(NULL,
                                      reinterpret_cast<const void**>(&urls[0]),
                                      1, NULL);
    ON_BLOCK_EXIT(CFRelease, cfUrls);

    LSLaunchURLSpec lSpec = { NULL, cfUrls, NULL,
        kLSLaunchDefaults | kLSLaunchDontAddToRecents, NULL };
    OSStatus rv = LSOpenFromURLSpec(&lSpec, NULL);

    return rv ? HANDLER_ERROR : HANDLER_OK;
}

}
