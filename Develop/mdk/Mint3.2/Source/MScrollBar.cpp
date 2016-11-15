#include "stdafx.h"
#include "MScrollBar.h"
#include "MColorTable.h"



namespace mint3
{



// MScrollBar
MScrollBar::MScrollBar( const char* szName, MWidget* pParent, MListener* pListener, int nStyle)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MScrollBarLook);

	SetEnableFocus( true);

	m_nStyle		= nStyle;
	m_nValue		= 0;
	m_nRangeMin		= 0;
	m_nRangeMax		= 0;
	m_nPushedRgn	= MSBP_NONE;
	m_nThumbLength	= 0;
}


// GetButtonState
const char* MScrollBar::GetButtonState( int button)
{
	if ( IsEnabled() == false)				return MWS_DISABLE;
	else if ( GetPushedRgn() == button)		return MWS_DOWN;
	return MWS_NORMAL;
}


// GetButtonStateNum
int MScrollBar::GetButtonStateNum( int button)
{
	if ( IsEnabled() == false)				return MWSN_DISABLE;
	else if ( GetPushedRgn() == button)		return MWSN_DOWN;
	return MWSN_NORMAL;
}


// SetScrollValue
int MScrollBar::SetScrollValue( int nValue)
{
	if ( nValue < m_nRangeMin)			nValue = m_nRangeMin;
	else if ( nValue > m_nRangeMax)		nValue = m_nRangeMax;

	m_nValue = nValue;

	return m_nValue;
}


// SetScrollValueFromCursor
void MScrollBar::SetScrollValueFromCursor( MPOINT point)
{
	MRECT r = GetClientRect();

	int width = GetWidth();
	int thumb_len = GetThumbLength();
	int length = ( IsHorz() ? r.w : r.h) - width - (thumb_len >> 1);
	if ( IsShowArrow())		length -= width << 1;

	int nValue;
	if ( IsHorz())	nValue = (int)( (float)((point.x - (thumb_len >> 1) - r.x - (IsShowArrow() ? width : 0)) * (m_nRangeMax - m_nRangeMin)) / (float)length + 0.5f) + m_nRangeMin;
	else			nValue = (int)( (float)((point.y - (thumb_len >> 1) - r.y - (IsShowArrow() ? width : 0)) * (m_nRangeMax - m_nRangeMin)) / (float)length + 0.5f) + m_nRangeMin;

	if ( m_nValue != nValue)
	{
		SetScrollValue( nValue);

		if ( IsAligned() == false)		OnValueChanged();
	}
}


// SetScrollRange
void MScrollBar::SetScrollRange( int min, int max)
{
	if ( min > max)		return;

	m_nRangeMin = min;
	m_nRangeMax = max;

	if ( m_nValue < min)			m_nValue = m_nRangeMin;
	else if ( m_nValue > max)		m_nValue = m_nRangeMax;

	if ( IsAlwaysShow() == false  &&  IsAlwaysHide() == false)
	{
		if ( m_nRangeMin == m_nRangeMax)	Show( false);
		else								Show( true);
	}
}


// GetScrollRange
void MScrollBar::GetScrollRange( int* min, int* max)
{
	*min = m_nRangeMin;
	*max = m_nRangeMax;
}


// SetAlwaysShow
void MScrollBar::SetAlwaysShow( bool bShow)
{
	if ( bShow == true)
	{
		m_nStyle &= ~MSBS_ALWAYSHIDE;
		m_nStyle |= MSBS_ALWAYSSHOW;

		Show( true);
	}
	else
	{
		m_nStyle &= ~MSBS_ALWAYSSHOW;

		if ( m_nRangeMin == m_nRangeMax)	Show( false);
		else								Show( true);
	}
}


// SetAlwaysHide
void MScrollBar::SetAlwaysHide( bool bHide)
{
	if ( bHide == true)
	{
		m_nStyle &= ~MSBS_ALWAYSSHOW;
		m_nStyle |= MSBS_ALWAYSHIDE;

		Show( false);
	}
	else
	{
		m_nStyle &= ~MSBS_ALWAYSHIDE;

		if ( m_nRangeMin == m_nRangeMax)	Show( false);
		else								Show( true);
	}
}


// GetLeftArrowRect
MRECT MScrollBar::GetLeftArrowRect()
{
	MRECT rect = GetClientRect();
	rect.w = GetWidth();
	return rect;
}


// GetRightArrowRect
MRECT MScrollBar::GetRightArrowRect()
{
	int width = GetWidth();

	MRECT rect = GetClientRect();
	rect.x += rect.w - width;
	rect.w = width;

	return rect;
}


// GetUpArrowRect
MRECT MScrollBar::GetUpArrowRect()
{
	MRECT rect = GetClientRect();
	rect.h = GetWidth();
	return rect;
}


// GetDownArrowRect
MRECT MScrollBar::GetDownArrowRect()
{
	int width = GetWidth();

	MRECT rect = GetClientRect();
	rect.y += rect.h - width;
	rect.h = width;
	return rect;
}


