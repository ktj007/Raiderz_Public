#include "stdafx.h"
#include "MTextFrame.h"
#include "MScrollBar.h"
#include "Mint.h"
#include "MColorTable.h"
#include <math.h>

namespace mint3
{


#define MTITLEBAR_HEIGHT			18
#define RESIZEBOX_SIZE				5
#define MAGNETDIST					10
#define DEFAULT_ITEMHEIGHT			17


#define _prev_(_n,_cap)				(_n = (_n == 0) ? _cap : _n-1)
#define _next_(_n,_cap)				(_n = (_n == _cap) ? 0 : _n+1)
#define _pos_(_start,_count,_cap)	(((_start+_count)>_cap) ? (_start+_count-_cap-1) : (_start+_count))
#define _count_(_head,_tail,_cap) 	((_tail>_head) ? (_cap+1-_tail+_head) : (_head-_tail))



// MTextFrame
MTextFrame::MTextFrame( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MTextFrameLook);

	SetEnableFocus( true);

	m_bZOrderChangable = true;
	m_bMovable = true;
	m_bMoving = false;
	m_bResizable = false;
	m_nResizing = 0;
	m_bShowTitleBar = false;
	m_tShowTitleBar.SetArrivalTime( 3000);

	m_pScrollBar = new MScrollBar( NULL, this, this, MSBS_VERT | MSBS_RIGHTALIGN);
	m_nCapacity = 256;
	m_StringList = new string[ m_nCapacity + 1];
	m_nHead = m_nTail = 0;
	m_bUpdateScrollBar = false;

	m_pCloseBtn = new MButton( NULL, this, this);
	m_pCloseBtn->SetRect( m_Rect.w - 25, 3, 20, 11);
	m_pCloseBtn->SetAnchor( MAnchorStyle( false, true, true, false));
	m_pCloseBtn->SetText( "-");
	m_pCloseBtn->Show( false);
}


// ~MTextFrame
MTextFrame::~MTextFrame()
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

	if ( m_pCloseBtn)
	{
		delete m_pCloseBtn;
		m_pCloseBtn = NULL;
	}
}


// IsFocus
bool MTextFrame::IsFocus()
{
	MWidget* pWidget = MWidget::m_pFocusedWidget;

	while ( pWidget != NULL)
	{
		if ( pWidget == this)	return true;
		pWidget = pWidget->GetParent();
	}

	return false;
}


// AddString
void MTextFrame::AddString( const char* szString)
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
void MTextFrame::ResetContent()
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
void MTextFrame::SetCapacity( size_t nCapacity)
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
int MTextFrame::GetItemHeight()
{
	int height = 0;
	if ( HasLookAndFill())
	{
		height = ((MTextFrameLook*)m_pLookAndFill)->GetItemHeight();
		if ( height <= 0)
			height = m_pLookAndFill->GetFont()->GetHeight() + 2;
	}
	if ( height <= 0)	height = DEFAULT_ITEMHEIGHT;

	return height;
}


// ScrollPage
void MTextFrame::ScrollPage( int delta)
{
	int _value = m_pScrollBar->GetScrollValue() + delta;

	if ( _value > m_pScrollBar->GetMax())	m_pScrollBar->SetScrollValue( m_pScrollBar->GetMax());
	else if ( _value < 0)					m_pScrollBar->SetScrollValue( 0);

	m_pScrollBar->SetScrollValue( _value);
}


// EnsureTitlebar
void MTextFrame::EnsureTitlebar( bool bShow)
{
	m_bShowTitleBar = bShow;
	m_pCloseBtn->Show( bShow);

	if ( bShow == true)
	{
		m_pCloseBtn->SetRect( m_Rect.w - 30 - (m_pScrollBar->GetShow() ? m_pScrollBar->GetWidth() : 0), 3, 20, 11);
		m_tShowTitleBar.Start();
	}
}


// InitMouseState
void MTextFrame::InitMouseState()
{
	MWidget::InitMouseState();

	m_bMoving = false;
	m_nResizing = 0;
}


// OnCommand
bool MTextFrame::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if ( pWidget == m_pCloseBtn  &&  nMsg == MEVENT_CLICK)
	{
		Show( false);
		EnsureTitlebar( false);
		return true;
	}

	return false;
}


