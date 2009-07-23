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

#ifndef ENUM_CACHE_H_
#define ENUM_CACHE_H_

#include "common/Vector.h"
#include "common/String.h"
#include "xs/datatypes/Facets.h"

XS_NAMESPACE_BEGIN
using COMMON_NS::String;
using COMMON_NS::Vector;
using COMMON_NS::RefCntPtr;

struct ExtEnum {
    ExtEnum(const String& t,  COMMON_NS::RefCntPtr<XS_NAMESPACE::EnumerationFacet>& e)
        :tag(t), en(e){}
    String tag;
    COMMON_NS::RefCntPtr<XS_NAMESPACE::EnumerationFacet> en;
};

class ExtEnumCache {
public:

    /*! Add current enum to schema cache.
     */
    void     addEnums(const String& tag,
                      COMMON_NS::RefCntPtr<XS_NAMESPACE::EnumerationFacet>& en);

    /*! Get enums from schema cache.
     */
    bool     cache(const String& tag,
                   COMMON_NS::Vector<XS_NAMESPACE::EnumerationFacet*>& enums);
    /*! Clear enums from schema cache - by tag or clear all if tag is null string.
     */
    void     resetEnums(const String& tag = String::null());

    XS_OALLOC(ExtEnumCache);

private:
    COMMON_NS::Vector<ExtEnum>  enumTable_;
};

////////////////////////////////////////////////////////

inline void ExtEnumCache::addEnums(const String& tag,
                                COMMON_NS::RefCntPtr<XS_NAMESPACE::EnumerationFacet>& en)
{
    enumTable_.push_back(ExtEnum(tag, en));
};

XS_NAMESPACE_END

#endif //  ENUM_CACHE_H_
