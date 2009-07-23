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
#ifdef USE_XPATH3
#include "xslt/impl/Instance.h"
#include "xslt/impl/instructions/CalsTable.h"
#include "xslt/impl/instances/CalsTableInst.h"
#include "xslt/impl/FunctionExprImpl.h"
#include "xslt/impl/xpath_values.h"
#include "common/TreelocRep.h"

// todo: create check-type for xse:cals-table-group, which can take value 'dita'
// for checking class attributes to determine spanspecs/colspecs

// #define MAKE_RPAD_CELLS

using namespace Common;

inline static bool check_spec(const GroveLib::Node* n,
                              bool is_spanspec,
                              bool is_dita)
{
    if (!is_dita)
        return n->nodeName() == (is_spanspec ? "spanspec" : "colspec");
    if (n->nodeType() != GroveLib::Node::ELEMENT_NODE)
        return false;
    const GroveLib::Attr* a =
        CONST_ELEMENT_CAST(n)->attrs().getAttribute("class");
    if (0 == a)
        return false;
    return (a->value().find(" topic/colspec ") >= 0);
}

namespace Xslt {

static Instance* getCalsParent(Instance* self, Instruction::Type t)
{
    Instance* ci = self->parent();
    for (; ci; ci = ci->parent())
        if (ci->instruction()->type() == t)
            break;
    if (0 == ci)
        throw Xslt::Exception(XsltMessages::instanceCalsInstr,
              self->instruction()->element()->nodeName(),
              self->contextString());
    return ci;
}

CalsTableCellInst::CalsTableCellInst(const CalsTableCell* instr,
                                     const InstanceInit& init, Instance* p)
    : Instance(instr, init, p),
      rowInst_(0), spanX_(1), spanY_(0), column_(-1)
{
    DBG(XSLT.CALS) << "Cals-Cell:: cons\n";
    rowInst_ = static_cast<CalsTableRowInst*>
        (getCalsParent(this, Instruction::CALS_TABLE_ROW));
    tgroup_ = rowInst_->tgroup();

    if (nsiContext().node()->nodeType() != GroveLib::Node::ELEMENT_NODE)
        throw Xslt::Exception(XsltMessages::instanceCalsCell, contextString());
    cellElem_ = static_cast<GroveLib::Element*>(nsiContext().node());

    // if new cell is inserted, just notify parent row
    if (!rowInst_->buildInProcess()) {
        DBG(XSLT.CALS) << "Cell::Cell: no build in process\n";
        rowInst_->setModified();
        return;
    }

    calculateProperties();
    // make left padding of empty cells
    int nEmptyCells = rowInst_->addCell(this);
    if (nEmptyCells > 0) {
        DBG(XSLT.CALS) << "Cell::Cell: making left-padding="
            << nEmptyCells << std::endl;
        for (int i = 0; i < nEmptyCells; ++i)
            rowInst_->emptyCellTemplate()->makeInst(init, this);
    }
    buildSubInstances(init.resultContext_);
    cellElem_->registerNodeVisitor(this);
    tgroup_.setWatcher(this);
}

int  CalsTableCellInst::spanDistance(const CalsTableRowInst* row) const
{
    int distance = 0;
    const CalsTableRowInst* i = rowInst_;
    while (row && i && i != row) {
        i = i->RowItem::nextSibling();
        distance++;
    }
    return (int)spanY() - distance;
}

void CalsTableCellInst::notifyChanged(const COMMON_NS::SubscriberPtrBase*)
{
    DBG(XSLT.CALS) << "CellInst:: notifyChanged\n";
    if (calculateProperties())
        rowInst_->setModified();
}

bool CalsTableCellInst::calculateProperties()
{
    bool modified = false;
    const GroveLib::Attr* a = cellElem_->attrs().getAttribute("morerows");
    int val = 0;
    if (a) {
        val = a->value().toInt();
        if (val != spanY_) {
            spanY_ = val;
            modified = true;
        }
    } else if (spanY_ != 0) {
        spanY_ = 0;
        modified = true;
    }
    String namest, nameend;
    a = cellElem_->attrs().getAttribute("spanname");
    if (a) {
        const GroveLib::Element* spanspec = tgroup()->findSpanspec(a->value());
        if (spanspec) {
            GroveLib::Attr* ap = spanspec->attrs().getAttribute("namest");
            if (ap)
                namest = ap->value();
            ap = spanspec->attrs().getAttribute("nameend");
            if (ap)
                nameend = ap->value();
        }
    } else {
        a = cellElem_->attrs().getAttribute("namest");
        if (a)
            namest = a->value();
        a = cellElem_->attrs().getAttribute("nameend");
        if (a)
            nameend = a->value();
    }
    if (!namest.isNull()) {
        int colstart = tgroup()->getColNumber(namest);
        int colend   = tgroup()->getColNumber(nameend);
        if (column_ != colstart) {
            column_ = colstart;
            modified = true;
        }
        int newspan = colend - colstart + 1;
        if (newspan != spanX_) {
            spanX_ = newspan;
            modified = true;
        }
        if (spanX_ <= 0) {
            spanX_ = 1;
            modified = true;
        }
    } else {
        a = cellElem_->attrs().getAttribute("colname");
        if (a) {
            int newcol = tgroup()->getColNumber(a->value());
            if (column_ != newcol) {
                column_ = newcol;
                modified = true;
            }
        }
    }
    if (namest.isNull() && 0 == a) { // no colname, spanspec, namest
        if (column_ != -1) {
            column_ = -1;
            modified = true;
        }
    }
    DBG(XSLT.CALS) << "Calculated properties: spanX=" << spanX_
        << " spanY=" << spanY_ << " column " << column_ << std::endl;
    return modified;
}

Instance* CalsTableCell::makeInst(const InstanceInit& init,
                                  Instance* p) const
{
    return new CalsTableCellInst(this, init, p);
}

void CalsTableCellInst::dump() const
{
    DDBG << "CalsTableCell: " << this << std::endl;
    Instance::dump();
}

CalsTableCellInst::~CalsTableCellInst()
{
    if (rowInst_ && !rowInst_->buildInProcess())
        rowInst_->removeCell(this);
    DBG(XSLT.CALS) << "~CalsTableCell " << this << std::endl;
}

////////////////////////////////////////////////////////////////////

CalsTableRowInst::CalsTableRowInst(const CalsTableRow* instr,
                                   const InstanceInit& init, Instance* p)
    : Instance(instr, init, p)
{
    tgroupInst_ = static_cast<CalsTableGroupInst*>
        (getCalsParent(this, Instruction::CALS_TABLE_GROUP));
    if (tgroupInst_->nCols() < 1)
        return;
    resizeCells(tgroupInst_->nCols());
    DBG(XSLT.CALS) << "Row::Row: nCols = " << cells_.size() << std::endl;
    tgroupInst_->addRow(this);
    build(&init.resultContext_);
}

int CalsTableRowInst::addCell(CalsTableCellInst* cell)
{
    int column = cell->getColumn() - 1;
    int i = 0;
    DBG(XSLT.CALS) << "AddCell to row " << RowItem::siblingIndex()
        << ": column = " << column << " cellp " << cell << std::endl;
    // column is explicitly specified
    if (column >= 0) {
        for (i = 0; i < cell->spanX(); ++i)
            setCell(i + column, cell);
        int nEmptyCells = 0;
        for (i = column - 1; i >= 0; --i)
            if (!cells_[i])
                nEmptyCells++;
        notifyNextRows(cell->spanY());
        return nEmptyCells;
    }
    // find empty cell
    for (i = 0; i < (int)cells_.size(); ++i)
        if (!cells_[i])
            break;
    for (int j = 0; j < cell->spanX(); ++j)
        setCell(i + j, cell);
    DBG(XSLT.CALS) << "After appending cell:\n";
    DBG_IF(XSLT.CALS) dump();
    DBG(XSLT.CALS) << "----------------------\n";
    notifyNextRows(cell->spanY());
    return 0;
}

void CalsTableRowInst::removeCell(CalsTableCellInst* cell)
{
    for (uint i = 0; i < cells_.size(); ++i)
        if (cells_[i] == cell)
            cells_[i] = 0;
    if (Instance::parent())
        setModified();
    notifyNextRows(maxSpan());
}

void CalsTableRowInst::setCell(uint col, CalsTableCellInst* cell)
{
    DBG(XSLT.CALS) << "SetCell: col=" << col << " cellp " << cell << std::endl;
    if (col >= cells_.size())
        resizeCells(col + 1);
    cells_[col] = cell;
}

void CalsTableRowInst::resizeCells(uint newsize)
{
    if (cells_.size() >= newsize)
        return;
    uint i = cells_.size();
    cells_.resize(newsize);
    while (i < newsize)
        cells_[i++] = 0;
}

int CalsTableRowInst::cellIndex(const CalsTableCellInst* cell) const
{
    uint index = 0;
    for (; index < cells_.size(); ++index)
        if (cells_[index] == cell)
            return index + 1;
    return -1;
}

/// Calculates maximum vertical span this row occupies
int CalsTableRowInst::maxSpan() const
{
    int maxcspan = -1;
    for (uint i = 0; i < cells_.size(); ++i) {
        CalsTableCellInst* cell = cells_[i];
        if (0 == cell)
            continue;
        int spanDist = cell->spanDistance(this);
        if (maxcspan < spanDist)
            maxcspan = spanDist;
    }
    return maxcspan;
}

void CalsTableRowInst::notifyNextRows(int cnt)
{
    CalsTableRowInst* row = RowItem::nextSibling();
    while (row && cnt-- > 0) {
        row->setModified();
        row = row->RowItem::nextSibling();
    }
}

void CalsTableRowInst::update()
{
    build(0);
}

void CalsTableRowInst::updateContext(int type)
{
    if (type & Instance::POS_CHANGED)
        tgroupInst_->rowPositionChanged(this);
    Instance::updateContext(type);
}

void CalsTableRowInst::build(const ResultContext* rc)
{
    DBG(XSLT.CALS) << "Row " << this << ": BUILD\n";
    buildInProcess_ = true;
    disposeResult();
    removeSubInstances();
    for (uint i = 0; i < cells_.size(); ++i)
        cells_[i] = 0;

    // fill cells which are busy with prev. cell vertical spans
    CalsTableRowInst* prevRow = 0;
    if (RowItem::parent())
        prevRow = RowItem::prevSibling();
    if (prevRow && prevRow->nsiContext().node()->parent() !=
        nsiContext().node()->parent())
            prevRow = 0;
    if (prevRow) {
        for (uint i = 0; i < prevRow->cells_.size(); ++i) {
            CalsTableCellInst* cell = prevRow->cells_[i];
            if (0 == cell)
                continue;
            if (cell->spanDistance(this) < 0)
                continue;
            DBG(XSLT.CALS) << "Set cell#" << i
                << " (" << cell << ") span from prev row\n";
            setCell(i, cell);
        }
    }
    // build cells
    buildSubInstances(rc ? *rc : resultContext());

#ifdef MAKE_RPAD_CELLS
    // pad with empty cells on the right side
    int col = 0;
    for (col = tgroupInst_->nCols() - 1; col >= 0; --col)
        if (cells_[col]) {
            DBG(XSLT.CALS) << "col=" << col << " val=" << cells_[col]
                << std::endl;
            break;
        }
    DBG(XSLT.CALS) << "ROW-BUILD: creating " << col << " rpad cells\n";
    col = tgroupInst_->nCols() - col - 1;
    while (col-- > 0)
        emptyCellTemplate()->makeInst(InstanceInit(nsiContext(),
            rc ? *rc : resultContext()), this);
#endif // MAKE_RPAD_CELLS
    buildInProcess_ = false;
}

const Instruction* CalsTableRowInst::emptyCellTemplate()
{
    return static_cast<const CalsTableRow*>(instruction())->emptyCellTemplate();
}

void CalsTableRowInst::dump() const
{
    DDBG << "CalsTableRowInst: (" << this << ") CELLS ("
        << cells_.size() << "): ";
    for (uint i = 0; i < cells_.size(); ++i) {
        CalsTableCellInst* cell = cells_[i];
        if (0 == cell) {
            DDBG << "[-] ";
        } else {
            DDBG << "[" << cell->spanDistance(this) << "] ";
        }
    }
    DDBG << std::endl;
    //Instance::dump();
}

void CalsTableRowInst::prepareRemove()
{
    tgroupInst_ = 0;
}

Instance* CalsTableRow::makeInst(const InstanceInit& init,
                                  Instance* p) const
{
    return new CalsTableRowInst(this, init, p);
}

CalsTableRowInst::~CalsTableRowInst()
{
    if (tgroupInst_)
        notifyNextRows(maxSpan());
    DBG(XSLT.CALS) << "~CalsTableRow " << this << std::endl;
}

////////////////////////////////////////////////////////////////

CalsTableGroupInst::CalsTableGroupInst(const CalsTableGroup* instr,
                                       const InstanceInit& init, Instance* p)
    : Instance(instr, init, p), nCols_(1)
{
    if (nsiContext().node()->nodeType() != GroveLib::Node::ELEMENT_NODE)
        throw Xslt::Exception(XsltMessages::instanceCalsGroup,
                              contextString());
    tgroup_ = static_cast<GroveLib::Element*>(nsiContext().node());
    tgroup_->registerNodeVisitor(this);
    const GroveLib::Attr* a = tgroup_->attrs().getAttribute("cols");
    if (a)
        nCols_ = a->value().toInt();
    if (nCols_ <= 0 || nCols_ > 100)
        nCols_ = 1;
    build_specs();
    buildSubInstances(init.resultContext_);
}

void CalsTableGroupInst::update()
{
    DBG(XSLT.CALS) << "CalsGroupInst: update\n";
    build_specs();
    SubscriberPtrPublisher::notifyChanged();
}

static TreelocRep relativeTreeloc(Instance* top, Instance* i)
{
    TreelocRep tloc;
    for (; i && i != top->parent(); i = i->parent())
        tloc += i->instanceSiblingIndex();
    return tloc;
}

void CalsTableGroupInst::addRow(CalsTableRowInst* newRow)
{
    DBG(XSLT.CALS) << "addRow: currently " << rows_.countChildren() << "rows\n";
    TreelocRep newTloc = relativeTreeloc(this, newRow);
    DBG(XSLT.CALS) << "newTLOC=" << newTloc.toString() << std::endl;
    CalsTableRowInst* nextRow = rows_.firstChild();
    for (; nextRow; nextRow = nextRow->RowItem::nextSibling()) {
        TreelocRep nextTloc = relativeTreeloc(this, nextRow);
        DBG(XSLT.CALS) << "nextRow=" << nextTloc.toString() << std::endl;
        // Shorter treeloc means that instance is not yet re-connected
        // by the ApplicatingInstance, and therefore is after
        if (newTloc.length() > nextTloc.length() ||
            newTloc < relativeTreeloc(this, nextRow))
                break;
    }
    DBG(XSLT.CALS) << "addRow: nextRow " << nextRow << std::endl;
    if (nextRow)
        nextRow->RowItem::insertBefore(newRow);
    else
        rows_.appendChild(newRow);
}

void CalsTableGroupInst::rowPositionChanged(CalsTableRowInst* row)
{
    DBG(XSLT.CALS) << "RowPositionChange: new pos "
        << row->nsiContext().pos() << std::endl;
    RefCntPtr<CalsTableRowInst> rowPtr(row);
    row->RowItem::remove();
    CalsTableRowInst* r = rows_.firstChild();
    for (; r; r = r->RowItem::nextSibling()) {
        if (r->nsiContext().node()->parent() !=
            row->nsiContext().node()->parent())
                continue; // case e.g. when one row in thead, other in tbody
        if (row->nsiContext().pos() <= r->nsiContext().pos()) {
            r->RowItem::insertBefore(row);
            if (row->maxSpan() > 0)
                r->setModified();
            return;
        }
    }
    rows_.appendChild(row);
}

void CalsTableGroupInst::build_specs()
{
    colspecs_.clear();
    spanspecs_.clear();
    const_cast<GroveLib::Element*>(tgroup_)->registerNodeVisitor(this);
    GroveLib::Node* n = tgroup_->firstChild();
    for (; n; n = n->nextSibling()) {
        if (n->nodeType() != GroveLib::Node::ELEMENT_NODE)
            continue;
        GroveLib::Element* e = static_cast<GroveLib::Element*>(n);
        if (check_spec(e, false, isDita()))
            colspecs_.push_back(e);
        else if (check_spec(e, true, isDita()))
            spanspecs_.push_back(e);
        else
            continue;
        const_cast<GroveLib::Node*>(n)->registerNodeVisitor(this);
        GroveLib::Attr* attr = e->attrs().firstChild();
        for (; attr; attr = attr->nextSibling())
            attr->registerNodeVisitor(this);
    }
}
static bool match_attr(const GroveLib::Element* elem,
                       const String& attname,
                       const String& expValue)
{
    const GroveLib::Attr* a = elem->attrs().getAttribute(attname);
    if (0 == a || a->value() != expValue)
        return false;
    return true;
}

const GroveLib::Element*
CalsTableGroupInst::findSpanspec(const Common::String& name) const
{
    for (uint i = 0; i < spanspecs_.size(); ++i) {
        if (match_attr(spanspecs_[i], "spanname", name))
            return spanspecs_[i];
    }
    return 0;
}

int CalsTableGroupInst::getColNumber(const Common::String& name) const
{
    int colnum = 0;
    for (uint i = 0; i < colspecs_.size(); ++i) {
        const GroveLib::Element* e = colspecs_[i];
        const GroveLib::Attr* a = e->attrs().getAttribute("colnum");
        if (a)
            colnum = a->value().toInt();
        else
            ++colnum;
        if (match_attr(e, "colname", name))
            return colnum;
    }
    return -1;
}

String CalsTableGroupInst::getColspec(const String& namest,
                                      const String& propName) const
{
    const GroveLib::Element* colspec = 0;
    for (uint i = 0; i < colspecs_.size(); ++i) {
        if (match_attr(colspecs_[i], "colname", namest)) {
            colspec = colspecs_[i];
            break;
        }
    }
    if (0 == colspec)
        return String::null();
    const GroveLib::Attr* a = colspec->attrs().getAttribute(propName);
    if (a)
        return a->value();
    return String::null();
}

String CalsTableGroupInst::getTopProp(int coln,
                                      const String& propName) const
{
    const GroveLib::Element* colspec = 0;
    int colnum = 0;
    const GroveLib::Attr* a = 0;
    for (uint i = 0; i < colspecs_.size(); ++i) {
        const GroveLib::Element* e = colspecs_[i];
        a = e->attrs().getAttribute("colnum");
        if (a)
            colnum = a->value().toInt();
        else
            ++colnum;
        if (colnum == coln) {
            colspec = colspecs_[i];
            break;
        }
    }
    if (colspec) {
        a = colspec->attrs().getAttribute(propName);
        if (a)
            return a->value();
    }
    a = tgroup_->attrs().getAttribute(propName);
    if (a)
        return a->value();
    return String::null();
}

String CalsTableGroupInst::getSpanValue(const String& spanname,
                                        const String& propName) const
{
    const GroveLib::Element* e = findSpanspec(spanname);
    if (0 == e)
        return String::null();
    const GroveLib::Attr* a = e->attrs().getAttribute(propName);
    if (a)
        return a->value();
    a = e->attrs().getAttribute("namest");
    if (0 == a)
        return String::null();
    e = 0;
    for (uint i = 0; i < colspecs_.size(); ++i) {
        if (match_attr(colspecs_[i], "colname", a->value())) {
            e = colspecs_[i];
            break;
        }
    }
    if (0 == e)
        return String::null();
    a = e->attrs().getAttribute(propName);
    if (a)
        return a->value();
    return String::null();
}

inline static bool need_check(const GroveLib::Node* n,
                              const CalsTableGroupInst* inst)
{
    return check_spec(n, false, inst->isDita()) ||
           check_spec(n, true,  inst->isDita());
}

void CalsTableGroupInst::nodeDestroyed(const GroveLib::Node* n)
{
    if (need_check(n, this))
        setModified();
}

void CalsTableGroupInst::childInserted(const GroveLib::Node* n)
{
    if (need_check(n, this))
        setModified();
}

void CalsTableGroupInst::childRemoved (const GroveLib::Node*,
                                       const GroveLib::Node* n)
{
    if (need_check(n, this))
        setModified();
}

void CalsTableGroupInst::dump() const
{
    DDBG << "Cals-Tgroup: cols=" << nCols_
        << " rows=" << rows_.countChildren() << std::endl;
}

Instance* CalsTableGroup::makeInst(const InstanceInit& init,
                                  Instance* p) const
{
    return new CalsTableGroupInst(this, init, p);
}

CalsTableGroupInst::~CalsTableGroupInst()
{

    DBG(XSLT.CALS) << "~CalsTableGroup " << this << std::endl;
    SubscriberPtrPublisher::clearSubscribers();
    CalsTableRowInst* row = rows_.firstChild();
    for (; row; row = row->RowItem::nextSibling())
        row->prepareRemove();
}

////////////////////////////////////////////////////////////////////

class CalsAttributeSubinst : public Xpath::ExprSubInst,
                             public GroveLib::NodeVisitor,
                             public Common::SubscriberPtrWatcher,
                             public Xpath::FuncArgEvaluator {
public:
    virtual int type() const
    {
        return XSLT_SUBINST + XsltFunctionId::CalsAttribute; 
    }
    CalsAttributeSubinst(const Xpath::NodeSetItem& nsi,
                         Xpath::ExprInst& ei,
                         const Xpath::FunctionExpr* fe,
                         CalsTableCellInst* cellInst)
        : FuncArgEvaluator(nsi, ei, fe),
          cellInst_(cellInst), modified_(false)
    {
        tgroup_ = cellInst_->tgroup();
        tgroup_.setWatcher(this);
        cellInst_->nsiContext().node()->registerNodeVisitor(this);
    }
    virtual void    nodeDestroyed(const GroveLib::Node*) {}
    virtual void    childInserted(const GroveLib::Node*) {}
    virtual void    childRemoved (const GroveLib::Node*,
                                  const GroveLib::Node*) {}
    virtual void    attributeChanged(const GroveLib::Attr*) { reeval(); }
    virtual void    attributeRemoved(const GroveLib::Element*,
                                     const GroveLib::Attr*) { reeval(); }
    virtual void    attributeAdded(const GroveLib::Attr*) { reeval(); }
    virtual void    textChanged(const GroveLib::Text*) {}

    String   eval();
    void     reeval()
    {
        DBG(XSLT.CALS) << "CalsAttribute: reeval\n";
        String new_value = eval();
        if (new_value != oldValue_) {
            oldValue_ = new_value;
            DBG(XSLT.CALS) << "CalsAttr: modified\n";
            modified_ = true;
            exprInst().setModified();
        }
    }
    virtual void exprChanged()
    {
        if (!modified_)     // reevaluation is not caused by myself
            delete this;
    }
    void notifyChanged(const SubscriberPtrBase*) { reeval(); }
    int  getColumn() { return cellInst_->row()->cellIndex(cellInst_); }

    CalsTableGroupInst* tgroup() const { return cellInst_->tgroup(); }

    CalsTableCellInst*  cellInst_;
    SubscriberPointer<CalsTableGroupInst> tgroup_;
    String              oldValue_;
    bool                modified_;
};

template<> Xpath::ConstValueImplPtr
FunctionExprImpl<XsltFunctionId::CalsAttribute>::eval
    (const Xpath::NodeSetItem& nsi, Xpath::ExprInst& ei) const
{
    CalsAttributeSubinst* subInst = static_cast<CalsAttributeSubinst*>
        (ei.findSubInst(Xpath::ExprSubInst::XSLT_SUBINST +
            XsltFunctionId::CalsAttribute));
    if (0 == subInst) {
        const Instance* ci = static_cast<const Instance*>(&ei.exprContext());
        while (ci && ci->instruction()->type() != Instruction::CALS_TABLE_CELL)
            ci = ci->parent();
        if (0 == ci)
            throw Xslt::Exception(XsltMessages::xpathInstCalsTableAttr,
                                  ei.exprContext().contextString());
        subInst = new CalsAttributeSubinst(nsi, ei, this,
            static_cast<CalsTableCellInst*>(const_cast<Instance*>(ci)));
        ei.appendSubInst(subInst);
        subInst->oldValue_ = subInst->eval();
    }
    return new Xpath::StringValue(subInst->oldValue_);
}

String CalsAttributeSubinst::eval()
{
    if (!argNum())
        throw Xslt::Exception(XsltMessages::xpathInstCalsTableArgs,
                              exprInst().exprContext().contextString());
    GroveLib::Node* n = cellInst_->nsiContext().node();
    if (n->nodeType() != GroveLib::Node::ELEMENT_NODE)
        throw Xslt::Exception(XsltMessages::xpathInstCalsTableNode,
                              exprInst().exprContext().contextString());
    GroveLib::Element* cellElem = static_cast<GroveLib::Element*>(n);

    String propname = arg(0)->to(Xpath::Value::STRING,
        &exprInst())->getString();
    String defval;
    if (argNum() > 1)
        defval = arg(1)->to(Xpath::Value::STRING, &exprInst())->getString();
    if (propname == "cals:colnum")
        return String::number(getColumn());
    else if (propname == "cals:colspan")
        return String::number(cellInst_->spanX());
    const GroveLib::Attr* a = cellElem->attrs().getAttribute(propname);
    if (a)
        return a->value();
    a = cellElem->attrs().getAttribute("spanname");
    String sv;
    if (a) {
        sv = tgroup_->getSpanValue(a->value(), propname);
        if (!sv.isNull())
            return sv;
    }
    a = cellElem->attrs().getAttribute("namest");
    if (0 == a)
        a = cellElem->attrs().getAttribute("colname");
    if (a) {
        String sv = tgroup_->getColspec(a->value(), propname);
        if (!sv.isNull())
            return sv;
    }
    sv = tgroup_->getTopProp(getColumn(), propname);
    if (!sv.isNull())
        return sv;
    return defval;
}

} // namespace Xslt

#endif // USE_XPATH3
