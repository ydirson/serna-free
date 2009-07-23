// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "Interpreter.h"
#include "InterpreterMessages.h"
#include "EvalContext.h"
#include "SosofoObj.h"
#include "Style.h"
#include "Insn.h"
#include "macros.h"
#include "ELObjMessageArg.h"
#include "LocNode.h"
#include "VM.h"
#include "Pattern.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class DescendantsNodeListObj : public NodeListObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  DescendantsNodeListObj(const NodePtr &, unsigned = 0);
  NodePtr nodeListFirst(EvalContext &, Interpreter &);
  NodeListObj *nodeListRest(EvalContext &, Interpreter &);
  NodeListObj *nodeListChunkRest(EvalContext &, Interpreter &, bool &);
private:
  static void advance(NodePtr &, unsigned &);
  static void chunkAdvance(NodePtr &, unsigned &);
  // nodes in node list are strictly after this node
  NodePtr start_;
  unsigned depth_;
};

class SiblingNodeListObj : public NodeListObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  SiblingNodeListObj(const NodePtr &first, const NodePtr &end);
  NodePtr nodeListFirst(EvalContext &, Interpreter &);
  NodeListObj *nodeListRest(EvalContext &, Interpreter &);
  NodeListObj *nodeListChunkRest(EvalContext &, Interpreter &, bool &);
private:
  NodePtr first_;
  NodePtr end_;
};

class SelectByClassNodeListObj : public NodeListObj {
public:
  SelectByClassNodeListObj(NodeListObj *nl, ComponentName::Id);
  NodePtr nodeListFirst(EvalContext &, Interpreter &);
  NodeListObj *nodeListRest(EvalContext &, Interpreter &);
  NodeListObj *nodeListChunkRest(EvalContext &, Interpreter &, bool &);
  void traceSubObjects(Collector &) const;
private:
  NodeListObj *nodeList_;
  ComponentName::Id cls_;
};

class MapNodeListObj : public NodeListObj {
public:
  class Context : public Resource {
  public:
    Context(const EvalContext &, const Location &);
    void set(EvalContext &) const;
    void traceSubObjects(Collector &) const;
    Location loc;
  private:
    NodePtr currentNode_;
    const ProcessingMode *processingMode_;
    StyleObj *overridingStyle_;
    bool haveStyleStack_;
  };
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  MapNodeListObj(FunctionObj *func, NodeListObj *nl, const ConstPtr<Context> &, NodeListObj *mapped = 0);
  NodePtr nodeListFirst(EvalContext &, Interpreter &);
  NodeListObj *nodeListRest(EvalContext &, Interpreter &);
  void traceSubObjects(Collector &) const;
  bool suppressError();
private:
  void mapNext(EvalContext &, Interpreter &);
  FunctionObj *func_;
  NodeListObj *nl_;
  NodeListObj *mapped_;
  ConstPtr<Context> context_;
};

class SelectElementsNodeListObj : public NodeListObj {
public:
  struct PatternSet : public Resource, public NCVector<Pattern> { };
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  SelectElementsNodeListObj(NodeListObj *, NCVector<Pattern> &);
  SelectElementsNodeListObj(NodeListObj *, const ConstPtr<PatternSet> &);
  void traceSubObjects(Collector &) const;
  NodePtr nodeListFirst(EvalContext &, Interpreter &);
  NodeListObj *nodeListRest(EvalContext &, Interpreter &);
private:
  NodeListObj *nodeList_;
  ConstPtr<PatternSet> patterns_;
};

#define PRIMITIVE(name, string, nRequired, nOptional, rest) \
class name ## PrimitiveObj : public PrimitiveObj { \
public: \
  static const Signature signature_; \
  name ## PrimitiveObj() : PrimitiveObj(&signature_) { } \
  ELObj *primitiveCall(int, ELObj **, EvalContext &, Interpreter &, const Location &); \
}; \
const Signature name ## PrimitiveObj::signature_ \
  = { nRequired, nOptional, rest };

#define XPRIMITIVE PRIMITIVE
#define PRIMITIVE2 PRIMITIVE
#include "primitive.h"
#undef PRIMITIVE
#undef XPRIMITIVE
#undef PRIMITIVE2

#define DEFPRIMITIVE(name, argc, argv, context, interp, loc) \
 ELObj *name ## PrimitiveObj \
  ::primitiveCall(int argc, ELObj **argv, EvalContext &context, Interpreter &interp, \
                  const Location &loc)

DEFPRIMITIVE(Cons, argc, argv, context, interp, loc)
{
  return new (interp) PairObj(argv[0], argv[1]);
}

DEFPRIMITIVE(List, argc, argv, context, interp, loc)
{
  if (argc == 0)
    return interp.makeNil();
  PairObj *head = new (interp) PairObj(argv[0], 0);
  ELObjDynamicRoot protect(interp, head);
  PairObj *tail = head;
  for (int i = 1; i < argc; i++) {
    PairObj *tem = new (interp) PairObj(argv[i], 0);
    tail->setCdr(tem);
    tail = tem;
  }
  tail->setCdr(interp.makeNil());
  return head;
}

DEFPRIMITIVE(IsNull, argc, argv, context, interp, loc)
{
  if (argv[0]->isNil())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsList, argc, argv, context, interp, loc)
{
  ELObj *obj = argv[0];
  for (;;) {
    PairObj *pair = obj->asPair();
    if (pair)
      obj = pair->cdr();
    else if (obj->isNil())
      return interp.makeTrue();
    else
      break;
  }
  return interp.makeFalse();
}

DEFPRIMITIVE(IsEqual, argc, argv, context, interp, loc)
{
  if (ELObj::equal(*argv[0], *argv[1]))
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsEqv, argc, argv, context, interp, loc)
{
  if (ELObj::eqv(*argv[0], *argv[1]))
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(Car, argc, argv, context, interp, loc)
{
  PairObj *pair = argv[0]->asPair();
  if (!pair)
    return argError(interp, loc,
		    InterpreterMessages::notAPair, 0, argv[0]);
  else
    return pair->car();
}

DEFPRIMITIVE(Cdr, argc, argv, context, interp, loc)
{
  PairObj *pair = argv[0]->asPair();
  if (!pair)
    return argError(interp, loc,
		    InterpreterMessages::notAPair, 0, argv[0]);
  else
    return pair->cdr();
}

DEFPRIMITIVE(Append, argc, argv, context, interp, loc)
{
  if (argc == 0)
    return interp.makeNil();
  PairObj *tail = interp.makePair(0, 0);
  PairObj *head = tail;
  ELObjDynamicRoot protect(interp, head);
  for (int i = 0; i < argc - 1; i++) {
    for (ELObj *p = argv[i]; !p->isNil();) {
      PairObj *tem = p->asPair();
      if (!tem)
	return argError(interp, loc,
			InterpreterMessages::notAList, i, p);
      PairObj *newTail = new (interp) PairObj(tem->car(), 0);
      tail->setCdr(newTail);
      tail = newTail;
      p = tem->cdr();
    }
  }
  tail->setCdr(argv[argc - 1]);
  return head->cdr();
}

DEFPRIMITIVE(Reverse, argc, argv, context, interp, loc)
{
  ELObjDynamicRoot protect(interp, interp.makeNil());
  ELObj *p = argv[0];
  while (!p->isNil()) {
    PairObj *tem = p->asPair();
    if (!tem)
      return argError(interp, loc,
		      InterpreterMessages::notAList, 0, argv[0]);
    protect = new (interp) PairObj(tem->car(), protect);
    p = tem->cdr();
  }
  return protect;
}

DEFPRIMITIVE(ListTail, argc, argv, context, interp, loc)
{
  long k;
  if (!argv[1]->exactIntegerValue(k))
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 1, argv[1]);
  if (k < 0) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::outOfRange);
    return interp.makeError();
  }
  ELObj *p = argv[0];
  for (; k > 0; k--) {
    PairObj *tem = p->asPair();
    if (!tem) {
      if (p->isNil()) {
        interp.setNextLocation(loc);
	interp.message(InterpreterMessages::outOfRange);
	return interp.makeError();
      }
      else
        return argError(interp, loc,
		        InterpreterMessages::notAList, 0, argv[0]);
    }
    p = tem->cdr();
  }
  return p;
}

DEFPRIMITIVE(ListRef, argc, argv, context, interp, loc)
{
  long k;
  if (!argv[1]->exactIntegerValue(k))
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 1, argv[1]);
  if (k < 0) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::outOfRange);
    return interp.makeError();
  }
  ELObj *p = argv[0];
  for (;;) {
    PairObj *tem = p->asPair();
    if (!tem)
      break;
    if (k == 0)
      return tem->car();
    --k;
    p = tem->cdr();
  }
  if (p->isNil()) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::outOfRange);
    return interp.makeError();
  }
  else
    return argError(interp, loc,
		    InterpreterMessages::notAList, 0, argv[0]);
}

DEFPRIMITIVE(Member, argc, argv, context, interp, loc)
{
  ELObj *p = argv[1];
  while (!p->isNil()) {
    PairObj *tem = p->asPair();
    if (!tem)
      return argError(interp, loc,
		      InterpreterMessages::notAList, 1, argv[1]);
    if (ELObj::equal(*argv[0], *tem->car()))
      return p;
    p = tem->cdr();
  }
  return interp.makeFalse();
}

DEFPRIMITIVE(Memv, argc, argv, context, interp, loc)
{
  ELObj *p = argv[1];
  while (!p->isNil()) {
    PairObj *tem = p->asPair();
    if (!tem)
      return argError(interp, loc,
		      InterpreterMessages::notAList, 1, argv[1]);
    if (ELObj::eqv(*argv[0], *tem->car()))
      return p;
    p = tem->cdr();
  }
  return interp.makeFalse();
}

DEFPRIMITIVE(Length, argc, argv, context, interp, loc)
{
  ELObj *obj = argv[0];
  long n = 0;
  for (;;) {
    PairObj *pair = obj->asPair();
    if (pair) {
      n++;
      obj = pair->cdr();
    }
    else if (obj->isNil())
      break;
    else if (interp.isError(obj))
      return obj;
    else
      return argError(interp, loc,
		      InterpreterMessages::notAList, 0, obj);
  }
  return interp.makeInteger(n);
}

DEFPRIMITIVE(Not, argc, argv, context, interp, loc)
{
  if (argv[0]->isTrue())
    return interp.makeFalse();
  else
    return interp.makeTrue();
}

DEFPRIMITIVE(IsSymbol, argc, argv, context, interp, loc)
{
  if (argv[0]->asSymbol())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsKeyword, argc, argv, context, interp, loc)
{
  if (argv[0]->asKeyword())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsInteger, argc, argv, context, interp, loc)
{
  long n;
  if (argv[0]->exactIntegerValue(n))
    return interp.makeTrue();
  double x;
  if (argv[0]->realValue(x) && modf(x, &x) == 0.0)
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsReal, argc, argv, context, interp, loc)
{
  double x;
  if (argv[0]->realValue(x))
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsNumber, argc, argv, context, interp, loc)
{
  double x;
  if (argv[0]->realValue(x))
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsQuantity, argc, argv, context, interp, loc)
{
  long n;
  double d;
  int dim;
  if (argv[0]->quantityValue(n, d, dim) != ELObj::noQuantity)
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsPair, argc, argv, context, interp, loc)
{
  if (argv[0]->asPair())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsProcedure, argc, argv, context, interp, loc)
{
  if (argv[0]->asFunction())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsBoolean, argc, argv, context, interp, loc)
{
  if (argv[0] == interp.makeTrue())
    return argv[0];
  else if (argv[0] == interp.makeFalse())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsChar, argc, argv, context, interp, loc)
{
  Char c;
  if (argv[0]->charValue(c))
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsCharEqual, argc, argv, context, interp, loc)
{
  Char c1, c2;
  if (!argv[0]->charValue(c1))
    return argError(interp, loc,
		    InterpreterMessages::notAChar, 0, argv[0]);
  if (!argv[1]->charValue(c2))
    return argError(interp, loc,
		    InterpreterMessages::notAChar, 1, argv[1]);
  if (c1 == c2)
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(String, argc, argv, context, interp, loc)
{
  StringObj *obj = new (interp) StringObj;
  for (int i = 0; i < argc; i++) {
    Char c;
    if (!argv[i]->charValue(c)) 
      return argError(interp, loc,
		      InterpreterMessages::notAChar, i, argv[i]);
    *obj += c;
  }
  return obj;
}

DEFPRIMITIVE(SymbolToString, argc, argv, context, interp, loc)
{
  SymbolObj *obj = argv[0]->asSymbol();
  if (!obj)
    return argError(interp, loc,
		    InterpreterMessages::notASymbol, 0, argv[0]);
  return obj->name();
}

DEFPRIMITIVE(StringToSymbol, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  return interp.makeSymbol(StringC(s, n));
}

DEFPRIMITIVE(IsString, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (argv[0]->stringData(s, n))
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(StringLength, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  return interp.makeInteger(n);
}

DEFPRIMITIVE(IsStringEqual, argc, argv, context, interp, loc)
{
  const Char *s1, *s2;
  size_t n1, n2;
  if (!argv[0]->stringData(s1, n1))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  if (!argv[1]->stringData(s2, n2))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 1, argv[1]);
  if (n1 == n2
      && (n1 == 0 || memcmp(s1, s2, n1*sizeof(Char)) == 0))
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(StringAppend, argc, argv, context, interp, loc)
{
  StringObj *result = new (interp) StringObj;
  for (int i = 0; i < argc; i++) {
    const Char *s;
    size_t n;
    if (!argv[i]->stringData(s, n))
      return argError(interp, loc,
		      InterpreterMessages::notAString, i,
		      argv[i]);
    result->append(s, n);
  }
  return result;
}

DEFPRIMITIVE(StringRef, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  long k;
  if (!argv[1]->exactIntegerValue(k))
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 1, argv[1]);
  if (k < 0 || (unsigned long)k >= n) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::outOfRange);
    return interp.makeError();
  }
  return interp.makeChar(s[size_t(k)]);
}

DEFPRIMITIVE(Substring, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  long start;
  if (!argv[1]->exactIntegerValue(start))
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 1, argv[1]);
  long end;
  if (!argv[2]->exactIntegerValue(end))
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 2, argv[2]);
  if (start < 0 || (unsigned long)end > n || start > end) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::outOfRange);
    return interp.makeError();
  }
  return new (interp) StringObj(s + size_t(start), size_t(end - start));
}

DEFPRIMITIVE(Equal, argc, argv, context, interp, loc)
{
  if (argc == 0)
    return interp.makeTrue();
  long lResult;
  double dResult;
  int dim;
  int i = 1;
  switch (argv[0]->quantityValue(lResult, dResult, dim)) {
  case ELObj::noQuantity:
    return argError(interp, loc,
		    InterpreterMessages::notAQuantity, 0, argv[0]);
  case ELObj::longQuantity:
    break;
  case ELObj::doubleQuantity:
    goto useDouble;
    break;
  default:
    CANNOT_HAPPEN();
  }
  long lResult2;
  double dResult2;
  int dim2;
  for (; i < argc; i++) {
    switch (argv[i]->quantityValue(lResult2, dResult2, dim2)) {
    case ELObj::noQuantity:
      return argError(interp, loc,
		    InterpreterMessages::notAQuantity, i, argv[i]);
    case ELObj::longQuantity:
      if (lResult2 != lResult || dim2 != dim)
	return interp.makeFalse();
      break;
    case ELObj::doubleQuantity:
      dResult = lResult;
      if (dResult2 != dResult || dim2 != dim)
	return interp.makeFalse();
      i++;
      goto useDouble;
    default:
      CANNOT_HAPPEN();
    }
  }
  return interp.makeTrue();
 useDouble:
  for (; i < argc; i++) {
    switch (argv[i]->quantityValue(lResult2, dResult2, dim2)) {
    case ELObj::noQuantity:
      return argError(interp, loc,
		      InterpreterMessages::notAQuantity, i, argv[i]);
    case ELObj::longQuantity:
      if (lResult2 != dResult || dim2 != dim)
	return interp.makeFalse();
      break;
    case ELObj::doubleQuantity:
      if (dResult2 != dResult || dim2 != dim)
	return interp.makeFalse();
      break;
    }
  }
  return interp.makeTrue();
}

