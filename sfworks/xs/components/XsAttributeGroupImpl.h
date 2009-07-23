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

#ifndef XS_ATTRIBUTE_GROUP_IMPL_
#define XS_ATTRIBUTE_GROUP_IMPL_

#include "xs/xs_defs.h"
#include "xs/XsAttributeGroup.h"

class Schema;

XS_NAMESPACE_BEGIN

class AttributeSet;

/*! Implementation of XsAttributeGroup.
 */
class XsAttributeGroupImpl : public XsAttributeGroup {
public:
    virtual bool    validate(Schema* s, GROVE_NAMESPACE::Element* elem,
                             COMMON_NS::Vector<GROVE_NAMESPACE::Attr*>& attrs,
                             Common::Vector<XsAttribute*>* = 0) const;

    virtual void    makeSkeleton(Schema* s, GROVE_NAMESPACE::Element* pe,
                                 Common::Vector<XsAttribute*>* = 0) const;

    virtual bool    attrList(COMMON_NS::Vector<XsAttribute*>& attrList) const;
    /*! Obtain an attribute set
     */
    const COMMON_NS::RefCntPtr<AttributeSet> attributeSet() const;

    XsAttributeGroupImpl(const Origin& origin, const NcnCred& cred);

    ~XsAttributeGroupImpl();

    virtual void    dump(int indent) const;

    XS_OALLOC(XsAttrGroupImpl);

protected:
     friend class ::GroveAstParser;
     void setAttributeSet(AttributeSet* aset);

     COMMON_NS::RefCntPtr<AttributeSet>   attributeSet_;
};

XS_NAMESPACE_END

#endif // XS_ATTRIBUTE_GROUP_IMPL_
