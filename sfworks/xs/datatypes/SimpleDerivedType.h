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

#ifndef SIMPLE_DERIVATION_METHOD_H_
#define SIMPLE_DERIVATION_METHOD_H_

#include "xs/xs_defs.h"
#include "xs/datatypes/XsSimpleTypeImpl.h"
#include "xs/ComponentRef.h"
#include "xs/XsEnumList.h"
#include "common/prtti.h"

class GroveAstParser;
class Schema;

XS_NAMESPACE_BEGIN

class Restriction;
class List;
class Union;

/*! Application-specific information associated with particular
    schema construct.
 */
class SimpleDerivedType : public XsSimpleTypeImpl {
public:
    enum DerivationMethod {
        ALL, LIST, RESTRICTION, UNION, ATOMIC, NONE
    };

    DerivationMethod    derivationMethod() const;
    DerivationMethod               final() const;

    const XsTypeInst&         baseType() const;
    const COMMON_NS::Vector<XsTypeInst>& baseVector() const;

    const String& trim() const;
    const String& trimResult() const;
    const String& pyPreFunc() const;
    const String& pyPostFunc() const;

    SimpleDerivedType(DerivationMethod derivedBy,
                      const Origin& origin,
                      const NcnCred& cred);
    virtual ~SimpleDerivedType();

    virtual bool fillPossibleEnums(EnumList& lst) = 0;
    virtual void dump(int indent) const = 0;

    PRTTI_DECL(SimpleDerivedType);

    PRTTI_DECL(Restriction);
    PRTTI_DECL(List);
    PRTTI_DECL(Union);
    XS_OALLOC(SimpleDerivedType);

private:
    friend class ::GroveAstParser;
    void addBaseType(const XsTypeInst& stype);
    void setBaseType(const XsTypeInst& stype);

    void setTrim(String& trim, String& trimResult);
    void setPyPreFunc(String& funcName);
    void setPyPostFunc(String& funcName);

    void setFinal(DerivationMethod final);

    DerivationMethod         derivationMethod_;
    COMMON_NS::Vector<XsTypeInst>       baseVector_;
    XsTypeInst               baseType_;
    String                   trim_;
    String                   trimResult_;
    String                   pyPreFunc_;
    String                   pyPostFunc_;
    DerivationMethod         final_;

private:
    friend class Restriction;
    friend class List;
    friend class Union;
    String getTrimmed(Schema* schema,
                      const GroveLib::Node* o,
                      const String& source) const;
};

inline const XsTypeInst& SimpleDerivedType::baseType() const
{
    return baseType_;
}

inline const COMMON_NS::Vector<XsTypeInst>& SimpleDerivedType::baseVector() const
{
    return baseVector_;
}

XS_NAMESPACE_END

#endif // SIMPLE_DERIVATION_METHOD_H_
