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

#ifndef FORMATTER_BORDER_PROPERTIES
#define FORMATTER_BORDER_PROPERTIES

#include "formatter/formatter_defs.h"
#include "formatter/types.h"
#include "formatter/impl/Property.h"

namespace Formatter {

typedef COMMON_NS::String CString;

/*
 */
class SideShorthand : public Shorthand {
public:
    typedef OValue<Property*>   OProp;

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
protected:
    virtual bool    calc(const StringList& specs, ParserContext* context,
                         const Allocation& alloc);
    virtual OProp   getSideProperties(ParserContext* pcontext,
                                      const Allocation& alloc) = 0;
    virtual void    copyValue(Property* to, Property* from) = 0;

    void            calcValue(Property* property, const CString& spec,
                              PropertyParser& parser);
    void            calcTwins(Property* first, Property* second,
                              const CString& spec, PropertyParser& parser);
};

/*!
 */
ORect getBorder(PropertySet& propertySet, const Allocation& alloc);

/*
 */
class BorderWidth : public SideShorthand {
public:
    FORMATTER_OALLOC(BorderWidth);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }

protected:
    virtual OProp   getSideProperties(ParserContext* pcontext,
                                      const Allocation& alloc);
    virtual void    copyValue(Property* to, Property* from);

private:
    static CString  name_;
};

/*
 */
class BorderSideWidth : public NumericProperty {
public:
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);

    friend class BorderWidth;
};

class BorderTopWidth : public BorderSideWidth {
public:
    FORMATTER_OALLOC(BorderTopWidth);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }
private:
    static CString  name_;
};

class BorderBottomWidth : public BorderSideWidth {
public:
    FORMATTER_OALLOC(BorderBottomWidth);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }
private:
    static CString  name_;
};

class BorderLeftWidth : public BorderSideWidth {
public:
    FORMATTER_OALLOC(BorderLeftWidth);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }
private:
    static CString  name_;
};

class BorderRightWidth : public BorderSideWidth {
public:
    FORMATTER_OALLOC(BorderRightWidth);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }
private:
    static CString  name_;
};

/*
 */
OValue<Rgb> getBorderColor(PropertySet& p_set, const Allocation& alloc);

class BorderColor : public SideShorthand {
public:
    typedef OValue<Property*>   OProp;

    FORMATTER_OALLOC(BorderColor);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }

protected:
    virtual OProp   getSideProperties(ParserContext* pcontext,
                                      const Allocation& alloc);
    virtual void    copyValue(Property* to, Property* from);

private:
    static CString  name_;
};

/*
 */
class BorderTopColor : public ColorProperty {
public:
    FORMATTER_OALLOC(BorderTopColor);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }
private:
    static CString  name_;
};

class BorderBottomColor : public ColorProperty {
public:
    FORMATTER_OALLOC(BorderBottomColor);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }
private:
    static CString  name_;
};

class BorderLeftColor : public ColorProperty {
public:
    FORMATTER_OALLOC(BorderLeftColor);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }
private:
    static CString  name_;
};

class BorderRightColor : public ColorProperty {
public:
    FORMATTER_OALLOC(BorderRightColor);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }
private:
    static CString  name_;
};

/*! Common Margin Properties
 */
ORect getMargin(PropertySet& propertySet, const Allocation& alloc);

class Margin : public SideShorthand {
public:
    typedef OValue<Property*>   OProp;

    FORMATTER_OALLOC(Margin);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }

protected:
    virtual OProp   getSideProperties(ParserContext* pcontext,
                                      const Allocation& alloc);
    virtual void    copyValue(Property* to, Property* from);

private:
    static CString  name_;
};

/*
 */
class MarginWidth : public NumericProperty {
public:
    static  bool    isInheritable() { return false; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;
    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = 0; }
};

/*
 */
class MarginTop : public MarginWidth {
public:
    FORMATTER_OALLOC(MarginTop);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
private:
    static CString  name_;
};

class MarginBottom : public MarginWidth {
public:
    FORMATTER_OALLOC(MarginBottom);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
private:
    static CString  name_;
};

class MarginLeft : public MarginWidth {
public:
    FORMATTER_OALLOC(MarginLeft);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
private:
    static CString  name_;
};

class MarginRight : public MarginWidth {
public:
    FORMATTER_OALLOC(MarginRight);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
private:
    static CString  name_;
};

/*
 */
/*! Common Padding Properties
 */
ORect getPadding(PropertySet& propertySet, const Allocation& alloc);

class Padding : public SideShorthand {
public:
    typedef OValue<Property*>   OProp;

    FORMATTER_OALLOC(Padding);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }

protected:
    virtual OProp   getSideProperties(ParserContext* pcontext,
                                      const Allocation& alloc);
    virtual void    copyValue(Property* to, Property* from);

private:
    static CString  name_;
};

/*
 */
class PaddingWidth : public NumericProperty {
public:
    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = 0; }
};

/*
 */
class PaddingTop : public PaddingWidth {
public:
    FORMATTER_OALLOC(PaddingTop);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }
private:
    static CString  name_;
};

class PaddingBottom : public PaddingWidth {
public:
    FORMATTER_OALLOC(PaddingBottom);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }
private:
    static CString  name_;
};

class PaddingLeft : public PaddingWidth {
public:
    FORMATTER_OALLOC(PaddingLeft);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }
private:
    static CString  name_;
};

class PaddingRight : public PaddingWidth {
public:
    FORMATTER_OALLOC(PaddingRight);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }
private:
    static CString  name_;
};


}

#endif // FORMATTER_BORDER_PROPERTIES
