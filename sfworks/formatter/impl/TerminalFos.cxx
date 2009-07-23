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

#include "formatter/impl/Areas.h"
#include "formatter/impl/TerminalFos.h"
#include "formatter/impl/formatter_utils.h"

#include "grove/udata.h"
#include "grove/SectionNodes.h"
#include "grove/RedlineUtils.h"
#include "grove/SectionSyncher.h"
#include "common/Url.h"

using namespace Common;
using namespace GroveLib;

#ifdef Q_OS_MACX
static const int CURSOR_WIDTH = 4;
#else
static const int CURSOR_WIDTH = 3;
#endif

namespace Formatter {

/////////////////////////////////////////////////////////////////////////////

void GraphicFo::calcProperties(const Allocation& alloc)
{
    String basePath, altPath;
    if (node() && node()->grove()) {
        const Node* origin = XslFoExt::origin(node());
        if (origin) {
            altPath  = get_ers(origin)->currentGrove()->topSysid();
            basePath = origin->grove()->topSysid();
        } 
        else
            basePath = node()->grove()->topSysid();
    }
    String src_uri = getProperty<SrcUri>(alloc).value();
    if (src_uri.left(4) == "url(" && src_uri.length() > 5) 
        src_uri = src_uri.mid(4, src_uri.length() - 5);
    String src, altSrc;
    src = Url(basePath).combinePath2Path(src_uri);
    if (!altPath.isEmpty() && altPath != basePath)
        altSrc = Url(altPath).combinePath2Path(src_uri);
    if (altSrc == src)
        altSrc = String();
    bool is_found = false;
    if (!src.isEmpty()) {
        setImage(data_.mediaInfo_.imageProvider_->
            makeExternalImage(src, content_));
        size_ = image()->size(&is_found);
    }
    if (!is_found && !altSrc.isEmpty()) 
        setImage(data_.mediaInfo_.imageProvider_->
            makeExternalImage(altSrc, content_));
    size_ = image()->size(&is_found);
    //! TODO: use border/padding specs
    content_.w_ = (is_found)
        ? getProperty<ContentWidth>(alloc, size_.w_).value() : size_.w_;
    ContentHeight& content_height = 
        getProperty<ContentHeight>(alloc, size_.h_);
    if (content_.w_ > alloc.space_.extent_.w_)
        content_.w_ = alloc.space_.extent_.w_;
    CType auto_height = content_.w_ / size_.w_ * size_.h_;
    if (content_height.isAuto()) 
        content_.h_ = auto_height;
    else {
        content_.h_ = (is_found) ? content_height.value() : size_.h_;
        if (content_.h_ > auto_height * 5)
            content_.h_  = auto_height * 5; // sanity check
    }
    contColor_ = Rgb(0, 0, 0, true);
}

bool GraphicFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    return ((alloc.maxh_ || alloc.space_.extent_.h_ >= content_.h_) &&
            (alloc.maxw_ || alloc.space_.extent_.w_ >= content_.w_));
}

Area* GraphicFo::makeArea(const Allocation& alloc, const Area* after,
                          bool forceToMake) const
{
    RT_MSG_ASSERT((0 == after), "Graphics cannot be spread among areas");

    if (!forceToMake && !isEnoughSpace(alloc, after))
        return 0;
    
    image()->resize(content_);
        
    GraphicArea* graphic = new GraphicArea(this, image());

    graphic->allcPoint_ = alloc.space_.origin_;

    graphic->contPoint_ = CPoint(0, 0);
    graphic->contRange_ = content_;
    graphic->base_      = content_.h_;

    return graphic;
}

String GraphicFo::name() const
{
    return "GraphicFo";
}

/////////////////////////////////////////////////////////////////////////////

TextFo::TextFo(const FoInit& init)
    : TerminalFo(init),
      wsTreatment_(PRESERVE_WHITESPACE),
      lfTreatment_(PRESERVE_LINEFEED),
      wsCollapse_(false),
      accender_(0),
      descender_(0)
{
}

