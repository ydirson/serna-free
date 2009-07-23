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

#ifndef ATTRIBUTE_SET_H_
#define ATTRIBUTE_SET_H_

#include "xs/xs_defs.h"
#include "common/RefCounted.h"
#include "common/RefCntPtr.h"
#include "common/Vector.h"
#include "grove/grove_defs.h"
#include "grove/Decls.h"
#include "xs/Component.h"
#include "xs/ComponentRef.h"

class Schema;

XS_NAMESPACE_BEGIN

class XsAnyAttribute;

class AttributeSet : public COMMON_NS::RefCounted<> {
public:
    bool    isEmpty() const;

    bool    validate(Schema* schema, GROVE_NAMESPACE::Element* elem,
                     COMMON_NS::Vector<GROVE_NAMESPACE::Attr*>& attrs) const;
    
    bool    validate(Schema* schema, GroveLib::Element* elem,
                     Common::Vector<GroveLib::Attr*>& attrs, 
                     Common::Vector<XsAttribute*>* requiredAttrs) const;

    void    makeSkeleton(Schema* s, GROVE_NAMESPACE::Element* elem) const;
    void    makeSkeleton(Schema* s, GROVE_NAMESPACE::Element* elem,
                         Common::Vector<XsAttribute*>* requiredAttrs) const;
    bool    attrList(COMMON_NS::Vector<XsAttribute*>& attrList) const;

    void    join(AttributeSet* attr);
    bool    check(Schema* schema,
                  const GroveLib::Node* o, AttributeSet* attr) const;
    void    dump(int indent) const;

    AttributeSet();
    ~AttributeSet();

    XS_OALLOC(AttributeSet);

private:
    friend class ::GroveAstParser;

    void    addAttribute(const XsAttributeInst& attr);
    void    addAttributeGroup(const XsAttributeGroupInst& attr_grp);
    void    setAnyAttribute(COMMON_NS::RefCntPtr<XsAnyAttribute>& any_attr);

    COMMON_NS::Vector<XsAttributeInst>      attrVec_;
    COMMON_NS::Vector<XsAttributeGroupInst> attrGroupVec_;
    COMMON_NS::RefCntPtr<XsAnyAttribute>    anyAttribute_;
};

XS_NAMESPACE_END

#endif // ATTRIBUTE_SET_H_
