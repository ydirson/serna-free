// Copyright (c) 1996, 1997 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "DssslApp.h"
#include "StorageManager.h"
#include "DssslAppMessages.h"
#include "StyleEngine.h"
#include "sptchar.h"
#include "macros.h"
#include "LocNode.h"
#include "SdNode.h"
#include "InputSource.h"
#include "jade_version.h"
#include "ArcEngine.h"

#include <ctype.h>
#include <string.h>

#ifdef SP_HAVE_LOCALE
#include <locale.h>
#endif

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

DssslApp::DssslApp(int unitsPerInch)
: GroveApp("unicode"), unitsPerInch_(unitsPerInch),
  dssslSpecOption_(0), debugMode_(0), dsssl2_(0)
{
  registerOption('G');
  registerOption('2');
  registerOption('d', SP_T("dsssl_spec"));
  registerOption('V', SP_T("variable"));
}

int DssslApp::init(int argc, AppChar **argv)
{
  int ret = GroveApp::init(argc, argv);
#ifdef SP_HAVE_LOCALE
  // Since we use strtod(), must have C numeric
  setlocale(LC_NUMERIC, "C");
#endif
  return ret;
}

int DssslApp::processSysid(const StringC &sysid)
{
  rootSystemId_ = sysid;
  ParsedSystemId v;
  if (!entityManager()->parseSystemId(sysid, systemCharset(), 0, 0,
				      *this, v))
    return 0;
  for (size_t i = v.size(); i > 0; i--)
    if (v[i - 1].storageManager->inheritable()) {
      ParsedSystemId specId;
      specId.resize(1);
      StorageObjectSpec &spec = specId[0];
      spec = v[i - 1];
      StringC &s = spec.specId;
      // replace an up to 5 character extension with .dsl
      for (size_t j = 0; j < 5; j++) {
	if (s.size() < j + 1)
	  break;
	if (s[s.size() - j - 1] == '.') {
	  s.resize(s.size() - j - 1);
	  break;
	}
      }
      if (strcmp(v[i - 1].storageManager->type(), "OSFILE") == 0)
	defaultOutputBasename_ = s;
      if (!dssslSpecOption_) {
	static const Char ext[] = { '.', 'd', 's', 'l' };
        s.append(ext, SIZEOF(ext));
	specId.unparse(systemCharset(), 0, dssslSpecSysid_);
      }
      break;
    }
  return GroveApp::processSysid(sysid);
}

void DssslApp::processOption(AppChar opt, const AppChar *arg)
{
  switch (opt) {
  case 'G':
    debugMode_ = 1;
    break;
  case '2':
    dsssl2_ = 1;
    break;
  case 'd':
    dssslSpecId_.resize(0);
    dssslSpecSysid_ = convertInput(arg);
    dssslSpecOption_ = 1;
    splitOffId(dssslSpecSysid_, dssslSpecId_);
    break;
  case 'V':
    defineVars_.push_back(convertInput(arg));
    break;
  case 'v':
    message(DssslAppMessages::versionInfo,
	    StringMessageArg(convertInput(JADE_VERSION)));
    // fall through
  default:
    GroveApp::processOption(opt, arg);
  }
}

void DssslApp::splitOffId(StringC &sysid, StringC &id)
{
  id.resize(0);
  for (size_t i = sysid.size(); i > 0; i--) {
    if (sysid[i - 1] == '#') {
      id.assign(sysid.data() + i,
		sysid.size() - i);
      sysid.resize(i - 1);
      break;
    }
  }
}

int DssslApp::generateEvents(ErrorCountEventHandler *eceh)
{
  groveTable_.insert(rootSystemId_, rootNode_);
  // Since the thread parsing the DSSSL spec is a different thread
  // from the thread parsing the document, we can't share an
  // entity manager.
  // The document parser has already been inited and so will
  // use the current entity manager.
  // The spec parser hasn't yet been inited and so will use
  // a new entity manager.
  // The parser thread is started in GroveApp::generateEvents
  // which hasn't happened yet.
  clearEntityManager();
  return GroveApp::generateEvents(eceh);
}

