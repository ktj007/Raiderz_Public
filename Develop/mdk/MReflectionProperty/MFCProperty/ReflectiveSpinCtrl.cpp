#include "StdAfx.h"
#include "ReflectiveSpinCtrl.h"
#include "ReflectiveFloatProperty.h"

/////////////////////////////////////////////////////////////////////////////
// ReflectiveSpinCtrl

float	CReflectiveSpinCtrl::m_fOriginalValue;
CPoint	CReflectiveSpinCtrl::m_ptBeginDrag;
BOOL	CReflectiveSpinCtrl::m_bDrag = FALSE;

CReflectiveSpinCtrl::CReflectiveSpinCtrl(CReflectiveFloatProperty* pProperty)
: m_pProperty( pProperty )
{
}

CReflectiveSpinCtrl::~CReflectiveSpinCtrl()
{
}

//{{AFX_MSG_MAP(ReflectiveSpinCtrl)
BEGIN_MESSAGE_MAP(CReflectiveSpinCtrl, CMFCSpinButtonCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSELEAVE, &CMFCSpinButtonCtrl::OnMouseLeave)
	ON_WM_CAPTURECHANGED()
	ON_WM_TIMER()
END_MESSAGE_MAP()
//}}AFX_MSG_MAP

#define TID_POLLMOUSE 100
#define MOUSE_POLL_DELAY 20

void CReflectiveSpinCtrl::SetCaptureTimer()
{
	SetTimer( TID_POLLMOUSE,MOUSE_POLL_DELAY,NULL);
}

void CReflectiveSpinCtrl::ReleaseCaptureTimer()
{
	KillTimer(TID_POLLMOUSE);
}

void CReflectiveSpinCtrl::OnDraw(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	CRect rectClient;
	GetClientRect(rectClient);

	if (CMFCToolBarImages::m_bIsDrawOnGlass)
	{
		CDrawingManager dm(*pDC);
		dm.DrawRect(rectClient, afxGlobalData.clrWindow, (COLORREF)-1);
	}
	else
	{
		pDC->FillRect(rectClient, &afxGlobalData.brWindow);
	}

	POINT pt;
	GetCursorPos(&pt);

	int nState = 0;

	if( m_ptBeginDrag.y < pt.y )
		nState |= AFX_SPIN_HIGHLIGHTEDDOWN;
	else if ( m_ptBeginDrag.y > pt.y )
	{
		nState |= AFX_SPIN_HIGHLIGHTEDUP;
	}

	if (!IsWindowEnabled())
	{
		nState |= AFX_SPIN_DISABLED;
	}

	CMFCVisualManager::GetInstance()->OnDrawSpinButtons(pDC, rectClient, nState, FALSE, this);
}

void CReflectiveSpinCtrl::OnTimer(UINT_PTR nIDEvent)
{
	POINT pt;
	GetCursorPos(&pt);

	const float FLOAT_STEP = 1.f;

// 	TRACE1( "OnTimer %3.2f\n", m_fLastValue);
	float fNewValue = m_fOriginalValue + FLOAT_STEP * float( m_ptBeginDrag.y - pt.y );
	if( m_fLastValue != fNewValue )
	{
		m_fLastValue = fNewValue;
		variant_t varValue = fNewValue;
		m_pProperty->SetValue( varValue );
		Invalidate();
	}

	SetCaptureTimer();

	if( GetAsyncKeyState(VK_LBUTTON) == false )
	{
		ReleaseCaptureTimer();
		m_bDrag = FALSE;
	}
}

void CReflectiveSpinCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	POINT pt;
	GetCursorPos(&pt);
	m_ptBeginDrag = pt;

	m_bDrag = TRUE;

	variant_t val = m_pProperty->GetValue();
	m_fLastValue = m_fOriginalValue = val;
	
	SetCaptureTimer();

	return __super::OnLButtonDown(nFlags, point);
}