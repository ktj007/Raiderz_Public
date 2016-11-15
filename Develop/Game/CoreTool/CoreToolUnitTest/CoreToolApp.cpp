// CoreTool 실행파일을 만들기 위한 App 파일
// 여기에 가급적 아무것도 추가하지 마시고 진입점을 찾으시려면 CoreToolRS3 프로젝트의 DLL_App 쪽을 찾아보세요
///////////////////////////////////////////////////////////////////////////////////////////////////
// CoreTool UnitTest 
// UnitTest의 시작은 InitInstance()->RunOnlyUnitTest()에서 수행하고,
// 결과값은 g_nUnitTestResult 변수에 저장되어 ExitInstance() 함수에서 리턴된다. 
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CoreToolApp.h"
#include "shlwapi.h"
#include <delayimp.h>

#include "Strsafe.h"

#include "DLL_Interface.h"
#include "AboutDlg.h"

#include "CUnitTestRunner.h"
#include "MDebug.h"

//#define _VLD

#ifdef _VLD
#include "vld.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExeApp

BEGIN_MESSAGE_MAP(CExeApp, CWinAppEx)
	//{{AFX_MSG_MAP(CExeApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()

int g_nUnitTestResult = 0;
int RunOnlyUnitTest();

/////////////////////////////////////////////////////////////////////////////
// CExeApp construction

CExeApp::CExeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CExeApp object

CExeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CExeApp initialization

BOOL CExeApp::InitInstance()
{
	CWinAppEx::InitInstance();

	// 종료시 0xC015000F: 에러 때문에 삽입
	// http://social.msdn.microsoft.com/Forums/en/vcgeneral/thread/c3feab0f-601b-4ca6-beb2-8d4d615438cc
	afxAmbientActCtx = FALSE;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	InitDll();

	pIDoc->OnInitInstance();


// #ifdef _AFXDLL
// 	Enable3dControls();			// Call this when using MFC in a shared DLL
// #else
// 	Enable3dControlsStatic();	// Call this when linking to MFC statically
// #endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("CoreToolRS3"));

	UINT nMaxMRU = 10;

	LoadStdProfileSettings(nMaxMRU);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

/*
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_EXETYPE,
		RUNTIME_CLASS(CExeDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CExeView));
	AddDocTemplate(pDocTemplate);
*/
	pIDoc->CreateDocTempl(this, C_DOC_1);

	// create main MDI Frame window
/*	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
*/
	pIDoc->CreateFrame();

	RunOnlyUnitTest();
	return TRUE;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	pIDoc->OnPostCreate();

#ifdef _VLD
	TRACE("======= Reload VLD Modules ========\n");
	VLDRefreshModules();
#endif

	// The main window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();

	pIDoc->OnShellOpenFile();
	return TRUE;
}

// App command to run the dialog
void CExeApp::OnAppAbout()
{
	HINSTANCE hInstResourceDll = AfxGetResourceHandle();
	HINSTANCE hInstResourceClient = (HINSTANCE) ::GetModuleHandle(NULL);

	AfxSetResourceHandle(hInstResourceClient);
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
	AfxSetResourceHandle(hInstResourceDll);
}

/////////////////////////////////////////////////////////////////////////////
// CExeApp message handlers


int CExeApp::ExitInstance() 
{
	TRACE("CExeApp::ExitInstance\n");

	pIDoc->OnExitInstance();

	//int nDocumentExitCode = pIDoc->GetExitCode();
	int nDocumentExitCode = g_nUnitTestResult;

 	int nExitCode = CWinAppEx::ExitInstance();

	// release document / dll
	ReleaseDll();

	TRACE("CExeApp::ExitInstance - ReleaseDll ok\n");

	//if (0 != nExitCode)
 	//	return nExitCode;
 
	return nDocumentExitCode;
}

LONG WINAPI DelayLoadExceptionFilter(PEXCEPTION_POINTERS pep)
{
	LONG lDisposition = EXCEPTION_EXECUTE_HANDLER;

	PDelayLoadInfo pdli = 
		PDelayLoadInfo(pep->ExceptionRecord->ExceptionInformation[0]);

	char sz[500] = {0};

	switch (pep->ExceptionRecord->ExceptionCode)
	{
	case VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND):
		//StringCchPrintfA(sz, _countof(sz), "Dll not found: %s", pdli->szDll);
		break;
	case VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND):
		if (pdli->dlp.fImportByName)
		{
			//StringCchPrintfA(sz, _countof(sz), "Function %s was not found in %s", pdli->dlp.szProcName, pdli->szDll);
		}
		else
		{
			//StringCchPrintfA(sz, _countof(sz), "Function ordinal %s was not found in %s", pdli->dlp.dwOrdinal, pdli->szDll);
		}
		break;
	default:
		lDisposition = EXCEPTION_CONTINUE_SEARCH;
		break;
	}

	AfxMessageBox(sz);

	return (lDisposition);
}

void CExeApp::InitDll()
{
	__try
	{
		__HrLoadAllImportsForDll(CORETOOL_DLL_NAME);
	}
	__except(DelayLoadExceptionFilter(GetExceptionInformation()))
	{

	}

	VERIFY( App_GetClassObject(CLSID_APP, IID_IDocument, (void**)&pIDoc) );
}

void CExeApp::ReleaseDll()
{
	pIDoc->Release();
	__FUnloadDelayLoadedDLL2(CORETOOL_DLL_NAME);
}

BOOL CExeApp::OnIdle(LONG lCount)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	BOOL bResult = CWinAppEx::OnIdle(lCount); // 리턴값 버린다
	return pIDoc->OnIdle(lCount);
}

//단축키등을 위한 메시처 전 처리 함수
BOOL CExeApp::PreTranslateMessage(MSG* pMsg) 
{
	if( pIDoc->PreTranslateMessage(pMsg) )
		return TRUE;

	return CWinAppEx::PreTranslateMessage(pMsg);
}

int RunOnlyUnitTest()
{
	// 로그 초기화
	MInitLog(MLOG_LEVEL_DEBUG, 
		MLOG_ALL, 
		NULL, 
		"mlog.txt");


	g_nUnitTestResult = CUnitTestRunner::CCNET_UNITTEST_SUCCESS;

	MUnitTestConfig unittest_config;
	unittest_config.Load();

	if (unittest_config.m_nRepeat > 1)
	{
		mlog("UnitTest Repeat Count = %d\n", unittest_config.m_nRepeat);
	}

	// stage1 --
	if (unittest_config.CheckStageFilter("Stage1"))
	{
		CUnitTestRunner	unitTestRunner;
		if (unitTestRunner.RunStage1() == false) 
		{
			g_nUnitTestResult = CUnitTestRunner::CCNET_UNITTEST_FAILED;
		}
	}

	return g_nUnitTestResult;
}
