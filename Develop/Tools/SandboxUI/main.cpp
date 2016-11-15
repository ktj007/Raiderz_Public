
#include "stdafx.h"
#include "shlwapi.h"
#include "main.h"
#include "RConfiguration.h"
#include "ZApplication.h"



/************************************************************************/
/* Load libraries                                                       */
/************************************************************************/
#ifdef _DEBUG
	#pragma comment( lib, "../../../sdk/lua51/lib/vc8/lua51d.lib")
	#pragma comment( lib, "../../../sdk/tinyxml/lib/tinyxmld_STL.lib")
#else
	#pragma comment( lib, "../../../sdk/lua51/lib/vc8/lua51.lib")
	#pragma comment( lib, "../../../sdk/tinyxml/lib/tinyxml_STL.lib")
#endif
#pragma comment( lib, "../../../sdk/zlib/lib/zlib.lib")
#pragma comment( lib, "winmm.lib")



/************************************************************************/
/* Variables                                                            */
/************************************************************************/
ZApplication	g_App;



/************************************************************************/
/* Functions                                                            */
/************************************************************************/

/*	WinMain
	Main function
*/
int PASCAL WinMain( HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
	// Set current directory
	char szModuleFileName[ _MAX_DIR] = {0,};
	GetModuleFileName( NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec( szModuleFileName);
	SetCurrentDirectory( szModuleFileName);


	// Set environment
	REngine::GetConfig().m_nWidth = 1024;
	REngine::GetConfig().m_nHeight = 768;


	// Run application
	return g_App.Run();
}