void adjust_redline_props(const Node* foNode, int& decoration, 
                          Rgb& textColor, Rgb& contColor)
{
    const Node* origin = 0;
    for (const Node* fo_node = foNode; fo_node; fo_node = fo_node->parent()) {
        if ((origin = XslFoExt::origin(fo_node)))
            break;
    }
    if (!origin || !has_redline(origin)) 
        return;
    RedlineSectionStart* rss = get_redline_section(origin);
    if (!rss) 
        return;
    if (1 << REDLINE_STRIKE_THRU & rss->redlineData()->redlineMask()) {
        decoration |= Font::DECORATION_STRIKEOUT;
        textColor = Rgb(0xD0, 0, 0);
        return;
    }
    if (1 << REDLINE_UNDERLINE & rss->redlineData()->redlineMask()) {
        decoration |= Font::DECORATION_UNDERLINE;
        textColor = Rgb(0, 0xA0, 0);
        return;
    }
    contColor = Rgb(0xff, 0xfd, 0xa3);
}
    
void TextFo::calcProperties(const Allocation& alloc)
{
    DBG(XSL.PROPDYN) << "Calc properties:" << this << std::endl;

    contColor_ = Rgb(0, 0, 0, true);
    textColor_ = getProperty<Color>(alloc).value();

    wsTreatment_ = getProperty<WhitespaceTreatment>(alloc).value();
    lfTreatment_ = getProperty<LinefeedTreatment>(alloc).value();
    wsCollapse_ = getProperty<WhitespaceCollapse>(alloc).value();

    treatSpaces();

    decoration_ = getProperty<TextDecoration>(alloc).value();
    adjust_redline_props(node(), decoration_, textColor_, contColor_);
    font_ = data_.fontMgr_->getFont(getProperty<FontFamily>(alloc).value(),
                                    getProperty<FontStyle>(alloc).value(),
                                    getProperty<FontWeight>(alloc).value(),
                                    Font::Decoration(decoration_),
                                    getProperty<FontSize>(alloc).pointSize());
    accender_ = font_->accender();
    descender_ = font_->descender();
}

bool TextFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    alloc.enoughW_ = alloc.maxw_ || alloc.space_.extent_.w_ > 0;
    alloc.enoughH_ = alloc.maxh_ ||
        alloc.space_.extent_.h_ > accender_ + descender_;
    //return (alloc.maxw_ || (alloc.enoughW_ && alloc.enoughH_));
    return (alloc.enoughW_ && alloc.enoughH_);
}

Area* TextFo::makeArea(const Allocation& alloc, const Area* after,
                       bool forceToMake) const
{
    if (!forceToMake && !isEnoughSpace(alloc, after))
        return 0;

    const TextArea* prev_text = SAFE_CAST(const TextArea*, after);
    ulong pos = (prev_text)
        ? prev_text->pos() + prev_text->text().length()
        : 0;

    bool is_last_after_linefeed =
        (0 < pos && text_.length() == pos && '\n' == text_[pos - 1]);

    RangeString rtext(text_);
    TextChunk t_chunk = (is_last_after_linefeed)
        ? TextChunk(RangeString(), CURSOR_WIDTH)
        : get_text_chunk(rtext.mid(pos), alloc.space_.extent_.w_,
                         alloc.maxw_, font_);

    if (!(forceToMake || alloc.maxw_) && !is_last_after_linefeed &&
        0 == t_chunk.text_.length() && !text_.isEmpty())
        return 0;

    rtext = (t_chunk.text_.length() || is_last_after_linefeed)
        ? t_chunk.text_ : rtext.mid(pos, 1);
    TextArea* text =  new TextArea(this, rtext, text_, pos);

    text->allcPoint_ = alloc.space_.origin_;
    text->base_ = accender_;
    text->contRange_.h_ = accender_ + descender_;
    if (t_chunk.text_.length() || is_last_after_linefeed)
        text->contRange_.w_ = t_chunk.width_;
    else
        text->contRange_.w_ = font_->width(text->text());

    return text;
}

