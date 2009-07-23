// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "ProcessContext.h"
#include "Interpreter.h"
#include "InterpreterMessages.h"
#include "SosofoObj.h"
#include "macros.h"
#include "IListIter.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

ProcessContext::ProcessContext(Interpreter &interp, FOTBuilder &fotb)
: Collector::DynamicRoot(interp), vm_(interp), flowObjLevel_(0), havePageType_(0), connectableStackLevel_(0)
{
  connectionStack_.insert(new Connection(&fotb));
}

void ProcessContext::process(const NodePtr &node)
{
  Interpreter &interp = *vm_.interp;
  StyleObj *style = interp.initialStyle();
  if (style) {
    currentStyleStack().push(style, vm(), currentFOTBuilder());
    currentFOTBuilder().startSequence();
  }
  processNode(node, interp.initialProcessingMode());
  if (style) {
    currentFOTBuilder().endSequence();
    currentStyleStack().pop();
  }
}

void ProcessContext::processNodeSafe(const NodePtr &nodePtr,
				     const ProcessingMode *processingMode,
				     bool chunk)
{
  unsigned long elementIndex;
  if (nodePtr->elementIndex(elementIndex) == accessOK) {
    unsigned groveIndex = nodePtr->groveIndex();
    for (size_t i = 0; i < nodeStack_.size(); i++) {
      const NodeStackEntry &nse = nodeStack_[i];
      if (nse.elementIndex == elementIndex
	  && nse.groveIndex == groveIndex
	  && nse.processingMode == processingMode) {
	vm_.interp->setNodeLocation(nodePtr);
	vm_.interp->message(InterpreterMessages::processNodeLoop);
	return;
      }
    }
    nodeStack_.resize(nodeStack_.size() + 1);
    NodeStackEntry &e = nodeStack_.back();
    e.elementIndex = elementIndex;
    e.groveIndex = groveIndex;
    e.processingMode = processingMode;
    processNode(nodePtr, processingMode, chunk);
    nodeStack_.resize(nodeStack_.size() - 1);
  }
  else
    processNode(nodePtr, processingMode, chunk);
}

void ProcessContext::processNode(const NodePtr &nodePtr,
				 const ProcessingMode *processingMode,
				 bool chunk)
{
  ASSERT(processingMode != 0);
  GroveString str;
  if (nodePtr->charChunk(*vm_.interp, str) == accessOK) 
    currentFOTBuilder().charactersFromNode(nodePtr, str.data(), chunk ? str.size() : 1);
  else {
    EvalContext::CurrentNodeSetter cns(nodePtr, processingMode, vm());
    ProcessingMode::Specificity saveSpecificity(matchSpecificity_);
    matchSpecificity_ = ProcessingMode::Specificity();
    bool hadStyle = 0;
    currentFOTBuilder().startNode(nodePtr, processingMode->name());
    for (;;) {
      const ProcessingMode::Rule *rule
       = vm().processingMode->findMatch(nodePtr, *vm_.interp, *vm_.interp,
					matchSpecificity_);
      if (!rule) {
	if (hadStyle) {
	  currentStyleStack().pushEnd(vm(), currentFOTBuilder());
	  currentFOTBuilder().startSequence();
	}
        processChildren(processingMode);
	break;
      }
      if (!matchSpecificity_.isStyle()) {
	SosofoObj *sosofoObj;
        InsnPtr insn;
	rule->action().get(insn, sosofoObj);
	if (hadStyle) {
	  currentStyleStack().pushEnd(vm(), currentFOTBuilder());
	  currentFOTBuilder().startSequence();
	}
	if (sosofoObj)
	  sosofoObj->process(*this);
	else {
	  ELObj *obj = vm().eval(insn.pointer());
	  if (vm_.interp->isError(obj)) {
	    if (processingMode->name().size() == 0)
	      processChildren(processingMode);
	  }
	  else {
	    ELObjDynamicRoot protect(*vm_.interp, obj);
	    ((SosofoObj *)obj)->process(*this);
	  }
	}
	break;
      }
      SosofoObj *sosofoObj;
      InsnPtr insn;
      rule->action().get(insn, sosofoObj);
      ELObj *obj = vm().eval(insn.pointer());
      if (!vm_.interp->isError(obj)) {
	if (!hadStyle) {
	  currentStyleStack().pushStart();
	  hadStyle = 1;
	}
	currentStyleStack().pushContinue((StyleObj *)obj, rule, nodePtr, vm_.interp);
      }
    }
    if (hadStyle) {
      currentFOTBuilder().endSequence();
      currentStyleStack().pop();
    }
    currentFOTBuilder().endNode();
    matchSpecificity_ = saveSpecificity;
  }
}

