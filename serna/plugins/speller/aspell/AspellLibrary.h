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

#ifndef ASPELL_LIBRARY_H_
#define ASPELL_LIBRARY_H_

#include "SpellChecker.h"
#include "common/common_defs.h"
#include "utils/utils_defs.h"
#include "common/DynamicLibrary.h"
#include "common/PropertyTree.h"
#include "common/StringDecl.h"

#ifndef SERNA_SYSPKG
# include "aspell.hpp"
#else
# include <aspell.h>
#endif

class AspellLibrary : protected Common::DynamicLibrary {
public:
    typedef Common::nstring nstring;
    static AspellLibrary& instance();
    //!
    virtual bool loadLibrary(const Common::ustring& libPath) = 0;
    virtual void* resolveSym(const char*) const = 0;
    //!
    virtual const Common::ustring& getLibError() const throw() = 0;
    virtual const Common::ustring& getSymError() const throw() = 0;
    //!
    virtual AspellConfig* getDefaultConfig() = 0;
    virtual AspellSpeller* makeSpeller(const nstring& id) = 0;
    virtual bool getDictList(SpellChecker::Strings& si,
                             SpellChecker::Status* = 0) = 0;
    //!
    virtual void setDict(const nstring& id) = 0;
    virtual const nstring& getDict() const = 0;
    virtual const nstring& getEncoding(const nstring& dict) = 0;
    //!
    virtual const nstring& findDict(const nstring& dict) = 0;
    //!
    virtual bool  setConfig(const Common::PropertyNode* configNode) = 0;
protected:
    AspellLibrary();
    virtual ~AspellLibrary();
private:
    DEFAULT_COPY_CTOR_DECL(AspellLibrary)
    DEFAULT_ASSIGN_OP_DECL(AspellLibrary)
};

struct AspellResolver {
    static void* resolve(const char* sym);
    static bool need_reload() { return false; }
};

class AspellErr : public SpellChecker::Error {
public:
    AspellErr(const SpellChecker::Error::Info* i) : Error(i) {}
    AspellErr(const Common::ustring& s) : Error(s) {}
    AspellErr(const char* s) : Error(s) {}
};

#ifndef SERNA_SYSPKG
# define FUN(x) \
      DynFunctor<FunTraits<TYPEOF(x)>::FunType, nm_##x, AspellResolver>()
#endif

#endif // ASPELL_LIBRARY_H_
