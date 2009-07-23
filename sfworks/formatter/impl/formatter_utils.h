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

#ifndef FORMATTER_UTILS_H
#define FORMATTER_UTILS_H

#include "common/String.h"
#include "common/RangeString.h"
#include "grove/Node.h"
#include "formatter/formatter_defs.h"
#include "formatter/decls.h"

namespace Formatter {

COMMON_NS::String
get_attr_value(const GroveLib::Node* const node,const COMMON_NS::String& name);

void get_fo_node_treeloc(const GroveLib::Node* foNode,
                         COMMON_NS::TreelocRep& tloc);

bool is_fo_node_less(const GroveLib::Node* foNode1,
                     const GroveLib::Node* foNode2);
enum WsTreatment {
    PRESERVE_WHITESPACE     = 0x0,
    IGNORE_BEFORE_LINEFEED  = 0x1,
    IGNORE_AFTER_LINEFEED   = 0x2,
    IGNORE_SURR_LINEFEED    = 0x3,
    IGNORE_WHITESPACE       = 0x7
};

enum LfTreatment {
    IGNORE_LINEFEED         = 0x0,
    PRESERVE_LINEFEED       = 0x1,
    TREAT_AS_SPACE          = 0x2
};

FORMATTER_EXPIMP ulong
to_stripped(const COMMON_NS::String& original,
            const COMMON_NS::String& stripped, ulong pos);

FORMATTER_EXPIMP ulong
from_stripped(const COMMON_NS::String& original,
              const COMMON_NS::String& stripped, ulong pos);

FORMATTER_EXPIMP COMMON_NS::String
treat_spaces(const COMMON_NS::String& str, WsTreatment wsTreatment,
             LfTreatment lfTreatment, bool wsCollapse, bool visualizeNbsp);

struct TextChunk {
    TextChunk(const COMMON_NS::RangeString& text, CType width)
        : text_(text),
          width_(width) {};

    const COMMON_NS::RangeString text_;
    const CType                  width_;
};

//! Returns text fragment which width doesn`t exeed naxWidth in given font
TextChunk get_text_chunk(const COMMON_NS::RangeString& text,
                         const CType maxWidth,
                         bool maxw, const FontPtr& font);

//! Returns text fragment which width doesn`t exeed naxWidth in given font
TextChunk get_max_text_chunk(const COMMON_NS::RangeString& text,
                             const CType maxWidth, const FontPtr& font);

} // namespace Formatter

#endif // FORMATTER_UTILS_H
