// Copyright (c) 1997 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "Pattern.h"
#include "macros.h"
#include "Vector.h"
#include "Interpreter.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

Pattern::Pattern()
: trivial_(0)
{
}

Pattern::Pattern(IList<Element> &ancestors)
: trivial_(computeTrivial(ancestors))
{
  ancestors_.swap(ancestors);
}

bool Pattern::computeTrivial(const IList<Element> &ancestors)
{
  IListIter<Element> iter(ancestors);
  if (iter.done())
    return 1;
  if (!iter.cur()->trivial())
    return 0;
  iter.next();
  if (!iter.done())
    return 0;
  return 1;
}

Pattern::Element::Element(const StringC &gi)
: gi_(gi), minRepeat_(1), maxRepeat_(1)
{
}

bool Pattern::Element::matches(const NodePtr &nd, MatchContext &context) const
{
  if (gi_.size()) {
    if (!nd->hasGi(GroveString(gi_.data(), gi_.size())))
      return 0;
  }
  else {
    GroveString tem;
    if (nd->getGi(tem) != accessOK)
      return 0;
  }
  for (IListIter<Qualifier> iter(qualifiers_); !iter.done(); iter.next())
    if (!iter.cur()->satisfies(nd, context))
      return 0;
  return 1;
}

bool Pattern::Element::trivial() const
{
  if (minRepeat_ > 1)
    return 0;
  for (IListIter<Qualifier> iter(qualifiers_); !iter.done(); iter.next())
    if (!iter.cur()->vacuous())
      return 0;
  return 1;
}

void Pattern::Element::contributeSpecificity(int *s) const
{
  if (gi_.size())
    s[giSpecificity] += minRepeat_;
  for (IListIter<Qualifier> iter(qualifiers_); !iter.done(); iter.next())
    iter.cur()->contributeSpecificity(s);
  if (minRepeat_ != maxRepeat_)
    s[repeatSpecificity] -= 1;
}

Pattern::Qualifier::~Qualifier()
{
}

bool Pattern::Qualifier::vacuous() const
{
  return 0;
}

bool Pattern::Qualifier::matchAttribute(const StringC &name,
					const StringC &value,
					const NodePtr &nd,
					MatchContext &context)
{
  NamedNodeListPtr atts;
  if (nd->getAttributes(atts) != accessOK)
    return 0;
  NodePtr att;
  if (atts->namedNode(GroveString(name.data(), name.size()), att) != accessOK)
    return 0;
  bool implied;
  if (att->getImplied(implied) == accessOK && implied)
    return 0;
  GroveString tokens;
  if (att->tokens(tokens) == accessOK) {
    if (tokens.size() != value.size())
      return 0;
    NodePtr node;
    NamedNodeListPtr normalizer;
    if (att->firstChild(node) != accessOK
        || node->getEntity(node) != accessOK
	|| node->getGroveRoot(node) != accessOK
	|| node->getEntities(normalizer) != accessOK)
      normalizer = atts;
    StringC tem(value);
    tem.resize(normalizer->normalize(&tem[0], tem.size()));
    if (tokens != GroveString(tem.data(), tem.size()))
      return 0;
  }
  else {
    NodePtr tem;
    StringC s;
    if (att->firstChild(tem) == accessOK) {
      do {
  	GroveString chunk;
	if (tem->charChunk(context, chunk) == accessOK)
	  s.append(chunk.data(), chunk.size());
	} while (tem.assignNextChunkSibling() == accessOK);
    }
    if (s != value)
      return 0;
  }
  return 1;
}

Pattern::ChildrenQualifier::ChildrenQualifier(IList<Element> &children)
{
  children.swap(children_);
}

bool Pattern::ChildrenQualifier::satisfies(const NodePtr &nd,
					   MatchContext &context) const
{
  ASSERT(!children_.empty());
  NodePtr child;
  if (nd->firstChild(child) != accessOK)
    return 0;
  Vector<const Element *> toMatch;
  for (IListIter<Element> iter(children_); !iter.done(); iter.next())
    toMatch.push_back(iter.cur());
  do {
    size_t j = 0;
    for (size_t i = 0; i < toMatch.size(); i++) {
      if (!toMatch[i]->matches(child, context)) {
	if (j != i)
	  toMatch[j] = toMatch[i];
	j++;
      }
    }
    if (j == 0)
      return 1;
    toMatch.resize(j);
  } while (child.assignNextChunkSibling() == accessOK);
  return 0;
}

