// Copyright (c) 1997 James Clark
// See the file COPYING for copying permission.

#ifndef SdNode_INCLUDED
#define SdNode_INCLUDED 1

#include "Boolean.h"
#include "Node.h"
#include "Sd.h"
#include "Syntax.h"
#include "GroveBuilder.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifdef GROVE_NAMESPACE
#define GROVE_NAMESPACE_SCOPE GROVE_NAMESPACE::
#else
#define GROVE_NAMESPACE_SCOPE
#endif

class SPGROVE_API SdNode {
public:
  virtual GROVE_NAMESPACE_SCOPE AccessResult
    getSd(ConstPtr<Sd> &sd,
          ConstPtr<Syntax> &prologSyntax,
	 ConstPtr<Syntax> &instanceSyntax) const = 0;
  static const GROVE_NAMESPACE_SCOPE Node::IID iid;
  static const SdNode *convert(const GROVE_NAMESPACE_SCOPE NodePtr &nd) {
    const void *p;
    if (nd && nd->queryInterface(iid, p))
      return (const SdNode *)p;
    else
      return 0;
  }
};

#undef GROVE_NAMESPACE_SCOPE

#ifdef SP_NAMESPACE
}
#endif

#endif /* not SdNode_INCLUDED */
