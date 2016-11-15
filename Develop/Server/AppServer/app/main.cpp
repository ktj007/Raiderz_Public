#include "stdafx.h"
#include "resource.h"
#include <shlwapi.h>
#include "PMemory.h"
#include "PCommandTable.h"
#include "PApplication.h"
#include "PConfig.h"
#include "MProfiler.h"
#include "SMinetInitScope.h"
#include "SProfileScope.h"
#include "SAppHelper.h"
#include "SLogger.h"
#include "PDef.h"
#include "MFileSystem.h"

#define PROG_RET_SUCCESS			0
#define PROG_RET_FAILED				1

PApplication* g_pGameServerApp = NULL;

void InitCrashDump()
{
	SAppHelper::InitCrashDump(APP_NAME, false, NULL, PConfig::m_strDumpDestServerIP, PConfig::m_nDumpDestServerPort);
}

void PreApp()
{
	Randomize();
	SAppHelper::SetupCurrentDirectory();	// Current Directory를 맞춤
	PConfig::Init_INI();
	SLogger::Init(MLogLevel(PConfig::m_nLogLevel), PConfig::m_strLogPath);
	InitCrashDump();
}

int PASCAL wWinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPWSTR cmdline, int cmdshow)
{
	// File system 초기화
	MCreateFileSystem( MFILEACCESS_GENERIC);

	PMinetInitScope MinetInitScope;
	PScopeMempoolReleaser MemPoolReleaser;
	SProfileScope profileScope(FILENAME_PROFILE);

	PreApp();

	// 서버 실행 ----------------------------------
	g_pGameServerApp = new PApplication(true);
	if (g_pGameServerApp->Create(0, 
		0, 
		APPLICATION_WIDTH, 
		APPLICATION_HEIGHT, 
		APP_CLASS_NAME, 
		APP_NAME,
		IDR_SYSTEM_MENU,
		IDR_MAINFRAME))
	{
		int ret = g_pGameServerApp->Run();
		delete g_pGameServerApp;
		return ret;
	}

	delete g_pGameServerApp;

	return PROG_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////