DEFPRIMITIVE(Plus, argc, argv, context, interp, loc)
{
  if (argc == 0)
    return interp.makeInteger(0);
  long lResult;
  double dResult;
  bool usingD;
  int dim;
  switch (argv[0]->quantityValue(lResult, dResult, dim)) {
  case ELObj::noQuantity:
    {
      const LengthSpec *lsp = argv[0]->lengthSpec();
      if (!lsp)
	return argError(interp, loc,
			InterpreterMessages::notAQuantityOrLengthSpec, 0, argv[0]);
      LengthSpec ls(*lsp);
      for (int i = 1; i < argc; i++) {
	lsp = argv[i]->lengthSpec();
	if (lsp)
	  ls += *lsp;
	else {
	  switch (argv[i]->quantityValue(lResult, dResult, dim)) {
	  case ELObj::noQuantity:
            return argError(interp, loc, InterpreterMessages::notAQuantityOrLengthSpec,
	  		    i, argv[i]);
	  case ELObj::longQuantity:
	    dResult = lResult;
	    // fall through
	  case ELObj::doubleQuantity:
	    if (dim != 1) {
	      interp.setNextLocation(loc);
	      interp.message(InterpreterMessages::incompatibleDimensions);
	      return interp.makeError();
	    }
	    ls += dResult;
	    break;
	  }
	}
      }
      return new (interp) LengthSpecObj(ls);
    }
  case ELObj::longQuantity:
    usingD = 0;
    break;
  case ELObj::doubleQuantity:
    usingD = 1;
    break;
  default:
    CANNOT_HAPPEN();
  }
  for (int i = 1; i < argc; i++) {
    long lResult2;
    double dResult2;
    int dim2;
    switch (argv[i]->quantityValue(lResult2, dResult2, dim2)) {
    case ELObj::noQuantity:
      return argError(interp, loc, InterpreterMessages::notAQuantity,
		      i, argv[i]);
    case ELObj::longQuantity:
      if (!usingD) {
	if (lResult2 < 0) {
	  if (lResult >= LONG_MIN - lResult2) {
	    lResult += lResult2;
	    break;
	  }
	}
	else {
	  if (lResult <= LONG_MAX - lResult2) {
	    lResult += lResult2;
	    break;
	  }
	}
	usingD = 1;
	dResult = double(lResult);
      }
      dResult += double(lResult2);
      break;
    case ELObj::doubleQuantity:
      if (!usingD) {
	dResult = lResult;
	usingD = 1;
      }
      dResult += dResult2;
      break;
    default:
      CANNOT_HAPPEN();
    }
    if (dim2 != dim) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::incompatibleDimensions);
      return interp.makeError();
    }
  }
  if (!usingD) {
    if (dim == 0)
      return interp.makeInteger(lResult);
    else if (dim == 1)
      return new (interp) LengthObj(lResult);
    else
      dResult = lResult;
  }
  if (dim == 0)
    return new (interp) RealObj(dResult);
  else
    return new (interp) QuantityObj(dResult, dim);
}

DEFPRIMITIVE(Minus, argc, argv, context, interp, loc)
{
  long lResult;
  double dResult;
  bool usingD;
  int dim;
  switch (argv[0]->quantityValue(lResult, dResult, dim)) {
  case ELObj::noQuantity:
     {
      const LengthSpec *lsp = argv[0]->lengthSpec();
      if (!lsp)
	return argError(interp, loc,
			InterpreterMessages::notAQuantityOrLengthSpec, 0, argv[0]);
      LengthSpec ls(*lsp);
      for (int i = 1; i < argc; i++) {
	lsp = argv[i]->lengthSpec();
	if (lsp)
	  ls -= *lsp;
	else {
	  switch (argv[i]->quantityValue(lResult, dResult, dim)) {
	  case ELObj::noQuantity:
            return argError(interp, loc, InterpreterMessages::notAQuantityOrLengthSpec,
	  		    i, argv[i]);
	  case ELObj::longQuantity:
	    dResult = lResult;
	    // fall through
	  case ELObj::doubleQuantity:
	    if (dim != 1) {
	      interp.setNextLocation(loc);
	      interp.message(InterpreterMessages::incompatibleDimensions);
	      return interp.makeError();
	    }
	    ls -= dResult;
	    break;
	  }
	}
      }
      return new (interp) LengthSpecObj(ls);
    }
 case ELObj::longQuantity:
    usingD = 0;
    break;
  case ELObj::doubleQuantity:
    usingD = 1;
    break;
  default:
    CANNOT_HAPPEN();
  }
  if (argc == 1) {
    if (usingD)
      dResult = -dResult;
    else
      lResult = -lResult;
  }
  else {
    for (int i = 1; i < argc; i++) {
      long lResult2;
      double dResult2;
      int dim2;
      switch (argv[i]->quantityValue(lResult2, dResult2, dim2)) {
      case ELObj::noQuantity:
	return argError(interp, loc,
			InterpreterMessages::notAQuantity, i,
			argv[i]);
      case ELObj::longQuantity:
	if (!usingD) {
	  if (lResult2 > 0) {
	    if (lResult >= LONG_MIN + lResult2) {
	      lResult -= lResult2;
	      break;
	    }
	  }
	  else {
	    if (lResult <= LONG_MAX + lResult2) {
	      lResult -= lResult2;
	      break;
	    }
	  }
	  usingD = 1;
	  dResult = double(lResult);
	}
	dResult -= double(lResult2);
	break;
      case ELObj::doubleQuantity:
	if (!usingD) {
	  dResult = lResult;
	  usingD = 1;
	}
	dResult -= dResult2;
	break;
      default:
	CANNOT_HAPPEN();
      }
      if (dim2 != dim) {
	interp.setNextLocation(loc);
	interp.message(InterpreterMessages::incompatibleDimensions);
	return interp.makeError();
      }
    }
  }
  if (!usingD) {
    if (dim == 0)
      return interp.makeInteger(lResult);
    else if (dim == 1)
      return new (interp) LengthObj(lResult);
    else
      dResult = lResult;
  }
  if (dim == 0)
    return new (interp) RealObj(dResult);
  else
    return new (interp) QuantityObj(dResult, dim);
}

DEFPRIMITIVE(Multiply, argc, argv, context, interp, loc)
{
  if (argc == 0)
    return interp.makeInteger(1);
  long lResult;
  double dResult;
  int dim;
  int i = 1;
  switch (argv[0]->quantityValue(lResult, dResult, dim)) {
  case ELObj::noQuantity:
    {
      const LengthSpec *ls = argv[0]->lengthSpec();
      if (ls) {
	LengthSpec result(*ls);
	double d;
	for (; i < argc; i++) {
	  if (!argv[i]->realValue(d))
	    return argError(interp, loc,
			    InterpreterMessages::notANumber, 1, argv[1]);
	  result *= d;
	}
	return new (interp) LengthSpecObj(result);
      }
    }
    return argError(interp, loc,
		    InterpreterMessages::notAQuantity, 0, argv[0]);
  case ELObj::longQuantity:
    break;
  case ELObj::doubleQuantity:
    goto useDouble;
  default:
    CANNOT_HAPPEN();
  }
  long lResult2;
  double dResult2;
  int dim2;
  for (; i < argc; i++) {
    switch (argv[i]->quantityValue(lResult2, dResult2, dim2)) {
    case ELObj::noQuantity:
      return argError(interp, loc,
		      InterpreterMessages::notAQuantity, i, argv[i]);
    case ELObj::longQuantity:
      dim += dim2;
      if (dim > 1
	  || (lResult2 != 0
	      && (lResult2 < 0
		  ? (lResult > 0
		     ? lResult > -(unsigned)LONG_MIN / -(unsigned)lResult2
		     : -(unsigned)lResult > LONG_MAX / -(unsigned)lResult2)
		  : (lResult > 0
		     ? lResult > LONG_MAX / lResult2
		     : -(unsigned)lResult > -(unsigned)LONG_MIN / lResult2)))) {
	dResult = double(lResult) * lResult2;
	i++;
	goto useDouble;
      }
      lResult *= lResult2;
      break;
    case ELObj::doubleQuantity:
      dim += dim2;
      dResult = lResult * dResult2;
      i++;
      goto useDouble;
    default:
      CANNOT_HAPPEN();
    }
  }
  if (dim == 0)
    return interp.makeInteger(lResult);
  else
    return new (interp) LengthObj(lResult);
 useDouble:
  for (; i < argc; i++) {
    switch (argv[i]->quantityValue(lResult2, dResult2, dim2)) {
    case ELObj::noQuantity:
      return argError(interp, loc,
		      InterpreterMessages::notAQuantity, i, argv[i]);
    case ELObj::longQuantity:
      dResult *= lResult2;
      break;
    case ELObj::doubleQuantity:
      dResult *= dResult2;
      break;
    }
    dim += dim2;
  }
  if (dim == 0)
    return new (interp) RealObj(dResult);
  else
    return new (interp) QuantityObj(dResult, dim);
}

DEFPRIMITIVE(Divide, argc, argv, context, interp, loc)
{
  long lResult;
  double dResult;
  int dim;
  if (argc == 1) {
    switch (argv[0]->quantityValue(lResult, dResult, dim)) {
    case ELObj::noQuantity:
      return argError(interp, loc,
		      InterpreterMessages::notAQuantity, 0, argv[0]);
    case ELObj::longQuantity:
      if (lResult == 0)
	goto divide0;
      dResult = 1.0/lResult;
      break;
    case ELObj::doubleQuantity:
      if (dResult == 0.0)
	goto divide0;
      dResult = 1.0/dResult;
      break;
    default:
      CANNOT_HAPPEN();
    }
    dim = -dim;
  }
  else {
    int i = 1;
    switch (argv[0]->quantityValue(lResult, dResult, dim)) {
    case ELObj::noQuantity:
      {
	const LengthSpec *ls = argv[0]->lengthSpec();
	if (ls) {
	  LengthSpec result(*ls);
	  double d;
	  for (; i < argc; i++) {
	    if (!argv[i]->realValue(d))
	      return argError(interp, loc,
			      InterpreterMessages::notANumber, 1, argv[1]);
	    if (d == 0.0)
	      goto divide0;
	    result /= d;
	  }
	  return new (interp) LengthSpecObj(result);
	}
      }
      return argError(interp, loc,
		      InterpreterMessages::notAQuantity, 0, argv[0]);
    case ELObj::longQuantity:
      break;
    case ELObj::doubleQuantity:
      goto useDouble;
    default:
      CANNOT_HAPPEN();
    }
    long lResult2;
    double dResult2;
    int dim2;
    for (; i < argc; i++) {
      switch (argv[i]->quantityValue(lResult2, dResult2, dim2)) {
      case ELObj::noQuantity:
	return argError(interp, loc,
			InterpreterMessages::notAQuantity, 0, argv[0]);
      case ELObj::longQuantity:
	if (lResult2 == 0)
	  goto divide0;
	dim -= dim2;
	// If dim and dim2 are both 1, must goto useDouble:
	// since lengths are inexact, result must be inexact.
	if (dim2 == 0 && lResult % lResult2 == 0) {
	  lResult /= lResult2;
	  break;
	}
	dResult = double(lResult)/lResult2;
	i++;
	goto useDouble;
      case ELObj::doubleQuantity:
	dim -= dim2;
	dResult = lResult;
	if (dResult2 == 0.0)
	  goto divide0;
	dResult /= dResult2;
	i++;
	goto useDouble;
      default:
	CANNOT_HAPPEN();
      }
    }
    if (dim == 0)
      return interp.makeInteger(lResult);
    else
      return new (interp) LengthObj(lResult);
  useDouble:
    for (; i < argc; i++) {
      switch (argv[i]->quantityValue(lResult2, dResult2, dim2)) {
      case ELObj::noQuantity:
	return argError(interp, loc,
			InterpreterMessages::notAQuantity, i, argv[i]);
      case ELObj::longQuantity:
	if (lResult2 == 0)
	  goto divide0;
	dResult /= lResult2;
	break;
      case ELObj::doubleQuantity:
	dResult /= dResult2;
	if (dResult2 == 0.0)
	  goto divide0;
	break;
      }
      dim -= dim2;
    }
  }
  if (dim == 0)
    return new (interp) RealObj(dResult);
  else
    return new (interp) QuantityObj(dResult, dim);
 divide0:
  interp.setNextLocation(loc);
  interp.message(InterpreterMessages::divideBy0);
  return interp.makeError();
}

DEFPRIMITIVE(Quotient, argc, argv, context, interp, loc)
{
  long n1;
  long n2;
  if (argv[0]->exactIntegerValue(n1) && argv[1]->exactIntegerValue(n2)) {
    if (n2 == 0) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::divideBy0);
      return interp.makeError();
    }
    // This isn't strictly portable.
    return interp.makeInteger(n1 / n2);
  }
  double d1;
  if (!argv[0]->realValue(d1) || modf(d1, &d1) != 0.0)
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 0, argv[0]);
  double d2;
  if (!argv[1]->realValue(d2) || modf(d2, &d2) != 0.0)
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 1, argv[1]);
  if (d2 == 0.0) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::divideBy0);
    return interp.makeError();
  }
  return new (interp) RealObj((d1 - fmod(d1, d2))/d2);
}

DEFPRIMITIVE(Remainder, argc, argv, context, interp, loc)
{
  long n1;
  long n2;
  if (argv[0]->exactIntegerValue(n1) && argv[1]->exactIntegerValue(n2)) {
    if (n2 == 0) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::divideBy0);
      return interp.makeError();
    }
    // This isn't strictly portable.
    return interp.makeInteger(n1 % n2);
  }
  double d1;
  if (!argv[0]->realValue(d1) || modf(d1, &d1) != 0.0)
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 0, argv[0]);
  double d2;
  if (!argv[1]->realValue(d2) || modf(d2, &d2) != 0.0)
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 1, argv[1]);
  if (d2 == 0.0) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::divideBy0);
    return interp.makeError();
  }
  return new (interp) RealObj(fmod(d1, d2));
}

DEFPRIMITIVE(Modulo, argc, argv, context, interp, loc)
{
  long n1;
  long n2;
  if (argv[0]->exactIntegerValue(n1) && argv[1]->exactIntegerValue(n2)) {
    if (n2 == 0) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::divideBy0);
      return interp.makeError();
    }
    long r = n1 % n2;
    if (n2 > 0 ? r < 0 : r > 0)
      r += n2;
    return interp.makeInteger(r);
  }
  double d1;
  if (!argv[0]->realValue(d1) || modf(d1, &d1) != 0.0)
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 0, argv[0]);
  double d2;
  if (!argv[1]->realValue(d2) || modf(d2, &d2) != 0.0)
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 1, argv[1]);
  if (d2 == 0.0) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::divideBy0);
    return interp.makeError();
  }
  double r = fmod(d1, d2);
  if (d2 > 0 ? r < 0 : r > 0)
    r += d2;
  return new (interp) RealObj(r);
}