void Pattern::ChildrenQualifier::contributeSpecificity(int *s) const
{
  for (IListIter<Element> iter(children_); !iter.done(); iter.next())
    iter.cur()->contributeSpecificity(s);
}

Pattern::IdQualifier::IdQualifier(const StringC &id)
: id_(id)
{
}

bool Pattern::IdQualifier::satisfies(const NodePtr &nd, MatchContext &context) const
{
  GroveString nodeId;
  if (nd->getId(nodeId) == accessOK) {
    size_t len = id_.size();
    if (nodeId.size() == len) {
      StringC tem(id_);
      Interpreter::normalizeGeneralName(nd, tem);
      GroveString patternId(tem.data(), tem.size());
      if (patternId == nodeId)
	return 1;
    }
  }
  const Vector<StringC> &idAtts = context.idAttributeNames();
  for (size_t i = 0; i < idAtts.size(); i++)
    if (matchAttribute(idAtts[i], id_, nd, context))
      return 1;
  return 0;
}

void Pattern::IdQualifier::contributeSpecificity(int *s) const
{
  s[idSpecificity] += 1;
}

Pattern::ClassQualifier::ClassQualifier(const StringC &cls)
: class_(cls)
{
}

bool Pattern::ClassQualifier::satisfies(const NodePtr &nd, MatchContext &context) const
{
  const Vector<StringC> &classAtts = context.classAttributeNames();
  for (size_t i = 0; i < classAtts.size(); i++)
    if (matchAttribute(classAtts[i], class_, nd, context))
      return 1;
  return 0;
}

void Pattern::ClassQualifier::contributeSpecificity(int *s) const
{
  s[classSpecificity] += 1;
}

Pattern::AttributeHasValueQualifier::AttributeHasValueQualifier(const StringC &name)
: name_(name)
{
}

bool Pattern::AttributeHasValueQualifier::satisfies(const NodePtr &nd, MatchContext &context) const
{
  NamedNodeListPtr atts;
  if (nd->getAttributes(atts) != accessOK)
    return 0;
  NodePtr att;
  if (atts->namedNode(GroveString(name_.data(), name_.size()), att) != accessOK)
    return 0;
  bool implied;
  if (att->getImplied(implied) == accessOK && implied)
    return 0;
  return 1;
}

void Pattern::AttributeHasValueQualifier::contributeSpecificity(int *s) const
{
  s[attributeSpecificity] += 1;
}

Pattern::AttributeMissingValueQualifier::AttributeMissingValueQualifier(const StringC &name)
: name_(name)
{
}

bool Pattern::AttributeMissingValueQualifier::satisfies(const NodePtr &nd, MatchContext &context) const
{
  NamedNodeListPtr atts;
  if (nd->getAttributes(atts) != accessOK)
    return 1;
  NodePtr att;
  if (atts->namedNode(GroveString(name_.data(), name_.size()), att) != accessOK)
    return 1;
  bool implied;
  if (att->getImplied(implied) == accessOK && implied)
    return 1;
  return 0;
}

void Pattern::AttributeMissingValueQualifier::contributeSpecificity(int *s) const
{
  s[attributeSpecificity] += 1;
}

Pattern::AttributeQualifier::AttributeQualifier(const StringC &name, const StringC &value)
: name_(name), value_(value)
{
}

bool Pattern::AttributeQualifier::satisfies(const NodePtr &nd, MatchContext &context) const
{
  return matchAttribute(name_, value_, nd, context);
}

void Pattern::AttributeQualifier::contributeSpecificity(int *s) const
{
  s[attributeSpecificity] += 1;
}

void Pattern::PositionQualifier::contributeSpecificity(int *s) const
{
  s[positionSpecificity] += 1;
}

bool Pattern::FirstOfTypeQualifier::satisfies(const NodePtr &nd, MatchContext &context) const
{
  GroveString ndType;
  nd->getGi(ndType);
  NodePtr tem;
  if (nd->firstSibling(tem) != accessOK)
    return 1; // must be document element
  while (*tem != *nd) {
    GroveString temType;
    if (tem->getGi(temType) == accessOK && temType == ndType)
      return 0;
    tem.assignNextChunkSibling();
  }
  return 1;
}

bool Pattern::LastOfTypeQualifier::satisfies(const NodePtr &nd, MatchContext &context) const
{
  NodePtr tem;
  if (nd->nextChunkSibling(tem) != accessOK)
    return 1;
  GroveString ndType;
  nd->getGi(ndType);
  do {
    GroveString temType;
    if (tem->getGi(temType) == accessOK && temType == ndType)
      return 0;
  } while (tem.assignNextChunkSibling() == accessOK);
  return 1;
}

