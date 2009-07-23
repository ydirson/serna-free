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

#include "xs/datatypes/Facets.h"
#include "xs/datatypes/FacetsContainer.h"
#include "xs/datatypes/DataTypes.h"
#include "xs/Origin.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

XS_NAMESPACE_BEGIN

bool PatternContainer::check(Schema* schema,
                             const String& what,
                             const Node* o,
                             const Piece* piece) const
{
    if (begin() == end())
        return true;
    bool ok = false;
    for (const_iterator i = begin(); i != end(); ++i) {
        if ((*i)->match(what)) {
            ok = true;
            break;
        }
    }
    if (!ok && 0 != schema)
        schema->mstream() << XsMessages::patternFacetConstrain
                          << Message::L_ERROR
                          << DVXS_ORIGIN(o, piece);
    return ok;
}

/*****************************************************************************
 * XsFacet - base class                                                      *
 *****************************************************************************/

XsFacet::XsFacet(XsFacet::FacetType type, bool fixed, const XsId& xsid)
  : Piece(Origin(0), xsid), type_(type), fixed_(fixed)
{
}

XsFacet::~XsFacet()
{
}

void XsFacet::fix()
{
    fixed_ = true;
}

bool XsFacet::fixed()
{
    return fixed_;
}

void XsFacet::setAppinfo(RefCntPtr<XS_NAMESPACE::Appinfo>& annotation)
{
    annotation_ = annotation;
}

void XsFacet::dump(int) const
{
    //TODO:
}

PRTTI_BASE_STUB(XsFacet, LengthFacet)
PRTTI_BASE_STUB(XsFacet, MinLengthFacet)
PRTTI_BASE_STUB(XsFacet, MaxLengthFacet)
PRTTI_BASE_STUB(XsFacet, PatternFacet)
PRTTI_BASE_STUB(XsFacet, EnumerationFacet)
PRTTI_BASE_STUB(XsFacet, WhiteSpaceFacet)
PRTTI_BASE_STUB(XsFacet, MaxInclusiveFacet)
PRTTI_BASE_STUB(XsFacet, MinInclusiveFacet)
PRTTI_BASE_STUB(XsFacet, MaxExclusiveFacet)
PRTTI_BASE_STUB(XsFacet, MinExclusiveFacet)
PRTTI_BASE_STUB(XsFacet, TotalDigitsFacet)
PRTTI_BASE_STUB(XsFacet, FractionDigitsFacet)
PRTTI_BASE_STUB(XsFacet, EncodingFacet)
PRTTI_BASE_STUB(XsFacet, DurationFacet)
PRTTI_BASE_STUB(XsFacet, PeriodFacet)

/*****************************************************************************
 * DurationFacet                                                             *
 *****************************************************************************/

DurationFacet::DurationFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::DURATION, fixed, xsid)
{
}

DurationFacet::DurationFacet(String& value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::DURATION, fixed, xsid)
{
    DurationType::toDateTime(value_, value);
}

void DurationFacet::setValue(String& value)
{
    DurationType::toDateTime(value_, value);
}

SchemaDateTime& DurationFacet::value()
{
    return value_;
}

PRTTI_IMPL(DurationFacet)

/*****************************************************************************
 * EncodingFacet                                                             *
 *****************************************************************************/

EncodingFacet::EncodingFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::ENCODING, fixed, xsid)
{
}

EncodingFacet::EncodingFacet(EncodingType value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::ENCODING, fixed, xsid),
      value_(value)
{
}

void EncodingFacet::setValue(EncodingType value)
{
    value_ = value;
}

EncodingFacet::EncodingType EncodingFacet::value()
{
    return value_;
}

PRTTI_IMPL(EncodingFacet)

/*****************************************************************************
 * EnumerationFacet                                                          *
 *****************************************************************************/

EnumerationFacet::EnumerationFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::ENUMERATION, fixed, xsid),
      tag_(String::null())
{
}

EnumerationFacet::EnumerationFacet(String& value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::ENUMERATION, fixed, xsid),
      value_(value),
      tag_(String::null())
{
}

void EnumerationFacet::setValue(String& value)
{
    value_ = value;
}

String& EnumerationFacet::value()
{
    return value_;
}

void EnumerationFacet::setTag(String& tag)
{
    tag_ = tag;
}

String& EnumerationFacet::tag()
{
    return tag_;
}

void EnumerationFacet::setTitle(String& title)
{
    title_ = title;
}

String& EnumerationFacet::title()
{
    return title_;
}

PRTTI_IMPL(EnumerationFacet)

/*****************************************************************************
 * FractionDigitsFacet                                                       *
 *****************************************************************************/


FractionDigitsFacet::FractionDigitsFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::SCALE, fixed, xsid)
{
}

FractionDigitsFacet::FractionDigitsFacet(uint value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::SCALE, fixed, xsid),
      value_(value)
{
}

void FractionDigitsFacet::setValue(uint value)
{
    value_ = value;
}

uint& FractionDigitsFacet::value()
{
    return value_;
}

PRTTI_IMPL(FractionDigitsFacet)

/*****************************************************************************
 * LengthFacet                                                               *
 *****************************************************************************/


LengthFacet::LengthFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::LENGTH, fixed, xsid)
{
}

LengthFacet::LengthFacet(uint value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::LENGTH, fixed, xsid),
      value_(value)
{
}

void LengthFacet::setValue(uint value)
{
    value_ = value;
}

uint& LengthFacet::value()
{
    return value_;
}

PRTTI_IMPL(LengthFacet)

/*****************************************************************************
 * MaxLengthFacet                                                            *
 *****************************************************************************/

MaxLengthFacet::MaxLengthFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::MAXLENGTH, fixed, xsid)
{
}

