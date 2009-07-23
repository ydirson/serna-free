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
 *  $RCSfile: AtomicType.h,v $
 *
 ***********************************************************************/

#ifndef SCHEMA_SIMPLE_TYPE_H
#define SCHEMA_SIMPLE_TYPE_H

#include "xs/xs_defs.h"
#include "xs/datatypes/XsSimpleTypeImpl.h"
#include "common/prtti.h"

XS_NAMESPACE_BEGIN

class XsFacet;
class EnumerationFacet;

#define CLONE_ATOMIC_TYPE(atype) \
AtomicType* atype::clone() const \
{ \
    return new  atype(origin(), *constCred()); \
}

class AtomicType : public XsSimpleTypeImpl {
public:
    /*! Value types.
     */
    enum Type {
        UNDEFINED = 0,
        ANYTYPE,
        STRING,
        BOOLEAN,
        FLOAT,
        DOUBLE,
        NUMBER,
        BINARY,
        ANYURI,
        ID,
        IDREF,
        IDREFS,
        ENTITY,
        ENTITIES,
        NOTATION,
        QNAME,
        NORMALIZEDSTRING,
        TOKEN,
        LANGUAGE,
        NMTOKEN,
        NMTOKENS,
        NAME,
        NCNAME,
        INTEGER,
        NONPOSITIVEINTEGER,
        NEGATIVEINTEGER,
        LONG,
        INT,
        SHORT,
        BYTE,
        NONNEGATIVEINTEGER,
        UNSIGNEDLONG,
        UNSIGNEDINT,
        UNSIGNEDSHORT,
        UNSIGNEDBYTE,
        POSITIVEINTEGER,
        RECURRING,
        TIMEPERIOD,
        DURATION,
        DATETIME,
        TIME,
        DATE,
        YEARMONTH,
        YEAR,
        MONTHDAY,
        DAY,
        MONTH,
        USERDEFINED,
        LASTTYPE
    };

    AtomicType(const Origin& origin, const NcnCred& cred);

    virtual ~AtomicType();

    virtual bool   validate(Schema* schema, const GroveLib::Node* o,
                             const String& source,
                             String* result = 0,
                             bool silent = false) const;

    virtual bool   validate(Schema* schema, const GroveLib::Node* o,
                             const String& source,
                             String& result) const = 0;

    virtual Type   type() const = 0;
    virtual void   dump(int indent) const = 0;

    virtual bool   addFacet(Schema* schema, XsFacet* facet) = 0;
    virtual bool   addEnumeration(Schema* schema,
                                   EnumerationFacet* enumeration) = 0;
    bool           addExtEnum(Schema* schema,
                              EnumerationFacet* enumeration);

    virtual AtomicType* clone() const = 0;

    PRTTI_DECL(AtomicType);
};

XS_NAMESPACE_END


#endif  // SCHEMA_SIMPLE_TYPE_H

