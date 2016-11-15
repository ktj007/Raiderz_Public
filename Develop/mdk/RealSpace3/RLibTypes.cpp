#include "stdafx.h"
#include "RToolApplication.h"
#include "RLibTypes.h"
#include "RDevice.h"
#include "RDeviceD3D.h"

using namespace rs3;

// DLL로 export할 때 필요한 것들
#ifdef _USRDLL

#ifdef _DEBUG
//#pragma comment (lib, "../../sdk/tinyXml/lib/tinyxmld_STL.lib")
//#pragma comment (lib, "../../sdk/zlib/lib/zlib.lib")
//#else
//#pragma comment (lib, "../../sdk/tinyXml/lib/tinyxml_STL.lib")
//#pragma comment (lib, "../../sdk/zlib/lib/zlib.lib")
#endif

static RToolApplication* g_pAPP = NULL;

RS_API rs3::RToolApplication* createToolApplication()
{
	rs3::RToolApplication* pApp = new rs3::RToolApplication();
	g_pAPP = pApp;

	pApp->Create();
	return pApp;
}

RS_API void destroyToolApplication()
{
	g_pAPP->Destroy();

	delete g_pAPP; g_pAPP = NULL;
	//rs3::RDevice* pDev = RGetDevice();
	//if (pDev)
	//{
	//	delete pDev;
	//	RSetDevice(NULL);
	//}
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			#ifdef _DEBUG
				_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
			#endif
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


#endif // #ifdef _USRDLL