void ProcessContext::nextMatch(StyleObj *overridingStyle)
{
  ProcessingMode::Specificity saveSpecificity(matchSpecificity_);
  StyleObj *saveOverridingStyle = vm().overridingStyle;
  if (overridingStyle)
    vm().overridingStyle = overridingStyle;
  const ProcessingMode::Rule *rule
   = vm().processingMode->findMatch(vm().currentNode, *vm_.interp, *vm_.interp,
				    matchSpecificity_);
  if (rule) {
    ASSERT(!matchSpecificity_.isStyle());
    SosofoObj *sosofoObj;
    InsnPtr insn;
    rule->action().get(insn, sosofoObj);
    if (sosofoObj)
      sosofoObj->process(*this);
    else {
      ELObj *obj = vm().eval(insn.pointer());
      if (vm_.interp->isError(obj)) 
	processChildren(vm().processingMode);
      else {
	ELObjDynamicRoot protect(*vm_.interp, obj);
	((SosofoObj *)obj)->process(*this);
      }
    }
  }
  else
    processChildren(vm().processingMode);
  vm().overridingStyle = saveOverridingStyle;
  matchSpecificity_ = saveSpecificity;
}

void ProcessContext::processChildren(const ProcessingMode *processingMode)
{
  if (vm().currentNode.assignFirstChild() == accessOK) {
    do {
      processNode(vm().currentNode, processingMode);
    } while (vm().currentNode.assignNextChunkSibling() == accessOK);
  }
  else if (vm().currentNode->getDocumentElement(vm().currentNode) == accessOK)
    processNode(vm().currentNode, processingMode);
}

inline
bool isWhiteSpace(Char c)
{
  switch (c) {
  case '\f':
  case '\r':
  case '\n':
  case '\t':
  case ' ':
    return 1;
  }
  return 0;
}

static
bool onlyWhiteSpaceFollows(const NodePtr &node, const SdataMapper &mapper)
{
  NodePtr tem;
  if (node->nextChunkSibling(tem) == accessOK) {
    do {
      GroveString str;
      if (tem->charChunk(mapper, str) == accessOK) {
	for (size_t i = 0; i < str.size(); i++)
	  if (!isWhiteSpace(str[i]))
	    return 0;
      }
      else if (tem->getGi(str) == accessOK)
	return 0;
    } while (tem.assignNextChunkSibling() == accessOK);
  }
  return 1;
}

void ProcessContext::processChildrenTrim(const ProcessingMode *processingMode)
{
  if (vm().currentNode.assignFirstChild() == accessOK) {
    bool atStart = 1;
    do {
      NodePtr curNode = vm().currentNode;
      GroveString str;
      if (curNode->charChunk(*vm().interp, str) == accessOK) {
	if (atStart) {
	  const Char *s = str.data();
	  size_t n = str.size();
	  for (; n > 0; n--, s++) {
	    if (!isWhiteSpace(*s))
	      break;
	  }
	  if (n == 0)
	    str.assign(0, 0);
	  else {
	    atStart = 0;
	    if (s != str.data()) {
	      if (curNode->followSiblingRef(str.size() - (n + 1), curNode) != accessOK)
		CANNOT_HAPPEN();
	      str.assign(s, n);
	    }
	  }
  	}
	if (str.size()) {
	  if (isWhiteSpace(str[str.size() - 1])
	      && onlyWhiteSpaceFollows(curNode, *vm().interp)) {
	    for (size_t n = str.size() - 1; n > 0; n--) {
	      if (!isWhiteSpace(str[n - 1])) {
		currentFOTBuilder().charactersFromNode(curNode, str.data(), n);
		return;
	      }
	    }
	    return;
	  }
	  currentFOTBuilder().charactersFromNode(curNode, str.data(), str.size());
	}
      }
      else {
	if (atStart && vm().currentNode->getGi(str) == accessOK)
	  atStart = 0;
	processNode(vm().currentNode, processingMode);
      }
    } while (vm().currentNode.assignNextChunkSibling() == accessOK);
  }
  else if (vm().currentNode->getDocumentElement(vm().currentNode) == accessOK)
    processNode(vm().currentNode, processingMode);
}

