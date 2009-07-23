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

#ifndef FORMATTER_TERMINAL_FOS_H
#define FORMATTER_TERMINAL_FOS_H

#include "common/Vector.h"
#include "formatter/Image.h"
#include "formatter/InlineObject.h"
#include "formatter/impl/ReferencedFo.h"

namespace Formatter {

/////////////////////////////////////////////////////////////////////////////

class GraphicFo : public TerminalFo,
                  public ImageContainer {
public:
    FORMATTER_OALLOC(GraphicFo);

    GraphicFo(const FoInit& init)
        : TerminalFo(init) {}

    FoType          type() const { return GRAPHIC_FO; }
    //!
    bool            isEnoughSpace(const Allocation& alloc,
                                  const Area* after) const;
    //!
    Area*           makeArea(const Allocation& alloc, const Area* after,
                             bool forceToMake) const;
    //!
    Common::String  name() const;
    const Common::String& source() const { return src_; }
    const Common::String& altSource() const { return altSrc_; }
    
protected:
    //!
    void            calcProperties(const Allocation& alloc);
private:
    Common::String  src_;
    Common::String  altSrc_;
    CRange          size_;
    CRange          content_;
};

/////////////////////////////////////////////////////////////////////////////

class FORMATTER_EXPIMP TextFo : public TerminalFo {
public:
    FORMATTER_OALLOC(TextFo);

    TextFo(const FoInit& init);

    FoType          type() const { return TEXT_FO; }
    //!
    bool            isEnoughSpace(const Allocation& alloc,
                                  const Area* after) const;
    //!
    Area*           makeArea(const Allocation& alloc, const Area* after,
                             bool forceToMake) const;
    //! Returns true if given area contains the last text chunk
    bool            isChainFinishedAt(const AreaPtr& area) const;
    //! Returns true if area has text
    bool            hasContent(const AreaPtr& area) const;
    //!
    bool            isPreserveLinefeed() const {
        return PRESERVE_LINEFEED == lfTreatment_;
    }
    //!
    Common::String  strippedText() const { return text_; }
    //! Prints the debugging info
    void            dump(int indent) const;
    //!
    ulong           convertPos(ulong pos, bool toStripped) const;
    //! Uses parent fo to obtain properties
    template <class T> T& getProperty(const Allocation& alloc,
                                      const CType percentBase = -1) {
        T& property = parentSet_->getProperty<T>(alloc, percentBase);
        usedPropertyMap_[T::name()] = 0;
        return property;
    }
    //!
    Common::String  name() const;
    int             textDecoration() const { return decoration_; }
    //!
    void            textChanged(const GroveLib::Text* node);
protected:
    //!
    void            calcProperties(const Allocation& alloc);
    //!
    void            treatSpaces();
private:
    WsTreatment     wsTreatment_ : 4;
    LfTreatment     lfTreatment_ : 4;
    bool            wsCollapse_  : 1;
    CType           accender_;
    CType           descender_;
    int             decoration_;
    Common::String  text_;
};

/*! \brief Generates no children
 */
class ExtensionFo : public TerminalFo {
public:
    FORMATTER_OALLOC(ExtensionFo);

    ExtensionFo(const FoInit& init)
        : TerminalFo(init), contentType_(INLINE_CONTENT),
          indent_(0) {}
    //!
    ContentType     contentType() const { return contentType_; }
    //!
    void            setContentType(ContentType type) { contentType_ = type; }
protected:
    //!
    void            calcProperties(const Allocation& alloc);
protected:
    ContentType     contentType_;
    CType           indent_;
};

/////////////////////////////////////////////////////////////////////////////

class SectionCornerFo : public ExtensionFo {
public:
    FORMATTER_OALLOC(SectionCornerFo);

    SectionCornerFo(const FoInit& init);

