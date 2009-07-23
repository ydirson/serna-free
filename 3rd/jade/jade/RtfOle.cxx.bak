// Copyright (c) 1997 James Clark
// See the file copying.txt for copying permission.

#include "config.h"
#define STRICT
#include <windows.h>
#include "RtfOle.h"
#include "StringC.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class OleUninitializer {
public:
  OleUninitializer() : enabled_(0) { }
  ~OleUninitializer() {
    if (enabled_)
      ::OleUninitialize();
  }
  void enable() { enabled_ = 1; }
  bool enabled() const { return enabled_; }
private:
  bool enabled_;
};

static OleUninitializer oleUninit;

struct ComException {
  ComException(HRESULT hRes, const char *fn, const char *ifn = 0)
    : hResult(hRes), functionName(fn), interfaceName(ifn) { }
  HRESULT hResult;
  const char *functionName;
  const char *interfaceName;
};

inline
void checkCom(HRESULT hRes, const char *functionName, const char *ifn = 0)
{
  if (FAILED(hRes))
    throw ComException(hRes, functionName, ifn);
}

#define COMFUNC(func, args) checkCom(func args, #func)
#define COMMETH(obj, iface, meth, args) checkCom((obj)->meth args, #meth, #iface)

static const char hexdigits[] = "0123456789ABCDEF";

class MyOleStream : public OLESTREAM {
public:
  MyOleStream(OutputByteStream &os): os_(&os), col_(0) {
    static OLESTREAMVTBL vtbl = { get, put };
    lpstbl = &vtbl;
  }
  unsigned long write(const char *p, unsigned long n) {
    for (size_t i = 0; i < n; i++) {
      if (col_++ % 64 == 0)
	 *os_ << "\n";
      *os_ << hexdigits[(p[i] >> 4) & 0xf] << hexdigits[p[i] & 0xf];
    }
    return n;
  }
private:
  static DWORD CALLBACK get(LPOLESTREAM, void *, DWORD)
  {
    return 0;
  }
  static DWORD CALLBACK put(LPOLESTREAM stm, const void *p, DWORD n)
  {
    return ((MyOleStream *)stm)->write((const char *)p, n);
  }
  unsigned col_;
  OutputByteStream *os_;
};

template<class T>
class ComPtr {
public:
  ComPtr() : p_(0) { }
  ComPtr(const ComPtr<T> &cp) : p_(cp) { if (p_) p_->AddRef(); }
  ~ComPtr() { if (p_) p_->Release(); }
  void operator=(const ComPtr<T> &cp) {
    if (cp.p_)
      cp.p_->AddRef();
    if (p_)
      p_->Release();
    p_ = cp.p_;
  }
  operator T *() const { return p_; }
  T *operator->() const { return p_; }
  // Legal only when p_ is 0.
  T **operator&() { return &p_; }
private:
  T *p_;
};

template<class T>
class GlobalPtr {
public:
  GlobalPtr(HGLOBAL h) : h_(h), p_((T *)GlobalLock(h)) { }
  ~GlobalPtr() { if (p_) GlobalUnlock(h_); }
  operator T *() const { return p_; }
  T *operator->() const { return p_; }
private:
  GlobalPtr(const GlobalPtr<T> &);
  void operator=(const GlobalPtr<T> &);
  T *p_;
  HGLOBAL h_;
};

class StorageMedium : public STGMEDIUM {
public:
  StorageMedium() { pUnkForRelease = 0; tymed = TYMED_NULL; }
  ~StorageMedium() { ReleaseStgMedium(this); }
private:
  StorageMedium(const StorageMedium &);
  void operator=(const StorageMedium &);
};

static
const wchar_t *makeAbsolute(const wchar_t *filename, StringC &buf);

