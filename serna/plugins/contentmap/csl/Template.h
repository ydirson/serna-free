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
#ifndef CSL_TEMPLATE_H_
#define CSL_TEMPLATE_H_

#include "csl/Instructions.h"
#include "xslt/PatternExpr.h"

namespace Csl {

class Profile;

class Template : public Common::XListItem<Template> {
public:
    typedef Common::Vector<Common::String> StringVector;
    
    Template(const GroveLib::Element*);
    Template();
    ~Template();
    
    const Xslt::PatternExpr& matchPattern() const { return *matchPattern_; }
    const Common::String&    profiles() const { return profiles_; }
    bool                     doFold() const { return doFold_; }
    const InstructionList&   instructions(bool isClosed) const
        { return isClosed ? (closedInstructions_.firstChild() ?
            closedInstructions_ : instructions_) : instructions_; }
    const Xpath::ExprPtr&    cursorExpr() const { return cursorExpr_; }
    int                      cursorPlacement() const { return cursPlacement_; }
    bool                     mixedOnly() const { return mixedOnly_; }
    uint                     maxLength() const { return maxLength_; }

    const Common::String&    fontStyle() const { return fontStyle_; }
    const Common::String&    fontWeight() const { return fontWeight_; }
    const Common::String&    fontDecoration() const { return fontDecoration_; }
    
    void                     dump() const;

private:
    Template(const Template&);
    Template& operator=(const Template&);
    
    Xslt::PatternExprPtr matchPattern_;
    Common::String       profiles_;
    Xpath::ExprPtr       cursorExpr_;
    int                  cursPlacement_;
    bool                 doFold_;
    bool                 mixedOnly_;
    uint                 maxLength_;
    InstructionList      instructions_;
    InstructionList      closedInstructions_;

    Common::String       fontStyle_;
    Common::String       fontWeight_;
    Common::String       fontDecoration_;
};

class TemplateList : public Common::OwnedXList<Template> {};

} // namespace Csl

#endif // CSL_TEMPLATE_H_

