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
 #include "common/common_defs.h"
#include "common/String.h"
#include "common/StringCvt.h"
#include "DynamicLibraryPrivate.h"
#include <QFileInfo>
#include <QDir>

#if defined(__APPLE__)
static const char SHLIB_SUFFIX[] = ".dylib";
#else
static const char SHLIB_SUFFIX[] = ".so";
#endif

COMMON_NS_BEGIN

static void set_error(String& err)
{
    err = from_local_8bit(dlerror());
}

bool DynamicLibrary::load(const String& path, int mode)
{
    unload();
    p_->origpath = p_->path = path;
    if (path.find(SHLIB_SUFFIX) < 0)
        p_->path += SHLIB_SUFFIX;

    if (mode & FORCE_ABSPATH) {
        QFileInfo fi(p_->path);
        if (fi.isRelative())
            p_->path = QDir::convertSeparators(fi.absFilePath());
    }

    unsigned m = (mode & RESOLVE_NOW) ? RTLD_NOW : RTLD_LAZY;
    if (mode & RESOLVE_GLOBAL)
        m |= RTLD_GLOBAL;
    p_->handle = dlopen(p_->path.latin1().c_str(), m);

    if (0 == p_->handle) {
        set_error(p_->errmsg);
        return false;
    }
    return true;
}

void DynamicLibrary::unload()
{
    p_->errmsg = p_->path = p_->origpath = String::null();

    if (p_->handle) {
        dlclose(p_->handle);
        p_->handle = 0;
    }
}

void* DynamicLibrary::resolve(const char* symbol) const
{
    if (0 != p_->handle) {
        if (void* rv = dlsym(p_->handle, symbol))
            return rv;
        set_error(p_->errmsg);
    }
    return 0;
}

COMMON_NS_END