bool TextFo::isChainFinishedAt(const AreaPtr& area) const
{
    if (text_.isEmpty())
        return true;
    if (area.isNull())
        return false;
    const TextArea* text = SAFE_CAST(const TextArea*, area.pointer());
    ulong len = text->text().length();
    if (0 < len && '\n' == text->text()[len - 1])
        return false;
    return (len + text->pos() == text_.length());
}

bool TextFo::hasContent(const AreaPtr& area) const
{
    return !area.isNull();
}

void TextFo::textChanged(const Text*)
{
    treatSpaces();
    DBG(XSL.FODYN) << "TextFo: text changed " << this << std::endl;
    registerModification(THIS_FO_MODIFIED);
}

void TextFo::treatSpaces()
{
    Text* text = const_cast<Text*>(SAFE_CAST(const Text*, node()));
    text_ = treat_spaces(text->data(), wsTreatment_,
                         lfTreatment_, wsCollapse_,
                         data_.mediaInfo_.visualizeNbsp_);
}

ulong TextFo::convertPos(ulong pos, bool toStripped) const
{
    Text* text = const_cast<Text*>(SAFE_CAST(const Text*, node()));
    return (toStripped)
        ? to_stripped(text->data(), text_, pos)
        : from_stripped(text->data(), text_, pos);
}

void TextFo::dump(int indent) const
{
    ReferencedFo::dump(4);
    DINDENT(indent + 4);
    DBG(XSL.CHAIN)
        << "<" << static_cast<const Text*>(node())->data() << ">" << std::endl;
}

String TextFo::name() const
{
    return "TextFo";
}

/////////////////////////////////////////////////////////////////////////////

void ExtensionFo::calcProperties(const Allocation& alloc)
{
    if (BLOCK_CONTENT == contentType_)
        indent_ = getProperty<StartIndent>(alloc).value();
}

/////////////////////////////////////////////////////////////////////////////

SectionCornerFo::SectionCornerFo(const FoInit& init)
    : ExtensionFo(init), 
      tagMetrix_(data_.tagMetrixTable_.getMetrix(
                 init.foNode_->nodeName(),
                 data_.fontMgr_->getTagFont()))
{
    const Element* elem = static_cast<const Element*>(init.foNode_);
    const Attr* type_attr = elem->attrs().getAttribute("type");
    String type = type_attr ? type_attr->value() : String();
    if ("internal-entity" == type) 
        tagSize_ = tagMetrix_->size(TagMetrix::INTERNAL_SECTION);
    else if ("external-entity" == type)
        tagSize_ = tagMetrix_->size(TagMetrix::EXTERNAL_SECTION);
    else if ("xinclude" == type || "xinclude-fallback" == type)
        tagSize_ = tagMetrix_->size(TagMetrix::XINCLUDE_SECTION);
    else if ("marked-section" == type)
        tagSize_ = tagMetrix_->size(TagMetrix::MARKED_SECTION);
    else // if ("redline" == type)
        tagSize_ = tagMetrix_->size(TagMetrix::REDLINE_SECTION);
}

bool SectionCornerFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    return ((alloc.maxh_ || alloc.space_.extent_.h_ >= tagSize_.h_) &&
            (alloc.maxw_ || alloc.space_.extent_.w_ >= tagSize_.w_));
}

Area* SectionCornerFo::makeArea(const Allocation& alloc, const Area*,
                                bool forceToMake) const
{
    if (!forceToMake && !isEnoughSpace(alloc, 0))
        return 0;

    SectionCornerArea* corner = new SectionCornerArea(this);

    corner->allcPoint_ = alloc.space_.origin_;
    corner->contPoint_ = CPoint(indent_, 0);
    corner->contRange_ = tagSize_;
    CType accender = data_.fontMgr_->getTagFont()->accender();
    CType descender = data_.fontMgr_->getTagFont()->descender();
    accender = accender - ((accender + descender - tagSize_.h_) / 2);
    corner->base_ = accender;
    return corner;
}

