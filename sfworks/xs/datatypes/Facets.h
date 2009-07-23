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
 *  $RCSfile: Facets.h,v $
 *
 ***********************************************************************/
#ifndef FACETS_H
#define FACETS_H

#include "xs/xs_defs.h"
#include "xs/Piece.h"
#include "common/RefCntPtr.h"
#include "common/prtti.h"
#include "common/String.h"
#include "xs/datatypes/SchemaDateTime.h"
#include <qregexp.h>

XS_NAMESPACE_BEGIN

/*****************************************************************************
 * XsFacet - base class                                                      *
 *****************************************************************************/

class LengthFacet;
class MinLengthFacet;
class MaxLengthFacet;
class PatternFacet;
class EnumerationFacet;
class WhiteSpaceFacet;
class MaxInclusiveFacet;
class MinInclusiveFacet;
class MaxExclusiveFacet;
class MinExclusiveFacet;
class TotalDigitsFacet;
class FractionDigitsFacet;
class EncodingFacet;
class DurationFacet;
class PeriodFacet;


class Appinfo;

class XsFacet : public Piece {
public:
    enum FacetType {
        LENGTH,
        MINLENGTH,
        MAXLENGTH,
        PATTERN,
        ENUMERATION,
        WHITESPACE,
        MAXINCLUSIVE,
        MAXEXCLUSIVE,
        MINEXCLUSIVE,
        MININCLUSIVE,
        PRECISION,
        SCALE,
        ENCODING,
        DURATION,
        PERIOD,
        UNKNOWN
    };

    XsFacet(FacetType type, bool fixed = false, const XsId& xsid = XsId());

    virtual ~XsFacet();

    FacetType type() const;

    void fix();
    bool fixed();

    void setAppinfo(COMMON_NS::RefCntPtr<Appinfo>& annotation);

    virtual void dump(int indent) const;

    PRTTI_DECL(LengthFacet);
    PRTTI_DECL(MinLengthFacet);
    PRTTI_DECL(MaxLengthFacet);
    PRTTI_DECL(PatternFacet);
    PRTTI_DECL(EnumerationFacet);
    PRTTI_DECL(WhiteSpaceFacet);
    PRTTI_DECL(MaxInclusiveFacet);
    PRTTI_DECL(MinInclusiveFacet);
    PRTTI_DECL(MaxExclusiveFacet);
    PRTTI_DECL(MinExclusiveFacet);

    PRTTI_DECL(TotalDigitsFacet);
    PRTTI_DECL(FractionDigitsFacet);
    PRTTI_DECL(EncodingFacet);
    PRTTI_DECL(DurationFacet);
    PRTTI_DECL(PeriodFacet);

    XS_OALLOC(XsFacet);

private:
    FacetType type_;
    XsId      id_;
    bool      fixed_;
    COMMON_NS::RefCntPtr<Appinfo> annotation_;
};

inline XsFacet::FacetType XsFacet::type() const
{
    return type_;
}


/*****************************************************************************
 * DurationFacet                                                             *
 *****************************************************************************/


class DurationFacet : public XsFacet {
public:
    DurationFacet(bool fixed = false, const XsId& xsid = XsId());
    DurationFacet(COMMON_NS::String& value, bool fixed = false, const XsId& xsid = XsId());

    void setValue(COMMON_NS::String& value);
    SchemaDateTime& value();

    PRTTI_DECL(DurationFacet);
    XS_OALLOC(DurationFacet);

private:
    SchemaDateTime  value_;
};


/*****************************************************************************
 * EncodingFacet                                                             *
 *****************************************************************************/

class EncodingFacet : public XsFacet {
public:
    enum EncodingType {
        HEX,
        BASE64
    };

    EncodingFacet(bool fixed = false, const XsId& xsid = XsId());
    EncodingFacet(EncodingType value, bool fixed = false,
                  const XsId& xsid = XsId());

    void setValue(EncodingType value);
    EncodingType value();

