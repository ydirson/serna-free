// CGroveBuilder.cpp : Implementation of CGroveBuilder

#include "stdafx.h"
#include "groveoa.h"
#include "CGroveBuilder.h"
#include "GroveBuilder.h"
#include "ErrorCountEventHandler.h"
#include "GroveNode.h"
#include "MessageTable.h"
#include <new>
#include <signal.h>
#include <process.h>
#define STRICT
#include <windows.h>

using SP_NAMESPACE::StringC;

#ifdef SP_NO_STD_NAMESPACE
#define std /* as nothing */
#endif

#define TRY try {
#define CATCH  } catch (std::bad_alloc) { return E_OUTOFMEMORY; }

/* We need to keep a reference to the entity manager, because
StoragePos can use storage managers that are owned by the entity
manager. */

class SpParserThread :
	public ParserThread,
	public SP_NAMESPACE::Messenger,
	public SP_NAMESPACE::MessageFormatter {
public:
  SpParserThread() : cancel_(0) { }
  ~SpParserThread();
  void run();
  void dispatchMessage(const SP_NAMESPACE::Message &);
  SP_NAMESPACE::Boolean getMessageText(const SP_NAMESPACE::MessageFragment &, SP_NAMESPACE::StringC &);

  SP_NAMESPACE::Owner<SP_NAMESPACE::EventHandler> eh;
  SP_NAMESPACE::SgmlParser parser;
  SP_NAMESPACE::Ptr<SP_NAMESPACE::EntityManager> em;
  static unsigned __stdcall start(void *p);
private:
  sig_atomic_t cancel_;
  HANDLE thread_;
};

STDMETHODIMP CGroveBuilder::parse(BSTR sysid, SgmlDocumentNode **retval)
{
  TRY
  SpParserThread *parserThread = new SpParserThread;
  SP_NAMESPACE::Owner<ParserThread> tem(parserThread);
  GROVE_NAMESPACE::NodePtr root;
  parserThread->eh
    = SP_NAMESPACE::GroveBuilder::make(0, parserThread, parserThread, validateOnly_ != 0, root);
  StringC str(sysid, sysid ? ::SysStringLen(sysid) : 0);
  app_.initParser(str, parserThread->parser, parserThread->em);
  parserThread->run();
  return makeRootNode(root, tem, retval);
  CATCH
}

static
HRESULT makeBSTR(const StringC &str, BSTR *retval)
{
  if (str.size() == 0)
    *retval = 0;
  else {
    *retval = ::SysAllocStringLen(str.data(), str.size());
    if (!*retval)
      return E_OUTOFMEMORY;
  }
  return NOERROR;
}

void convertBSTR(BSTR str, StringC &result)
{
  if (str)
    result.assign(str, ::SysStringLen(str));
  else
    result.resize(0);
}

STDMETHODIMP CGroveBuilder::get_ExtraCatalogs(BSTR *retval)
{
  return makeBSTR(app_.extraCatalogs(), retval);
}

STDMETHODIMP CGroveBuilder::put_ExtraCatalogs(BSTR str)
{
  TRY
  StringC tem;
  convertBSTR(str, tem);
  app_.setExtraCatalogs(tem);
  return NOERROR;
  CATCH
}

STDMETHODIMP CGroveBuilder::get_DefaultCatalogs(BSTR *retval)
{
  return makeBSTR(app_.defaultCatalogs(), retval);
}

STDMETHODIMP CGroveBuilder::put_DefaultCatalogs(BSTR str)
{
  TRY
  StringC tem;
  convertBSTR(str, tem);
  app_.setDefaultCatalogs(tem);
  return NOERROR;
  CATCH
}

STDMETHODIMP CGroveBuilder::get_ExtraDirectories(BSTR *retval)
{
  return makeBSTR(app_.extraDirectories(), retval);
}

STDMETHODIMP CGroveBuilder::put_ExtraDirectories(BSTR str)
{
  TRY
  StringC tem;
  convertBSTR(str, tem);
  app_.setExtraDirectories(tem);
  return NOERROR;
  CATCH
}

STDMETHODIMP CGroveBuilder::get_DefaultDirectories(BSTR *retval)
{
  return makeBSTR(app_.defaultDirectories(), retval);
}

STDMETHODIMP CGroveBuilder::put_DefaultDirectories(BSTR str)
{
  TRY
  StringC tem;
  convertBSTR(str, tem);
  app_.setDefaultDirectories(tem);
  return NOERROR;
  CATCH
}

STDMETHODIMP CGroveBuilder::get_Encoding(BSTR *retval)
{
  return makeBSTR(app_.encoding(), retval);
}

STDMETHODIMP CGroveBuilder::put_Encoding(BSTR str)
{
  TRY
  StringC tem;
  convertBSTR(str, tem);
  app_.setEncoding(tem);
  // FIXME give an error for a illegal encoding
  return NOERROR;
  CATCH
}

STDMETHODIMP CGroveBuilder::get_DefaultEncoding(BSTR *retval)
{
  return makeBSTR(app_.defaultEncoding(), retval);
}

STDMETHODIMP CGroveBuilder::put_DefaultEncoding(BSTR str)
{
  TRY
  StringC tem;
  convertBSTR(str, tem);
  app_.setDefaultEncoding(tem);
  return NOERROR;
  CATCH
}

