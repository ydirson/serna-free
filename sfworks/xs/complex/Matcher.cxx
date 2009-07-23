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
#include "xs/complex/FsmMatcher.h"
#include "xs/complex/Connectors.h"
#include "xs/Schema.h"

XS_NAMESPACE_BEGIN

Matcher* Matcher::build(Schema* s, Connector* top,
                        const COMMON_NS::String& name)
{
    Connector* chain = top;
    while (chain && chain->type() == Connector::GROUPREF)
        chain = static_cast<GroupConnector*>(chain)->connector();
    if (chain && chain->type() == Connector::ALL) 
        return static_cast<AllConnector*>(chain)->buildAllMatcher(s);
    if (s && s->pstream() && !name.isEmpty())
        *(s->pstream()) << ("XSV: making FSM for " + name);
    return new FsmMatcher(s, top);
}

/////////////////////////////////////////////////////////////

XS_NAMESPACE_END
