// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef SosofoObj_INCLUDED
#define SosofoObj_INCLUDED 1

#include "ELObj.h"
#include "Location.h"
#include "FOTBuilder.h"
#include "Owner.h"
#include "CopyOwner.h"
#include "Insn.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class ProcessContext;
class Interpreter;

class SosofoObj : public ELObj {
public:
  virtual void process(ProcessContext &) = 0;
  SosofoObj *asSosofo();
  virtual bool tableBorderStyle(StyleObj *&);
  virtual bool ruleStyle(ProcessContext &, StyleObj *&);
  virtual bool isRule();
  virtual bool characterStyle(ProcessContext &, StyleObj *&, FOTBuilder::CharacterNIC &);
  virtual bool isCharacter();
};

class NextMatchSosofoObj : public SosofoObj {
public:
  NextMatchSosofoObj(StyleObj *);
  void process(ProcessContext &);
private:
  StyleObj *style_;
};

class EmptySosofoObj : public SosofoObj {
public:
  void process(ProcessContext &);
};

class ProcessingMode;

class ProcessNodeSosofoObj : public SosofoObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  ProcessNodeSosofoObj(const NodePtr &node, const ProcessingMode *mode);
  void process(ProcessContext &);
private:
  NodePtr node_;
  const ProcessingMode *mode_;
};

class ProcessNodeListSosofoObj : public SosofoObj {
public:
  ProcessNodeListSosofoObj(NodeListObj *, const ProcessingMode *);
  void process(ProcessContext &);
  void traceSubObjects(Collector &) const;
private:
  NodeListObj *nodeList_;
  const ProcessingMode *mode_;
};

class AppendSosofoObj : public SosofoObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  AppendSosofoObj() { hasSubObjects_ = 1; }
  void process(ProcessContext &);
  void append(SosofoObj *);
  void traceSubObjects(Collector &) const;
private:
  Vector<SosofoObj *> v_;
};

class LiteralSosofoObj : public SosofoObj {
public:
  LiteralSosofoObj(ELObj *str) : str_(str) { hasSubObjects_ = 1; }
  void process(ProcessContext &);
  void traceSubObjects(Collector &) const;
private:
  ELObj *str_;
};

class CurrentNodePageNumberSosofoObj : public SosofoObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  CurrentNodePageNumberSosofoObj(const NodePtr &node) : node_(node) { }
  void process(ProcessContext &);
private:
  NodePtr node_;
};

class PageNumberSosofoObj : public SosofoObj {
public:
  PageNumberSosofoObj() { }
  void process(ProcessContext &);
};

class CompoundFlowObj;

class FlowObj : public SosofoObj {
public:
  FlowObj();
  virtual FlowObj *copy(Collector &) const = 0;
  virtual CompoundFlowObj *asCompoundFlowObj();
  virtual bool hasNonInheritedC(const Identifier *) const;
  virtual bool hasPseudoNonInheritedC(const Identifier *) const;
  virtual void setNonInheritedC(const Identifier *, ELObj *, const Location &, Interpreter &);
  void traceSubObjects(Collector &) const;
  void setStyle(StyleObj *);
  void process(ProcessContext &);
  virtual void pushStyle(ProcessContext &, unsigned &);
  virtual void popStyle(ProcessContext &, unsigned);
  virtual void processInner(ProcessContext &) = 0;
protected:
  static
    bool setDisplayNIC(FOTBuilder::DisplayNIC &,
                       const Identifier *, ELObj *,
		       const Location &, Interpreter &);
  static bool isDisplayNIC(const Identifier *);
  StyleObj *style_;
};

class CompoundFlowObj : public FlowObj {
public:
  CompoundFlowObj() : content_(0) { }
  void processInner(ProcessContext &);
  void traceSubObjects(Collector &) const;
  void setContent(SosofoObj *content) { content_ = content; }
  CompoundFlowObj *asCompoundFlowObj();
protected:
  SosofoObj *content() const;
private:
  SosofoObj *content_;
};

class SequenceFlowObj : public CompoundFlowObj {
public:
  SequenceFlowObj() { }
  void processInner(ProcessContext &);
  FlowObj *copy(Collector &) const;
};

class ProcessingMode;

class ProcessChildrenSosofoObj : public SosofoObj {
public:
  ProcessChildrenSosofoObj(const ProcessingMode *mode) : mode_(mode) { }
  void process(ProcessContext &);
private:
  const ProcessingMode *mode_;
};

class ProcessChildrenTrimSosofoObj : public SosofoObj {
public:
  ProcessChildrenTrimSosofoObj(const ProcessingMode *mode) : mode_(mode) { }
  void process(ProcessContext &);
private:
  const ProcessingMode *mode_;
};

class SetNonInheritedCsSosofoObj : public SosofoObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  SetNonInheritedCsSosofoObj(FlowObj *, const InsnPtr &, ELObj **);
  ~SetNonInheritedCsSosofoObj();
  void process(ProcessContext &);
  void traceSubObjects(Collector &) const;
  bool characterStyle(ProcessContext &, StyleObj *&, FOTBuilder::CharacterNIC &);
  bool isCharacter();
  bool ruleStyle(ProcessContext &, StyleObj *&);
  bool isRule();
private:
  ELObj *resolve(ProcessContext &);

  FlowObj *flowObj_;
  ELObj **display_;
  InsnPtr code_;
};

class Expression;

// This wraps a flow object that has a label:.
class LabelSosofoObj : public SosofoObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  LabelSosofoObj(SymbolObj *, const Location &, SosofoObj *content);
  void process(ProcessContext &);
  void traceSubObjects(Collector &) const;
private:
  SymbolObj *label_;
  Owner<Location> locp_;
  SosofoObj *content_;
};

// This wraps the content of a sosofo with a content-map:.

class ContentMapSosofoObj : public SosofoObj {
public:
  ContentMapSosofoObj(ELObj *, const Location *, SosofoObj *content);
  void process(ProcessContext &);
  void traceSubObjects(Collector &) const;
private:
  ELObj *contentMap_;
  const Location *locp_;
  SosofoObj *content_; 
};

class DiscardLabeledSosofoObj : public SosofoObj {
public:
  DiscardLabeledSosofoObj(SymbolObj *, SosofoObj *);
  void process(ProcessContext &);
  void traceSubObjects(Collector &) const;
private:
  SymbolObj *label_;
  SosofoObj *content_; 
};

class PageTypeSosofoObj : public SosofoObj {
public:
  PageTypeSosofoObj(unsigned, SosofoObj *, SosofoObj *);
  void process(ProcessContext &);
  void traceSubObjects(Collector &) const;
private:
  unsigned pageTypeFlag_;
  SosofoObj *match_;
  SosofoObj *noMatch_;
};

inline
void FlowObj::setStyle(StyleObj *style)
{
  style_ = style;
}

inline
void AppendSosofoObj::append(SosofoObj *obj)
{
  v_.push_back(obj);
}

inline
SosofoObj *CompoundFlowObj::content() const
{
  return content_;
}

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not SosofoObj_INCLUDED */

