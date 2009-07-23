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

#include "grove/Nodes.h"
#include "xslt/impl/debug.h"
#include "xslt/impl/InstructionFactory.h"
#include "xslt/impl/instructions/Template.h"
#include "xslt/impl/instructions/Stylesheet.h"
#include "common/StringTokenizer.h"

using namespace Common;

namespace Xslt {

Stylesheet::Stylesheet(const GroveLib::Element* element,
                       const COMMON_NS::String& origin,
                       Instruction* p,
                       const Stylesheet* parentStylesheet,
                       const TopStylesheet* topStylesheet)
    : Instruction(element, this, p),
      origin_(origin),
      topStylesheet_(topStylesheet ?
        topStylesheet : parentStylesheet->topStylesheet()),
      parentSheet_(parentStylesheet),
      nextImported_(0),
      tprovider_(new TemplateProvider(this))
{
    attr("id",   false);
    extensionPrefixes_ = attr("extension-element-prefixes",   false);
    attr("exclude-result-prefixes",   false);
    bool ok = false;
    version_ = attr("version", true).toDouble(&ok);
    if (!ok)
        version_ = 0;
    else
        isFc_ = (1.0 < version_);
}

bool Stylesheet::isExtNode(const GroveLib::Node* n) const
{
    using namespace GroveLib;
    if (extensionPrefixes_.isEmpty())
        return false;
    switch (n->nodeType()) {
        case Node::ELEMENT_NODE:
        case Node::ATTRIBUTE_NODE:
            break;
        default:
            return false;
    }
    const NodeWithNamespace& nn = static_cast<const NodeWithNamespace&>(*n);
    StringTokenizer st(extensionPrefixes_);
    while (st) 
        if (st.next() == nn.xmlNsPrefix())
            return true;
    return false;
}

Stylesheet::~Stylesheet()
{
}

bool Stylesheet::allowsChild(Type type) const
{
    switch (type) {
        case IMPORT :
        case INCLUDE :
        case STRIP_SPACE :
        case PRESERVE_SPACE :
        case OUTPUT :
        case KEY :
        case DECIMAL_FORMAT :
        case NAMESPACE_ALIAS :
        case ATTRIBUTE_SET :
        case VARIABLE :
        case PARAM :
        case TEMPLATE :
            return true;
        default :
            return false;
    }
    return false;
}

void Stylesheet::dump() const
{
    DDBG << "Stylesheet " << this << ": v=" << version_
        << " o:" << origin_ << " p:" << parentSheet_
        << " n:" << nextImported_ << std::endl;
    Instruction::dump();
}

}