#define DEFCOMPARE(NAME, OP) \
DEFPRIMITIVE(NAME, argc, argv, context, interp, loc) \
{ \
  if (argc == 0) \
    return interp.makeTrue(); \
  long lResult; \
  double dResult; \
  int dim; \
  bool lastWasDouble; \
  switch (argv[0]->quantityValue(lResult, dResult, dim)) { \
  case ELObj::noQuantity: \
    return argError(interp, loc, \
		    InterpreterMessages::notAQuantity, 0, argv[0]); \
  case ELObj::longQuantity: \
    lastWasDouble = 0; \
    break; \
  case ELObj::doubleQuantity: \
    lastWasDouble = 1; \
    break; \
  default: \
    CANNOT_HAPPEN(); \
  } \
  for (int i = 1; i < argc; i++) { \
    long lResult2; \
    double dResult2; \
    int dim2; \
    switch (argv[i]->quantityValue(lResult2, dResult2, dim2)) { \
    case ELObj::noQuantity: \
      return argError(interp, loc, \
		      InterpreterMessages::notAQuantity, i, argv[i]); \
    case ELObj::longQuantity: \
      if (dim2 != dim) \
	goto badDim; \
      if (!(lastWasDouble \
	    ? (dResult OP lResult2) \
	    : (lResult OP lResult2))) \
	return interp.makeFalse(); \
      lResult = lResult2; \
      lastWasDouble = 0; \
      break; \
    case ELObj::doubleQuantity: \
      if (dim != dim2) \
	goto badDim; \
      if (!(lastWasDouble \
	    ? (dResult OP dResult2) \
	    : (lResult OP dResult2))) \
	return interp.makeFalse(); \
      dResult = dResult2; \
      lastWasDouble = 1; \
      break; \
    } \
  } \
  return interp.makeTrue(); \
 badDim: \
  interp.setNextLocation(loc); \
  interp.message(InterpreterMessages::incompatibleDimensions); \
  return interp.makeError(); \
}

DEFCOMPARE(Less, <)
DEFCOMPARE(Greater, >)
DEFCOMPARE(LessEqual, <=)
DEFCOMPARE(GreaterEqual, >=)

DEFPRIMITIVE(Min, argc, argv, context, interp, loc)
{
  long lResult;
  double dResult;
  int dim;
  int i = 1;
  switch (argv[0]->quantityValue(lResult, dResult, dim)) {
  case ELObj::noQuantity:
    return argError(interp, loc,
		    InterpreterMessages::notAQuantity, 0, argv[0]);
  case ELObj::longQuantity:
    break;
  case ELObj::doubleQuantity:
    goto useDouble;
  default:
    CANNOT_HAPPEN();
  }
  // Note that result is inexact if any of the arguments are
  for (; i < argc; i++) {
    long lResult2;
    double dResult2;
    int dim2;
    switch (argv[i]->quantityValue(lResult2, dResult2, dim2)) {
    case ELObj::noQuantity:
      return argError(interp, loc,
		      InterpreterMessages::notAQuantity, i, argv[i]);
    case ELObj::longQuantity:
      if (dim2 != dim)
	goto badDim;
      if (lResult2 < lResult)
	lResult = lResult2;
      break;
    case ELObj::doubleQuantity:
      if (dim != dim2)
	goto badDim;
      if (dResult2 < lResult)
	dResult = dResult2;
      else if (dim)
	break;
      else
	dResult = lResult;
      i++;
      goto useDouble;
    }
  }
  if (dim == 0)
    return interp.makeInteger(lResult);
  else
    return new (interp) LengthObj(lResult);
 useDouble:
  for (; i < argc; i++) {
    long lResult2;
    double dResult2;
    int dim2;
    switch (argv[i]->quantityValue(lResult2, dResult2, dim2)) {
    case ELObj::noQuantity:
      return argError(interp, loc,
		      InterpreterMessages::notAQuantity, i, argv[i]);
    case ELObj::longQuantity:
      if (dim2 != dim)
	goto badDim;
      if (lResult2 < dResult)
	dResult = lResult2;
      break;
    case ELObj::doubleQuantity:
      if (dim != dim2)
	goto badDim;
      if (dResult2 < dResult)
	dResult = dResult2;
      break;
    }
  }
  if (dim == 0)
    return new (interp) RealObj(dResult);
  else
    return new (interp) QuantityObj(dResult, dim);
 badDim:
  interp.setNextLocation(loc);
  interp.message(InterpreterMessages::incompatibleDimensions);
  return interp.makeError();
}

DEFPRIMITIVE(Max, argc, argv, context, interp, loc)
{
  long lResult;
  double dResult;
  int dim;
  int i = 1;
  switch (argv[0]->quantityValue(lResult, dResult, dim)) {
  case ELObj::noQuantity:
    return argError(interp, loc,
		    InterpreterMessages::notAQuantity, 0, argv[0]);
  case ELObj::longQuantity:
    break;
  case ELObj::doubleQuantity:
    goto useDouble;
  default:
    CANNOT_HAPPEN();
  }
  // Note that result is inexact if any of the arguments are
  for (; i < argc; i++) {
    long lResult2;
    double dResult2;
    int dim2;
    switch (argv[i]->quantityValue(lResult2, dResult2, dim2)) {
    case ELObj::noQuantity:
      return argError(interp, loc,
		      InterpreterMessages::notAQuantity, i, argv[i]);
    case ELObj::longQuantity:
      if (dim2 != dim)
	goto badDim;
      if (lResult2 > lResult)
	lResult = lResult2;
      break;
    case ELObj::doubleQuantity:
      if (dim != dim2)
	goto badDim;
      if (dResult2 > lResult)
	dResult = dResult2;
      else if (dim)
	break;
      else
	dResult = lResult;
      i++;
      goto useDouble;
    }
  }
  if (dim == 0)
    return interp.makeInteger(lResult);
  else
    return new (interp) LengthObj(lResult);
 useDouble:
  for (; i < argc; i++) {
    long lResult2;
    double dResult2;
    int dim2;
    switch (argv[i]->quantityValue(lResult2, dResult2, dim2)) {
    case ELObj::noQuantity:
      return argError(interp, loc,
		      InterpreterMessages::notAQuantity, i, argv[i]);
    case ELObj::longQuantity:
      if (dim2 != dim)
	goto badDim;
      if (lResult2 > dResult)
	dResult = lResult2;
      break;
    case ELObj::doubleQuantity:
      if (dim != dim2)
	goto badDim;
      if (dResult2 > dResult)
	dResult = dResult2;
      break;
    }
  }
  if (dim == 0)
    return new (interp) RealObj(dResult);
  else
    return new (interp) QuantityObj(dResult, dim);
 badDim:
  interp.setNextLocation(loc);
  interp.message(InterpreterMessages::incompatibleDimensions);
  return interp.makeError();
}

DEFPRIMITIVE(Floor, argc, argv, context, interp, loc)
{
  double d;
  if (argv[0]->inexactRealValue(d))
    return new (interp) RealObj(floor(d));
  long n;
  if (argv[0]->exactIntegerValue(n))
    return argv[0];
  return argError(interp, loc,
		  InterpreterMessages::notANumber, 0, argv[0]);
}

DEFPRIMITIVE(Ceiling, argc, argv, context, interp, loc)
{
  double d;
  if (argv[0]->inexactRealValue(d))
    return new (interp) RealObj(ceil(d));
  long n;
  if (argv[0]->exactIntegerValue(n))
    return argv[0];
  return argError(interp, loc,
		  InterpreterMessages::notANumber, 0, argv[0]);
}

DEFPRIMITIVE(Round, argc, argv, context, interp, loc)
{
  double d;
  if (argv[0]->inexactRealValue(d)) {
    double result = floor(d + .5);
    // That rounded it upwards.
    // Now figure out if that was different from round to
    // even.
    if (result - d == 0.5 && fmod(result, 2.0) != 0)
      result -= 1.0;
    return new (interp) RealObj(result);
  }
  long n;
  if (argv[0]->exactIntegerValue(n))
    return argv[0];
  return argError(interp, loc,
		  InterpreterMessages::notANumber, 0, argv[0]);
}

DEFPRIMITIVE(Truncate, argc, argv, context, interp, loc)
{
  double d;
  if (argv[0]->inexactRealValue(d)) {
    double iPart;
    modf(d, &iPart);
    return new (interp) RealObj(iPart);
  }
  long n;
  if (argv[0]->exactIntegerValue(n))
    return argv[0];
  return argError(interp, loc,
		  InterpreterMessages::notANumber, 0, argv[0]);
}

DEFPRIMITIVE(Abs, argc, argv, context, interp, loc)
{
  long lResult;
  double dResult;
  int dim;
  switch (argv[0]->quantityValue(lResult, dResult, dim)) {
  case ELObj::noQuantity:
    return argError(interp, loc,
		    InterpreterMessages::notAQuantity, 0, argv[0]);
  case ELObj::longQuantity:
    if (lResult != LONG_MIN) {
      if (lResult >= 0)
	return argv[0];
      if (dim == 0)
	return interp.makeInteger(-lResult);
      else
	return new (interp) LengthObj(-lResult);
    }
    dResult = lResult;
    break;
  case ELObj::doubleQuantity:
    break;
  default:
    CANNOT_HAPPEN();
  }
  if (dResult >= 0)
    return argv[0];
  if (dim == 0)
    return new (interp) RealObj(-dResult);
  else
    return new (interp) QuantityObj(-dResult, dim);
}

DEFPRIMITIVE(Sqrt, argc, argv, context, interp, loc)
{
  long lResult;
  double dResult;
  int dim;
  ELObj::QuantityType type
    = argv[0]->quantityValue(lResult, dResult, dim);
  switch (type) {
  case ELObj::noQuantity:
    return argError(interp, loc,
		    InterpreterMessages::notAQuantity, 0, argv[0]);
  case ELObj::longQuantity:
    dResult = lResult;
    break;
  case ELObj::doubleQuantity:
    break;
  default:
    CANNOT_HAPPEN();
  }
  if ((dim & 1) || dResult < 0.0) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::outOfRange);
    return interp.makeError();
  }
  dim /= 2;
  dResult = sqrt(dResult);
  if (type == ELObj::longQuantity && dim == 0) {
    long n = long(dResult);
    if (n*n == lResult)
      return interp.makeInteger(n);
  }
  return new (interp) QuantityObj(dResult, dim);
}

DEFPRIMITIVE(Time, argc, argv, context, interp, loc)
{
  // This assumes a Posix compatible time().
  time_t t = time(0);
  return interp.makeInteger(long(t));
}

DEFPRIMITIVE(TimeToString, argc, argv, context, interp, loc)
{
  long k;
  if (!argv[0]->exactIntegerValue(k))
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 0, argv[0]);
  time_t t = time_t(k);
  const struct tm *p;
  if (argc > 1 && argv[1] != interp.makeFalse())
    p = gmtime(&t);
  else
    p = localtime(&t);
  char buf[64];
  sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d",
          p->tm_year + 1900, p->tm_mon + 1, p->tm_mday,
	  p->tm_hour, p->tm_min, p->tm_sec);
  return new (interp) StringObj(interp.makeStringC(buf));
}

DEFPRIMITIVE(CharProperty, argc, argv, context, interp, loc)
{
  SymbolObj *sym = argv[0]->asSymbol();
  if (!sym)
    return argError(interp, loc,
		    InterpreterMessages::notASymbol, 0, argv[0]);
  Char c;
  if (!argv[1]->charValue(c))
    return argError(interp, loc,
		    InterpreterMessages::notAChar, 1, argv[1]);
  // FIXME
  if (argc > 2)
    return argv[2];
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(Literal, argc, argv, context, interp, loc)
{
  if (argc == 0)
    return new (interp) EmptySosofoObj;
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc, InterpreterMessages::notAString,
		      0, argv[0]);
  if (argc == 1)
    return new (interp) LiteralSosofoObj(argv[0]);
  StringObj *strObj = new (interp) StringObj(s, n);
  for (int i = 1; i < argc; i++) {
    if (!argv[i]->stringData(s, n))
      return argError(interp, loc, InterpreterMessages::notAString,
		      i, argv[i]);
    strObj->append(s, n);
  }
  ELObjDynamicRoot protect(interp, strObj);
  return new (interp) LiteralSosofoObj(strObj);
}

DEFPRIMITIVE(ProcessChildren, argc, argv, context, interp, loc)
{
  if (!context.processingMode) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::noCurrentProcessingMode);
    return interp.makeError();
  }
  return new (interp) ProcessChildrenSosofoObj(context.processingMode);
}

DEFPRIMITIVE(ProcessChildrenTrim, argc, argv, context, interp, loc)
{
  if (!context.processingMode) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::noCurrentProcessingMode);
    return interp.makeError();
  }
  return new (interp) ProcessChildrenTrimSosofoObj(context.processingMode);
}

DEFPRIMITIVE(SosofoAppend, argc, argv, context, interp, loc)
{
  AppendSosofoObj *obj = new (interp) AppendSosofoObj;
  for (int i = 0; i < argc; i++) {
    SosofoObj *sosofo = argv[i]->asSosofo();
    if (!sosofo)
      return argError(interp, loc, InterpreterMessages::notASosofo,
		      i, argv[i]);
    obj->append(sosofo);
  }
  return obj;
}

DEFPRIMITIVE(NextMatch, argc, argv, context, interp, loc)
{
  if (!context.processingMode) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::noCurrentProcessingMode);
    return interp.makeError();
  }
  StyleObj *style;
  if (argc == 0)
    style = 0;
  else {
    style = argv[0]->asStyle();
    if (!style)
      return argError(interp, loc, InterpreterMessages::notAStyle, 0, argv[0]);
  }
  return new (interp) NextMatchSosofoObj(style);
}

DEFPRIMITIVE(EmptySosofo, argc, argv, context, interp, loc)
{
  return new (interp) EmptySosofoObj;
}

DEFPRIMITIVE(SosofoLabel, argc, argv, context, interp, loc)
{
  SosofoObj *sosofo = argv[0]->asSosofo();
  if (!sosofo)
    return argError(interp, loc, InterpreterMessages::notASosofo,
		    0, argv[0]);

  SymbolObj *sym = argv[1]->asSymbol();
  if (!sym)
    return argError(interp, loc,
		    InterpreterMessages::notASymbol, 1, argv[1]);
  return new (interp) LabelSosofoObj(sym, loc, sosofo);
}

DEFPRIMITIVE(SosofoDiscardLabeled, argc, argv, context, interp, loc)
{
  SosofoObj *sosofo = argv[0]->asSosofo();
  if (!sosofo)
    return argError(interp, loc, InterpreterMessages::notASosofo,
		    0, argv[0]);

  SymbolObj *sym = argv[1]->asSymbol();
  if (!sym)
    return argError(interp, loc,
		    InterpreterMessages::notASymbol, 1, argv[1]);
  return new (interp) DiscardLabeledSosofoObj(sym, sosofo);
}

DEFPRIMITIVE(IsSosofo, argc, argv, context, interp, loc)
{
  if (argv[0]->asSosofo())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(MergeStyle, argc, argv, context, interp, loc)
{
  MergeStyleObj *merged = new (interp) MergeStyleObj;
  for (int i = 0; i < argc; i++) {
    StyleObj *style = argv[i]->asStyle();
    if (!style)
      return argError(interp, loc,
		      InterpreterMessages::notAStyle, i, argv[i]);
    merged->append(style);
  }
  return merged;
}

DEFPRIMITIVE(IsStyle, argc, argv, context, interp, loc)
{
  if (argv[0]->asStyle())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(CurrentNodePageNumberSosofo, argc, argv, context, interp, loc)
{
  if (!context.currentNode)
    return noCurrentNodeError(interp, loc);
  return new (interp) CurrentNodePageNumberSosofoObj(context.currentNode);
}

DEFPRIMITIVE(PageNumberSosofo, argc, argv, context, interp, loc)
{
  return new (interp) PageNumberSosofoObj;
}

DEFPRIMITIVE(ProcessElementWithId, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc, InterpreterMessages::notAString, 0, argv[0]);
  if (!context.currentNode)
    return noCurrentNodeError(interp, loc);
  if (!context.processingMode) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::noCurrentProcessingMode);
    return interp.makeError();
  }
  NodePtr root;
  NamedNodeListPtr elements;
  if (context.currentNode->getGroveRoot(root) == accessOK
      && root->getElements(elements) == accessOK) {
    NodePtr node;
    if (elements->namedNode(GroveString(s, n), node) == accessOK)
      return new (interp) ProcessNodeSosofoObj(node, context.processingMode);
  }
  return new (interp) EmptySosofoObj;
}

