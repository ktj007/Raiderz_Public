#include "stdafx.h"
#include "MSpinCtrl.h"
#include "MColorTable.h"
#include "MEdit.h"


namespace mint3
{



// MSpinCtrl
MSpinCtrl::MSpinCtrl( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MSpinCtrlLook);

	SetEnableFocus( true);

	m_nType = MSPT_VERTICAL;
	m_nMouseOver = MSPP_NONE;
	m_nBtnPressed = MSPP_NONE;
	m_pBuddy = NULL;
}


// GetButtonState
const char* MSpinCtrl::GetButtonState( int button)
{
	if ( IsEnabled() == false)
		return MWS_DISABLE;

	else if ( m_pParent == NULL  ||  m_pParent->IsEnabled() == false)
		return MWS_DISABLE;

	else if ( m_nBtnPressed == button  &&  m_nMouseOver == button)
		return MWS_DOWN;

	else if ( m_nMouseOver == button)
		return MWS_FOCUS;

	return MWS_NORMAL;
}


// SetBuddy
void MSpinCtrl::SetBuddy( MWidget* pBuddy)
{
	if ( strcmp( pBuddy->GetClassName(), MINT_EDIT) != 0)
	{
		m_pBuddy = NULL;
		return;
	}

	m_pBuddy = (MEdit*)pBuddy;
	m_pBuddy->SetNumber( true);
	m_pBuddy->SetText( "0");
	m_pBuddy->SetSel( 0, m_pBuddy->GetLength());
	m_pBuddy->OnValueChanged();
}


// GetUpArrowRgn
MRECT MSpinCtrl::GetUpArrowRgn()
{
	MRECT r = GetClientRect();
	r.h = r.h >> 1;
	return r;
}


// GetDownArrowRgn
MRECT MSpinCtrl::GetDownArrowRgn()
{
	MRECT r = GetClientRect();
	r.y += r.h >> 1;
	r.h = r.h - (r.h >> 1);
	return r;
}


// GetLeftArrowRgn
MRECT MSpinCtrl::GetLeftArrowRgn()
{
	MRECT r = GetClientRect();
	r.w = r.w >> 1;
	return r;
}


// GetRightArrowRgn
MRECT MSpinCtrl::GetRightArrowRgn()
{
	MRECT r = GetClientRect();
	r.x += r.w >> 1;
	r.w = r.w - (r.w >> 1);

	return r;
}


// OnEvent
bool MSpinCtrl::OnEvent(MEvent* pEvent, MListener* pListener)
{
	if ( m_pBuddy == NULL  ||  m_pParent == NULL  ||  m_pParent->IsEnabled() == false)
	{
		m_nMouseOver = MSPP_NONE;
		m_nBtnPressed = MSPP_NONE;
		return false;
	}


	if ( MWidget::OnEvent( pEvent, pListener) == true)		return true;


	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN:
	case MWM_LBUTTONDBLCLK:
		{
			if ( m_nMouseOver == MSPP_UPARROW  ||  m_nMouseOver == MSPP_RIGHTARROW)
			{
				int num = atoi( m_pBuddy->GetText()) + 1;

				char szTmp[ 256];
				_itoa( num, szTmp, 10);
				m_pBuddy->SetText( szTmp);
				m_pBuddy->SetSel( 0, m_pBuddy->GetLength());
				m_pBuddy->OnValueChanged();
			}
			else if ( m_nMouseOver == MSPP_DOWNARROW  ||  m_nMouseOver == MSPP_LEFTARROW)
			{
				int num = atoi( m_pBuddy->GetText()) - 1;
				if ( num < 0)	num = 0;

				char szTmp[ 256];
				_itoa( num, szTmp, 10);
				m_pBuddy->SetText( szTmp);
				m_pBuddy->SetSel( 0, m_pBuddy->GetLength());
				m_pBuddy->OnValueChanged();
			}

			m_nBtnPressed = m_nMouseOver;
			m_ptCursorPos = pEvent->Pos;
			m_dwTimer = timeGetTime();
			break;
		}

	case MWM_LBUTTONUP :
		m_nBtnPressed = MSPP_NONE;
		break;

	case MWM_MOUSEMOVE:
		{
			if ( m_nType == MSPT_VERTICAL)
			{
				if ( GetUpArrowRgn().InPoint( pEvent->Pos))				m_nMouseOver = MSPP_UPARROW;
				else if ( GetDownArrowRgn().InPoint( pEvent->Pos))		m_nMouseOver = MSPP_DOWNARROW;
				else													m_nMouseOver = MSPP_NONE;
			}
			else
			{
				if ( GetLeftArrowRgn().InPoint( pEvent->Pos))			m_nMouseOver = MSPP_LEFTARROW;
				else if ( GetRightArrowRgn().InPoint( pEvent->Pos))		m_nMouseOver = MSPP_RIGHTARROW;
				else													m_nMouseOver = MSPP_NONE;
			}

			m_ptCursorPos = pEvent->Pos;
			break;
		}

	default :
		break;
	}

	return false;
}


