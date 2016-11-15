#include "stdafx.h"
#include "MViewCtrl.h"
#include "MScrollBar.h"
#include "MColorTable.h"



namespace mint3
{



// MViewCtrl
MViewCtrl::MViewCtrl( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	m_pPanel = new MPanel( NULL, this, this);
	m_pPanel->SetRect( 0, 0, m_Rect.w, m_Rect.h);
	m_pPanel->SetAnchor( MAnchorStyle( true, true, true, true));
	m_pPanel->ShowBorder( false);

	m_pScrollBar = new MScrollBar( NULL, this, this, MSBS_VERT | MSBS_RIGHTALIGN);

	m_nMinScrollRange = 0;
	m_nMaxScrollRange = 0;
}


// ~MListBox
MViewCtrl::~MViewCtrl()
{
	if ( m_pPanel)
	{
		delete m_pPanel;
		m_pPanel = NULL;
	}


	if ( m_pScrollBar)
	{
		delete m_pScrollBar;
		m_pScrollBar = NULL;
	}
}


// ScrollPage
void MViewCtrl::ScrollPage( int delta)
{
	int _value = m_pScrollBar->GetScrollValue() + delta;

	if ( _value > m_pScrollBar->GetMax())	m_pScrollBar->SetScrollValue( m_pScrollBar->GetMax());
	else if ( _value < 0)					m_pScrollBar->SetScrollValue( 0);

	m_pScrollBar->SetScrollValue( _value);

	m_pPanel->SetPosition( 0, -m_pScrollBar->GetScrollValue());
}


// SetScrollValue
void MViewCtrl::SetScrollValue( int nValue)
{
	m_pScrollBar->SetScrollValue( nValue);

	m_pPanel->SetPosition( 0, -m_pScrollBar->GetScrollValue());
}


// GetScrollValue
int MViewCtrl::GetScrollValue() const
{
	return m_pScrollBar->GetScrollValue();
}


// SetScrollRange
void MViewCtrl::SetScrollRange( int min, int max)
{
	m_nMinScrollRange = min;
	m_nMaxScrollRange = max;

	MRECT r = GetClientRect();
	if ( max <= r.h)
	{
		m_pScrollBar->SetScrollRange( 0, 0);
		m_pScrollBar->SetScrollValue( 0);
	}
	else
	{
		m_pScrollBar->SetScrollRange( m_nMinScrollRange, m_nMaxScrollRange - r.h);
	}

	m_pPanel->SetRect( 0, -m_pScrollBar->GetScrollValue(), m_Rect.w, m_nMaxScrollRange);
}


// GetScrollRange
void MViewCtrl::GetScrollRange( int* min, int* max)
{
	*min = m_nMinScrollRange;
	*max = m_nMaxScrollRange;
}


// GetScrollBarRange
void MViewCtrl::GetScrollBarRange( int* min, int* max)
{
	m_pScrollBar->GetScrollRange( min, max);
}


// OnEvent
bool MViewCtrl::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)		return true;


	// Translate message
	switch ( pEvent->nMessage)
	{
	case MWM_MOUSEWHEEL :
		ScrollPage( -(int)( (float)(pEvent->nDelta * 0.1f)));
		return true;
	}

	return false;
}


// OnSize
void MViewCtrl::OnSize( int w, int h)
{
	MWidget::OnSize( w, h);


	MRECT r = GetClientRect();
	if ( m_nMaxScrollRange <= r.h)
	{
		m_pScrollBar->SetScrollRange( 0, 0);
		m_pScrollBar->SetScrollValue( 0);
	}
	else
	{
		m_pScrollBar->SetScrollRange( m_nMinScrollRange, m_nMaxScrollRange - r.h);
	}

	m_pPanel->SetPosition( 0, -m_pScrollBar->GetScrollValue());
}



// OnScrollBarChanged
void MViewCtrl::OnScrollBarChanged( int nPos)
{
	m_pPanel->SetPosition( 0, -m_pScrollBar->GetScrollValue());

	MWidget::OnScrollBarChanged( nPos);
}


// glueGetScrollRange
MWLua::ret_int2 MViewCtrl::glueGetScrollRange()
{
	int _min, _max;
	GetScrollRange( &_min, &_max);

	return MWLua::ret_int2( _min, _max);
}


// glueGetScrollBarRange
MWLua::ret_int2 MViewCtrl::glueGetScrollBarRange()
{
	int _min, _max;
	GetScrollBarRange( &_min, &_max);

	return MWLua::ret_int2( _min, _max);
}


} // namespace mint3