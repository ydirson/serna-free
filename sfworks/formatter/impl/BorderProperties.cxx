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

#include "formatter/impl/debug.h"
#include "formatter/impl/BorderProperties.h"
#include "formatter/impl/FoImpl.h"

USING_COMMON_NS;

namespace Formatter {

const CType THIN_BORDER     = 1; //pt
const CType MEDIUM_BORDER   = 2; //pt
const CType THICK_BORDER    = 4; //pt

const CType MM_PER_INCH = 25.4;
const CType PT_PER_INCH = 72;
const CType DEFAULT_PAGE_WIDTH = 210 / MM_PER_INCH;
const CType DEFAULT_PAGE_HEIGHT = 297 / MM_PER_INCH;

/*
 */
String BorderWidth::name_       = "border-width";
String BorderColor::name_       = "border-color";

String BorderTopWidth::name_    = "border-top-width";
String BorderBottomWidth::name_ = "border-bottom-width";
String BorderLeftWidth::name_   = "border-left-width";
String BorderRightWidth::name_  = "border-right-width";

String BorderTopColor::name_    = "border-top-color";
String BorderBottomColor::name_ = "border-bottom-color";
String BorderLeftColor::name_   = "border-left-color";
String BorderRightColor::name_  = "border-right-color";

/*
 */
inline void SideShorthand::calcValue(Property* property, const String& spec,
                                     PropertyParser& parser)
{
    if (!property->isExplicit())
        property->calc(spec, parser);
}

inline void SideShorthand::calcTwins(Property* first, Property* second,
                                     const String& spec,
                                     PropertyParser& parser)
{
    if (!first->isExplicit()) {
        first->calc(spec, parser);
        copyValue(second, first);
    }
    else
        calcValue(second, spec, parser);
}

void SideShorthand::init(ParserContext* context, const Allocation& alloc,
                         const CType& percentBase)
{
    OValue<Property*> side(getSideProperties(context, alloc));
    if (!side.top_->isExplicit())
        side.top_->init(context, alloc, percentBase);
    if (!side.bottom_->isExplicit())
        side.bottom_->init(context, alloc, percentBase);
    if (!side.left_->isExplicit())
        side.left_->init(context, alloc, percentBase);
    if (!side.right_->isExplicit())
        side.right_->init(context, alloc, percentBase);
}

bool SideShorthand::calc(const StringList& specs, ParserContext* context,
                         const Allocation& alloc)
{
    if (specs.empty())
        return false;
    OValue<Property*> side(getSideProperties(context, alloc));

    PropertyParser parser(*context, alloc, percentBase_);
    StringList::const_iterator i = specs.begin();
    switch (specs.size()) {
        case 1 :
            if (!side.top_->isExplicit()) {
                side.top_->calc((*i), parser);
                copyValue(side.bottom_, side.top_);
                copyValue(side.left_, side.top_);
                copyValue(side.right_, side.top_);
            }
            else
                if (!side.bottom_->isExplicit()) {
                    side.bottom_->calc((*i), parser);
                    copyValue(side.left_, side.bottom_);
                    copyValue(side.right_, side.bottom_);
                }
                else
                    calcTwins(side.left_, side.right_, (*i), parser);
            break;
        case 2 :
            calcTwins(side.top_, side.bottom_, (*i), parser);
            i++;
            calcTwins(side.left_, side.right_, (*i), parser);
            break;
        case 3 :
            calcValue(side.top_, (*i), parser);
            i++;
            calcTwins(side.left_, side.right_, (*i), parser);
            i++;
            calcValue(side.bottom_, (*i), parser);
            break;
        case 4 :
            calcValue(side.top_, (*i), parser);
            i++;
            calcValue(side.right_, (*i), parser);
            i++;
            calcValue(side.bottom_, (*i), parser);
            i++;
            calcValue(side.left_, (*i), parser);
            break;
        default:
            break;
    }
    return true;
}

/*
 */
OValue<Property*> BorderWidth::getSideProperties(ParserContext* pcontext,
                                                 const Allocation& alloc)
{
    PropertySet* property_set = static_cast<PropertySet*>(pcontext);
    return OValue<Property*>(
        &property_set->getProperty<BorderTopWidth>(alloc),
        &property_set->getProperty<BorderBottomWidth>(alloc),
        &property_set->getProperty<BorderLeftWidth>(alloc),
        &property_set->getProperty<BorderRightWidth>(alloc));
}

void BorderWidth::copyValue(Property* to, Property* from)
{
    if (!to->isExplicit())
        static_cast<BorderSideWidth*>(to)->value_ =
            static_cast<BorderSideWidth*>(from)->value_;
}

ORect getBorder(PropertySet& p_set, const Allocation& alloc)
{
    p_set.getProperty<BorderWidth>(alloc);
    
    BorderTopWidth& border_top       =
        p_set.getProperty<BorderTopWidth>(alloc);
    BorderBottomWidth& border_bottom =
        p_set.getProperty<BorderBottomWidth>(alloc);
    BorderLeftWidth& border_left     =
        p_set.getProperty<BorderLeftWidth>(alloc);
    BorderRightWidth& border_right   =
        p_set.getProperty<BorderRightWidth>(alloc);

    return ORect(border_top.value(), border_bottom.value(),
                 border_left.value(), border_right.value());
}

/*
 */
void BorderSideWidth::init(ParserContext*, const Allocation&,
                           const CType&)
{
    value_ = 0;//pcontext->dpi() * MEDIUM_BORDER / PT_PER_INCH;;
}

bool BorderSideWidth::resolveEnumToken(const String& token, ValueTypePair& rv,
                                       const ParserContext& pcontext,
                                       const Allocation&) const
{
    if ("thin" == token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = pcontext.dpi() * THIN_BORDER / PT_PER_INCH;
        return true;
    }
    else
        if ("medium" == token) {
            rv.type_ = Value::V_NUMERIC;
            rv.value_.nval_ = pcontext.dpi() * MEDIUM_BORDER / PT_PER_INCH;
            return true;
        }
        else
            if ("thick" == token) {
                rv.type_ = Value::V_NUMERIC;
                rv.value_.nval_ = pcontext.dpi() * THICK_BORDER / PT_PER_INCH;
                return true;
            }
    return false;
}

/*
 */
OValue<Property*> BorderColor::getSideProperties(ParserContext* pcontext,
                                                 const Allocation& alloc)
{
    PropertySet* property_set = static_cast<PropertySet*>(pcontext);
    return OValue<Property*>(
        &property_set->getProperty<BorderTopColor>(alloc),
        &property_set->getProperty<BorderBottomColor>(alloc),
        &property_set->getProperty<BorderLeftColor>(alloc),
        &property_set->getProperty<BorderRightColor>(alloc));
}

void BorderColor::copyValue(Property* to, Property* from)
{
    if (!to->isExplicit())
        static_cast<ColorProperty*>(to)->value_ =
            static_cast<ColorProperty*>(from)->value_;
}

OValue<Rgb> getBorderColor(PropertySet& p_set, const Allocation& alloc)
{
    p_set.getProperty<BorderColor>(alloc);
    
    BorderTopColor& border_top       =
        p_set.getProperty<BorderTopColor>(alloc);
    BorderBottomColor& border_bottom =
        p_set.getProperty<BorderBottomColor>(alloc);
    BorderLeftColor& border_left     =
        p_set.getProperty<BorderLeftColor>(alloc);
    BorderRightColor& border_right   =
        p_set.getProperty<BorderRightColor>(alloc);
    return OValue<Rgb>(border_top.value(), border_bottom.value(),
                       border_left.value(), border_right.value());
}

/*
 */
OValue<Property*>
Margin::getSideProperties(ParserContext* pcontext, const Allocation& alloc)
{
    PropertySet* property_set = static_cast<PropertySet*>(pcontext);
    return OValue<Property*>(
        &property_set->getProperty<MarginTop>(alloc),
        &property_set->getProperty<MarginBottom>(alloc),
        &property_set->getProperty<MarginLeft>(alloc),
        &property_set->getProperty<MarginRight>(alloc));
}

void Margin::copyValue(Property* to, Property* from)
{
    if (!to->isExplicit())
        static_cast<NumericProperty*>(to)->value_ =
            static_cast<NumericProperty*>(from)->value_;
}

ORect getMargin(PropertySet& p_set, const Allocation& alloc)
{
    p_set.getProperty<Margin>(alloc);

    MarginTop& margin_top       = p_set.getProperty<MarginTop>(alloc);
    MarginBottom& margin_bottom = p_set.getProperty<MarginBottom>(alloc);
    MarginLeft& margin_left     = p_set.getProperty<MarginLeft>(alloc);
    MarginRight& margin_right   = p_set.getProperty<MarginRight>(alloc);

    ORect rect(margin_top.value(), margin_bottom.value(),
               margin_left.value(), margin_right.value());

    DBG(XSL.PROP) << "Margin" << std::endl;
    DBG_IF(XSL.PROP) rect.dump();

    return rect;
}

String Margin::name_        = "margin";
String MarginTop::name_     = "margin-top";
String MarginBottom::name_  = "margin-bottom";
String MarginLeft::name_    = "margin-left";
String MarginRight::name_   = "margin-right";

bool MarginWidth::resolveEnumToken(const String& token, ValueTypePair& rv,
                                   const ParserContext&,
                                   const Allocation&) const
{
    if ("auto"== token) {
        rv.type_ = Value::V_NUMERIC;
        rv.value_.nval_ = 0;
        return true;
    }
    return false;
}

/* Common Padding Properties
 */
OValue<Property*>
Padding::getSideProperties(ParserContext* pcontext, const Allocation& alloc)
{
    PropertySet* property_set = static_cast<PropertySet*>(pcontext);
    return OValue<Property*>(
        &property_set->getProperty<PaddingTop>(alloc),
        &property_set->getProperty<PaddingBottom>(alloc),
        &property_set->getProperty<PaddingLeft>(alloc),
        &property_set->getProperty<PaddingRight>(alloc));
}

void Padding::copyValue(Property* to, Property* from)
{
    if (!to->isExplicit())
        static_cast<NumericProperty*>(to)->value_ =
            static_cast<NumericProperty*>(from)->value_;
}

ORect getPadding(PropertySet& p_set, const Allocation& alloc)
{
    p_set.getProperty<Padding>(alloc);

    PaddingTop& padding_top       = p_set.getProperty<PaddingTop>(alloc);
    PaddingBottom& padding_bottom = p_set.getProperty<PaddingBottom>(alloc);
    PaddingLeft& padding_left     = p_set.getProperty<PaddingLeft>(alloc);
    PaddingRight& padding_right   = p_set.getProperty<PaddingRight>(alloc);

    ORect rect(padding_top.value(), padding_bottom.value(),
               padding_left.value(), padding_right.value());

    DBG(XSL.PROP) << "Padding" << std::endl;
    DBG_IF(XSL.PROP) rect.dump();

    return rect;
}

String Padding::name_       = "padding";
String PaddingTop::name_    = "padding-top";
String PaddingBottom::name_ = "padding-bottom";
String PaddingLeft::name_   = "padding-left";
String PaddingRight::name_  = "padding-right";

}

