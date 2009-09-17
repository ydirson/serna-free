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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "utils.h"
#include "utils/DocSrcInfo.h"
#include "xslt/ResultOrigin.h"
#include "grove/ElementMatcher.h"
#include "grove/Nodes.h"

using namespace Common;
using namespace GroveLib;

ElementSkipper::ElementSkipper(const Common::PropertyNode* dsi)
{
    const PropertyNode* spellProps =
        dsi->getProperty(DocSrcInfo::SPELLER_PROPS);
    if (0 == spellProps)
        return;
    String skipList =
        spellProps->getSafeProperty(DocSrcInfo::SPELLER_SKIPELEMS)->getString();
    if (skipList.isEmpty())
        return;
    em_ = new GroveLib::ElementMatcher(skipList);
}

bool ElementSkipper::mustSkip(const GroveLib::Node* fo_node) const
{
    const Node* n = Xslt::resultOrigin(fo_node);
    if (!n)
        return true;   // skip generated text
    if (em_.isNull())
        return false;
    const Node* pn = parentNode(n);
    if (!pn || pn->nodeType() != Node::ELEMENT_NODE)
        return false;
    return em_->matchElement(CONST_ELEMENT_CAST(pn));
}

ElementSkipper::~ElementSkipper()
{
}

////////////////////////////////////////////////////////////

Common::String get_lang(const GroveLib::Node* np)
{
    for (np = Xslt::resultOrigin(np); np; np = parentNode(np)) {
        if (np->nodeType() != Node::ELEMENT_NODE) 
            continue;
        const Element* ep = static_cast<const Element*>(np);
        const Attr* lattr = ep->attrs().firstChild();
        for (; lattr; lattr = lattr->nextSibling())
            if (lattr->localName() == NOTR("lang"))
                return lattr->value();
    }
    return String::null();   
}
