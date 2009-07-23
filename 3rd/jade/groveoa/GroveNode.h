#ifndef GroveNode_INCLUDED
#define GroveNode_INCLUDED 1

#include "Owner.h"
#include "Boolean.h"
#include "Node.h"

struct ParserThread {
  virtual ~ParserThread() { }
};

HRESULT makeRootNode(const GROVE_NAMESPACE::NodePtr &,
		     SP_NAMESPACE::Owner<ParserThread> &,
		     SgmlDocumentNode **);

#endif /* not GroveNode_INCLUDED */
