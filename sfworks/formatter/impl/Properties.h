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

#ifndef FORMATTER_PROPERTIES
#define FORMATTER_PROPERTIES

#include "formatter/formatter_defs.h"
#include "formatter/types.h"
#include "formatter/Font.h"
#include "formatter/impl/formatter_utils.h"
#include "formatter/impl/Property.h"

namespace Formatter {

typedef COMMON_NS::String CString;

/* Page Properties
 */
class PageWidth : public NumericProperty {
public:
    FORMATTER_OALLOC(PageWidth);

    const CString&  propertyName() const { return name_; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
private:
    static CString  name_;
};

/*
 */
class PageHeight : public NumericProperty {
public:
    FORMATTER_OALLOC(PageHeight);

    const CString&  propertyName() const { return name_; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
private:
    static CString  name_;
};

/*
 */
class Height : public NumericProperty {
public:
    FORMATTER_OALLOC(Height);

    const CString&  propertyName() const { return name_; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
private:
    static CString  name_;
};

/*
 */
class Width : public NumericProperty {
public:
    FORMATTER_OALLOC(Width);

    const CString&  propertyName() const { return name_; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
private:
    static CString  name_;
};

/*
 */
class Indent : public NumericProperty {
public:
    static  bool    isInheritable() { return true; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = 0; }
    void            setValue(CType val) { value_ = val; }
};

class StartIndent : public Indent {
public:
    FORMATTER_OALLOC(StartIndent);

    const CString&  propertyName() const { return name_; }

    static const CString& name() { return name_; }
private:
    static CString  name_;
};

class EndIndent : public Indent {
public:
    FORMATTER_OALLOC(EndIndent);

    const CString&  propertyName() const { return name_; }

    static const CString& name() { return name_; }
private:
    static CString  name_;
};

/*
 */
class BackgroundColor : public ColorProperty {
public:
    FORMATTER_OALLOC(BackgroundColor);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }
private:
    static CString  name_;
};

/*
 */
class Color : public ColorProperty {
public:
    FORMATTER_OALLOC(Color);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return true; }
private:
    static CString  name_;
};

/* Common Font Properties
 */
class FontProperty : public Shorthand {
public:
    FORMATTER_OALLOC(FontProperty);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
protected:
    virtual bool    calc(const StringList& specs, ParserContext* context,
                         const Allocation& alloc);
    void            calcValue(Property* property, const CString& spec,
                              PropertyParser& parser);
private:
    static CString  name_;
};

/*
 */
class TextDecoration : public Property {
public:
    const Font::Decoration& value() const { return value_; }

    const CString&          propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return true; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
protected:
    virtual bool    calc(const CString& specified, PropertyParser& parser);
private:
    static CString          name_;
    Font::Decoration        value_;
};

/*
 */
class FontFamily : public LiteralProperty {
public:
    FORMATTER_OALLOC(FontFamily);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return true; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
private:
    static CString  name_;
};

/*
 */
class FontSize : public NumericProperty {
public:
    FORMATTER_OALLOC(FontSize);

    const CString&  propertyName() const { return name_; }
    double          pointSize() const { return ptSize_; }
    double          pixelSize() const { return pxSize_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return true; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;
protected:
    virtual bool    calc(const CString& specified, PropertyParser& parser);
private:
    double          ptSize_;
    double          pxSize_;
    static CString  name_;
};

/*
 */
class FontWeight : public EnumProperty<Font::Weight> {
public:
    FORMATTER_OALLOC(FontWeight);

    const CString&  propertyName() const { return name_; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return true; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
private:
    static CString  name_;
};

/*
 */
class FontStyle : public EnumProperty<Font::Style> {
public:
    FORMATTER_OALLOC(FontStyle);

    const CString&  propertyName() const { return name_; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return true; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = Font::STYLE_NORMAL; }
private:
    static CString  name_;
};

/* Common Block/Line Properties
 */
enum TAlign { ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER };

class TextAlign : public EnumProperty<TAlign> {
public:
    FORMATTER_OALLOC(TextAlign);

