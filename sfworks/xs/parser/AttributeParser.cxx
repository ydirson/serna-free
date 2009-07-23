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
#include "xs/XsMessages.h"
#include "xs/Schema.h"
#include "xs/parser/AttributeParser.h"
#include "xs/XsDataImpl.h"
#include "xs/complex/Particle.h"
#include "grove/Nodes.h"
#include <iostream>

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

AttributeParser::AttributeParser(const GroveLib::Element* elem,
                                 Schema* schema,
                                 bool autoCheck)
  : schema_(schema), autoCheck_(autoCheck), origin_(elem)
{
    const XsDataImpl* xsi = schema->xsi();
    for (Attr* a = elem->attrs().firstChild(); a; a = a->nextSibling()) {
        if (xsi->isSchemaNode(a) || xsi->isSchemaExtNode(a))
            attrs_.push_back(AttrItem(a));
    }
}

bool AttributeParser::checkExt(const String& name) const
{
    for (ulong i = 0; i < attrs_.size(); ++i) {
        const GROVE_NAMESPACE::Attr* a = attrs_[i].attr;
        if (name == a->localName()) {
            if (schema_->xsi()->isSchemaExtNode(a))
                return true;
            if(schema_)
                schema_->mstream() << XsMessages::isNotAnExtAttr
                << Message::L_ERROR  << a->name() << XSN_ORIGIN(origin_);
            return false;
        }
    }
    return false;
}

bool AttributeParser::get(const String& name, String* vp)
{
    for (ulong i = 0; i < attrs_.size(); i++) {
        if (name == attrs_[i].attr->localName()) {
            attrs_[i].used = true;
            if (vp)
                *vp = attrs_[i].attr->value();
            return true;
        }
    }
    return false;
}

bool AttributeParser::has(const String& name) const
{
    for (ulong i = 0; i < attrs_.size(); i++) {
        if (name == attrs_[i].attr->localName())
            return true;
    }
    return false;
}


bool AttributeParser::getTokenized(const String& name, Vector<String>& tokens)
{
    for (ulong i = 0; i < attrs_.size(); i++) {
        if (name == attrs_[i].attr->localName()) {
            attrs_[i].used = true;
            String attr_set = attrs_[i].attr->value();
            attr_set.simplifyWhiteSpace();
            int end = 0;
            int start = 0;
            while (attr_set.length() > ulong(start)) {
                end = attr_set.find(' ', start);
                if (-1 == end)
                    end = attr_set.length();
                String temp = attr_set.mid(start, end - start);
                tokens.push_back(temp);
                start = end + 1;
            }
            return true;
        }
    }
    return false;
}

bool AttributeParser::getBoolean(const String& name, bool* bv)
{
    for (ulong i = 0; i < attrs_.size(); i++) {
        if (name == attrs_[i].attr->localName()) {
            attrs_[i].used = true;
            String test = attrs_[i].attr->value().lower();
            bool rv;
            if ("true" == test || "1" == test) {
                rv = true;
            } else if ("false" == test || "0" == test) {
                rv = false;
            } else {
                schema_->mstream() << XsMessages::noValidBool
                                   << Message::L_ERROR << XSN_ORIGIN(origin_);
                rv = false;
            }
            if (bv)
                *bv = rv;
            return true;
        }
    }
    return false;
}

bool AttributeParser::getUnsigned(const String& name, uint* u)
{
    for (ulong i = 0; i < attrs_.size(); i++) {
        if (name == attrs_[i].attr->localName()) {
            attrs_[i].used = true;
            String test = attrs_[i].attr->value();
            bool is_ok;
            if (u)
                *u = test.toUInt(&is_ok);
            if (!is_ok) {
                schema_->mstream() << XsMessages::noValidInt
                                   << Message::L_ERROR
                                   << XSN_ORIGIN(origin_);
            }
            return true;
        }
    }
    return false;
}

bool AttributeParser::check() const
{
    bool isOk = true;
    for (ulong i = 0; i < attrs_.size(); i++) {
        if (!attrs_[i].used) {
            schema_->mstream() << XsMessages::notUsedAttr
                               << Message::L_ERROR
                               << attrs_[i].attr->name()
                               << XSN_ORIGIN(origin_);
            isOk = false;
        }
    }
    return isOk;
}

AttributeParser::~AttributeParser()
{
    if (autoCheck_)
        check();
}

void AttributeParser::dump() const
{
#ifdef XS_DEBUG
    std::cerr << NOTR("AttributeParser: ");
    for (ulong i = 0; i < attrs_.size(); ++i) {
        const AttrItem& a = attrs_[i];
        std::cerr << a.attr->localName() << '/' << a.attr->value()
            << '/' << a.used << ' ';
    }
    std::cerr << std::endl;
#endif // XS_DEBUG
}

XS_NAMESPACE_END
