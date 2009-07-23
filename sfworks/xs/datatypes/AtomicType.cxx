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
 *  $RCSfile: AtomicType.cxx,v $
 *
 ***********************************************************************/

#include "xs/xs_defs.h"
#include "common/ThreadingPolicies.h"
#include "xs/datatypes/AtomicType.h"

#include "grove/Nodes.h"
#include "xs/Schema.h"
#include "xs/XsDataImpl.h"
#include "xs/complex/Particle.h"
#include "xs/EnumFetcher.h"
#include "xs/ExtEnumCache.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

AtomicType::AtomicType(const Origin& origin, const NcnCred& cred)
    : XsSimpleTypeImpl(XsSimpleTypeImpl::ATOMIC, origin, cred)
{
}

bool AtomicType::validate(Schema* schema,
                          const GroveLib::Node* o,
                          const String& source,
                          String* result,
                          bool silent) const
{
    String x;
    if (result)
        return validate(silent ? 0 : schema, o, source, *result);
    else
        return validate(silent ? 0 : schema, o, source, x);
}

bool AtomicType::addExtEnum(Schema* schema,
                            EnumerationFacet* enumeration)
{
    if (schema)
        XS_NAMESPACE::XsDataImpl::Lock(*schema->xsi());
    if (!enumeration->tag().isNull()) {
        bool ok = true;
        String tag = enumeration->tag();
        Vector<EnumerationFacet*> enums;
        if (schema && schema->getEnumCache()->cache(tag, enums)) {
            for (uint i = 0; i < enums.size(); i++)
                ok = addEnumeration(schema, enums[i]) && ok;
            return ok;
        }
        if (schema && schema->getEnumFetcher()) {
            Vector<String> rv;
            schema->getEnumFetcher()->
                fetchEnum(tag, rv);
            if ( 0 == rv.size())
                return addEnumeration(schema, enumeration);
            for (uint i = 0; i < rv.size(); i++) {
                RefCntPtr<EnumerationFacet> en =
                    new EnumerationFacet(rv[i], false);
                schema->getEnumCache()->addEnums(tag, en);
                ok = addEnumeration(schema, en.pointer()) && ok;
            }
            return ok;
        }
    }
    return addEnumeration(schema, enumeration);
}

AtomicType::~AtomicType()
{
}

PRTTI_IMPL(AtomicType);

XS_NAMESPACE_END
