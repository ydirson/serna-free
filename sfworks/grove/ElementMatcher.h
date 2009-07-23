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
#ifndef GROVE_ELEMENT_MATCHER_H_
#define GROVE_ELEMENT_MATCHER_H_

#include "grove/grove_defs.h"
#include "grove/Decls.h"
#include "common/OwnerPtr.h"
#include "common/String.h"

namespace GroveLib {

/*! Match an element or attribute against a black/white list of patterns.
    Pattern grammar:
        pattern:        norm_pattern | '!' norm_pattern ; { negation }
        norm_pattern:   elem_pattern                      { element path }
                        | elem_pattern '/' '@' QNAME      { path/@attr }
                        | '@' QNAME ;                     { @attr }
        elem_pattern:   pattern_token | pattern_token '/' elem_pattern ;
        pattern_token:  QNAME | '*' ;

    Matching precedence rules:
    - rule precedence is calculated by adding precedences of matched
      path components:
        QNAME has precedence 1
        *     has precedence 0
      (as a result, longest exact matches have the higher precedence)
    - non-negating rules have precedence over negating rules when their
      calculated precedences are equal.
*/
class GROVE_EXPIMP ElementMatcher {
public:
    class PatternNode;

    ElementMatcher(const Common::String& pattern = Common::String());
    ~ElementMatcher();

    /// Add more pattern(s). Multiple patterns can be specified in the
    /// single line; they should be separated with a whitespace.
    void    addPattern(const Common::String& pattern);

    bool    matchElement(const Element* elem) const;
    bool    matchElement(const Element* parent,
                         const Common::String& elemQname) const;
    bool    matchAttr(const Attr* attr) const;
    bool    matchAttr(const Element* parent,
                      const Common::String& attrQname) const;

    void    dump() const;

private:
    Common::OwnerPtr<PatternNode> elemsEnable_;
    Common::OwnerPtr<PatternNode> elemsDisable_;
    Common::OwnerPtr<PatternNode> attrsEnable_;
    Common::OwnerPtr<PatternNode> attrsDisable_;
};

} // namespace GroveLib

#endif // GROVE_ELEMENT_MATCHER_H_
