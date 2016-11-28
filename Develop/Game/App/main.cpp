#include "stdafx.h"
#include "CCommandTable.h"
#include "XCommandTable.h"
#include "MCrashDump.h"
#include "RDeviceD3D.h"
#include "MFileSystem.h"
#include "XGameApp.h"
#include "XGlobal.h"
#include "XMemPoolScopeReleaser.h"
#include "MMemoryLeak.h"
#include "XCrashReporter.h"
#include <ShellAPI.h>
#include "XNetwork.h"
#include "XNetClient.h"
#include "XReplay.h"
#include "XSystemInfoUtil.h"
#include "XAntiHack.h"
#include "XPath.h"
#include "XResourceLoader.h"
#include "MLocale.h"
#include "XAppErrorCollector.h"
#include "REngine.h"


//#define VLD_CHECK_LEAK	// stdafx.h 에 있으면 넣을때마나 거의 rebuild 하게 되어서 cpp 쪽으로 뺐어요
#ifdef _DEBUG
#ifdef VLD_CHECK_LEAK

#define VLD_MAX_DATA_DUMP		0xFF		// vld에서 데이터 덤프는 FF Byte만 해랑.

#include "../../../sdk/vld/vld.h"
#pragma comment(lib,"vldmt.lib")
#else
#endif
#endif // _DEBUG
// Leak 체크 Define ----------------------------------------------


#pragma comment(lib, "../../../sdk/zlib/lib/zlib.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")



/// 크래쉬 핸들러
void XCrashDumpHandler(const char* szFileName)
{
	if (XCONST::AUTO_REPORT_CRASHDUMP)
	{
		XCrashReporter::bAutoReport = true;
	}

	XCrashReporter::LogInfo();
	XCrashReporter::Report(szFileName, "game");
}


bool PreAppRun(LPWSTR cmdline)
{
	wchar_t szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);

	// 만약 Raiderz.exe인데 인자가 아무 것도 없으면 런처를 실행하고 종료한다.
	wstring strPureFileName = MLocale::ConvAnsiToUTF16(MGetRemovePathName(MLocale::ConvUTF16ToAnsi(szModuleFileName)).c_str());

	if (!_wcsicmp(strPureFileName.c_str(), L"raiderz.exe"))
	{
		if (wcslen(cmdline) <= 0) 
		{
			ShellExecute(0, NULL, FILENAME_LAUNCHER, L"", NULL, SW_NORMAL);
			return false;
		}
	}

	// Current Directory를 맞춤
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);

	return true;
}

void PostAppRun()
{
#ifdef M_CHECK_LEAK
	_ASSERT(MMemoryLeak::CheckMemory());
#endif
}

bool InitLogNConst()
{
	//// 게임에서 사용하는 상수값을 읽는다.
	XCONST::Init();

	MLogLevel nLogLevel = MLOG_LEVEL_DEBUG;
	MLogOutput nLogOutput = MLOG_FILED;

#ifdef _PUBLISH
	nLogLevel = MLOG_LEVEL_INFORMATION;
	nLogOutput = MLOG_FILE;
#else

#ifndef _DEBUG
	if (XCONST::DEBUG_LOG == false)
	{
		nLogLevel = MLOG_LEVEL_INFORMATION;
	}
#endif


#endif

	// 로그 초기화
	MInitLog(nLogLevel, 
		nLogOutput, 
		NULL, 
		"mlog.txt",
		"mlog.xml",
		MLocale::ConvUTF16ToAnsi(XPath::GetMyDocumentsRaiderzSubPath(PATH_MYDOC_SAVE).c_str()).c_str());

#ifndef _DEBUG
	MCrashDump::Init(NULL, NULL, XCONST::FULL_DUMP, &XCrashDumpHandler);
#endif

	return true;
}

bool InitLogForFileSystemError()
{
	MLogLevel nLogLevel = MLOG_LEVEL_DEBUG;
	MLogOutput nLogOutput = MLOG_FILED;

#ifdef _PUBLISH
	nLogLevel = MLOG_LEVEL_INFORMATION;
	nLogOutput = MLOG_FILE;
#endif

	// 로그 초기화
	MInitLog(nLogLevel, 
		nLogOutput, 
		NULL, 
		"mlog.txt",
		"mlog.xml",
		MLocale::ConvUTF16ToAnsi(XPath::GetMyDocumentsRaiderzSubPath(PATH_MYDOC_SAVE).c_str()).c_str());

	return true;
}

class XMinetInitScope
{
public:
	XMinetInitScope() 
	{
		minet::Init(SH_COMMAND_VERSION);
		XAddCommandTable();
	}
	~XMinetInitScope() 
	{
		minet::Fini();
	}
};

