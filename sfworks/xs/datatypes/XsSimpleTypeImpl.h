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
 *  $RCSfile: XsSimpleTypeImpl.h,v $
 *
 ***********************************************************************/

#ifndef SCHEMA_SIMPLE_TYPE_IMPL_H
#define SCHEMA_SIMPLE_TYPE_IMPL_H

#include "xs/xs_defs.h"
#include "xs/XsType.h"
#include "common/String.h"
#include "grove/Decls.h"

class Schema;
class GroveAstParser;

XS_NAMESPACE_BEGIN

class SimpleDerivedType;
class AtomicType;

/*! StringType class represents Schema String type.
 */
class XsSimpleTypeImpl: public XsType {
public:
    enum WhatType {
        DERIVED, ATOMIC
    };

    virtual bool    validate(Schema* s,
                             GROVE_NAMESPACE::Element* elem,
                             bool recursive) const;

    virtual bool    validate(Schema* schema,
                             const String& source,
                             String* result = 0) const;

    virtual bool    makeContent(Schema* schema,
                                GROVE_NAMESPACE::Element* parentElem, FixupSet*,
                                const String* defv = 0) const;

    virtual void    makeAttrs(Schema* schema,
                              GROVE_NAMESPACE::Element* elem) const;

    virtual bool    cdataAllowed() const { return true; }
    /*! Validation with context - dvorigin.
     */
    virtual bool    validate(Schema* schema, const GroveLib::Node* o,
                             const String& source,
                             String* result = 0,
                             bool silent = false) const = 0;

    virtual void dump(int indent) const = 0;

    /*! Type of the SimpleType.
     */
    WhatType type() const;

    PRTTI_DECL(XsSimpleTypeImpl);
    PRTTI_DECL(SimpleDerivedType);
    PRTTI_DECL(AtomicType);

    XsSimpleTypeImpl(WhatType type, const Origin& origin,
                     const NcnCred& cred);
    virtual ~XsSimpleTypeImpl();

    XS_OALLOC(XsSimpTypeImpl);

private:
    friend class ::GroveAstParser;
    WhatType    type_;
};

GroveLib::Text* make_text_choice(const XsType*);
bool validate_simple_data(Schema* schema, 
                          const XsSimpleTypeImpl* xsti,
                          GroveLib::Element* elem);

XS_NAMESPACE_END

#endif // SCHEMA_SIMPLE_TYPE_IMPL_H
