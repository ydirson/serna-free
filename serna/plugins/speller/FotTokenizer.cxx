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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "speller_debug.h"
#include "FotTokenizer.h"
#include "editableview/EditableView.h"
#include "grove/Grove.h"
#include "grove/Nodes.h"
#include "groveeditor/GrovePos.h"
#include "common/String.h"
#include "common/StringCvt.h"
#include "common/XTreeIterator.h"
#include "common/WordTokenizer.h"
#include "xslt/ResultOrigin.h"
#include "utils/DocSrcInfo.h"

using namespace Common;
using namespace GroveLib;
using std::endl;

bool TextSrcNodeIterator::isSrcText() const
{
    Node* np = it_.node();
    if (np->nodeType() != Node::TEXT_NODE)
        return false;
    Node* origin = Xslt::resultOrigin(np);
    if (!origin)
        return false;
    return origin->grove() == grove_;
}

FotTokenizer::FotTokenizer(const PropertyNode* dsi,
                           EditableView* evp,
                           GroveLib::Grove* fd,
                           const GroveLib::Grove* srcGrove)
 :  evp_(evp), fot_(fd),
    cur_node_(0, srcGrove),
    em_(new ElementSkipper(dsi)),
    is_lang_changed_(false)
{
    DDBG << "FotTokenizer(), this:" << abr(this) << ",evp_:" << abr(evp_)
         << ", fot_:" << abr(fot_) << endl;
}

FotTokenizer::~FotTokenizer()
{
    DDBG << "~FotTokenizer(), this:" << abr(this) << endl;
}

static bool lookup_lang(const Node* np, String& old_lang)
{
    String lang = get_lang(np);
    if (lang.isEmpty() && !old_lang.empty()) {
        DDBG << "lang changed from " << sqt(old_lang) << " to "
             << "default" << endl;
        old_lang.resize(0);
        return true;
    }
    if (lang != old_lang) {
        DDBG << "lang changed from " << sqt(old_lang) << " to "
             << sqt(lang) << endl;
        old_lang = lang;
        return true;
    }
    return false;
}

void FotTokenizer::resetRange()
{
    const Text& tn = *cur_node_;
    const Char* p(0 == &tn ? 0 : tn.data().data());
    range_ = RangeString(p, p);
}

RangeString FotTokenizer::getWord()
{
    if (0 == &*cur_node_) 
        return range_ = RangeString();
    do {
        WordTokenizer wtok(range_.end(), cur_node_->data().end());
        if (wtok.next(range_)) 
            return range_;
        ++cur_node_;
        while (&*cur_node_ && em_->mustSkip(&*cur_node_))
            ++cur_node_;
        DDBG << "FotTokenizer: Next node:" << abr(&*cur_node_) << ", type: "
             << (&*cur_node_ ? cur_node_->nodeType() : -1) << std::endl;
        is_lang_changed_ = lookup_lang(&*cur_node_, lang_);
        resetRange();
    } while (&*cur_node_);
    return range_ = RangeString();
}

using GroveEditor::GrovePos;

void FotTokenizer::getPosRange(GrovePos& begin, GrovePos& end)
{
    const String& txt = cur_node_->data();
    const unsigned start(range_.data() - str_data(txt));
    begin = GrovePos(&*cur_node_, start);
    end = GrovePos(&*cur_node_, start + range_.size());
}

void FotTokenizer::rewind()
{
    range_.second = range_.begin();
}

bool FotTokenizer::isLanguageChanged() const
{
    bool rv(is_lang_changed_);
    is_lang_changed_ = false;
    return rv;
}

const String& FotTokenizer::getCurrentLanguage() const { return lang_; }

void FotTokenizer::sync()
{
    DDBG << "FotTokenizer::sync(), this:" << abr(this) << std::endl;
    // get current cursor position

    if (evp_->context().srcPos().isNull())
        return;

    const GroveEditor::GrovePos* gp = 0;
    GrovePos sel_start_pos;
    unsigned pos = 0;

    const Formatter::TreeSelection& selection = evp_->getSelection().tree_;
    if (!selection.isEmpty()) {
        const Formatter::TreePos& min_loc = selection.minLoc();
        Formatter::AreaPos min_area_pos(min_loc.toAreaPos(evp_->rootArea()));
        sel_start_pos = EditContext::getFoPos(min_area_pos);
        if (!sel_start_pos.isNull()) {
            gp = &sel_start_pos;
            pos = EditContext::getSrcPos(min_area_pos, false).idx();
        }
        DDBG << "selStartPos.node() = " << sel_start_pos.node() << std::endl;
    }
    if (0 == gp) {
        gp = &(evp_->context().foPos());
        pos = evp_->context().srcPos().idx();
        DDBG << "gp was 0, now = " << gp << ", pos = " << pos << std::endl;
    }

    Node* cn(gp->isNull() ? fot_->document()->documentElement() : gp->node());

    // this automatically repositions cur_node_ to the nearest text node
    cur_node_ = cn;
    if (&*cur_node_ == cn) {
        if (0 != &*cur_node_ && cur_node_->data().size() == pos) {
            // we're at the end of a text node
            ++cur_node_;
            pos = 0;
        }
    }
    DDBG << "cur_node_:" << abr(&*cur_node_) << ", type: "
         << (&*cur_node_ ? cur_node_->nodeType() : -1) << std::endl;
    if (0 == &*cur_node_) {
        is_lang_changed_ = !lang_.empty();
        lang_.resize(0);
        range_ = RangeString();
        return;
    }
    is_lang_changed_ = lookup_lang(&*cur_node_, lang_);
    const Char* ptext = cur_node_->data().unicode();
    DDBG << "stext:" << sqt(cur_node_->data()) << std::endl;
    while (0 < pos && WordTokenizer::tokenChar(ptext[pos]))
        --pos;
    ptext += pos;
    range_ = RangeString(ptext, ptext);
}

void FotTokenizer::advance(unsigned offset)
{
    range_.second = range_.begin() + offset;
}

void FotTokenizer::skipToNextLanguage()
{
    for (++cur_node_; 0 != &*cur_node_; ++cur_node_)
        if ((is_lang_changed_ = lookup_lang(&*cur_node_, lang_)))
            break;
    resetRange();
}

void FotTokenizer::skipToNextElement()
{
    Node* parent(parentNode(&*cur_node_));
    for (++cur_node_; 0 != &*cur_node_; ++cur_node_)
        if (parentNode(&*cur_node_) != parent)
            break;
    resetRange();
}

