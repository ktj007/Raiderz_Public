#include "stdafx.h"
#include "MToolTip.h"
#include "MColorTable.h"
#include "Mint.h"

namespace mint3
{

#define MAX_WIDTH		300
#define MWAIT_TIME		100
#define GAP				5



enum MTOOLTIPSHOWSTATE
{
	MTS_HIDE			= 0,
	MTS_WAITCOUNT,
	MTS_FADEIN,
	MTS_SHOW,
	MTS_FADEOUT
};



IMPLEMENT_LOOK( MTooltip, MTooltipLook)



// MTooltipLook
MTooltipLook::MTooltipLook()
{
	m_ptOffset = MPOINT( 0, -20);
}


// OnDraw
void MTooltipLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	if ( pWidget->IsVisible() == false)
		return;


	MRECT r = pWidget->GetClientRect();

	pDC->SetColor( MColorTable::MTOOLTIP_PLANE);
	pDC->FillRectangle( r);

	pDC->SetColor( MColorTable::MTOOLTIP_OUTLINE);
	pDC->Rectangle( r);
}









// MTooltip
MTooltip::MTooltip( MWidget* pParent) : MWidget( "ToolTip", pParent, NULL)
{
	LOOK_IN_CONSTRUCTOR();

	SetFocusEnable(false);
	SetAnchor( MAnchorStyle( false, false, false, false));
	Show(true);

	SetBounds( 0, 0, MAX_WIDTH, 100);
	SetOpacity( 0.0f);

	m_pSender		= NULL;
	m_nShowState	= MTS_HIDE;


	// Create text view
	MRECT r = GetRect();
	m_pTextView = new MTextView( "ToolTipMsg", this, NULL);
	m_pTextView->SetBounds( r.x + GAP, r.y + GAP, r.w - GAP - GAP, r.h - GAP - GAP);
	m_pTextView->SetAnchor( MAnchorStyle( true, true, true, true));
	m_pTextView->Show( true);
	m_pTextView->Enable( false);
}


MTooltip::~MTooltip(void)
{
}


// OnEvent
bool MTooltip::OnEvent( MEvent* pEvent, MListener* pListener)
{
	return false;
}


// OnRun
void MTooltip::OnRun( void)
{
	static DWORD dwPrevious = timeGetTime();
	DWORD dwCurrent = timeGetTime();
	DWORD dwElapsed = max( 1, timeGetTime() - dwPrevious);
	dwPrevious = dwCurrent;


	switch ( m_nShowState)
	{
	case MTS_HIDE :
		{
			m_pSender = NULL;
			if ( m_strText.empty() == false)
				m_strText.clear();

			if ( IsVisible())
				Show( false);

			SetOpacity( 0.0f);
			break;
		}
	case MTS_WAITCOUNT :
		{
			dwElapsed = dwCurrent - m_dwTimer;
			if ( dwElapsed > MWAIT_TIME)
			{
				SetTextView();
				m_nShowState = MTS_FADEIN;
			}
			break;
		}
	case MTS_FADEIN :
		{
			if ( IsVisible() == false)
				Show( true);

			float opacity = GetOpacity() + (float)( (double)dwElapsed * 0.002);
			if ( opacity >= 1.0f)
			{
				SetOpacity( 1.0f);
				m_nShowState = MTS_SHOW;
			}
			else
				SetOpacity( opacity);
			break;
		}
	case MTS_SHOW :
		{
			SetOpacity( 1.0f);
			break;
		}
	case MTS_FADEOUT :
		{
			float opacity = GetOpacity() - (float)( (double)dwElapsed * 0.002);
			if ( opacity <= 0.0f)
			{
				SetOpacity( 0.0f);
				m_nShowState = MTS_HIDE;
			}
			else
				SetOpacity( opacity);
			break;
		}
	}
}


// SetTooltip
void MTooltip::SetTooltip( MWidget* pSender, MPOINT& pos, const string& strMsg)
{
	// 메시지를 보낸 위젯이 NULL이거나 스트링이 없음
	if ( pSender == NULL  ||  strMsg.empty())
	{
		switch ( m_nShowState)
		{
		case MTS_HIDE :
			break;
		case MTS_WAITCOUNT :
			m_nShowState = MTS_HIDE;
			break;
		case MTS_FADEIN :
		case MTS_SHOW :
			m_nShowState = MTS_FADEOUT;
			break;
		case MTS_FADEOUT :
			break;
		}

		return;
	}

	// 메시지를 보낸 위젯과 스트링이 같음
	else if ( pSender == m_pSender  &&  m_strText.length() == strMsg.length()  &&  m_strText == strMsg)
	{
		switch ( m_nShowState)
		{
		case MTS_HIDE :
		case MTS_WAITCOUNT :
			m_ptPosition = pos;
			break;
		case MTS_FADEIN :
		case MTS_SHOW :
			break;
		case MTS_FADEOUT :
			m_nShowState = MTS_FADEIN;
			break;
		}

		return;
	}

	// 메시지를 보낸 위젯이 다르거나 같은 위젯이 메시지만 변경해서 보냄
	else
	{
		m_pSender = pSender;
		m_ptPosition = pos;
		m_strText = strMsg;

		switch ( m_nShowState)
		{
		case MTS_HIDE :
		case MTS_WAITCOUNT :
			m_nShowState = MTS_WAITCOUNT;
			m_dwTimer = timeGetTime();
			break;
		case MTS_FADEIN :
		case MTS_SHOW :
			SetTextView();
			break;
		case MTS_FADEOUT :
			SetTextView();
			m_nShowState = MTS_FADEIN;
			break;
		}
	}
}


// SetTextView
void MTooltip::SetTextView()
{
	m_pTextView->Clear();
	SetBounds( 0, 0, MAX_WIDTH, 100);
	m_pTextView->SetText( m_strText.c_str());

	MPOINT _offset = m_pCustomLook ? ((MTooltipLook*)m_pCustomLook)->GetOffset() : MPOINT( 0, 0);
	MSIZE _size = m_pTextView->GetPageSize();
	
	SetPosition( m_ptPosition.x + _offset.x, m_ptPosition.y + _offset.y);
	SetSize( _size.w + GAP + GAP, _size.h + GAP + GAP);
}


} // namespace mint3