    const CString&  propertyName() const { return name_; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return true; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = ALIGN_LEFT; }
private:
    static CString  name_;
};

/*
 */
class FORMATTER_EXPIMP LinefeedTreatment : public EnumProperty<LfTreatment> {
public:
    FORMATTER_OALLOC(LinefeedTreatment);

    const CString&  propertyName() const { return name_; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return true; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = TREAT_AS_SPACE; }
private:
    static CString  name_;
};

/*
 */
class FORMATTER_EXPIMP WhitespaceTreatment : public EnumProperty<WsTreatment> {
public:
    FORMATTER_OALLOC(WhitespaceTreatment);

    const CString&  propertyName() const { return name_; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return true; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = IGNORE_SURR_LINEFEED; }
private:
    static CString  name_;
};

/*
 */
class FORMATTER_EXPIMP WhitespaceCollapse : public BooleanProperty {
public:
    FORMATTER_OALLOC(WhitespaceCollapse);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return true; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = true; }
private:
    static CString  name_;
};

/* List Related Properties
 */
class ProvisionalDistance : public NumericProperty {
public:
    FORMATTER_OALLOC(ProvisionalDistance);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return true; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
private:
    static CString  name_;
};

/*
 */
class ProvisionalSeparation : public NumericProperty {
public:
    FORMATTER_OALLOC(ProvisionalSeparation);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return true; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
protected:
    virtual bool    calc(const CString& specified, PropertyParser& parser);
private:
    static CString  name_;
};

/* Table Related Properties
 */
class ColumnWidth : public NumericProperty {
public:
    FORMATTER_OALLOC(ColumnWith);
    ColumnWidth();

    const CString&  propertyName() const { return name_; }
    const CType&    proportion() const { return proportion_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }

    virtual void    init(ParserContext*, const Allocation&, const CType&);
protected:
    virtual bool    calc(const CString& specified, PropertyParser& parser);
private:
    CType           proportion_;
    static CString  name_;
};

/*
 */
class ColumnRepeats : public NumericProperty {
public:
    FORMATTER_OALLOC(ColumnRepeats);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = 1; }
private:
    static CString  name_;
};

/*
 */
class StartsRow : public BooleanProperty {
public:
    FORMATTER_OALLOC(StartsRow);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = false; }
private:
    static CString  name_;
};

/*
 */
class EndsRow : public BooleanProperty {
public:
    FORMATTER_OALLOC(EndsRow);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = false; }
private:
    static CString  name_;
};

/*
 */
class ColumnsSpanned : public NumericProperty {
public:
    FORMATTER_OALLOC(ColumnsSpanned);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = 1; }
private:
    static CString  name_;
};

/*
 */
class RowsSpanned : public NumericProperty {
public:
    FORMATTER_OALLOC(RowsSpanned);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = 1; }
private:
    static CString  name_;
};

/*
 */
class ColumnNumber : public NumericProperty {
public:
    FORMATTER_OALLOC(ColumnNumber);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = -1; }
private:
    static CString  name_;
};

/* External Graphics Fo Properties
 */

class SrcUri : public LiteralProperty {
public:
    FORMATTER_OALLOC(SrcUri);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return true; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
private:
    static CString  name_;
};

/*
 */
class ContentWidth : public NumericProperty {
public:
    ContentWidth()
        : isAuto_(false) {}
    FORMATTER_OALLOC(ContentWidth);

    bool            isAuto() const { return isAuto_; }
    const CString&  propertyName() const { return name_; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
private:
    static CString  name_;
    mutable bool    isAuto_;
};

/*
 */
class ContentHeight : public ContentWidth {
public:
    FORMATTER_OALLOC(ContentHeight);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }

private:
    static CString  name_;
};

/*
 */
class BaselineShift : public NumericProperty {
public:
    FORMATTER_OALLOC(BaselineShift);

    const CString&  propertyName() const { return name_; }
    virtual bool    resolveEnumToken(const CString& token, ValueTypePair& rv,
                                     const ParserContext& pcontext,
                                     const Allocation& alloc) const;

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }

    virtual void    init(ParserContext* pcontext, const Allocation& alloc,
                         const CType& percentBase);
private:
    static CString  name_;
};


}

#endif // FORMATTER_PROPERTIES