Boolean DssslApp::getDssslSpecFromGrove()
{
  NodeListPtr nl;
  if (rootNode_->getProlog(nl) != accessOK)
    return 0;
  for (;;) {
    NodePtr nd;
    if (nl->first(nd) != accessOK)
      break;
    GroveString pi;
    if (nd->getSystemData(pi) == accessOK) {
      Location loc;
      const LocNode *lnd = LocNode::convert(nd);
      if (lnd)
	lnd->getLocation(loc);
      if (getDssslSpecFromPi(pi.data(), pi.size(), loc))
	return 1;
    }
    if (nl.assignRest() != accessOK)
      break;
  }
  return 0;
}

Boolean DssslApp::getDssslSpecFromPi(const Char *s, size_t n,
				     const Location &loc)
{
  static struct {
    const char *key;
    Boolean (DssslApp::*handler)(const Char *s, size_t, const Location &);
  } pis[] = {
    { "xml-stylesheet", &DssslApp::handleAttlistPi },
    { "xml:stylesheet", &DssslApp::handleAttlistPi },
    { "stylesheet", &DssslApp::handleAttlistPi },
    { "dsssl", &DssslApp::handleSimplePi },
  };
  for (size_t i = 0; i < SIZEOF(pis); i++) {
    size_t len = strlen(pis[i].key);
    if (n >= len
        && matchCi(s, len, pis[i].key)
	&& (n == len || isS(s[len]))) {
      s += len;
      n -= len;
      return (this->*pis[i].handler)(s, n, loc);
    }
  }
  return 0;
}

Boolean DssslApp::handleSimplePi(const Char *s, size_t n,
				 const Location &loc)
{
  skipS(s, n);
  if (n == 0)
    return 0;
  StringC sysid(s, n);
  splitOffId(sysid, dssslSpecId_);
  return entityManager()->expandSystemId(sysid, loc, 0, systemCharset(), 0, *this,
				         dssslSpecSysid_);
}

Boolean DssslApp::handleAttlistPi(const Char *s, size_t n,
				  const Location &loc)
{
  // FIXME maybe give warnings if syntax is wrong
  Boolean hadHref = 0;
  StringC href;
  Boolean isDsssl = 0;
  StringC name;
  StringC value;
  while (getAttribute(s, n, name, value)) {
    if (matchCi(name, "type")) {
      static const char *types[] = {
	"text/dsssl",
	"text/x-dsssl",
	"application/dsssl",
	"application/x-dsssl"
      };
      for (size_t i = 0; i < SIZEOF(types); i++)
	if (matchCi(value, types[i])) {
	  isDsssl = 1;
	  break;
	}
      if (!isDsssl)
	return 0;
    }
    else if (matchCi(name, "href")) {
      hadHref = 1;
      value.swap(href);
    }
  }
  if (!isDsssl || !hadHref)
    return 0;
  splitOffId(href, dssslSpecId_);
  // FIXME should use location of attribute value rather than location of PI
  return entityManager()->expandSystemId(href, loc, 0, systemCharset(), 0, *this,
				         dssslSpecSysid_);
}

void DssslApp::skipS(const Char *&s, size_t &n)
{
  while (n > 0 && isS(*s))
    s++, n--;
}

Boolean DssslApp::isS(Char c)
{
  return c <= CHAR_MAX && isspace((unsigned char)c);
}

Boolean DssslApp::matchCi(const StringC &s, const char *key)
{
  return matchCi(s.data(), s.size(), key);
}

Boolean DssslApp::matchCi(const Char *s, size_t n, const char *key)
{
  for (; *key; key++, s++, n--) {
    if (!n)
      return 0;
    if (*s != tolower(*key) && *s != toupper(*key))
      return 0;
  }
  return n == 0;
}

