// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef EvalContext_INCLUDED
#define EvalContext_INCLUDED 1

#include "Boolean.h"
#include "Node.h"
#include "Vector.h"
#include <stddef.h>

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class StyleStack;
class StyleObj;
class ProcessingMode;

class EvalContext {
public:
  class CurrentNodeSetter {
  public:
   CurrentNodeSetter(const NodePtr &, const ProcessingMode *, EvalContext &);
   ~CurrentNodeSetter();
  private:
    EvalContext *ec_;
    NodePtr saveCurrentNode_;
    const ProcessingMode *saveProcessingMode_;
  };

  EvalContext();
  StyleStack *styleStack;
  unsigned specLevel; // level of the specification flow object in the style stack
  StyleObj *overridingStyle;
  Vector<size_t> *actualDependencies;
  NodePtr currentNode;
  const ProcessingMode *processingMode;
};

inline
EvalContext::EvalContext()
: styleStack(0), overridingStyle(0),
  actualDependencies(0), processingMode(0)
{
}

inline
EvalContext::CurrentNodeSetter::CurrentNodeSetter(const NodePtr &node,
						  const ProcessingMode *mode,
						  EvalContext &ec)
: ec_(&ec), saveCurrentNode_(ec.currentNode), saveProcessingMode_(ec.processingMode)
{
  ec.currentNode = node;
  ec.processingMode = mode;
}

inline
EvalContext::CurrentNodeSetter::~CurrentNodeSetter()
{
  ec_->currentNode = saveCurrentNode_;
  ec_->processingMode = saveProcessingMode_;
}

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not EvalContext_INCLUDED */