// OnEvent
bool MTextFrame::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)		return true;


	MRECT cr = GetInitialClientRect();
	MPOINT sp = MClientToScreen( this, pEvent->Pos);

	switch(pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN:
		if ( pEvent->nHitTest == MHTCAPTION  &&  m_bMovable == true)
		{
			m_ptLastCursor = pEvent->Pos;
			m_bMoving = true;
		}
		else if ( pEvent->nHitTest > MHTSIZEFIRST  &&  pEvent->nHitTest < MHTSIZELAST  &&  m_bResizable == true)
		{
			m_ptLastCursor = pEvent->Pos;
			m_nResizing = pEvent->nHitTest;
		}
		break;

	case MWM_LBUTTONUP:
		m_bMoving = false;
		m_nResizing = HTNOWHERE;
		break;

	case MWM_MOUSEMOVE:
		if ( pEvent->Pos.y >= 0  &&  pEvent->Pos.y <= 20)
			EnsureTitlebar( true);

		if ( IsCapture() == false)
			break;

		else if ( m_bMoving == true)
		{
			MPOINT _p( m_Rect.x + pEvent->Pos.x - m_ptLastCursor.x, m_Rect.y + pEvent->Pos.y - m_ptLastCursor.y);
			SetPosition( _p);
		}

		else if ( m_nResizing > MHTSIZEFIRST  &&  m_nResizing < MHTSIZELAST)
		{
			MRECT _r = m_Rect;
			MPOINT delta;
			delta.x = pEvent->Pos.x - m_ptLastCursor.x;
			delta.y = pEvent->Pos.y - m_ptLastCursor.y;

			bool _left = false, _top = false, _right = false, _bottom = false;
			if ( m_nResizing == MHTLEFT  ||  m_nResizing == MHTTOPLEFT  ||  m_nResizing == MHTBOTTOMLEFT)			_left = true;
			if ( m_nResizing == MHTTOP  ||  m_nResizing == MHTTOPLEFT  ||  m_nResizing == MHTTOPRIGHT)				_top = true;
			if ( m_nResizing == MHTRIGHT  ||  m_nResizing == MHTTOPRIGHT  ||  m_nResizing == MHTBOTTOMRIGHT)		_right = true;
			if ( m_nResizing == MHTBOTTOM  ||  m_nResizing == MHTBOTTOMLEFT  ||  m_nResizing == MHTBOTTOMRIGHT)		_bottom = true;

			if ( _left == true)		{ _r.x += delta.x;		_r.w -= delta.x;		delta.x = 0;	}
			if ( _top == true)		{ _r.y += delta.y;		_r.h -= delta.y;		delta.y = 0;	}
			if ( _right == true)	{ _r.w += delta.x;		}
			if ( _bottom == true)	{ _r.h += delta.y;		}

			_r.w = max( 50, _r.w);
			_r.h = max( 50, _r.h);
			m_ptLastCursor.x += delta.x;
			m_ptLastCursor.y += delta.y;

			SetPosition( _r.x, _r.y);
			SetSize( _r.w, _r.h);

			return true;
		}

		break;

	case MWM_MOUSEWHEEL :
		ScrollPage( -(int)( (float)(pEvent->nDelta * 0.1f)));
		return true;
	}

	return false;
}


// OnRun
void MTextFrame::OnRun( DWORD dwCurrTime)
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

	if ( m_bShowTitleBar == true  &&  m_tShowTitleBar.IsArrival())
		EnsureTitlebar( false);
}


// OnSize
void MTextFrame::OnSize( int w, int h)
{
	MWidget::OnSize( w, h);

	UpdateScrollBar();
}


