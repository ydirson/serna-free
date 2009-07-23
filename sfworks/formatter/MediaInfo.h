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
#ifndef FORMATTER_MEDIA_INFO_H
#define FORMATTER_MEDIA_INFO_H

#include "formatter/formatter_defs.h"
#include "formatter/types.h"
#include "common/String.h"

namespace Formatter {

extern const float  INCH;
extern const CRange A4;

class ImageProvider;
class InlineObjectFactory;

/////////////////////////////////////////////////////////////////////

class FORMATTER_EXPIMP ColorScheme {
public:
    FORMATTER_OALLOC(ColorScheme);
    virtual ~ColorScheme() {};
    //!
    virtual Rgb     evenContColor() const = 0;
    //!
    virtual Rgb     oddContColor() const = 0;
    //!
    virtual void    tagColor(const Common::String& nsUri,
                             Rgb& tagColor, Rgb& textColor) const = 0;
};

/////////////////////////////////////////////////////////////////////

struct FORMATTER_EXPIMP MediaInfo {
public:
    FORMATTER_OALLOC(MediaInfo);

    MediaInfo(CType dpi, bool isToShowTags, bool isPaginated, 
              CType indent, const ColorScheme* colorScheme,
              const ImageProvider* imageProvider,
              const InlineObjectFactory* objectFactory = 0,
              bool  visNbsp = false)
        : dpi_(dpi),
          isShowTags_(isToShowTags),
          isPaginated_(isPaginated),
          visualizeNbsp_(visNbsp),
          indent_(indent),
          colorScheme_(colorScheme),
          imageProvider_(imageProvider),
          inlineObjectFactory_(objectFactory) {}

    CType               dpi_;
    bool                isShowTags_;
    bool                isPaginated_;
    bool                visualizeNbsp_;
    CType               indent_;
    const ColorScheme*  colorScheme_;
    const ImageProvider* imageProvider_;
    const InlineObjectFactory* inlineObjectFactory_;
};

} // namespace Formatter

#endif // FORMATTER_MEDIA_INFO_H
