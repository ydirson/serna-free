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
#include "xslt/impl/FunctionRegistry.h"
#include "xslt/impl/debug.h"
#include "common/PtrSet.h"
#include <map>

using namespace Common;

namespace Xslt {

typedef Common::RefCntPtr<ExternalFunction> ExternalFunctionPtr;

inline static Common::String make_extfunc_key2(const Common::String& localName,
                                               const Common::String& xmlns)
{
    String res;
    res.reserve(localName.length() + xmlns.length() + 2);
    res.append(localName);
    res.append(":");
    res.append(xmlns);
    return res;
}

inline static Common::String make_extfunc_key(const ExternalFunction* ef)
{
    return make_extfunc_key2(ef->localName(), ef->xmlns());
}

template <class V, class K> class ExternalFunctionKF {
public:
    typedef K KeyType;
    static const K key(const COMMON_NS::PtrBase<V>& value)
    {
        return make_extfunc_key(value.pointer());
    }
    static const K key(const V* value)
    {
        return make_extfunc_key(value);
    }
};

class FunctionRegistry::ExternalFuncMap:
    public Common::SmartPtrSet<Common::RefCntPtr<ExternalFunction>, 
        ExternalFunctionKF<ExternalFunction, Common::String> > {};

void FunctionRegistry::registerExternalFunction(ExternalFunction* func,
                                                bool doRegister)
{
    DDBG << "RegisterExtFunction: localName=" << func->localName()
        << " ns=" << func->xmlns() << std::endl;
    if (doRegister)
        externalFuncMap_->insert(func, true);
    else {
        ExternalFuncMap::iterator it = 
            externalFuncMap_->find(make_extfunc_key(func));
        if (it != externalFuncMap_->end())
            externalFuncMap_->erase(it);
    }
}

const ExternalFunction* 
FunctionRegistry::getExternalFunc(const Common::String& name,
                                  const Common::String& nsUri) const
{
    ExternalFuncMap::iterator it =
        externalFuncMap_->find(make_extfunc_key2(name, nsUri));
    return (it == externalFuncMap_->end()) ? 0 : it->pointer();
}

FunctionRegistry::FunctionRegistry()
    : externalFuncMap_(new ExternalFuncMap)
{
}

FunctionRegistry::~FunctionRegistry()
{
    ExternalFuncMap::iterator it = externalFuncMap_->begin();
    for (; it != externalFuncMap_->end(); ++it)
        (*it)->xsltEngine_ = 0;
}

} // namespace Xslt