Boolean DssslApp::getAttribute(const Char *&s, size_t &n,
			       StringC &name, StringC &value)
{
  name.resize(0);
  value.resize(0);
  skipS(s, n);
  for (;;) {
    if (n == 0)
      return 0;
    if (*s == '=' || isS(*s))
      break;
    name += *s;
    s++, n--;
  }
  skipS(s, n);
  if (n == 0 || *s != '=')
    return 0;
  s++, n--;
  skipS(s, n);
  Char quote = 0;
  if (n > 0 && (*s == '"' || *s == '\'')) {
    quote = *s;
    s++, n--;
  }
  for (;;) {
    if (n == 0) {
      if (quote)
	return 0;
      break;
    }
    if (quote) {
      if (*s == quote) {
	s++, n--;
	break;
      }
    }
    else if (isS(*s))
      break;
    value += *s;
    s++, n--;
  }
  // FIXME resolve numeric character references
  return 1;
}

Boolean DssslApp::initSpecParser()
{
  if (!dssslSpecOption_ && !getDssslSpecFromGrove() && dssslSpecSysid_.size() == 0) {
    message(DssslAppMessages::noSpec);
    return 0;
  }
  SgmlParser::Params params;
  params.sysid = dssslSpecSysid_;
  params.entityManager = entityManager().pointer();
  params.options = &options_;
  specParser_.init(params);
  specParser_.allLinkTypesActivated();
  return 1;
}

void DssslApp::processGrove()
{
  if (!initSpecParser())
    return;
  const FOTBuilder::Extension *extensions = 0;
  Owner<FOTBuilder> fotb(makeFOTBuilder(extensions));
  if (!fotb)
    return;
  StyleEngine se(*this, *this, unitsPerInch_, debugMode_, dsssl2_, extensions);
  for (size_t i = 0; i < defineVars_.size(); i++)
    se.defineVariable(defineVars_[i]);
  se.parseSpec(specParser_, systemCharset(), dssslSpecId_, *this);
  se.process(rootNode_, *fotb);
}

bool DssslApp::load(const StringC &sysid, const Vector<StringC> &active,
		    const NodePtr &parent, NodePtr &rootNode, const Vector<StringC> &architecture)
{
  SgmlParser::Params params;
  params.sysid = sysid;
  const NodePtr *ndp = groveTable_.lookup(params.sysid);
  if (ndp) {
    rootNode = *ndp;
    return 1;
  }
  ErrorCountEventHandler *eceh;
  const SdNode *sdNode;
  NodePtr parentRoot;
  if (parent
      && parent->getGroveRoot(parentRoot) == accessOK
      && (sdNode = SdNode::convert(parentRoot)) != 0
      && sdNode->getSd(params.sd, params.prologSyntax, params.instanceSyntax) == accessOK) {
    params.entityType = SgmlParser::Params::subdoc;
    eceh = GroveBuilder::make(groveTable_.count() + 1, this, this, 0,
			      params.sd, params.prologSyntax, params.instanceSyntax,
			      rootNode);
  }
  else
    eceh = GroveBuilder::make(groveTable_.count() + 1, this, this, 0, rootNode);
  Owner<EventHandler> eh(eceh);
  groveTable_.insert(params.sysid, rootNode);
  params.entityManager = entityManager().pointer();
  params.options = &options_;
  SgmlParser parser;
  parser.init(params);

  for (size_t i = 0; i < active.size(); i++)
    parser.activateLinkType(active[i]);
  parser.allLinkTypesActivated();

  if (architecture.size() > 0) {
    SelectOneArcDirector director(architecture, *eh);
    ArcEngine::parseAll(parser, director, director, eceh->cancelPtr());
  }
  else
    parser.parseAll(*eh, eceh->cancelPtr());
  return 1;
}

bool DssslApp::readEntity(const StringC &sysid, StringC &contents) 
{
  Owner<InputSource> in(entityManager()->open(sysid,
					      systemCharset(),
					      InputSourceOrigin::make(),
					      0,
					      *this));
  if (!in)
    return 0;
  for (;;) {
    Xchar c = in->get(*this);
    if (c == InputSource::eE)
      break;
    in->extendToBufferEnd();
    contents.append(in->currentTokenStart(), in->currentTokenLength());
  }
  return !in->accessError();
}

#ifdef DSSSL_NAMESPACE
}
#endif