bool Pattern::FirstOfAnyQualifier::satisfies(const NodePtr &nd, MatchContext &) const
{
  NodePtr tem;
  if (nd->firstSibling(tem) != accessOK)
    return 1; // must be document element
  while (*tem != *nd) {
    GroveString temType;
    if (tem->getGi(temType) == accessOK)
      return 0;
    tem.assignNextChunkSibling();
  }
  return 1;
}

bool Pattern::LastOfAnyQualifier::satisfies(const NodePtr &nd, MatchContext &context) const
{
  NodePtr tem;
  if (nd->nextChunkSibling(tem) != accessOK)
    return 1;
  GroveString ndType;
  nd->getGi(ndType);
  do {
    GroveString temType;
    if (tem->getGi(temType) == accessOK)
      return 0;
  } while (tem.assignNextChunkSibling() == accessOK);
  return 1;
}

void Pattern::OnlyQualifier::contributeSpecificity(int *s) const
{
  s[onlySpecificity] += 1;
}

bool Pattern::OnlyOfTypeQualifier::satisfies(const NodePtr &nd, MatchContext &context) const
{
  GroveString ndType;
  nd->getGi(ndType);
  NodePtr tem;
  if (nd->firstSibling(tem) != accessOK)
    return 1; // must be document element
  unsigned count = 0;
  do {
    GroveString temType;
    if (tem->getGi(temType) == accessOK && temType == ndType) {
      if (count++)
	return 0;
    }
  } while (tem.assignNextChunkSibling() == accessOK);
  return 1;
}

bool Pattern::OnlyOfAnyQualifier::satisfies(const NodePtr &nd, MatchContext &context) const
{
  NodePtr tem;
  if (nd->firstSibling(tem) != accessOK)
    return 1; // must be document element
  unsigned count = 0;
  do {
    GroveString temType;
    if (tem->getGi(temType) == accessOK) {
      if (count++)
	return 0;
    }
  } while (tem.assignNextChunkSibling() == accessOK);
  return 1;
}

bool Pattern::VacuousQualifier::vacuous() const
{
  return 1;
}

Pattern::PriorityQualifier::PriorityQualifier(long n)
: n_(n)
{
}

void Pattern::PriorityQualifier::contributeSpecificity(int *s) const
{
  s[prioritySpecificity] += n_;
}

bool Pattern::PriorityQualifier::satisfies(const NodePtr &, MatchContext &) const
{
  return 1;
}

Pattern::ImportanceQualifier::ImportanceQualifier(long n)
: n_(n)
{
}

void Pattern::ImportanceQualifier::contributeSpecificity(int *s) const
{
  s[importanceSpecificity] += n_;
}

bool Pattern::ImportanceQualifier::satisfies(const NodePtr &, MatchContext &) const
{
  return 1;
}

bool Pattern::matchAncestors1(const IListIter<Element> &ancestors,
			      const NodePtr &node,
			      MatchContext &context)
{
  const Element &r = *ancestors.cur();
  NodePtr tem(node);
  for (Repeat i = 0; i < r.minRepeat(); i++) {
    if (!tem || !r.matches(tem, context))
      return 0;
    if (tem->getParent(tem) != accessOK)
      tem.clear();
  }
  Repeat i = r.minRepeat();
  for (;;) {
    IListIter<Element> up(ancestors);
    up.next();
    if (matchAncestors(up, tem, context))
      break;
    if (i == r.maxRepeat() || !tem || !r.matches(tem, context))
      return 0;
    i++;
    if (tem->getParent(tem) != accessOK)
      tem.clear();
  }
  return 1;
}

void Pattern::computeSpecificity(int *s) const
{
  for (int i = 0; i < nSpecificity; i++)
    s[i] = 0;
  for (IListIter<Element> iter(ancestors_); !iter.done(); iter.next())
    iter.cur()->contributeSpecificity(s);
}

int Pattern::compareSpecificity(const Pattern &pattern1, const Pattern &pattern2)
{
  int s1[nSpecificity];
  int s2[nSpecificity];
  int i;  // declare here to avoid gcc bug
  pattern1.computeSpecificity(s1);
  pattern2.computeSpecificity(s2);
  for (i = 0; i < nSpecificity; i++) {
    if (s1[i] != s2[i])
      return s1[i] > s2[i] ? -1 : 1;
  }
  return 0;
}

#ifdef DSSSL_NAMESPACE
}
#endif
