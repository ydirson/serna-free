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

#include "xslt/xslt_defs.h"
#include "xslt/XsltResource.h"
#include "xslt/impl/DocumentHolder.h"
#include "xslt/impl/WhitespaceStripper.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/debug.h"
#include "grove/Nodes.h"
#include "grove/Grove.h"
#include "spgrovebuilder/SpGroveBuilder.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

namespace Xslt {

DocumentHolder::DocumentHolder(XsltResource* xsltRes)
    : xsltResource_(xsltRes)
{
}

DocumentHolder::~DocumentHolder()
{
}

void DocumentHolder::addGrove(Uri::GroveResource* gr)
{
    groves_.push_back(gr);
    if (!gr->parent())
        xsltResource_->appendChild(gr);
}

COMMON_NS::String DocumentHolder::findUri(const GroveLib::Node* n) const
{
    const GroveLib::Grove* g = n->grove();
    if (g) {
        GroveList::const_iterator it = groves_.begin();
        for (; it != groves_.end(); ++it) {
            if (dynamic_cast<Uri::GroveResource*>(it->pointer())->grove() == g)
                return (*it)->uri();
        }
    }
    return *uriStack_.begin();
}

} // namespace Xslt