void ProcessContext::startConnection(SymbolObj *label, const Location &loc)
{
  unsigned connLevel = connectableStackLevel_;
  for (IListIter<Connectable> iter(connectableStack_); !iter.done(); iter.next(), --connLevel) {
    Connectable *conn = iter.cur();
    for (size_t i = 0; i < conn->ports.size(); i++) {
      Port &port = conn->ports[i];
      for (size_t j = 0; j < port.labels.size(); j++)
	if (port.labels[j] == label) {
	  restoreConnection(connLevel, i);
	  return;
	}
    }
    for (size_t i = 0; i < conn->principalPortLabels.size(); i++)
      if (conn->principalPortLabels[i] == label) {
	restoreConnection(connLevel, size_t(-1));
	return;
      }
  }
  vm().interp->setNextLocation(loc);
  vm().interp->message(InterpreterMessages::badConnection,
		       StringMessageArg(*label->name()));
  connectionStack_.head()->nBadFollow++;
}


void ProcessContext::endConnection()
{
  if (inTableRow() && tableStack_.head()->rowConnectableLevel == connectableStackLevel_)
    endTableRow();
  if (connectionStack_.head()->nBadFollow > 0)
    connectionStack_.head()->nBadFollow--;
  else {
    currentFOTBuilder().endNode();
    Port *port = connectionStack_.head()->port;
    if (port && --(port->connected) == 0) {
      while (!port->saveQueue.empty()) {
	SaveFOTBuilder *saved = port->saveQueue.get();
	saved->emit(*port->fotb);
	delete saved;
      }
    }
    delete connectionStack_.get();
  }
}

void ProcessContext::restoreConnection(unsigned connectableLevel, size_t portIndex)
{
  unsigned connLevel = connectableStackLevel_;
  IListIter<Connectable> iter(connectableStack_);
  for (; connLevel != connectableLevel; iter.next(), --connLevel)
    ;
  Connectable *conn = iter.cur();
  if (portIndex != size_t(-1)) {
    Port &port = conn->ports[portIndex];
    Connection *c = new Connection(conn->styleStack, &port, connLevel);
    if (port.connected) {
      port.connected++;
      SaveFOTBuilder *save = new SaveFOTBuilder(vm().currentNode,
						vm().processingMode->name());
      c->fotb = save;
      port.saveQueue.append(save);
    }
    else {
      c->fotb = port.fotb;
      port.connected = 1;
    }
    connectionStack_.insert(c);
    currentFOTBuilder().startNode(vm().currentNode,
				  vm().processingMode->name());
  }
  else {
    Connection *c = new Connection(conn->styleStack, 0, connLevel);
    if (conn->flowObjLevel == flowObjLevel_) {
      c->fotb = &currentFOTBuilder();
    }
    else {
      SaveFOTBuilder *save = new SaveFOTBuilder(vm().currentNode,
						vm().processingMode->name());
      c->fotb = save;
      if (conn->flowObjLevel >= principalPortSaveQueues_.size())
	principalPortSaveQueues_.resize(conn->flowObjLevel + 1);
      principalPortSaveQueues_[conn->flowObjLevel].append(save);
    }
    connectionStack_.insert(c);
    currentFOTBuilder().startNode(vm().currentNode,
				  vm().processingMode->name());
  }
}

void ProcessContext::endFlowObj()
{
  flowObjLevel_--;
  if (flowObjLevel_ < principalPortSaveQueues_.size()) {
    IQueue<SaveFOTBuilder> &saveQueue = principalPortSaveQueues_[flowObjLevel_];
    while (!saveQueue.empty()) {
      SaveFOTBuilder *saved = saveQueue.get();
      saved->emit(currentFOTBuilder());
      delete saved;
    }
  }
}


ProcessContext::Connection::Connection(const StyleStack &s, Port *p, unsigned connLevel)
: styleStack(s), port(p), nBadFollow(0), connectableLevel(connLevel)
{
}

ProcessContext::Connection::Connection(FOTBuilder *f)
: fotb(f), port(0), nBadFollow(0), connectableLevel(0)
{
}

