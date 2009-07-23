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
#include "formatter/impl/PropertyParser.h"

namespace Formatter
{

///// Hardcoded functions

const Value::ValueType ParserContext::minMaxArgTypes[] =
    { Value::V_NUMERIC, Value::V_NUMERIC, Value::V_NONE };

bool ParserContext::minValue(const FunctionArgList& al, ValueTypePair& rv,
                             const PropertyContext*, const Allocation&) const
{
    rv.type_ = Value::V_NUMERIC;
    if (al[0].value_.nval_ < al[1].value_.nval_)
        rv.value_.nval_ = al[0].value_.nval_;
    else
        rv.value_.nval_ = al[1].value_.nval_;
    return true;
}

///////////////////////////////////////////////////////////////////////

bool ParserContext::maxValue(const FunctionArgList& al, ValueTypePair& rv,
                             const PropertyContext*, const Allocation&) const
{
    rv.type_ = Value::V_NUMERIC;
    if (al[0].value_.nval_ > al[1].value_.nval_)
        rv.value_.nval_ = al[0].value_.nval_;
    else
        rv.value_.nval_ = al[1].value_.nval_;
    return true;
}

///////////////////////////////////////////////////////////////////////

const Value::ValueType ParserContext::inheritedPropertyArgTypes[] =
    { Value::V_NCNAME, Value::V_NONE };

bool ParserContext::inheritedPropertyValue(const FunctionArgList&,
                                           ValueTypePair&,
                                           const PropertyContext*,
                                           const Allocation&) const
{
    return false;
}

///////////////////////////////////////////////////////////////////////

const Value::ValueType ParserContext::proportionalArgTypes[] =
    { Value::V_NUMERIC, Value::V_NONE };

bool ParserContext::proportionalColumnWidth(const FunctionArgList&,
                                            ValueTypePair&,
                                            const PropertyContext*,
                                            const Allocation&) const
{
    return false;
}

///////////////////////////////////////////////////////////////////////

const Value::ValueType ParserContext::bodyStartArgTypes[] =
    { Value::V_NONE };

bool ParserContext::bodyStart(const FunctionArgList&, ValueTypePair&,
                              const PropertyContext*, const Allocation&) const
{
    return false;
}

///////////////////////////////////////////////////////////////////////

const Value::ValueType ParserContext::labelEndArgTypes[] =
    { Value::V_NONE };

bool ParserContext::labelEnd(const FunctionArgList&, ValueTypePair&,
                             const PropertyContext*, const Allocation&) const
{
    return false;
}

}
