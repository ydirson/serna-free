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

#ifndef _DERIVATION_TYPES_H_
#define _DERIVATION_TYPES_H_

#include "xs/xs_defs.h"
#include "xs/datatypes/SimpleDerivedType.h"
#include "common/Vector.h"
#include "common/RefCntPtr.h"
#include "common/prtti.h"

XS_NAMESPACE_BEGIN

class XsFacet;

/*****************************************************************************
 * Restriction                                                               *
 *****************************************************************************/

/*! A restriction of a simple type
 */
class Restriction : public SimpleDerivedType {
public:
    /*! Validation with context - dvorigin.
     */
    virtual bool    validate(Schema* schema, const GroveLib::Node* o,
                             const String& source,
                             String* result = 0,
                             bool silent = false) const;

    bool            validate(Schema* schema, const GroveLib::Node* o,
                             const String& source,
                             String* result,
                             bool silent,
                             const XsSimpleTypeImpl* root) const;

    virtual void dump(int indent) const;

    void addFacet(COMMON_NS::RefCntPtr<XsFacet>& facet);
    virtual bool fillPossibleEnums(EnumList& lst);

    Restriction(const Origin& origin, const NcnCred& cred);

    virtual ~Restriction();

    PRTTI_DECL(Restriction);
    XS_OALLOC(Restriction);

private:
    mutable COMMON_NS::RefCntPtr<AtomicType>       restricted_;
    mutable COMMON_NS::Vector<COMMON_NS::RefCntPtr<XsFacet> > facetsVector_;
    EnumList    enumList_;
};

/*****************************************************************************
 * List                                                                      *
 *****************************************************************************/
/*! Application-specific information associated with particular
    schema construct.
 */
class List : public SimpleDerivedType {
public:
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source,
                          String* result = 0,
                          bool silent = false) const;

    virtual void dump(int indent) const;
    bool fillPossibleEnums(EnumList& lst);

    List(const Origin& origin, const NcnCred& cred);
    virtual ~List();

    PRTTI_DECL(List);
    XS_OALLOC(List);
};

/*****************************************************************************
 * Union                                                                     *
 *****************************************************************************/

/*! Application-specific information associated with particular
    schema construct.
 */
class Union : public SimpleDerivedType {
public:
    Union(const Origin& origin, const NcnCred& cred);

    virtual ~Union();

    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source,
                          String* result = 0,
                          bool silent = false) const;
    bool fillPossibleEnums(EnumList& lst);
    virtual void dump(int indent) const;

    PRTTI_DECL(Union);
    XS_OALLOC(Union);
};

XS_NAMESPACE_END

#endif // DERIVATION_TYPES_H_
