
// testDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "test.h"
#include "testDlg.h"


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


// CtestDlg 대화 상자




CtestDlg::CtestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CtestDlg::IDD, pParent)
	, m_pPropCtrl( NULL )
	, m_pPropCtrl2( NULL )
	, m_pPropCtrl3( NULL )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

CtestDlg::~CtestDlg()
{
	SAFE_DELETE(m_pPropCtrl);
	SAFE_DELETE(m_pPropCtrl2);
	SAFE_DELETE(m_pPropCtrl3);
}

void CtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CtestDlg 메시지 처리기

BOOL CtestDlg::OnInitDialog()
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

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	//InitPropGrid();
	 
	//**************************** MFC ReflectiveProperty 사용 방법 **************************************************
	m_pPropCtrl =	m_UI.createPropGridCtrl( GetDlgItem(IDC_STATIC_PROPERTY) );		//이 다이알로그를 부모로 가지는 프로퍼티그리드콘트롤 생성
	m_UI.BuildUIForObject(m_pPropCtrl,&m_box);														//해당그룹내에 프로퍼티그리드아이템추가

	m_pPropCtrl2 =	m_UI.createPropGridCtrl( GetDlgItem(IDC_STATIC_PROPERTY2) );		//이 다이알로그를 부모로 가지는 프로퍼티그리드콘트롤 생성
	m_UI.BuildUIForObject(m_pPropCtrl2,&m_circle);

// 	// multiple object
// 	CReflectivePropertyGroup* pGroupMulti =		m_UI.createGridGroup(m_pPropCtrl,"Multiple Objects");	//프로퍼티그리드 그룹생성

	OBJECT_LIST listObjects;
	listObjects.push_back( &m_multiBox1 );
	listObjects.push_back( &m_multiBox2 );

	m_multiBox2.setNumber( 1 );			// 다르다
	m_multiBox2.setHeight( 1 );			// 달라서 값이 UI 에 안나와야 한다
	m_multiBox1.setName( "SameName" );
	m_multiBox2.setName( "SameName" );	// 같아서 나온다
	m_multiBox1.setTestName( "DiffName1" );	// 달라서 안나와야 함
	m_multiBox2.setTestName( "DiffName2" );

	m_pPropCtrl3 =	m_UI.createPropGridCtrl( GetDlgItem(IDC_STATIC_PROPERTY3) );		//이 다이알로그를 부모로 가지는 프로퍼티그리드콘트롤 생성
	m_UI.BuildUIForObjects(m_pPropCtrl3, listObjects);								//해당그룹내에 프로퍼티그리드아이템추가

	//****************************************************************************************************************

	CReflectivePropertyGridCtrl::PROPERTY_CHANGE_HANDLER handler(this, &CtestDlg::OnPropertyChanged );
	m_pPropCtrl->SetCallback( handler );

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CtestDlg::OnPropertyChanged()
{
	// !! 프로퍼티가 변경되었을때
}

CMFCPropertyGridProperty* CtestDlg::createGridGroup( std::string groupName )
{
	CMFCPropertyGridProperty * pGroup = new CMFCPropertyGridProperty( _T(groupName.c_str()) );
	m_wndPropList.AddProperty( pGroup );
	return pGroup;
}

//MFC의 프로퍼티 그리드를 사용하는 예제. 해당프로젝트하고는 상관없는 함수. 즉, 이소스 안봐도 된다는 이야기.
//하지만, MFC 프로퍼티 그리드사용하는 법을 알고 싶으면 보세요.
void CtestDlg::InitPropGrid()  
{
	// Switch look and feel to office 2007 style
 	CMFCVisualManagerOffice2007::SetStyle( CMFCVisualManagerOffice2007::Office2007_ObsidianBlack );
 	CMFCVisualManager::SetDefaultManager( RUNTIME_CLASS( CMFCVisualManagerOffice2007 ));
 
 	CRect rect;
 	this->GetClientRect( rect );
 	this->MapWindowPoints( this, &rect );
 
	rect.bottom-= 50;

 	m_wndPropList.Create( WS_CHILD | WS_BORDER | WS_VISIBLE | WS_TABSTOP, rect, this, 1231 );
 	m_wndPropList.EnableHeaderCtrl( TRUE, _T( "LEFT Property" ), _T( "Right Value" ));
 	m_wndPropList.SetVSDotNetLook( TRUE );

	// Create a property group for appearance


	CMFCPropertyGridProperty * pGroupTest = new CMFCPropertyGridProperty( _T("TEST") );
	m_wndPropList.AddProperty( pGroupTest );
	const int MaxNesting = 5;
	CMFCPropertyGridProperty *pParent = pGroupTest;
	for( int Index = 0; Index < MaxNesting; ++Index )
	{
		CString Text;
		Text.Format( _T( "Nesting %d" ), Index + 1 );

		CMFCPropertyGridProperty* pChild = new CMFCPropertyGridProperty( Text );

		// Display's a combo with options as True, False, Cool!
		COleVariant Bool((short)VARIANT_FALSE, VT_BOOL);
		pParent->AddSubItem(new CMFCPropertyGridProperty(_T("Bool test"), Bool, false));//_T("Testing kids")));
		pParent->AddSubItem( pChild );
		pParent = pChild;
	}

	// A font property
	LOGFONT lf = { 0 };
	GetFont()->GetLogFont( &lf );
	CMFCPropertyGridFontProperty* pFntProp = new CMFCPropertyGridFontProperty( _T( "Font (Font dialog comes up)" ), lf );
	pGroupTest->AddSubItem( pFntProp );

	// Combo property, set sub options which are displayed in a combo
	CMFCPropertyGridProperty* pCmbProp = new CMFCPropertyGridProperty(_T("Border (A combo box)"), _T("Dialog Frame"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
	pCmbProp->AddOption(_T("None"));
	pCmbProp->AddOption(_T("Thin"));
	pCmbProp->AddOption(_T("Resizable"));
	pCmbProp->AddOption(_T("Dialog Frame"));
	pCmbProp->AllowEdit(FALSE);
	pGroupTest->AddSubItem( pCmbProp );

	// A folder browse dialog property
	CMFCPropertyGridFileProperty* pFolderProp = new CMFCPropertyGridFileProperty( _T( "Select folder (Browse for folder dialog)" ), _T( "C:\\Windows" ));
	pGroupTest->AddSubItem( pFolderProp );

	// A file open dialog property
	CMFCPropertyGridFileProperty* pFileProp = new CMFCPropertyGridFileProperty( _T( "Select file (Open file dialog)" ), TRUE, _T( "C:\\Windows" ));
	pGroupTest->AddSubItem( pFileProp );

	// A masked edit control for phone number
	pGroupTest->AddSubItem( new CMFCPropertyGridProperty(_T("Phone (Masked edit)"), _T("(123) 123-12-12"), _T("Enter a phone number"), 0, _T(" ddd  ddd dd dd"), _T("(___) ___-__-__")));

	// A color property
	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty( _T( "Select color" ), RGB( 120, 198, 250 ));
	pGroupTest->AddSubItem( pColorProp );

	// Set custom colors for property grid
	m_wndPropList.SetCustomColors(RGB(228, 243, 254), RGB(46, 70, 165), RGB(200, 236, 209), RGB(33, 102, 49), RGB(255, 229, 216), RGB(128, 0, 0), RGB(159, 159, 255));
}

void CtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CtestDlg::OnPaint()
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
HCURSOR CtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


