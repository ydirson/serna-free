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
#ifndef SAPI_COMMON_XTN_WRAP_IMPL_H_
#define SAPI_COMMON_XTN_WRAP_IMPL_H_

#include "sapi/common/xtn_wrap.h"
#include "sapi/common/SString.h"

#define XTREENODE_FUNC_WRAP1(func, baseType, type, cls) \
type cls::func() const \
{ \
    type p; if (getRep()) \
        p.setRep(static_cast<baseType*>(getRep())->func()); return p; \
}

#define XLIST_WRAP_IMPL2(cls, type, baseType) \
    XTREENODE_FUNC_WRAP1(firstChild,  baseType, type, cls) \
    XTREENODE_FUNC_WRAP1(lastChild,   baseType, type, cls) \
    OTHER_XLIST_FUNCTIONS(baseType, type, cls)

#define XLIST_WRAP_IMPL(type, baseType) XLIST_WRAP_IMPL2(type, type, baseType)

#define XTREENODE_WRAP_IMPL(type, baseType) \
    XTREENODE_WRAP_IMPL2(type, baseType, remove)

#define XTREENODE_WRAP_IMPL2(type, baseType, removeFuncName) \
    XLIST_WRAP_IMPL(type, baseType) \
    XTREENODE_FUNC_WRAP1(nextSibling, baseType, type, type) \
    XTREENODE_FUNC_WRAP1(prevSibling, baseType, type, type) \
    XTREENODE_FUNC_WRAP1(parent,      baseType, type, type) \
    XTREENODE_FUNC_WRAP1(root,        baseType, type, type) \
    OTHER_XTREENODE_FUNCTIONS(baseType, type, removeFuncName)

#define OTHER_XLIST_FUNCTIONS(baseType, type, cls) \
void cls::removeAllChildren() \
{ \
    if (getRep()) static_cast<baseType*>(getRep())->removeAllChildren(); \
} \
type cls::getChild(int n) const \
{ \
    type p; if (getRep())\
        p.setRep(static_cast<baseType*>(getRep())->getChild(n)); return p; \
} \
int cls::countChildren() const \
{ \
    return getRep() ? static_cast<baseType*>(getRep())->countChildren() : 0;\
} \
void cls::appendChild(const type& cn) \
{ \
    if (getRep() && cn) \
        static_cast<baseType*>(getRep())-> \
            appendChild(static_cast<baseType*>(cn.getRep())); \
}

#define XTN_CHECK_EXISTING_PARENT(cn, baseType) \
    if (!cn.getRep() || static_cast<baseType*>(cn.getRep())->parent()) return;

#define OTHER_XTREENODE_FUNCTIONS(baseType, type, removeFuncName) \
void type::remove() \
{ \
    if (getRep()) static_cast<baseType*>(getRep())->removeFuncName(); \
} \
void type::insertBefore(const type& cn) \
{ \
    XTN_CHECK_EXISTING_PARENT(cn, baseType) \
    if (getRep()) \
        static_cast<baseType*>(getRep())-> \
            insertBefore(static_cast<baseType*>(cn.getRep())); \
} \
void type::insertAfter(const type& cn) \
{ \
    XTN_CHECK_EXISTING_PARENT(cn, baseType) \
    if (getRep()) \
        static_cast<baseType*>(getRep())-> \
            insertAfter(static_cast<baseType*>(cn.getRep())); \
} \
void type::removeGroup(const type& cn) \
{ \
    XTN_CHECK_EXISTING_PARENT(cn, baseType) \
    if (getRep()) \
        static_cast<baseType*>(getRep())-> \
            removeGroup(static_cast<baseType*>(cn.getRep())); \
} \
int type::siblingIndex() const \
{ \
    if (getRep()) \
        return static_cast<baseType*>(getRep())->siblingIndex(); \
    return 0;\
} \
\
SString type::getTreelocString(const type& topNode) const   \
{                                                           \
    if (getRep()) {                                         \
            Common::TreelocRep tloc;                        \
            static_cast<baseType*>(getRep())->treeLoc(tloc, \
                static_cast<baseType*>(topNode.getRep()));  \
            return tloc.toString();                         \
    } else                                                  \
        return SString();                                    \
} \
type type::getByTreelocString(const SString& ts) const      \
{                                                           \
    if (!getRep())                                          \
        return 0;                                           \
    Common::TreelocRep tloc;                                \
    if (!tloc.fromString(ts))                                 \
        return 0;                                           \
    return static_cast<baseType*>(getRep())->getByTreeloc(tloc); \
}                                                                

#endif // SAPI_COMMON_XTN_WRAP_IMPL_H_