// OnNcHitTest
int MTextFrame::OnNcHitTest( MEvent* pEvent)
{
	int _hittest = MHTNOWHERE;

	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	if ( m_EventHandler.OnScriptEvent( this, MEVENT_NCHITTEST) == true)
	{
		_hittest = MWidget::m_EventArgs.m_nHitTest;
	}
	else
	{
		MRECT r = GetWindowRect();
		if ( r.InPoint( pEvent->Pos) == true)
		{
			_hittest = MHTCLIENT;

			MRECT _wr = GetWindowRect();
			MRECT _caption( _wr.x, _wr.y, _wr.w, MTITLEBAR_HEIGHT);
			if ( _caption.InPoint( pEvent->Pos) == true)
			{
				_hittest = MHTCAPTION;
			}
			else
			{
				if ( ( pEvent->Pos.x - r.x) < RESIZEBOX_SIZE)				_hittest = MHTLEFT;
				else if ( ( r.x + r.w - pEvent->Pos.x) < RESIZEBOX_SIZE)	_hittest = MHTRIGHT;

				if ( ( pEvent->Pos.y - r.y) < RESIZEBOX_SIZE)
				{
					if ( _hittest == MHTLEFT)								_hittest = MHTTOPLEFT;
					else if ( _hittest == MHTRIGHT)							_hittest = MHTTOPRIGHT;
					else													_hittest = MHTTOP;
				}
				else if ( ( r.y + r.h - pEvent->Pos.y) < RESIZEBOX_SIZE)
				{
					if ( _hittest == MHTLEFT)								_hittest = MHTBOTTOMLEFT;
					else if ( _hittest == MHTRIGHT)							_hittest = MHTBOTTOMRIGHT;
					else													_hittest = MHTBOTTOM;
				}
			}
		}
	}

	return _hittest;
}








// MTextFrameLook
MTextFrameLook::MTextFrameLook()
{
	SetTextAlign( MAM_LEFT | MAM_VCENTER);
	SetWorkRectOffset( MRECT( 3, 3, -6, -6));
	m_nTitlebarHeight = MTITLEBAR_HEIGHT;
	m_nItemHeight = 0;
}

void MTextFrameLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	MRECT r = GetBkgrndRect( pWidget);
	if ( Draw( pDC, r, MWP_BORDER, pWidget->GetState()) == false)
	{
		pDC->SetColor( 0, 0, 0, 160);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ pWidget->GetStateNum()]);
		pDC->Rectangle( r);
	}
}

void MTextFrameLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	MTextFrame* pTextFrame = (MTextFrame*)pWidget;

	OnDrawItems( pTextFrame, pDC);

	// Titlebar
	if ( pTextFrame->IsShowTitleBar() == true)
	{
		MRECT r = pTextFrame->GetClientRect();
		r.h = MTITLEBAR_HEIGHT;

		OnDrawTitlebar( pTextFrame, r, pDC);
	}
}

void MTextFrameLook::OnDrawItems( MTextFrame* pTextFrame, MDrawContext* pDC)
{
	// Set font
	pDC->SetFont( GetFont());

	// Draw items
	MRECT r = pTextFrame->GetClientRect();
	int x = 0, y = 0;
	int _item_height = pTextFrame->GetItemHeight();
	size_t _head = pTextFrame->GetHead();
	size_t _tail = pTextFrame->GetTail();
	size_t _cap = pTextFrame->GetCapacity();
	size_t _start = _pos_( _tail, pTextFrame->GetScrollBar()->GetScrollValue(), _cap);
	size_t _i = _start;
	for ( size_t i = _start;  i != _head;  _next_(i, _cap))
	{
		MRECT _r( x, y, r.w, _item_height);

		// Draw item
		OnDrawItemText( pTextFrame, i, _r, pDC);

		y += _item_height;
		if ( y > r.h)	break;

		_i = i;
	}
}

void MTextFrameLook::OnDrawItemText( MTextFrame* pTextFrame, size_t index, MRECT& r, MDrawContext* pDC)
{
	pDC->SetColor( GetFontColor( MWP_BORDER, MWS_NORMAL, MColorTable::TEXT[ MWSN_NORMAL]));
	pDC->Text( r, pTextFrame->GetItemText( index), GetTextAlign());
}

void MTextFrameLook::OnDrawTitlebar( MTextFrame* pTextFrame, MRECT& r, MDrawContext* pDC)
{
	pDC->SetFont( GetFont());

	// Titlebar
	if ( Draw( pDC, r, MWP_TITLEBAR, pTextFrame->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::TITLEBAR[ pTextFrame->GetStateNum()]);
		pDC->FillRectangle( r.x + 1, r.y + 1, r.w - 1, r.h - 1);
	}

	// Title
	pDC->SetColor( GetFontColor( MWP_TITLEBAR, pTextFrame->GetState(), MColorTable::TITLE[ pTextFrame->GetStateNum()]));
	pDC->Text( r, pTextFrame->GetText(), GetTextAlign());
}


} // namespace mint3


