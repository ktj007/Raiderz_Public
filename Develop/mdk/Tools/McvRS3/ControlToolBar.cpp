// ControlToolBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ControlToolBar.h"


IMPLEMENT_DYNAMIC(CControlToolBar, CDialogBar)

CControlToolBar::CControlToolBar(CWnd* pParent /*=NULL*/)
	: CDialogBar()
{
	EnableAutomation();
}

CControlToolBar::~CControlToolBar()
{
}

LRESULT CControlToolBar::InitDialog(WPARAM wParam, LPARAM lParam)
{
	if ( !HandleInitDialog(wParam, lParam) || !UpdateData(FALSE))
	{
		TRACE0("Warning: UpdateData failed during dialog init.\n");
		return FALSE;
	}
	return OnInitDialog();
}

BOOL CControlToolBar::OnInitDialog() 
{
	// TODO: Add extra initialization here
	UpdateData(FALSE);

	CTabCtrl* pTab = (CTabCtrl*)GetDlgItem( IDC_TAB_MENU);
	if ( pTab)
	{
		pTab->InsertItem( 0, "Animation");
		pTab->InsertItem( 1, "Model Info");
		pTab->InsertItem( 2, "View Control");
		pTab->InsertItem( 3, "Light");
	}


	m_cAnimationSidebar.Create( IDD_DIALOG_ANIMATION, this);
	m_cAnimationSidebar.ShowWindow( SW_SHOW);

	m_cModelInfoSidebar.Create( IDD_DIALOG_MODELINFO, this);
	m_cModelInfoSidebar.ShowWindow( SW_HIDE);

	m_cViewControlSidebar.Create( IDD_DIALOG_VIEWCONTROL, this);
	m_cViewControlSidebar.ShowWindow( SW_HIDE);

	m_cLightSidebar.Create( IDD_DIALOG_LIGHT, this);
	m_cLightSidebar.ShowWindow( SW_HIDE);

	return TRUE;  
}

LRESULT CControlToolBar::HandleInitDialog(WPARAM wParam, LPARAM lParam)
{
	OnInitDialog();

	return CDialogBar::HandleInitDialog(wParam, lParam);
}

void CControlToolBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CControlToolBar)
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CControlToolBar, CDialogBar)
//{{AFX_MSG_MAP(CControlToolBar)
ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MENU, &CControlToolBar::OnTcnSelchangeTabMenu)
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CControlToolBar::OnTcnSelchangeTabMenu(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CTabCtrl* pTab = (CTabCtrl*)GetDlgItem( IDC_TAB_MENU);
	if ( pTab)
	{
		int nSel = pTab->GetCurSel();
		if ( nSel >= 0)
		{
			m_cAnimationSidebar.ShowWindow( (nSel == 0) ? SW_SHOW : SW_HIDE);
			m_cModelInfoSidebar.ShowWindow( (nSel == 1) ? SW_SHOW : SW_HIDE);
			m_cViewControlSidebar.ShowWindow( (nSel == 2) ? SW_SHOW : SW_HIDE);
			m_cLightSidebar.ShowWindow( (nSel == 3) ? SW_SHOW : SW_HIDE);
		}
	}

	*pResult = 0;
}

void CControlToolBar::OnSize(UINT nType, int cx, int cy)
{
	CDialogBar::OnSize( nType, cx, cy);

	if ( 1)
	{
		CRect rectDlg;
		GetClientRect( &rectDlg);

		rectDlg.top += 50;

		if ( m_cAnimationSidebar)
			m_cAnimationSidebar.MoveWindow( rectDlg);
		if ( m_cModelInfoSidebar)
			m_cModelInfoSidebar.MoveWindow( rectDlg);
		if ( m_cViewControlSidebar)
			m_cViewControlSidebar.MoveWindow( rectDlg);
		if ( m_cLightSidebar)
			m_cLightSidebar.MoveWindow( rectDlg);
	}
}