DEFPRIMITIVE(ProcessFirstDescendant, argc, argv, context, interp, loc)
{
  if (!context.processingMode) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::noCurrentProcessingMode);
    return interp.makeError();
  }
  if (!context.currentNode)
    return noCurrentNodeError(interp, loc);
  
  NCVector<Pattern> patterns(argc);
  for (size_t i = 0; i < argc; i++) {
    if (!interp.convertToPattern(argv[i], loc, patterns[i]))
      return interp.makeError();
  }
  NodeListObj *nl = new (interp) DescendantsNodeListObj(context.currentNode);
  ELObjDynamicRoot protect(interp, nl);
  nl = new (interp) SelectElementsNodeListObj(nl, patterns);
  protect = nl;
  NodePtr nd(nl->nodeListFirst(context, interp));
  if (!nd)
    return new (interp) EmptySosofoObj;
  return new (interp) ProcessNodeSosofoObj(nd, context.processingMode);
}

DEFPRIMITIVE(ProcessMatchingChildren, argc, argv, context, interp, loc)
{
  if (!context.processingMode) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::noCurrentProcessingMode);
    return interp.makeError();
  }
  if (!context.currentNode)
    return noCurrentNodeError(interp, loc);
  NCVector<Pattern> patterns(argc);
  for (size_t i = 0; i < argc; i++) {
    if (!interp.convertToPattern(argv[i], loc, patterns[i])) 
      return interp.makeError();
  }
  NodeListPtr nlPtr;
  // FIXME handle root
  if (patterns.size() == 0 || context.currentNode->children(nlPtr) != accessOK)
    return new (interp) EmptySosofoObj;
  NodeListObj *nl = new (interp) NodeListPtrNodeListObj(nlPtr);
  ELObjDynamicRoot protect(interp, nl);
  nl = new (interp) SelectElementsNodeListObj(nl, patterns);
  protect = nl;
  return new (interp) ProcessNodeListSosofoObj(nl, context.processingMode);
}

DEFPRIMITIVE(SelectElements, argc, argv, context, interp, loc)
{
  NodeListObj *nl = argv[0]->asNodeList();
  if (!nl)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  NCVector<Pattern> patterns(1);
  if (!interp.convertToPattern(argv[1], loc, patterns[0]))
    return interp.makeError();
  return new (interp) SelectElementsNodeListObj(nl, patterns);
}

DEFPRIMITIVE(IsMatchElement, argc, argv, context, interp, loc)
{
  Pattern pattern;
  if (!interp.convertToPattern(argv[0], loc, pattern))
    return interp.makeError();
  NodePtr node;
  if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
    return argError(interp, loc,
		    InterpreterMessages::notASingletonNode, 1, argv[1]);
  if (pattern.matches(node, interp))
    return interp.makeTrue();
  return interp.makeFalse();
}

DEFPRIMITIVE(ProcessNodeList, argc, argv, context, interp, loc)
{
  if (!context.processingMode) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::noCurrentProcessingMode);
    return interp.makeError();
  }
  NodeListObj *nl = argv[0]->asNodeList();
  if (!nl)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  return new (interp) ProcessNodeListSosofoObj(nl, context.processingMode);
}

static
void reverse(StringC &s)
{
  size_t i = 0;
  size_t j = s.size() - 1;
  while (i < j) {
    Char tem = s[i];
    s[i] = s[j];
    s[j] = tem;
    i++;
    j--;
  }
}

static
StringC formatNumberLetter(long n, const char *letters)
{
  StringC result;
  if (n == 0)
    result += '0';
  else {
    bool neg;
    // FIXME possibility of overflow
    if (n < 0) {
      n = -n;
      neg = 1;
    }
    else
      neg = 0;
    do {
      n--;
      int r = n % 26;
      n -= r;
      n /= 26;
      result += letters[r];
    } while (n > 0);
    if (neg)
      result += '-';
    reverse(result);
  }
  return result;
}

static
StringC formatNumberDecimal(long n, size_t minWidth)
{
  StringC result;
  char buf[32];
  sprintf(buf, "%ld", n);
  const char *p = buf;
  if (*p == '-') {
    p++;
    result += '-';
  }
  size_t len = strlen(p);
  while (len < minWidth) {
    result += '0';
    len++;
  }
  while (*p)
    result += *p++;
  return result;
}

static
StringC formatNumberRoman(long n, const char *letters)
{
  StringC result;
  if (n > 5000 || n < -5000 || n == 0)
    return formatNumberDecimal(n, 1);
  if (n < 0) {
    n = -n;
    result += '-';
  }
  while (n >= 1000) {
    result += letters[0];
    n -= 1000;
  }
  for (int i = 100; i > 0; i /= 10, letters += 2) {
    long q = n / i;
    n -= q * i;
    switch (q) {
    case 1:
      result += letters[2];
      break;
    case 2:
      result += letters[2];
      result += letters[2];
      break;
    case 3:
      result += letters[2];
      result += letters[2];
      result += letters[2];
      break;
    case 4:
      result += letters[2];
      result += letters[1];
      break;
    case 5:
      result += letters[1];
      break;
    case 6:
      result += letters[1];
      result += letters[2];
      break;
    case 7:
      result += letters[1];
      result += letters[2];
      result += letters[2];
      break;
    case 8:
      result += letters[1];
      result += letters[2];
      result += letters[2];
      result += letters[2];
      break;
    case 9:
      result += letters[2];
      result += letters[0];
      break;
    }
  }
  return result;
}

static
bool formatNumber(long n, const Char *s, size_t len, StringC &result)
{
  if (len > 0) {
    switch (s[len - 1]) {
    case 'a':
      result += formatNumberLetter(n, "abcdefghijklmnopqrstuvwxyz");
      return 1;
    case 'A':
      result += formatNumberLetter(n, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
      return 1;
    case 'i':
      result += formatNumberRoman(n, "mdclxvi");
      return 1;
    case 'I':
      result += formatNumberRoman(n, "MDCLXVI");
      return 1;
    case '1':
      result += formatNumberDecimal(n, len);
      return 1;
    default:
      break;
    }
  }
  result += formatNumberDecimal(n, 1);
  return 0;
}

DEFPRIMITIVE(FormatNumber, argc, argv, context, interp, loc)
{
  long n;
  if (!argv[0]->exactIntegerValue(n))
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 0, argv[0]);
  const Char *s;
  size_t len;
  if (!argv[1]->stringData(s, len))
    return argError(interp, loc, InterpreterMessages::notAString, 1, argv[1]);
  StringObj *result = new (interp) StringObj;
  if (!formatNumber(n, s, len, *result)) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::invalidNumberFormat,
                   StringMessageArg(StringC(s, len)));
  }
  return result;
}

DEFPRIMITIVE(FormatNumberList, argc, argv, context, interp, loc)
{
  ELObj *numbers = argv[0];
  ELObj *formats = argv[1];
  ELObj *seps = argv[2];
  StringObj *result = new (interp) StringObj;
  while (!numbers->isNil()) {
    PairObj *tem;
    const Char *s;
    size_t len;
    if (numbers != argv[0]) {
      if (!seps->stringData(s, len)) {
	tem = seps->asPair();
	if (!tem)
	  return argError(interp, loc,
  	    	          InterpreterMessages::notAList, 2, argv[2]);
	if (!tem->car()->stringData(s, len))
	  return argError(interp, loc,
  	  	          InterpreterMessages::notAString, 2, tem->car());
	seps = tem->cdr();
      }
      result->append(s, len);
    }
    tem = numbers->asPair();
    if (!tem)
      return argError(interp, loc,
  		      InterpreterMessages::notAList, 0, argv[0]);
    long k;
    if (!tem->car()->exactIntegerValue(k))
      // FIXME message not quite right
      return argError(interp, loc,
  		      InterpreterMessages::notAnExactInteger, 0, tem->car());
    numbers = tem->cdr();
    if (!formats->stringData(s, len)) {
      tem = formats->asPair();
      if (!tem)
	return argError(interp, loc,
  	  	        InterpreterMessages::notAList, 1, argv[1]);
      if (!tem->car()->stringData(s, len))
        return argError(interp, loc,
  		        InterpreterMessages::notAString, 0, tem->car());
      formats = tem->cdr();
    }
    if (!formatNumber(k, s, len, *result)) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::invalidNumberFormat,
		     StringMessageArg(StringC(s, len)));
    }
  }
  return result;
}

DEFPRIMITIVE(ExternalProcedure, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  StringC tem(s, n);
  FunctionObj *func = interp.lookupExternalProc(tem);
  if (func)
    return func;
  return interp.makeFalse();
}

DEFPRIMITIVE(Error, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  interp.setNextLocation(loc);
  interp.message(InterpreterMessages::errorProc,
		 StringMessageArg(StringC(s, n)));
  return interp.makeError();
}

DEFPRIMITIVE(StringToNumber, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  long radix;
  if (argc > 1) {
    if (!argv[1]->exactIntegerValue(radix))
      return argError(interp, loc,
		      InterpreterMessages::notAnExactInteger, 1, argv[1]);
    switch (radix) {
    case 2:
    case 8:
    case 10:
    case 16:
      break;
    default:
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::invalidRadix);
      radix = 10;
      break;
    }
  }
  else
    radix = 10;
  ELObj *result = interp.convertNumber(StringC(s, n), int(radix));
  if (result) {
    result = result->resolveQuantities(0, interp, loc);
    if (interp.isError(result))
      return result;
    long n;
    double d;
    int dim;
    if (result->quantityValue(n, d, dim) != ELObj::noQuantity)
      return result;
  }
  return interp.makeFalse();
}

DEFPRIMITIVE(NumberToString, argc, argv, context, interp, loc)
{
  double x;
  if (!argv[0]->realValue(x))
    return argError(interp, loc,
		    InterpreterMessages::notANumber, 0, argv[0]);
  unsigned radix;
  if (argc > 1) {
    long r;
    if (!argv[1]->exactIntegerValue(r))
      return argError(interp, loc,
		      InterpreterMessages::notAnExactInteger, 1, argv[1]);
    switch (r) {
    case 2:
    case 8:
    case 10:
    case 16:
      radix = unsigned(r);
      break;
    default:
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::invalidRadix);
      radix = 10;
      break;
    }
  }
  else
    radix = 10;
  StrOutputCharStream os;
  argv[0]->print(interp, os, radix);
  StringC tem;
  os.extractString(tem);
  return new (interp) StringObj(tem);
}

DEFPRIMITIVE(QuantityToString, argc, argv, context, interp, loc)
{
  long lResult;
  double dResult;
  int dim;
  if (argv[0]->quantityValue(lResult, dResult, dim) == ELObj::noQuantity)
    return argError(interp, loc,
		    InterpreterMessages::notAQuantity, 0, argv[0]);
  unsigned radix;
  if (argc > 1) {
    long r;
    if (!argv[1]->exactIntegerValue(r))
      return argError(interp, loc,
		      InterpreterMessages::notAnExactInteger, 1, argv[1]);
    switch (r) {
    case 2:
    case 8:
    case 10:
    case 16:
      radix = unsigned(r);
      break;
    default:
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::invalidRadix);
      radix = 10;
      break;
    }
  }
  else
    radix = 10;
  StrOutputCharStream os;
  argv[0]->print(interp, os, radix);
  StringC tem;
  os.extractString(tem);
  return new (interp) StringObj(tem);
}

DEFPRIMITIVE(DisplaySize, argc, argv, context, interp, loc)
{
  return new (interp) LengthSpecObj(LengthSpec(LengthSpec::displaySize, 1.0));
}

DEFPRIMITIVE(TableUnit, argc, argv, context, interp, loc)
{
  long k;
  if (!argv[0]->exactIntegerValue(k))
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 0, argv[0]);

  return new (interp) LengthSpecObj(LengthSpec(LengthSpec::tableUnit, double(k)));
}

DEFPRIMITIVE(IsDisplaySpace, argc, argv, context, interp, loc)
{
  if (argv[0]->asDisplaySpace())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}


DEFPRIMITIVE(DisplaySpace, argc, argv, context, interp, loc)
{
  FOTBuilder::DisplaySpace displaySpace;
  if (!interp.convertLengthSpec(argv[0], displaySpace.nominal))
    return argError(interp, loc,
		    InterpreterMessages::notALengthSpec, 0, argv[0]);
  displaySpace.min = displaySpace.nominal;
  displaySpace.max = displaySpace.nominal;
  // first specified keyword argument takes priority,
  // so scan them backwards...
  for (int i = argc - 1; i > 0; i -= 2) {
    if ((argc & 1) == 0) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::oddKeyArgs);
      return interp.makeError();
    }
    KeywordObj *keyObj = argv[i - 1]->asKeyword();
    if (!keyObj) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::keyArgsNotKey);
      return interp.makeError();
    }
    Identifier::SyntacticKey key;
    if (!keyObj->identifier()->syntacticKey(key)) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::invalidKeyArg,
		     StringMessageArg(keyObj->identifier()->name()));
      return interp.makeError();
    }
    else {
      switch (key) {
      case Identifier::keyMin:
	if (!interp.convertLengthSpec(argv[i], displaySpace.min))
	  return argError(interp, loc,
			  InterpreterMessages::notALengthSpec, i, argv[i]);
	break;
      case Identifier::keyMax:
	if (!interp.convertLengthSpec(argv[i], displaySpace.max))
	  return argError(interp, loc,
			  InterpreterMessages::notALengthSpec, i, argv[i]);
	break;
      case Identifier::keyIsConditional:
	if (argv[i] == interp.makeTrue())
	  displaySpace.conditional = 1;
	else if (argv[i] == interp.makeFalse())
	  displaySpace.conditional = 0;
	else
	  return argError(interp, loc,
		  	  InterpreterMessages::notABoolean, i, argv[i]);
	break;
      case Identifier::keyPriority:
	if (argv[i]->exactIntegerValue(displaySpace.priority))
	  displaySpace.force = 0;
	else {
	  SymbolObj *sym = argv[i]->asSymbol();
	  if (sym && sym->cValue() == FOTBuilder::symbolForce)
	    displaySpace.force = 1;
	  else
	    return argError(interp, loc,
			    InterpreterMessages::notAPriority, i, argv[i]);
	}
	break;
      default:
	interp.setNextLocation(loc);
	interp.message(InterpreterMessages::invalidKeyArg,
	               StringMessageArg(keyObj->identifier()->name()));
	return interp.makeError();
      }
    }
  }
  return new (interp) DisplaySpaceObj(displaySpace);
}

DEFPRIMITIVE(IsInlineSpace, argc, argv, context, interp, loc)
{
  if (argv[0]->asInlineSpace())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(InlineSpace, argc, argv, context, interp, loc)
{
  FOTBuilder::InlineSpace inlineSpace;
  if (!interp.convertLengthSpec(argv[0], inlineSpace.nominal))
    return argError(interp, loc,
		    InterpreterMessages::notALengthSpec, 0, argv[0]);
  inlineSpace.min = inlineSpace.nominal;
  inlineSpace.max = inlineSpace.nominal;
  // first specified keyword argument takes priority,
  // so scan them backwards...
  for (int i = argc - 1; i > 0; i -= 2) {
    if ((argc & 1) == 0) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::oddKeyArgs);
      return interp.makeError();
    }
    KeywordObj *keyObj = argv[i - 1]->asKeyword();
    if (!keyObj) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::keyArgsNotKey);
      return interp.makeError();
    }
    Identifier::SyntacticKey key;
    if (!keyObj->identifier()->syntacticKey(key)) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::invalidKeyArg,
		     StringMessageArg(keyObj->identifier()->name()));
      return interp.makeError();
    }
    else {
      switch (key) {
      case Identifier::keyMin:
	if (!interp.convertLengthSpec(argv[i], inlineSpace.min))
	  return argError(interp, loc,
			  InterpreterMessages::notALengthSpec, i, argv[i]);
	break;
      case Identifier::keyMax:
	if (!interp.convertLengthSpec(argv[i], inlineSpace.max))
	  return argError(interp, loc,
			  InterpreterMessages::notALengthSpec, i, argv[i]);
	break;
      default:
	interp.setNextLocation(loc);
	interp.message(InterpreterMessages::invalidKeyArg,
	               StringMessageArg(keyObj->identifier()->name()));
	return interp.makeError();
      }
    }
  }
  return new (interp) InlineSpaceObj(inlineSpace);
  return argv[0];
}