int outputObject(const wchar_t *filename,
		 const wchar_t *clsidString,
		 OutputByteStream &os)
{
  try {
    if (!oleUninit.enabled()) {
      COMFUNC(OleInitialize, (NULL));
      oleUninit.enable();
    }
    StringC buf;
    filename = makeAbsolute(filename, buf);

    FORMATETC fmt;
    fmt.cfFormat = CF_METAFILEPICT;
    fmt.ptd = 0;
    fmt.dwAspect = DVASPECT_CONTENT;
    fmt.lindex = -1;
    fmt.tymed = TYMED_MFPICT;
    ComPtr<IStorage> pStorage;
#if 0
    ComPtr<ILockBytes> pBytes;
    COMFUNC(CreateILockBytesOnHGlobal, (0, 1, &pBytes));
    COMFUNC(StgCreateDocfileOnILockBytes,(pBytes, STGM_CREATE|STGM_TRANSACTED|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, &pStorage));
#else
    COMFUNC(StgCreateDocfile,(NULL,STGM_DELETEONRELEASE|STGM_CREATE|STGM_TRANSACTED|STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, &pStorage));
#endif
    ComPtr<IPersistStorage> pPersist;
    CLSID clsid;
    if (clsidString && *clsidString)
      COMFUNC(CLSIDFromString, ((wchar_t *)clsidString, &clsid));
    else
      COMFUNC(GetClassFile, (filename, &clsid));
    COMFUNC(CoCreateInstance,
            (clsid,
             NULL,
	     CLSCTX_SERVER,
	     IID_IPersistStorage,
	     (void **)&pPersist));
    COMMETH(pPersist, IPersistStorage, InitNew, (pStorage));
    ComPtr<IPersistFile> pPersistF;
    COMMETH(pPersist, IUnknown, QueryInterface, (IID_IPersistFile, (void **)&pPersistF));
    COMMETH(pPersistF, IPersistFile, Load, (filename, STGM_READ|STGM_SHARE_DENY_WRITE));
    COMFUNC(OleSave, (pPersist, pStorage, 1));
    COMMETH(pPersist, IPersistStorage, SaveCompleted, (pStorage));
#if 0
    COMFUNC(OleCreateLinkToFile,
      (filename,
      IID_IPersistStorage,
      OLERENDER_FORMAT,
      &fmt,
      NULL, // pClientSite
      pStorage,
      (void **)&pPersist));
    COMFUNC(OleSave, (pPersist, pStorage, 0));
#endif
    os << "{\\object\\objemb";
    os << "{\\*\\objdata";
    MyOleStream stm(os);
    COMFUNC(OleConvertIStorageToOLESTREAM, (pStorage, &stm));
    os << "}";

    ComPtr<IDataObject> pData;
    COMMETH(pPersist, IUnknown, QueryInterface, (IID_IDataObject, (void **)&pData));
    StorageMedium med;
    COMMETH(pData, IDataObject, GetData, (&fmt, &med));
    if (med.tymed != TYMED_MFPICT) {
      return 0;
    }
    GlobalPtr<METAFILEPICT> pMeta(med.hMetaFilePict);
    os << "{\\result";
    os << "{\\pict\\wmetafile" << pMeta->mm
      << "\\picw" << pMeta->xExt
      << "\\pich" << pMeta->yExt
      ;
    UINT n = GetMetaFileBitsEx(pMeta->hMF, 0, 0);
    if (n == 0) {
      return 0;
    }
    char *s = new char[n];
    n = GetMetaFileBitsEx(pMeta->hMF, n, s);
    if (n == 0) {
      return 0;
    }
    for (size_t i = 0; i < n; i++) {
      if (i % 64 == 0)
	os << "\n";
      os << hexdigits[(s[i] >> 4) & 0xf] << hexdigits[s[i] & 0xf];
    }
    delete [] s;
    os << "}"; // pict
    os << "}"; // result
    os << "}"; // object
  }    
  catch (const ComException &) {
    return 0;
  }
  return 1;
}

static
void multiByteToWideChar(const char *mb, StringC &w)
{
  int n = ::MultiByteToWideChar(CP_ACP, 0, mb, -1, 0, 0);
  w.resize(n);
  ::MultiByteToWideChar(CP_ACP, 0, mb, -1, &w[0], n);
}

static
void wideCharToMultiByte(const wchar_t *w, String<char> &mb)
{
  int n = ::WideCharToMultiByte(CP_ACP, 0, w, -1, 0, 0, 0, 0);
  mb.resize(n);
  ::WideCharToMultiByte(CP_ACP, 0, w, -1, &mb[0], n, 0, 0);
}

static
const wchar_t *makeAbsolute(const wchar_t *filename, StringC &fbuf)
{
  {
    fbuf.resize(128);
    Char *tem;
    int n = GetFullPathNameW(filename, fbuf.size(), &fbuf[0], &tem);
    if (n > fbuf.size()) {
      fbuf.resize(n);
      n = GetFullPathNameW(filename, fbuf.size(), &fbuf[0], &tem);
    }
    if (n) {
      fbuf.resize(n + 1);
      return fbuf.data();
    }
  }
  {
    String<char> mb;
    wideCharToMultiByte(filename, mb);
    mb += 0;
    String<char> buf;
    buf.resize(128);
    char *tem;
    int n = GetFullPathNameA(mb.data(), buf.size(), &buf[0], &tem);
    if (n > buf.size()) {
      buf.resize(n);
      n = GetFullPathNameA(mb.data(), buf.size(), &buf[0], &tem);
    }
    if (n) {
      buf.resize(n + 1);
      multiByteToWideChar(buf.data(), fbuf);
      fbuf += 0;
      return fbuf.data();
    }
  }
  return filename;
}


#ifdef DSSSL_NAMESPACE
}
#endif