STDMETHODIMP CGroveBuilder::get_Includes(BSTR *retval)
{
  TRY
  SP_NAMESPACE::StringC tem;
  for (size_t i = 0; i < app_.options.includes.size(); i++) {
    if (i)
      tem += ',';
    tem += app_.options.includes[i];
  }
  return makeBSTR(tem, retval);
  CATCH
}

STDMETHODIMP CGroveBuilder::put_Includes(BSTR str)
{
  TRY
  SP_NAMESPACE::Vector<SP_NAMESPACE::StringC> &v = app_.options.includes;
  v.resize(0);
  if (str) {
    SP_NAMESPACE::Boolean started = 0;
    for (; *str; str++) {
      if (*str == ',')
	started = 0;
      else {
	if (!started) {
	  v.resize(v.size() + 1);
	  started = 1;
	}
	v.back() += *str;
      }
    }
  }
  return NOERROR;
  CATCH
}

STDMETHODIMP CGroveBuilder::get_Warning(WarningType type, VARIANT_BOOL *retval)
{
  *retval = 0;
  SP_NAMESPACE::PackedBoolean *p = lookupWarningType(type);
  if (!p)
    return E_INVALIDARG;
  *retval = *p ? -1 : 0;
  return NOERROR;
}

STDMETHODIMP CGroveBuilder::put_Warning(WarningType type, VARIANT_BOOL b)
{
  SP_NAMESPACE::PackedBoolean *p = lookupWarningType(type);
  if (!p)
    return E_INVALIDARG;
  *p = b ? 1 : 0;
  return NOERROR;
}

SP_NAMESPACE::PackedBoolean *CGroveBuilder::lookupWarningType(WarningType type)
{
  switch (type) {
#define ENTRY(w) case warning##w: return &app_.options.warn##w;
  ENTRY(SgmlDecl)
  ENTRY(DuplicateEntity)
  ENTRY(Should)
  ENTRY(UndefinedElement)
  ENTRY(DefaultEntityReference)
  ENTRY(MixedContent)
  ENTRY(EmptyTag)
  ENTRY(UnusedMap)
  ENTRY(UnusedParam)
  ENTRY(NotationSystemId)
#undef ENTRY
  case warningUnclosedTag:
    return &app_.options.noUnclosedTag;
  case warningNet:
    return &app_.options.noNet;
  }
  return 0;
}

STDMETHODIMP CGroveBuilder::get_Error(ErrorType type, VARIANT_BOOL *retval)
{
  *retval = 0;
  if (type == errorValid) {
    *retval = (app_.options.typeValid == 0 ? 0 : -1);
    return NOERROR;
  }
  SP_NAMESPACE::PackedBoolean *p = lookupErrorType(type);
  if (!p)
    return E_INVALIDARG;
  *retval = *p ? -1 : 0;
  return NOERROR;
}

STDMETHODIMP CGroveBuilder::put_Error(ErrorType type, VARIANT_BOOL b)
{
  if (type == errorValid) {
    app_.options.typeValid = (b ? SP_NAMESPACE::ParserOptions::sgmlDeclTypeValid : 0);
    return NOERROR;
  }
  SP_NAMESPACE::PackedBoolean *p = lookupErrorType(type);
  if (!p)
    return E_INVALIDARG;
  *p = b ? 1 : 0;
  return NOERROR;
}

SP_NAMESPACE::PackedBoolean *CGroveBuilder::lookupErrorType(ErrorType type)
{
  switch (type) {
#define ENTRY(e) case error##e: return &app_.options.error##e;
  ENTRY(Idref)
  ENTRY(Significant)
  ENTRY(Afdr)
#undef ENTRY
  case errorLpdNotation:
    return &errorLpdNotation_;
  }
  return 0;
}

STDMETHODIMP CGroveBuilder::get_ValidateOnly(VARIANT_BOOL *retval)
{
  *retval = validateOnly_;
  return NOERROR;
}

STDMETHODIMP CGroveBuilder::put_ValidateOnly(VARIANT_BOOL b)
{
  validateOnly_ = b;
  return NOERROR;
}

SpParserThread::~SpParserThread()
{
  cancel_ = 1;
  ::WaitForSingleObject(thread_, INFINITE);
  ::CloseHandle(thread_);
}

unsigned __stdcall SpParserThread::start(void *p)
{
  SpParserThread *arg = (SpParserThread *)p;
  try {
    arg->parser.parseAll(*arg->eh, &arg->cancel_);
  }
  catch (std::bad_alloc) {
    // FIXME how to report this?
  }
  arg->eh.clear();
  // Release the parser
  SP_NAMESPACE::SgmlParser tem;
  tem.swap(arg->parser);
  return 0;
}

void SpParserThread::run()
{
  unsigned id;
  thread_ = HANDLE(_beginthreadex(NULL, 0, start, this, 0, &id));
}

void SpParserThread::dispatchMessage(const SP_NAMESPACE::Message &)
{
}

SP_NAMESPACE::Boolean
SpParserThread::getMessageText(const SP_NAMESPACE::MessageFragment &frag,
			       StringC &text)
{
  SP_NAMESPACE::String<SP_NAMESPACE::SP_TCHAR> str;
  if (!SP_NAMESPACE::MessageTable::instance()->getText(frag, str))
    return 0;
  text.assign((const SP_NAMESPACE::Char *)str.data(), str.size());
  return 1;
}
