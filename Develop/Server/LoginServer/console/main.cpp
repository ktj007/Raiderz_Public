#include "stdafx.h"
#include <shlwapi.h>
#include "LMemory.h"
#include "LCommandTable.h"
#include "LConsole.h"
#include "LConfig.h"
#include "MProfiler.h"
#include "SMinetInitScope.h"
#include "SProfileScope.h"
#include "SAppHelper.h"
#include "SLogger.h"
#include "LDef.h"
#include "SServiceAdapter.h"
#include <tchar.h>
#include <atlconv.h>
#include <atlexcept.h>
#include "MFileSystem.h"

#define PROG_RET_SUCCESS			0
#define PROG_RET_FAILED				1

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

int wmain(int argc, wchar_t* argv[])
{
	// File system 초기화
	MCreateFileSystem( MFILEACCESS_GENERIC);

	LMinetInitScope MinetInitScope;
	LScopeMempoolReleaser MemPoolReleaser;
	SProfileScope profileScope(FILENAME_PROFILE);

	PreApp();

	
	auto_ptr<LConsole> pConsolApp(new LConsole());
	USES_CONVERSION_EX;
	SServiceAdapter service(pConsolApp.get(), APPTYPE_CONSOLE, W2CT_EX(LConfig::m_strServerName.c_str(), 0));

	if (!service.Run())
	{
		return PROG_RET_FAILED;
	}


	return PROG_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////
