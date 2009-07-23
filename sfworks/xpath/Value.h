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

#ifndef XPATH_VALUE_H
#define XPATH_VALUE_H

#include "common/common_defs.h"
#include "common/String.h"
#include "common/RefCntPtr.h"
#include "common/SernaApiRefCounted.h"
#include "xpath/xpath_defs.h"
#include "xpath/Exception.h"

namespace Xpath {

class NodeSet;

/*! \brief Value represents the xpath value.

  Types of the xpath Value is defined in XPATH specification as:
  - boolean
  - numeric
  - string
  - nodeset

  User can convert the values from one type to another using the to(type) method.
 */
class Value;

typedef COMMON_NS::RefCntPtr<const Value> ConstValuePtr;

class XPATH_EXPIMP Value : public Common::SernaApiRefCounted {
public:
    XPATH_OALLOC(Value);
    enum Type {
        UNDEFINED = 0,
        BOOLEAN,
        STRING,
        NUMERIC,
        NODESET,
        LASTTYPE
    };
    virtual ~Value() {}
    
    //! Returns the type of the value
    virtual Type                type() const = 0;
    //! Returns true when tho values are exactly the same
    virtual bool                isEqual(const Value& val) const = 0;
    //!
    virtual bool                operator==(const Value& val) const = 0;
    virtual bool                operator!=(const Value& val) const = 0;
    virtual bool                operator<(const Value& val) const = 0;
    virtual bool                operator<=(const Value& val) const = 0;
    virtual bool                operator>(const Value& val) const = 0;
    virtual bool                operator>=(const Value& val) const = 0;

    virtual bool                getBool() const = 0;
    virtual double              getDouble() const = 0;
    virtual const COMMON_NS::String&       getString() const = 0;
    virtual const NodeSet&      getNodeSet() const = 0;
    //! Returns the value converted to given type
    virtual ConstValuePtr       to(Type type) const = 0;
    
    static Value*               makeBool(bool);
    static Value*               makeString(const Common::String&);
    static Value*               makeDouble(double);
    static Value*               makeNodeSet(const NodeSet&);

    virtual void                dump() const = 0;
};

} // namespace Xpath

#endif // XPATH_VALUE_H
