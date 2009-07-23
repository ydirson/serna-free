// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef Expression_INCLUDED
#define Expression_INCLUDED 1

#include "ELObj.h"
#include "Owner.h"
#include "Vector.h"
#include "NCVector.h"
#include "Resource.h"
#include "Ptr.h"
#include "Insn.h"
#include "Named.h"
#include "Location.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class Interpreter;
class Identifier;

struct BoundVar {
  const Identifier *ident;
  enum {
    usedFlag = 01,
    assignedFlag = 02,
    sharedFlag = 04,
    uninitFlag = 010,
    boxedFlags = assignedFlag|sharedFlag
  };
  static bool flagsBoxed(unsigned f) { return (f & boxedFlags) == boxedFlags; }
  bool boxed() const { return flagsBoxed(flags); }
  unsigned flags;
  unsigned reboundCount;
};

class BoundVarList : public Vector<BoundVar> {
public:
  BoundVarList() { }
  BoundVarList(const Vector<const Identifier *> &);
  BoundVarList(const Vector<const Identifier *> &, size_t, unsigned flags = 0);
  void append(const Identifier *, unsigned flags);
  void mark(const Identifier *, unsigned flags);
  void removeUnused();
  void remove(const Vector<const Identifier *> &);
  void rebind(const Vector<const Identifier *> &);
  void unbind(const Vector<const Identifier *> &);
  BoundVar *find(const Identifier *);
};

class Environment {
public:
  Environment();
  Environment(const BoundVarList &frameVars,
	      const BoundVarList &closureVars);
  void boundVars(BoundVarList &) const;
  bool lookup(const Identifier *var,
	      bool &isFrame, int &index, unsigned &flags) const;
  void augmentFrame(const BoundVarList &,
		    int stackPos);
private:
  struct FrameVarList : public Resource {
    int stackPos;
    const BoundVarList *vars;
    ConstPtr<FrameVarList> next;
  };
  ConstPtr<FrameVarList> frameVarList_;
  const BoundVarList *closureVars_;
};

class Expression {
public:
  Expression(const Location &);
  virtual ~Expression() { }
  virtual InsnPtr compile(Interpreter &, const Environment &, int,
			  const InsnPtr &) = 0;
  static
    InsnPtr optimizeCompile(Owner<Expression> &, Interpreter &, const Environment &, int,
                            const InsnPtr &);
  virtual void markBoundVars(BoundVarList &vars, bool);
  virtual void optimize(Interpreter &, const Environment &, Owner<Expression> &);
  virtual ELObj *constantValue() const;
  virtual bool canEval(bool maybeCall) const = 0;
  virtual const Identifier *keyword() const;
  const Location &location() const;
protected:
  static
  InsnPtr compilePushVars(Interpreter &interp,
			  const Environment &env, int stackPos,
			  const BoundVarList &vars, size_t varIndex,
			  const InsnPtr &next);
private:
  Location loc_;
};

