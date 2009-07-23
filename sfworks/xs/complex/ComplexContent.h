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

#ifndef COMPLEX_CONTENT_H_
#define COMPLEX_CONTENT_H_

#include "xs/xs_defs.h"
#include "xs/complex/XsContent.h"
#include "xs/ComponentRef.h"
#include "grove/Decls.h"
#include "common/OwnerPtr.h"

XS_NAMESPACE_BEGIN

class Connector;
class Component;
class Matcher;

/*! Complex content of the type
 */
class ComplexContent : public XsContent {
public:
    virtual bool validate(Schema* s,
                          Element* elem,
                          bool mixed,
                          bool recursive) const;

    virtual bool validate(Schema* s,
                          const GroveLib::Node* o,
                          const COMMON_NS::String& source,
                          String* result) const;

    virtual bool makeContent(Schema* schema,
                             Element* parentElem,
                             FixupSet* elemSet,
                             const COMMON_NS::String* defValue = 0) const;

    bool isMixed() const { return mixed_; }

    virtual void dump(int indent) const;

    Connector* connector() const { return connector_.pointer();}
    XS_EXPIMP const Matcher* matcher(Schema* s,
                                     const COMMON_NS::String& name) const;

    ComplexContent(DerivationType type,
                   const Origin& o,
                   const XsId& xsid = XsId());
    virtual ~ComplexContent();

    PRTTI_DECL(ComplexContent);
    XS_OALLOC(ComplexContent);

    virtual bool cdataAllowed() const;

private:
    friend class ::GroveAstParser;
    void setBaseType(const XsTypeInst& type);
    void setConnector(COMMON_NS::RefCntPtr<Connector>& connector);
    void setMixed(bool v) { mixed_ = v; }
    void setAnyBase(bool v) { baseIsAny_ = v; }

    /* check derivation extension or restriction.
     */
    bool checkDerivation(Schema* schema,
                         Element* elem,
                         bool mixed) const;

    COMMON_NS::RefCntPtr<Connector> connector_;
    mutable COMMON_NS::OwnerPtr<Matcher> matcher_;
    XsTypeInst      baseType_;
    bool            baseProcessed_;
    bool            mixed_;
    bool            baseIsAny_;
};

XS_NAMESPACE_END

#endif // COMPLEX_CONTENT_H_