class XProfileScope
{
public:
	XProfileScope() 
	{
	}
	~XProfileScope() 
	{
#ifdef _PUBLISH
		return;
#endif

		MSaveStatisticsProfile("profile_game.txt", true);
	}
};


void CallbackDetectedInvalidFile()
{
	XGetAppErrorCollector()->SetLastError( ERRORTYPE_SYSTEM, L"An invalid file detected. Please check for anti-virus program.");
	XGetAppErrorCollector()->ExitApplication( 300000);
}


bool InitFileSystem() 
{
#ifdef _PUBLIS

	global.filesys = MCreateFileSystem( MFILEACCESS_ENCRYPTED);
	global.filesys->SetDetectInvalidFileCallback( CallbackDetectedInvalidFile);
	global.filesys->ReadFileIndex( "fileindex.msf");

#else

	// 폴더 내에 READ_MRS.sys 파일이 있으면 MRS 전용 읽기 모드로 실행
	FILE* pFile;
	fopen_s( &pFile, "READ_MRS.sys", "r");
	if ( pFile != NULL)
	{
		fclose( pFile);
		if( rs3::REngine::IsDevelopVersion() )
			global.filesys = MCreateFileSystem( MFILEACCESS_GENERIC, "EngineResDev;Data");
		else
			global.filesys = MCreateFileSystem( MFILEACCESS_GENERIC, "EngineRes;Data");
	}
	else
	{
		global.filesys = MCreateFileSystem( MFILEACCESS_ENCRYPTED);
		global.filesys->SetDetectInvalidFileCallback( CallbackDetectedInvalidFile);
		global.filesys->ReadFileIndex( "fileindex.msf");


	}

#endif

	return true;
}

int PASCAL wWinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPWSTR cmdline, int cmdshow)
{
	// Create anti-hack module
	if ( XGetAntiHack()->CreateModule() == false)
		return 0;


#ifdef _PUBLISH
	// 윈도우 XP 이하 버젼은 실행 안됨.
	OSVERSIONINFOEX osvi;
	ZeroMemory( &osvi, sizeof( OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX);
	if ( GetVersionEx( (OSVERSIONINFO*)&osvi) == FALSE)
	{
		osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO);
		if ( GetVersionEx( (OSVERSIONINFO*)&osvi) == FALSE)
		{
			XGetAppErrorCollector()->SetLastError( ERRORTYPE_SYSTEM, L"현재 실행 중인 윈도우 버젼을 확인할 수 없기 때문에 프로그램을 실행할 수 없습니다. 불편을 끼쳐드려 죄송합니다.");
			return 0;
		}
	}

	if ( osvi.dwPlatformId != VER_PLATFORM_WIN32_NT  ||  osvi.dwMajorVersion < 5  ||  ( osvi.dwMajorVersion == 5  &&  osvi.dwMinorVersion == 0))
	{
		XGetAppErrorCollector()->SetLastError( ERRORTYPE_SYSTEM, L"이 프로그램은 Windows XP 이상에서만 실행가능합니다. 불편을 끼쳐드려 죄송합니다.");
		return 0;
	}
#endif


#ifdef _PUBLISH
	// Publish 모드에서 파라미터가 존재하지 않는다면 종료한다
	if ( cmdline == NULL  ||  cmdline[ 0] == 0)
	{
		XGetAppErrorCollector()->SetLastError( ERRORTYPE_APPLICATION, L"No command line parameters.");
		XGetAntiHack()->RemoveModule();
		return 0;
	}
#endif


	XMinetInitScope MinetInitScope;
	XProfileScope ProfileScope;
	XMemPoolScopeReleaser	MemPoolScopeReleaser;


	// Pre-run application 
	if ( PreAppRun( cmdline) == false)
	{
		XGetAntiHack()->RemoveModule();
		return 0;
	}


	// Create file system
	if ( InitFileSystem() == false)
	{
		InitLogForFileSystemError();
		mlog("InitFileSystem Failed\n");

		XGetAppErrorCollector()->SetLastError( ERRORTYPE_APPLICATION, L"Can't create file system.\n\nMay not enough system resource.");
		XGetAntiHack()->RemoveModule();
		return 0;
	}


	// Init LOG and constance defines
	InitLogNConst();


	// System Info Log
	XSystemInfoUtil systemInfoUtil;
	systemInfoUtil.SetLogSystemInfo();

	// Create application
	XGameApp* g_pApp = new XGameApp();
	g_pApp->Init(cmdline);

	int ret = g_pApp->Run();
	if (!ret)
		mlog_popuplasterror(MLocale::ConvUTF16ToAnsi(APPLICATION_NAME).c_str());

	PostAppRun();

	delete g_pApp;


	// Remove anti-hack module
	XGetAntiHack()->RemoveModule();

	return ret;
}



