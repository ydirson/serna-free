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

#include "grove/grove_defs.h"
#include "grove/grove_trace.h"
#include "grove/XmlNs.h"
#include "grove/XmlName.h"
#include "grove/Nodes.h"

#include "common/String.h"
#include "common/Debug.h"

using Common::String;

GROVE_NAMESPACE_BEGIN

void QualifiedName::parse(const String& name)
{
    int colonpos = name.find(':');
    if (colonpos < 0) {
        first = name;
        second = XmlNs::defaultNs();
    }
    else {
        first  = name.mid(colonpos + 1);
        second = name.left(colonpos);
    }
}

bool ExpandedName::resolve(const NodeWithNamespace* context,
                           const QualifiedName& qname)
{
    second = context->getXmlNsByPrefix(qname.prefix());
    first = qname.localName();
    return !second.isNull();
}

bool QualifiedName::resolve(const Element* context,
                            const ExpandedName& ename)
{
    first = ename.localName();
    if (context && ename.isQualified()) {
        String prefix = context->getPrefixByXmlNs(ename.uri());
        if (!prefix.isNull() && !prefix.isEmpty())
            second = prefix;
        //TODO: generate prefix if needed
    }
    return !second.isNull();
}

String QualifiedName::asString() const
{
    if (prefix().isEmpty())
        return localName();
    else
        return prefix() + ":" + localName();
}

void QualifiedName::dump() const
{
#ifdef GROVE_DEBUG
    DDBG << '{' << prefix().utf8() << '#' << localName().utf8() << '}';
#endif // GROVE_DEBUG
}

void ExpandedName::dump() const
{
#ifdef GROVE_DEBUG
    DDBG << '{' << uri().utf8() << '#' << localName().utf8() << '}';
#endif // GROVE_DEBUG
}

GROVE_NAMESPACE_END
