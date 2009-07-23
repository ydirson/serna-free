// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef ProcessContext_INCLUDED
#define ProcessContext_INCLUDED 1

#include "Resource.h"
#include "Ptr.h"
#include "Vector.h"
#include "NCVector.h"
#include "Owner.h"
#include "Collector.h"
#include "Style.h"
#include "FOTBuilder.h"
#include "ELObj.h"
#include "SosofoObj.h"
#include "VM.h"
#include "ProcessingMode.h"
#include "Link.h"
#include "IList.h"
#include "IQueue.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class Expression;

class ProcessContext : public Collector::DynamicRoot {
public:
  ProcessContext(Interpreter &, FOTBuilder &);
  FOTBuilder &currentFOTBuilder();
  StyleStack &currentStyleStack();
  void process(const NodePtr &);
  void processNode(const NodePtr &, const ProcessingMode *, bool chunk = 1);
  void processNodeSafe(const NodePtr &, const ProcessingMode *, bool chunk = 1);
  void nextMatch(StyleObj *);
  void processChildren(const ProcessingMode *);
  void processChildrenTrim(const ProcessingMode *);
  void trace(Collector &) const;
  void startFlowObj();
  void endFlowObj();
  // Uses of label: do this
  void startConnection(SymbolObj *, const Location &);
  void endConnection();
  // happens only for objcet with a non-principal port
  void pushPorts(bool hasPrincipalPort,
		 const Vector<SymbolObj *> &ports, const Vector<FOTBuilder *> &fotbs);
  void popPorts();
  // happens inside pushPorts() (if any)
  void startMapContent(ELObj *, const Location &);
  void endMapContent();
  void startDiscardLabeled(SymbolObj *);
  void endDiscardLabeled();
  // table support
  void startTable();
  void endTable();
  void startTablePart();
  void endTablePart();
  void addTableColumn(unsigned columnIndex, unsigned span, StyleObj *);
  unsigned currentTableColumn();
  void noteTableCell(unsigned colIndex, unsigned colSpan, unsigned rowSpan);
  StyleObj *tableColumnStyle(unsigned columnIndex, unsigned span);
  StyleObj *tableRowStyle();
  void startTableRow(StyleObj *);
  bool inTable() const;
  bool inTableRow();
  void endTableRow();
  void clearPageType();
  void setPageType(unsigned);
  bool getPageType(unsigned &) const;

  VM &vm();
private:
  ProcessContext(const ProcessContext &); // undefined
  void operator=(const ProcessContext &); // undefined
  void badContentMap(bool &, const Location &);
  void coverSpannedRows();
  void restoreConnection(unsigned connectableLevel, size_t portIndex);
  struct Port {
    Port();
    FOTBuilder *fotb;
    IQueue<SaveFOTBuilder> saveQueue;
    Vector<SymbolObj *> labels;
    unsigned connected;
  };
  // A flow object with a port that can be connected to.
  struct Connectable;
  friend struct Connectable;
  struct Connectable : public Link {
    Connectable(int nPorts, const StyleStack &, unsigned);
    NCVector<Port> ports;
    StyleStack styleStack;
    unsigned flowObjLevel;
    Vector<SymbolObj *> principalPortLabels;
  };
  // An connection between a flow object and its flow parent
  // made with label:.
  struct Connection;
  friend struct Connection;
  struct Connection : public Link {
    Connection(FOTBuilder *);
    Connection(const StyleStack &, Port *, unsigned connectableLevel);
    FOTBuilder *fotb;
    StyleStack styleStack;
    Port *port;
    unsigned connectableLevel;
    unsigned nBadFollow;
  };
  struct Table : public Link {
    Table();
    unsigned currentColumn;
    // first index is column (zero-based)
    // second is span - 1.
    Vector<Vector<StyleObj *> > columnStyles;
    // for each column, how many rows are covered
    // starting with the current row
    Vector<unsigned> covered;
    unsigned nColumns;
    StyleObj *rowStyle;
    bool inTableRow;
    unsigned rowConnectableLevel;
  };
  struct NodeStackEntry {
    unsigned long elementIndex;
    unsigned groveIndex;
    const ProcessingMode *processingMode;
  };
  FOTBuilder ignoreFotb_;
  IList<Connection> connectionStack_;
  IList<Connectable> connectableStack_;
  unsigned connectableStackLevel_;
  IList<Table> tableStack_;
  NCVector<IQueue<SaveFOTBuilder> > principalPortSaveQueues_;
  VM vm_;
  ProcessingMode::Specificity matchSpecificity_;
  unsigned flowObjLevel_;
  bool havePageType_;
  unsigned pageType_;
  Vector<NodeStackEntry> nodeStack_;
  friend class CurrentNodeSetter;
  friend struct Table;
};

inline
FOTBuilder &ProcessContext::currentFOTBuilder()
{
  return *connectionStack_.head()->fotb;
}

inline
StyleStack &ProcessContext::currentStyleStack()
{
  return connectionStack_.head()->styleStack;
}

inline
VM &ProcessContext::vm()
{
  return vm_;
}

inline
void ProcessContext::startFlowObj()
{
  flowObjLevel_++;
}

inline
void ProcessContext::setPageType(unsigned n)
{
  havePageType_ = 1;
  pageType_ = n;
}

inline
void ProcessContext::clearPageType()
{
  havePageType_ = 0;
}

inline
bool ProcessContext::getPageType(unsigned &n) const
{
  if (!havePageType_)
    return 0;
  n = pageType_;
  return 1;
}

inline
bool ProcessContext::inTable() const
{
  return !tableStack_.empty();
}

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not ProcessContext_INCLUDED */

