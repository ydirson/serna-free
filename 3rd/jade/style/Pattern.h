// Copyright (c) 1997 James Clark
// See the file copying.txt for copying permission.

#ifndef Pattern_INCLUDED
#define Pattern_INCLUDED 1

#include "Boolean.h"
#include "Node.h"
#include "Link.h"
#include "IList.h"
#include "IListIter.h"
#include "StringC.h"
#include "Vector.h"
#include "Ptr.h"
#include "Resource.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class Pattern {
public:
  class MatchContext : public SdataMapper {
  public:
    const Vector<StringC> &classAttributeNames() const;
    const Vector<StringC> &idAttributeNames() const;
  protected:
    Vector<StringC> classAttributeNames_;
    Vector<StringC> idAttributeNames_;
  };
  class Qualifier : public Link {
  public:
    virtual ~Qualifier();
    virtual bool satisfies(const NodePtr &, MatchContext &) const = 0;
    virtual void contributeSpecificity(int *) const = 0;
    virtual bool vacuous() const;
  protected:
    static bool matchAttribute(const StringC &name,
			       const StringC &value,
			       const NodePtr &nd,
			       MatchContext &context);
  };
  class IdQualifier : public Qualifier {
  public:
    IdQualifier(const StringC &);
    bool satisfies(const NodePtr &, MatchContext &) const;
    void contributeSpecificity(int *) const;
  private:
    StringC id_;
  };
  class ClassQualifier : public Qualifier {
  public:
    ClassQualifier(const StringC &);
    bool satisfies(const NodePtr &, MatchContext &) const;
    void contributeSpecificity(int *) const;
  private:
    StringC class_;
  };
  class AttributeHasValueQualifier : public Qualifier {
  public:
    AttributeHasValueQualifier(const StringC &);
    bool satisfies(const NodePtr &, MatchContext &) const;
    void contributeSpecificity(int *) const;
  private:
    StringC name_;
  };
  class AttributeMissingValueQualifier : public Qualifier {
  public:
    AttributeMissingValueQualifier(const StringC &);
    bool satisfies(const NodePtr &, MatchContext &) const;
    void contributeSpecificity(int *) const;
  private:
    StringC name_;
  };
  class AttributeQualifier : public Qualifier {
  public:
    AttributeQualifier(const StringC &, const StringC &);
    bool satisfies(const NodePtr &, MatchContext &) const;
    void contributeSpecificity(int *) const;
  private:
    StringC name_;
    StringC value_;
  };
  class PositionQualifier : public Qualifier {
  public:
    void contributeSpecificity(int *) const;
  };
  class FirstOfTypeQualifier : public PositionQualifier {
  public:
    bool satisfies(const NodePtr &, MatchContext &) const;
  };
  class LastOfTypeQualifier : public PositionQualifier {
  public:
    bool satisfies(const NodePtr &, MatchContext &) const;
  };
  class FirstOfAnyQualifier : public PositionQualifier {
  public:
    bool satisfies(const NodePtr &, MatchContext &) const;
  };
  class LastOfAnyQualifier : public PositionQualifier {
  public:
    bool satisfies(const NodePtr &, MatchContext &) const;
  };
  class OnlyQualifier : public Qualifier {
  public:
    void contributeSpecificity(int *) const;
  };
  class OnlyOfTypeQualifier : public OnlyQualifier {
  public:
    bool satisfies(const NodePtr &, MatchContext &) const;
  };
  class OnlyOfAnyQualifier : public OnlyQualifier {
  public:
    bool satisfies(const NodePtr &, MatchContext &) const;
  };
  class VacuousQualifier : public Qualifier {
  public:
    bool vacuous() const;
  };
  class PriorityQualifier : public VacuousQualifier {
  public:
    PriorityQualifier(long);
    void contributeSpecificity(int *) const;
    bool satisfies(const NodePtr &, MatchContext &) const;
  private:
    long n_;
  };
  class ImportanceQualifier : public VacuousQualifier {
  public:
    ImportanceQualifier(long);
    void contributeSpecificity(int *) const;
    bool satisfies(const NodePtr &, MatchContext &) const;
  private:
    long n_;
  };
  typedef unsigned Repeat;
  class Element : public Link {
  public:
    Element(const StringC &);
    bool matches(const NodePtr &, MatchContext &) const;
    void contributeSpecificity(int *) const;
    void addQualifier(Qualifier *);
    void setRepeat(Repeat minRepeat, Repeat maxRepeat);
    Repeat minRepeat() const;
    Repeat maxRepeat() const;
    bool mustHaveGi(StringC &) const;
    bool trivial() const;
  private:
    StringC gi_;
    Repeat minRepeat_;
    Repeat maxRepeat_;
    IList<Qualifier> qualifiers_;
  };
  class ChildrenQualifier : public Qualifier {
  public:
    ChildrenQualifier(IList<Element> &);
    bool satisfies(const NodePtr &, MatchContext &) const;
    void contributeSpecificity(int *) const;
  private:
    IList<Element> children_;
  };
  Pattern();
  Pattern(IList<Element> &);
  void swap(Pattern &);
  bool matches(const NodePtr &, MatchContext &) const;
  // If mustHaveGi returns true, then any element that matches the pattern
  // must have the string returned in str.
  bool mustHaveGi(StringC &str) const;
  // A pattern is trivial if any element that has the gi returned my mustHaveGi
  // matches the pattern, or, if mustHaveGi returns false, if any element matches
  // the pattern.
  bool trivial() const;
  static int compareSpecificity(const Pattern &, const Pattern &);
  enum {
    importanceSpecificity,
    idSpecificity,
    classSpecificity,
    giSpecificity,
    repeatSpecificity,
    prioritySpecificity,
    onlySpecificity,
    positionSpecificity,
    attributeSpecificity,
    nSpecificity
  };
