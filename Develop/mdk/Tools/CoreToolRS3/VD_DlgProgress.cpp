// VD_DlgProgress.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VD_DlgProgress.h"

// CDlgProgress 대화 상자입니다.
IMPLEMENT_DYNAMIC(CDlgProgress, CDialog)
CDlgProgress::CDlgProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProgress::IDD, pParent)
{
}

CDlgProgress::~CDlgProgress()
{
}

void CDlgProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
}


BEGIN_MESSAGE_MAP(CDlgProgress, CDialog)
END_MESSAGE_MAP()


// CDlgProgress 메시지 처리기입니다.

void CDlgProgress::SetRange(DWORD start, DWORD end)
{
	m_Progress.SetRange((short)start, (short)end);
}

void CDlgProgress::SetStep(int value)
{
	m_Progress.SetStep(value);
}

void CDlgProgress::StepIt()
{
	if ( this )
		m_Progress.StepIt();
}

void CDlgProgress::SetPos(DWORD pos)
{
	m_Progress.SetPos(pos);
}

void CDlgProgress::ShowProgress()
{
	RECT rc;
	AfxGetMainWnd()->GetClientRect(&rc);
	ClientToScreen(&rc);
	/*int startX = rc.left + (rc.right - rc.left)/2 - 200;
	int startY = rc.top + (rc.bottom - rc.top)/2 - 50;*/
	int startX = rc.right - 550;
	int startY = rc.bottom - 60;

	SetWindowPos(NULL, startX, startY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	ShowWindow(SW_SHOW);
}