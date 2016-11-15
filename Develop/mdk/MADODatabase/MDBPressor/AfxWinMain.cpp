#include "stdafx.h"
#include "Shlwapi.h"

#include "MMemoryLeak.h"


char	g_szLogFilePath[512]	= {0,};
LPTSTR	g_pszLogFilePath		= g_szLogFilePath;

static BOOL GetRecommandLogFileName( TCHAR* ptszBuf, INT nBuffer )
{
	if (PathIsDirectory(_T("Log")) == FALSE)
		CreateDirectory(_T("Log"), NULL);

	time_t		tClock;
	struct tm	tmTime;

	time( &tClock );
	localtime_s( &tmTime,&tClock );

	INT nFooter	= 1;
	while (TRUE)
	{
		_stprintf_s(g_szLogFilePath, 512, _T("Log/DBPressLog_%02d-%02d-%02d-%d.csv"), 
			tmTime.tm_year+1900, tmTime.tm_mon+1, tmTime.tm_mday, nFooter);

		if (PathFileExists(g_szLogFilePath) == FALSE)
			break;

		nFooter++;
		if (nFooter > 100)
			return FALSE;
	}
	_tcscpy_s(ptszBuf, nBuffer, g_szLogFilePath);

	return TRUE;
}



INT AFXAPI AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,	LPTSTR lpCmdLine, INT nCmdShow)
{
	TCHAR szLogFileName[_MAX_DIR];
	if (GetRecommandLogFileName(szLogFileName,sizeof(szLogFileName)) == FALSE)
		return FALSE;

	MInitLog(MLOG_LEVEL_INFORMATION, MLOG_FILE, 0, szLogFileName);

	// Wrap WinMain in a structured exception handler (different from C++
	// exception handling) in order to make sure that all access violations
	// and other exceptions are displayed - regardless of when they happen.
	// This should be done for each thread, if at all possible, so that exceptions
	// will be reliably caught, even inside the debugger.
#ifdef SUPPORT_EXCEPTIONHANDLING
	char szDumpFileName[_MAX_DIR]; 
	strcpy_s(szDumpFileName, szLogFileName);
	strcat_s(szDumpFileName, ".dmp");
	__try {
#endif
		// The code inside the __try block is the MFC version of AfxWinMain(),
		// copied verbatim from the MFC source code.
		ASSERT(hPrevInstance == NULL);

		INT nReturnCode	= -1;
		CWinApp* pApp	= AfxGetApp();

		// AFX internal initialization
		if (!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
			goto InitFailure;

		// App global initializations (rare)
		ASSERT_VALID(pApp);
		if (!pApp->InitApplication())
			goto InitFailure;
		ASSERT_VALID(pApp);

		// Perform specific initializations
		if (!pApp->InitInstance())
		{
			if (pApp->m_pMainWnd != NULL)
			{
				TRACE(_T("Warning: Destroying non-NULL m_pMainWnd\n"));
				pApp->m_pMainWnd->DestroyWindow();
			}
			nReturnCode = pApp->ExitInstance();
			goto InitFailure;
		}

		ASSERT_VALID(pApp);

		nReturnCode = pApp->Run();

		ASSERT_VALID(pApp);

InitFailure:
#ifdef _DEBUG
		// Check for missing AfxLockTempMap calls
		if (AfxGetModuleThreadState()->m_nTempMapLock != 0)
		{
			TRACE(_T("Warning: Temp map lock count non-zero (%ld).\n"),
				AfxGetModuleThreadState()->m_nTempMapLock);
		}
		AfxLockTempMaps();
		AfxUnlockTempMaps(-1);
#endif

		AfxWinTerm();
		return nReturnCode;

#ifdef SUPPORT_EXCEPTIONHANDLING
	}
//	__except(MFilterException(GetExceptionInformation())){
	//__except(CrashExceptionDump(GetExceptionInformation(), szDumpFileName)){
	__except(MCrashDump::TopLevelExceptionFilter(GetExceptionInformation())){
		
//		char szFileName[_MAX_DIR];
//		GetModuleFileName(NULL, szFileName, _MAX_DIR);
//		WinExec(szFileName, SW_SHOW);	// Launch again
		MMatchServer::GetInstance()->CheckMemoryTest();
		MGetServerStatusSingleton()->Dump();
	}
#endif
	return 0;
}