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

#ifndef FILE_UTILS_H_
#define FILE_UTILS_H_

#include "utils/utils_defs.h"
#include "common/String.h"
#include "common/Url.h"

namespace GroveLib {
    class Grove;
}

namespace GroveEditor {
    class GrovePos;
}

namespace Common {
    class PropertyNode;
}

namespace FileUtils {

UTILS_EXPIMP Common::String get_default_doc_dir();

UTILS_EXPIMP bool mkdirp(const Common::String& path);

UTILS_EXPIMP bool rename(const Common::String& oldpath,
                         const Common::String& newpath);

UTILS_EXPIMP bool unlink(const Common::String& path);

    /// Returns the date and time when the file was last modified.
UTILS_EXPIMP Common::String getFileModifiedTime(const Common::Url& url);

    //! Returns true if file was modified outside.
    //  lastTime is result of previous function getFileModifiedTime()
    //  called on open file,
UTILS_EXPIMP bool isFileModified(const Common::Url& url,
                                 const Common::String& lastTime);

UTILS_EXPIMP void doTimeStamps(const GroveLib::Grove* grove,
                               Common::PropertyNode* mtime);

UTILS_EXPIMP Common::String check_document_reload(Common::PropertyNode* dsi);
/**
 * @param argv_0 specifies argv[0] (must be called from within main())
 * ignored on Win32
 *
 * @return reference to a singleton String that contains full
 * path name of the executable
 */
UTILS_EXPIMP const Common::String& argv0(const char* argv_0 = 0);

UTILS_EXPIMP bool copy_file(const Common::String& srcfile,
                            const Common::String& dstfile);

// input:  full long path name
// output: corresponding short name (if any)
UTILS_EXPIMP Common::String get_short_path(const Common::String& path);

UTILS_EXPIMP const Common::PropertyNode* get_handler(const Common::String& url,
                                                     const Common::String& ext);
//
enum LaunchCode { HANDLER_OK, HANDLER_UNDEF, HANDLER_ERROR, HANDLER_BADURL };

UTILS_EXPIMP LaunchCode launch_file_handler(const Common::String& url,
                                            const Common::String& type =
                                                Common::String::null(),
                                            const Common::String& action =
                                                Common::String::null());

}

#endif // FILE_UTILS_H_
