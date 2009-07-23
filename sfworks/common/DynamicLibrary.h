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
/*! \file
    Dynamic library support (dlopen wrapper)
 */
#ifndef DYNAMIC_LIBRARY_H_
#define DYNAMIC_LIBRARY_H_

#include "common/common_defs.h"
#include "common/OwnerPtr.h"
#include "common/StringDecl.h"


COMMON_NS_BEGIN

/*! Implements simple OS-independent dynamic library load interface.
 *  Unlike dlopen, it supports relative file paths.
 */
class COMMON_EXPIMP DynamicLibrary {
public:
    enum Mode {
        RESOLVE_LAZY = 00,   ///< External references are resolved lazily
        RESOLVE_NOW  = 01,   ///< Resolve all references at load time
        RESOLVE_GLOBAL = 02, ///< Make all symbols available to other libs
        FORCE_ABSPATH = 04   ///< convert relative name to absolute path
    };
    /// Load dynamic library. Returns true on success.
    bool    load(const char* path, int mode = RESOLVE_LAZY);

    /// Load dynamic library. Returns true on success
    bool    load(const String& path, int mode = RESOLVE_LAZY);

    /// Resolve symbol in library
    void*   resolve(const char* symbol) const;

    /// Checks whether dynamic library has been succesfully loaded
    bool    isLoaded() const;

    /// Returns (possibly unmodified) file name path of the loaded library
    String  fileName(bool origpath = false) const;

    /// Unload library on destruction? Default is false.
    void    setAutoUnload(bool v) { autoUnload_ = v; }

    /// Unload the dynamic library
    void    unload();

    /// Error condition message, of any
    String  errorMsg() const;

    DynamicLibrary();
    ~DynamicLibrary();

private:
    class DynamicLibraryPrivate;
    OwnerPtr<DynamicLibraryPrivate> p_;
    bool    autoUnload_;
};

COMMON_NS_END

#endif // DYNAMIC_LIBRARY_H_
