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

#include "xs/xs_defs.h"
#include "xs/components/XsAnyAttribute.h"
#include "grove/Nodes.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

XsAnyAttribute::XsAnyAttribute(Wildcard* w, const Origin& origin, const XsId& xsid)
    : Piece(origin, xsid),
      wildcard_(w)
{
}

bool XsAnyAttribute::validate(Schema*, RefCntPtr<GROVE_NAMESPACE::Attr> attr) const
{
    return wildcard()->match(attr->expandedName().uri());
    //TODO:processContents
}


void XsAnyAttribute::dump(int) const
{
#ifdef SE_DEBUG
    for(int i = 0; i < indent; i++) {
         std::cerr << ' ';
    }
    if (wildcard())
        std::cerr << NOTR("XsAnyAttribute:")
                  << NOTR("[NS:")
                  << wildcard()->format()
                  << NOTR(", ProcessContents:")
                  << wildcard()-> processMode()
                  << NOTR("]")
                  << std::endl;
    else
        std::cerr << NOTR("XsAnyAttribute is broken") << std::endl;
#endif // SE_DEBUG
}
XS_NAMESPACE_END
