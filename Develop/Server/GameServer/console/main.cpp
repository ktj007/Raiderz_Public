#include "stdafx.h"
#include "resource.h"
#include "GMemory.h"
#include "GCommandTable.h"
#include "GConst.h"
#include "GConfig_Arg.h"
#include "GDef.h"
#include "GConfig.h"
#include "GConsole.h"
#include "SMinetInitScope.h"
#include "SProfileScope.h"
#include "SAppHelper.h"
#include "GCrashReporter.h"
#include "SLogger.h"
#include "tbb/task_scheduler_init.h"
#include "SServiceAdapter.h"


#define APPLICATION_WIDTH				855
#define APPLICATION_HEIGHT				720
#define PROG_RET_SUCCESS			0
#define PROG_RET_FAILED				1

void InitCrashDump();
void SetDsnFactory();

void SetDsnFactory();
void PreApp()
{
	Randomize();
	SAppHelper::SetupCurrentDirectory();	// Current Directory를 맞춤
	GConfig::Init_INI();
	SLogger::Init(MLogLevel(GConfig::m_nLogLevel), GConfig::m_strLogPath);
	InitCrashDump();
}

int wmain(int argc, WCHAR* argv[])
{
	// File system 초기화
	MCreateFileSystem( MFILEACCESS_GENERIC);

	// tbb 초기화
	tbb::task_scheduler_init init;

	// scope
	GMinetInitScope MinetInitScope;
	GScopeMempoolReleaser MemPoolReleaser;

	PreApp();

	wstring strProfileFileName = GConfig::m_strCommandProfiler_LogFolder;
	strProfileFileName += L"profile_";
	strProfileFileName += SAppHelper::MakeCurrentDateTimeStringForFileName();
	strProfileFileName += L".txt";

	// 서버 실행 ----------------------------------
	auto_ptr<GConsole> pConsolApp(new GConsole(true));
	SServiceAdapter service(pConsolApp.get(), APPTYPE_CONSOLE, GConfig::m_strServerName);

	try 
	{
		if (!service.Run())
		{
			return PROG_RET_FAILED;
		}
	}
	catch(std::bad_alloc e)
	{
		mlog3("bad alloc: %s\n", e.what());
	}

	return PROG_RET_SUCCESS;
}



void InitCrashDump()
{
	if (!GConfig::m_bDumpEnable)
		return;
	
	bool bFullMemoryDump = GConfig::m_bFullMemoryCrashDump;

	SAppHelper::InitCrashDump(GConfig::m_strServerName.c_str(), bFullMemoryDump, GCrashReporter::CrashDumpHandler, GConfig::m_strDumpDestServerIP.c_str(), GConfig::m_nDumpDestServerPort);
}

