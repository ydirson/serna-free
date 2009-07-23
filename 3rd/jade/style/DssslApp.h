// Copyright (c) 1996, 1997 James Clark
// See the file copying.txt for copying permission.

#ifndef DssslApp_INCLUDED
#define DssslApp_INCLUDED 1

#include "GroveApp.h"
#include "FOTBuilder.h"
#include "GroveManager.h"
#include "dsssl_ns.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class STYLE_API DssslApp : public GroveApp, public GroveManager {
public:
  DssslApp(int unitsPerInch);
  virtual FOTBuilder *makeFOTBuilder(const FOTBuilder::Extension *&) = 0;
  int processSysid(const StringC &);
  bool load(const StringC &sysid, const Vector<StringC> &active,
	    const NodePtr &parent, NodePtr &rootNode,
	    const Vector<StringC> &architecture);
  bool readEntity(const StringC &, StringC &);
protected:
  void processOption(AppChar opt, const AppChar *arg);
  int init(int argc, AppChar **argv);
  int unitsPerInch_;
  StringC defaultOutputBasename_;
private:
  void processGrove();
  int generateEvents(ErrorCountEventHandler *eceh);
  Boolean getDssslSpecFromGrove();
  Boolean getDssslSpecFromPi(const Char *s, size_t n,
			     const Location &loc);
  static void splitOffId(StringC &, StringC &);
  Boolean handleSimplePi(const Char *, size_t, const Location &);
  Boolean handleAttlistPi(const Char *, size_t, const Location &);
  static void skipS(const Char *&s, size_t &n);
  static Boolean isS(Char c);
  static Boolean matchCi(const StringC &s, const char *key);
  static Boolean matchCi(const Char *s, size_t n, const char *key);
  static Boolean getAttribute(const Char *&s, size_t &n,
			      StringC &name, StringC &value);
  Boolean initSpecParser();

  Boolean dssslSpecOption_;
  StringC dssslSpecSysid_; // system ID of doc
  StringC dssslSpecId_; // unique ID in doc
  // Variables to be defined as true
  Vector<StringC> defineVars_;
  SgmlParser specParser_;
  HashTable<StringC,NodePtr> groveTable_;
  StringC rootSystemId_;
  bool debugMode_;
  bool dsssl2_;
};

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not DssslApp_INCLUDED */
