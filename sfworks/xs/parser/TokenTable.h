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
 */

#ifndef TOKEN_TABLE_H_
#define TOKEN_TABLE_H_

#include "xs/xs_defs.h"
#include "common/Singleton.h"
#include "common/RefCounted.h"
#include "common/RefCntPtr.h"
#include "common/PtrSet.h"
#include "common/String.h"

XS_NAMESPACE_BEGIN
using COMMON_NS::String;
using COMMON_NS::RefCounted;
using COMMON_NS::SmartPtrSet;
using COMMON_NS::PtrBase;
using COMMON_NS::RefCntPtr;
using COMMON_NS::SingletonHolder;
/*! A single token of an XML schema representation.
 */
struct XmlSchemaToken : public RefCounted<> {
public:
    XmlSchemaToken(const String& name, int tok)
    : name_(name), token(tok) {}

    String name_;
    int     token;
};

template <class V, class K> class NamedTableKeyFunction {
public:
    typedef K KeyType;
    static const K& key(const PtrBase<V>& nt) { return nt->name_; }
    static const K& key(const V* nt) { return nt->name_; }
};

typedef SmartPtrSet<COMMON_NS::RefCntPtr<XmlSchemaToken>,
                   NamedTableKeyFunction<XmlSchemaToken,
                   COMMON_NS::String> > XmlSchemaTokenMap;

/*! A mapper of element names in Xml Schema to tokens.
 */
class Tokens {
public:
    int     getToken(const String& name) const;

    Tokens();
private:
    XmlSchemaTokenMap   map_;
};

typedef SingletonHolder<Tokens> TokenTable;

///////////////////////////////////////////////////////////

inline int Tokens::getToken(const String& name) const
{
    XmlSchemaTokenMap::iterator it = map_.find(name);
    if (map_.end() == it)
        return 0;
    return it->pointer()->token;
}

XS_NAMESPACE_END

#endif // TOKEN_TABLE_H_