void ProcessContext::pushPorts(bool,
			       const Vector<SymbolObj *> &labels,
			       const Vector<FOTBuilder *> &fotbs)
{
  Connectable *c = new Connectable(labels.size(), currentStyleStack(), flowObjLevel_);
  connectableStack_.insert(c);
  for (size_t i = 0; i < labels.size(); i++) {
    c->ports[i].labels.push_back(labels[i]);
    c->ports[i].fotb = fotbs[i];
  }
  connectableStackLevel_++;
  // FIXME deal with !hasPrincipalPort
}

void ProcessContext::popPorts()
{
  connectableStackLevel_--;
  delete connectableStack_.get();
}

void ProcessContext::startDiscardLabeled(SymbolObj *label)
{
  startFlowObj();
  Connectable *c = new Connectable(1, currentStyleStack(), flowObjLevel_);
  connectableStack_.insert(c);
  c->ports[0].labels.push_back(label);
  c->ports[0].fotb = &ignoreFotb_;
}

void ProcessContext::endDiscardLabeled()
{
  delete connectableStack_.get();
  endFlowObj();
}

void ProcessContext::startMapContent(ELObj *contentMap, const Location &loc)
{
  bool badFlag = 0;
  if (!connectableStack_.head() || connectableStack_.head()->flowObjLevel != flowObjLevel_)
    connectableStack_.insert(new Connectable(0, currentStyleStack(), flowObjLevel_));
  Connectable &conn = *connectableStack_.head();
  Vector<SymbolObj *> portNames(conn.ports.size());
  for (size_t i = 0; i < conn.ports.size(); i++) {
    portNames[i] = conn.ports[i].labels[0];
    conn.ports[i].labels.clear();
  }
  for (;;) {
    if (contentMap->isNil())
      break;
    PairObj *tem = contentMap->asPair();
    if (!tem) {
      badContentMap(badFlag, loc);
      break;
    }
    ELObj *entry = tem->car();
    contentMap = tem->cdr();
    tem = entry->asPair();
    if (tem) {
      SymbolObj *label = tem->car()->asSymbol();
      if (label) {
	// FIXME check if label already specified
	tem = tem->cdr()->asPair();
	if (tem) {
	  SymbolObj *port = tem->car()->asSymbol();
	  if (port) {
	    for (size_t i = 0; i < portNames.size(); i++)
	      if (portNames[i] == port) {
		conn.ports[i].labels.push_back(label);
		port = 0;
		break;
	      }
	    if (port) {
	      vm().interp->setNextLocation(loc);
	      vm().interp->message(InterpreterMessages::contentMapBadPort,
		                   StringMessageArg(*port->name()));
	    }
	  }
	  else if (tem->car() == vm().interp->makeFalse())
	    conn.principalPortLabels.push_back(label);
	  else
	    badContentMap(badFlag, loc);
	  if (!tem->cdr()->isNil())
	    badContentMap(badFlag, loc);
	}
	else
	  badContentMap(badFlag, loc);
      }
      else
	badContentMap(badFlag, loc);
    }
    else
      badContentMap(badFlag, loc);
  }
}

void ProcessContext::endMapContent()
{
  if (connectableStack_.head()->ports.size() == 0)
    delete connectableStack_.get();
}
 
void ProcessContext::badContentMap(bool &badFlag, const Location &loc)
{
  if (badFlag)
    return;
  badFlag = 1;
  vm().interp->setNextLocation(loc);
  vm().interp->message(InterpreterMessages::badContentMap);
}

ProcessContext::Connectable::Connectable(int nPorts, const StyleStack &ss, unsigned fol)
: ports(nPorts), styleStack(ss), flowObjLevel(fol)
{
}

ProcessContext::Port::Port()
: connected(0), fotb(0)
{
}

void ProcessContext::trace(Collector &c) const
{
  for (IListIter<Connection> iter(connectionStack_); !iter.done(); iter.next())
    iter.cur()->styleStack.trace(c);
  for (IListIter<Connectable> iter(connectableStack_); !iter.done(); iter.next())
    iter.cur()->styleStack.trace(c);
  for (IListIter<Table> iter(tableStack_); !iter.done(); iter.next()) {
    c.trace(iter.cur()->rowStyle);
    Vector<Vector<StyleObj *> > &styles = iter.cur()->columnStyles;
    for (size_t i = 0; i < styles.size(); i++)
      for (size_t j = 0; j < styles[i].size(); j++)
        c.trace(styles[i][j]);
  }
}
SosofoObj *SosofoObj::asSosofo()
{
  return this;
}

