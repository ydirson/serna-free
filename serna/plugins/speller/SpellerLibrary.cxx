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

#include "SpellerLibrary.h"
#include "utils/Config.h"
#include "common/PropertyTree.h"
#include "common/Singleton.h"

using namespace Common;

bool SpellerLibrary::loadLibrary(const String& libPath)
{
    if (!isLoaded() || libPath != lib_path_) {
        if (isLoaded())
            unload();
        lib_path_ = libPath;
        if (!libPath.empty())
            DynamicLibrary::load(libPath);
    }
    return isLoaded();
}

void* SpellerLibrary::resolveSym(const char* name) const
{
    if (void* sym = resolve(name)) {
        sym_error_ = String::null();
        return sym;
    }
    sym_error_ = errorMsg();
    return 0;
}

void SpellerLibrary::setLibError(const String& s)
{
    lib_error_ = s;
}

SpellerLibrary::SpellerLibrary()
{
}

SpellerLibrary::~SpellerLibrary()
{
}

/////////////////////////////////////////////////////////////////////////

class SpellerFactory {
public:
    typedef SpellLibraryRegistrar::InstanceFunc InstanceFunc;
    static SpellerFactory& instance();
    void    addInstanceMaker(const char* id, InstanceFunc f)
    {
        makerMap_[id] = f;
    }
    SpellerLibrary* getLibrary() const;

private:
    typedef std::map<String, InstanceFunc> MakerMap;
    MakerMap makerMap_;        
};

SpellerLibrary* SpellerFactory::getLibrary() const
{
    String libid = config().root()->getString(SPELL_CFG_VAR, SPELL_USE_VAR);
    if (libid.isEmpty())
        libid = NOTR("aspell"); // default
    MakerMap::const_iterator it = makerMap_.find(libid);
    if (makerMap_.end() == it)
        return 0;
    return (*it->second)();
}

SpellLibraryRegistrar::SpellLibraryRegistrar(const char* id, InstanceFunc mf)
{
    SpellerFactory::instance().addInstanceMaker(id, mf);
}

SpellerFactory& SpellerFactory::instance()
{
    return SingletonHolder<SpellerFactory>::instance();
}

SpellerLibrary* SpellerLibrary::instance()
{
    return SpellerFactory::instance().getLibrary();
}
