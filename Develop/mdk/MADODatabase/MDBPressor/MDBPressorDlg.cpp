// MDBPressorDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MDBPressorApp.h"
#include "MDBPressorDlg.h"

#include "MDBPressor.h"
#include "MDBTimePressor.h"
#include "LogListBox.h"
#include "TestReportList.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


BOOL MDBPressorDlg::Run()
{
	if (NULL != m_pDBPressor)
	{
		if (m_pDBPressor->IsEndTest())
		{
			this->OutputEndReport();
			m_pDBPressor->CatchEndTest();
		}

		if (m_pDBPressor->IsEndPress())
		{
			m_pDBPressor->EndPress();
			m_pDBPressor->Destroy();
			delete m_pDBPressor;
			m_pDBPressor	= NULL;

			GetDlgItem(IDC_BTN_QUERY_START)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BTN_QUERY_STOP)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_BTN_RELOAD)->EnableWindow(TRUE);
			GetDlgItem(IDC_BTN_QUERY_START)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_TIMER)->EnableWindow(TRUE);
			if (static_cast<CButton* >(GetDlgItem(IDC_CHECK_TIMER))->GetCheck())
			{
				this->EnableTimerSet(TRUE);
			}
		}
	}

	return TRUE;
}
void MDBPressorDlg::OutputEndReport()
{
	int nCurrentTestNum		= m_pDBPressor->GetCurrentTestNum();
	char szStartTime[32]	= {0,};
	char szEndTime[64]		= {0,};

	const char*	pStartTime	= m_pDBPressor->GetStartTestTime();
	strncpy_s(szStartTime, 32, pStartTime, 32);

	const char*	pEndTime	= m_pDBPressor->GetEndTestTime();
	strncpy_s(szEndTime, 32, pEndTime, 32);

	int		nCompleteCount	= m_pDBPressor->GetCompleteCount();
	int		nFailCount		= m_pDBPressor->GetFailCount();
	double	dElapsedTime	= m_pDBPressor->GetElapsedTime();
	int		nTestCount		= m_pDBPressor->GetExecuteCount();
	int		nCountPerSec	= nTestCount/dElapsedTime;

	m_pTestReport->OutputTestResult(nCurrentTestNum, szStartTime, szEndTime, dElapsedTime, nCompleteCount, nFailCount, nCountPerSec);
}

void MDBPressorDlg::OutputTestSummaryReport()
{
	m_pTestReport->OutputTestSummaryHeader();
	m_pTestReport->OutputTestSummary(m_strDataSource,m_strCatalog, m_strPoolSize, m_strQuery);
}

void MDBPressorDlg::OutputTimerInfoReport()
{
	m_pTestReport->OutputTimerInfoHeader();

	CString strTimeH;
	GetDlgItemText(IDC_EDIT_TIME_H, strTimeH);
	CString strTimeM;
	GetDlgItemText(IDC_EDIT_TIME_M, strTimeM);
	CString strTimeS;
	GetDlgItemText(IDC_EDIT_TIME_S, strTimeS);
	CString strTimeTerm;
	GetDlgItemText(IDC_EDIT_TIME_TERM, strTimeTerm);

	m_pTestReport->OutputTimerInfo(strTimeH, strTimeM, strTimeS, strTimeTerm);
}

const int MAX_INIT_STRING_SIZE	= 128;

