#include "stdafx.h"
#include "MLinePrinter.h"
#include "MScrollBar.h"
#include "Mint.h"
#include "MColorTable.h"
#include <math.h>


namespace mint3
{


#define DEFAULT_ITEMHEIGHT			17
#define _prev_(_n,_cap)				(_n = (_n == 0) ? _cap : _n-1)
#define _next_(_n,_cap)				(_n = (_n == _cap) ? 0 : _n+1)
#define _pos_(_start,_count,_cap)	(((_start+_count)>_cap) ? (_start+_count-_cap-1) : (_start+_count))
#define _count_(_head,_tail,_cap) 	((_tail>_head) ? (_cap+1-_tail+_head) : (_head-_tail))



// MLinePrinter
MLinePrinter::MLinePrinter( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MLinePrinterLook);

	SetEnableFocus( true);

	m_pScrollBar = new MScrollBar( NULL, this, this, MSBS_VERT | MSBS_RIGHTALIGN);
	m_nCapacity = 256;
	m_StringList = new string[ m_nCapacity + 1];
	m_nHead = m_nTail = 0;
	m_bUpdateScrollBar = false;
}


// ~MLinePrinter
MLinePrinter::~MLinePrinter()
{
	if ( m_StringList != NULL)
	{
		delete [] m_StringList;
		m_StringList = NULL;
	}

	if ( m_pScrollBar)
	{
		delete m_pScrollBar;
		m_pScrollBar = NULL;
	}
}


// AddString
void MLinePrinter::AddString( const char* szString)
{
	if ( szString == NULL  ||  szString[ 0] == 0)	return;

	m_StringList[ m_nHead] = szString;
	_next_( m_nHead, m_nCapacity);

	if ( m_nHead == m_nTail)
	{
		_next_( m_nTail, m_nCapacity);

		if ( m_pScrollBar->GetScrollValue() <  m_pScrollBar->GetMax())
			m_pScrollBar->SetScrollValue( m_pScrollBar->GetScrollValue() - 1);
	}

	UpdateScrollBar();
}



// ResetContent
void MLinePrinter::ResetContent()
{
	if ( m_StringList != NULL)
	{
		for ( size_t i = 0;  i <= m_nCapacity;  i++)
			m_StringList[ i].clear();
	}

	m_nHead = m_nTail = 0;

	m_pScrollBar->SetScrollRange( 0, 0);
	m_pScrollBar->SetScrollValue( 0);
}


// SetCapacity
void MLinePrinter::SetCapacity( size_t nCapacity)
{
	if ( m_nCapacity == nCapacity)	return;
	m_nCapacity = nCapacity;

	if ( m_StringList != NULL)
	{
		delete [] m_StringList;
		m_StringList = NULL;
	}

	m_StringList = new string[ m_nCapacity + 1];
}


// GetItemHeight
int MLinePrinter::GetItemHeight()
{
	int height = 0;
	if ( HasLookAndFill())
	{
		height = ((MLinePrinterLook*)m_pLookAndFill)->GetItemHeight();
		if ( height <= 0)
			height = m_pLookAndFill->GetFont()->GetHeight() + 2;
	}
	if ( height <= 0)	height = DEFAULT_ITEMHEIGHT;

	return height;
}


// ScrollPage
void MLinePrinter::ScrollPage( int delta)
{
	int _value = m_pScrollBar->GetScrollValue() + delta;

	if ( _value > m_pScrollBar->GetMax())	m_pScrollBar->SetScrollValue( m_pScrollBar->GetMax());
	else if ( _value < 0)					m_pScrollBar->SetScrollValue( 0);

	m_pScrollBar->SetScrollValue( _value);
}


// OnEvent
bool MLinePrinter::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)	return true;

	// Translate message
	switch ( pEvent->nMessage)
	{
	case MWM_MOUSEWHEEL :
		ScrollPage( -(int)( (float)(pEvent->nDelta * 0.1f)));
		return true;
	}

	return false;
}


// OnRun
void MLinePrinter::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	// Update scrollbar
	if ( m_bUpdateScrollBar == true)
	{
		m_bUpdateScrollBar = false;

		MRECT r = GetClientRect();
		int _item_height = GetItemHeight();
		int _height = _count_( m_nHead, m_nTail, m_nCapacity) * _item_height;
		if ( _height <= r.h)
		{
			m_pScrollBar->SetScrollRange( 0, 0);
			m_pScrollBar->SetScrollValue( 0);
		}
		else
		{
			int _max = (int)ceil( ((double)_height - (double)r.h) / (double)_item_height);

			int _val = m_pScrollBar->GetScrollValue();
			if ( _val == m_pScrollBar->GetMax())	_val = _max;

			m_pScrollBar->SetScrollRange( 0, _max);
			m_pScrollBar->SetScrollValue( _val);
		}
	}
}


// OnSize
void MLinePrinter::OnSize( int w, int h)
{
	MWidget::OnSize( w, h);

	UpdateScrollBar();
}













// MLinePrinterLook
MLinePrinterLook::MLinePrinterLook()
{
	SetTextAlign( MAM_LEFT | MAM_VCENTER);
	m_nItemHeight = 0;
}

void MLinePrinterLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	OnDrawItems( (MLinePrinter*)pWidget, pDC);
}

void MLinePrinterLook::OnDrawItems( MLinePrinter* pLinePrinter, MDrawContext* pDC)
{
	// Set font
	pDC->SetFont( GetFont());

	// Draw items
	MRECT r = pLinePrinter->GetClientRect();
	int x = 0, y = 0;
	int _item_height = pLinePrinter->GetItemHeight();
	size_t _head = pLinePrinter->GetHead();
	size_t _tail = pLinePrinter->GetTail();
	size_t _cap = pLinePrinter->GetCapacity();
	size_t _start = _pos_( _tail, pLinePrinter->GetScrollBar()->GetScrollValue(), _cap);
	size_t _i = _start;
	for ( size_t i = _start;  i != _head;  _next_(i, _cap))
	{
		MRECT _r( x, y, r.w, _item_height);

		// Draw item
		OnDrawItemText( pLinePrinter, i, _r, pDC);

		y += _item_height;
		if ( y > r.h)	break;

		_i = i;
	}
}

void MLinePrinterLook::OnDrawItemText( MLinePrinter* pLinePrinter, size_t index, MRECT& r, MDrawContext* pDC)
{
	pDC->SetColor( GetFontColor( MWP_BORDER, MWS_NORMAL, MColorTable::TEXT[ MWSN_NORMAL]));
	pDC->Text( r, pLinePrinter->GetItemText( index), GetTextAlign());
}


} // namespace mint3


