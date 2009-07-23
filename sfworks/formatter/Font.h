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
 *
 */

#ifndef FORMATTER_FONT_H
#define FORMATTER_FONT_H

#include "common/String.h"
#include "common/RefCntPtr.h"
#include "common/RefCounted.h"
#include "formatter/formatter_defs.h"

class QFont;

namespace Formatter
{

/*! \brief FontInst
 */
class FORMATTER_EXPIMP Font : public Common::RefCounted<> {
public:
    FORMATTER_OALLOC(Font);
    enum Weight {
        WEIGHT_NORMAL,
        WEIGHT_BOLD
    };
    enum Style {
        STYLE_NORMAL,
        STYLE_OBLIQUE,
        STYLE_ITALIC,
        STYLE_BACKSLANT
    };
    enum Decoration {
        DECORATION_NORMAL       = 0x0,
        DECORATION_UNDERLINE    = 0x1,
        DECORATION_OVERLINE     = 0x2,
        DECORATION_STRIKEOUT    = 0x4
    };

    Font()
        : spaceWidth_(-1) {}
    virtual ~Font() {}
    //!
    virtual uint    width(const Common::String& text) const = 0;
    uint            spaceWidth() const;

    //! obtain maximal yMax of a font (in pixels)
    virtual int     accender() const = 0;
    //! obtain maximal absolute value of yMin of a font
    virtual int     descender() const = 0;
    //! set hinting mode for instance
    //!
    virtual COMMON_NS::String toString() const = 0;
    //! get font family. Used in FontProperty for default font
    virtual COMMON_NS::String fontFamily() const = 0;

    virtual const QFont& qfont() const = 0;

    static Font*    make(const Common::String&);

private:
    mutable int spaceWidth_;
};

inline uint Font::spaceWidth() const
{
    if (spaceWidth_ < 0)
        spaceWidth_ = width(" ");
    return spaceWidth_;
}

} //namespace Formatter

#endif //FONT_INSTANCE_H