bool SosofoObj::tableBorderStyle(StyleObj *&)
{
  return 0;
}

bool SosofoObj::isRule()
{
  return 0;
}

bool SosofoObj::ruleStyle(ProcessContext &, StyleObj *&)
{
  return 0;
}

bool SosofoObj::isCharacter()
{
  return 0;
}

bool SosofoObj::characterStyle(ProcessContext &, StyleObj *&, FOTBuilder::CharacterNIC &)
{
  return 0;
}

void AppendSosofoObj::process(ProcessContext &context)
{
  for (size_t i = 0; i < v_.size(); i++)
    v_[i]->process(context);
}

void AppendSosofoObj::traceSubObjects(Collector &c) const
{
  for (size_t i = 0; i < v_.size(); i++)
    c.trace(v_[i]);
}

void LiteralSosofoObj::process(ProcessContext &context)
{
  const Char *s;
  size_t n;
  if (str_->stringData(s, n))
    context.currentFOTBuilder().characters(s, n);
}

void LiteralSosofoObj::traceSubObjects(Collector &c) const
{
  c.trace(str_);
}

void ProcessChildrenSosofoObj::process(ProcessContext &context)
{
  NodePtr node(context.vm().currentNode);
  context.processChildren(mode_);
  context.vm().currentNode = node;
}

void ProcessChildrenTrimSosofoObj::process(ProcessContext &context)
{
  NodePtr node(context.vm().currentNode);
  context.processChildrenTrim(mode_);
  context.vm().currentNode = node;
}

NextMatchSosofoObj::NextMatchSosofoObj(StyleObj *style)
: style_(style)
{
}

void NextMatchSosofoObj::process(ProcessContext &context)
{
  context.nextMatch(style_);
}

void EmptySosofoObj::process(ProcessContext &)
{
  // nothing needed
}

ProcessNodeListSosofoObj::ProcessNodeListSosofoObj(NodeListObj *nodeList,
						   const ProcessingMode *mode)
: nodeList_(nodeList), mode_(mode)
{
  hasSubObjects_ = 1;
}

void ProcessNodeListSosofoObj::process(ProcessContext &context)
{
  NodeListObj *nl = nodeList_;
  Interpreter &interp = *context.vm().interp;
  ELObjDynamicRoot protect(interp, nl);
  for (;;) {
    NodePtr node = nl->nodeListFirst(context.vm(), interp);
    if (!node)
      break;
    bool chunk;
    nl = nl->nodeListChunkRest(context.vm(), interp, chunk);
    protect = nl;
    context.processNodeSafe(node, mode_, chunk);
  }
}

void ProcessNodeListSosofoObj::traceSubObjects(Collector &c) const
{
  c.trace(nodeList_);
}

ProcessNodeSosofoObj::ProcessNodeSosofoObj(const NodePtr &node, const ProcessingMode *mode)
: node_(node), mode_(mode)
{
}

void ProcessNodeSosofoObj::process(ProcessContext &context)
{
  context.processNode(node_, mode_);
}

void CurrentNodePageNumberSosofoObj::process(ProcessContext &context)
{
  context.currentFOTBuilder().currentNodePageNumber(node_);
}

void PageNumberSosofoObj::process(ProcessContext &context)
{
  context.currentFOTBuilder().pageNumber();
}

SetNonInheritedCsSosofoObj
::SetNonInheritedCsSosofoObj(FlowObj *flowObj, const InsnPtr &code, ELObj **display)
: flowObj_(flowObj), code_(code), display_(display)
{
  hasSubObjects_ = 1;
}

SetNonInheritedCsSosofoObj::~SetNonInheritedCsSosofoObj()
{
  delete [] display_;
}

ELObj *SetNonInheritedCsSosofoObj::resolve(ProcessContext &context)
{
  VM &vm = context.vm();
  StyleStack *saveStyleStack = vm.styleStack;
  vm.styleStack = &context.currentStyleStack();
  unsigned saveSpecLevel = vm.specLevel;
  vm.specLevel = vm.styleStack->level();
  Vector<size_t> dep;
  vm.actualDependencies = &dep;
  ELObj *obj = vm.eval(code_.pointer(), display_, flowObj_->copy(*vm.interp));
  vm.styleStack = saveStyleStack;
  vm.specLevel = saveSpecLevel;
  if (vm.interp->isError(obj))
    return 0;
  return obj;
}

