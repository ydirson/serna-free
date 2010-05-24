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

#include "formatter/impl/SimpleFormFos.h"
#include "formatter/impl/Areas.h"

using namespace Common;
using namespace GroveLib;

namespace Formatter {

/////////////////////////////////////////////////////////////////////////////

class IsEditable : public BooleanProperty {
public:
    FORMATTER_OALLOC(IsEditable);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = false; }
private:
    static CString  name_;
};

String IsEditable::name_ = "is-editable";

/////////////////////////////////////////////////////////////////////////////

class IsEnabled : public BooleanProperty {
public:
    FORMATTER_OALLOC(IsEnabled);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static  bool            isInheritable() { return false; }

    virtual void    init(ParserContext*, const Allocation&,
                         const CType&) { value_ = true; }
private:
    static CString  name_;
};

String IsEnabled::name_ = "is-enabled";

/////////////////////////////////////////////////////////////////////////////

class ComboBoxValue : public LiteralProperty {
public:
    FORMATTER_OALLOC(ComboBoxValue);

    const CString&  propertyName() const { return name_; }

    static const CString&   name() { return name_; }
    static bool             isInheritable() { return false; }

    virtual void    init(ParserContext*, const Allocation&, const CType&)
    {
        value_ = String::null();
    }
    
private:
    static CString  name_;
};

String ComboBoxValue::name_ = "value";

/////////////////////////////////////////////////////////////////////////////

void SimpleFormFo::calcProperties(const Allocation& alloc)
{
    DBG(XSL.PROPDYN) << "Calc properties:" << this << std::endl;

    contColor_ = Rgb(0, 0, 0, true);
    textColor_ = getProperty<Color>(alloc).value();

    isEnabled_ = getProperty<IsEnabled>(alloc).value();
    width_ = getProperty<Width>(alloc).value();
    font_ = data_.fontMgr_->getFont(getProperty<FontFamily>(alloc).value(),
                                    getProperty<FontStyle>(alloc).value(),
                                    getProperty<FontWeight>(alloc).value(),
                                    getProperty<TextDecoration>(alloc).value(),
                                    getProperty<FontSize>(alloc).pointSize());
}

bool SimpleFormFo::isChainFinishedAt(const AreaPtr& area) const
{
    return !area.isNull();
}

bool SimpleFormFo::hasContent(const AreaPtr& area) const
{
    return !area.isNull();
}

void SimpleFormFo::attributeChanged(const Attr* attr)
{
    ReferencedFo::attributeChanged(attr);
}

void SimpleFormFo::attributeRemoved(const Element* elem, const Attr* attr)
{
    ReferencedFo::attributeRemoved(elem, attr);
}

void SimpleFormFo::attributeAdded(const Attr* attr)
{
    ReferencedFo::attributeAdded(attr);
}

/////////////////////////////////////////////////////////////////////////////

ComboBoxFo::ComboBoxFo(const FoInit& init)
    : SimpleFormFo(init),
      accender_(0),
      descender_(0)
{
}

void ComboBoxFo::calcProperties(const Allocation& alloc)
{
    SimpleFormFo::calcProperties(alloc);
    
    isEditable_ = getProperty<IsEditable>(alloc).value();
    valueList_.clear();
    for (Node* c = node()->firstChild(); c; c = c->nextSibling()) {
        if (Node::ELEMENT_NODE != c->nodeType() || "se:value" != c->nodeName())
            continue;
        c->registerNodeVisitor(this, NOTIFY_CHILD_INSERTED|
            NOTIFY_CHILD_REMOVED);
        String value;
        for (Node* txt = c->firstChild(); txt; txt = txt->nextSibling()) {
            if (Node::TEXT_NODE != txt->nodeType())
                continue;
            value += static_cast<Text*>(txt)->data();
            txt->registerNodeVisitor(this, NOTIFY_TEXT_CHANGED);
        }
        valueList_.push_back(value);
    }
    text_ = getProperty<ComboBoxValue>(alloc).value();
    accender_ = font_->accender() + 3;
    descender_ = font_->descender() + 3;
    trackChildren_ = true;
}

bool ComboBoxFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    return ((alloc.maxh_ ||
             alloc.space_.extent_.h_ > accender_ + descender_) &&
            (alloc.maxw_ || alloc.space_.extent_.w_ >= width_));
}

Area* ComboBoxFo::makeArea(const Allocation& alloc, const Area* after,
                           bool forceToMake) const
{
    forceToMake |= alloc.maxw_;

    if (!forceToMake && !isEnoughSpace(alloc, after))
        return 0;

    ComboBoxArea* combo_box = 
        new ComboBoxArea(this, text_, valueList_, isEditable_, isEnabled_);

    combo_box->allcPoint_ = alloc.space_.origin_;
    combo_box->base_ = accender_;
    combo_box->contRange_.h_ = accender_ + descender_;
    combo_box->contRange_.w_ = width_;

    return combo_box;
}

void ComboBoxFo::childInserted(const Node* node)
{
    if ("se:value" == node->nodeName()) {
        PropertySet::setModified();
        registerModification(THIS_FO_MODIFIED);
    }
}

void ComboBoxFo::childRemoved(const Node*, const Node* node)
{
    if ("se:value" == node->nodeName())  {
        PropertySet::setModified();
        registerModification(THIS_FO_MODIFIED);
    }
}

void ComboBoxFo::textChanged(const Text* text) 
{
    if (text->parent() && "se:value" == text->parent()->nodeName()) {
        PropertySet::setModified();
        registerModification(THIS_FO_MODIFIED);
    }
}

String ComboBoxFo::name() const
{
    return "ComboBoxFo";
}

/////////////////////////////////////////////////////////////////////////////

LineEditFo::LineEditFo(const FoInit& init)
    : SimpleFormFo(init),
      accender_(0),
      descender_(0)
{
}

void LineEditFo::calcProperties(const Allocation& alloc)
{
    SimpleFormFo::calcProperties(alloc);
    text_ = getProperty<ComboBoxValue>(alloc).value();
    accender_ = font_->accender() + 3;
    descender_ = font_->descender() + 3;
}

bool LineEditFo::isEnoughSpace(const Allocation& alloc, const Area*) const
{
    return ((alloc.maxh_ ||
             alloc.space_.extent_.h_ > accender_ + descender_) &&
            (alloc.maxw_ || alloc.space_.extent_.w_ >= width_));
}

Area* LineEditFo::makeArea(const Allocation& alloc, const Area* after,
                           bool forceToMake) const
{
    forceToMake |= alloc.maxw_;

    if (!forceToMake && !isEnoughSpace(alloc, after))
        return 0;

    LineEditArea* line_edit = new LineEditArea(this, text_, isEnabled_);

    line_edit->allcPoint_ = alloc.space_.origin_;
    line_edit->base_ = accender_;
    line_edit->contRange_.h_ = accender_ + descender_;
    line_edit->contRange_.w_ = width_;

    return line_edit;
}

String LineEditFo::name() const
{
    return "LineEditFo";
}

} // namespace Formatter