void MDBPressorDlg::InitDlgItem()
{
	this->LoadSettingFile();
	this->OutputEnvInfo();

	SetDlgItemText( IDC_EDIT_TIME_H,	"0" );
	SetDlgItemText( IDC_EDIT_TIME_M,	"0" );
	SetDlgItemText( IDC_EDIT_TIME_S,	"0" );
	SetDlgItemText( IDC_EDIT_TIME_TERM,	"0" );
	this->EnableTimerSet( FALSE );
}
void MDBPressorDlg::LoadSettingFile()
{
	char szPath[512]				= {0,};
	GetCurrentDirectory( 512, szPath );
	strncat_s( szPath, "\\DBPress.INI", 512 );


	char szDataSource[MAX_INIT_STRING_SIZE]		= {0,};
	char szCatalog[MAX_INIT_STRING_SIZE]		= {0,};
	char szID[MAX_INIT_STRING_SIZE]				= {0,};
	char szPWD[MAX_INIT_STRING_SIZE]			= {0,};
	char szPoolSize[MAX_INIT_STRING_SIZE]		= {0,};
	char szExecuteThread[MAX_INIT_STRING_SIZE]	= {0,};
	int	nExecuteCount	= 0;
	DWORD dwRet			= 0;

	/////////////////////////////////////////////////////////////////////////
	///	INI 파일에서 Connection 관련 정보 읽어오기.
	dwRet	= GetPrivateProfileString( "DB","DataSource",	"127.0.0.1", 
										szDataSource,	MAX_INIT_STRING_SIZE, szPath);
	dwRet	= GetPrivateProfileString( "DB","Catalog",		"master", 
										szCatalog,		MAX_INIT_STRING_SIZE, szPath);
	dwRet	= GetPrivateProfileString( "DB","UserID",		"sa",
										szID,			MAX_INIT_STRING_SIZE, szPath);
	dwRet	= GetPrivateProfileString( "DB","Password",	"password", 
										szPWD,			MAX_INIT_STRING_SIZE, szPath);

	char szQueryString[1024]	= {0,};

	dwRet			= GetPrivateProfileString( "SETTING",	"PoolSize",	"100",
												szPoolSize,		MAX_INIT_STRING_SIZE, szPath);
	dwRet			= GetPrivateProfileString( "SETTING", "QueryString", "",
												szQueryString,	256, szPath);
	nExecuteCount	= GetPrivateProfileInt( "SETTING", "ExecuteCount", 1, szPath);



	m_strDataSource			= szDataSource;
	m_strCatalog			= szCatalog;
	m_strID					= szID;
	m_strPassword			= szPWD;
	m_strPoolSize			= szPoolSize;

	m_strConnectionString
		=	"Data Source="		+ m_strDataSource
		+	";Initial Catalog="	+ m_strCatalog
		+	";user id="			+ m_strID
		+	";password="		+ m_strPassword
		+	";Asynchronous Processing=true"
		+	";Max Pool Size="	+ m_strPoolSize;

	m_strQuery				= szQueryString;
	m_nExecuteCount			= nExecuteCount;
}

void MDBPressorDlg::OutputEnvInfo()
{
	std::string	strTestInfo;
	strTestInfo	=	"DataSource\t\t: ";
	strTestInfo	+=  m_strDataSource;
	strTestInfo	+=	"\r\n";

	strTestInfo	+=	"Catalog\t\t\t: ";
	strTestInfo	+=  m_strCatalog;
	strTestInfo	+=	"\r\n";

	strTestInfo	+=	"ID\t\t\t: ";
	strTestInfo	+=	m_strID;
	strTestInfo	+=	"\r\n";

	strTestInfo	+=	"PWD\t\t\t: ";
	strTestInfo	+=	m_strPassword;
	strTestInfo	+=	"\r\n";

	strTestInfo	+=	"Connection Pool Size\t: ";
	strTestInfo	+=	m_strPoolSize;
	strTestInfo	+=	"\r\n";

	strTestInfo	+=	"Test Query\t\t: \"";
	strTestInfo	+=	m_strQuery;
	strTestInfo	+=	"\"\r\n";

	char szExecuteCount[128];
	_itoa_s( m_nExecuteCount, szExecuteCount, 128, 10 );
	strTestInfo	+=	"Execute Count\t\t: ";
	strTestInfo	+=	szExecuteCount;
	strTestInfo	+=	"\r\n";

	strTestInfo	+=	"\r\n";
	strTestInfo	+=	"Log File Path\t\t: ";
	strTestInfo	+=	g_pszLogFilePath;

	SetDlgItemText( IDC_EDIT_TESTENV, strTestInfo.c_str() );
}

void MDBPressorDlg::CheckTimerSet(BOOL bCheck)
{
	static_cast<CButton*>(GetDlgItem( IDC_CHECK_TIMER ))->SetCheck( bCheck );
}