void SetNonInheritedCsSosofoObj::process(ProcessContext &context)
{
  context.startFlowObj();
  unsigned flags = 0;
  flowObj_->pushStyle(context, flags);
  ELObj *obj = resolve(context);
  if (obj) {
    ELObjDynamicRoot protect(*context.vm().interp, obj);
    ((FlowObj *)obj)->processInner(context);
  }
  flowObj_->popStyle(context, flags);
  context.endFlowObj();
}

bool SetNonInheritedCsSosofoObj::isCharacter()
{
  return flowObj_->isCharacter();
}

bool SetNonInheritedCsSosofoObj::isRule()
{
  return flowObj_->isRule();
}

bool SetNonInheritedCsSosofoObj::characterStyle(ProcessContext &context, StyleObj *&style, FOTBuilder::CharacterNIC &nic)
{
  ELObj *obj = resolve(context);
  if (obj) {
    ELObjDynamicRoot protect(*context.vm().interp, obj);
    return ((SosofoObj *)obj)->characterStyle(context, style, nic);
  }
  return 0;
}

bool SetNonInheritedCsSosofoObj::ruleStyle(ProcessContext &context, StyleObj *&style)
{
  ELObj *obj = resolve(context);
  if (obj) {
    ELObjDynamicRoot protect(*context.vm().interp, obj);
    return ((SosofoObj *)obj)->ruleStyle(context, style);
  }
  return 0;
}

void SetNonInheritedCsSosofoObj::traceSubObjects(Collector &c) const
{
  c.trace(flowObj_);
  if (display_)
    for (ELObj **p = display_; *p; p++)
      c.trace(*p);
}

LabelSosofoObj::LabelSosofoObj(SymbolObj *label, const Location &loc, SosofoObj *content)
: label_(label), locp_(new Location(loc)), content_(content)
{
  hasSubObjects_ = 1;
}

void LabelSosofoObj::process(ProcessContext &context)
{
  context.startConnection(label_, *locp_);
  content_->process(context);
  context.endConnection();
}

void LabelSosofoObj::traceSubObjects(Collector &c) const
{
  // Symbols are permanent and don't need tracing.
  c.trace(content_);
}

ContentMapSosofoObj::ContentMapSosofoObj(ELObj *contentMap,
					 const Location *locp, SosofoObj *content)
: contentMap_(contentMap), locp_(locp), content_(content)
{
  hasSubObjects_ = 1;
}

void ContentMapSosofoObj::process(ProcessContext &context)
{
  context.startMapContent(contentMap_, *locp_);
  content_->process(context);
  context.endMapContent();
}

void ContentMapSosofoObj::traceSubObjects(Collector &c) const
{
  c.trace(contentMap_);
  c.trace(content_);
}

DiscardLabeledSosofoObj::DiscardLabeledSosofoObj(SymbolObj *label, SosofoObj *content)
: label_(label), content_(content)
{
  hasSubObjects_ = 1;
}

void DiscardLabeledSosofoObj::process(ProcessContext &context)
{
  context.startDiscardLabeled(label_);
  content_->process(context);
  context.endDiscardLabeled();
}

void DiscardLabeledSosofoObj::traceSubObjects(Collector &c) const
{
  c.trace(content_);
}

PageTypeSosofoObj::PageTypeSosofoObj(unsigned pageTypeFlag, SosofoObj *match, SosofoObj *noMatch)
: pageTypeFlag_(pageTypeFlag), match_(match), noMatch_(noMatch)
{
  hasSubObjects_ = 1;
}

void PageTypeSosofoObj::process(ProcessContext &context)
{
  unsigned pageType;
  if (context.getPageType(pageType)) {
    if (pageType & pageTypeFlag_)
      match_->process(context);
    else
      noMatch_->process(context);
  }
}

void PageTypeSosofoObj::traceSubObjects(Collector &c) const
{
  c.trace(match_);
  c.trace(noMatch_);
}

#ifdef DSSSL_NAMESPACE
}
#endif
