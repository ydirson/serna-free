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

#include "formatter/impl/Property.h"

USING_COMMON_NS;

namespace Formatter {

/*
 */
void Property::calculate(const String& specified, ParserContext* context,
                         const Allocation& alloc, const CType& percentBase)
{
    DBG(XSL.PROP) << "Property: calculate:" << propertyName()
                  << " = " << specified << std::endl;
    isExplicit_ = true;
    isModified_ = false;
    if (-1 != percentBase)
        percentBase_ = percentBase;
    PropertyParser parser(*context, alloc, percentBase_);
    if (!calc(specified, parser))
        init(context, alloc, percentBase_);
}

void tokenize(const COMMON_NS::String& specified, StringList& tokenList)
{
    String spec = specified;
    spec.simplifyWhiteSpace();
    const uint len = spec.length();
    uint c = 0;
    while (c < len) {
        const int next_space = spec.find(' ', c);
        if (-1 != next_space) {
            tokenList.push_back(spec.mid(c, next_space - c));
            c = next_space + 1;
        }
        else {
            tokenList.push_back(spec.mid(c));
            break;
        }
    }
}

/*
 */
void Shorthand::calculate(const String& specified, ParserContext* context,
                          const Allocation& alloc, const CType& percentBase)
{
    DBG(XSL.PROP) << "Shorthand: calculate:" << propertyName()
                  << " = " << specified << std::endl;
    isExplicit_ = true;
    isModified_ = false;
    if (-1 != percentBase)
        percentBase_ = percentBase;

    StringList spec_list;
    tokenize(specified, spec_list);
    /*
    String spec = specified;
    spec.simplifyWhiteSpace();
    const uint len = spec.length();
    uint c = 0;
    while (c < len) {
        const uint next_space = spec.find(' ', c);
        if (-1 != next_space) {
            spec_list.push_back(spec.mid(c, next_space - c));
            c = next_space + 1;
        }
        else {
            spec_list.push_back(spec.mid(c));
            break;
        }
    }
    */
    if (spec_list.empty() || !calc(spec_list, context, alloc))
        init(context, alloc, percentBase_);
}

/*
 */
bool NumericProperty::calc(const COMMON_NS::String& specified,
                           PropertyParser& parser)
{
    bool is_relative = false;
    CType calc_value = value_;
    if (parser.parseNumeric(specified, (PropertyContext*)this,
                            calc_value, is_relative)) {
        DBG(XSL.PROP) << "NumericProperty calc " << propertyName() << " : "
                      << specified << " = " << calc_value << std::endl;
        value_ = calc_value;
    }
    else {
        DBG(XSL.PROP) << "Error parsing property " << propertyName()
                      << ":" << parser.errMsg() << std::endl;
        return false;
    }
    return true;
}

/*
 */
bool LiteralProperty::calc(const COMMON_NS::String& specified,
                           PropertyParser& parser)
{
    if (!parser.parseLiteral(specified, (PropertyContext*)this, value_)) {
        DBG(XSL.PROP) << "Error parsing property " << propertyName()
                      << ":" << parser.errMsg() << std::endl;
        return false;
    }
    return true;
}

/*
 */
bool BooleanProperty::calc(const COMMON_NS::String& specified,
                        PropertyParser& parser)
{
    bool is_relative = false;
    CType calc_value = value_;
    if (parser.parseNumeric(specified, (PropertyContext*)this,
                            calc_value, is_relative))
        value_ = (0 != calc_value);
    else {
        DBG(XSL.PROP) << "Error parsing property " << propertyName()
                      << ":" << parser.errMsg() << std::endl;
        return false;
    }
    return true;
}

bool BooleanProperty::resolveEnumToken(const String& token, ValueTypePair& rv,
                                       const ParserContext&, 
                                       const Allocation&) const
{
    if ("true"== token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = 1;
        return true;
    }
    else
        if ("false"== token) {
            rv.type_ = Value::V_NUMERIC;
            rv.value_.nval_ = 0;
            return true;
        }
    return false;
}

/*
 */
class ColorSpec {
public:
    String  name_;
    uint    color_;
};

static ColorSpec color_list[] = {
    { "black",  0x000000 },
    { "gray",   0x808080 },
    { "silver", 0xC0C0C0 },
    { "white",  0xFFFFFF },
    { "maroon", 0x800000 },
    { "red",    0xFF0000 },
    { "purple", 0x800080 },
    { "fuchsia",0xFF00FF },
    { "green",  0x008000 },
    { "lime",   0x00FF00 },
    { "olive",  0x808000 },
    { "yellow", 0xFFFF00 },
    { "navy",   0x000080 },
    { "blue",   0x0000FF },
    { "teal",   0x008080 },
    { "aqua",   0x00FFFF }
};

static uint color_list_size = sizeof(color_list) / sizeof(ColorSpec);

bool ColorProperty::calc(const COMMON_NS::String& specified,
                         PropertyParser& parser)
{
    uint color = 0;
    if (parser.parseColor(specified, (PropertyContext*)this, color))
        value_ = Rgb((color >> 16)  & 0xff,
                     (color >> 8)   & 0xff,
                     (color)        & 0xff);
    else {
        DBG(XSL.PROP) << "Error parsing property " << propertyName()
                      << ":" << parser.errMsg() << std::endl;
        return false;
    }
    return true;
}

bool ColorProperty::resolveEnumToken(const String& token, ValueTypePair& rv,
                                     const ParserContext&,
                                     const Allocation&) const
{
    for (uint c = 0; c < color_list_size; c++) {
        if (token == color_list[c].name_) {
            rv.type_ = Value::V_COLOR;
            rv.value_.ival_ = color_list[c].color_;
            return true;
        }
    }
    return false;
}

}
