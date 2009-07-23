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
#include "xslt/xslt_defs.h"
#include "xslt/impl/instructions/CalsTable.h"
#include "xslt/impl/instructions/Template.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/TemplateProvider.h"
#include "xslt/impl/debug.h"

namespace Xslt {

static bool cals_allow_child(Instruction::Type type)
{
    switch (type) {
        case Instruction::IMPORT:
        case Instruction::INCLUDE:
        case Instruction::ATTRIBUTE_SET:
        case Instruction::DECIMAL_FORMAT:
        case Instruction::KEY:
        case Instruction::NAMESPACE_ALIAS:
        case Instruction::OUTPUT:
        case Instruction::PRESERVE_SPACE:
        case Instruction::STRIP_SPACE:
        case Instruction::SORT:
        case Instruction::TEMPLATE:
            return false;

        default :
            return true;
    }
}

CalsTableCell::CalsTableCell(const GroveLib::Element* element,
                             const Stylesheet* context, Instruction* p)
    : Instruction(element, context, p)
{
    buildSubInstructions();
}

bool CalsTableCell::allowsChild(Type t) const
{
    return cals_allow_child(t);
}

void CalsTableCell::dump() const
{
    DDBG << "CalsTableCell:\n";
    Instruction::dump();
}

/////////////////////////////////////////////////////////////////////

CalsTableRow::CalsTableRow(const GroveLib::Element* element,
                           const Stylesheet* context, Instruction* p)
    : Instruction(element, context, p),
      emptyCellTemplate_(0)
{
    buildSubInstructions();
}

const Instruction* CalsTableRow::emptyCellTemplate() const
{
    if (!emptyCellTemplate_) {
        emptyCellTemplate_ = styleContext()->templateProvider()->
            getTemplate("cals-table-empty-cell");
        if (0 == emptyCellTemplate_)
            throw Xslt::Exception(XsltMessages::instrCalsTable,
                                  contextString());
    }
    return emptyCellTemplate_;
}

bool CalsTableRow::allowsChild(Type t) const
{
    return cals_allow_child(t);
}

void CalsTableRow::dump() const
{
    DDBG << "CalsTableRow:\n";
    Instruction::dump();
}

/////////////////////////////////////////////////////////////////////

CalsTableGroup::CalsTableGroup(const GroveLib::Element* element,
                               const Stylesheet* context, Instruction* p)
    : Instruction(element, context, p)
{
    isDita_ = attr("mode", false) == "dita";
    buildSubInstructions();
}

bool CalsTableGroup::allowsChild(Type t) const
{
    return cals_allow_child(t);
}

void CalsTableGroup::dump() const
{
    DDBG << "CalsTableGroup:\n";
    Instruction::dump();
}

/////////////////////////////////////////////////////////////////////

} // namespace Xslt
