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

#ifndef XSELEMENT_H_
#define XSELEMENT_H_

#include "xs/XsData.h"
#include "xs/Component.h"
#include "xs/XsData.h"
#include "grove/Decls.h"
#include "xs/XsEnumList.h"

GROVE_NAMESPACE_BEGIN
    class QualifiedName;
GROVE_NAMESPACE_END

namespace Common {
    class PropertyNode;
}

XS_NAMESPACE_BEGIN

class FixupSet;
class XsType;

/*! Application-specific information associated with particular
    schema construct.
 */
class XS_EXPIMP XsElement : public Component {
public:
    typedef GROVE_NAMESPACE::Element Element;
    typedef GROVE_NAMESPACE::Grove Grove;
    typedef GROVE_NAMESPACE::Node Node;
    typedef GROVE_NAMESPACE::ElementPtr ElementPtr;
    typedef GROVE_NAMESPACE::QualifiedName QualifiedName;
    /*! Validate source and return result - modified source in accordance
        with other constraints.
    */
    virtual bool validate(Schema* schema,
                          Element* elem,
                          bool recursive = true) const = 0;

    /* Create (non-recursive) instance of an element type
     */
    virtual void makeInstance(Schema* s,
                              GROVE_NAMESPACE::Grove* referenceGrove,
                              GROVE_NAMESPACE::ElementPtr& ep,
                              const Element* pe = 0) const = 0;

    /*! Recursively create content of an existing element type
     */
    virtual bool makeContent(Schema* s, Element* pe,
                             FixupSet* elemSet) const = 0;

    /*! Create recursive skeleton of an element type.
     */
    virtual bool makeSkeleton(Schema* s,
                              GROVE_NAMESPACE::Grove* referenceGrove,
                              GROVE_NAMESPACE::ElementPtr& ep,
                              const Element* pe = 0) const = 0;

    /*! Returns true if it is not ElementStub.
     */
    virtual bool isValid() const = 0;
    virtual bool isAnyType() const = 0;

    virtual void getPossibleEnumlist(Common::PropertyNode* node) const = 0;
    virtual bool cdataAllowed() const = 0;

    virtual XsType* xstype() const = 0;

    /*! Debug dump
     */
    virtual void    dump(int indent) const = 0;

    static XsElement* makeEmptyStub(const NcnCred&);

    XsElement(const Origin& origin, const NcnCred&);

    virtual ~XsElement();

    PRTTI_DECL(XsElement);
};

XS_NAMESPACE_END

#endif // XSELEMENT_H_
