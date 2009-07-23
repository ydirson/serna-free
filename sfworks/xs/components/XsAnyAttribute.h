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

#ifndef XS_ANYATTRIBUTE_H_
#define XS_ANYATTRIBUTE_H_

#include "xs/Piece.h"
#include "grove/grove_defs.h"
#include "grove/Decls.h"
#include "xs/complex/Wildcard.h"

class Schema;

XS_NAMESPACE_BEGIN

/*! Application-specific information associated with particular
    schema construct.
 */
class XsAnyAttribute : public Piece {
public:
    Wildcard*       wildcard() const;

    XsAnyAttribute(Wildcard* w, const Origin& origin,
                   const XsId& xsid = XsId());

    /*! Validate attribute \a Attr
     */
    bool validate(Schema* s, COMMON_NS::RefCntPtr<GROVE_NAMESPACE::Attr> attr) const;

    /*! Dump Any attribute information item
     */
    virtual void dump(int indent) const;

    XS_OALLOC(XsAnyAttribute);

private:
    COMMON_NS::RefCntPtr<Wildcard> wildcard_;
};

inline Wildcard* XsAnyAttribute::wildcard() const
{
    return wildcard_.pointer();
}

XS_NAMESPACE_END

#endif // XS_ANYATTRIBUTE_H_
