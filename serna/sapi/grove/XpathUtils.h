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
#ifndef SAPI_XPATH_UTILS_H_
#define SAPI_XPATH_UTILS_H_

#include "sapi/sapi_defs.h"
#include "sapi/grove/GroveNodes.h"
#include "sapi/common/RefCntPtr.h"
#include "sapi/common/Vector.h"

namespace SernaApi {

/// Linear list of GroveNodes
class SAPI_EXPIMP XpathNodeSet : public Vector<GroveNode> {
public:
    /// Returns first node of nodeset
    GroveNode       firstNode() const;
    /// Returns last node of nodeset
    GroveNode       lastNode() const;
};

/// Returned by XPath eval() function.
/*! It can represent
 *  four basic types (string, double, bool, node-set), as described in 
 *  XPath specification.
 */
class SAPI_EXPIMP XpathValue : public RefCountedWrappedObject {
public:
    XpathValue(const SString&);
    XpathValue(double);
    XpathValue(bool);
    XpathValue(const XpathNodeSet&);
    ~XpathValue();
    
    /// Get xpath value as string
    SString         getString() const;
    /// Get xpath value as double
    double          getDouble() const;
    /// Get xpath value as nodeset
    XpathNodeSet    getNodeSet() const;
    /// Get xpath value as boolean
    bool            getBool() const;
    /// Set xpath value as string    
    void            setString(const SString&);
    /// Set xpath value as double
    void            setDouble(double);
    /// Set xpath value as boolean
    void            setBool(bool);
    /// Set xpath value as nodeset
    void            setNodeSet(const XpathNodeSet&);

    /// Force value change notification. This is intended primarily for the
    /// user-defined XSLT functions, which must notify XSLT about change
    /// of their return value.
    void            notifyChanged();
   
    // for internal use only
    XpathValue(SernaApiBase* = 0);
};

/// Builds XPath expression and evaluates it in the specified context.
class SAPI_EXPIMP XpathExpr : public RefCountedWrappedObject {
public:
    /// Constructs XPath expression. If you use namespaces, you must also
    /// supply \a nsResolver for resolution of namespace prefixes within 
    /// expression.
    XpathExpr(const SString& expr, 
              const GroveNodeWithNamespace& nsResolver = 
                  GroveNodeWithNamespace(0));
    
    /// Evaluate an expression in \a context, and return the value. 
    XpathValue eval(const GroveNode& context) const;
};

/// XpathValue container.
class SAPI_EXPIMP XpathValueList : public Vector<XpathValue> {};

/// Is for making external functions.
class SAPI_EXPIMP XsltExternalFunction : public RefCountedWrappedObject {
public:
    /// Create extension function with given name and namespace URI
    XsltExternalFunction(const SString& name, const SString& nsUri);
    virtual ~XsltExternalFunction();

    /// This function must evaluate argument list and return a value
    virtual XpathValue eval(const XpathValueList&) const;
};

} // namespace SernaApi

#endif // SAPI_XPATH_UTILS_H_

