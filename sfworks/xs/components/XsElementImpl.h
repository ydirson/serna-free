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

#ifndef XS_ELEMENT_IMPL_H_
#define XS_ELEMENT_IMPL_H_

#include "xs/XsElement.h"
#include "xs/ComponentRef.h"
#include "xs/complex/Particle.h"
#include "common/prtti.h"
#include "grove/Decls.h"


class Schema;
class GroveAstParser;

XS_NAMESPACE_BEGIN

class Connector;

/*! Application-specific information associated with particular
    schema construct.
 */
class XsElementImpl : public XsElement {
public:
    enum ElementForm  {
        QUALIFIED, UNQUALIFIED
    };
    enum NoDerivation {
        ALL, SUBSTITUTION, EXTENSION, RESTRICTION, NONE
    };

    ElementForm         elementForm() const;
    NoDerivation        noDerivation() const;

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema,
                          Element* elem,
                          bool recursive) const;

    virtual bool makeSkeleton(Schema* s,
                              Grove* referenceGrove,
                              ElementPtr& ep,
                              const Element* pe = 0) const;

    virtual void makeInstance(Schema* s,
                              Grove* referenceGrove,
                              ElementPtr& ep,
                              const Element* pe = 0) const;
    virtual bool cdataAllowed() const;
    virtual bool makeContent(Schema* s, Element* pe, FixupSet* elemSet) const;
    virtual bool isValid() const;
    virtual bool substGroup(GroveLib::ExpandedName& ename) const;
    virtual void getPossibleEnumlist(Common::PropertyNode* node) const;
    virtual void dump(int indent) const;
    virtual bool isAnyType() const { return isAnyType_; }

    virtual XsType*      xstype() const;

    XsElementImpl(const Origin& origin,
                  const NcnCred& cred, bool isTopLevel = false);

    virtual ~XsElementImpl();

    XS_OALLOC(XsElementImpl);

private:
    friend class ::GroveAstParser;
    friend class Connector;
    friend class XsDataImpl;

    void                addIConstraint(const XsIdentityConstraintInst& key);
    bool                checkIConstraint(Schema* schema,
                                         Element* elem) const;
    void                make_inst(Grove* referenceGrove,
                                  const Element* pe,
                                  ElementPtr& ep) const;
    bool                abstract_;
    NoDerivation        block_;
    String              default_;
    NoDerivation        final_;
    String              fixed_;
    ElementForm         form_;
    bool                nullable_;
    GroveLib::ExpandedName substitutionGroup_;
    XsTypeInst          baseType_;
    COMMON_NS::Vector<XsIdentityConstraintInst> keyVector_;
    bool                isAnyType_;
    bool                isTopLevel_;
};

inline XsElementImpl::ElementForm XsElementImpl::elementForm() const
{
    return form_;
}

inline XsElementImpl::NoDerivation XsElementImpl::noDerivation() const
{
    if (NONE == block_)
        return final_;
    return block_;
}

XS_NAMESPACE_END

#endif // XS_ELEMENT_IMPL_H_
