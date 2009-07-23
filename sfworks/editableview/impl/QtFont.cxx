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
#include "editableview/EditableView.h"
#include "formatter/Font.h"
#include "formatter/FontMgr.h"
#include "common/Singleton.h"
#include <iostream>

#include <QApplication>
#include <QFont>
#include <QFontMetrics>
#include <map>

using namespace Common;

namespace {
    const char* TAG_FONT     = NOTR("verdana");
    const char* DEFAULT_FONT = NOTR("arial");

class QtFont : public Formatter::Font {
public:
    QtFont(const String& fromString)
    {
        font_.fromString(fromString);
        init_metrics();
    }
    QtFont(const QFont& f)
        : font_(f)
    {
        init_metrics();
    }
    void init_metrics()
    {
#ifdef __APPLE__	
	font_.setPointSizeF(font_.pointSizeF() * 96 / 72);
#endif // __APPLE__
        QFontMetrics fm(font_);
        accender_  = fm.ascent();
        descender_ = fm.descent();
    }
    virtual uint    width(const String& str) const;
    virtual int     accender() const  { return accender_;  }
    virtual int     descender() const { return descender_; }

    String          toString() const { return font_.toString(); }
    String          fontFamily() const { return font_.family(); }

    const QFont&    qfont() const { return font_; }

private:
    QFont           font_;
    int             accender_;
    int             descender_;
};
    
} // namespace

uint QtFont::width(const String& str) const
{
    if (str.isEmpty())
        return 0;
    Char c  = str[str.length()-1];
    QFontMetrics fm(font_);
    int rbearing = fm.rightBearing(c);
    if (rbearing > 0)
        rbearing = 0;
    return fm.width(str) - rbearing;
}

////////////////////////////////////////////////////////////////////////

class QtFontMgr : public Formatter::FontMgr {
public:
    typedef std::map<String, Formatter::FontPtr>   FontMap;

    QtFontMgr();
    virtual ~QtFontMgr() {}
    //!
    void                clear() { fontMap_.clear(); }
    //!
    Formatter::FontPtr  getFont(const String& family,
                                Formatter::Font::Style style,
                                Formatter::Font::Weight weight,
                                Formatter::Font::Decoration decor,
                                Formatter::CType size);
    //!
    Formatter::FontPtr  getTagFont() { return tagFont_; }
    //!
    Formatter::FontPtr  getDefaultFont() { return defaultFont_; }
    //!
    void                setTagFont(const String& fontSpecs);
    //!
    void                setDefaultFont(const String& fontSpecs);
    //!
    String              tagFontSpecs() const  { return tagFont_->toString(); }

private:
    FontMap             fontMap_;
    Formatter::FontPtr  tagFont_;
    Formatter::FontPtr  defaultFont_;
};

QtFontMgr::QtFontMgr()
{
    tagFont_ = getFont(TAG_FONT, Formatter::Font::STYLE_NORMAL,
                       Formatter::Font::WEIGHT_NORMAL,
                       Formatter::Font::DECORATION_NORMAL, 10);
    defaultFont_ = getFont(DEFAULT_FONT, Formatter::Font::STYLE_NORMAL,
                         Formatter::Font::WEIGHT_NORMAL,
                         Formatter::Font::DECORATION_NORMAL,10);
}

Formatter::FontPtr QtFontMgr::getFont(const String& family, 
                                      Formatter::Font::Style style,
                                      Formatter::Font::Weight weight, 
                                      Formatter::Font::Decoration decor,
                                      Formatter::CType pointSize)
{
    String font_id;
    font_id.reserve(64);
    if (family.isEmpty())
        font_id += DEFAULT_FONT;
    else
        font_id += family;
    font_id += "-" + String::number(int(pointSize));
    font_id += "-" + String::number(style) + "-" + String::number(weight);
    font_id += String::number(decor);

    FontMap::const_iterator i = fontMap_.find(font_id);
    if (fontMap_.end() == i) {
        using namespace Formatter;
        QFont qfont(family, int(pointSize),
            (Font::WEIGHT_BOLD == weight) ? 75 : 50,
            (Font::STYLE_ITALIC == style));
        if (Font::DECORATION_UNDERLINE & decor)
            qfont.setUnderline(true);
        if (Font::DECORATION_OVERLINE & decor)
            qfont.setOverline(true);
        if (Font::DECORATION_STRIKEOUT & decor)
            qfont.setStrikeOut(true);
        FontPtr font = new QtFont(qfont);
        fontMap_[font_id] = font;
        return font;
    }
    return (*i).second;
}

void QtFontMgr::setTagFont(const String& fontSpecs)
{
    tagFont_ = new QtFont(fontSpecs);
}

void QtFontMgr::setDefaultFont(const String& fontSpecs)
{
    defaultFont_ = new QtFont(fontSpecs);
}

Formatter::FontMgr* EditableView::fontManager()
{
    return &SingletonHolder<QtFontMgr>::instance();
}

namespace Formatter {

Font* Font::make(const String& str)
{
    return new QtFont(str);
}

} // namespace