// OnRun
void MSpinCtrl::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	if ( m_pBuddy == NULL)		return;


	if ( m_bLButtonDown == true  &&  m_nBtnPressed > MSPP_NONE  &&  m_nBtnPressed == m_nMouseOver)
	{
		DWORD curr = timeGetTime();
		DWORD diff = curr - m_dwTimer;

		// 250ms마다 수행
		if ( diff < 250)			return;
		m_dwTimer = curr;


		switch ( m_nBtnPressed)
		{
		case MSPP_NONE :
			break;

		case MSPP_UPARROW :
		case MSPP_RIGHTARROW:
			{
				MListener* pListener = GetListener();
				if ( pListener)		pListener->OnCommand( this, MEVENT_VALUECHANGED);

				int num = atoi( m_pBuddy->GetText()) + 1;
				char szTmp[ 256];
				_itoa( num, szTmp, 10);
				m_pBuddy->SetText( szTmp);
				m_pBuddy->SetSel( 0, m_pBuddy->GetLength());
				m_pBuddy->OnValueChanged();
				break;
			}

		case MSPP_DOWNARROW :
		case MSPP_LEFTARROW:
			{
				MListener* pListener = GetListener();
				if ( pListener)		pListener->OnCommand( this, MEVENT_VALUECHANGED);

				int num = max( atoi( m_pBuddy->GetText()) - 1, 0);
				char szTmp[ 256];
				_itoa( num, szTmp, 10);
				m_pBuddy->SetText( szTmp);
				m_pBuddy->SetSel( 0, m_pBuddy->GetLength());
				m_pBuddy->OnValueChanged();
				break;
			}
		}
	}


	// 부모 위젯의 크기에 맞춰서 사이즈를 변경한다. 안좋은 위치...
	MRECT r = m_pBuddy->GetRect();
	if ( m_nType == MSPT_VERTICAL)		m_Rect = MRECT( r.x + r.w, r.y, GetWidth(), r.h);
	else								m_Rect = MRECT( r.x, r.y + r.h, r.w, GetWidth());
}


// OnSetFocus
void MSpinCtrl::OnSetFocus()
{
	if ( m_pBuddy)
		m_pBuddy->SetFocus();
}


// OnLeave
void MSpinCtrl::OnLeave()
{
	MWidget::OnLeave();

	m_nMouseOver = MSPP_NONE;
}


// OnSetCursor
bool MSpinCtrl::OnSetCursor( MEvent* pEvent, string* pCursor)
{
	if ( IsEnabled() == true  &&  m_bStatic == false  &&  m_nMouseOver > MSPP_NONE)
		*pCursor = MCURSOR_HAND;

	return MWidget::OnSetCursor( pEvent, pCursor);
}












// MSpinCtrlLook
MSpinCtrlLook::MSpinCtrlLook()
{
	m_nWidth = 15;
}

void MSpinCtrlLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
}

void MSpinCtrlLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	if ( pWidget == NULL)		return;
	MSpinCtrl* pSpin = (MSpinCtrl*)pWidget;


	if ( pSpin->GetType() == MSPT_VERTICAL)
	{
		OnDrawUpArrow( pSpin, pSpin->GetUpArrowRgn(), pDC);
		OnDrawDownArrow( pSpin, pSpin->GetDownArrowRgn(), pDC);
	}
	else
	{
		OnDrawLeftArrow( pSpin, pSpin->GetLeftArrowRgn(), pDC);
		OnDrawRightArrow( pSpin, pSpin->GetRightArrowRgn(), pDC);
	}
}

void MSpinCtrlLook::OnDrawUpArrow( MSpinCtrl* pSpin, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_UPARROW, pSpin->GetButtonState( MSPP_UPARROW)) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::FACE[ pSpin->GetStateNum()]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ pSpin->GetStateNum()]);
		pDC->Rectangle( r);
	}
}

void MSpinCtrlLook::OnDrawDownArrow( MSpinCtrl* pSpin, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_DOWNARROW, pSpin->GetButtonState( MSPP_DOWNARROW)) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::FACE[ pSpin->GetStateNum()]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ pSpin->GetStateNum()]);
		pDC->Rectangle( r);
	}
}

void MSpinCtrlLook::OnDrawLeftArrow( MSpinCtrl* pSpin, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_LEFTARROW, pSpin->GetButtonState( MSPP_LEFTARROW)) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::FACE[ pSpin->GetStateNum()]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ pSpin->GetStateNum()]);
		pDC->Rectangle( r);
	}
}

void MSpinCtrlLook::OnDrawRightArrow( MSpinCtrl* pSpin, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_RIGHTARROW, pSpin->GetButtonState( MSPP_RIGHTARROW)) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::FACE[ pSpin->GetStateNum()]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ pSpin->GetStateNum()]);
		pDC->Rectangle( r);
	}
}


} // namespace mint3


