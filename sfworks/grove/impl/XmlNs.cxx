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

#include "grove/XmlNs.h"
#include "grove/Nodes.h"
#include "grove/XmlPredefNs.h"
#include "grove/Grove.h"

using Common::String;

namespace GroveLib {

namespace XmlNs {

const String& defaultNs() 
{
    static String default_ns("");
    return default_ns;
}

const String& xmlnsatt() 
{
    static String xmlns("xmlns");
    return xmlns;
}    

const String& xmlpref()
{
    static String xmlpref("xml");
    return xmlpref;
}

const String& xincludeNs()
{
    static String xins(W3C_XINCLUDE_NAMESPACE);
    return xins;
}

bool parseXmlNsAtt(const String& att, String& prefix)
{
    if (0 != att.find(XmlNs::xmlnsatt(), 0))
        return false;
    int colonpos = att.find(':', 5);
    if (colonpos < 0) {
        if (att != XmlNs::xmlnsatt())
            return false;
        prefix = XmlNs::defaultNs();
    }
    else {
        if (colonpos != 5)
            return false;
        prefix = att.mid(colonpos + 1);
    }
    return true;
}

void makePredefinedNamespaces(Grove* grove)
{
    Element* d = grove->document()->documentElement();
    if (!d)
        return;
    if (d->lookupPrefixMap(XmlNs::defaultNs()).isNull())
        d->addToPrefixMap(XmlNs::defaultNs(), XmlNs::defaultNs());
    if (d->lookupPrefixMap(XmlNs::xmlpref()).isNull())
        d->addToPrefixMap(XmlNs::xmlpref(), W3C_XML_NAMESPACE);
}

} // namespace XmlNs

} // namespace GroveLib
