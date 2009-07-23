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
#include "xs/xs_defs.h"
#include "grove/Nodes.h"
#include "xs/XsDataImpl.h"
#include "xs/complex/Particle.h"
#include "xs/Origin.h"
#include "xs/Schema.h"
#include "xs/datatypes/DataTypes.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

#define MAKE_ATYPE(type, cn) \
    space.insert(new type(rootOrigin, NcnCred(cn, schemaNs_)))

void XsDataImpl::initAtomicTypes()
{
    Origin rootOrigin(schemaRoot_.pointer());
    TypeSpace& space = typeSpace();

    MAKE_ATYPE(AnyType,                 "anyType");
    MAKE_ATYPE(StringType,              "string");
    MAKE_ATYPE(StringType,              "base64Binary");
    MAKE_ATYPE(AnyUriType,              "anyURI");
    MAKE_ATYPE(FloatType,               "float");
    MAKE_ATYPE(BooleanType,             "boolean");
    MAKE_ATYPE(DoubleType,              "double");
    MAKE_ATYPE(NumberType,              "decimal");
    MAKE_ATYPE(QNameType,               "QName");
    MAKE_ATYPE(NormalizedStringType,    "normalizedString");
    MAKE_ATYPE(TokenType,               "token");
    MAKE_ATYPE(LanguageType,            "language");
    MAKE_ATYPE(NmtokenType,             "NMTOKEN");
    MAKE_ATYPE(NmtokenType,             "NOTATION");
    MAKE_ATYPE(NmtokenType,             "ENTITY");
    MAKE_ATYPE(NmtokensType,            "NMTOKENS");
    MAKE_ATYPE(NmtokensType,            "ENTITIES");
    MAKE_ATYPE(NameType,                "Name");
    MAKE_ATYPE(NCNameType,              "NCName");
    MAKE_ATYPE(IDType,                  "ID");
    MAKE_ATYPE(IDREFType,               "IDREF");
    MAKE_ATYPE(IDREFSType,              "IDREFS");
    MAKE_ATYPE(IntegerType,             "integer");
    MAKE_ATYPE(NonPositiveIntegerType,  "nonPositiveInteger");
    MAKE_ATYPE(NegativeIntegerType,     "negativeInteger");
    MAKE_ATYPE(LongType,                "long");
    MAKE_ATYPE(XS_NAMESPACE::IntType,   "int");
    MAKE_ATYPE(ShortType,               "short");
    MAKE_ATYPE(ByteType,                "byte");
    MAKE_ATYPE(NonNegativeIntegerType,  "nonNegativeInteger");
    MAKE_ATYPE(UnsignedLongType,        "unsignedLong");
    MAKE_ATYPE(UnsignedIntType,         "unsignedInt");
    MAKE_ATYPE(UnsignedShortType,       "unsignedShort");
    MAKE_ATYPE(UnsignedByteType,        "unsignedByte");
    MAKE_ATYPE(PositiveIntegerType,     "positiveInteger");
    MAKE_ATYPE(DurationType,            "duration");
    MAKE_ATYPE(DateTimeType,            "dateTime");
    MAKE_ATYPE(TimeType,                "time");
    MAKE_ATYPE(DateType,                "date");
    MAKE_ATYPE(YearMonthType,           "gYearMonth");
    MAKE_ATYPE(YearType,                "gYear");
    MAKE_ATYPE(MonthDayType,            "gMonthDay");
    MAKE_ATYPE(DayType,                 "gDay");
}

XS_NAMESPACE_END
