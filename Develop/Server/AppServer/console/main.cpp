#include "stdafx.h"
#include <shlwapi.h>
#include "PMemory.h"
#include "PCommandTable.h"
#include "PConsole.h"
#include "PConfig.h"
#include "MProfiler.h"
#include "SMinetInitScope.h"
#include "SProfileScope.h"
#include "SAppHelper.h"
#include "SLogger.h"
#include "PDef.h"
#include "SServiceAdapter.h"
#include "SIOCPServiceAdapter.h"
#include "MFileSystem.h"


#define PROG_RET_SUCCESS			0
#define PROG_RET_FAILED				1

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

#include "MCommandLine.h"

int wmain(int argc, wchar_t* argv[])
{
	// File system 초기화
	MCreateFileSystem( MFILEACCESS_GENERIC);

	PMinetInitScope MinetInitScope;
	PScopeMempoolReleaser MemPoolReleaser;
	SProfileScope profileScope(FILENAME_PROFILE);

	PreApp();

	cml2::MCommandLine cl;
	cl.ParseFromCommandLine();

	string str = cl.GetCommandLineString();
	mlog(str.c_str());


	// 서버 실행 ----------------------------------
	auto_ptr<PConsole> pConsolApp(new PConsole());
	SServiceAdapter* service = NULL;
	if (true == SConfig::m_iocpAppService)
	{
		service = new SIOCPServiceAdapter(pConsolApp.get(), APPTYPE_CONSOLE, PConfig::m_strServerName);
	}
	else
	{
		service = new SServiceAdapter(pConsolApp.get(), APPTYPE_CONSOLE, PConfig::m_strServerName);		
	}
	

	if (!service->Run())
	{
		delete service;
		return PROG_RET_FAILED;
	}
	delete service;


	return PROG_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////


