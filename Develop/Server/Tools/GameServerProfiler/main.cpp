#include "stdafx.h"
#include "GMemory.h"
#include "GCommandTable.h"
#include "GConst.h"
#include "GConfig_Arg.h"
#include "GDef.h"
#include "GConfig.h"
#include "SMinetInitScope.h"
#include "SProfileScope.h"
#include "SAppHelper.h"
#include "Profiler.h"
#include "MCommandLine.h"
#include "GCrashReporter.h"
#include "SLogger.h"

#define CL_SWITCH_CONFIG "config"

void InitCrashDump();

void PreApp()
{
	Randomize();
	SAppHelper::SetupCurrentDirectory();	// Current Directory를 맞춤
	MInitLog(MLOG_LEVEL_DEBUG, MLOG_DEBUG|MLOG_FILE|MLOG_CONSOLE, NULL, MLocale::ConvUTF16ToAnsi(FILENAME_LOG).c_str());
	GConfig::Init_INI();
	InitCrashDump();
}

void DisplayUsage(cml2::MCommandLine& cl)
{
	printf("Usage: %s -%s=<filename>.xml\n", cl.GetProgram().c_str(), CL_SWITCH_CONFIG);
}

int wmain(int argc, wchar_t* argv[])
{
	MCreateFileSystem( MFILEACCESS_GENERIC);

	wchar_t buff[512];
	GetCurrentDirectory(512, buff);
	wprintf(L"Current Directory; %s\n", buff);

	cml2::MCommandLine cl;
	cl.ParseFromCommandLine();
	if (!cl.HasSwitch(CL_SWITCH_CONFIG))
	{
		DisplayUsage(cl);
		return 1;
	}
	
	string strConfigFileName = cl.GetSwitchValue(CL_SWITCH_CONFIG);
	printf("Config filename: %s\n", strConfigFileName.c_str());

	// scope
	GMinetInitScope MinetInitScope;
	GScopeMempoolReleaser MemPoolReleaser;	

	PreApp();

	Profiler profiler;
	profiler.LoadConfig(strConfigFileName);
	if (!profiler.Init())
	{
		printf("설정 파일을 읽을 수 없습니다.\n");
		return -1;
	}

	profiler.Run();

	return profiler.IsFail();
}

///////////////////////////////////////////////////////////////////


void InitCrashDump()
{
	if (!GConfig::m_bDumpEnable)
		return;

	bool bFullMemoryDump = GConfig::m_bFullMemoryCrashDump;

	SAppHelper::InitCrashDump(GConfig::m_strServerName.c_str(), bFullMemoryDump, GCrashReporter::CrashDumpHandler);
}


