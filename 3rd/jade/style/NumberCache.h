// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef NumberCache_INCLUDED
#define NumberCache_INCLUDED 1

#include "Boolean.h"
#include "Node.h"
#include "NamedTable.h"
#include "StringC.h"
#include "NCVector.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class NumberCache {
public:
  NumberCache();
  // Return 0 if argument is not a node.
  bool childNumber(const NodePtr &node, unsigned long &);
  // Number of elements before or equal to node whose gi is equal to s.
  unsigned long elementNumber(const NodePtr &node, const StringC &s);
  // Number of elements before or equal to node whose gi is equal to s,
  // and that are after the last element before node with gi reset.
  unsigned long elementNumberAfter(const NodePtr &, const StringC &s,
				   const StringC &reset);
private:
  struct Entry : Named {
    Entry(const StringC &);
    NodePtr node;
    unsigned long num;
  };
  // A separate cache for each level of the tree.
  NCVector<NamedTable<Entry> > childNumbers_;
  struct ElementEntry : Entry {
    ElementEntry(const StringC &);
    NodePtr subNode;
    unsigned long subNum;
  };
  NamedTable<ElementEntry> elementNumbers_;
  friend struct ElementEntry;
};

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not NumberCache_INCLUDED */