String SectionCornerFo::name() const
{
    return "SectionCornerFo";
}

/////////////////////////////////////////////////////////////////////////////

ChoiceFo::ChoiceFo(const FoInit& init)
    : ExtensionFo(init),
      tagMetrix_(data_.tagMetrixTable_.getMetrix(
                 "Choose", data_.fontMgr_->getTagFont()))
{
}

void ChoiceFo::calcProperties(const Allocation& alloc)
{
    ExtensionFo::calcProperties(alloc);
    font_ = data_.fontMgr_->getFont(getProperty<FontFamily>(alloc).value(),
                                    getProperty<FontStyle>(alloc).value(),
                                    getProperty<FontWeight>(alloc).value(),
                                    getProperty<TextDecoration>(alloc).value(),
                                    getProperty<FontSize>(alloc).pointSize());
}

bool ChoiceFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    CRange tag = tagMetrix_->size(TagMetrix::CHOICE);
    return ((alloc.maxh_ || alloc.space_.extent_.h_ >= tag.h_) &&
            (alloc.maxw_ || alloc.space_.extent_.w_ >= tag.w_));
}

Area* ChoiceFo::makeArea(const Allocation& alloc, const Area*,
                          bool forceToMake) const
{
    if (!forceToMake && !isEnoughSpace(alloc, 0))
        return 0;

    ChoiceArea* choice = new ChoiceArea(this);

    choice->allcPoint_ = alloc.space_.origin_;
    choice->contPoint_ = CPoint(indent_, 0);
    choice->contRange_ = tagMetrix_->size(TagMetrix::CHOICE);
    choice->base_ = font_->accender();

    return choice;
}

String ChoiceFo::name() const
{
    return "ChoiceFo";
}

/////////////////////////////////////////////////////////////////////////////

CommentFo::CommentFo(const FoInit& init)
    : ExtensionFo(init),
      ImageContainer(data_.mediaInfo_.imageProvider_->
        makeBuiltinImage(ImageProvider::COMMENT_ICON))
{
}

bool CommentFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    const CRange size(image()->size());
    return ((alloc.maxh_ || alloc.space_.extent_.h_ >= size.h_) &&
            (alloc.maxw_ || alloc.space_.extent_.w_ >= size.w_));
}

Area* CommentFo::makeArea(const Allocation& alloc, const Area*,
                          bool forceToMake) const
{
    if (!forceToMake && !isEnoughSpace(alloc, 0))
        return 0;

    CommentArea* unknown = new CommentArea(this);

    unknown->allcPoint_ = alloc.space_.origin_;
    unknown->contPoint_ = CPoint(indent_, 0);
    unknown->contRange_ = image()->size();
    unknown->base_ = unknown->contRange_.h_;

    return unknown;
}

String CommentFo::name() const
{
    return "CommentFo";
}

/////////////////////////////////////////////////////////////////////////////

PiFo::PiFo(const FoInit& init)
    : ExtensionFo(init),
      ImageContainer(data_.mediaInfo_.imageProvider_->
        makeBuiltinImage(ImageProvider::PI_ICON))
{
}

bool PiFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    const CRange size(image()->size());
    return ((alloc.maxh_ || alloc.space_.extent_.h_ >= size.h_) &&
            (alloc.maxw_ || alloc.space_.extent_.w_ >= size.w_));
}

Area* PiFo::makeArea(const Allocation& alloc, const Area*,
                          bool forceToMake) const
{
    if (!forceToMake && !isEnoughSpace(alloc, 0))
        return 0;

    PiArea* unknown = new PiArea(this);

    unknown->allcPoint_ = alloc.space_.origin_;
    unknown->contPoint_ = CPoint(indent_, 0);
    unknown->contRange_ = image()->size();
    unknown->base_ = unknown->contRange_.h_;

    return unknown;
}

String PiFo::name() const
{
    return "PiFo";
}

/////////////////////////////////////////////////////////////////////////////

