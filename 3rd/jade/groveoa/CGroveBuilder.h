// CGroveBuilder.h : Declaration of the CGroveBuilder

#ifndef __GROVEBUILDER_H_
#define __GROVEBUILDER_H_

#include "resource.h"       // main symbols
#include "config.h"
#include "WinApp.h"
#include "Message.h"
#include "MessageFormatter.h"

/////////////////////////////////////////////////////////////////////////////
// CGroveBuilder
class ATL_NO_VTABLE CGroveBuilder : 
	public CComObjectRoot,
	public CComCoClass<CGroveBuilder, &CLSID_GroveBuilder>,
	public IDispatchImpl<_GroveBuilder, &IID__GroveBuilder, &LIBID_GROVE>
{
public:
	CGroveBuilder() : validateOnly_(0), errorLpdNotation_(0) { }

DECLARE_REGISTRY_RESOURCEID(IDR_GROVEBUILDER)

BEGIN_COM_MAP(CGroveBuilder)
	COM_INTERFACE_ENTRY(_GroveBuilder)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()
// IGroveBuilder
public:
	STDMETHOD(parse)(BSTR sysid, SgmlDocumentNode **root);
	STDMETHOD(get_ExtraCatalogs)(BSTR *);
	STDMETHOD(put_ExtraCatalogs)(BSTR);
	STDMETHOD(get_DefaultCatalogs)(BSTR *);
	STDMETHOD(put_DefaultCatalogs)(BSTR);
	STDMETHOD(get_ExtraDirectories)(BSTR *);
	STDMETHOD(put_ExtraDirectories)(BSTR);
	STDMETHOD(get_DefaultDirectories)(BSTR *);
	STDMETHOD(put_DefaultDirectories)(BSTR);
	STDMETHOD(get_Encoding)(BSTR *);
	STDMETHOD(put_Encoding)(BSTR);
	STDMETHOD(get_DefaultEncoding)(BSTR *);
	STDMETHOD(put_DefaultEncoding)(BSTR);
	STDMETHOD(get_Includes)(BSTR *);
	STDMETHOD(put_Includes)(BSTR);
	STDMETHOD(get_Warning)(WarningType, VARIANT_BOOL *);
	STDMETHOD(put_Warning)(WarningType, VARIANT_BOOL);
	STDMETHOD(get_Error)(ErrorType, VARIANT_BOOL *);
	STDMETHOD(put_Error)(ErrorType, VARIANT_BOOL);
	STDMETHOD(get_ValidateOnly)(VARIANT_BOOL *);
	STDMETHOD(put_ValidateOnly)(VARIANT_BOOL);
private:
        SP_NAMESPACE::PackedBoolean *lookupWarningType(WarningType type);
	SP_NAMESPACE::PackedBoolean *lookupErrorType(ErrorType type);
	SP_NAMESPACE::WinApp app_;
	VARIANT_BOOL validateOnly_;
	// This is just for binary compatibility.
	SP_NAMESPACE::PackedBoolean errorLpdNotation_;
};

#endif //__GROVEBUILDER_H_
