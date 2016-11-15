// AnimationDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FrameControlBar.h"
#include "resource.h"
#include "Mcv.h"


// CAniControlBar 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFrameControlBar, CDialogBar)

CFrameControlBar::CFrameControlBar(CWnd* pParent /*=NULL*/)
	: CDialogBar()
	, m_strFrame( "0 / 0 frame")
	, m_strSecond( "0.00 / 0.00 sec")
{

}

CFrameControlBar::~CFrameControlBar()
{
}

LRESULT CFrameControlBar::HandleInitDialog(WPARAM wParam, LPARAM lParam)
{
	OnInitDialog();

	return CDialogBar::HandleInitDialog(wParam, lParam);
}

BOOL CFrameControlBar::OnInitDialog() 
{
	// TODO: Add extra initialization here
	UpdateData( FALSE);

	m_SliderAniSpeed.SetRange(0,10);
	m_SliderAniSpeed.SetPos(5);

	return TRUE;  
}

void CFrameControlBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CFrameControlBar)
	DDX_Control(pDX, IDC_SLIDER_ANIMATION, m_SliderAnimation);
	DDX_Text(pDX, IDC_STATIC_INFO, m_strFrame);
	DDX_Text(pDX, IDC_STATIC_MSECOND, m_strSecond);
	DDX_Control(pDX, IDC_SLIDER_SPEED, m_SliderAniSpeed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrameControlBar, CDialogBar)
	//{{AFX_MSG_MAP(CFrameControlBar)
	ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFrameControlBar::_resize( int nID, int delta, bool left_anchor)
{
	CWnd* pWnd = GetDlgItem( nID);
	if ( pWnd == NULL)
		return;

	CRect rect;
	pWnd->GetWindowRect( &rect);
	ScreenToClient( rect);
	
	if ( left_anchor == false)
		rect.left += delta;
	
	rect.right += delta;

	GetDlgItem( nID)->MoveWindow( rect);
}

void CFrameControlBar::OnSize(UINT nType, int cx, int cy)
{
	CDialogBar::OnSize( nType, cx, cy);

	if ( 1)
	{
		CWnd* pWnd = GetDlgItem( IDC_STATIC_FRAMEGROUP);
		if ( pWnd == NULL)	return;
		CRect rectFrame;
		pWnd->GetWindowRect( &rectFrame);
		ScreenToClient( rectFrame);

		pWnd = GetDlgItem( IDC_STATIC_SPEEDGROUP);
		if ( pWnd == NULL)	return;
		CRect rectSpeed;
		pWnd->GetWindowRect( &rectSpeed);
		ScreenToClient( rectSpeed);

		int dx = max( cx, 500) - rectFrame.left - rectSpeed.right;

		_resize( IDC_STATIC_FRAMEGROUP, dx, true);
		_resize( IDC_SLIDER_ANIMATION, dx, true);
		_resize( IDC_PLAY_PAUSE, dx, false);
		_resize( IDC_STOP, dx, false);

		_resize( IDC_STATIC_SPEEDGROUP, dx, false);
		_resize( IDC_SLIDER_SPEED, dx, false);
		_resize( IDC_STATIC_SPEED, dx, false);
	}
}


void CFrameControlBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if((void*)pScrollBar==(void*)&m_SliderAnimation)
	{
		// 애니메이션 프레임 업데이트

		RActor* pActor = GetMainView()->GetActor();
		if(!pActor) return;

		GetMainView()->PauseAnimation();
		GetMainView()->SetFrame( m_SliderAnimation.GetPos());

		return;
	}

	else if((void*)pScrollBar==(void*)&m_SliderAniSpeed)
	{
		float speed = m_SliderAniSpeed.GetPos() * 0.2f;
		CString str;
		str.Format( "Speed : x%.1f", speed);

		GetDlgItem(IDC_STATIC_SPEED)->SetWindowText(str);

		return;
	}


	CDialogBar::OnHScroll(nSBCode, nPos, pScrollBar);
}
