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
#include "sapi/grove/GroveNodes.h"
#include "sapi/grove/GrovePos.h"
#include "sapi/common/impl/xtn_wrap_impl.h"
#include "grove/Node.h"
#include "groveeditor/GrovePos.h"
#include "common/safecast.h"
#include "common/asserts.h"

#define NODE_REP(n)     static_cast<GroveLib::Node*>(n.getRep())
#define GPOS            ((GroveEditor::GrovePos*) pvt_)

namespace SernaApi {

GrovePos::GrovePos(const ::GroveEditor::GrovePos& pos)
{
    new (pvt_) GroveEditor::GrovePos(pos);
}

GrovePos::operator ::GroveEditor::GrovePos() const
{
    return *((GroveEditor::GrovePos*) pvt_);
}

GrovePos::GrovePos()
{
    RT_ASSERT(sizeof(GroveEditor::GrovePos) == sizeof(pvt_));
    new (pvt_) GroveEditor::GrovePos;
}

GrovePos::GrovePos(const GroveNode& node)
{
    RT_ASSERT(sizeof(GroveEditor::GrovePos) == sizeof(pvt_));
    new (pvt_) GroveEditor::GrovePos(NODE_REP(node));
}

GrovePos::GrovePos(const GroveNode& node,
                   const GroveNode& before)
{
    RT_ASSERT(sizeof(GroveEditor::GrovePos) == sizeof(pvt_));
    new (pvt_) GroveEditor::GrovePos(NODE_REP(node), NODE_REP(before));
}

GrovePos::GrovePos(const GroveText& text, long idx)
{
    RT_ASSERT(sizeof(GroveEditor::GrovePos) == sizeof(pvt_));
    new (pvt_) GroveEditor::GrovePos
        (static_cast<GroveLib::Text*>(text.getRep()), idx);
}

GrovePos::GrovePos(const SString& path, const GroveNode& root)
{
    RT_ASSERT(sizeof(GroveEditor::GrovePos) == sizeof(pvt_));
    new (pvt_) GroveEditor::GrovePos(path, NODE_REP(root));
}

SString GrovePos::asString() const
{
    return GPOS->asString();
}

bool GrovePos::isNull() const
{
    return GPOS->isNull();
}

GrovePos::Type GrovePos::type() const
{
    return (Type) GPOS->type();
}

GroveNode GrovePos::node() const
{
    return GPOS->node();
}

GroveNode GrovePos::before() const
{
    return GPOS->before();
}

GroveText GrovePos::text() const
{
    return GPOS->text();
}

long GrovePos::idx() const
{
    return GPOS->idx();
}

GroveErs GrovePos::getErs() const
{
    return GPOS->getErs();
}

void GrovePos::dump() const
{
    GPOS->dump();
}

} // namespace SernaApi
