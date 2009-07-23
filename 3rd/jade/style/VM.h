// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef VM_INCLUDED
#define VM_INCLUDED 1

#include "Collector.h"
#include "EvalContext.h"
#include "Location.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class ELObj;
class ContinuationObj;
class Interpreter;
class Insn;

class VM : public EvalContext, private Collector::DynamicRoot  {
public:
  VM(Interpreter &);
  VM(EvalContext &, Interpreter &);
  virtual ~VM();
  ELObj **sp;
  Interpreter *interp;
  ELObj **closure;
  ELObj *protectClosure;
  ELObj **frame;
  int nActualArgs;
  Location closureLoc;
  ELObj *eval(const Insn *, ELObj **display = 0, ELObj *arg = 0);
  void initStack();
  void needStack(int);
  void pushFrame(const Insn *next, int argsPushed);
  const Insn *popFrame();
  void setClosureArgToCC();
  void trace(Collector &) const;
  Vector<const ProcessingMode *> modeStack;
private:
  void growStack(int);
  void init();
  void stackTrace();

  ELObj **slim;
  ELObj **sbase;

  struct ControlStackEntry {
    int frameSize;		// before pushing args
    ELObj **closure;
    ELObj *protectClosure;
    Location closureLoc;
    ContinuationObj *continuation;
    const Insn *next;
  };

  ControlStackEntry *csp;
  ControlStackEntry *csbase;
  ControlStackEntry *cslim;
  friend class ContinuationObj;
};

inline
void VM::needStack(int n)
{
  if (slim - sp < n)
    growStack(n);
}

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not VM_INCLUDED */