DEFPRIMITIVE(IsColor, argc, argv, context, interp, loc)
{
  if (argv[0]->asColor())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsColorSpace, argc, argv, context, interp, loc)
{
  if (argv[0]->asColorSpace())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(ColorSpace, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
     return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  if (StringC(s, n)
      != interp.makeStringC("ISO/IEC 10179:1996//Color-Space Family::Device RGB")) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::unknownColorSpaceFamily,
                   StringMessageArg(StringC(s, n)));
    return interp.makeError();
  }
  if (argc > 1) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::deviceRGBColorSpaceNoArgs);
  }
  return new (interp) DeviceRGBColorSpaceObj;
}

DEFPRIMITIVE(Color, argc, argv, context, interp, loc)
{
  ColorSpaceObj *colorSpace = argv[0]->asColorSpace();
  if (!colorSpace)
    return argError(interp, loc,
  		    InterpreterMessages::notAColorSpace, 0, argv[0]);
  return colorSpace->makeColor(argc - 1, argv + 1, interp, loc);
 }

DEFPRIMITIVE(IsAddress, argc, argv, context, interp, loc)
{
  if (argv[0]->asAddress())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsAddressLocal, argc, argv, context, interp, loc)
{
  AddressObj *address = argv[0]->asAddress();
  if (!address)
    return argError(interp, loc,
  		    InterpreterMessages::notAnAddress, 0, argv[0]);
  if (!context.currentNode)
    return noCurrentNodeError(interp, loc);
  switch (address->address().type) {
  case FOTBuilder::Address::resolvedNode:
    if (address->address().node->sameGrove(*context.currentNode))
      return interp.makeTrue();
    else
      return interp.makeFalse();
  case FOTBuilder::Address::idref:
    return interp.makeTrue();
  case FOTBuilder::Address::entity:
    return interp.makeFalse();
  default:
    break;
  }
  return interp.makeFalse();
}

DEFPRIMITIVE(IsAddressVisited, argc, argv, context, interp, loc)
{
  AddressObj *address = argv[0]->asAddress();
  if (!address)
    return argError(interp, loc,
  		    InterpreterMessages::notAnAddress, 0, argv[0]);
  // FIXME
  return interp.makeFalse();
}

DEFPRIMITIVE(CurrentNodeAddress, argc, argv, context, interp, loc)
{
  if (!context.currentNode)
    return noCurrentNodeError(interp, loc);
  return new (interp) AddressObj(FOTBuilder::Address::resolvedNode, context.currentNode);
}

DEFPRIMITIVE(HytimeLinkend, argc, argv, context, interp, loc)
{
  if (!context.currentNode)
    return noCurrentNodeError(interp, loc);
  return new (interp) AddressObj(FOTBuilder::Address::hytimeLinkend, context.currentNode);
}

DEFPRIMITIVE(SgmlDocumentAddress, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  StringC sysid(s, n);
  if (!argv[1]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 1, argv[1]);
  return new (interp) AddressObj(FOTBuilder::Address::sgmlDocument, NodePtr(), sysid, StringC(s, n));
}

DEFPRIMITIVE(IdrefAddress, argc, argv, context, interp, loc)
{
  // The advantage of doing this rather than using an NodeAddressObj,
  // is that when it's a forward reference we don't have to
  // wait for the node.  It might be cleaner to use a ProxyNode class
  // for this.
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  if (!context.currentNode)
    return noCurrentNodeError(interp, loc);
  return new (interp) AddressObj(FOTBuilder::Address::idref, context.currentNode, StringC(s, n));
}

DEFPRIMITIVE(EntityAddress, argc, argv, context, interp, loc)
{
  // Note that multiple space separated entity names are allowed;
  // currently Address doesn't support multiple nodes, so we can't resolve here.
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  if (!context.currentNode)
    return noCurrentNodeError(interp, loc);
  return new (interp) AddressObj(FOTBuilder::Address::entity, context.currentNode, StringC(s, n));
}

DEFPRIMITIVE(NodeListAddress, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (!argv[0]->optSingletonNodeList(context, interp, node) || !node)
    return argError(interp, loc,
		    InterpreterMessages::notASingletonNode, 0, argv[0]);
  return new (interp) AddressObj(FOTBuilder::Address::resolvedNode, node);
}

DEFPRIMITIVE(CharScriptCase, argc, argv, context, interp, loc)
{
  if (!context.styleStack) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::notInCharacteristicValue);
    return interp.makeError();
  }
  for (size_t i = 0; i < argc; i += 2) {
    const Char *s;
    size_t n;
    if (!argv[i]->stringData(s, n))
      return argError(interp, loc,
  		      InterpreterMessages::notAString, i, argv[i]);
  }
  // FIXME
  return argv[argc - 1];
}

DEFPRIMITIVE(IsGlyphId, argc, argv, context, interp, loc)
{
  if (argv[0]->glyphId())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(GlyphId, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  return interp.convertGlyphId(s, n, loc);
}

DEFPRIMITIVE(IsGlyphSubstTable, argc, argv, context, interp, loc)
{
  if (argv[0]->asGlyphSubstTable())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(GlyphSubstTable, argc, argv, context, interp, loc)
{
  ELObj *p = argv[0];
  Ptr<FOTBuilder::GlyphSubstTable> table = new FOTBuilder::GlyphSubstTable;
  table->uniqueId = interp.allocGlyphSubstTableUniqueId();
  while (!p->isNil()) {
    PairObj *tem = p->asPair();
    if (!tem)
      return argError(interp, loc,
		      InterpreterMessages::notAGlyphIdPairList, 0, argv[0]);
    p = tem->cdr();
    tem = tem->car()->asPair();
    const FOTBuilder::GlyphId *g1, *g2;      
    if (!tem
        || (g1 = tem->car()->glyphId()) == 0
	|| (g2 = tem->cdr()->glyphId()) == 0)
      return argError(interp, loc,
		      InterpreterMessages::notAGlyphIdPairList, 0, argv[0]);
    table->pairs.push_back(*g1);
    table->pairs.push_back(*g2);
  }
  return new (interp) GlyphSubstTableObj(table);
}

DEFPRIMITIVE(GlyphSubst, argc, argv, context, interp, loc)
{
  GlyphSubstTableObj *table = argv[0]->asGlyphSubstTable();
  if (!table)
    return argError(interp, loc,
		    InterpreterMessages::notAGlyphSubstTable, 0, argv[0]);
  const FOTBuilder::GlyphId *glyphId = argv[1]->glyphId();
  if (!glyphId)
    return argError(interp, loc,
		    InterpreterMessages::notAGlyphId, 1, argv[1]);
  return new (interp) GlyphIdObj(table->glyphSubstTable()->subst(*glyphId));
}

// Core query language

DEFPRIMITIVE(CurrentNode, argc, argv, context, interp, loc)
{
  if (!context.currentNode)
    return noCurrentNodeError(interp, loc);
  return new (interp) NodePtrNodeListObj(context.currentNode);
}

DEFPRIMITIVE(NodeListError, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  if (!argv[1]->asNodeList())
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 1, argv[1]);
  NodePtr nd;
  const LocNode *lnp;
  Location nodeLoc;
  if (argv[1]->optSingletonNodeList(context, interp, nd)
      && (lnp = LocNode::convert(nd)) != 0
      && lnp->getLocation(nodeLoc) == accessOK)
    interp.setNextLocation(nodeLoc);
  else
    interp.setNextLocation(loc);
  interp.message(InterpreterMessages::errorProc,
		 StringMessageArg(StringC(s, n)));
  return interp.makeError();
}

DEFPRIMITIVE(IsNodeListEmpty, argc, argv, context, interp, loc)
{
  NodeListObj *nl = argv[0]->asNodeList();
  if (!nl)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  if (nl->nodeListFirst(context, interp))
    return interp.makeFalse();
  else
    return interp.makeTrue();
}

DEFPRIMITIVE(IsNodeList, argc, argv, context, interp, loc)
{
  if (argv[0]->asNodeList())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(Parent, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 0) {
    if (!argv[0]->optSingletonNodeList(context, interp, node))
      return argError(interp, loc,
		      InterpreterMessages::notAnOptSingletonNode, 0, argv[0]);
    if (!node)
      return argv[0];
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  if (node->getParent(node) != accessOK)
    return interp.makeEmptyNodeList();
  return new (interp) NodePtrNodeListObj(node);
}

static
bool convertGeneralName(ELObj *obj, const NodePtr &node, StringC &result)
{
  const Char *s;
  size_t n;
  if (!obj->stringData(s, n))
    return 0;
  result.assign(s, n);
  NodePtr root;
  node->getGroveRoot(root);
  NamedNodeListPtr elements;
  root->getElements(elements);
  result.resize(elements->normalize(result.begin(), result.size()));
  return 1;
}

DEFPRIMITIVE(Ancestor, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  StringC gi;
  if (!convertGeneralName(argv[0], node, gi))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  while (node->getParent(node) == accessOK) {
    GroveString str;
    if (node->getGi(str) == accessOK && str == GroveString(gi.data(), gi.size()))
      return new (interp) NodePtrNodeListObj(node);
  }
  return interp.makeEmptyNodeList();
}

DEFPRIMITIVE(Gi, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 0) {
    if (!argv[0]->optSingletonNodeList(context, interp, node))
      return argError(interp, loc,
		      InterpreterMessages::notAnOptSingletonNode, 0, argv[0]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  GroveString str;
  if (node && node->getGi(str) == accessOK)
    return new (interp) StringObj(str.data(), str.size());
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(FirstChildGi, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 0) {
    if (!argv[0]->optSingletonNodeList(context, interp, node))
      return argError(interp, loc,
		      InterpreterMessages::notAnOptSingletonNode, 0, argv[0]);
    if (!node)
      return interp.makeFalse();
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  if (node.assignFirstChild() != accessOK)
    return interp.makeFalse();
  for (;;) {
    GroveString str;
    if (node->getGi(str) == accessOK)
      return new (interp) StringObj(str.data(), str.size());
    if (node.assignNextChunkSibling() != accessOK)
      break;
  }
  return interp.makeFalse();
}

DEFPRIMITIVE(Id, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 0) {
    if (!argv[0]->optSingletonNodeList(context, interp, node))
      return argError(interp, loc,
		      InterpreterMessages::notAnOptSingletonNode, 0, argv[0]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  GroveString str;
  if (node && node->getId(str) == accessOK)
    return new (interp) StringObj(str.data(), str.size());
  else
    return interp.makeFalse();
}

static
bool nodeAttributeString(const NodePtr &node, const Char *s, size_t n,
			 const SdataMapper &mapper, StringC &value)
{
  NamedNodeListPtr atts;
  if (node->getAttributes(atts) != accessOK)
    return 0;
  NodePtr att;
  if (atts->namedNode(GroveString(s, n), att) != accessOK)
    return 0;
  bool implied;
  if (att->getImplied(implied) == accessOK && implied)
    return 0;
  GroveString tokens;
  if (att->tokens(tokens) == accessOK) {
    value.assign(tokens.data(), tokens.size());
    return 1;
  }
  NodePtr tem;
  value.resize(0);
  if (att->firstChild(tem) == accessOK) {
    do {
      GroveString chunk;
      if (tem->charChunk(mapper, chunk) == accessOK)
	value.append(chunk.data(), chunk.size());
    } while (tem.assignNextChunkSibling() == accessOK);
  }
  return 1;
}

DEFPRIMITIVE(AttributeString, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node))
      return argError(interp, loc,
		      InterpreterMessages::notAnOptSingletonNode, 1, argv[1]);
    if (!node)
      return interp.makeFalse();
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc, InterpreterMessages::notAString, 0, argv[0]);
  StringC value;
  if (nodeAttributeString(node, s, n, interp, value))
    return new (interp) StringObj(value);
  return interp.makeFalse();
}

DEFPRIMITIVE(InheritedAttributeString, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node))
      return argError(interp, loc,
		      InterpreterMessages::notAnOptSingletonNode, 1, argv[1]);
    if (!node)
      return interp.makeFalse();
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc, InterpreterMessages::notAString, 0, argv[0]);
  do {
    StringC value;
    if (nodeAttributeString(node, s, n, interp, value))
      return new (interp) StringObj(value);
  } while (node->getParent(node) == accessOK);
  return interp.makeFalse();
}

DEFPRIMITIVE(InheritedElementAttributeString, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 2) {
    if (!argv[2]->optSingletonNodeList(context, interp, node))
      return argError(interp, loc,
		      InterpreterMessages::notAnOptSingletonNode, 2, argv[2]);
    if (!node)
      return interp.makeFalse();
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  StringC gi;
  if (!convertGeneralName(argv[0], node, gi))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  const Char *s;
  size_t n;
  if (!argv[1]->stringData(s, n))
    return argError(interp, loc, InterpreterMessages::notAString, 1, argv[1]);
  do {
    GroveString nodeGi;
    StringC value;
    if (node->getGi(nodeGi) == accessOK
        && nodeGi == GroveString(gi.data(), gi.size())
	&& nodeAttributeString(node, s, n, interp, value))
      return new (interp) StringObj(value);
  } while (node->getParent(node) == accessOK);
  return interp.makeFalse();
}

DEFPRIMITIVE(IsFirstSibling, argc, argv, context, interp, loc)
{
  NodePtr nd;
  if (argc > 0) {
    if (!argv[0]->optSingletonNodeList(context, interp, nd) || !nd)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 0, argv[0]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    nd = context.currentNode;
  }
  GroveString gi;
  NodePtr p;
  if (nd->firstSibling(p) != accessOK
      || nd->getGi(gi) != accessOK)
    return interp.makeFalse();
  while (*p != *nd) {
    GroveString tem;
    if (p->getGi(tem) == accessOK && tem == gi)
      return interp.makeFalse();
    if (p.assignNextChunkSibling() != accessOK)
      CANNOT_HAPPEN();
  }
  return interp.makeTrue();
}

DEFPRIMITIVE(IsAbsoluteFirstSibling, argc, argv, context, interp, loc)
{
  NodePtr nd;
  if (argc > 0) {
    if (!argv[0]->optSingletonNodeList(context, interp, nd) || !nd)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 0, argv[0]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    nd = context.currentNode;
  }
  NodePtr p;
  if (nd->firstSibling(p) != accessOK)
    return interp.makeFalse();
  while (*p != *nd) {
    GroveString tem;
    if (p->getGi(tem) == accessOK)
      return interp.makeFalse();
    if (p.assignNextChunkSibling() != accessOK)
      CANNOT_HAPPEN();
  }
  return interp.makeTrue();
}

DEFPRIMITIVE(IsLastSibling, argc, argv, context, interp, loc)
{
  NodePtr nd;
  if (argc > 0) {
    if (!argv[0]->optSingletonNodeList(context, interp, nd) || !nd)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 0, argv[0]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    nd = context.currentNode;
  }
  GroveString gi;
  if (nd->getGi(gi) != accessOK)
    return interp.makeFalse();
  while (nd.assignNextChunkSibling() == accessOK) {
    GroveString tem;
    if (nd->getGi(tem) == accessOK && tem == gi)
      return interp.makeFalse();
  }
  return interp.makeTrue();
}

