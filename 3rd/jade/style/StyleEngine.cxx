// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "StyleEngine.h"
#include "Interpreter.h"
#include "SchemeParser.h"
#include "FOTBuilder.h"
#include "DssslSpecEventHandler.h"
#include "ArcEngine.h"
#include "ProcessContext.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

StyleEngine::StyleEngine(Messenger &mgr,
			 GroveManager &groveManager,
			 int unitsPerInch,
			 bool debugMode,
			 bool dsssl2,
			 const FOTBuilder::Extension *extensionTable)
: interpreter_(new Interpreter(&groveManager, &mgr, unitsPerInch, debugMode, dsssl2,
			       extensionTable))
{
}

void StyleEngine::parseSpec(SgmlParser &specParser,
			    const CharsetInfo &charset,
			    const StringC &id,
			    Messenger &mgr)
{
  DssslSpecEventHandler specHandler(mgr);
  Vector<DssslSpecEventHandler::Part *> parts;
  specHandler.load(specParser, charset, id, parts);
  for (size_t i = 0; i < parts.size(); i++) {
    for (DssslSpecEventHandler::Part::Iter iter(parts[i]->iter());
         !iter.done();
	 iter.next()) {
      Owner<InputSource> in;
      iter.cur()->makeInputSource(specHandler, in);
      if (in) {
	SchemeParser scm(*interpreter_, in);
	scm.parse();
      }
    }
    interpreter_->endPart();
  }
  interpreter_->compile();
}

void StyleEngine::defineVariable(const StringC &ident)
{
  interpreter_->defineVariable(ident);
}

StyleEngine::~StyleEngine()
{
  delete interpreter_;
}

void StyleEngine::process(const NodePtr &node, FOTBuilder &fotb)
{
  ProcessContext context(*interpreter_, fotb);
  context.process(node);
}

#ifdef DSSSL_NAMESPACE
}
#endif
