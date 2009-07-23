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

#ifndef XS_CONTENT_H_
#define XS_CONTENT_H_

#include "xs/xs_defs.h"
#include "xs/Piece.h"
#include "xs/Component.h"
#include "common/prtti.h"
#include "common/Vector.h"
#include "common/RefCntPtr.h"
#include "grove/Decls.h"

XS_NAMESPACE_BEGIN

class ComplexContent;
class SimpleContent;
class AttributeSet;
class FixupSet;
class ComponentInstBase;

/*! A content part of either a simple type, complex type, or
    extension/restriction sections.
 */
class XsContent : public Piece {
public:
    typedef GROVE_NAMESPACE::Element Element;
    enum DerivationType {
        NONE, EXTENSION, RESTRICTION
    };

    /*! Obtain a derivation type of a content
     */
    DerivationType      derivationType() const;

    /*! Obtain an attribute set
     */
    const COMMON_NS::RefCntPtr<AttributeSet> attributeSet() const;

    virtual bool validate(Schema* s,
                          Element* elem,
                          bool mixed,
                          bool recursive) const = 0;

    virtual bool validate(Schema* s,
                          const GroveLib::Node* o,
                          const String& source,
                          COMMON_NS::String* result) const = 0;

    void    makeAttrs(Schema* schema, Element* elem) const;

    virtual bool makeContent(Schema* schema,
                             Element* parentElem, FixupSet* elemSet,
                             const COMMON_NS::String* defValue = 0) const = 0;

    PRTTI_DECL(ComplexContent);
    PRTTI_DECL(SimpleContent);

    XsContent(DerivationType type,
              const Origin& o,
              const XsId& xsid = XsId());
    virtual ~XsContent();

    virtual bool cdataAllowed() const = 0;
    virtual void dump(int indent) const = 0;
    void dumpAttrs(int indent) const;

protected:
    friend class ::GroveAstParser;
    void setAttributeSet(AttributeSet* aset);

    DerivationType            derivationType_;
    COMMON_NS::RefCntPtr<AttributeSet> attributeSet_;
};

XS_NAMESPACE_END

#endif // XS_CONTENT_H_
