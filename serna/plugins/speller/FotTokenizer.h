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

/** \file
 */

#ifndef FOT_TOKENIZER_H_
#define FOT_TOKENIZER_H_

#include "common/StringUtils.h"
#include "common/XTreeIterator.h"
#include "common/OwnerPtr.h"
#include "grove/Grove.h"
#include "grove/ElementMatcher.h"
#include "common/PropertyTree.h"
#include "groveeditor/GrovePos.h"

class EditableView;

struct TextSrcNodeIterator {
    typedef GroveLib::Node Node;
    typedef GroveLib::Text Text;

    TextSrcNodeIterator(Node* np, const GroveLib::Grove* grove)
        : it_(np), grove_(grove)
    { reset(); }
    TextSrcNodeIterator& operator=(Node* np)
    {
        it_ = base_type(np);
        reset();
        return *this;
    }

    Text*    operator->() const { return static_cast<Text*>(it_.node()); }
    Text&    operator*() const { return *static_cast<Text*>(it_.node()); }

    TextSrcNodeIterator& operator++()
    {
        for (++it_; 0 != it_.node() && !isSrcText(); ++it_);
        return *this;
    }
    TextSrcNodeIterator& operator--()
    {
        for (--it_; 0 != it_.node() && !isSrcText(); --it_);
        return *this;
    }
private:
    typedef Common::XTreeDocOrderIterator<Node> base_type;
    bool isSrcText() const;
    void reset()
    {
        if (0 != it_.node() && !isSrcText())
            ++(*this);
    }
    base_type it_;
    const GroveLib::Grove* grove_;
};

class FotTokenizer {
public:
    FotTokenizer(const Common::PropertyNode* dsi,
                 EditableView* evp,
                 GroveLib::Grove* fd, const GroveLib::Grove* srcGrove);
    virtual ~FotTokenizer();
    //!
    virtual Common::UCRange getWord();
    //! fill begin & end with positions of start and end of current word
    void getPosRange(GroveEditor::GrovePos& begin, GroveEditor::GrovePos& end);
    //! reposition tokenizer to the cursor position
    void sync();
    //! reposition tokenizer so that next call to getWord() returns current word
    void rewind();
    //! advance input position within current text node
    void advance(unsigned offset);
    //!
    virtual const COMMON_NS::String& getCurrentLanguage() const;
    //!
    virtual bool isLanguageChanged() const;
    //!
    virtual void skipToNextLanguage();
    //!
    virtual void skipToNextElement();
    //!
private:
    EditableView*       evp_;
    GroveLib::Grove*    fot_;
    TextSrcNodeIterator cur_node_;
    Common::UCRange     range_;
    Common::String      lang_;
    Common::OwnerPtr<GroveLib::ElementMatcher> em_;
    mutable bool        is_lang_changed_;
};

#endif // FOT_TOKENIZER_H_
