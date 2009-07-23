// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef Insn2_INCLUDED
#define Insn2_INCLUDED 1

#include "Insn.h"
#include "SosofoObj.h"
#include "Style.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class CheckSosofoInsn : public Insn {
public:
  CheckSosofoInsn(const Location &loc, InsnPtr next) : loc_(loc), next_(next) { }
  const Insn *execute(VM &vm) const;
private:
  Location loc_;
  InsnPtr next_;
};

class CheckStyleInsn : public Insn {
public:
  CheckStyleInsn(const Location &loc, InsnPtr next) : loc_(loc), next_(next) { }
  const Insn *execute(VM &vm) const;
private:
  Location loc_;
  InsnPtr next_;
};

class ProcessingMode;

class PushModeInsn : public Insn {
public:
  PushModeInsn(const ProcessingMode *mode, InsnPtr next)
    : mode_(mode), next_(next) { }
  const Insn *execute(VM &) const;
private:
  const ProcessingMode *mode_;
  InsnPtr next_;
};

class PopModeInsn : public Insn {
public:
  PopModeInsn(InsnPtr next) : next_(next) { }
  const Insn *execute(VM &) const;
private:
  InsnPtr next_;
};

// This creates a SetNonInheritedCsSosofo
// displayLength_ vars are on the top of the stack.
// flowObj is next.

class SetNonInheritedCsSosofoInsn : public Insn {
public:
  SetNonInheritedCsSosofoInsn(InsnPtr code, int displayLength, InsnPtr next);
  const Insn *execute(VM &) const;
private:
  InsnPtr code_;
  int displayLength_;
  InsnPtr next_;
};

class CopyFlowObjInsn : public Insn {
public:
  CopyFlowObjInsn(FlowObj *, InsnPtr);
  const Insn *execute(VM &) const;
private:
  FlowObj *flowObj_;
  InsnPtr next_;
};

// Value for characteristic is on top of the stack
// Flow object is next down.

class SetPseudoNonInheritedCInsn : public Insn {
public:
  SetPseudoNonInheritedCInsn(const Identifier *, const Location &, InsnPtr);
  const Insn *execute(VM &) const;
private:
  Location loc_;
  const Identifier *nic_;
  InsnPtr next_;
};

class SetNonInheritedCInsn : public SetPseudoNonInheritedCInsn {
public:
  SetNonInheritedCInsn(const Identifier *, const Location &, InsnPtr);
  const Insn *execute(VM &) const;
};

// sosofo with content is on top of the stack
// flow object is next

class SetContentInsn : public Insn {
public:
  SetContentInsn(const CompoundFlowObj *, InsnPtr next);
  const Insn *execute(VM &vm) const;
private:
  InsnPtr next_;
  const CompoundFlowObj *flowObj_;
};

class SetDefaultContentInsn : public Insn {
public:
  SetDefaultContentInsn(const CompoundFlowObj *, const Location &loc, InsnPtr next);
  const Insn *execute(VM &vm) const;
private:
  Location loc_;
  InsnPtr next_;
  const CompoundFlowObj *flowObj_;
};

class MakeDefaultContentInsn : public Insn {
public:
  MakeDefaultContentInsn(const Location &loc, InsnPtr next);
  const Insn *execute(VM &vm) const;
private:
  Location loc_;
  InsnPtr next_;
};

class SosofoAppendInsn : public Insn {
public:
  SosofoAppendInsn(size_t, InsnPtr);
  const Insn *execute(VM &vm) const;
private:
  size_t n_;
  InsnPtr next_;
};

// This creates a VarStyleObj.
// displayLength vars are on the top, then the use StyleObj is there is one.

class VarStyleInsn : public Insn {
public:
  VarStyleInsn(const ConstPtr<StyleSpec> &, unsigned, bool, InsnPtr);
  const Insn *execute(VM &vm) const;
private:
  unsigned displayLength_;
  bool hasUse_;
  ConstPtr<StyleSpec> styleSpec_;
  InsnPtr next_;
};

// BasicStyleObj is on top of stack.
// If there is a current overriding style,
// replace it by an OverridenStyleObj.

class MaybeOverrideStyleInsn : public Insn {
public:
  MaybeOverrideStyleInsn(InsnPtr next);
  const Insn *execute(VM &vm) const;
private:
  InsnPtr next_;
};

// StyleObj is on top
// FlowObj is next down
// calls set style on the flowobj

class SetStyleInsn : public Insn {
public:
  SetStyleInsn(InsnPtr next);
  const Insn *execute(VM &vm) const;
private:
  InsnPtr next_;
};

class Expression;

// label is on the top
// content is next down

class LabelSosofoInsn : public Insn {
public:
  LabelSosofoInsn(const Location &, InsnPtr next);
  const Insn *execute(VM &) const;
private:
  Location loc_;
  InsnPtr next_;
};

// Constructs a ContentMapSosofo
// content-map is on top
// content is next down

class ContentMapSosofoInsn : public Insn {
public:
  ContentMapSosofoInsn(const Location &loc, InsnPtr next);
  const Insn *execute(VM &) const;
private:
  Location loc_;
  InsnPtr next_;
};

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not Insn2_INCLUDED */