private:
  Pattern(const Pattern &);	// undefined
  void operator=(const Pattern &); // undefined
  void computeSpecificity(int *) const;
  static bool computeTrivial(const IList<Element> &);

  static bool matchAncestors(const IListIter<Element> &,
			     const NodePtr &,
			     MatchContext &);
  static bool matchAncestors1(const IListIter<Element> &,
			      const NodePtr &,
			      MatchContext &);

  IList<Element> ancestors_; // first is self, second is parent ...
  bool trivial_;
};

inline
void Pattern::Element::setRepeat(Repeat minRepeat, Repeat maxRepeat)
{
  minRepeat_ = minRepeat;
  maxRepeat_ = maxRepeat;
}

inline
Pattern::Repeat Pattern::Element::minRepeat() const
{
  return minRepeat_;
}

inline
Pattern::Repeat Pattern::Element::maxRepeat() const
{
  return maxRepeat_;
}

inline
void Pattern::Element::addQualifier(Qualifier *q)
{
  qualifiers_.insert(q);
}

inline
bool Pattern::matchAncestors(const IListIter<Element> &ancestors,
			     const NodePtr &node,
			     MatchContext &context)
{
  return ancestors.done() || matchAncestors1(ancestors, node, context);
}

inline
bool Pattern::matches(const NodePtr &nd, MatchContext &context) const
{
  return matchAncestors(IListIter<Element>(ancestors_),
			nd,
			context);
}

inline
void Pattern::swap(Pattern &pattern)
{
  ancestors_.swap(pattern.ancestors_);
  bool tem = trivial_;
  trivial_ = pattern.trivial_;
  pattern.trivial_ = tem;
}

inline
bool Pattern::Element::mustHaveGi(StringC &gi) const
{
  if (minRepeat_ == 0)
    return 0;
  else if (gi_.size()) {
    gi = gi_;
    return 1;
  }
  else
    return 0;
}

inline
bool Pattern::mustHaveGi(StringC &gi) const
{
  return !ancestors_.empty() && ancestors_.head()->mustHaveGi(gi);
}

inline
const Vector<StringC> &Pattern::MatchContext::classAttributeNames() const
{
  return classAttributeNames_;
}

inline
const Vector<StringC> &Pattern::MatchContext::idAttributeNames() const
{
  return idAttributeNames_;
}

inline
bool Pattern::trivial() const
{
  return trivial_;
}

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not Pattern_INCLUDED */
