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

#ifndef XS_ATTRIBUTE_H_
#define XS_ATTRIBUTE_H_

#include "xs/xs_defs.h"
#include "xs/Component.h"
#include "xs/XsEnumList.h"

namespace Common {
    class PropertyNode;
}

XS_NAMESPACE_BEGIN

/*! Application-specific information associated with particular
    schema construct.
 */
class XS_EXPIMP XsAttribute : public Component {
public:
    enum AttributeForm {
        QUALIFIED, UNQUALIFIED, NONE, AF_UNDEF
    };

    enum AttributeUse {
        A_PROHIBITED, A_OPTIONAL, A_REQUIRED, A_DEFAULT, A_FIXED, A_UNDEF
    };
    virtual AttributeForm attributeForm() const = 0;
    virtual AttributeUse  attributeUse() const = 0;
    virtual const String& defValue() const = 0;
    virtual const String& fixValue() const = 0;

    // Create attribute specification
    virtual Common::PropertyNode* 
        makeSpec(const GroveLib::Element*) const = 0;

    /*! Validate value of the attribute
     */
    virtual bool    validate(Schema* s, GROVE_NAMESPACE::Attr* attr) const = 0;

    /*! Create an attribute
     */
    virtual bool    createAttr(Schema* s,
                GROVE_NAMESPACE::Element* contextElem,
                COMMON_NS::RefCntPtr<GROVE_NAMESPACE::Attr>& a) const = 0;

    virtual String  skeletonValue(Schema* s) const = 0;
    virtual XsType* xstype() const = 0;
    virtual const XsAttribute* refAttr() const = 0;
    virtual void    getPossibleEnumlist(EnumList& lst) const = 0;
    virtual void    dump(int indent) const = 0;

    static XsAttribute* makeEmptyStub(const NcnCred&);

    XsAttribute(const Origin& origin, const NcnCred& cr);

    virtual ~XsAttribute();

    PRTTI_DECL(XsAttribute);
};

XS_NAMESPACE_END

#endif // XS_ATTRIBUTE_H_