void MDBPressorDlg::EnableTimerSet(BOOL bEnable)
{
	if (bEnable)
	{
		GetDlgItem( IDC_EDIT_TIME_H )->EnableWindow( TRUE );
		GetDlgItem( IDC_EDIT_TIME_M )->EnableWindow( TRUE );
		GetDlgItem( IDC_EDIT_TIME_S )->EnableWindow( TRUE );
		GetDlgItem( IDC_EDIT_TIME_TERM )->EnableWindow( TRUE );
	}
	else
	{
		GetDlgItem( IDC_EDIT_TIME_H )->EnableWindow( FALSE );
		GetDlgItem( IDC_EDIT_TIME_M )->EnableWindow( FALSE );
		GetDlgItem( IDC_EDIT_TIME_S )->EnableWindow( FALSE );
		GetDlgItem( IDC_EDIT_TIME_TERM )->EnableWindow( FALSE );
	}

	this->OutputTimerInfo();
}
void MDBPressorDlg::OutputTimerInfo()
{
	CString strTimerInfo	= "";

	unsigned int nWholeTime			= this->CalTimerWholeTime();
	unsigned int nTerm				= GetDlgItemInt( IDC_EDIT_TIME_TERM );
	unsigned int nWholeTestCount	= this->CalTimerWholeTestCount();

	if (0 != nWholeTime)
	{
		char szWholeTime[128];
		_ultoa_s( nWholeTime, szWholeTime, 128, 10 );

		strTimerInfo	+= szWholeTime;
		strTimerInfo	+= "s간,";
	}

	if (0 != nTerm)
	{
		char szTerm[128];
		_ultoa_s( nTerm, szTerm, 128, 10 );
		strTimerInfo	+= szTerm;
		strTimerInfo	+= "ms간격으로,";
	}

	char szWholeTestCount[128];
	_ultoa_s( nWholeTestCount, szWholeTestCount, 128, 10 );

	strTimerInfo	+= szWholeTestCount;
	strTimerInfo	+= "번 실행";


	SetDlgItemText( IDC_EDIT_TIMERINFO, strTimerInfo );
}

unsigned int MDBPressorDlg::CalTimerWholeTime()
{
	unsigned int nTimeH	= GetDlgItemInt( IDC_EDIT_TIME_H );
	unsigned int nTimeM	= GetDlgItemInt( IDC_EDIT_TIME_M );
	unsigned int nTimeS	= GetDlgItemInt( IDC_EDIT_TIME_S );
	unsigned int nWholeTime	= ((nTimeH * 60 * 60) + (nTimeM * 60) + nTimeS);

	return nWholeTime;
}
unsigned int MDBPressorDlg::CalTimerWholeTestCount()
{
	unsigned int nWholeTime			= this->CalTimerWholeTime() * 1000;
	unsigned int nTerm				= GetDlgItemInt( IDC_EDIT_TIME_TERM );
	unsigned int nExecuteCount		= m_nExecuteCount;

	unsigned int nWholeExecuteCount	= nExecuteCount;

	if (0 != nWholeTime)
	{
		if (0 != nTerm)
		{
			nWholeExecuteCount	= (nWholeTime/nTerm) * nExecuteCount;
		}
	}
		

	return nWholeExecuteCount;
}

MDBPressorDlg::MDBPressorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MDBPressorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pTestReport	= NULL;
	m_pDBPressor	= NULL;
	m_nExecuteCount	= 0;
}

void MDBPressorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TESTREPORT, m_listTestReport);
}

BEGIN_MESSAGE_MAP(MDBPressorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_QUERY_START, &MDBPressorDlg::OnBnClickedBtnQueryStart)
	ON_BN_CLICKED(IDC_BTN_QUERY_STOP, &MDBPressorDlg::OnBnClickedBtnQueryStop)
	ON_BN_CLICKED(IDC_BTN_RELOAD, &MDBPressorDlg::OnBnClickedBtnReload)
	ON_BN_CLICKED(IDC_CHECK_TIMER, &MDBPressorDlg::OnBnClickedCheckTimer)
	ON_EN_CHANGE(IDC_EDIT_TIME_H, &MDBPressorDlg::OnEnChangeEditTimeNTerm)
	ON_EN_CHANGE(IDC_EDIT_TIME_M, &MDBPressorDlg::OnEnChangeEditTimeNTerm)
	ON_EN_CHANGE(IDC_EDIT_TIME_S, &MDBPressorDlg::OnEnChangeEditTimeNTerm)
	ON_EN_CHANGE(IDC_EDIT_TIME_TERM, &MDBPressorDlg::OnEnChangeEditTimeNTerm)