// GetThumbRect
MRECT MScrollBar::GetThumbRect()
{
	MRECT r = GetClientRect();

	int width = GetWidth();
	int thumb_len = GetThumbLength();
	int length = (IsHorz() ? r.w : r.h) - thumb_len;
	if ( IsShowArrow())		length -= width << 1;

	MRECT rect;
	float fRatio = (float)( m_nValue - m_nRangeMin) / (float)( m_nRangeMax - m_nRangeMin);
	if ( IsHorz())
	{
		rect.x = r.x + (int)( (float)length * fRatio + 0.5f) + (IsShowArrow() ? width : 0);
		rect.y = r.y;
		rect.w = thumb_len;
		rect.h = r.h;
	}
	else			
	{
		rect.x = r.x;
		rect.y = r.y + (int)( (float)length * fRatio + 0.5f) + (IsShowArrow() ? width : 0);
		rect.w = r.w;
		rect.h = thumb_len;
	}

	return rect;
}


// GetTrackRect
MRECT MScrollBar::GetTrackRect()
{
	MRECT rect = GetClientRect();
	if ( IsShowArrow() == false)	return rect;

	int width = GetWidth();
	if ( IsHorz())
	{
		rect.x += width;
		rect.w -= width << 1;
	}
	else
	{
		rect.y += width;
		rect.h -= width << 1;
	}

	return rect;
}


// GetThumbLength
int MScrollBar::GetThumbLength()
{
	if ( m_nThumbLength > 0)			return m_nThumbLength;

	return GetWidth();
}


// IsFocus
bool MScrollBar::IsFocus()
{
	if ( m_pParent)	return m_pParent->IsFocus();
	return MWidget::IsFocus();
}


// OnEvent
bool MScrollBar::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)		return true;

	switch ( pEvent->nMessage)
	{
		case MWM_LBUTTONDOWN:
		{
			m_nPushedRgn = MSBP_NONE;

			if ( IsShowArrow() == true)
			{
				if ( IsHorz())
				{
					if ( GetLeftArrowRect().InPoint( pEvent->Pos))			m_nPushedRgn = MSBP_LEFTARROW;
					else if ( GetRightArrowRect().InPoint( pEvent->Pos))	m_nPushedRgn = MSBP_RIGHTARROW;
				}

				else
				{
					if ( GetUpArrowRect().InPoint( pEvent->Pos))			m_nPushedRgn = MSBP_UPARROW;
					else if ( GetDownArrowRect().InPoint( pEvent->Pos))		m_nPushedRgn = MSBP_DOWNARROW;
				}
			}

			if ( m_nPushedRgn == MSBP_NONE)
			{
				if ( GetThumbRect().InPoint( pEvent->Pos))					m_nPushedRgn = MSBP_THUMB;
				else														m_nPushedRgn = MSBP_TRACK;
			}

			m_ptCursor = pEvent->Pos;
			m_dwTimer = timeGetTime() - 1000;

			break;
		}

		case MWM_LBUTTONUP:
			m_nPushedRgn = MSBP_NONE;
			break;

		case MWM_MOUSEMOVE:
			m_ptCursor = pEvent->Pos;
			break;
	}


	return false;
}


// OnRun
void MScrollBar::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	if ( IsVisibled() == false)		return;

	// 20ms마다 업데이트
	DWORD curr = timeGetTime();
	DWORD diff = curr - m_dwTimer;
	if ( diff < 20)				return;
	m_dwTimer = curr;

	switch ( m_nPushedRgn)
	{
		case MSBP_NONE :
			break;

		case MSBP_UPARROW :
		case MSBP_LEFTARROW :
		{
			m_nValue--;
			if ( m_nValue < m_nRangeMin)
			{
				m_nValue = m_nRangeMin;
				break;
			}

			MListener* pListener = GetListener();
			if ( pListener)		pListener->OnCommand( this, MEVENT_VALUECHANGED);

			break;
		}

		case MSBP_DOWNARROW :
		case MSBP_RIGHTARROW :
		{
			m_nValue++;
			if ( m_nValue > m_nRangeMax)
			{
				m_nValue = m_nRangeMax;
				break;
			}

			MListener* pListener = GetListener();
			if ( pListener)		pListener->OnCommand( this, MEVENT_VALUECHANGED);

			break;
		}
		

		case MSBP_THUMB :
		case MSBP_TRACK :
		{
			SetScrollValueFromCursor( m_ptCursor);

			MListener* pListener = GetListener();
			if ( pListener)		pListener->OnCommand( this, MEVENT_VALUECHANGED);

			break;
		}
	}


	if ( IsAligned() == true  &&  GetShow() == true)
		CalculateAlignRect();
}


// OnSetCursor
bool MScrollBar::OnSetCursor( MEvent* pEvent, string* pCursor)
{
	if ( IsEnabled() == true  &&  m_bStatic == false  &&  m_nRangeMin < m_nRangeMax)
		*pCursor = MCURSOR_HAND;

	return MWidget::OnSetCursor( pEvent, pCursor);
}