DEFPRIMITIVE(IsAbsoluteLastSibling, argc, argv, context, interp, loc)
{
  NodePtr nd;
  if (argc > 0) {
    if (!argv[0]->optSingletonNodeList(context, interp, nd) || !nd)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 0, argv[0]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    nd = context.currentNode;
  }  while (nd.assignNextChunkSibling() == accessOK) {
    GroveString tem;
    if (nd->getGi(tem) == accessOK)
      return interp.makeFalse();
  }
  return interp.makeTrue();
}

// Return 0 on error.

bool matchAncestors(ELObj *obj, const NodePtr &node, ELObj *&unmatched)
{
  NodePtr parent;
  if (node->getParent(parent) != accessOK) {
    unmatched = obj;
    return 1;
  }
  if (!matchAncestors(obj, parent, unmatched))
    return 0;
  if (!unmatched->isNil()) {
    PairObj *pair = unmatched->asPair();
    if (!pair)
      return 0;
    StringC gi;
    if (!convertGeneralName(pair->car(), node, gi))
      return 0;
    GroveString tem;
    if (parent->getGi(tem) == accessOK
	&& tem == GroveString(gi.data(), gi.size()))
      unmatched = pair->cdr();
  }
  return 1;
}

DEFPRIMITIVE(IsHaveAncestor, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  StringC gi;
  if (convertGeneralName(argv[0], node, gi)) {
    while (node->getParent(node) == accessOK) {
      GroveString tem;
      if (node->getGi(tem) == accessOK && tem == GroveString(gi.data(), gi.size()))
	return interp.makeTrue();
    }
    return interp.makeFalse();
  }
  ELObj *unmatched;
  if (!matchAncestors(argv[0], node, unmatched))
    return argError(interp, loc,
		    InterpreterMessages::notAList, 0, argv[0]);
  else if (unmatched->isNil())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(ChildNumber, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 0) {
    if (!argv[0]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 0, argv[0]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  unsigned long num;
  if (!interp.childNumber(node, num))
    return interp.makeFalse();
  return interp.makeInteger(num + 1);
}

DEFPRIMITIVE(AncestorChildNumber, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  StringC gi;
  if (!convertGeneralName(argv[0], node, gi))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  while (node->getParent(node) == accessOK) {
    GroveString str;
    if (node->getGi(str) == accessOK
        && str == GroveString(gi.data(), gi.size())) {
      unsigned long num;
      interp.childNumber(node, num);
      return interp.makeInteger(num + 1);
    }
  }
  return interp.makeFalse();
}

DEFPRIMITIVE(HierarchicalNumber, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  // Make a reversed copy of the list of GIs.
  ELObjDynamicRoot list(interp, 0);
  ELObj *p = argv[0];
  while (!p->isNil()) {
    PairObj *tem = p->asPair();
    if (!tem)
      return argError(interp, loc,
		      InterpreterMessages::notAList, 0, argv[0]);
    list = new (interp) PairObj(tem->car(), list);
    p = tem->cdr();
  }
  p = list;
  while (p) {
    PairObj *pair = (PairObj *)p;
    StringC gi;
    // FIXME error message not quite right
    if (!convertGeneralName(pair->car(), node, gi))
      return argError(interp, loc,
  		      InterpreterMessages::notAString, 0, pair->car());
    for (;;) {
      // Replace the GI by its number.
      if (node->getParent(node) != accessOK) {
	pair->setCar(interp.makeInteger(0));
	break;
      }
      GroveString str;
      if (node->getGi(str) == accessOK
          && str == GroveString(gi.data(), gi.size())) {
	unsigned long num;
	interp.childNumber(node, num);
	pair->setCar(interp.makeInteger(num + 1));
	break;
      }
    }
    p = pair->cdr();
  }
  // Reverse the list of numbers in place.
  p = list;
  ELObj *result = interp.makeNil();
  while (p) {
    PairObj *tem = (PairObj *)p;
    p = tem->cdr();
    tem->setCdr(result);
    result = tem;
  }
  return result;
}

DEFPRIMITIVE(HierarchicalNumberRecursive, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  StringC gi;
  if (!convertGeneralName(argv[0], node, gi))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  ELObjDynamicRoot result(interp, interp.makeNil());
  while (node->getParent(node) == accessOK) {
    GroveString str;
    if (node->getGi(str) == accessOK
        && str == GroveString(gi.data(), gi.size())) {
      unsigned long num;
      interp.childNumber(node, num);
      PairObj *pair = new (interp) PairObj(0, result);
      result = pair;
      pair->setCar(interp.makeInteger(num + 1));
    }
  }
  return result;
}

DEFPRIMITIVE(ElementNumber, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 0) {
    if (!argv[0]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 0, argv[0]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  GroveString gi;
  if (node->getGi(gi) != accessOK)
    return interp.makeFalse();
  StringC buf(gi.data(), gi.size());
  unsigned long num = interp.elementNumber(node, buf);
  return interp.makeInteger(num);
}

DEFPRIMITIVE(ElementNumberList, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  ELObjDynamicRoot list(interp, 0);
  ELObj *p = argv[0];
  while (!p->isNil()) {
    PairObj *tem = p->asPair();
    if (!tem)
      return argError(interp, loc,
		      InterpreterMessages::notAList, 0, argv[0]);
    list = new (interp) PairObj(tem->car(), list);
    p = tem->cdr();
  }
  ELObjDynamicRoot result(interp, interp.makeNil());
  p = list;
  if (p) {
    PairObj *pair = (PairObj *)p;
    StringC gi;
    if (!convertGeneralName(pair->car(), node, gi))
      return argError(interp, loc,
  		      InterpreterMessages::notAString, 0, pair->car());
    p = pair->cdr();
    while (p) {
      pair = (PairObj *)p;
      StringC numGi(gi);
      if (!convertGeneralName(pair->car(), node, gi))
        return argError(interp, loc,
  			InterpreterMessages::notAString, 0, pair->car());
      unsigned long num = interp.elementNumberAfter(node, numGi, gi);
      PairObj *tem = new (interp) PairObj(0, result);
      result = tem;
      tem->setCar(interp.makeInteger(num));
      p = pair->cdr();
    }
    unsigned long num = interp.elementNumber(node, gi);
    PairObj *tem = new (interp) PairObj(0, result);
    result = tem;
    tem->setCar(interp.makeInteger(num));
  }
  return result;
}

DEFPRIMITIVE(EntityAttributeString, argc, argv, context, interp, loc)
{
  const Char *entityName;
  size_t entityNameLen;
  if (!argv[0]->stringData(entityName, entityNameLen))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  const Char *attName;
  size_t attNameLen;
  if (!argv[1]->stringData(attName, attNameLen))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 1, argv[1]);
  NodePtr node;
  if (argc > 2) {
    if (!argv[2]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 2, argv[2]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  NamedNodeListPtr entities;
  StringC value;
  if (node->getGroveRoot(node) == accessOK
      && node->getEntities(entities) == accessOK
      && entities->namedNode(GroveString(entityName, entityNameLen), node) == accessOK
      && nodeAttributeString(node, attName, attNameLen, interp, value))
    return new (interp) StringObj(value);
  return interp.makeFalse();
}

DEFPRIMITIVE(EntityGeneratedSystemId, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  GroveString str;
  NamedNodeListPtr entities;
  if (node->getGroveRoot(node) == accessOK
      && node->getEntities(entities) == accessOK
      && entities->namedNode(GroveString(s, n), node) == accessOK
      && node->getExternalId(node) == accessOK
      && node->getGeneratedSystemId(str) == accessOK)
    return new (interp) StringObj(str.data(), str.size());
  return interp.makeFalse();
}
 
DEFPRIMITIVE(EntitySystemId, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  GroveString str;
  NamedNodeListPtr entities;
  if (node->getGroveRoot(node) == accessOK
      && node->getEntities(entities) == accessOK
      && entities->namedNode(GroveString(s, n), node) == accessOK
      && node->getExternalId(node) == accessOK
      && node->getSystemId(str) == accessOK)
    return new (interp) StringObj(str.data(), str.size());
  return interp.makeFalse();
}

DEFPRIMITIVE(EntityPublicId, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  GroveString str;
  NamedNodeListPtr entities;
  if (node->getGroveRoot(node) == accessOK
      && node->getEntities(entities) == accessOK) {
    StringC tem(s, n);
    tem.resize(entities->normalize(tem.begin(), tem.size()));
    if (entities->namedNode(GroveString(tem.data(), tem.size()), node) == accessOK
	&& node->getExternalId(node) == accessOK
	&& node->getPublicId(str) == accessOK)
      return new (interp) StringObj(str.data(), str.size());
  }
  return interp.makeFalse();
}

DEFPRIMITIVE(EntityNotation, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  GroveString str;
  NamedNodeListPtr entities;
  if (node->getGroveRoot(node) == accessOK
      && node->getEntities(entities) == accessOK
      && entities->namedNode(GroveString(s, n), node) == accessOK
      && node->getNotation(node) == accessOK
      && node->getName(str) == accessOK)
    return new (interp) StringObj(str.data(), str.size());
  return interp.makeFalse();
}

DEFPRIMITIVE(EntityText, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  GroveString str;
  NamedNodeListPtr entities;
  if (node->getGroveRoot(node) == accessOK
      && node->getEntities(entities) == accessOK
      && entities->namedNode(GroveString(s, n), node) == accessOK
      && node->getText(str) == accessOK)
    return new (interp) StringObj(str.data(), str.size());
  return interp.makeFalse();
}

DEFPRIMITIVE(EntityType, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  NamedNodeListPtr entities;
  Node::EntityType type;
  if (node->getGroveRoot(node) == accessOK
      && node->getEntities(entities) == accessOK
      && entities->namedNode(GroveString(s, n), node) == accessOK
      && node->getEntityType(type) == accessOK) {
    const char *s;
    switch (type) {
    case Node::text:
      s = "text";
      break;
    case Node::cdata:
      s = "cdata";
      break;
    case Node::sdata:
      s = "sdata";
      break;
    case Node::ndata:
      s = "ndata";
      break;
    case Node::subdocument:
      s = "subdocument";
      break;
    case Node::pi:
      s = "pi";
      break;
    default:
      CANNOT_HAPPEN();
    }
    return interp.makeSymbol(interp.makeStringC(s));
  }
  return interp.makeFalse();
}

DEFPRIMITIVE(NotationSystemId, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  GroveString str;
  NamedNodeListPtr notations;
  if (node->getGroveRoot(node) == accessOK
      && node->getGoverningDoctype(node) == accessOK
      && node->getNotations(notations) == accessOK
      && notations->namedNode(GroveString(s, n), node) == accessOK
      && node->getExternalId(node) == accessOK
      && node->getSystemId(str) == accessOK)
    return new (interp) StringObj(str.data(), str.size());
  return interp.makeFalse();
}

DEFPRIMITIVE(NotationPublicId, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  GroveString str;
  NamedNodeListPtr notations;
  if (node->getGroveRoot(node) == accessOK
      && node->getGoverningDoctype(node) == accessOK
      && node->getNotations(notations) == accessOK
      && notations->namedNode(GroveString(s, n), node) == accessOK
      && node->getExternalId(node) == accessOK
      && node->getPublicId(str) == accessOK)
    return new (interp) StringObj(str.data(), str.size());
  return interp.makeFalse();
}

DEFPRIMITIVE(NotationGeneratedSystemId, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  GroveString str;
  NamedNodeListPtr notations;
  if (node->getGroveRoot(node) == accessOK
      && node->getGoverningDoctype(node) == accessOK
      && node->getNotations(notations) == accessOK
      && notations->namedNode(GroveString(s, n), node) == accessOK
      && node->getExternalId(node) == accessOK
      && node->getGeneratedSystemId(str) == accessOK)
    return new (interp) StringObj(str.data(), str.size());
  return interp.makeFalse();
}

DEFPRIMITIVE(GeneralNameNormalize, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
    		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  StringC result;
  if (!convertGeneralName(argv[0], node, result))
    return argError(interp, loc, InterpreterMessages::notAString, 0, argv[0]);
  return new (interp) StringObj(result);
}

DEFPRIMITIVE(EntityNameNormalize, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc, InterpreterMessages::notAString, 0, argv[0]);
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
    		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  StringC result(s, n);

  node->getGroveRoot(node);
  NamedNodeListPtr entities;
  node->getEntities(entities);
  result.resize(entities->normalize(result.begin(), result.size()));
  return new (interp) StringObj(result);
}

DEFPRIMITIVE(NodeListFirst, argc, argv, context, interp, loc)
{
  NodeListObj *nl = argv[0]->asNodeList();
  if (!nl)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  NodePtr nd = nl->nodeListFirst(context, interp);
  return new (interp) NodePtrNodeListObj(nd);
}

DEFPRIMITIVE(NodeListRest, argc, argv, context, interp, loc)
{
  NodeListObj *nl = argv[0]->asNodeList();
  if (!nl)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  return nl->nodeListRest(context, interp);
}

DEFPRIMITIVE(NodeList, argc, argv, context, interp, loc)
{
  if (argc == 0)
    return interp.makeEmptyNodeList();
  int i = argc - 1;
  NodeListObj *nl = argv[i]->asNodeList();
  if (!nl)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, i, argv[i]);
  if (i > 0) {
    ELObjDynamicRoot protect(interp, nl);
    for (;;) {
      i--;
      NodeListObj *tem = argv[i]->asNodeList();
      if (!tem)
        return argError(interp, loc,
	                InterpreterMessages::notANodeList, i, argv[i]);
      nl = new (interp) PairNodeListObj(tem, nl);
      if (i == 0)
	break;
      protect = nl;
    }
  }
  return nl;
}

DEFPRIMITIVE(NodeListNoOrder, argc, argv, context, interp, loc)
{
  NodeListObj *nl = argv[0]->asNodeList();
  if (!nl)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  return nl->nodeListNoOrder(interp);
}

DEFPRIMITIVE(IsNodeListEqual, argc, argv, context, interp, loc)
{
  NodeListObj *nl1 = argv[0]->asNodeList();
  if (!nl1)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  if (nl1 == argv[1])
    return interp.makeTrue();
  NodeListObj *nl2 = argv[1]->asNodeList();
  if (!nl2)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 1, argv[1]);
  ELObjDynamicRoot protect1(interp, nl1);
  ELObjDynamicRoot protect2(interp, nl2);
  for (;;) {
    NodePtr nd1 = nl1->nodeListFirst(context, interp);
    NodePtr nd2 = nl2->nodeListFirst(context, interp);
    if (!nd1) {
      if (nd2)
	return interp.makeFalse();
      else
	break;
    }
    else if (!nd2)
      return interp.makeFalse();
    else if (*nd1 != *nd2)
      return interp.makeFalse();
    nl1 = nl1->nodeListRest(context, interp);
    protect1 = nl1;
    nl2 = nl2->nodeListRest(context, interp);
    protect2 = nl2;
  }
  return interp.makeTrue();
}

DEFPRIMITIVE(IsNamedNodeList, argc, argv, context, interp, loc)
{
  if (argv[0]->asNamedNodeList())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(NamedNode, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  NamedNodeListObj *nnl = argv[1]->asNamedNodeList();
  if (!nnl)
    return argError(interp, loc,
		    InterpreterMessages::notANamedNodeList, 1, argv[1]);
  return new (interp) NodePtrNodeListObj(nnl->namedNode(s, n));
}

DEFPRIMITIVE(NamedNodeListNormalize, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc, InterpreterMessages::notAString, 0, argv[0]);
  NamedNodeListObj *nnl = argv[1]->asNamedNodeList();
  if (!nnl)
    return argError(interp, loc,
		    InterpreterMessages::notANamedNodeList, 1, argv[1]);
  if (!argv[2]->asSymbol())
    return argError(interp, loc,
		    InterpreterMessages::notASymbol, 2, argv[2]);
  StringC result(s, n);
  result.resize(nnl->normalize(result.begin(), result.size()));
  return new (interp) StringObj(result);
}

DEFPRIMITIVE(NamedNodeListNames, argc, argv, context, interp, loc)
{
  NamedNodeListObj *nnl = argv[0]->asNamedNodeList();
  if (!nnl)
    return argError(interp, loc,
		    InterpreterMessages::notANamedNodeList, 0, argv[0]);
  NodeListObj *nl = nnl;
  PairObj *tail = interp.makePair(0, 0);
  PairObj *head = tail;
  ELObjDynamicRoot protect(interp, head);
  for (;;) {
    ELObjDynamicRoot protect(interp, nl);
    NodePtr nd = nl->nodeListFirst(context, interp);
    if (!nd)
      break;
    GroveString str;
    if (nnl->nodeName(nd, str)) {
      // protect the StringObj by putting in the head's car
      head->setCar(new (interp) StringObj(str.data(), str.size()));
      PairObj *newTail = new (interp) PairObj(head->car(), 0);
      tail->setCdr(newTail);
      tail = newTail;
    }
    nl = nl->nodeListRest(context, interp);
  }
  tail->setCdr(interp.makeNil());
  return head->cdr();
}

DEFPRIMITIVE(Children, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (!argv[0]->optSingletonNodeList(context, interp, node)) {
    NodeListObj *nl = argv[0]->asNodeList();
    if (nl)
      return new (interp) MapNodeListObj(this, nl, new MapNodeListObj::Context(context, loc));
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  }
  if (!node)
    return argv[0];
  NodeListPtr nl;
  if (node->children(nl) != accessOK)
    return interp.makeEmptyNodeList();
  return new (interp) NodeListPtrNodeListObj(nl);
}

DEFPRIMITIVE(Follow, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (!argv[0]->optSingletonNodeList(context, interp, node)) {
    NodeListObj *nl = argv[0]->asNodeList();
    if (nl)
      return new (interp) MapNodeListObj(this, nl, new MapNodeListObj::Context(context, loc));
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  }
  if (!node)
    return argv[0];
  NodeListPtr nl;
  if (node->follow(nl) != accessOK)
    return interp.makeEmptyNodeList();
  return new (interp) NodeListPtrNodeListObj(nl);
}

DEFPRIMITIVE(Descendants, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (!argv[0]->optSingletonNodeList(context, interp, node)) {
    NodeListObj *nl = argv[0]->asNodeList();
    if (nl)
      return new (interp) MapNodeListObj(this, nl, new MapNodeListObj::Context(context, loc));
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  }
  return new (interp) DescendantsNodeListObj(node);
}

DEFPRIMITIVE(Preced, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (!argv[0]->optSingletonNodeList(context, interp, node)) {
    NodeListObj *nl = argv[0]->asNodeList();
    if (nl)
      return new (interp) MapNodeListObj(this, nl, new MapNodeListObj::Context(context, loc));
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  }
  NodePtr first;
  if (!node || node->firstSibling(first) != accessOK)
    return interp.makeEmptyNodeList();
  return new (interp) SiblingNodeListObj(first, node);
}

DEFPRIMITIVE(Attributes, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (!argv[0]->optSingletonNodeList(context, interp, node)) {
    NodeListObj *nl = argv[0]->asNodeList();
    if (nl)
      return new (interp) MapNodeListObj(this, nl, new MapNodeListObj::Context(context, loc));
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  }
  if (!node)
    return argv[0];
  NamedNodeListPtr nnl;
  if (node->getAttributes(nnl) != accessOK)
    return interp.makeEmptyNodeList();
  return new (interp) NamedNodeListPtrNodeListObj(nnl);
}

static
void nodeData(const NodePtr &nd, const SdataMapper &mapper, bool chunk, StringC &s)
{
  GroveString tem;
  if (nd->charChunk(mapper, tem) == accessOK) {
    s.append(tem.data(), chunk ? tem.size() : 1);
    return;
  }
  if (nd->tokens(tem) == accessOK) {
    s.append(tem.data(), tem.size());
    return;
  }
  NodePtr cnd;
  if (nd->firstChild(cnd) == accessOK) {
    do {
      nodeData(cnd, mapper, 1, s);
    } while (cnd.assignNextChunkSibling() == accessOK);
    return;
  }
  // This happens if the data procedure is called on an AVT node
  if (nd->getToken(tem) == accessOK)
    s.append(tem.data(), tem.size());
}

DEFPRIMITIVE(Data, argc, argv, context, interp, loc)
{
  NodeListObj *nl = argv[0]->asNodeList();
  if (!nl)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  StringObj *s = new (interp) StringObj;
  ELObjDynamicRoot protect(interp, s);
  for (;;) {
    ELObjDynamicRoot protect(interp, nl);
    NodePtr nd = nl->nodeListFirst(context, interp);
    if (!nd)
      break;
    bool chunk;
    nl = nl->nodeListChunkRest(context, interp, chunk);
    nodeData(nd, interp, chunk, *s);
  }
  return s;
}

DEFPRIMITIVE(ElementWithId, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc, InterpreterMessages::notAString, 0, argv[0]);
  NodePtr node;
  if (argc > 1) {
    if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, 1, argv[1]);
  }
  else {
    node = context.currentNode;
    if (!node)
      return noCurrentNodeError(interp, loc);
  }
  NamedNodeListPtr elements;
  if (node->getGroveRoot(node) == accessOK
      && node->getElements(elements) == accessOK
      && elements->namedNode(GroveString(s, n), node) == accessOK)
    return new (interp) NodePtrNodeListObj(node);
  return interp.makeEmptyNodeList();
}