    FoType          type() const { return SECTION_CORNER_FO; }
    //!
    bool            isEnoughSpace(const Allocation& alloc,
                                  const Area* after) const;
    //!
    Area*           makeArea(const Allocation& alloc, const Area* after,
                             bool forceToMake) const;
    //!
    Common::String  name() const;
private:
    TagMetrix*      tagMetrix_;
    CRange          tagSize_;
};

/////////////////////////////////////////////////////////////////////////////

class ChoiceFo : public ExtensionFo {
public:
    FORMATTER_OALLOC(ChoiceFo);

    ChoiceFo(const FoInit& init);

    FoType          type() const { return CHOICE_FO; }
    //!
    bool            isEnoughSpace(const Allocation& alloc,
                                  const Area* after) const;
    //!
    Area*           makeArea(const Allocation& alloc, const Area* after,
                             bool forceToMake) const;
    //!
    Common::String  name() const;
protected:
    //!
    void            calcProperties(const Allocation& alloc);
private:
    TagMetrix*      tagMetrix_;
};

/////////////////////////////////////////////////////////////////////////////

class CommentFo : public ExtensionFo,
                  public ImageContainer {
public:
    FORMATTER_OALLOC(CommentFo);

    CommentFo(const FoInit& init);

    FoType          type() const { return COMMENT_FO; }
    //!
    bool            isEnoughSpace(const Allocation& alloc,
                                  const Area* after) const;
    //!
    Area*           makeArea(const Allocation& alloc, const Area* after,
                             bool forceToMake) const;
    //!
    Common::String  name() const;
};

/////////////////////////////////////////////////////////////////////////////

class PiFo : public ExtensionFo,
             public ImageContainer {
public:
    FORMATTER_OALLOC(PiFo);

    PiFo(const FoInit& init);

    FoType          type() const { return PI_FO; }
    //!
    bool            isEnoughSpace(const Allocation& alloc,
                                  const Area* after) const;
    //!
    Area*           makeArea(const Allocation& alloc, const Area* after,
                             bool forceToMake) const;
    //!
    Common::String  name() const;
};

/////////////////////////////////////////////////////////////////////////////

class FORMATTER_EXPIMP FoldFo : public ExtensionFo {
public:
    FORMATTER_OALLOC(FoldFo);

    FoldFo(const FoInit& init);

    FoType          type() const { return FOLD_FO; }
    //!
    bool            isEnoughSpace(const Allocation& alloc,
                                  const Area* after) const;
    //!
    Area*           makeArea(const Allocation& alloc, const Area* after,
                             bool forceToMake) const;
    //!
    Common::String  name() const;
    //!
    bool            isShowName() const;
private:
    TagMetrix*      tagMetrix_;
    CRange          size_;
};

/////////////////////////////////////////////////////////////////////////////

class InlineObjectFo : public TerminalFo {
public:
    FORMATTER_OALLOC(InlineObjectFo);

    InlineObjectFo(const FoInit& init);

    FoType          type() const { return INLINE_OBJECT_FO; }
    //!
    bool            isEnoughSpace(const Allocation& alloc,
                                  const Area* after) const;
    //!
    Area*           makeArea(const Allocation& alloc, const Area* after,
                             bool forceToMake) const;
    InlineObject*   inlineObject() const { return object_.pointer(); }
    //!
    Common::String  name() const;
    
protected:
    void            calcProperties(const Allocation& alloc);
    Common::OwnerPtr<InlineObject> object_;
};

/////////////////////////////////////////////////////////////////////////////

class UnknownFo : public ExtensionFo,
                  public ImageContainer {
public:
    FORMATTER_OALLOC(UnknownFo);

    UnknownFo(const FoInit& init);

    FoType          type() const { return UNKNOWN_FO; }
    //!
    bool            isEnoughSpace(const Allocation& alloc,
                                  const Area* after) const;
    //!
    Area*           makeArea(const Allocation& alloc, const Area* after,
                             bool forceToMake) const;
    //!
    Common::String  name() const;
};

} // namespace Formatter

#endif // FORMATTER_REFERENCED_FO_H
