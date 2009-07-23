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

#include "grove/Grove.h"
#include "grove/Nodes.h"
#include "xpath/NodeSet.h"
#include "xslt/impl/xpath_values.h"
#include "xslt/impl/ResultTreeFragment.h"
#include "xslt/impl/debug.h"

namespace Xslt {

Xpath::Value* ResultTreeFragment::emptyValue_ = 0;

ResultTreeFragment::ResultTreeFragment()
{
    GroveLib::DocumentFragment* rtf = new GroveLib::DocumentFragment;
    rtf_ = rtf;
    Xpath::NodeSet node_set;
    node_set += rtf_.pointer();
    rtf_->registerNodeVisitor(this,
        GroveLib::NodeVisitor::NOTIFY_CHILD_INSERTED|
        GroveLib::NodeVisitor::NOTIFY_CHILD_REMOVED);
    rtfRoot_ = new Xpath::NodeSetValue(node_set);
}

Xpath::ConstValuePtr ResultTreeFragment::value() const
{
    if (rtf_->firstChild())
        return rtfRoot_;
    if (0 == emptyValue_) {
        Xpath::NodeSet node_set;
        emptyValue_ = new Xpath::NodeSetValue(node_set);
        emptyValue_->incRefCnt();
    }
    return emptyValue_;
}

ResultTreeFragment::~ResultTreeFragment()
{
}

} // namespace Xslt