DEFPRIMITIVE(EmptyNodeList, argc, argv, context, interp, loc)
{
  return interp.makeEmptyNodeList();
}

static
bool decodeKeyArgs(int argc, ELObj **argv, const Identifier::SyntacticKey *keys,
		   int nKeys, Interpreter &interp, const Location &loc, int *pos)
{
  if ((argc & 1) == 1) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::oddKeyArgs);
    return 0;
  }
  for (int i = 0; i < nKeys; i++)
    pos[i] = -1;
  // First has priority, so scan in reverse order
  for (int i = argc - 1; i > 0; i -= 2) {
    KeywordObj *keyObj = argv[i - 1]->asKeyword();
    if (!keyObj) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::keyArgsNotKey);
      return 0;
    }
    bool found = 0;
    Identifier::SyntacticKey key;
    if (keyObj->identifier()->syntacticKey(key)) {
      for (int j = 0; j < nKeys; j++) {
	if (key == keys[j]) {
	  pos[j] = i;
	  found = 1;
	}
      }
    }
    if (!found) {
      interp.setNextLocation(loc);
      interp.message(InterpreterMessages::invalidKeyArg,
		     StringMessageArg(keyObj->identifier()->name()));
      return 0;
    }
  }
  return 1;
}

class ELObjPropertyValue : public PropertyValue {
public:
  ELObjPropertyValue(Interpreter &interp, bool rcs) : interp_(&interp), rcs_(rcs), obj(0) { }
  void set(const NodePtr &nd) {
    obj = new (*interp_) NodePtrNodeListObj(nd);
  }
  void set(const NodeListPtr &nl) {
    obj = new (*interp_) NodeListPtrNodeListObj(nl);
  }
  void set(const NamedNodeListPtr &nnl) {
    obj = new (*interp_) NamedNodeListPtrNodeListObj(nnl);
  }
  void set(bool b) {
    if (b)
      obj = interp_->makeTrue();
    else
      obj = interp_->makeFalse();
  }
  void set(GroveChar c) {
    obj = interp_->makeChar(c);
  }
  void set(GroveString s) {
    obj = new (*interp_) StringObj(s.data(), s.size());
  }
  void set(ComponentName::Id id) {
    const char *s = rcs_ ? ComponentName::rcsName(id) : ComponentName::sdqlName(id);
    obj = interp_->makeSymbol(interp_->makeStringC(s));
  }
  void set(const ComponentName::Id *names) {
    PairObj *head = new (*interp_) PairObj(0, 0);
    ELObjDynamicRoot protect(*interp_, head);
    PairObj *tail = head;
    for (int i = 0; names[i] != ComponentName::noId; i++) {
      const char *s = (rcs_
	               ? ComponentName::rcsName(names[i])
		       : ComponentName::sdqlName(names[i]));
      SymbolObj *sym = interp_->makeSymbol(interp_->makeStringC(s));
      tail->setCdr(sym); // in case we ever gc symbols
      PairObj *tem = new (*interp_) PairObj(sym, 0);
      tail->setCdr(tem);
      tail = tem;
    }
    tail->setCdr(interp_->makeNil());
    obj = head->cdr();
  }
  ELObj *obj;
private:
  Interpreter *interp_;
  bool rcs_;
};

DEFPRIMITIVE(NodeProperty, argc, argv, context, interp, loc)
{
  StringObj *str = argv[0]->convertToString();
  if (!str)
    return argError(interp, loc,
		    InterpreterMessages::notAStringOrSymbol, 0, argv[0]);
  NodePtr node;
  if (!argv[1]->optSingletonNodeList(context, interp, node) || !node)
    return argError(interp, loc,
		    InterpreterMessages::notASingletonNode, 1, argv[1]);
  static const Identifier::SyntacticKey keys[3] = {
    Identifier::keyDefault, Identifier::keyNull, Identifier::keyIsRcs
  };
  int pos[3];
  if (!decodeKeyArgs(argc - 2, argv + 2, keys, 3, interp, loc, pos))
    return interp.makeError();
  ComponentName::Id id;
  if (interp.lookupNodeProperty(*str, id)) {
    ELObjPropertyValue value(interp,
			     pos[2] >= 0
			     && argv[pos[2] + 2] != interp.makeFalse());
    AccessResult ret = node->property(id, interp, value);
    if (ret == accessOK)
      return value.obj;
    if (ret == accessNull && pos[1] >= 0)
      return argv[pos[1] + 2];
  }
  if (pos[0] < 0) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::noNodePropertyValue,
		   StringMessageArg(*str));
    return interp.makeError();
  }
  return argv[pos[0] + 2];
}

DEFPRIMITIVE(SelectByClass, argc, argv, context, interp, loc)
{
  NodeListObj *nl = argv[0]->asNodeList();
  if (!nl)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  StringObj *str = argv[1]->convertToString();
  if (!str)
    return argError(interp, loc,
		    InterpreterMessages::notAStringOrSymbol, 1, argv[1]);
  ComponentName::Id id;
  if (!interp.lookupNodeProperty(*str, id))
    return interp.makeEmptyNodeList();
  return new (interp) SelectByClassNodeListObj(nl, id);
}

DEFPRIMITIVE(NodeListMap, argc, argv, context, interp, loc)
{
  FunctionObj *func = argv[0]->asFunction();
  if (!func)
    return argError(interp, loc,
		    InterpreterMessages::notAProcedure, 0, argv[0]);
  if (func->nRequiredArgs() > 1) {
    interp.setNextLocation(loc);
    // FIXME
    interp.message(InterpreterMessages::missingArg);
    return interp.makeError();
  }
  if (func->nRequiredArgs() + func->nOptionalArgs() + func->restArg() == 0) {
    interp.setNextLocation(loc);
    // FIXME
    interp.message(InterpreterMessages::tooManyArgs);
    return interp.makeError();
  }
  interp.makeReadOnly(func);
  NodeListObj *nl = argv[1]->asNodeList();
  if (!nl)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 1, argv[1]);
  return new (interp) MapNodeListObj(func, nl, new MapNodeListObj::Context(context, loc));
}

DEFPRIMITIVE(NodeListRef, argc, argv, context, interp, loc)
{
  NodeListObj *nl = argv[0]->asNodeList();
  if (!nl)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  long k;
  if (!argv[1]->exactIntegerValue(k))
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 1, argv[1]);
  // Must use temporary variable, because operator new may bew called before nodeListRef.
  NodePtr nd(nl->nodeListRef(k, context, interp));
  return new (interp) NodePtrNodeListObj(nd);
}

DEFPRIMITIVE(NodeListReverse, argc, argv, context, interp, loc)
{
  NodeListObj *nl = argv[0]->asNodeList();
  if (!nl)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  return nl->nodeListReverse(context, interp);
}

DEFPRIMITIVE(NodeListLength, argc, argv, context, interp, loc)
{
  NodeListObj *nl = argv[0]->asNodeList();
  if (!nl)
    return argError(interp, loc,
		    InterpreterMessages::notANodeList, 0, argv[0]);
  return interp.makeInteger(nl->nodeListLength(context, interp));
}

DEFPRIMITIVE(SgmlParse, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  StringC sysid(s, n);
  static const Identifier::SyntacticKey keys[3] = {
      Identifier::keyActive, Identifier::keyArchitecture, Identifier::keyParent
  };
  int pos[3];
  if (!decodeKeyArgs(argc - 1, argv + 1, keys, 3, interp, loc, pos))
    return interp.makeError();
  Vector<StringC> lists[2];
  for (int i = 0; i < 2; i++) {
    if (pos[i] >= 0) {
      ELObj *obj = argv[pos[0] + 1];
      while (!obj->isNil()) {
	PairObj *pair = obj->asPair();
	if (!pair)
   	  return argError(interp, loc,
			  InterpreterMessages::notAList, pos[i] + 1, argv[pos[i] + 1]);
	if (!pair->car()->stringData(s, n))
	  return argError(interp, loc,
  		          InterpreterMessages::notAString, pos[i] + 1, pair->car());
	lists[i].resize(lists[i].size() + 1);
	lists[i].back().assign(s, n);
	obj = pair->cdr();
      }
    }
  }

  NodePtr parent;
  if (pos[2] >= 0) {
    if (!argv[pos[2] + 1]->optSingletonNodeList(context, interp, parent) || !parent)
      return argError(interp, loc,
		      InterpreterMessages::notASingletonNode, pos[2] + 1, argv[pos[2] + 1]);
  }

  NodePtr nd;
  if (!interp.groveManager()->load(sysid, lists[0], parent, nd, lists[1]))
    return interp.makeEmptyNodeList();
  return new (interp) NodePtrNodeListObj(nd);
}

DEFPRIMITIVE(ReadEntity, argc, argv, context, interp, loc)
{
  const Char *s;
  size_t n;
  if (!argv[0]->stringData(s, n))
    return argError(interp, loc,
		    InterpreterMessages::notAString, 0, argv[0]);
  StringC sysid(s, n);
  StringObj *contents = new (interp) StringObj;
  if (interp.groveManager()->readEntity(sysid, *contents))
    return contents;
  return interp.makeError();
}

DEFPRIMITIVE(Debug, argc, argv, context, interp, loc)
{
  interp.setNextLocation(loc);
  interp.message(InterpreterMessages::debug, ELObjMessageArg(argv[0], interp));
  return argv[0];
}

DEFPRIMITIVE(IfFirstPage, argc, argv, context, interp, loc)
{
  SosofoObj *sosofo[2];
  for (int i = 0; i < 2; i++) {
    sosofo[i] = argv[i]->asSosofo();
    if (!sosofo[i])
      return argError(interp, loc, InterpreterMessages::notASosofo,
  		      i, argv[i]);
  }
  return new (interp) PageTypeSosofoObj(FOTBuilder::firstHF, sosofo[0], sosofo[1]);
}

DEFPRIMITIVE(IfFrontPage, argc, argv, context, interp, loc)
{
  SosofoObj *sosofo[2];
  for (int i = 0; i < 2; i++) {
    sosofo[i] = argv[i]->asSosofo();
    if (!sosofo[i])
      return argError(interp, loc, InterpreterMessages::notASosofo,
  		      i, argv[i]);
  }
  return new (interp) PageTypeSosofoObj(FOTBuilder::frontHF, sosofo[0], sosofo[1]);
}

