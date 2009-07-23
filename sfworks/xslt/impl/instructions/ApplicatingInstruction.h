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

#ifndef XSLT_APPLICATING_INSTRUCTION_H
#define XSLT_APPLICATING_INSTRUCTION_H

#include "xslt/xslt_defs.h"
#include "xpath/Expr.h"

namespace Xslt {

class Instruction;

class ApplicatingInstruction {
public:
    enum SectionProcess {
        UNSPEC_SECTIONS, OMIT_SECTIONS, PRESERVE_SECTIONS, BALANCE_SECTIONS,
        PRESERVE_RIGHT_SECTIONS, PRESERVE_LEFT_SECTIONS
    };
    enum FoldProcess {
        FOLDING_DISABLED = 0, FOLDING_ENABLED, UNSPEC_FOLD
    };
    ApplicatingInstruction(const COMMON_NS::String& sel,
                           const Instruction* icontext);
    virtual ~ApplicatingInstruction() {}

    const Xpath::Expr&  selectExpr() const { return *selectExpr_.pointer(); }
    SectionProcess      sectProcType() const { return (SectionProcess)spc_; }
    FoldProcess         foldProcType() const { return (FoldProcess)canFold_; }
    const COMMON_NS::String& selectString() const { return select_; }
    void                setSpc(const COMMON_NS::String& sect);
    void                setCanFold(const Common::String& foldType);

    PRTTI_DECL(ApplicatingInstruction);

private:
    COMMON_NS::String   select_;
    Xpath::ExprPtr      selectExpr_;
    char                spc_;
    char                canFold_;
};

} // namespace Xslt

#endif // XSLT_APPLICATING_INSTRUCTION_H