MaxLengthFacet::MaxLengthFacet(uint value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::MAXLENGTH, fixed, xsid),
      value_(value)
{
}

void MaxLengthFacet::setValue(uint value)
{
    value_ = value;
}

uint& MaxLengthFacet::value()
{
    return value_;
}

PRTTI_IMPL(MaxLengthFacet)

/*****************************************************************************
 * MinLengthFacet                                                            *
 *****************************************************************************/

MinLengthFacet::MinLengthFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::MINLENGTH, fixed, xsid)
{
}

MinLengthFacet::MinLengthFacet(uint value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::MINLENGTH, fixed, xsid),
      value_(value)
{
}

void MinLengthFacet::setValue(uint value)
{
    value_ = value;
}

uint& MinLengthFacet::value()
{
    return value_;
}

PRTTI_IMPL(MinLengthFacet)

/*****************************************************************************
 * MaxExclusiveFacet                                                         *
 *****************************************************************************/

MaxExclusiveFacet::MaxExclusiveFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::MAXEXCLUSIVE, fixed, xsid)
{
}

MaxExclusiveFacet::MaxExclusiveFacet(String& value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::MAXEXCLUSIVE, fixed, xsid),
      value_(value)
{
}

void MaxExclusiveFacet::setValue(String& value)
{
    value_ = value;
}

String& MaxExclusiveFacet::value()
{
    return value_;
}

PRTTI_IMPL(MaxExclusiveFacet)

/*****************************************************************************
 * MaxInclusiveFacet                                                         *
 *****************************************************************************/

MaxInclusiveFacet::MaxInclusiveFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::MAXINCLUSIVE, fixed, xsid)
{
}

MaxInclusiveFacet::MaxInclusiveFacet(String& value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::MAXINCLUSIVE, fixed, xsid),
      value_(value)
{
}

void MaxInclusiveFacet::setValue(String& value)
{
    value_ = value;
}

String& MaxInclusiveFacet::value()
{
    return value_;
}

PRTTI_IMPL(MaxInclusiveFacet)

/*****************************************************************************
 * MinExclusiveFacet                                                         *
 *****************************************************************************/

MinExclusiveFacet::MinExclusiveFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::MINEXCLUSIVE, fixed, xsid)
{
}

MinExclusiveFacet::MinExclusiveFacet(String& value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::MINEXCLUSIVE, fixed, xsid),
      value_(value)
{
}

void MinExclusiveFacet::setValue(String& value)
{
    value_ = value;
}

String& MinExclusiveFacet::value()
{
    return value_;
}

PRTTI_IMPL(MinExclusiveFacet)

/*****************************************************************************
 * MinInclusiveFacet                                                         *
 *****************************************************************************/

MinInclusiveFacet::MinInclusiveFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::MININCLUSIVE, fixed, xsid)
{
}

MinInclusiveFacet::MinInclusiveFacet(String& value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::MININCLUSIVE, fixed, xsid),
      value_(value)
{
}

void MinInclusiveFacet::setValue(String& value)
{
    value_ = value;
}

String& MinInclusiveFacet::value()
{
    return value_;
}

PRTTI_IMPL(MinInclusiveFacet)

/*****************************************************************************
 * PatternFacet                                                              *
 *****************************************************************************/

PatternFacet::PatternFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::PATTERN, fixed, xsid)
{
}

PatternFacet::PatternFacet(String& value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::PATTERN, fixed, xsid),
      value_(value.qstring())
{
}

void PatternFacet::setValue(String& value)
{
    value_.setPattern(value.qstring());
}

bool PatternFacet::match(const String& what)
{
    return value_.exactMatch(what);
}

PRTTI_IMPL(PatternFacet)

/*****************************************************************************
 * PeriodFacet                                                               *
 *****************************************************************************/

PeriodFacet::PeriodFacet(bool fixed, const XsId& xsid)
        : XsFacet(XsFacet::PERIOD, fixed, xsid)
{}

PeriodFacet::PeriodFacet(String& value, bool fixed, const XsId& xsid)
        : XsFacet(XsFacet::PERIOD, fixed, xsid)
{
    DurationType::toDateTime(value_, value);
}

void PeriodFacet::setValue(String& value)
{
    DurationType::toDateTime(value_, value);
}

SchemaDateTime& PeriodFacet::value()
{
    return value_;
}

PRTTI_IMPL(PeriodFacet)

/*****************************************************************************
 * TotalDigitsFacet                                                          *
 *****************************************************************************/

TotalDigitsFacet::TotalDigitsFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::PRECISION, fixed, xsid)
{
}

TotalDigitsFacet::TotalDigitsFacet(uint value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::PRECISION, fixed, xsid),
      value_(value)
{
}

void TotalDigitsFacet::setValue(uint value)
{
    value_ = value;
}

uint& TotalDigitsFacet::value()
{
    return value_;
}

PRTTI_IMPL(TotalDigitsFacet)

/*****************************************************************************
 * WhiteSpaceFacet                                                           *
 *****************************************************************************/

WhiteSpaceFacet::WhiteSpaceFacet(bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::WHITESPACE, fixed, xsid)
{
}

WhiteSpaceFacet::WhiteSpaceFacet(StripType value, bool fixed, const XsId& xsid)
    : XsFacet(XsFacet::WHITESPACE, fixed, xsid),
      value_(value)
{
}

void WhiteSpaceFacet::setValue(StripType value)
{
    value_ = value;
}

WhiteSpaceFacet::StripType WhiteSpaceFacet::value()
{
    return value_;
}

PRTTI_IMPL(WhiteSpaceFacet)

XS_NAMESPACE_END
