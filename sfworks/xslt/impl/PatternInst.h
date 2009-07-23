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

#ifndef XSLT_PATTERN_INST_H
#define XSLT_PATTERN_INST_H

#ifdef USE_XPATH3
# include "xslt/impl/PatternInst3.h"
#else
#include "xslt/xslt_defs.h"
#include "xslt/PatternExpr.h"
#include "xslt/impl/xpath_exprinst.h"
#include "xslt/impl/PatternExprImpl.h"

/*!
 */
namespace Xslt {

class PatternInst : public Xpath::ExprInst {
public:
    XSLT_OALLOC(PatternInst);
    PatternInst(const PatternExprImpl& expr,
                const Xpath::NodeSetItem& nsi,
                const Xpath::ExprContext& context)
        : ExprInst(expr, nsi, context) {}

    /// Returns match priority. May be different from priority()
    /// for AltPatternExpr's.
    virtual double matchPriority() const;
    virtual const PatternExprImpl* matchedAltExpr() const;
};

typedef COMMON_NS::SubscriberPtr<PatternInst> PatternInstPtr;

} // namespace Xslt

#endif // USE_XPATH3
#endif // XSLT_PATTERN_INST_H