    PRTTI_DECL(EncodingFacet);
    XS_OALLOC(EncodingFacet);

private:
    EncodingType value_;
};

/*****************************************************************************
 * EnumerationFacet                                                          *
 *****************************************************************************/

class EnumerationFacet : public XsFacet {
public:
    EnumerationFacet(bool fixed = false, const XsId& xsid = XsId());
    EnumerationFacet(COMMON_NS::String& value, bool fixed = false, const XsId& xsid = XsId());

    void setValue(COMMON_NS::String& value);
    void setTag  (COMMON_NS::String& tag);
    void setTitle  (COMMON_NS::String& tag);
    COMMON_NS::String& value();
    COMMON_NS::String& tag();
    COMMON_NS::String& title();

    PRTTI_DECL(EnumerationFacet);
    XS_OALLOC(EnumerationFacet);

private:
    COMMON_NS::String  value_;
    COMMON_NS::String  tag_;
    COMMON_NS::String  title_;
};

/*****************************************************************************
 * FractionDigitsFacet                                                       *
 *****************************************************************************/

class FractionDigitsFacet : public XsFacet {
public:
    FractionDigitsFacet(bool fixed = false, const XsId& xsid = XsId());
    FractionDigitsFacet(uint value, bool fixed = false,
                        const XsId& xsid = XsId());
    void setValue(uint value);
    uint& value();

    PRTTI_DECL(FractionDigitsFacet);
    XS_OALLOC(FractDigitsFacet);

private:
    uint    value_;
};

/*****************************************************************************
 * LengthFacet                                                               *
 *****************************************************************************/

class LengthFacet : public XsFacet {
public:
    LengthFacet(bool fixed = false, const XsId& xsid = XsId());
    LengthFacet(uint value, bool fixed = false, const XsId& xsid = XsId());

    void setValue(uint value);
    uint& value();

    PRTTI_DECL(LengthFacet);
    XS_OALLOC(LengthFacet);

private:
    uint    value_;
};

/*****************************************************************************
 * MaxLengthFacet                                                            *
 *****************************************************************************/

class MaxLengthFacet : public XsFacet {
public:
    MaxLengthFacet(bool fixed = false, const XsId& xsid = XsId());
    MaxLengthFacet(uint value, bool fixed = false, const XsId& xsid = XsId());

    void setValue(uint value);
    uint& value();

    PRTTI_DECL(MaxLengthFacet);
    XS_OALLOC(MaxLengthFacet);

private:
    uint    value_;
};

/*****************************************************************************
 * MinLengthFacet                                                            *
 *****************************************************************************/

class MinLengthFacet : public XsFacet {
public:
    MinLengthFacet(bool fixed = false, const XsId& xsid = XsId());
    MinLengthFacet(uint value, bool fixed = false, const XsId& xsid = XsId());

    void setValue(uint value);
    uint& value();

    PRTTI_DECL(MinLengthFacet);
    XS_OALLOC(MinLengthFacet);

private:
    uint    value_;
};

/*****************************************************************************
 * MaxExclusiveFacet                                                         *
 *****************************************************************************/

class MaxExclusiveFacet : public XsFacet {
public:
    MaxExclusiveFacet(bool fixed = false, const XsId& xsid = XsId());
    MaxExclusiveFacet(COMMON_NS::String& value, bool fixed = false,
                      const XsId& xsid = XsId());

    void setValue(COMMON_NS::String& value);
    COMMON_NS::String& value();

    PRTTI_DECL(MaxExclusiveFacet);
    XS_OALLOC(MaxExclFacet);

private:
    COMMON_NS::String  value_;
};

/*****************************************************************************
 * MaxInclusiveFacet                                                         *
 *****************************************************************************/

class MaxInclusiveFacet : public XsFacet {
public:
    MaxInclusiveFacet(bool fixed = false, const XsId& xsid = XsId());
    MaxInclusiveFacet(COMMON_NS::String& value, bool fixed = false,
                      const XsId& xsid = XsId());

