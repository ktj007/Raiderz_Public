#include "stdafx.h"
#include "McvConfig.h"
#include "MDebug.h"
#include "Mcv.h"
#include "RUnitTestReporter.h"
#include "UnitTest++.h"
#include "RUnitTestRunner.h"
#include "MFileSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#pragma comment ( lib, "UnitTest++.vsnet2008d.lib" )
#else
#pragma comment ( lib, "UnitTest++.vsnet2008.lib" )
#endif

int RunUnitTest()
{
	MInitLog();
	mlog("run unit test\n");

	const char* TEST_XML_FILENAME = "Result_UnitTest.xml";

	std::ofstream f(TEST_XML_FILENAME);
	RXmlAndStdTestReporter reporter(f);  

	RUnitTestRunner::GetInstance().InitMockEngine();
	int failure_count = UnitTest::RunAllTests(reporter, UnitTest::Test::GetTestList(), NULL, 0); 
	RUnitTestRunner::GetInstance().HaltMockEngine();

	MFinalizeLog();

	return failure_count;
}


BEGIN_MESSAGE_MAP(CMcvApp, CWinApp)
	//{{AFX_MSG_MAP(CMcvApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMcvApp::CMcvApp()
: m_pMcvMain(NULL)
{
}

CMcvApp::~CMcvApp()
{
//	MFinalLog();
}

CMcvApp theApp;

CMcvApp*   GetMainApp()		{ return &theApp; }

#include "../Tools/Composite/Composite.h"

BOOL CMcvApp::InitInstance()
{
	string strPath;
	if( REngine::IsDevelopVersion() )
		strPath = "../../EngineResDev;../../Data";
	else
		strPath = "../../EngineRes;../../Data";

	// 디버깅용 하드 코딩
	//strPath = "../../EngineRes;../../../../../Game/Runtime/Data";

	MCreateFileSystem( MFILEACCESS_GENERIC, strPath.c_str() );

//	_CrtSetBreakAlloc(78);

	if( strcmp(m_lpCmdLine,"/test")==0 ) 
		return FALSE;

	m_strConfigFile = "config.xml";
	
	char* szConfigFile = strstr(m_lpCmdLine, "/c");
	if( szConfigFile != NULL )
	{
		m_strConfigFile = szConfigFile+2;
	}

	if(!McvConfig::GetInstance().LoadConfig( m_strConfigFile ))
	{
		MessageBox( NULL, "can't initialize. please check config.xml", "error", MB_OK );
		return FALSE;
	}

	MInitLog(MLOG_LEVEL_DEBUG, MLOG_FILE|MLOG_DEBUG | MLOG_PROG,CMcvView::McvLog);

	CoInitialize(NULL);

	AfxEnableControlContainer();

	SetRegistryKey( McvConfig::GetInstance().m_strRegistryRoot.c_str() );

	LoadStdProfileSettings(10);  

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate( 
		IDR_MAINFRAME, 
		RUNTIME_CLASS(CMcvDoc), 
		RUNTIME_CLASS(CMainFrame), 
		RUNTIME_CLASS(CMcvView));
	if (!pDocTemplate)
		return FALSE;

	AddDocTemplate(pDocTemplate);

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (!ProcessShellCommand(cmdInfo))	return FALSE;


	CRect rect;
	rect.left = 20;
	rect.right = rect.left + 1200;
	rect.top = 20;
	rect.bottom = rect.top + 1000;
	m_pMainWnd->MoveWindow( rect);
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	m_pMcvMain = new CMcvMain;
	if(!m_pMcvMain->Create(GetMainView()->m_hWnd))
	{
		SAFE_DELETE(m_pMcvMain);
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMcvApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMcvApp message handlers

BOOL CMcvApp::OnIdle(LONG lCount) 
{
	CMcvView* pView = GetMainView();

	if(pView) {

		BOOL bWindowTop = pView->IsTopParentActive();

		if( bWindowTop ) {
			pView->Idle();
			::Sleep(1);
		}
		else {
			pView->Idle();
			::Sleep(10);
		}
	}

	CWinApp::OnIdle(1);

	return 1;
}

int CMcvApp::ExitInstance() 
{
	MFinalizeLog();

	SAFE_DELETE(m_pMcvMain);

	// 유닛테스트 리턴
	if( strcmp(m_lpCmdLine,"/test")==0 ) 
		return RunUnitTest();

	CoUninitialize();
	return CWinApp::ExitInstance();
}

BOOL CAboutDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	return CDialog::PreCreateWindow(cs);
}

CDocument* CMcvApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

//	return CWinApp::OpenDocumentFile(lpszFileName);

	GetMainView()->FileOpen((char*)lpszFileName);
	return GetMainView()->GetDocument();
}

CMainFrame* GetMainFrame()
{
	return (CMainFrame*)AfxGetMainWnd();
}

CControlToolBar* GetControlToolBar()
{
	return (CMainFrame*)AfxGetMainWnd() ? ((CMainFrame*)AfxGetMainWnd())->GetControlToolBar() : NULL;
}

CFrameControlBar* GetFrameControlBar()
{
	return (CMainFrame*)AfxGetMainWnd() ? ((CMainFrame*)AfxGetMainWnd())->GetFrameControlBar() : NULL;
}

CAnimationSidebar* GetAnimationSidebar()
{
	return (CMainFrame*)AfxGetMainWnd() ? ((CMainFrame*)AfxGetMainWnd())->GetControlToolBar()->GetAnimationSidebar() : NULL;
}

CModelInfoSidebar* GetModelInfoSidebar()
{
	return (CMainFrame*)AfxGetMainWnd() ? ((CMainFrame*)AfxGetMainWnd())->GetControlToolBar()->GetModelInfoSidebar() : NULL;
}

CViewControlSidebar* GetViewControlSidebar()
{
	return (CMainFrame*)AfxGetMainWnd() ? ((CMainFrame*)AfxGetMainWnd())->GetControlToolBar()->GetViewControlSidebar() : NULL;
}

CLightSidebar* GetLightSidebar()
{
	return (CMainFrame*)AfxGetMainWnd() ? ((CMainFrame*)AfxGetMainWnd())->GetControlToolBar()->GetLightSidebar() : NULL;
}