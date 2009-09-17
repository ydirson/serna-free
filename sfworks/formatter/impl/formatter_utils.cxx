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

#include "grove/Nodes.h"
#include "grove/EntityReferenceTable.h"
#include "xslt/Result.h"

#include "formatter/Font.h"
#include "formatter/impl/formatter_utils.h"
#include "formatter/impl/debug.h"

USING_GROVE_NAMESPACE;
USING_COMMON_NS;

namespace Formatter {

String get_attr_value(const Node* const node, const String& name)
{
    if (0 == node || Node::ELEMENT_NODE != node->nodeType())
        RT_MSG_ASSERT(false, "Attempt to get attribute from nonelement");

    Attr* attr = static_cast<const Element*>(node)->attrs().getAttribute(name);
    if (attr)
        return attr->value();
    return String::null();
}

void get_fo_node_treeloc(const Node* foNode, COMMON_NS::TreelocRep& tloc)
{
    RT_ASSERT(foNode);
    const Xslt::XsltResult* xslt_result = foNode->asConstXsltResult();
    RT_ASSERT(xslt_result);
    xslt_result->getTreeloc(tloc);
}

bool is_fo_node_less(const GroveLib::Node* foNode1,
                     const GroveLib::Node* foNode2)
{
    return foNode1->comparePos(foNode2) == LESS;
}

static const unsigned short NBSP_CHAR   = 0xA0;
static const unsigned short NBSP_VISUAL = 0xB0;

enum TokenType {
    NONE                = 0x0,
    SPACE               = 0x1,
    LINEFEED            = 0x2,
    LINEFEED_AS_SPACE   = 0x3,
    TEXT                = 0x4,
    NBSP                = 0x8,
    ANY                 = 0xF
};

// used to speedup calls to routines which consume qstrings, such as
// font->width()
static inline String mid_qstr(const String& str, uint from, uint len)
{
    if (str.length() < (from + len))
        len = str.length() - from;     
    return String(str.unicode() + from, len, true);
}

static inline TokenType token_type(const Char& c)
{
    switch (c.unicode()) {
        case ' ':       return SPACE;
        case '\n':      return LINEFEED;
        case NBSP_CHAR: return NBSP;
        default:        return TEXT;
    }
}

ulong convert_stripped(const String& original, const String& stripped,
                       ulong pos, bool fromStripped)
{
    if (fromStripped)
        if (pos >= stripped.length())
            pos = stripped.length();
    else
        if (pos >= original.length())
            pos = original.length();

    const ulong s_max = (fromStripped) ? pos : stripped.length();
    const ulong o_max = (fromStripped) ? original.length() : pos;

    ulong o_pos = 0;
    ulong s_pos = 0;

    while (s_pos <= s_max) {
        const Char& s_char = stripped[s_pos];
        TokenType type = token_type(s_char);
        for (;;) {
            const Char& o_char = original[o_pos];
            if ((o_char == s_char) ||
                (SPACE == type && (' ' == o_char || '\n' == o_char)) ||
                (o_char.unicode() == NBSP_CHAR &&
                 s_char.unicode() == NBSP_VISUAL))
                    break;
            if (o_pos < o_max)
                ++o_pos;
            else
                return (fromStripped) ? o_pos : s_pos;
        }
        if (o_pos >= o_max)
            return (fromStripped) ? o_pos : s_pos;
        ++s_pos;
        if (s_pos <= s_max)
            ++o_pos;
    }
    return (fromStripped) ? o_pos : s_pos;
}

ulong to_stripped(const String& original, const String& stripped, ulong pos)
{
    return convert_stripped(original, stripped, pos, false);
}

ulong from_stripped(const String& original, const String& stripped, ulong pos)
{
    return convert_stripped(original, stripped, pos, true);
}

/*! Splits text string to tokens for shitespace stripping
 */
class TokenIter {
public:
    TokenIter(const String& string)
        : text_(string),
          len_(text_.length()),
          from_(0),
          to_(0),
          type_(NONE)
        {
        }
    inline bool         isEmpty() const { return (from_ == to_); }
    inline String       text() const { return text_.mid(from_, to_ - from_); }
    inline TokenType    type() const { return type_; }
    inline TokenIter&   operator++()
        {
            if (to_ < len_) {
                type_ = token_type(text_[to_]);
                const int look_for = (LINEFEED == type_) ? ANY : ANY & ~type_;
                from_ = to_;
                for (++to_; to_ < len_; ++to_)
                    if (look_for & token_type(text_[to_]))
                        break;
            }
            else {
                from_ = to_;
                type_ = NONE;
            }
            return *this;
        }
private:
    const String&   text_;
    const ulong     len_;
    ulong           from_;
    ulong           to_;
    TokenType       type_;
};

inline void add_whitespace(String& result, String& whitespace,
                           String& prev_token, TokenType& prev_token_type,
                           bool collapse)
{
    RT_MSG_ASSERT((SPACE & prev_token_type),
                  "Previous token is not whitespace");
    result += (collapse) ? String(' ') : whitespace + prev_token;
    whitespace = prev_token = String::null();
    prev_token_type = NONE;
}

/* Does whitespace stripping and linefeed treatment
 */
String treat_spaces(const String& str, WsTreatment wsTreatment,
                    LfTreatment lfTreatment, bool wsCollapse,
                    bool visualizeNbsp)
{
    DBG(XSL.TEST) << "Stripping wsTreatment:" << wsTreatment
                  << " lfTreatment:" << lfTreatment << " wsCollapse:"
                  << wsCollapse << std::endl;
    if (str.isEmpty())        
        return str;

    String      stripped;
    TokenIter   token(str);
    String      prev_token;
    TokenType   prev_token_type;
    String      whitespace;
    bool        after_linefeed = false;

    stripped.reserve(str.length());
    for (++token; !token.isEmpty(); ++token) {
        DBG_IF(XSL.TEST) {
            if (LINEFEED == token.type())
                DBG(XSL.TEST) << "LINEFEED";
            else
                DBG(XSL.TEST) << '[' << token.text() << ']';
        }
        switch (token.type()) {
            case SPACE :
                //! Do white-space-treatment
                if ((IGNORE_WHITESPACE == wsTreatment) ||
                    (after_linefeed && IGNORE_AFTER_LINEFEED & wsTreatment)) {
                    DBG(XSL.TEST) << " IGNORED;" << std::endl;
                    after_linefeed = false;
                    continue;
                }
                after_linefeed = false;
                if (!prev_token.isEmpty()) {
                    RT_MSG_ASSERT((SPACE & prev_token_type),
                                  "Previous token is not whitespace");
                    whitespace += prev_token;
                }
                prev_token = token.text();
                prev_token_type = SPACE;
                break;
            case LINEFEED :
                after_linefeed = true;
                //! Do white-space-treatment
                if (!prev_token.isEmpty() && (SPACE == prev_token_type) &&
                    (IGNORE_BEFORE_LINEFEED & wsTreatment)) {
                    DBG(XSL.TEST) << " prev token IGNORED;";
                    prev_token = String::null();
                }
                //! Do linefeed-treatment
                switch (lfTreatment) {
                    case PRESERVE_LINEFEED :
                        if (!prev_token.isEmpty())
                            add_whitespace(stripped, whitespace, prev_token,
                                           prev_token_type, wsCollapse);
                        stripped += token.text();
                        break;
                    case TREAT_AS_SPACE :
                        if (!prev_token.isEmpty()) {
                            RT_MSG_ASSERT((SPACE & prev_token_type),
                                          "Previous token is not whitespace");
                            whitespace += prev_token;
                        }
                        prev_token = ' ';
                        prev_token_type = LINEFEED_AS_SPACE;
                        DBG(XSL.TEST) << " linefeed TREATED AS SPACE";
                        break;
                    default:
                        DBG(XSL.TEST) << " IGNORED";
                        break;
                }
                break;
            case NBSP:
                after_linefeed = false;
                if (!prev_token.isEmpty())
                    add_whitespace(stripped, whitespace, prev_token,
                                   prev_token_type, wsCollapse);
                if (visualizeNbsp) {
                    for (uint i = token.text().length(); i > 0; --i)
                        stripped += Char(NBSP_VISUAL);
                } else 
                    stripped += token.text();
                break;
            default:
                after_linefeed = false;
                if (!prev_token.isEmpty())
                    add_whitespace(stripped, whitespace, prev_token,
                                   prev_token_type, wsCollapse);
                stripped += token.text();
                break;
        }
        DBG(XSL.TEST) << std::endl;
    }
    if (!prev_token.isEmpty())
        add_whitespace(stripped, whitespace, prev_token,
                       prev_token_type, wsCollapse);
    return stripped;
}

struct ChunkSpecs {
public:
    ChunkSpecs(ulong length, CType width)
        : length_(length),
          width_(width) {};