class ConstantExpression : public Expression {
public:
  ConstantExpression(ELObj *, const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  void optimize(Interpreter &, const Environment &, Owner<Expression> &);
  bool canEval(bool maybeCall) const;
  const Identifier *keyword() const;
private:
  ELObj *obj_;			// must be permanent
};

class ResolvedConstantExpression : public Expression {
public:
  ResolvedConstantExpression(ELObj *, const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  bool canEval(bool maybeCall) const;
  ELObj *constantValue() const;
private:
  ELObj *obj_;
};

class CallExpression : public Expression {
public:
  CallExpression(Owner<Expression> &, NCVector<Owner<Expression> > &,
		 const Location &loc);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  void markBoundVars(BoundVarList &vars, bool);
  int nArgs();
  bool canEval(bool maybeCall) const;
private:
  Owner<Expression> op_;
  NCVector<Owner<Expression> > args_;
};

class VariableExpression : public Expression {
public:
  VariableExpression(const Identifier *, const Location &loc);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  void optimize(Interpreter &, const Environment &, Owner<Expression> &);
  void markBoundVars(BoundVarList &vars, bool);
  bool canEval(bool maybeCall) const;
private:
  const Identifier *ident_;
  bool isTop_;
};

class IfExpression : public Expression {
public:
  IfExpression(Owner<Expression> &,
	       Owner<Expression> &,
	       Owner<Expression> &,
	       const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  void markBoundVars(BoundVarList &vars, bool);
  bool canEval(bool maybeCall) const;
  void optimize(Interpreter &, const Environment &, Owner<Expression> &);
private:
  Owner<Expression> test_;
  Owner<Expression> consequent_;
  Owner<Expression> alternate_;
};

class OrExpression : public Expression {
public:
  OrExpression(Owner<Expression> &,
	       Owner<Expression> &,
	       const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  void markBoundVars(BoundVarList &vars, bool);
  bool canEval(bool maybeCall) const;
  void optimize(Interpreter &, const Environment &, Owner<Expression> &);
private:
  Owner<Expression> test1_;
  Owner<Expression> test2_;
};

class CondFailExpression : public Expression {
public:
  CondFailExpression(const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  bool canEval(bool maybeCall) const;
};

class CaseExpression : public Expression {
public:
  struct Case {
    Vector<ELObj *> datums;
    Owner<Expression> expr;
  };
  CaseExpression(Owner<Expression> &,
		 NCVector<Case> &,
		 Owner<Expression> &,
		 const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  void markBoundVars(BoundVarList &vars, bool);
  bool canEval(bool maybeCall) const;
  void optimize(Interpreter &, const Environment &, Owner<Expression> &);
private:
  Owner<Expression> key_;
  NCVector<Case> cases_;
  Vector<unsigned> nResolved_;
  Owner<Expression> else_;
};

class LambdaExpression : public Expression {
public:
  LambdaExpression(Vector<const Identifier *> &vars,
		   NCVector<Owner<Expression> > &inits,
		   int nOptional,
		   bool hasRest,
		   int nKey,
		   Owner<Expression> &body,
		   const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  void markBoundVars(BoundVarList &vars, bool);
  bool canEval(bool maybeCall) const;
private:
  Vector<const Identifier *> formals_;
  NCVector<Owner<Expression> > inits_;
  Signature sig_;
  Owner<Expression> body_;
};

class LetExpression : public Expression {
public:
  LetExpression(Vector<const Identifier *> &vars,
		NCVector<Owner<Expression> > &inits,
		Owner<Expression> &body,
		const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  void markBoundVars(BoundVarList &vars, bool);
  bool canEval(bool maybeCall) const;
protected:
  InsnPtr compileInits(Interpreter &interp, const Environment &env,
		       const BoundVarList &initVars,
		       size_t initIndex, int stackPos, const InsnPtr &next);
  Vector<const Identifier *> vars_;
  NCVector<Owner<Expression> > inits_;
  Owner<Expression> body_;
};

class LetStarExpression : public LetExpression {
public:
  LetStarExpression(Vector<const Identifier *> &vars,
		    NCVector<Owner<Expression> > &inits,
		    Owner<Expression> &body,
		    const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
private:
  InsnPtr compileInits(Interpreter &interp, const Environment &env,
		       const BoundVarList &initVars,
		       size_t initIndex, int stackPos, const InsnPtr &next);
};

class LetrecExpression : public Expression {
public:
  LetrecExpression(Vector<const Identifier *> &vars,
		   NCVector<Owner<Expression> > &inits,
		   Owner<Expression> &body,
		   const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  void markBoundVars(BoundVarList &vars, bool);
  bool canEval(bool maybeCall) const;
private:
  InsnPtr compileInits(Interpreter &interp, const Environment &env,
		       size_t initIndex, int stackPos, const InsnPtr &next);
  Vector<const Identifier *> vars_;
  NCVector<Owner<Expression> > inits_;
  Owner<Expression> body_;
};

class QuasiquoteExpression : public Expression {
public:
  enum Type {
    listType,
    improperType,
    vectorType
  };
  QuasiquoteExpression(NCVector<Owner<Expression> > &,
		       Vector<PackedBoolean> &spliced,
		       Type type,
		       const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  void markBoundVars(BoundVarList &vars, bool);
  bool canEval(bool maybeCall) const;
  void optimize(Interpreter &, const Environment &, Owner<Expression> &);
private:
  NCVector<Owner<Expression> > members_;
  Vector<PackedBoolean> spliced_;
  Type type_;
};

class SequenceExpression : public Expression {
public:
  SequenceExpression(NCVector<Owner<Expression> > &,
		     const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  void markBoundVars(BoundVarList &vars, bool);
  bool canEval(bool maybeCall) const;
  void optimize(Interpreter &, const Environment &, Owner<Expression> &);
private:
  NCVector<Owner<Expression> > sequence_;
};

class AssignmentExpression : public Expression {
public:
  AssignmentExpression(const Identifier *,
                       Owner<Expression> &,
		       const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  void markBoundVars(BoundVarList &vars, bool);
  bool canEval(bool maybeCall) const;
private:
  const Identifier *var_;
  Owner<Expression> value_;
};

class ProcessingMode;

class WithModeExpression : public Expression {
public:
  WithModeExpression(const ProcessingMode *, Owner<Expression> &,
		     const Location &);
  InsnPtr compile(Interpreter &, const Environment &, int,
		  const InsnPtr &);
  void markBoundVars(BoundVarList &vars, bool);
  bool canEval(bool maybeCall) const;
private:
  const ProcessingMode *mode_;
  Owner<Expression> expr_;
};

class StyleExpression : public Expression {
public:
  StyleExpression(Vector<const Identifier *> &,
		  NCVector<Owner<Expression> > &,
		  const Location &loc);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
  void markBoundVars(BoundVarList &vars, bool);
  bool canEval(bool maybeCall) const;
protected:
  virtual void unknownStyleKeyword(const Identifier *ident, Interpreter &interp,
                                   const Location &loc) const;
  virtual bool maybeStyleKeyword(const Identifier *ident) const;
  Vector<const Identifier *> keys_;
  NCVector<Owner<Expression> > exprs_;
};

class FlowObj;

class MakeExpression : public StyleExpression {
public:
  MakeExpression(const Identifier *,
                 Vector<const Identifier *> &,
		 NCVector<Owner<Expression> > &,
		 const Location &loc);
  InsnPtr compile(Interpreter &, const Environment &, int, const InsnPtr &);
private:
  InsnPtr compileNonInheritedCs(Interpreter &interp, const Environment &env,
				int stackPos, const InsnPtr &next);
  FlowObj *applyConstNonInheritedCs(FlowObj *, Interpreter &, const Environment &);
  void unknownStyleKeyword(const Identifier *ident, Interpreter &interp,
			   const Location &loc) const;
  bool maybeStyleKeyword(const Identifier *ident) const;

  const Identifier *foc_;
};

inline
const Location &Expression::location() const
{
  return loc_;
}

inline
InsnPtr Expression::optimizeCompile(Owner<Expression> &expr, Interpreter &interp,
				    const Environment &env, int stackPos,
				    const InsnPtr &next)
{
  expr->optimize(interp, env, expr);
  return expr->compile(interp, env, stackPos, next);
}

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not Expression_INCLUDED */
