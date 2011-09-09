// 
// Copyright(c) 2011 Syntext, Inc. All Rights Reserved.
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
#ifndef SPELLER_LIBRARY_
#define SPELLER_LIBRARY_

#include "common/DynamicLibrary.h"
#include "SpellChecker.h"

class SpellerLibrary : public Common::DynamicLibrary {
public:
    SpellerLibrary() {}

    bool        loadLibrary(const Common::String& libPath);
    void*       resolveSym(const char*) const;
    
    void                  setLibError(const Common::String& = Common::String());
    const Common::String& getLibError() const { return lib_error_; }
    const Common::String& getSymError() const { return sym_error_; }

private:
    SpellerLibrary(const SpellerLibrary&);
    SpellerLibrary& operator=(const SpellerLibrary&);

    Common::String lib_path_, lib_error_;
    mutable Common::String sym_error_;
};

template <class L> struct SpellerLibraryResolver {
    static void* resolve(const char* sym) {
        L& lib = L::instance();
        if (void*p = lib.resolveSym(sym))
            return p;
        throw SpellChecker::Error(lib.getSymError());
    }
    static bool  need_reload() { return false; }
};
    
#endif // SPELLER_LIBRARY_