FoldFo::FoldFo(const FoInit& init)
    : ExtensionFo(init),
      tagMetrix_(data_.tagMetrixTable_.getMetrix(
                     node_name(foOrigin_), data_.fontMgr_->getTagFont())),
      size_(tagMetrix_->size(TagMetrix::FOLD))
{
    if (!isShowName())
        size_.w_ = size_.h_;
}

bool FoldFo::isShowName() const
{
    return ("false" != get_attr_value(node(), "show-element-name"));
}

bool FoldFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    return ((alloc.maxh_ || alloc.space_.extent_.h_ >= size_.h_) &&
            (alloc.maxw_ || alloc.space_.extent_.w_ >= size_.w_));
}

Area* FoldFo::makeArea(const Allocation& alloc, const Area*,
                       bool forceToMake) const
{
    if (!forceToMake && !isEnoughSpace(alloc, 0))
        return 0;

    FoldArea* fold = new FoldArea(this);

    fold->allcPoint_ = alloc.space_.origin_;
    fold->contPoint_ = CPoint(indent_, 0);
    fold->contRange_ = size_;
    fold->base_ = fold->contRange_.h_;

    return fold;
}

String FoldFo::name() const
{
    return "FoldFo";
}

/////////////////////////////////////////////////////////////////////////////

InlineObjectFo::InlineObjectFo(const FoInit& init)
    : TerminalFo(init)
{
}

void InlineObjectFo::calcProperties(const Allocation& alloc)
{
    const Chain* chain = parent();
    while (chain) {
        contColor_ = chain->contColor();
        if (!contColor_.isTransparent_)
            break;
        chain = chain->parentChain();
    }
    String fontFamily = getProperty<FontFamily>(alloc).value();
    const CType fontSize = getProperty<FontSize>(alloc).pointSize();
    object_ = data_.mediaInfo_.inlineObjectFactory_->make
        (InlineObjectData(foOrigin_, contColor_, fontFamily, fontSize));
}

bool InlineObjectFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    const CRange& size = object_->size();
    return ((alloc.maxh_ || alloc.space_.extent_.h_ >= size.h_) &&
            (alloc.maxw_ || alloc.space_.extent_.w_ >= size.w_));
}

Area* InlineObjectFo::makeArea(const Allocation& alloc, const Area* after,
                          bool forceToMake) const
{
    RT_MSG_ASSERT((0 == after), "InlineContent cannot be spread among areas");

    if (!forceToMake && !isEnoughSpace(alloc, after))
        return 0;

    InlineObjectArea* area = new InlineObjectArea(this);

    area->allcPoint_ = alloc.space_.origin_;
    area->contPoint_ = CPoint(0, 0);
    area->contRange_ = object_->size();
    area->base_ = area->contRange_.h_;

    return area;
}

String InlineObjectFo::name() const
{
    return "InlineObjectFo";
}

/////////////////////////////////////////////////////////////////////////////

UnknownFo::UnknownFo(const FoInit& init)
    : ExtensionFo(init),
      ImageContainer(data_.mediaInfo_.imageProvider_->
        makeBuiltinImage(ImageProvider::UNKNOWN_ICON))
{
}

bool UnknownFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    const CRange size(image()->size());
    return ((alloc.maxh_ || alloc.space_.extent_.h_ >= size.h_) &&
            (alloc.maxw_ || alloc.space_.extent_.w_ >= size.w_));
}

Area* UnknownFo::makeArea(const Allocation& alloc, const Area*,
                          bool forceToMake) const
{
    if (!forceToMake && !isEnoughSpace(alloc, 0))
        return 0;

    UnknownArea* unknown = new UnknownArea(this);

    unknown->allcPoint_ = alloc.space_.origin_;
    unknown->contPoint_ = CPoint(indent_, 0);
    unknown->contRange_ = image()->size();
    unknown->base_ = unknown->contRange_.h_;

    return unknown;
}

String UnknownFo::name() const
{
    return "UnknownFo";
}

} // namespace Formatter
