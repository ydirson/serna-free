// groveoa.cxx : Implementation of DLL Exports.

// You will need VC 4.2 and the full 4.2b patch (http://msvc/vc42b/vc42b.htm) in 
// order to build this project.  This patch includes the final NT 4.0 SDK.
// You will also need MIDL 3.00.75, which is included with ATL 2.0.

// Note: Proxy/Stub Information
//		To merge the proxy/stub code into the object DLL, add the file 
//		dlldatax.c to the project.  Make sure precompiled headers 
//		are turned off for this file, and add _MERGE_PROXYSTUB to the 
//		defines for the project.  
//
//		If you are not running MIDL with /Oicf (the default), you will need
//		to remove the following defines from the top of dlldatax.c.
//		#define _WIN32_WINNT 0x0400
//		#define USE_STUBLESS_PROXY
//
//		Modify the custom build rule for groveoa.idl by adding the following 
//		files to the Outputs.
//			groveoa_p.c
//			dlldata.c
//		To build a separate proxy/stub DLL, 
//		run nmake -f groveoaps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include "groveoa.h"

#include "groveoa_i.c"
#include "CGroveBuilder.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_GroveBuilder, CGroveBuilder)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	lpReserved;
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		_Module.Init(ObjectMap, hInstance);
		DisableThreadLibraryCalls(hInstance);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
		_Module.Term();
	return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
	return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	_Module.UnregisterServer();
#if _WIN32_WINNT >= 0x0400
	UnRegisterTypeLib(LIBID_GROVE, 1, 0, LOCALE_USER_DEFAULT, SYS_WIN32);
#endif
	return S_OK;
}