DEFPRIMITIVE(AllElementNumber, argc, argv, context, interp, loc)
{
  NodePtr node;
  if (argc > 0) {
    if (!argv[0]->optSingletonNodeList(context, interp, node))
      return argError(interp, loc,
		      InterpreterMessages::notAnOptSingletonNode, 0, argv[0]);
  }
  else {
    if (!context.currentNode)
      return noCurrentNodeError(interp, loc);
    node = context.currentNode;
  }
  unsigned long n;
  if (node && node->elementIndex(n) == accessOK)
    return interp.makeInteger(long(n) + 1);
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(IsVector, argc, argv, context, interp, loc)
{
  if (argv[0]->asVector())
    return interp.makeTrue();
  else
    return interp.makeFalse();
}

DEFPRIMITIVE(Vector, argc, argv, context, interp, loc)
{
  Vector<ELObj *> v(argc);
  for (size_t i = 0; i < argc; i++)
    v[i] = argv[i];
  return new (interp) VectorObj(v);
}

DEFPRIMITIVE(MakeVector, argc, argv, context, interp, loc)
{
  long k;
  if (!argv[0]->exactIntegerValue(k))
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 0, argv[0]);
  if (k < 0) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::outOfRange);
    return interp.makeError();
  }
  ELObj *fill = argc > 1 ? argv[1] : interp.makeUnspecified();
  Vector<ELObj *> v((size_t)k);
  for (size_t i = 0; i < v.size(); i++)
    v[i] = fill;
  return new (interp) VectorObj(v);
}

DEFPRIMITIVE(VectorSet, argc, argv, context, interp, loc)
{
  VectorObj *v = argv[0]->asVector();
  if (!v)
    return argError(interp, loc,
		    InterpreterMessages::notAVector, 0, argv[0]);
  long k;
  if (!argv[1]->exactIntegerValue(k))
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 1, argv[1]);
  if (k < 0 || (unsigned long)k >= v->size()) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::outOfRange);
    return interp.makeError();
  }
  if (v->readOnly()) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::readOnly);
    return interp.makeError();
  }
  (*v)[k] = argv[2];
  return interp.makeUnspecified();
}

DEFPRIMITIVE(VectorRef, argc, argv, context, interp, loc)
{
  VectorObj *v = argv[0]->asVector();
  if (!v)
    return argError(interp, loc,
		    InterpreterMessages::notAVector, 0, argv[0]);
  long k;
  if (!argv[1]->exactIntegerValue(k))
    return argError(interp, loc,
		    InterpreterMessages::notAnExactInteger, 1, argv[1]);
  if (k < 0 || (unsigned long)k >= v->size()) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::outOfRange);
    return interp.makeError();
  }
  return (*v)[k];
}

DEFPRIMITIVE(VectorToList, argc, argv, context, interp, loc)
{
  VectorObj *v = argv[0]->asVector();
  if (!v)
    return argError(interp, loc,
		    InterpreterMessages::notAVector, 0, argv[0]);
  Vector<ELObj *> &vec = *v;
  ELObjDynamicRoot result(interp, interp.makeNil());
  for (size_t i = vec.size(); i > 0; i--)
    result = new (interp) PairObj(vec[i - 1], result);
  return result;
}

DEFPRIMITIVE(ListToVector, argc, argv, context, interp, loc)
{
  Vector<ELObj *> v;
  ELObj *obj = argv[0];
  while (!obj->isNil()) {
    PairObj *pair = obj->asPair();
    if (!pair)
      return argError(interp, loc, InterpreterMessages::notAList, 0, obj);
    v.push_back(pair->car());
    obj = pair->cdr();
  }
  return new (interp) VectorObj(v);
}

DEFPRIMITIVE(VectorFill, argc, argv, context, interp, loc)
{
  VectorObj *v = argv[0]->asVector();
  if (!v)
    return argError(interp, loc,
		    InterpreterMessages::notAVector, 0, argv[0]);
  if (v->readOnly()) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::readOnly);
    return interp.makeError();
  }
  Vector<ELObj *> &vec = *v;
  for (size_t i = 0; i < vec.size(); i++)
    vec[i] = argv[1];
  return interp.makeUnspecified();
}

void Interpreter::installPrimitives()
{
#define PRIMITIVE(name, string, nRequired, nOptional, rest) \
  installPrimitive(string, new (*this) name ## PrimitiveObj);
#define XPRIMITIVE(name, string, nRequired, nOptional, rest) \
  installXPrimitive(string, new (*this) name ## PrimitiveObj);
#define PRIMITIVE2(name, string, nRequired, nOptional, rest) \
  if (dsssl2()) installPrimitive(string, new (*this) name ## PrimitiveObj);
#include "primitive.h"
#undef PRIMITIVE
#undef XPRIMITIVE
#undef PRIMITIVE2
  FunctionObj *apply = new (*this) ApplyPrimitiveObj;
  makePermanent(apply);
  lookup(makeStringC("apply"))->setValue(apply);
  if (dsssl2()) {
    FunctionObj *callCC = new (*this) CallWithCurrentContinuationPrimitiveObj;
    makePermanent(callCC);
    lookup(makeStringC("call-with-current-continuation"))->setValue(callCC);
  }
  if (dsssl2())
    lookup(makeStringC("string->quantity"))
      ->setValue(lookup(makeStringC("string->number"))->computeValue(0, *this));
}

void Interpreter::installPrimitive(const char *s, PrimitiveObj *value)
{
  makePermanent(value);
  Identifier *ident = lookup(makeStringC(s));
  ident->setValue(value);
  value->setIdentifier(ident);
  StringC pubid(makeStringC("ISO/IEC 10179:1996//Procedure::"));
  pubid += makeStringC(s);
  externalProcTable_.insert(pubid, value);
}

void Interpreter::installXPrimitive(const char *s, PrimitiveObj *value)
{
  makePermanent(value);
  value->setIdentifier(lookup(makeStringC(s)));
  StringC pubid(makeStringC("UNREGISTERED::James Clark//Procedure::"));
  pubid += makeStringC(s);
  externalProcTable_.insert(pubid, value);
}

DescendantsNodeListObj::DescendantsNodeListObj(const NodePtr &start, unsigned depth)
: start_(start), depth_(depth)
{
  advance(start_, depth_);
}

NodePtr DescendantsNodeListObj::nodeListFirst(EvalContext &, Interpreter &)
{
  return start_;
}

NodeListObj *DescendantsNodeListObj::nodeListRest(EvalContext &context, Interpreter &interp)
{
  DescendantsNodeListObj *obj = new (interp) DescendantsNodeListObj(*this);
  advance(obj->start_, obj->depth_);
  return obj;
}

NodeListObj *DescendantsNodeListObj::nodeListChunkRest(EvalContext &context, Interpreter &interp, bool &chunk)
{
  DescendantsNodeListObj *obj = new (interp) DescendantsNodeListObj(*this);
  chunkAdvance(obj->start_, obj->depth_);
  chunk = 1;
  return obj;
}

void DescendantsNodeListObj::advance(NodePtr &nd, unsigned &depth)
{
  if (!nd)
    return;
  if (nd.assignFirstChild() == accessOK) {
    depth++;
    return;
  }
  if (depth == 0) {
    nd.clear();
    return;
  }
  while (nd.assignNextSibling() != accessOK) {
    if (depth == 1 || nd.assignOrigin() != accessOK) {
      nd.clear();
      return;
    }
    depth--;
  }
}

void DescendantsNodeListObj::chunkAdvance(NodePtr &nd, unsigned &depth)
{
  if (!nd)
    return;
  if (nd.assignFirstChild() == accessOK) {
    depth++;
    return;
  }
  if (depth == 0) {
    nd.clear();
    return;
  }
  while (nd.assignNextChunkSibling() != accessOK) {
    if (depth == 1 || nd.assignOrigin() != accessOK) {
      nd.clear();
      return;
    }
    depth--;
  }
}

SelectByClassNodeListObj::SelectByClassNodeListObj(NodeListObj *nl, ComponentName::Id cls)
: nodeList_(nl), cls_(cls)
{
  hasSubObjects_ = 1;
}

NodePtr SelectByClassNodeListObj::nodeListFirst(EvalContext &context, Interpreter &interp)
{
  for (;;) {
    NodePtr nd = nodeList_->nodeListFirst(context, interp);
    if (!nd || nd->classDef().className == cls_)
      return nd;
    // All nodes in a chunk have the same class
    bool chunk;
    nodeList_ = nodeList_->nodeListChunkRest(context, interp, chunk);
  }
  // not reached
  return NodePtr();
}

NodeListObj *SelectByClassNodeListObj::nodeListRest(EvalContext &context, Interpreter &interp)
{
  for (;;) {
    NodePtr nd = nodeList_->nodeListFirst(context, interp);
    if (!nd || nd->classDef().className == cls_)
      break;
    // All nodes in a chunk have the same class
    bool chunk;
    nodeList_ = nodeList_->nodeListChunkRest(context, interp, chunk);
  }
  NodeListObj *tem = nodeList_->nodeListRest(context, interp);
  ELObjDynamicRoot protect(interp, tem);
  return new (interp) SelectByClassNodeListObj(tem, cls_);
}

NodeListObj *SelectByClassNodeListObj::nodeListChunkRest(EvalContext &context, Interpreter &interp, bool &chunk)
{
  for (;;) {
    NodePtr nd = nodeList_->nodeListFirst(context, interp);
    if (!nd)
      return interp.makeEmptyNodeList();
    if (nd->classDef().className == cls_)
      break;
    bool tem;
    nodeList_ = nodeList_->nodeListChunkRest(context, interp, tem);
  }
  NodeListObj *tem = nodeList_->nodeListChunkRest(context, interp, chunk);
  ELObjDynamicRoot protect(interp, tem);
  return new (interp) SelectByClassNodeListObj(tem, cls_);
}

void SelectByClassNodeListObj::traceSubObjects(Collector &c) const
{
  c.trace(nodeList_);
}

MapNodeListObj::MapNodeListObj(FunctionObj *func, NodeListObj *nl,
			       const ConstPtr<Context> &context,
			       NodeListObj *mapped)
: func_(func), nl_(nl), context_(context), mapped_(mapped)
{
  hasSubObjects_ = 1;
}

NodePtr MapNodeListObj::nodeListFirst(EvalContext &context, Interpreter &interp)
{
  for (;;) {
    if (!mapped_) {
      mapNext(context, interp);
      if (!mapped_)
	break;
    }
    NodePtr nd = mapped_->nodeListFirst(context, interp);
    if (nd)
      return nd;
    mapped_ = 0;
  }
  return NodePtr();
}

NodeListObj *MapNodeListObj::nodeListRest(EvalContext &context, Interpreter &interp)
{
  for (;;) {
    if (!mapped_) {
      mapNext(context, interp);
      if (!mapped_)
	break;
    }
    NodePtr nd = mapped_->nodeListFirst(context, interp);
    if (nd) {
      NodeListObj *tem = mapped_->nodeListRest(context, interp);
      ELObjDynamicRoot protect(interp, tem);
      return new (interp) MapNodeListObj(func_, nl_, context_, tem);
    }
    mapped_ = 0;
  }
  return interp.makeEmptyNodeList();
}

void MapNodeListObj::mapNext(EvalContext &context, Interpreter &interp)
{
  if (!func_)
    return;
  NodePtr nd = nl_->nodeListFirst(context, interp);
  if (!nd)
    return;
  VM vm(context, interp);
  context_->set(vm);
  InsnPtr insn(func_->makeCallInsn(1, interp, context_->loc, InsnPtr()));
  ELObj *ret = vm.eval(insn.pointer(), 0, new (interp) NodePtrNodeListObj(nd));
  if (interp.isError(ret)) {
    func_ = 0;
    return;
  }
  mapped_ = ret->asNodeList();
  if (!mapped_) {
    interp.setNextLocation(context_->loc);
    interp.message(InterpreterMessages::returnNotNodeList);
    func_ = 0;
    return;
  }
  nl_ = nl_->nodeListRest(context, interp);
}

void MapNodeListObj::traceSubObjects(Collector &c) const
{
  c.trace(nl_);
  c.trace(func_);
  c.trace(mapped_);
  context_->traceSubObjects(c);
}

bool MapNodeListObj::suppressError()
{
  return func_ == 0;
}

MapNodeListObj::Context::Context(const EvalContext &context, const Location &l)
: loc(l),
  haveStyleStack_(context.styleStack != 0),
  processingMode_(context.processingMode),
  currentNode_(context.currentNode),
  overridingStyle_(context.overridingStyle)
{
}

void MapNodeListObj::Context::set(EvalContext &context) const
{
  context.processingMode = processingMode_;
  context.currentNode = currentNode_;
  context.overridingStyle = overridingStyle_;
  if (!haveStyleStack_)
    context.styleStack = 0;
}

void MapNodeListObj::Context::traceSubObjects(Collector &c) const
{
  c.trace(overridingStyle_);
}

SelectElementsNodeListObj::SelectElementsNodeListObj(NodeListObj *nodeList,
						     const ConstPtr<PatternSet> &patterns)
: nodeList_(nodeList), patterns_(patterns)
{
  ASSERT(!patterns_.isNull());
  hasSubObjects_ = 1;
}

SelectElementsNodeListObj::SelectElementsNodeListObj(NodeListObj *nodeList,
						     NCVector<Pattern> &patterns)
: nodeList_(nodeList)
{
  hasSubObjects_ = 1;
  Ptr<PatternSet> tem(new PatternSet);
  tem->swap(patterns);
  patterns_ = tem;
}

void SelectElementsNodeListObj::traceSubObjects(Collector &c) const
{
  c.trace(nodeList_);
}

NodePtr SelectElementsNodeListObj::nodeListFirst(EvalContext &context, Interpreter &interp)
{
  for (;;) {
    NodePtr nd = nodeList_->nodeListFirst(context, interp);
    if (!nd)
      return nd;
    for (size_t i = 0; i < patterns_->size(); i++)
      if ((*patterns_)[i].matches(nd, interp))
        return nd;
    bool chunk;
    nodeList_ = nodeList_->nodeListChunkRest(context, interp, chunk);
  }
  // not reached
  return NodePtr();
}

NodeListObj *SelectElementsNodeListObj::nodeListRest(EvalContext &context, Interpreter &interp)
{
  for (;;) {
    NodePtr nd = nodeList_->nodeListFirst(context, interp);
    if (!nd)
      break;
    bool matched = 0;
    for (size_t i = 0; i < patterns_->size(); i++) {
      if ((*patterns_)[i].matches(nd, interp)) {
        matched = 1;
        break;
      }
    }
    if (matched)
      break;
    bool chunk;
    nodeList_ = nodeList_->nodeListChunkRest(context, interp, chunk);
  }
  bool chunk;
  NodeListObj *tem = nodeList_->nodeListChunkRest(context, interp, chunk);
  ELObjDynamicRoot protect(interp, tem);
  return new (interp) SelectElementsNodeListObj(tem, patterns_);
}

SiblingNodeListObj::SiblingNodeListObj(const NodePtr &first, const NodePtr &end)
: first_(first), end_(end)
{
}

NodePtr SiblingNodeListObj::nodeListFirst(EvalContext &, Interpreter &)
{
  if (*first_ == *end_)
    return NodePtr();
  return first_;
}

NodeListObj *SiblingNodeListObj::nodeListRest(EvalContext &context, Interpreter &interp)
{
  if (*first_ == *end_)
    return interp.makeEmptyNodeList();
  NodePtr nd;
  if (first_->nextSibling(nd) != accessOK)
    CANNOT_HAPPEN();
  return new (interp) SiblingNodeListObj(nd, end_);
}

NodeListObj *SiblingNodeListObj::nodeListChunkRest(EvalContext &context, Interpreter &interp, bool &chunk)
{
  if (first_->chunkContains(*end_)) {
    chunk = 0;
    return nodeListRest(context, interp);
  }
  NodePtr nd;
  if (first_->nextChunkSibling(nd) != accessOK)
    CANNOT_HAPPEN();
  chunk = 1;
  return new (interp) SiblingNodeListObj(nd, end_);
}

#ifdef DSSSL_NAMESPACE
}
#endif

#include "primitive_inst.cxx"
