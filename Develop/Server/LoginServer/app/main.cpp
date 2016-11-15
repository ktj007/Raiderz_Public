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
#include "SLogger.h"
#include "LDef.h"
#include "MFileSystem.h"
#include <tchar.h>
#include <atlconv.h>
#include <atlexcept.h>


#define PROG_RET_SUCCESS			0
#define PROG_RET_FAILED				1

LApplication* g_pGameServerApp = NULL;

void InitCrashDump()
{
	USES_CONVERSION_EX;
	SAppHelper::InitCrashDump(APP_NAME, false, NULL, W2CT_EX(LConfig::m_strDumpDestServerIP.c_str(), 0), LConfig::m_nDumpDestServerPort);
}

void PreApp()
{
	Randomize();
	SAppHelper::SetupCurrentDirectory();	// Current Directory를 맞춤
	LConfig::Init_INI();
	SLogger::Init(MLogLevel(LConfig::m_nLogLevel), LConfig::m_strLogPath);
	InitCrashDump();
}

int PASCAL wWinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPWSTR cmdline, int cmdshow)
{
	// File system 초기화
	MCreateFileSystem( MFILEACCESS_GENERIC);

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


