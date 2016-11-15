#include "stdafx.h"
#include "resource.h"
#include <shlwapi.h>
#include "LMemory.h"
#include "LCommandTable.h"
#include "LApplication.h"
#include "LConfig.h"
#include "MProfiler.h"
#include "SMinetInitScope.h"
#include "SProfileScope.h"
#include "SAppHelper.h"
#include "LDef.h"

#define PROG_RET_SUCCESS			0
#define PROG_RET_FAILED				1

LApplication* g_pGameServerApp = NULL;

void InitCrashDump()
{
	SAppHelper::InitCrashDump(APP_NAME, false);
}

void PreApp()
{
	Randomize();
	SAppHelper::SetupCurrentDirectory();	// Current Directory를 맞춤
	SAppHelper::InitLog(FILENAME_LOG);	
	LConfig::Init_INI();
	InitCrashDump();
}

int PASCAL WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
	LMinetInitScope MinetInitScope;
	LScopeMempoolReleaser MemPoolReleaser;
	SProfileScope profileScope(FILENAME_PROFILE);

	PreApp();

	// 서버 실행 ----------------------------------
	g_pGameServerApp = new LApplication(true);
	if (g_pGameServerApp->Create(0, 
		0, 
		APPLICATION_WIDTH, 
		APPLICATION_HEIGHT, 
		APP_CLASS_NAME, 
		APP_NAME,
		IDR_SYSTEM_MENU,
		IDR_MAINFRAME,
		cmdline))
	{
		int ret = g_pGameServerApp->Run();
		delete g_pGameServerApp;
		return ret;
	}

	delete g_pGameServerApp;

	return PROG_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////


