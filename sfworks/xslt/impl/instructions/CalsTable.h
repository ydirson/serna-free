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
#ifndef XSLT_CALS_TABLE_H
#define XSLT_CALS_TABLE_H

#include "xslt/xslt_defs.h"
#include "xpath/Expr.h"
#include "xslt/impl/Instruction.h"

namespace Xslt {

class Template;

class CalsTableCell : public Instruction {
public:
    XSLT_OALLOC(CalsTableCell);

    CalsTableCell(const GroveLib::Element* element,
                  const Stylesheet* context, Instruction* p);
    virtual ~CalsTableCell() {}

    virtual Type        type() const { return CALS_TABLE_CELL; }
    virtual Instance*   makeInst(const InstanceInit& init, Instance* p) const;
    virtual void        dump() const;

private:
    virtual bool        allowsChild(Type) const;
};

class CalsTableRow : public Instruction {
public:
    XSLT_OALLOC(CalsTableRow);

    CalsTableRow(const GroveLib::Element* element,
                  const Stylesheet* context, Instruction* p);
    virtual ~CalsTableRow() {}

    virtual Type        type() const { return CALS_TABLE_ROW; }
    virtual Instance*   makeInst(const InstanceInit& init, Instance* p) const;
    virtual void        dump() const;
    const Instruction*  emptyCellTemplate() const;

private:
    virtual bool        allowsChild(Type) const;
    mutable const Template* emptyCellTemplate_;
};

class CalsTableGroup : public Instruction {
public:
    XSLT_OALLOC(CalsTableGroup);

    CalsTableGroup(const GroveLib::Element* element,
                  const Stylesheet* context, Instruction* p);
    virtual ~CalsTableGroup() {}

    virtual Type        type() const { return CALS_TABLE_GROUP; }
    virtual Instance*   makeInst(const InstanceInit& init, Instance* p) const;
    virtual void        dump() const;
    bool                isDita() const { return isDita_; }

private:
    virtual bool        allowsChild(Type) const;
    bool isDita_;
};

} // namespace Xslt

#endif // XSLT_CALS_TABLE_H

