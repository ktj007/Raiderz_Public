// RCPUAnalyzeDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RS3Analyzer.h"
#include "RCPUAnalyzeDialog.h"
#include "RDebuggingInfo.h"

// RCPUAnalyzeDialog 대화 상자입니다.

IMPLEMENT_DYNCREATE(RCPUAnalyzeDialog, CDialog)

RCPUAnalyzeDialog::RCPUAnalyzeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(RCPUAnalyzeDialog::IDD, pParent)
{
	m_bPause = false;
	m_bTopMost = false;
	m_pProfileIDToName = NULL;
}

RCPUAnalyzeDialog::~RCPUAnalyzeDialog()
{
	if( NULL != m_pProfileIDToName )
	{
		delete m_pProfileIDToName;
		m_pProfileIDToName = NULL;
	}
}

void RCPUAnalyzeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ProfileTreeListCtrlLeft);
	DDX_Control(pDX, IDC_LIST2, m_ProfileTreeListCtrlRight);
	DDX_Control(pDX, IDC_STATIC_GRAPH, m_GraphCtrl);
	DDX_Control(pDX, IDC_EDIT_YSCALE,m_YScaleCtrl);
}

BOOL RCPUAnalyzeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_ProfileTreeListCtrlLeft.Init();
	m_ProfileTreeListCtrlRight.Init();
	m_GraphCtrl.SetProfileDialog(this);
	m_GraphCtrl.SetInit(true);
	m_YScaleCtrl.SetWindowText("700");	//GraphCtrl에 보면 m_YScale이 700으로 하드코딩되어있음.

	SetWindowText(DEBUGGER_PROFILE_DIALOG_NAME); 

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

BEGIN_MESSAGE_MAP(RCPUAnalyzeDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &RCPUAnalyzeDialog::OnBnClickedButtonPause)
	ON_EN_CHANGE(IDC_EDIT_YSCALE, &RCPUAnalyzeDialog::OnEnChangeEditYscale)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_TOPMOST, &RCPUAnalyzeDialog::OnBnClickedCheckTopmost)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_LEFT, &RCPUAnalyzeDialog::OnBnClickedButtonExportLeft)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_RIGHT, &RCPUAnalyzeDialog::OnBnClickedButtonExportRight)
END_MESSAGE_MAP()





// RCPUAnalyzeDialog 메시지 처리기입니다.

void RCPUAnalyzeDialog::AddProfileInfo( char* pBuffer,int Size )
{
	if( true == m_bPause ) return;

	MBuffer ProfileBuffer;
	ProfileBuffer.SetBuffer(pBuffer,Size);

	CProfileManager* pM = new CProfileManager;
	pM->MakeFromBinary(&ProfileBuffer);

	m_GraphCtrl.AddItem(pM);
}

void RCPUAnalyzeDialog::SetProfileIDToName( char* pBuffer,int Size )
{
	if( true == m_bPause ) return;

	MBuffer ProfileBuffer;
	ProfileBuffer.SetBuffer(pBuffer,Size);

	if( m_pProfileIDToName )
	{
		delete	m_pProfileIDToName;
		m_pProfileIDToName = NULL;
	}

	m_pProfileIDToName = new CProfileIDToName;
	m_pProfileIDToName->MakeFromNameBinary(&ProfileBuffer);
}

void RCPUAnalyzeDialog::SetLeftProfileManager( CProfileManager* pProfileManager )
{
	m_ProfileTreeListCtrlLeft.SetProfileManager(pProfileManager,m_pProfileIDToName);
}

void RCPUAnalyzeDialog::SetRightProfileManager( CProfileManager* pProfileManager )
{
	m_ProfileTreeListCtrlRight.SetProfileManager(pProfileManager,m_pProfileIDToName);
}

void RCPUAnalyzeDialog::OnBnClickedButtonPause()	//Pause버튼 클릭시에
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bPause = !m_bPause;
}

void RCPUAnalyzeDialog::OnEnChangeEditYscale()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString String;
	m_YScaleCtrl.GetWindowText(String);
	int YScale = _ttoi(String);

	m_GraphCtrl.SetYScale(YScale);
}

void RCPUAnalyzeDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	static const int DX = 10;
	static const int DY = 10;
	static const int BASE_Y = 200;

	if( m_GraphCtrl.IsInitialized() )
	{
		CRect PR;
		this->GetClientRect(&PR);

		/*
		// TODO: 그래프가 리사이즈 되면서 오른쪽 콘트롤들을 덮어쓰네요, 
		// 오른쪽에 콘트롤들만큼 공백을 주고 그래프 리사이즈를 만들어야겠습니다.

		m_GraphCtrl.SetWindowPos(NULL,
								PR.left + DX, PR.top + DY,
								(PR.right - DX) - (PR.left + DX ) , (PR.top + BASE_Y - DY) - (PR.top + DY),
								SWP_SHOWWINDOW								);
		*/
	}

	if( true == m_ProfileTreeListCtrlLeft.IsInitialized() )
	{
		CRect PR;
		this->GetClientRect(&PR);

		m_ProfileTreeListCtrlLeft.SetWindowPos(NULL,
			PR.left + DX ,			PR.top + BASE_Y + DY,
			(int)(PR.right * 0.5 - DX) - (PR.left + DX ) ,	(PR.bottom - DY) - (PR.top + BASE_Y + DY) ,
			SWP_SHOWWINDOW);
	}

	if( true == m_ProfileTreeListCtrlRight.IsInitialized() )
	{
		CRect PR;
		this->GetClientRect(&PR);

		m_ProfileTreeListCtrlRight.SetWindowPos(NULL,
			(int)(PR.right * 0.5 + DX) ,			PR.top + BASE_Y + DY,
			(PR.right - DX) - (int)(PR.right * 0.5 + DX ) ,	(PR.bottom - DY) - (PR.top + BASE_Y + DY) ,
			SWP_SHOWWINDOW);
	}

}

void RCPUAnalyzeDialog::OnBnClickedCheckTopmost()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bTopMost = !m_bTopMost;
	if( m_bTopMost )
	{
		SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE | SWP_NOMOVE);
	}
	else
	{
		SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	}
}

void RCPUAnalyzeDialog::ExportToFile( const char* szFileName, CCPUAnalyzeTreeListCtrl& listCtrl )
{
	FILE* fp;
	fopen_s(&fp, szFileName, "w+" );

	if(fp)
	{
		listCtrl.ExportToFile(fp);

		fclose(fp);

		ShellExecute( m_hWnd, _T("open"), szFileName, NULL, NULL, SW_SHOW );
	}
	else
	{
		MessageBox("파일을 여는데 실패했습니다.");
	}
}

void RCPUAnalyzeDialog::OnBnClickedButtonExportLeft()
{
	ExportToFile("cpu_result_left.txt", m_ProfileTreeListCtrlLeft);
}

void RCPUAnalyzeDialog::OnBnClickedButtonExportRight()
{
	ExportToFile("cpu_result_right.txt", m_ProfileTreeListCtrlRight);
}