// CalculateAlignRect
void MScrollBar::CalculateAlignRect()
{
	if ( IsAligned()  &&  m_pParent != NULL)
	{
		int width = GetWidth();
		MRECT r = m_pParent->GetClientRect();
		if ( IsHorz())
		{
			if ( m_nStyle & MSBS_TOPALIGN)		SetRect( 0, -width, r.w, width);
			else /* Bottom align */				SetRect( 0, r.h, r.w, width);
		}
		else /* Vertical */
		{
			if ( m_nStyle & MSBS_LEFTALIGN)		SetRect( -width, 0, width, r.h);
			else /* Right align */				SetRect( r.w, 0, width, r.h);
		}
	}
}


// glueSetScrollRange
MWLua::ret_int2 MScrollBar::glueSetScrollRange( int _min, int _max)
{
	SetScrollRange( _min, _max);

	return MWLua::ret_int2( m_nRangeMin, m_nRangeMax);
}
















// MScrollBarLook
MScrollBarLook::MScrollBarLook()
{
	m_nWidth = 15;
}

void MScrollBarLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
}

void MScrollBarLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	MScrollBar* pScrollBar = (MScrollBar*)pWidget;

	// Draw track
	OnDrawTrack( pScrollBar, pScrollBar->GetTrackRect(), pDC);

	if ( pScrollBar->IsShowArrow())
	{
		if ( pScrollBar->IsHorz())
		{
			OnDrawLeftArrow( pScrollBar, pScrollBar->GetLeftArrowRect(), pDC);
			OnDrawRightArrow( pScrollBar, pScrollBar->GetRightArrowRect(), pDC);
		}
		else
		{
			OnDrawUpArrow( pScrollBar, pScrollBar->GetUpArrowRect(), pDC);
			OnDrawDownArrow( pScrollBar, pScrollBar->GetDownArrowRect(), pDC);
		}
	}

	// Draw thumb
	if ( pScrollBar->GetMin() != pScrollBar->GetMax())
		OnDrawThumb( pScrollBar, pScrollBar->GetThumbRect(), pDC);
}

void MScrollBarLook::OnDrawLeftArrow( MScrollBar* pScrollbar, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_LEFTARROW, pScrollbar->GetButtonState( MSBP_LEFTARROW)) == false)
	{
		// Draw default
		int state = pScrollbar->GetButtonStateNum( MSBP_LEFTARROW);
		pDC->SetColor( MColorTable::FACE[ state]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ state]);
		pDC->Rectangle( r);
	}
}

void MScrollBarLook::OnDrawRightArrow( MScrollBar* pScrollbar, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_RIGHTARROW, pScrollbar->GetButtonState( MSBP_RIGHTARROW)) == false)
	{
		// Draw default
		int state = pScrollbar->GetButtonStateNum( MSBP_RIGHTARROW);
		pDC->SetColor( MColorTable::FACE[ state]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ state]);
		pDC->Rectangle( r);
	}
}

void MScrollBarLook::OnDrawUpArrow( MScrollBar* pScrollbar, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_UPARROW, pScrollbar->GetButtonState( MSBP_UPARROW)) == false)
	{
		// Draw default
		int state = pScrollbar->GetButtonStateNum( MSBP_UPARROW);
		pDC->SetColor( MColorTable::FACE[ state]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ state]);
		pDC->Rectangle( r);
	}
}

void MScrollBarLook::OnDrawDownArrow( MScrollBar* pScrollbar, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_DOWNARROW, pScrollbar->GetButtonState( MSBP_DOWNARROW)) == false)
	{
		// Draw default
		int state = pScrollbar->GetButtonStateNum( MSBP_DOWNARROW);
		pDC->SetColor( MColorTable::FACE[ state]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ state]);
		pDC->Rectangle( r);
	}
}

void MScrollBarLook::OnDrawThumb( MScrollBar* pScrollbar, MRECT& r, MDrawContext* pDC)
{
	if ( Draw( pDC, r, MWP_THUMB, pScrollbar->GetButtonState( MSBP_THUMB)) == false)
	{
		// Draw default
		int state = pScrollbar->GetButtonStateNum( MSBP_THUMB);
		pDC->SetColor( MColorTable::FACE[ state]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ state]);
		pDC->Rectangle( r);
	}
}

void MScrollBarLook::OnDrawTrack( MScrollBar* pScrollbar, MRECT& r, MDrawContext* pDC)
{
	const char* part = pScrollbar->IsHorz() ? MWP_HORZTRACK : MWP_VERTTRACK;

	if ( Draw( pDC, r, part, pScrollbar->GetButtonState( MSBP_TRACK)) == false)
	{
		// Draw default
		int state = pScrollbar->GetButtonStateNum( MSBP_TRACK);
		pDC->SetColor( MColorTable::BORDER[ state]);
		pDC->Rectangle( r);
	}
}





} // namespace mint3
