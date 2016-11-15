// dll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 이게 없으면 Dllmain 이 겹친다는 링크 에러가 나는데 
// How to use DllMain in a MFC dll project (ZZ) 라는 글에서 찾았다
// http://www.vlight.net/bbs/viewthread.php?tid=308
// http://www.codeguru.com/Cpp/Cpp/cpp_mfc/article.php/c4033/
// The following symbol used to force inclusion of this module for _USRDLL
#ifdef _USRDLL
#ifdef _X86_
extern "C" { int _afxForceUSRDLL; }
#else
extern "C" { int __afxForceUSRDLL; }
#endif
#endif

static AFX_EXTENSION_MODULE DllDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("DLL.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(DllDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(DllDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("DLL.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(DllDLL);
	}
	return 1;   // ok
}
