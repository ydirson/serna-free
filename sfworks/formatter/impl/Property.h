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

#ifndef FORMATTER_PROPERTY_H
#define FORMATTER_PROPERTY_H

#include <list>
#include "formatter/formatter_defs.h"
#include "formatter/impl/PropertySet.h"

namespace Formatter {

/*
 */
class FORMATTER_EXPIMP Property : public PropertyBase {
public:
    FORMATTER_OALLOC(Property);

    virtual void    calculate(const COMMON_NS::String& specified,
                              ParserContext* context, const Allocation& alloc,
                              const CType& percentBase);
protected:
    virtual bool    calc(const CString& specified, PropertyParser& parser) = 0;

    friend class Shorthand;
    friend class SideShorthand;
    friend class FontProperty;
};

typedef std::list<COMMON_NS::String>    StringList;
void tokenize(const COMMON_NS::String& specified, StringList& tokenList);

/*
 */
class FORMATTER_EXPIMP Shorthand : public PropertyBase {
public:
    void            calculate(const COMMON_NS::String& specified,
                              ParserContext* context, const Allocation& alloc,
                              const CType& percentBase);
protected:
    virtual bool    calc(const StringList& specs, ParserContext* context,
                         const Allocation& alloc) = 0;
};

/*
 */
class NumericProperty : public Property {
public:
    NumericProperty()
        : value_(0) {}

    const CType&    value() const { return value_; }
protected:
    virtual bool    calc(const CString& specified, PropertyParser& parser);

    friend class Margin;
    friend class Padding;
protected:
    CType           value_;
};

/*
 */
class LiteralProperty : public Property {
public:
    const CString&  value() const { return value_; }
protected:
    virtual bool    calc(const CString& specified, PropertyParser& parser);
protected:
    CString         value_;
};

/*
 */
class BooleanProperty : public Property {
public:
    BooleanProperty()
        : value_(false) {}

    bool            value() const { return value_; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;
protected:
    virtual bool    calc(const CString& specified, PropertyParser& parser);

protected:
    bool            value_;
};

/*
 */
class ColorProperty : public Property {
public:
    const Rgb&      value() const { return value_; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;
    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = Rgb(0, 0, 0, true); }
protected:
    virtual bool    calc(const CString& specified, PropertyParser& parser);

    friend class BorderColor;
protected:
    Rgb             value_;
};

/*
 */
template <class T> class EnumProperty : public Property {
public:
    const T&        value() const { return value_; }
protected:
    virtual bool    calc(const CString& specified, PropertyParser& parser)
    {
        bool is_relative = false;
        CType calc_value = 0;
        if (parser.parseNumeric(specified, (PropertyContext*)this,
                                calc_value, is_relative))
            value_ = T(int(calc_value));
        else {
            //DBG(XSL.PROP) << "Error parsing property " << propertyName()
            //              << ":" << parser.errMsg() << std::endl;
            return false;
        }
        return true;
    }
protected:
    T               value_;
};

}

#endif // FORMATTER_PROPERTY_H
