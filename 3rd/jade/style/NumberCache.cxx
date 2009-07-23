// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "NumberCache.h"
#include "macros.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

NumberCache::NumberCache()
{
}

inline
void advance(NodePtr &node)
{
  if (node.assignNextChunkAfter() != accessOK)
    CANNOT_HAPPEN();
}

// This caching scheme will be defeated (but not disastrously so)
// by numbering both notes and footnotes, say, per chapter.

unsigned long NumberCache::elementNumberAfter(const NodePtr &node, const StringC &gi,
					      const StringC &resetGi)
{
  NodePtr start;
  NodePtr lastMatch;
  unsigned long resetCount = 0;
  unsigned long count = 0;
  ElementEntry *entry = elementNumbers_.lookup(resetGi);
  if (entry) {
    unsigned long nodeIndex, entryIndex;
    bool useSubnode = 1;
    node->elementIndex(nodeIndex);
    unsigned long nodeGroveIndex = node->groveIndex();
    if (entry->node) {
      if (*entry->node == *node)
	return 0;
      entry->node->elementIndex(entryIndex);
      if (entryIndex < nodeIndex && entry->node->groveIndex() == nodeGroveIndex) {
	start = lastMatch = entry->node;
	advance(start);
	resetCount = entry->num;
      }
      else
	useSubnode = 0;
    }
    if (entry->subNode && useSubnode) {
      GroveString tem;
      if (entry->subNode->getGi(tem) == accessOK
	  && tem == GroveString(gi.data(), gi.size())) {
	if (*entry->subNode == *node)
	  return entry->subNum;
	entry->subNode->elementIndex(entryIndex);
	if (entryIndex < nodeIndex && entry->subNode->groveIndex() == nodeGroveIndex) {
	  start = entry->subNode;
	  advance(start);
	  count = entry->subNum;
	}
      }
    }
  }
  else
    elementNumbers_.insert(entry = new ElementEntry(resetGi));
  if (!start) {
    node->getGroveRoot(start);
    start->getDocumentElement(start);
  }
  for (;;) {
    GroveString tem;
    if (start->getGi(tem) == accessOK) {
      if (tem == GroveString(resetGi.data(), resetGi.size())) {
	lastMatch = start;
	resetCount++;
	count = 0;
      }
      else if (tem == GroveString(gi.data(), gi.size()))
	count++;
    }
    if (*start == *node)
      break;
    advance(start);
  }
  entry->node = lastMatch;
  entry->num = resetCount;
  entry->subNode = node;
  entry->subNum = count;
  return count;
}

unsigned long NumberCache::elementNumber(const NodePtr &node, const StringC &gi)
{
  NodePtr start;
  NodePtr lastMatch;
  unsigned long count = 0;
  ElementEntry *entry = elementNumbers_.lookup(gi);
  if (entry && entry->node) {
    if (*entry->node == *node) {
      lastMatch = node;
      return entry->num;
    }
    unsigned long nodeIndex, entryIndex;
    entry->node->elementIndex(entryIndex);
    node->elementIndex(nodeIndex);
    if (entryIndex < nodeIndex && node->groveIndex() == entry->node->groveIndex()) {
      lastMatch = start = entry->node;
      count = entry->num;
      advance(start);
    }
  }
  if (!start) {
    node->getGroveRoot(start);
    start->getDocumentElement(start);
  }
  for (;;) {
    GroveString tem;
    if (start->getGi(tem) == accessOK && tem == GroveString(gi.data(), gi.size())) {
      lastMatch = start;
      count++;
    }
    if (*start == *node)
      break;
    advance(start);
  }
  if (count) {
    ASSERT(lastMatch);
    if (!entry)
      elementNumbers_.insert(entry = new ElementEntry(gi));
    entry->node = lastMatch;
    entry->subNode.clear();
    entry->num = count;
  }
  return count;
}

bool NumberCache::childNumber(const NodePtr &node, unsigned long &result)
{
  GroveString gs;
  if (node->getGi(gs) != accessOK)
    return 0;
  NodePtr tem;
  if (node->getParent(tem) != accessOK) {
    // must be document element
    result = 0;
    return 1;
  }
  NodePtr parent(tem);
  unsigned level = 0;
  while (tem->getParent(tem) == accessOK)
    level++;
  StringC gi(gs.data(), gs.size());
  if (level >= childNumbers_.size())
    childNumbers_.resize(level + 1);
  NodePtr start;
  unsigned long count = 0;
  Entry *entry = childNumbers_[level].lookup(gi);
  if (entry) {
    if (*entry->node == *node) {
      result = entry->num;
      return 1;
    }
    // Start counting from the cached entry if it has the same
    // parent as us and it is before us.
    NodePtr entryParent;
    entry->node->getParent(entryParent);
    if (*entryParent == *parent) {
      unsigned long nodeIndex, entryIndex;
      entry->node->elementIndex(entryIndex);
      node->elementIndex(nodeIndex);
      if (entryIndex < nodeIndex && node->groveIndex() == entry->node->groveIndex()) {
	start = entry->node;
	count = entry->num;
      }
    }
  }
  else {
    entry = new Entry(gi);
    childNumbers_[level].insert(entry);
  }
  if (!start)
    node->firstSibling(start);
  while (*start != *node) {
    GroveString tem;
    if (start->getGi(tem) == accessOK && tem == gs)
      count++;
    if (start.assignNextChunkSibling() != accessOK)
      CANNOT_HAPPEN();
  }
  entry->node = node;
  entry->num = count;
  result = count;
  return 1;
}

NumberCache::Entry::Entry(const StringC &name)
: Named(name)
{
}

NumberCache::ElementEntry::ElementEntry(const StringC &name)
: NumberCache::Entry(name)
{
}

#ifdef DSSSL_NAMESPACE
}
#endif

