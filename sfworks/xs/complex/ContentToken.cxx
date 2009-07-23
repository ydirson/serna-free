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
#include "xs/complex/ContentToken.h"
#include "common/String.h"
#include "grove/Nodes.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

bool ContentToken::match(const SeqEndToken* t) const
{
    return t == this;     // seqEndToken matches only to itself
}

String ContentToken::format_proto(const char* n) const
{
    String os(n);
#ifdef XS_DEBUG
    os += "[" + String::number(id_) + "]";
#endif // XS_DEBUG
    return os;
}

bool ContentToken::less(const ContentToken* t) const
{
    return dataaddr() < t->dataaddr();
}

const void* ContentToken::dataaddr() const
{
    return this;
}

///////////////////////////////////////////////////////

bool WildcardToken::match(const ContentToken* t) const
{
    return t->match(this);
}

bool WildcardToken::match(const WildcardToken* t) const
{
    return wildcard_->match(t->wildcard_.pointer());
}

bool WildcardToken::match(const QnameToken* t) const
{
    return wildcard_->match(t->ename().uri());
}

String WildcardToken::format() const
{
    String os(format_proto("Wildcard"));
    os += "<" + wildcard_->format() + ">";
    return os;
}

const void* WildcardToken::dataaddr() const
{
    return wildcard_.pointer();
}

///////////////////////////////////////////////////////

bool QnameToken::match(const ContentToken* t) const
{
    return t->match(this);
}

bool QnameToken::match(const WildcardToken* t) const
{
    return t->match(this);
}

const void* QnameToken::dataaddr() const
{
    return &ename_;
}

bool QnameToken::match(const QnameToken* t) const
{
    return (ename_.localName() == t->ename_.localName() &&
            ename_.uri() == t->ename_.uri());
}

String QnameToken::format() const
{
    if (ename_.uri().isEmpty())
        return ename_.localName();
    String os(format_proto("Qname"));
    os += '{' + ename_.uri() + ',' + ename_.localName() + '}';
    return os;
}

///////////////////////////////////////////////////////

InputToken::InputToken(GROVE_NAMESPACE::Element* elem)
  : QnameToken(expandedName_, 0), expandedName_(elem->expandedName()),
    elem_(elem)
{
}

InputToken::InputToken()
  : QnameToken(GROVE_NAMESPACE::ExpandedName("<unnamed>"), 0), elem_(0)
{
}

InputToken::~InputToken()
{
}

///////////////////////////////////////////////////////

bool SeqEndToken::match(const WildcardToken*) const
{
    return false;
}

bool SeqEndToken::match(const QnameToken*) const
{
    return false;
}

bool SeqEndToken::match(const ContentToken* t) const
{
    return t == this;
}

String SeqEndToken::format() const
{
    return format_proto("SE");
}

#ifdef XS_DEBUG
uint ContentToken::sequence_ = 0;
#endif // XS_DEBUG

XS_NAMESPACE_END
