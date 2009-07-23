// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef ProcessingMode_INCLUDED
#define ProcessingMode_INCLUDED 1

#include "Named.h"
#include "StringC.h"
#include "Location.h"
#include "Owner.h"
#include "Vector.h"
#include "NCVector.h"
#include "NamedTable.h"
#include "Expression.h"
#include "Insn.h"
#include "Boolean.h"
#include "Node.h"
#include "FOTBuilder.h"
#include "IList.h"
#include "Pattern.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class Interpreter;
class SosofoObj;

class ProcessingMode : public Named {
public:
  enum RuleType {
    styleRule,
    constructionRule
  };
  enum { nRuleType = 2 };

  class Specificity {
  public:
    Specificity();
    bool isStyle() const;
  private:
    bool toInitial_; // 1 if the match fell through from a named processing mode to
	             // the initial processing mode
    RuleType ruleType_;
    size_t nextRuleIndex_;
    friend class ProcessingMode;
  };

  class Action : public Resource {
  public:
    Action(unsigned partIndex, Owner<Expression> &, const Location &);
    void compile(Interpreter &, RuleType);
    void get(InsnPtr &, SosofoObj *&) const;
    const Location &location() const;
    unsigned partIndex() const;
  private:
    Location defLoc_;
    Owner<Expression> expr_;
    // One of these will be non-null.
    InsnPtr insn_;
    // must be permanent
    SosofoObj *sosofo_;
    unsigned partIndex_;
  };

  class Rule {
  public:
    Rule();
    Rule(const Ptr<Action> &);
    const Action &action() const;
    Action &action();
    virtual int compareSpecificity(const Rule &) const;
    const Location &location() const;
    void swap(Rule &);
  private:
    Ptr<Action> action_;
  };

  class ElementRule : public Rule, public Pattern, public Link {
  public:
    ElementRule(const Ptr<Action> &, Pattern &);
    int compareSpecificity(const Rule &) const;
  };

  ProcessingMode(const StringC &, const ProcessingMode *initial = 0);
  void addRule(bool matchesRoot, NCVector<Pattern> &, Owner<Expression> &expr,
	       RuleType, const Location &, Interpreter &);
  // Specificity gives specificity of last match; gets specificity of current match.
  const Rule *findMatch(const NodePtr &, Pattern::MatchContext &, Messenger &,
			Specificity &) const;
  void compile(Interpreter &);
  bool defined() const;
  void setDefined();

  struct ElementRules : public Named {
  public:
    ElementRules(const StringC &);
    Vector<const ElementRule *> rules[nRuleType];
  };

  struct GroveRules {
    GroveRules();
    bool built;
    NamedTable<ElementRules> elementTable;
    Vector<const ElementRule *> otherRules[nRuleType];
    void build(const IList<ElementRule> *, const NodePtr &, Messenger &);
    static void sortRules(Vector<const ElementRule *> &v);
  };
private:
  const Rule *findElementMatch(const StringC &, const NodePtr &,
			       Pattern::MatchContext &, Messenger &,
			       Specificity &) const;
  const Rule *findRootMatch(const NodePtr &, Pattern::MatchContext &, Messenger &,
			    Specificity &) const;
  const GroveRules &groveRules(const NodePtr &, Messenger &) const;
  static void elementRuleAdvance(const NodePtr &nd, Pattern::MatchContext &context,
		          Messenger &mgr, Specificity &specificity,
			  const Vector<const ElementRule *> &vec);

  Vector<Rule> rootRules_[nRuleType];
  IList<ElementRule> elementRules_[nRuleType];
  NCVector<GroveRules> groveRules_;
  const ProcessingMode *initial_; // 0 for initial mode
  bool defined_;
};


inline
bool ProcessingMode::defined() const
{
  return defined_;
}

inline
void ProcessingMode::setDefined()
{
  defined_ = 1;
}

inline
ProcessingMode::Specificity::Specificity()
: toInitial_(0), nextRuleIndex_(0), ruleType_(styleRule)
{
}

inline
bool ProcessingMode::Specificity::isStyle() const
{
  return ruleType_ == styleRule;
}

inline
void ProcessingMode::Action::get(InsnPtr &insn, SosofoObj *&sosofo) const
{
  insn = insn_;
  sosofo = sosofo_;
}

inline
const Location &ProcessingMode::Action::location() const
{
  return defLoc_;
}

inline
ProcessingMode::Action &ProcessingMode::Rule::action()
{
  return *action_;
}

inline
const ProcessingMode::Action &ProcessingMode::Rule::action() const
{
  return *action_;
}

inline
unsigned ProcessingMode::Action::partIndex() const
{
  return partIndex_;
}

inline
const Location &ProcessingMode::Rule::location() const
{
  return action_->location();
}

inline
void ProcessingMode::Rule::swap(Rule &r)
{
  action_.swap(r.action_);
}

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not ProcessingMode_INCLUDED */
