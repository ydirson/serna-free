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

#ifndef XS_COMPLEX_TYPE_IMPL_
#define XS_COMPLEX_TYPE_IMPL_

#include "grove/Decls.h"
#include "xs/xs_defs.h"
#include "xs/XsType.h"

class Schema;

XS_NAMESPACE_BEGIN

class XsContent;
class FixupSet;

/*! An implementation of a complex type. For the documentation of a
    public functions, see docs for XsComplexType
 */
class XsComplexTypeImpl : public XsType {
public:
    virtual bool    validate(Schema* s,
                             Element* elem,
                             bool recursive) const;

    virtual bool    validate(Schema* schema,
                             const String& source,
                             String* result = 0) const;

    virtual bool    makeContent(Schema* schema,
                                Element* parentElem, FixupSet* elemSet,
                                const String* defValue = 0) const;

    virtual void    makeAttrs(Schema* schema,
                              Element* elem) const;
    virtual bool    cdataAllowed() const;

     enum NoDerivation {
        ALL, EXTENSION, RESTRICTION, NONE
    };
    NoDerivation noDerivation() const;

    XS_EXPIMP XsContent* content() const;

    const String& pyFunc() const;

    bool isMixed() const { return mixed_; }

    XsComplexTypeImpl(const Origin& origin, const NcnCred& cred);

    virtual ~XsComplexTypeImpl();

    virtual void    dump(int indent) const;

    PRTTI_DECL(XsComplexTypeImpl);
    XS_OALLOC(XsComplexTypeImpl);

private:
    friend class ::GroveAstParser;
    void setContent(COMMON_NS::RefCntPtr<XsContent>& content);
    bool check_abstract(Schema* s) const;
    void setPyFunc(String& funcName);


    COMMON_NS::RefCntPtr<XsContent>  content_;
    NoDerivation    block_;
    NoDerivation    final_;
    bool            abstract_;
    bool            mixed_;
    String          pyFunc_;
};

inline XsComplexTypeImpl::NoDerivation XsComplexTypeImpl::noDerivation() const
{
    if (NONE == block_)
        return final_;
    return block_;
}

XS_NAMESPACE_END

#endif // XS_COMPLEX_TYPE_IMPL_
