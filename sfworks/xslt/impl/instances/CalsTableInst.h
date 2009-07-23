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
#ifndef XSLT_CALS_TABLE_INST_H
#define XSLT_CALS_TABLE_INST_H

#include "xslt/xslt_defs.h"
#include "xpath/ExprContext.h"
#include "xpath/NodeSet.h"
#include "grove/NodeVisitor.h"
#include "common/SubscriberPtr.h"

namespace Xslt {

class ResultContext;
class CalsTableCellInst;
class CalsTableGroupInst;

class CalsTableRowInst : public Instance,
                         public Common::XListItem<CalsTableRowInst> {
public:
    typedef Common::XListItem<CalsTableRowInst> RowItem;
    typedef Common::Vector<CalsTableCellInst*> Cells;

    CalsTableRowInst(const CalsTableRow* instr,
                     const InstanceInit& init, Instance* p);
    virtual ~CalsTableRowInst();

    bool                buildInProcess() const { return buildInProcess_; }
    void                build(const ResultContext*);

    int                 addCell(CalsTableCellInst*);
    void                removeCell(CalsTableCellInst* cell);

    const Instruction*  emptyCellTemplate();
    void                prepareRemove();
    CalsTableGroupInst* tgroup() const { return tgroupInst_; }
    int                 cellIndex(const CalsTableCellInst* cell) const;
    int                 maxSpan() const;

    // Reimplemented from Instance
    virtual void        update();
    virtual void        dump() const;
    virtual void        updateContext(int);

private:
    void                notifyNextRows(int cnt);
    void                setCell(uint, CalsTableCellInst*);
    void                resizeCells(uint newsize);

    CalsTableGroupInst* tgroupInst_;
    Cells               cells_;
    bool                buildInProcess_;
};

class CalsTableCellInst : public Instance,
                          public GroveLib::NodeVisitor,
                          public Common::SubscriberPtrWatcher {
public:
    XSLT_OALLOC(CalsTableCellInst);

    CalsTableCellInst(const CalsTableCell* instr,
                      const InstanceInit& init, Instance* p);
    virtual ~CalsTableCellInst();

    int     spanX() const { return spanX_; }
    int     spanY() const { return spanY_; }
    int     getColumn() const { return column_; } // -1 == unspecified

    virtual void    update() {}
    virtual void    dump() const;
    virtual void    notifyChanged(const Common::SubscriberPtrBase*);

    int     spanDistance(const CalsTableRowInst*) const;

    virtual void nodeDestroyed(const GroveLib::Node*)    {}
    virtual void childInserted(const GroveLib::Node*)    {}
    virtual void childRemoved (const GroveLib::Node*,
                               const GroveLib::Node*)    {}
    virtual void attributeChanged(const GroveLib::Attr*) { notifyChanged(0); }
    virtual void attributeRemoved(const GroveLib::Element*,
                                  const GroveLib::Attr*) { notifyChanged(0); }
    virtual void attributeAdded(const GroveLib::Attr*)   { notifyChanged(0); }
    virtual void textChanged(const GroveLib::Text*)      {}

    CalsTableGroupInst* tgroup() const { return tgroup_.pointer(); }
    CalsTableRowInst*   row() const { return rowInst_; }

private:
    bool                calculateProperties();

    CalsTableRowInst*   rowInst_;
    GroveLib::Element*  cellElem_;
    int                 spanX_;
    int                 spanY_;
    int                 column_;
    Common::SubscriberPointer<CalsTableGroupInst> tgroup_;
};

class CalsTableGroupInst : public Instance,
                           public GroveLib::NodeVisitor,
                           public Common::SubscriberPtrPublisher {
public:
    XSLT_OALLOC(CalsTableGroupInst);

    typedef Common::XList<CalsTableRowInst> Rows;

    CalsTableGroupInst(const CalsTableGroup* instr,
                       const InstanceInit& init, Instance* p);
    virtual ~CalsTableGroupInst();

    virtual void    update();
    virtual void    dump() const;

    void            addRow(CalsTableRowInst*);
    void            rowPositionChanged(CalsTableRowInst*);
    int             nCols() const { return nCols_; }

    Common::String getColspec(const Common::String& namest,
                              const Common::String& propName) const;

    Common::String getTopProp(int colnum,
                              const Common::String& propName) const;

    Common::String getSpanValue(const Common::String& spanname,
                                const Common::String& propName) const;
    const GroveLib::Element* findSpanspec(const Common::String& name) const;

    int             getColNumber(const Common::String& name) const;

    virtual void    nodeDestroyed(const GroveLib::Node*);
    virtual void    childInserted(const GroveLib::Node*);
    virtual void    childRemoved (const GroveLib::Node*,
                                  const GroveLib::Node*);
    virtual void    attributeChanged(const GroveLib::Attr*) { setModified(); }
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*) { setModified(); }
    virtual void    attributeAdded(const GroveLib::Attr*)   { setModified(); }
    virtual void    textChanged(const GroveLib::Text*)      {}
    bool            isDita() const
    {
        return static_cast<const CalsTableGroup*>(instruction())->isDita();
    }

private:
    typedef Common::Vector<GroveLib::Element*> SpecVector;

    void                build_specs();

    GroveLib::Element*  tgroup_;
    SpecVector          colspecs_;
    SpecVector          spanspecs_;
    int                 nCols_;
    Rows                rows_;
};

} // namespace Xslt

#endif // XSLT_CALS_TABLE_INST_H

