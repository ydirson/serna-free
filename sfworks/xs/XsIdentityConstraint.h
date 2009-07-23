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

#ifndef XS_IDENTITY_CONSTRAINT_H_
#define XS_IDENTITY_CONSTRAINT_H_

#include "xs/xs_defs.h"
#include "xs/Component.h"
#include "common/prtti.h"

#ifndef NO_XPATH
#include "xpath/ExprContext.h"
namespace XPATH_NAMESPACE {
class Value;
}
#endif  //NO_XPATH

XS_NAMESPACE_BEGIN

class XsKeyImpl;
class XsKeyRefImpl;
class XsUniqueImpl;

/*! The interface for all XML Schema key, keyRef and unique types.
 */
class XS_EXPIMP XsIdentityConstraint : public Component
#ifndef NO_XPATH
    , public Xpath::ExprContext
#endif // NO_XPATH
{
public:
    USE_OALLOC(Xs::IdentityConstraint);
    enum TypeClass {
        unknown, key, keyRef, unique
    };
    TypeClass           typeClass() const;

#ifndef NO_XPATH
    /*! Evaluate string to XPath expression,
      used to get expression from selector and fields.
    */
    COMMON_NS::RefCntPtr<const XPATH_NAMESPACE::Value> evaluate(String& exprstr,
                                 GROVE_NAMESPACE::Element* elem) const;
#endif  //NO_XPATH

    /*! Check uniqueness of key-sequences, and correspondencies
        between keyRef and key.
     */
    virtual bool        validate(Schema* s,
                                 GROVE_NAMESPACE::Element* elem) const = 0;

    virtual void        dump(int indent) const = 0;

    const String& selector() const;
    const String& field(uint i) const;

    static XsIdentityConstraint*  makeEmptyStub(const NcnCred&);

    XsIdentityConstraint(TypeClass typeclass, const Origin& origin,
                         const NcnCred& cred);
    virtual ~XsIdentityConstraint();

    PRTTI_DECL(XsKeyImpl);
    PRTTI_DECL(XsKeyRefImpl);
    PRTTI_DECL(XsUniqueImpl);

private:
    friend class ::GroveAstParser;
    friend class XsKeyImpl;
    friend class XsKeyRefImpl;
    friend class XsUniqueImpl;

    void  setSelector(String& selector);
    void  addField   (String& field);

    TypeClass      typeClass_;
    String         selector_;
    COMMON_NS::Vector<String> fields_;
};

////////////////////////////////////////////////////////////////

inline XsIdentityConstraint::TypeClass XsIdentityConstraint::typeClass() const
{
    return typeClass_;
}

inline void XsIdentityConstraint::setSelector(String& selector)
{
    selector_ = selector;
}

inline void XsIdentityConstraint::addField(String& field)
{
    fields_.push_back(field);
}

inline const String& XsIdentityConstraint::selector() const
{
    return selector_;
}

inline const String& XsIdentityConstraint::field(uint i) const
{
    return fields_[i];
}

XS_NAMESPACE_END

#endif // XS_IDENTITY_CONSTRAINT_H_
