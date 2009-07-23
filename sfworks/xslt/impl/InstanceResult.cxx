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

#include "xslt/impl/InstanceResult.h"

namespace Xslt {

InstanceResult::InstanceResult(GroveLib::Node* parent)
    : parent_(parent)
{
}

InstanceResult::InstanceResult(GroveLib::Node* parent, GroveLib::Node* node)
    : parent_(parent),
      resultNode_(node)
{
}

void InstanceResult::dispose()
{
    if (resultNode_.isNull())
        return;
    resultNode_->remove();
    resultNode_.clear();
}

void InstanceResult::insertBefore(XsltResult* result, GroveLib::Node* before)
{
    RT_ASSERT(!before || GroveLib::Node::ATTRIBUTE_NODE != before->nodeType());

    resultNode_ = result->node();

    if (result->node()->nodeType() == GroveLib::Node::ATTRIBUTE_NODE) {
        RT_ASSERT(GroveLib::Node::ELEMENT_NODE == parent_->nodeType());
        GroveLib::Element* const elem =
            static_cast<GroveLib::Element*>(parent_.pointer());
        GroveLib::Attr* const attr =
            static_cast<GroveLib::Attr*>(resultNode_.pointer());
        //RT_ASSERT(!elem->attrs().getAttribute(attr->name()));
        elem->attrs().setAttribute(attr);
    } else {
        if (before)
            before->insertBefore(resultNode_.pointer());
        else
            parent_->appendChild(resultNode_.pointer());
    }
}

} // namespace Xslt