    void setValue(COMMON_NS::String& value);
    COMMON_NS::String& value();

    PRTTI_DECL(MaxInclusiveFacet);
    XS_OALLOC(MaxInclFacet);

private:
    COMMON_NS::String  value_;
};

/*****************************************************************************
 * MinExclusiveFacet                                                         *
 *****************************************************************************/

class MinExclusiveFacet : public XsFacet {
public:
    MinExclusiveFacet(bool fixed = false, const XsId& xsid = XsId());
    MinExclusiveFacet(COMMON_NS::String& value, bool fixed = false,
                      const XsId& xsid = XsId());
    void setValue(COMMON_NS::String& value);
    COMMON_NS::String& value();

    PRTTI_DECL(MinExclusiveFacet);
    XS_OALLOC(MinExclFacet);

private:
    COMMON_NS::String  value_;
};

/*****************************************************************************
 * MinInclusiveFacet                                                         *
 *****************************************************************************/

class MinInclusiveFacet : public XsFacet {
public:
    MinInclusiveFacet(bool fixed = false, const XsId& xsid = XsId());
    MinInclusiveFacet(COMMON_NS::String& value, bool fixed = false,
                      const XsId& xsid = XsId());
    void setValue(COMMON_NS::String& value);
    COMMON_NS::String& value();

    PRTTI_DECL(MinInclusiveFacet);
    XS_OALLOC(MinInclFacet);

private:
    COMMON_NS::String  value_;
};

/*****************************************************************************
 * PatternFacet                                                              *
 *****************************************************************************/

class PatternFacet : public XsFacet {
public:
    PatternFacet(bool fixed = false, const XsId& xsid = XsId());
    PatternFacet(COMMON_NS::String& value, 
                 bool fixed = false, 
                 const XsId& xsid = XsId());

    void      setValue(COMMON_NS::String& value);
    bool      match(const Common::String& what);

    PRTTI_DECL(PatternFacet);
    XS_OALLOC(PatternFacet);

private:
    QRegExp  value_;
};

/*****************************************************************************
 * PeriodFacet                                                               *
 *****************************************************************************/

class PeriodFacet : public XsFacet {
public:
    PeriodFacet(bool fixed = false, const XsId& xsid = XsId());
    PeriodFacet(COMMON_NS::String& value, bool fixed = false,
                const XsId& xsid = XsId());

    void setValue(COMMON_NS::String& value);
    SchemaDateTime& value();

    PRTTI_DECL(PeriodFacet);
    XS_OALLOC(PeriodFacet);

private:
    SchemaDateTime  value_;
};

/*****************************************************************************
 * TotalDigitsFacet                                                          *
 *****************************************************************************/

class TotalDigitsFacet : public XsFacet {
public:
    TotalDigitsFacet(bool fixed = false, const XsId& xsid = XsId());
    TotalDigitsFacet(uint value, bool fixed = false, const XsId& xsid = XsId());

    void setValue(uint value);
    uint& value();

    PRTTI_DECL(TotalDigitsFacet);
    XS_OALLOC(TotalDigFacet);

private:
    uint    value_;
};

/*****************************************************************************
 * WhiteSpaceFacet                                                           *
 *****************************************************************************/

class WhiteSpaceFacet : public XsFacet {
public:
    enum StripType {
        PRESERVE,
        REPLACE,
        COLLAPSE
    };

    WhiteSpaceFacet(bool fixed = false, const XsId& xsid = XsId());
    WhiteSpaceFacet(StripType value, bool fixed = false,
                    const XsId& xsid = XsId());
    void setValue(StripType value);
    StripType value();

    PRTTI_DECL(WhiteSpaceFacet);
    XS_OALLOC(WhiteSpFacet);

private:
    StripType value_;
};

XS_NAMESPACE_END

#endif  //FACETS_H