END_MESSAGE_MAP()


// MDBPressorDlg 메시지 처리기

BOOL MDBPressorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	/// Create Test Report List
	m_pTestReport	= new TestReportList( &m_listTestReport );
	m_pTestReport->Init();

	this->InitDlgItem();

	GetDlgItem(IDC_BTN_QUERY_STOP)->ShowWindow(SW_HIDE);


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void MDBPressorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void MDBPressorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR MDBPressorDlg::OnQueryDragIcon()
{ 
	return static_cast<HCURSOR>(m_hIcon);
}


void MDBPressorDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	MDBPressorApp* pMDBPressorApp	= static_cast<MDBPressorApp* >(AfxGetApp());
	pMDBPressorApp->Shutdown();

	if (NULL != m_pTestReport)
		delete m_pTestReport;

	CDialog::OnClose();
}


void MDBPressorDlg::OnBnClickedBtnQueryStart()
{
	///	Create DBPressor
	int nCheck	= static_cast<CButton* >(GetDlgItem(IDC_CHECK_TIMER))->GetCheck();
	if (nCheck)
	{
		m_pDBPressor	= new MDBTimePressor();

		int nTimeH	= GetDlgItemInt( IDC_EDIT_TIME_H );
		int nTimeM	= GetDlgItemInt( IDC_EDIT_TIME_M );
		int nTimeS	= GetDlgItemInt( IDC_EDIT_TIME_S );
		int nTerm	= GetDlgItemInt( IDC_EDIT_TIME_TERM );
		int nWholeTime	= ((nTimeH * 60 * 60) + (nTimeM * 60) + nTimeS) * 1000;
		static_cast<MDBTimePressor* >(m_pDBPressor)->SetTimer( nWholeTime, nTerm );
	}
	else
	{
		m_pDBPressor	= new MDBPressor();
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (FALSE == m_pDBPressor->Init( m_strDataSource, m_strCatalog, m_strID, m_strPassword, m_strPoolSize ))
	{
		AfxMessageBox("Error=>m_pDBPressor->Init()");
		return;
	}

	m_pDBPressor->SetTestQuery( m_strQuery );
	m_pDBPressor->SetExecuteCount( m_nExecuteCount );

	if (FALSE == m_pDBPressor->StartPress())
	{
		AfxMessageBox("Error=>m_pDBPressor->StartPress()");
		return;
	}

	this->OutputTestSummaryReport();
	if (nCheck)
	{
		this->OutputTimerInfoReport();
	}
	m_pTestReport->OutputTestResultHeader();

	GetDlgItem(IDC_BTN_QUERY_START)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTN_QUERY_STOP)->ShowWindow(SW_SHOW);

	GetDlgItem( IDC_BTN_RELOAD )->EnableWindow( FALSE );
	GetDlgItem( IDC_BTN_QUERY_START )->EnableWindow( FALSE );
	GetDlgItem( IDC_CHECK_TIMER )->EnableWindow(FALSE);
	this->EnableTimerSet(FALSE);
}

void MDBPressorDlg::OnBnClickedBtnQueryStop()
{
	m_pDBPressor->EndPress();
	m_pDBPressor->Destroy();
	delete m_pDBPressor;
	m_pDBPressor	= NULL;

	GetDlgItem(IDC_BTN_QUERY_START)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_BTN_QUERY_STOP)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_BTN_RELOAD)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_QUERY_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_TIMER)->EnableWindow(TRUE);
	if (static_cast<CButton* >(GetDlgItem(IDC_CHECK_TIMER))->GetCheck())
	{
		this->EnableTimerSet(TRUE);
	}
}

void MDBPressorDlg::OnBnClickedBtnReload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	this->InitDlgItem();
}

void MDBPressorDlg::OnBnClickedCheckTimer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCheck	= static_cast<CButton*>(GetDlgItem( IDC_CHECK_TIMER ))->GetCheck();

	this->EnableTimerSet( nCheck );
}

void MDBPressorDlg::OnEnChangeEditTimeNTerm()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	this->OutputTimerInfo();
}
