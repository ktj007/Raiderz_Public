#include "stdafx.h"
#include <shlwapi.h>
#include "ZMemory.h"
#include "ZCommandTable.h"
#include "ZConsole.h"
#include "ZConfig.h"
#include "MProfiler.h"
#include "SMinetInitScope.h"
#include "SProfileScope.h"
#include "SAppHelper.h"
#include "SLogger.h"
#include "ZDef.h"
#include "SServiceAdapter.h"
#include "MFileSystem.h"


#define PROG_RET_SUCCESS			0
#define PROG_RET_FAILED				1


void InitCrashDump()
{
	SAppHelper::InitCrashDump(APP_NAME, false, NULL, ZConfig::m_strDumpDestServerIP, ZConfig::m_nDumpDestServerPort);
}

void PreApp()
{
	Randomize();
	SAppHelper::SetupCurrentDirectory();	// Current Directory를 맞춤
	ZConfig::Init_INI();
	SLogger::Init(MLogLevel(ZConfig::m_nLogLevel), ZConfig::m_strLogPath);
	InitCrashDump();
}

int wmain(int argc, wchar_t* argv[])
{
	// File system 초기화
	MCreateFileSystem( MFILEACCESS_GENERIC);

	ZMinetInitScope MinetInitScope;
	ZScopeMempoolReleaser MemPoolReleaser;
	SProfileScope profileScope(FILENAME_PROFILE);

	PreApp();

	
	// 서버 실행 ----------------------------------
	auto_ptr<ZConsole> pConsolApp(new ZConsole());
	SServiceAdapter service(pConsolApp.get(), APPTYPE_CONSOLE, ZConfig::m_strServerName);

	if (!service.Run())
	{
		return PROG_RET_FAILED;
	}

	
	return PROG_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////


