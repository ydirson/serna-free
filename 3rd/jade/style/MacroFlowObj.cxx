// Copyright (c) 1997 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "MacroFlowObj.h"
#include "VM.h"
#include "Insn2.h"
#include "Insn.h"
#include "Expression.h"
#include "Interpreter.h"
#include "ProcessContext.h"
#include "macros.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

MacroFlowObj::MacroFlowObj(Vector<const Identifier *> &nics,
			   NCVector<Owner<Expression> > &inits,
			   const Identifier *contentsId,
			   Owner<Expression> &body)
: def_(new Definition(nics, inits, contentsId, body))
{
  size_t n = def_->nics().size();
  charicVals_ = new ELObj *[n];
  for (size_t i = 0; i < n; i++)
    charicVals_[i] = 0;
}

MacroFlowObj::MacroFlowObj(const MacroFlowObj &obj)
: CompoundFlowObj(obj), def_(obj.def_)
{
  size_t n = def_->nics().size();
  charicVals_ = new ELObj *[n];
  for (size_t i = 0; i < n; i++)
    charicVals_[i] = obj.charicVals_[i];
}

MacroFlowObj::~MacroFlowObj()
{
  delete [] charicVals_;
}

FlowObj *MacroFlowObj::copy(Collector &c) const
{
  return new (c) MacroFlowObj(*this);
}

CompoundFlowObj *MacroFlowObj::asCompoundFlowObj()
{
  if (def_->isCompound())
    return this;
  else
    return 0;
}

bool MacroFlowObj::hasNonInheritedC(const Identifier *id) const
{
  const Vector<const Identifier *> &nics = def_->nics();
  for (size_t i = 0; i < nics.size(); i++)
    if (nics[i] == id)
      return 1;
  return 0;
}

void MacroFlowObj::setNonInheritedC(const Identifier *id, ELObj *obj, const Location &, Interpreter &)
{
  const Vector<const Identifier *> &nics = def_->nics();
  for (size_t i = 0;; i++) {
    if (nics[i] == id) {
      charicVals_[i] = obj;
      return;
    }
  }
}

void MacroFlowObj::traceSubObjects(Collector &c) const
{
  size_t n = def_->nics().size();
  for (size_t i = 0; i < n; i++)
    c.trace(charicVals_[i]);
  CompoundFlowObj::traceSubObjects(c);
}

class EmptyStyleObj : public StyleObj {
public:
  void appendIter(StyleObjIter &) const { }
};

void MacroFlowObj::unpack(VM &vm)
{
  size_t n = def_->nics().size();
  vm.needStack(n + 1 + def_->isCompound());
  for (size_t i = 0; i < n; i++)
    *vm.sp++ = charicVals_[i];
  if (def_->isCompound()) {
    ELObj *tem = content();
    if (!tem)
      tem = new (*vm.interp) ProcessChildrenSosofoObj(vm.interp->initialProcessingMode());
    *vm.sp++ = tem;
  }
}

void MacroFlowObj::processInner(ProcessContext &context)
{
  FOTBuilder &fotb = context.currentFOTBuilder();
  fotb.startSequence();
  def_->process(context, this);
  fotb.endSequence();
}

MacroFlowObj::Definition::Definition(Vector<const Identifier *> &charics,
				     NCVector<Owner<Expression> > &charicInits,
				     const Identifier *contentsId,
				     Owner<Expression> &body)
: contentsId_(contentsId)
{
  charics.swap(charics_);
  charicInits.swap(charicInits_);
  charicInits_.resize(charics_.size());
  body.swap(body_);
}

void MacroFlowObj::Definition::process(ProcessContext &context,
				       MacroFlowObj *macro)
{
  VM &vm = context.vm();
  Interpreter &interp = *vm.interp;;
  if (code_.isNull())
    compile(interp);
  StyleStack *saveStyleStack = vm.styleStack;
  vm.styleStack = &context.currentStyleStack();
  unsigned saveSpecLevel = vm.specLevel;
  vm.specLevel = vm.styleStack->level();
  Vector<size_t> dep;
  vm.actualDependencies = &dep;
  ELObj *obj = context.vm().eval(code_.pointer(), 0, macro);
  vm.styleStack = saveStyleStack;
  vm.specLevel = saveSpecLevel;
  if (!interp.isError(obj)) {
    ELObjDynamicRoot protect(interp, obj);
    ((SosofoObj *)obj)->process(context);
  }
}

class UnpackMacroFlowObjInsn : public Insn {
public:
  UnpackMacroFlowObjInsn(InsnPtr next) : next_(next) { }
  const Insn *execute(VM &vm) const {
    ((MacroFlowObj *)*--vm.sp)->unpack(vm);
    return next_.pointer();
  }
private:
  InsnPtr next_;
};

void MacroFlowObj::Definition::compile(Interpreter &interp)
{
  InsnPtr result;
  result = new CheckSosofoInsn(body_->location(), result);
  int nPush = charics_.size() + (contentsId_ != 0);
  result = PopBindingsInsn::make(nPush, result);
  BoundVarList frameVars;
  for (size_t i = 0; i < charics_.size(); i++) {
    if (i > 0 && charicInits_[i])
      charicInits_[i]->markBoundVars(frameVars, 0);
    frameVars.append(charics_[i], 0);
  }
  if (contentsId_)
    frameVars.append(contentsId_, 0);
  body_->markBoundVars(frameVars, 0);
  result = Expression::optimizeCompile(body_, interp,
				       Environment(frameVars, BoundVarList()),
				       nPush, result);
  for (size_t i = charics_.size(); i > 0; i--) {
    int stackOffset = int(i) - nPush - 1;
    if (frameVars[i - 1].boxed())
      result = new BoxStackInsn(stackOffset, result);
    InsnPtr ifNull = new SetKeyArgInsn(stackOffset, result);
    if (charicInits_[i - 1]) {
      BoundVarList f(frameVars);
      f.resize(i - 1);
      ifNull = Expression::optimizeCompile(charicInits_[i - 1], interp,
				           Environment(f, BoundVarList()),
					   nPush, ifNull);
    }
    else
      ifNull = new ConstantInsn(interp.makeFalse(), ifNull);
    result = new TestNullInsn(stackOffset, ifNull, result);
  }
  code_ = new UnpackMacroFlowObjInsn(result);
}

#ifdef DSSSL_NAMESPACE
}
#endif
