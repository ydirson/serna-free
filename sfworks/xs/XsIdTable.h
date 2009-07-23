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
#ifndef XS_ID_TABLE_H_
#define XS_ID_TABLE_H_

#include "xs/xs_defs.h"
#include "xs/XsId.h"
#include "common/PtrSet.h"

XS_NAMESPACE_BEGIN

// class XsIdKeyFunction {
// public:
//     static inline const String& key(const XsId& id) {
//      return id.id();
//     }
// };

template <class V, class K> class XsIdKeyFunction {
public:
    typedef K KeyType;
    static const K& key(const COMMON_NS::PtrBase<V>& id) { return id->id(); }
    static const K& key(const V* id) { return id->id(); }
};
/*! A table which keeps Id's of a schema components - so it becomes
    possible to reference a schema component by ID directly.
 */
template <class T> class XsIdTable :
    public COMMON_NS::SmartPtrSet<COMMON_NS::RefCntPtr<T>,
                      XsIdKeyFunction<T, COMMON_NS::String> > {};

XS_NAMESPACE_END

#endif // XS_ID_TABLE_H_
