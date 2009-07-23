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

#ifdef _MSC_VER
// XsType declaration must be present in order to compile ComponentInst<Component>
# include "xs/XsType.h"
#endif

#include "xs/xs_defs.h"
#include "grove/Nodes.h"
#include "xs/complex/XsContent.h"
#include "xs/complex/ComplexContent.h"
#include "xs/complex/SimpleContent.h"
#include "xs/components/AttributeSet.h"
#include "xs/components/XsAnyAttribute.h"
#include "xs/XsAttribute.h"
#include "xs/XsAttributeGroup.h"
#include "xs/XsId.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

XsContent::XsContent(DerivationType type,
                     const Origin& o,
                     const XsId& xsid)
    : Piece(o, xsid), derivationType_(type)
{
    attributeSet_ = new AttributeSet;
}

void XsContent::makeAttrs(Schema* schema, GROVE_NAMESPACE::Element* pe) const
{
    attributeSet_->makeSkeleton(schema, pe);
}

XsContent::DerivationType XsContent::derivationType() const
{
    return derivationType_;
}

const RefCntPtr<AttributeSet> XsContent::attributeSet() const
{
    return attributeSet_;
}

void XsContent::setAttributeSet(AttributeSet* aset)
{
    attributeSet_ = aset;
}

XsContent::~XsContent()
{
}

void XsContent::dumpAttrs(int indent) const
{
    (void) indent;
#ifdef XS_DEBUG
    if(!attributeSet_.isNull())
        attributeSet_->dump(indent+1);
#endif // XS_DEBUG
}

PRTTI_BASE_STUB(XsContent, ComplexContent)
PRTTI_BASE_STUB(XsContent, SimpleContent)

XS_NAMESPACE_END
