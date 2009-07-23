// Copyright (c) 1997 James Clark
// See the file COPYING for copying permission.

#ifndef LocNode_INCLUDED
#define LocNode_INCLUDED 1

#include "Boolean.h"
#include "Node.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class Location;

#ifdef GROVE_NAMESPACE
#define GROVE_NAMESPACE_SCOPE GROVE_NAMESPACE::
#else
#define GROVE_NAMESPACE_SCOPE
#endif

class GROVE_API LocNode {
public:
  virtual GROVE_NAMESPACE_SCOPE AccessResult getLocation(Location &) const = 0;
  static const GROVE_NAMESPACE_SCOPE Node::IID iid;
  static const LocNode *convert(const GROVE_NAMESPACE_SCOPE NodePtr &nd) {
    const void *p;
    if (nd && nd->queryInterface(iid, p))
      return (const LocNode *)p;
    else
      return 0;
  }
};

#undef GROVE_NAMESPACE_SCOPE

#ifdef SP_NAMESPACE
}
#endif

#endif /* not LocNode_INCLUDED */
