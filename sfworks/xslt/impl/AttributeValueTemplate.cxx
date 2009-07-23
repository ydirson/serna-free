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

#include "xslt/impl/PatternExprImpl.h"
#include "xslt/impl/debug.h"
#include "xslt/impl/AttributeValueTemplate.h"
#include "xslt/impl/Instruction.h"
#include "xslt/impl/xpath_values.h"
#include "xslt/impl/xpath_exprs.h"
#include "xslt/impl/XsltFunctionFactory.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xpath/Engine.h"

USING_COMMON_NS;

namespace Xslt {

AttributeValueTemplate::AttributeValueTemplate(const GroveLib::Attr* origin,
                                        const Xpath::FunctionFactory* fact)
    : origin_(origin),
      name_(origin_->nodeName()),
      value_(origin_->value())
{
    const Char* cp = value_.unicode();
    const Char* ce = cp + value_.length();
    const Char* s = 0;
    Vector<Xpath::ExprImplPtr> exprVec;
    exprVec.reserve(16);
    String ctoken;

    for (;;) {
        if (cp >= ce)
            break;
        if (*cp == '}') {
            ++cp;
            if (cp < ce && *cp == '}') {
                ctoken += *cp++;
                continue;
            }
            throw Xslt::Exception(XsltMessages::attrValTmplBrace);
        }
        if (*cp == '{') {
            ++cp;
            if (cp < ce && *cp == '{') {
                ctoken += *cp++;
                continue;
            }
            // parse AVT here
            if (!ctoken.isEmpty()) {
                exprVec.push_back(new Xpath::ConstExpr
                    (new Xpath::StringValue(ctoken)));
                ctoken = String::null();
            }
            if (cp >= ce)
                break;
            s = cp;
            while (cp < ce && *cp != '}') {
                if (*cp == '\'') {
                    while (cp < ce && *cp != '\'')
                        ++cp;
                    if (cp >= ce)
                        break;
                    ++cp;
                } else if (*cp == '"') {
                    while (cp < ce && *cp != '"')
                        ++cp;
                    if (cp >= ce)
                        break;
                    ++cp;
                } else
                    ++cp;
            }
            try {
                exprVec.push_back(PatternExprImpl::makeExpr(
                    String(s, cp - s), origin, fact));
            }
            catch (Xpath::Exception& e) {
                throw Xslt::Exception(XsltMessages::attrValTmpl, e.what());
            }
            ++cp;
            continue;
        }
        ctoken += *cp++;
    }
    if (!ctoken.isEmpty())
        exprVec.push_back(new Xpath::ConstExpr(new Xpath::StringValue(ctoken)));
    if (exprVec.size() == 0) {
        valueExpr_ = new Xpath::ConstExpr(new Xpath::StringValue(""));
        return;
    }
    if (exprVec.size() == 1) {
        if (exprVec[0]->type() == Xpath::Expr::CONST &&
            static_cast<const Xpath::ConstExpr*>(exprVec[0].pointer())->
                value()->type() == Xpath::Value::STRING) {
                    valueExpr_ = exprVec[0];
                    return;
        }
        valueExpr_ = fact->makeFunction("string", 
            new Xpath::FunctionArgExpr
                (static_cast<Xpath::ExprImpl*>(exprVec[0].pointer()), 0),
                origin_->element());
        return;
    }
    RefCntPtr<Xpath::FunctionArgExpr> args;
    for (int i = exprVec.size() - 1; i >= 0; --i)
        args = new Xpath::FunctionArgExpr
            (static_cast<Xpath::ExprImpl*>(exprVec[i].pointer()),
                args.pointer());
    valueExpr_ = fact->makeFunction("concat",
        args.pointer(), origin_->element());
}

bool AttributeValueTemplate::isAvt() const
{
    return !(valueExpr_->type() == Xpath::Expr::CONST);
}

}

