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
#include "xs/Schema.h"
#include "xs/complex/Wildcard.h"
#include <qstringlist.h>

USING_COMMON_NS
USING_GROVE_NAMESPACE

// START_IGNORE_LITERALS
namespace {
    static String any      = "##any";
    static String other    = "##other";
    static String targetNs = "##targetNamespace";
    static String local    = "##local";
}
// STOP_IGNORE_LITERALS

XS_NAMESPACE_BEGIN

Wildcard* Wildcard::make(Schema* schema,
                    ProcessMode pmode,
                    const String& istr)
{
    if (istr == any)
        return new AnyNsWildcard(pmode);

    if (istr == other)
        return new OtherNsWildcard(schema->targetNsUri(), pmode);

    UriListWildcard* ulw = new UriListWildcard(pmode);
    QStringList list = QStringList::split(' ', istr);
    for (uint i = 0; i < list.count(); i++) {
        String temp = list[i];
        if (targetNs == temp)
            temp = schema->targetNsUri();
        if (local == temp)
            temp = String::null();
        ulw->addUri(temp);
    }
    return  ulw;
}

bool AnyNsWildcard::match(const Wildcard*) const
{
    return true;
}

bool AnyNsWildcard::match(const String&) const
{
    return true;
}

String AnyNsWildcard::format() const
{
    return any;
}

OtherNsWildcard::OtherNsWildcard(const String& myns, ProcessMode pmode)
  : Wildcard(Wildcard::OTHER, pmode), myns_(myns),
    simplifiedMyns_(myns.lower().simplifyWhiteSpace())
{
}

bool OtherNsWildcard::match(const Wildcard* w) const
{
    if (Wildcard::LIST != w->type())
        return true;
    const UriListWildcard* ulw = static_cast<const UriListWildcard*>(w);
    for (ulong i = 0; i < ulw->uriList_.size(); i++) {
        if (match(ulw->uriList_[i]))
            return true;
    }
    return false;
}

bool OtherNsWildcard::match(const String& uri) const
{
    if (myns_.isNull())
        return true;
    if (uri.isNull())
        return false;       // ##other never matches ##local
    String nuri = TS_COPY(uri).lower().simplifyWhiteSpace();
    if (simplifiedMyns_ != nuri)
            return true;
    return false;
}

String OtherNsWildcard::format() const
{
    return other;
}

bool UriListWildcard::match(const Wildcard* w) const
{
    if (Wildcard::ANY == w->type())
        return true;
    for (uint i = 0; i < uriList_.size(); i++) {
        if (w->match(uriList_[i]))
            return true;
    }
    return false;
}

bool UriListWildcard::match(const String& uri) const
{
    String nuri = TS_COPY(uri).lower().simplifyWhiteSpace();
    for (uint i = 0; i < uriList_.size(); i++) {
        if (simplifiedUriList_[i] == nuri)
            return true;
    }
    return false;
}

void UriListWildcard::addUri(const String& uri)
{
    uriList_.push_back(uri);
    simplifiedUriList_.push_back(uri.lower().simplifyWhiteSpace());
}

String UriListWildcard::format() const
{
    String os("");
    for (ulong i = 0; i < uriList_.size(); ) {
        os += uriList_[i];
        if (++i != uriList_.size())
            os += ' ';
    }
    return os;
}

XS_NAMESPACE_END
