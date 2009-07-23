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
#ifndef XS_TYPE_H_
#define XS_TYPE_H_

#include "xs/xs_defs.h"
#include "xs/Component.h"
#include "common/prtti.h"
#include "grove/Decls.h"

XS_NAMESPACE_BEGIN

class XsSimpleTypeImpl;
class XsComplexTypeImpl;
class FixupSet;
class ComponentInstBase;

/*! A fat interface for both XML Schema complex and simple types.
    Certain methods are not implemented for a simple types; they
    produce error messages if non-applicable method are called.
 */
class XS_EXPIMP XsType : public Component {
public:
    typedef GROVE_NAMESPACE::Element Element;
    typedef COMMON_NS::String String;

    enum TypeClass {
        unknown, simpleType, complexType
    };
    TypeClass       typeClass() const;

    /*! Validate element
     */
    virtual bool    validate(Schema* s,
                             Element* elem,
                             bool recursive) const = 0;

    /*! Validate simple type by value. This function returns error
        for complex types.
     */
    virtual bool    validate(Schema* schema,
                             const String& source,
                             String* result = 0) const = 0;

    /*! Create a skeleton of a complex/simple type content; attach results
        to the \a parentElem
     */
    virtual bool    makeContent(Schema* schema,
                                Element* parentElem,
                                FixupSet* elemSet,
                                const String* defv = 0) const = 0;

    /*! Create attributes on already existing element
     */
    virtual void    makeAttrs(Schema* schema,
                              Element* elem) const = 0;

    virtual bool    cdataAllowed() const = 0;

    static XsType*  makeEmptyStub(const NcnCred&);

    virtual void    dump(int indent) const = 0;

    const XsSimpleTypeImpl* getSimpleType() const;

    XsType(TypeClass typeclass, const Origin& origin, const NcnCred& cred);

    virtual ~XsType();

    PRTTI_DECL(XsSimpleTypeImpl);
    PRTTI_DECL(XsComplexTypeImpl);

private:
    TypeClass   typeClass_;
};

////////////////////////////////////////////////////////////////

inline XsType::TypeClass XsType::typeClass() const
{
    return typeClass_;
}

XS_NAMESPACE_END

#endif // XS_TYPE_H_
