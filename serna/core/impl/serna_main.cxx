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
 #include "serna_main.h"

#include "core/Serna.h"
#include "core/AxSerna.h"
#include "core/debug_core.h"

#include "utils/file_utils.h"
#include "utils/config_defaults.h"
#include "utils/Config.h"

#include "common/Debug.h"
#include "common/FlexString.h"
#include "common/common_opt.h"
#include "common/StringCvt.h"
#include "common/ProfileGuard.h"

#if defined(__APPLE__)
# include <Carbon/Carbon.h>
#endif

using namespace Common;

int serna_main(int argc, char* argv[])
{
    if (AxSernaBase::needMain())
        return AxSernaBase::main();

#if defined(__APPLE__)
    CFURLRef plugin_ref = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef mac_path = CFURLCopyFileSystemPath(plugin_ref,
                                                   kCFURLPOSIXPathStyle);
    const char *path = CFStringGetCStringPtr(mac_path,
                                             CFStringGetSystemEncoding());
    nstring base(path);
    base += NOTR("/Contents");
    setenv(NOTR("SERNA_DATA_DIR"), base.c_str(), 0);
#endif
    set_ldlibpath();

    if (argv) {
        FileUtils::argv0(argv[0]);
        argc = process_common_options(argc, argv);
    }

    ProfileGuard::start();
    int retval = -1;
    try {
        retval = Serna::exec(argc, argv);
    }
    catch (std::exception& e) {
// removed cause it breaks win32 release build with plugins
//        DBG(CORE.TEST) << e.what() << std::endl;
    }
    return retval;
}