    ulong   length_;
    CType   width_;
};

//! Binary search of text position to split text into chunks
static inline ChunkSpecs 
find_chunk_in(const RangeString& text, ulong initMin, ulong initMax,
              CType initWidth, const CType maxWidth,
              const FontPtr& font, bool addSpace)
{
    ulong min = initMin;
    ulong max = initMax;
    CType width = initWidth;
    while (max != min) {
        ulong mid = min + (max - min) / 2;
        if (mid == min)
            mid = max;
        CType mid_width;
        if (mid > min) {
            RangeString mid_str = text.mid(min, mid - min);
            mid_width = font->width(mid_str);
            if (addSpace && mid_str.end() == text.end())
                mid_width += font->spaceWidth();
        } else
            mid_width = 0;
        const CType tmp_width = mid_width + width;

        if (tmp_width == maxWidth) {
            max = mid;
            width = tmp_width;
            break;
        }
        if (tmp_width > maxWidth)
            max = mid - 1;
        else {
            min = mid;
            width = tmp_width;
        }
    }
    return ChunkSpecs(max, width);
}

const ulong LARGE_STRING = 100;

/*! Long string optimization for the search:
    makes the best guessing for binary search (above)
 */
static ChunkSpecs
find_long_chunk(const RangeString& text, const CType maxWidth,
                const FontPtr& font, bool addSpace)
{
    const ulong len = text.length() + addSpace;
    if (len < LARGE_STRING) {
        ulong width = font->width(text);
        if (addSpace)
            width += font->spaceWidth();
        if (maxWidth >= width)
            return ChunkSpecs(len, width);
        else
            return find_chunk_in(text, 0, len, 0, maxWidth, font, addSpace);
    }
    else {
        ulong min = 0;
        ulong max = LARGE_STRING;
        CType width = 0;
        RangeString mid = text.mid(min, max - min);
        CType width_inc = font->width(mid);
        if (addSpace && mid.end() == text.end())
            width_inc += font->spaceWidth();
        while (max < len && width + width_inc <= maxWidth) {
            min = max;
            max = (max * 2 > len) ? len : max * 2;
            width += width_inc;
            mid = text.mid(min, max - min);
            width_inc = font->width(mid);
            if (addSpace && mid.end() == text.end())
                width_inc += font->spaceWidth();
        }
        return find_chunk_in(text, min, max, width, maxWidth, font, addSpace);
    }
}

static inline ChunkSpecs 
find_stripped_chunk(const RangeString& rtext, const CType maxWidth,
                    const FontPtr& font)
{
    long max_len = rtext.find('\n');
    return (-1 != max_len) 
        ? find_long_chunk(rtext.left(max_len), maxWidth, font, true)
        : find_long_chunk(rtext, maxWidth, font, false);
}

TextChunk get_text_chunk(const RangeString& rtext, const CType maxWidth,
                         bool maxw, const FontPtr& font)
{
    ChunkSpecs chunk = find_stripped_chunk(rtext, maxWidth, font);
    //std::cerr << "w:" << maxWidth << '/' << chunk.width_
    //          << " l:" << text.length() << '/' << chunk.length_ << std::endl;

    //! Try not to split the word at the end of the chunk
    if (chunk.length_ > 0 && chunk.length_ < rtext.length()) {
        if ('\n' == rtext[chunk.length_ - 1])
            return TextChunk(rtext.left(chunk.length_), chunk.width_);

        if ('\n' != rtext[chunk.length_ - 1]) {
            //! Keep with next space
            Char next(rtext[chunk.length_]);
            if (' ' == next)
                return TextChunk(rtext.left(chunk.length_ + 1),
                                 chunk.width_ + font->spaceWidth());
        }
        //! Or find the prev space if any
        long last_space = ('\n' == rtext[chunk.length_ - 1])
            ? -1 : rtext.findRev(' ', chunk.length_ - 1);
        if (-1 != last_space) {
            chunk.length_ = last_space + 1;
            chunk.width_ = font->width(RangeString(rtext.unicode(),
                chunk.length_));
        }
        else
            if (!maxw)
                return TextChunk(RangeString(), 0);
    }
    return TextChunk(rtext.left(chunk.length_), chunk.width_);
}

TextChunk get_max_text_chunk(const RangeString& text, const CType maxWidth,
                             const FontPtr& font)
{
    ChunkSpecs chunk = find_stripped_chunk(text, maxWidth, font);
    return TextChunk(text.left(chunk.length_), chunk.width_);
}

/////////////////////////////////////////////

int Font::space_width() const
{
    return width(String(" "));
}

} // namespace Formatter
