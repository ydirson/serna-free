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
#include "structeditor/StructEditor.h"
#include "structeditor/impl/ViewParamImpl.h"
#include "editableview/EditableView.h"
#include "formatter/types.h"
#include "utils/Properties.h"

#include "genui/StructDocumentActions.hpp"

#include <iostream>

USING_COMMON_NS;

void StructEditor::initViewParam()
{
    const PropertyNode* doc_look =
        config().root()->makeDescendant(DocLook::DOC_LOOK);

    colorSchemeImpl_ = new ColorSchemeImpl();
    viewParam_ = new ViewParam(
        doc_look->getSafeProperty(DocLook::INDENT)->getInt(), 
        getDsi()->getSafeProperty(DocSrcInfo::SHOW_TAGS)->getBool(),
        getDsi()->getSafeProperty(DocSrcInfo::SHOW_PAGINATED)->getBool(),
        colorSchemeImpl_.pointer(),
        doc_look->getSafeProperty(DocLook::SHOW_NBSP)->getBool());
    //! move tagfont and defaultfont to viewParam
    Formatter::FontMgr* font_mgr = EditableView::fontManager();
    if (font_mgr) {
        String tag_font =
            doc_look->getSafeProperty(DocLook::TAG_FONT)->getString();
        if (!tag_font.isEmpty())
            font_mgr->setTagFont(tag_font);
    }

    uiActions().toggleMarkup()->setToggled(viewParam_->showTags_);
}

ColorSchemeImpl::ColorSchemeImpl()
{
    setColors();
}

void ColorSchemeImpl::setColors()
{
    const PropertyNode* doc_look =
        config().root()->makeDescendant(DocLook::DOC_LOOK);

    evenContColor_ = DocLook::parse_rgb_color(
        doc_look->getSafeProperty(DocLook::EVEN_COLOR)->getString());
    oddContColor_ = DocLook::parse_rgb_color(
        doc_look->getSafeProperty(DocLook::ODD_COLOR)->getString());

    const PropertyNode* ptn = doc_look->getProperty(DocLook::NS_COLOR_MAP);
    if (ptn)
        ptn = ptn->firstChild();
    colorMap_.clear();
    for (; ptn; ptn = ptn->nextSibling()) {
        if (ptn->name() != DocLook::NS_COLOR_MAP_ENTRY)
            continue;
        String uri = (ptn->getProperty(DocLook::DEFAULT_NS_COLOR))
            ? "" : ptn->getSafeProperty(DocLook::NS_URI)->getString();
        const PropertyNode* tag_color =
            ptn->getProperty(DocLook::NS_TAG_COLOR);
        const PropertyNode* text_color =
            ptn->getProperty(DocLook::NS_TEXT_COLOR);
        if (uri.isNull() || !tag_color || !text_color)
            continue;
        if (uri.isNull())
            uri = "";
        colorMap_[uri] = std::pair<Rgb, Rgb>(
            DocLook::parse_rgb_color(tag_color->getString()),
            DocLook::parse_rgb_color(text_color->getString()));
    }
}

void ColorSchemeImpl::tagColor(const COMMON_NS::String& nsUri,
                               Formatter::Rgb& tagColor,
                               Formatter::Rgb& textColor) const
{
    ColorMap::const_iterator it = colorMap_.find(nsUri);
    if (it == colorMap_.end())
        it = colorMap_.find("");
    if (it == colorMap_.end()) {
        tagColor  = Rgb(161, 221, 221);
        textColor = Rgb(0, 0, 0);
    }
    else {
        tagColor  = it->second.first;
        textColor = it->second.second;
    }
}
