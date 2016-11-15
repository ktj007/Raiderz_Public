// EventDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EventDlg.h"

// CEventDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEventDlg, CDialog)

CEventDlg::CEventDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventDlg::IDD, pParent)
	, m_nUncondition(0)
	, m_nMaxFrame( 100)
{
}

CEventDlg::~CEventDlg()
{
}

void CEventDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_MSG_MAP(CEventDlg)
	DDX_Text(pDX, IDC_EDIT_EVENT_FRAME, m_strFrame);
	DDX_CBIndex(pDX, IDC_COMBO_EVENTNAME, m_nName);
	DDX_Text(pDX, IDC_EDIT_EVENT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_EVENT_PARAM1, m_strParam1);
	DDX_Text(pDX, IDC_EDIT_EVENT_PARAM2, m_strParam2);
	DDX_Text(pDX, IDC_EDIT_EVENT_PARAM3, m_strParam3);
	DDX_CBIndex(pDX, IDC_COMBO_EVENT_UNCOND, m_nUncondition);
	//}}AFX_MSG_MAP
}


BEGIN_MESSAGE_MAP(CEventDlg, CDialog)
	//{{AFX_MSG_MAP(CAnimationSidebar)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_EVENTNAME, &CEventDlg::OnCbnSelchangeComboEventname)
END_MESSAGE_MAP()


// CEventDlg 메시지 처리기입니다.
BOOL CEventDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_FRAME);
	pSpin->SetRange( 0, m_nMaxFrame);


	CComboBox* pCombo = (CComboBox*)GetDlgItem( IDC_COMBO_EVENTNAME);
	if ( pCombo)
	{
		int index;
		for ( index = 0;  index < pCombo->GetCount();  index++)
		{
			CString str;
			pCombo->GetLBText( index, str);

			str.MakeLower();
			str.Insert( 0, GetMainView()->GetProjectType()->GetPrefix());

			if ( m_strName == str)
			{
				pCombo->SetCurSel( index);
				break;
			}
		}

		if ( index == pCombo->GetCount())
			pCombo->SetCurSel( 0);
	}

	UpdateEventName();

	return TRUE;
}

void CEventDlg::UpdateEventName()
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem( IDC_COMBO_EVENTNAME);
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_EDIT_EVENT_NAME);
	if ( pCombo  &&  pEdit)
	{
		pEdit->EnableWindow( (pCombo->GetCurSel() > 0) ? FALSE : TRUE);
		UpdateData( FALSE);
	}
}

void CEventDlg::SetFrame( int nFrame)
{
	m_strFrame.Format( "%d", nFrame);
}

void CEventDlg::SetMaxFrame( int nMaxFrame)
{
	m_nMaxFrame = nMaxFrame;
}

void CEventDlg::SetName( const char* szName)
{
	m_strName = szName;
}

void CEventDlg::SetParam1( const char* szParam)
{
	m_strParam1 = szParam;
}

void CEventDlg::SetParam2( const char* szParam)
{
	m_strParam2 = szParam;
}

void CEventDlg::SetParam3( const char* szParam)
{
	m_strParam3 = szParam;
}

void CEventDlg::SetUncondition( bool bSet)
{
	if ( bSet == true)
		m_nUncondition = 1;
	else
		m_nUncondition = 0;
}

int CEventDlg::GetFrame()
{
	int frame = atoi( m_strFrame);
	return frame;
}

const char* CEventDlg::GetName()
{
	return m_strName.GetBuffer();
}

const char* CEventDlg::GetParam1()
{
	return m_strParam1.GetBuffer();
}

const char* CEventDlg::GetParam2()
{
	return m_strParam2.GetBuffer();
}

const char* CEventDlg::GetParam3()
{
	return m_strParam3.GetBuffer();
}

bool CEventDlg::GetUncondition()
{
	if ( m_nUncondition == 1)
		return true;
	else
		return false;
}

void CEventDlg::OnCbnSelchangeComboEventname()
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem( IDC_COMBO_EVENTNAME);
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_EDIT_EVENT_NAME);
	if ( pCombo->GetCurSel() > 0)
	{
		CString str;
		pCombo->GetLBText( pCombo->GetCurSel(), str);

		str.MakeLower();
		str.Insert( 0, GetMainView()->GetProjectType()->GetPrefix());

		m_strName = str;
	}
	else
		m_strName = "";

	UpdateEventName();
}
