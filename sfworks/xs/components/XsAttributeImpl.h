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

#ifndef XS_ATTRIBUTE_IMPL_H_
#define XS_ATTRIBUTE_IMPL_H_

#include "xs/xs_defs.h"
#include "xs/XsType.h"
#include "xs/XsAttribute.h"
#include "xs/ComponentRef.h"

class Schema;
class GroveAstParser;

XS_NAMESPACE_BEGIN

class XsAttributeImpl : public XsAttribute {
public:
    virtual bool validate(Schema* s, GROVE_NAMESPACE::Attr* attr) const;

    virtual bool createAttr(Schema* s,
                        GROVE_NAMESPACE::Element* pe,
                        COMMON_NS::RefCntPtr<GROVE_NAMESPACE::Attr>& a) const;

    virtual AttributeForm   attributeForm() const;
    virtual AttributeUse    attributeUse() const;
    virtual const String&   defValue() const;
    virtual const String&   fixValue() const;
    virtual Common::PropertyNode* makeSpec(const GroveLib::Element*) const;

    virtual String skeletonValue(Schema* s) const;

    XsAttributeImpl(const Origin& origin, const NcnCred& cred);

    virtual XsType* xstype() const;
    virtual void    getPossibleEnumlist(EnumList& lst) const;
    virtual void    dump(int indent) const;
    const XsAttribute* refAttr() const;

    XS_OALLOC(XsAttrImpl);

private:
    friend class ::GroveAstParser;
    void            setBaseType(const XsTypeInst& type);
    void            setForm(AttributeForm form) { form_ = form; }
    void            setUse(AttributeUse  use)  { use_ = use; }
    void            setRef(const XsAttributeInst& inst);

    AttributeForm   form_;
    AttributeUse    use_;
    String          default_;
    String          fixed_;

    XsTypeInst      type_;
    XsAttributeInst ref_;
};

XS_NAMESPACE_END

#endif // XS_ATTRIBUTE_IMPL_H_
