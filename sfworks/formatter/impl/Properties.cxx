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

#include "formatter/impl/Fo.h"
#include "formatter/impl/debug.h"
#include "formatter/impl/Properties.h"
#include "formatter/impl/FoImpl.h"
#include <ctype.h>

USING_COMMON_NS;

namespace Formatter {

const CType MM_PER_INCH = 25.4;
const CType PT_PER_INCH = 72;
const CType DEFAULT_PAGE_WIDTH = 210 / MM_PER_INCH;
const CType DEFAULT_PAGE_HEIGHT = 297 / MM_PER_INCH;

const CType DEFAULT_PROVISIONAL_DISTANCE = 24 / PT_PER_INCH;
const CType DEFAULT_PROVISIONAL_SEPARATION = 6 / PT_PER_INCH;

/* Page Properties
 */
String PageWidth::name_ = "page-width";

void PageWidth::init(ParserContext* pcontext, const Allocation&, const CType&)
{
    value_ = pcontext->dpi() * DEFAULT_PAGE_WIDTH;
}

bool PageWidth::resolveEnumToken(const String& token, ValueTypePair& rv,
                                 const ParserContext& pcontext,
                                 const Allocation&) const
{
    if ("auto"== token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = pcontext.dpi() * DEFAULT_PAGE_WIDTH;
        return true;
    }
    else
        if ("indefinite"== token) { //TODO: process indefinite value
            rv.type_ = Value::V_NUMERIC;
            rv.value_.nval_ = pcontext.dpi() * DEFAULT_PAGE_WIDTH;
            return true;
        }
    return false;
}

/*
 */
String PageHeight::name_ = "page-height";

void PageHeight::init(ParserContext* pcontext, const Allocation&, const CType&)
{
    value_ = pcontext->dpi() * DEFAULT_PAGE_HEIGHT;
}

bool PageHeight::resolveEnumToken(const String& token, ValueTypePair& rv,
                                  const ParserContext& pcontext,
                                  const Allocation&) const
{
    if ("auto"== token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = pcontext.dpi() * DEFAULT_PAGE_HEIGHT;
        return true;
    }
    else
        if ("indefinite"== token) { //TODO: process indefinite value
            rv.type_ = Value::V_NUMERIC;
            rv.value_.nval_ = pcontext.dpi() * DEFAULT_PAGE_HEIGHT;
            return true;
        }
    return false;
}

/*
 */
String Height::name_ = "height";

void Height::init(ParserContext*, const Allocation&, const CType&)
{
    value_ = 0;
}

bool Height::resolveEnumToken(const String& token, ValueTypePair& rv,
                              const ParserContext&, const Allocation&) const
{
    if ("auto"== token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = 0;
        return true;
    }
    return false;
}

/*
 */
String Width::name_ = "width";

void Width::init(ParserContext*, const Allocation&, const CType&)
{
    value_ = 100;
}

bool Width::resolveEnumToken(const String& token, ValueTypePair& rv,
                             const ParserContext&, const Allocation&) const
{
    if ("auto"== token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = 0;
        return true;
    }
    return false;
}

/*
 */
String StartIndent::name_ = "start-indent";
String EndIndent::name_ = "end-indent";

String BackgroundColor::name_ = "background-color";
String Color::name_ = "color";

/* Common Font Properties
 */
/*
 */
String FontProperty::name_ = "font";

inline void FontProperty::calcValue(Property* property, const String& spec,
                                    PropertyParser& parser)
{
    if (!property->isExplicit())
        property->calc(spec, parser);
}

void FontProperty::init(ParserContext* context, const Allocation& alloc,
                        const CType& percentBase)
{
    PropertySet* p_set = static_cast<PropertySet*>(context);
    FontSize& font_size = p_set->getProperty<FontSize>(alloc, percentBase);
    if (!font_size.isExplicit())
        font_size.init(context, alloc, percentBase);
    FontWeight& font_weight = p_set->getProperty<FontWeight>(alloc);
    if (!font_weight.isExplicit())
        font_weight.init(context, alloc, percentBase);
    FontStyle& font_style = p_set->getProperty<FontStyle>(alloc);
    if (!font_style.isExplicit())
        font_style.init(context, alloc, percentBase);
    FontFamily& font_family = p_set->getProperty<FontFamily>(alloc);
    if (!font_family.isExplicit())
        font_family.init(context, alloc, percentBase);
}

bool FontProperty::calc(const StringList& specs, ParserContext* context,
                        const Allocation& alloc)
{
    if (specs.empty())
        return false;

    PropertySet* p_set = static_cast<PropertySet*>(context);
    FontSize& font_size = p_set->getProperty<FontSize>(alloc, percentBase_);
    FontWeight& font_weight = p_set->getProperty<FontWeight>(alloc);
    FontStyle& font_style = p_set->getProperty<FontStyle>(alloc);
    FontFamily& font_family = p_set->getProperty<FontFamily>(alloc);

    if (!font_family.isExplicit())
        font_family.calculate(specs.back(), context, alloc, percentBase_);
    //specs.pop_back();
    if (specs.empty())
        return true;

    if (!font_size.isExplicit())
        font_size.calculate(specs.back(), context, alloc, percentBase_);

    return true;
}

/*
*/
String TextDecoration::name_ = "text-decoration";

void TextDecoration::init(ParserContext*, const Allocation&, const CType&)
{
    value_ = Font::DECORATION_NORMAL;
}

bool TextDecoration::calc(const CString& specified, PropertyParser&)
{
    int val = Font::DECORATION_NORMAL;

    StringList token_list;
    tokenize(specified, token_list);
    StringList::const_iterator i = token_list.begin();
    for (; i != token_list.end(); i++) {
        if ("underline" == (*i))
            val |= Font::DECORATION_UNDERLINE;
        if ("overline" == (*i))
            val |= Font::DECORATION_OVERLINE;
        if ("line-through" == (*i))
            val |= Font::DECORATION_STRIKEOUT;
    }
    value_ = Font::Decoration(val);
    return true;
}

/*
*/
String FontFamily::name_ = "font-family";

void FontFamily::init(ParserContext*, const Allocation&, const CType&)
{
    //! TODO: value_ = fontMgr()->getDefaultFont()->fontFamily()
    value_ = String::null();
}

/*
 */
String FontSize::name_ = "font-size";

void FontSize::init(ParserContext* pcontext, const Allocation&, const CType&)
{
    //! TODO: pxSize_ = fontMgr()->getDefaultFont()->pixelSize()
    pxSize_ = 10;
    ptSize_ = (PT_PER_INCH * pxSize_) / pcontext->dpi();
}

bool FontSize::calc(const COMMON_NS::String& specified,
                    PropertyParser& parser)
{
    if (NumericProperty::calc(specified, parser)) {
        pxSize_ = value_;
        ptSize_ = (PT_PER_INCH * pxSize_) / parser.parserContext().dpi();
        return true;
    }
    return false;
}

bool FontSize::resolveEnumToken(const String& token, ValueTypePair& rv,
                                const ParserContext&,
                                 const Allocation&) const
{
    if ("xx-small" == token || "x-small" == token || "small" == token ||
        "medium" == token ||
        "large" == token || "x-large" == token || "xx-large" == token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = 10;
        return true;
    }
    return false;
}

/*
 */
String FontWeight::name_ = "font-weight";

void FontWeight::init(ParserContext*, const Allocation&, const CType&)
{
    value_ = Font::WEIGHT_NORMAL;
}

bool FontWeight::resolveEnumToken(const String& token, ValueTypePair& rv,
                                  const ParserContext&,
                                 const Allocation&) const
{
    if ("normal" == token || "lighter" == token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = Font::WEIGHT_NORMAL;
        return true;
    }
    else
        if ("bold" == token || "bolder" == token) {
            rv.type_ = Value::V_NUMERIC;
            rv.value_.nval_ = Font::WEIGHT_BOLD;
            return true;
        }
    return false;
}

/*
 */
String FontStyle::name_ = "font-style";

bool FontStyle::resolveEnumToken(const String& token, ValueTypePair& rv,
                                 const ParserContext&,
                                 const Allocation&) const
{
    if ("normal" == token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = Font::STYLE_NORMAL;
        return true;
    }
    else
        if ("italic" == token) {
            rv.type_ = Value::V_NUMERIC;
            rv.value_.nval_ = Font::STYLE_ITALIC;
            return true;
        }
        else
            if ("oblique" == token) {
                rv.type_ = Value::V_NUMERIC;
                rv.value_.nval_ = Font::STYLE_OBLIQUE;
                return true;
            }
            else
                if ("backslant" == token) {
                    rv.type_ = Value::V_NUMERIC;
                    rv.value_.nval_ = Font::STYLE_BACKSLANT;
                    return true;
                }
    return false;
}

/*
 */
String TextAlign::name_ = "text-align";

bool TextAlign::resolveEnumToken(const String& token, ValueTypePair& rv,
                                 const ParserContext&,
                                 const Allocation&) const
{
    if ("left" == token || "start" == token ||
        "inside" == token || "justify" == token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = ALIGN_LEFT;
        return true;
    }
    else
        if ("right" == token || "end" == token || "outside" == token) {
            rv.type_ = Value::V_NUMERIC;
            rv.value_.nval_ = ALIGN_RIGHT;
            return true;
        }
        else
            if ("center" == token) {
                rv.type_ = Value::V_NUMERIC;
                rv.value_.nval_ = ALIGN_CENTER;
                return true;
            }
    return false;
}

/*
 */
String LinefeedTreatment::name_ = "linefeed-treatment";

bool LinefeedTreatment::resolveEnumToken(const String& token,
                                         ValueTypePair& rv,
                                         const ParserContext&,
                                         const Allocation&) const
{
    if ("ignore" == token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = IGNORE_LINEFEED;
        return true;
    }
    else
        if ("preserve" == token) {
            rv.type_ = Value::V_NUMERIC;
            rv.value_.nval_ = PRESERVE_LINEFEED;
            return true;
        }
    return false;
}

/*
 */
String WhitespaceTreatment::name_ = "white-space-treatment";

bool WhitespaceTreatment::resolveEnumToken(const String& token,
                                           ValueTypePair& rv,
                                           const ParserContext&,
                                           const Allocation&) const
{
    if ("ignore" == token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = IGNORE_WHITESPACE;
        return true;
    }
    else
        if ("preserve" == token) {
            rv.type_ = Value::V_NUMERIC;
            rv.value_.nval_ = PRESERVE_WHITESPACE;
            return true;
        }
        else
            if ("ignore-if-before-linefeed" == token) {
                rv.type_ = Value::V_NUMERIC;
                rv.value_.nval_ = IGNORE_BEFORE_LINEFEED;
                return true;
            }
            else
                if ("ignore-if-after-linefeed" == token) {
                    rv.type_ = Value::V_NUMERIC;
                    rv.value_.nval_ = IGNORE_AFTER_LINEFEED;
                    return true;
                }
    return false;
}

/*
*/
String WhitespaceCollapse::name_ = "white-space-collapse";

/*
 */
String ProvisionalDistance::name_ = "provisional-distance-between-starts";

void ProvisionalDistance::init(ParserContext* pcontext, const Allocation&,
                               const CType&)
{
    value_ = pcontext->dpi() * DEFAULT_PROVISIONAL_DISTANCE;
}

/*
 */
String ProvisionalSeparation::name_ = "provisional-label-separation";

void ProvisionalSeparation::init(ParserContext* pcontext, const Allocation&,
                                 const CType&)
{
    value_ = pcontext->dpi() * DEFAULT_PROVISIONAL_SEPARATION;
}

bool ProvisionalSeparation::calc(const COMMON_NS::String& specified,
                                 PropertyParser& parser)
{
    if (!NumericProperty::calc(specified, parser) || 0 >= value_)
        value_ = parser.parserContext().dpi() * DEFAULT_PROVISIONAL_SEPARATION;
    return true;
}

/*
 */
String ColumnWidth::name_ = "column-width";

ColumnWidth::ColumnWidth()
    : proportion_(1)
{
    value_ = 0;
}

void ColumnWidth::init(ParserContext* parserContext, const Allocation&,
                       const CType&)
{
    Allocation alloc;
    PropertyParser parser(*parserContext, alloc, 0);
    if (!calc("proportional-column-width(1)", parser)) {
        value_ = 0;
        proportion_ = 1;
    }
}

bool ColumnWidth::calc(const COMMON_NS::String& specified,
                       PropertyParser& parser)
{
    if (specified.isEmpty()) {
        value_ = 0;
        proportion_ = 1;

        return true;
    }
    if (specified.contains("proportional-column-width(")) {
        int from = specified.find('(');
        int to = specified.find(')', from);
        if (to > from + 1) {
            String num = specified.mid(from + 1, to - from - 1);
            proportion_ = num.toDouble();
        }
        if (0 == proportion_)
            proportion_ = 1;
    }
    else
        proportion_ = 0;
    return NumericProperty::calc(specified, parser);
}

/*
 */
String ColumnRepeats::name_ = "number-columns-repeated";

/*
 */
String ColumnNumber::name_ = "column-number";

/*
 */
String StartsRow::name_ = "starts-row";

/*
 */
String EndsRow::name_ = "ends-row";

/*
 */
String ColumnsSpanned::name_ = "number-columns-spanned";

/*
 */
String RowsSpanned::name_ = "number-rows-spanned";

/*
 */
String SrcUri::name_ = "src";

void SrcUri::init(ParserContext*, const Allocation&, const CType&)
{
    value_ = String::null();
}

/*
 */
String ContentWidth::name_ = "content-width";

String ContentHeight::name_ = "content-height";

void ContentWidth::init(ParserContext*, const Allocation&,
                        const CType& percentBase)
{
    value_ = percentBase;
    isAuto_ = false;
}

bool ContentWidth::resolveEnumToken(const String& token, ValueTypePair& rv,
                                    const ParserContext&,
                                    const Allocation& alloc) const
{
    if ("auto" == token) {
        isAuto_ = true;
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = (percentBase_ < alloc.space_.extent_.w_) 
            ? percentBase_ : alloc.space_.extent_.w_;
        return true;
    }
    else {
        isAuto_ = false;
        if ("scale-to-fit" == token) {
            rv.type_ = Value::V_NUMERIC;
            rv.value_.nval_ = alloc.space_.extent_.w_;
            return true;
        }
    }
    return false;
}

/*
 */
String BaselineShift::name_ = "baseline-shift";

void BaselineShift::init(ParserContext*, const Allocation&, const CType&)
{
    value_ = 0;
}

bool BaselineShift::resolveEnumToken(const String& token,
                                     ValueTypePair& rv,
                                     const ParserContext&,
                                     const Allocation&) const
{
    if ("sub" == token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = -(percentBase_ * 0.25);
        return true;
    }
    else
        if ("super" == token) {
            rv.type_ = Value::V_NUMERIC;
            rv.value_.nval_ = percentBase_ * 0.30;
            return true;
        }
    return false;
}

}

