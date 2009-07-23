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

#ifndef XSATTRIBUTE_GROUP_H_
#define XSATTRIBUTE_GROUP_H_

#include "xs/xs_defs.h"
#include "xs/Component.h"
#include "grove/Decls.h"

XS_NAMESPACE_BEGIN

/*! An XML Schema attribute group.
 */
class XS_EXPIMP XsAttributeGroup : public Component {
public:
    /*! Validate all attributes in \a elem against an attribute group.
     */
    virtual bool    validate(Schema* s, GROVE_NAMESPACE::Element* elem,
                             COMMON_NS::Vector<GROVE_NAMESPACE::Attr*>& attrs,
                             Common::Vector<XsAttribute*>* = 0) const = 0;

    virtual void    makeSkeleton(Schema* s, GROVE_NAMESPACE::Element* pe,
                                 Common::Vector<XsAttribute*>* = 0) const = 0;
    virtual bool    attrList(COMMON_NS::Vector<XsAttribute*>& attrList) const = 0;
    virtual void    dump(int indent) const = 0;

    static XsAttributeGroup* makeEmptyStub(const NcnCred&);

    XsAttributeGroup(const Origin& origin, const NcnCred&);

    virtual ~XsAttributeGroup();

    PRTTI_DECL(XsAttributeGroup);
};

XS_NAMESPACE_END

#endif // XSATTRIBUTE_GROUP_H_
