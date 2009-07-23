// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#ifndef GroveApp_INCLUDED
#define GroveApp_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "ParserApp.h"
#include "GroveBuilder.h"
#include "HashTable.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class SPGROVE_API GroveApp : public ParserApp {
public:
  GroveApp(const char *requiredCodingSystem = 0);
  ErrorCountEventHandler *makeEventHandler();
  int generateEvents(ErrorCountEventHandler *eceh);
  virtual void processGrove() = 0;

  class GenerateEventArgs {
  public:
    int run() { return app_->inheritedGenerateEvents(eceh_); }
  private:
    GenerateEventArgs(ErrorCountEventHandler *eceh, GroveApp *app)
      : app_(app), eceh_(eceh) { }
    ErrorCountEventHandler *eceh_;
    GroveApp *app_;
    friend class GroveApp;
  };
  friend class GenerateEventArgs;
  void dispatchMessage(const Message &);
protected:
  GROVE_NAMESPACE_SCOPE NodePtr rootNode_;
private:
  int inheritedGenerateEvents(ErrorCountEventHandler *eceh) {
    return ParserApp::generateEvents(eceh);
  }
};

inline
OutputCharStream &operator<<(OutputCharStream &os, 
#ifdef GROVE_NAMESPACE
			     GROVE_NAMESPACE::
#endif
			     GroveString &str)
{
  return os.write(str.data(), str.size());
}

#ifdef SP_NAMESPACE
}
#endif
#endif /* not GroveApp_INCLUDED */
