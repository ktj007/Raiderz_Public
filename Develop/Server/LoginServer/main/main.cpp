#include "stdafx.h"
#include "resource.h"
#include <shlwapi.h>
#include "GMemory.h"
#include "GCommandTable.h"
#include "GApplication.h"
#include "GConfig.h"

#define GAMESERVER_CLASS_NAME			"LoginServer"
#define GAMESERVER_APPLICATION_NAME		"LoginServer"
#define APPLICATION_WIDTH				475
#define APPLICATION_HEIGHT				720
#define UNITTEST_RET_SUCCESS			0
#define UNITTEST_RET_FAILED				1

GApplication* g_pGameServerApp = NULL;

void InitLog();
void InitCrashDump();
void InitServerINI();
void InitServerArg(LPSTR cmdline);
void SetupCurrentDirectory();

class GMinetInitScope
{
public:
	GMinetInitScope() 
	{
		minet::Init(SH_COMMAND_VERSION);
		GAddCommandTable();
	}
	~GMinetInitScope() 
	{
		minet::Fini();
	}
};

class GProfileScope
{
public:
	GProfileScope() 
	{
	}
	~GProfileScope() 
	{
		MSaveProfile(FILENAME_PROFILE);
	}
};

int PASCAL WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
	GMinetInitScope MinetInitScope;
	GScopeMempoolReleaser MemPoolReleaser;
	GProfileScope	profileScope;

	Randomize();
	SetupCurrentDirectory();	// Current Directory를 맞춤
	InitLog();	
	InitCrashDump();
	GConfig::Init_INI();

	// 서버 실행 ----------------------------------
	g_pGameServerApp = new GApplication();
	if (g_pGameServerApp->Create(0, 
		0, 
		APPLICATION_WIDTH, 
		APPLICATION_HEIGHT, 
		GAMESERVER_CLASS_NAME, 
		GAMESERVER_APPLICATION_NAME,
		IDR_SYSTEM_MENU,
		cmdline))
	{
		int ret = g_pGameServerApp->Run();
		delete g_pGameServerApp;
		return ret;
	}

	delete g_pGameServerApp;

	return UNITTEST_RET_SUCCESS;
}

///////////////////////////////////////////////////////////////////


void InitLog()
{
	MInitLog(MLOG_LEVEL_DEBUG, MLOG_DEBUG | MLOG_FILE, NULL, FILENAME_LOG);

	MLogLevel nLogLevel = MLOG_LEVEL_INFORMATION;

#ifndef _PUBLISH
	nLogLevel = MLOG_LEVEL_DEBUG;
#	ifndef _DEBUG
		nLogLevel = MLOG_LEVEL_INFORMATION;
#	endif

	unsigned long int nOutputFlags = MLOG_DEBUG | MLOG_PROG | MLOG_FILE;

	// Prog Output은 나중에 세팅한다.
	MInitLog(nLogLevel, nOutputFlags, NULL, FILENAME_LOG);
#endif
}

void InitCrashDump()
{
#ifndef _DEBUG
	MCrashDump::Init();

	// 나중에 필요할 때에만 풀덤프를 뜨자. 용량이 너무 크다. -_- - birdkr(2007-06-14)
	//MCrashDump::Init(NULL, NULL, true);
#endif
}


// Current Directory를 맞춤
void SetupCurrentDirectory()
{
	char szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);
}
