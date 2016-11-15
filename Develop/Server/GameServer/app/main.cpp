#include "stdafx.h"
#include "resource.h"
#include "GMemory.h"
#include "GCommandTable.h"
#include "GConst.h"
#include "GConfig_Arg.h"
#include "GDef.h"
#include "GConfig.h"
#include "GApplication.h"
#include "SMinetInitScope.h"
#include "SProfileScope.h"
#include "SAppHelper.h"
#include "SLogger.h"
#include "GCrashReporter.h"
#include "tbb/task_scheduler_init.h"

#define APPLICATION_WIDTH				855
#define APPLICATION_HEIGHT				720
#define PROG_RET_SUCCESS			0
#define PROG_RET_FAILED				1

GApplication* g_pGameServerApp = NULL;

void InitCrashDump();

void PreApp()
{
	Randomize();
	SAppHelper::SetupCurrentDirectory();	// Current Directory를 맞춤
	GConfig::Init_INI();
	SLogger::Init(MLogLevel(GConfig::m_nLogLevel), GConfig::m_strLogPath);
	InitCrashDump();
}

int PASCAL wWinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPWSTR cmdline, int cmdshow)
{
	// File system 초기화
	MCreateFileSystem( MFILEACCESS_GENERIC);

	// tbb 초기화
	tbb::task_scheduler_init init;

	// scope
	GMinetInitScope MinetInitScope;
	GScopeMempoolReleaser MemPoolReleaser;
	SProfileScope profileScope(FILENAME_PROFILE);

	PreApp();

	// 서버 실행 ----------------------------------
	g_pGameServerApp = new GApplication(true);

	if (g_pGameServerApp->Create(0, 
		0, 
		APPLICATION_WIDTH, 
		APPLICATION_HEIGHT, 
		GAMESERVER_CLASS_NAME,
		GConfig::m_strServerName.c_str(),
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



void InitCrashDump()
{
	if (!GConfig::m_bDumpEnable)
		return;
	
	bool bFullMemoryDump = GConfig::m_bFullMemoryCrashDump;

	SAppHelper::InitCrashDump(GConfig::m_strServerName.c_str(), bFullMemoryDump, GCrashReporter::CrashDumpHandler, GConfig::m_strDumpDestServerIP.c_str(), GConfig::m_nDumpDestServerPort);
}


