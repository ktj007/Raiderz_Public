
// RS3DebuggerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "RS3Analyzer.h"
#include "RS3AnalyzeDialog.h"
#include "MProfiler.h"
#include <string>

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


// CRS3AnalyzeDialoga 대화 상자




CRS3AnalyzeDialoga::CRS3AnalyzeDialoga(CWnd* pParent /*=NULL*/)
	: CDialog(CRS3AnalyzeDialoga::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//변수 초기화
	m_pPerformanceViewerDialog = NULL;
	m_pProfileDialog = NULL;
}

CRS3AnalyzeDialoga::~CRS3AnalyzeDialoga( void )
{
	if( NULL != m_pPerformanceViewerDialog )
	{
		m_pPerformanceViewerDialog->DestroyWindow();
		delete m_pPerformanceViewerDialog;
		m_pPerformanceViewerDialog = NULL;
	}

	if( NULL != m_pProfileDialog)
	{
		m_pProfileDialog->DestroyWindow();
		delete m_pProfileDialog;
		m_pProfileDialog = NULL;
	}
}

void CRS3AnalyzeDialoga::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRS3AnalyzeDialoga, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_BUTTON1, &CRS3AnalyzeDialoga::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CRS3AnalyzeDialoga::OnBnClickedButton2)
END_MESSAGE_MAP()


// CRS3AnalyzeDialoga 메시지 처리기

BOOL CRS3AnalyzeDialoga::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	SetWindowText(DEBUGGER_DIALOG_NAME); 
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRS3AnalyzeDialoga::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRS3AnalyzeDialoga::OnPaint()
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
HCURSOR CRS3AnalyzeDialoga::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





BOOL CRS3AnalyzeDialoga::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->message == WM_COPYDATA)
	{
		int d=0;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CRS3AnalyzeDialoga::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	switch( pCopyDataStruct->dwData )
	{
		case DI_CM_PROFILEINFO:
		{
			if( NULL != m_pProfileDialog )
				m_pProfileDialog->AddProfileInfo((char*)pCopyDataStruct->lpData,pCopyDataStruct->cbData);
		}break;

		case DI_CM_PROFILESTRING:
		{
			if( NULL != m_pProfileDialog )
				m_pProfileDialog->SetProfileIDToName((char*)pCopyDataStruct->lpData,pCopyDataStruct->cbData);
		}break;

		case DI_CM_PERFORMANCEINFO:
		{
			if( NULL != m_pPerformanceViewerDialog )
				m_pPerformanceViewerDialog->UpdateProfileInfo((char*)pCopyDataStruct->lpData,pCopyDataStruct->cbData);
		}break;
	}

	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

void CRS3AnalyzeDialoga::OnBnClickedButton1()//PerformanceViewer버튼
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PerformanceViewerDialogOnOff();
}

void CRS3AnalyzeDialoga::PerformanceViewerDialogOnOff( void )
{
	if( NULL == m_pPerformanceViewerDialog )
	{//ON
		m_pPerformanceViewerDialog = new RResourceAnalyzeDialog;
		m_pPerformanceViewerDialog->Create(IDD_DIALOG_PERFORMANCE,this);
		m_pPerformanceViewerDialog->ShowWindow(SW_SHOW);
	}
	else
	{//OFF
		m_pPerformanceViewerDialog->DestroyWindow();
		delete m_pPerformanceViewerDialog;
		m_pPerformanceViewerDialog = NULL;
	}
}
void CRS3AnalyzeDialoga::OnBnClickedButton2()//Profile버튼
{
	if( NULL == m_pProfileDialog)
	{
		m_pProfileDialog = new RCPUAnalyzeDialog;
		m_pProfileDialog->Create(IDD_DIALOG_PROFILE,this);
		m_pProfileDialog->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pProfileDialog->DestroyWindow();
		delete m_pProfileDialog;
		m_pProfileDialog = NULL;
	}

}
