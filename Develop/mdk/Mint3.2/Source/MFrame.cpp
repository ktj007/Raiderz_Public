#include "stdafx.h"
#include "MFrame.h"
#include "MColorTable.h"
#include "Mint.h"



namespace mint3
{


#define MTITLEBAR_HEIGHT		18
#define RESIZEBOX_SIZE			5
#define MAGNETDIST				10




// MFrame
MFrame::MFrame( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MFrameLook);

	SetEnableFocus( true);

	m_bShow = false;
	m_bZOrderChangable = true;
	m_bShowTitlebar = true;
	m_pIcon = NULL;
	m_bMovable = true;
	m_bMoving = false;
	m_bResizable = false;
	m_nResizing = 0;
}


// IsFocus
bool MFrame::IsFocus()
{
	MWidget* pWidget = MWidget::m_pFocusedWidget;

	while ( pWidget != NULL)
	{
		if ( pWidget == this)	return true;
		pWidget = pWidget->GetParent();
	}

	return false;
}


// GetDrawState
int MFrame::GetDrawState()
{
	if ( m_bShow == false  &&  m_tFade.IsArrival() == false)		return true;
	return MWidget::GetDrawState();
}


// InitMouseState
void MFrame::InitMouseState()
{
	MWidget::InitMouseState();

	m_bMoving = false;
	m_nResizing = 0;
}


// OnEvent
bool MFrame::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)		return true;

	switch ( pEvent->nMessage)
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
		if ( IsCapture() == false)
			break;
		
		else if ( m_bMoving == true)
		{
			MPOINT _p( m_Rect.x + pEvent->Pos.x - m_ptLastCursor.x, m_Rect.y + pEvent->Pos.y - m_ptLastCursor.y);
			MRECT _bgoffset = HasLookAndFill()  ?  ((MFrameLook*)GetLookAndFill())->GetMagneticRectOffset()  :  MRECT( 0, 0, 0, 0);
			MRECT _r( _p.x + _bgoffset.x, _p.y + _bgoffset.y, m_Rect.w + _bgoffset.w, m_Rect.h + _bgoffset.h);

			if ( m_bModalState == false)
			{
				// Check other frame
				if ( m_pParent != NULL  &&  pEvent->bAlt == false)
				{
					list< MWidget*>& _child = m_pParent->GetChilList();
					for ( list< MWidget*>::iterator itr = _child.begin();  itr != _child.end();  itr++)
					{
						MWidget* pWidget = (*itr);
						if ( pWidget == NULL  ||  pWidget == this  ||  pWidget->GetShow() == false  ||  strcmp( pWidget->GetClassName(), MINT_FRAME) != 0)
							continue;

						MRECT _cr = pWidget->GetRect();
						MRECT _offset = pWidget->HasLookAndFill()  ?  ((MFrameLook*)pWidget->GetLookAndFill())->GetMagneticRectOffset()  :  MRECT( 0, 0, 0, 0);
						_cr.x += _offset.x;
						_cr.y += _offset.y;
						_cr.w += _offset.w;
						_cr.h += _offset.h;

						if ( _r.x >= (_cr.x - _r.w - MAGNETDIST)  &&  _r.x <= (_cr.x + _cr.w + MAGNETDIST)  &&
							_r.y >= (_cr.y - _r.h - MAGNETDIST)  &&  _r.y <= (_cr.y + _cr.h + MAGNETDIST) )
						{
							if ( abs( _r.x - _cr.x) < MAGNETDIST)							_r.x = _cr.x;
							else if ( abs( _r.x - (_cr.x + _cr.w)) < MAGNETDIST)			_r.x = _cr.x + _cr.w;
							else if ( abs( (_r.x + _r.w) - _cr.x) < MAGNETDIST)				_r.x = _cr.x - _r.w;
							else if ( abs( (_r.x + _r.w) - (_cr.x + _cr.w)) < MAGNETDIST)	_r.x = _cr.x + _cr.w - _r.w;

							if ( abs( _r.y - _cr.y) < MAGNETDIST)							_r.y = _cr.y;
							else if ( abs( _r.y - (_cr.y + _cr.h)) < MAGNETDIST)			_r.y = _cr.y + _cr.h;
							else if ( abs( (_r.y + _r.h) - _cr.y) < MAGNETDIST)				_r.y = _cr.y - _r.h;
							else if ( abs( (_r.y + _r.h) - (_cr.y + _cr.h)) < MAGNETDIST)	_r.y = _cr.y + _cr.h - _r.h;
						}
					}
				}

				// Check window bound rectangle
				if ( true)
				{
					if ( abs( _r.x) < MAGNETDIST)													_r.x = 0;
					else if ( abs( MGetMint()->GetWorkspaceWidth() - (_r.x + _r.w)) < MAGNETDIST)	_r.x = MGetMint()->GetWorkspaceWidth() - _r.w;

					if ( abs( _r.y) < MAGNETDIST)													_r.y = 0;
					else if ( abs( MGetMint()->GetWorkspaceHeight() - (_r.y + _r.h)) < MAGNETDIST)	_r.y = MGetMint()->GetWorkspaceHeight() - _r.h;
				}
			}

			SetPosition( _r.x - _bgoffset.x, _r.y - _bgoffset.y);
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

	case MWM_MOUSEWHEEL:
		return true;
	}


	return false;
}


// OnPreDraw
void MFrame::OnPreDraw( MDrawContext* pDC)
{
	if ( m_tFade.IsEnabled() == false)		return;

	float ratio = (float)m_tFade.GetRatio();
	pDC->SetOpacity( pDC->GetOpacity() * ratio);

	if ( m_bShow == true  &&  m_bStatic == false)
	{
		MPOINT origin = pDC->GetOrigin();
		origin.y += (int)( 10.0f - 10.0f * ratio);
		pDC->SetOrigin( origin);
	}
}


// OnShow
void MFrame::OnShow( bool bShow)
{
	MWidget::OnShow( bShow);

	m_tFade.SetReverse( !bShow);


	if ( bShow == false)		MGetMint()->GetToolTipManager()->Reset();
}


// OnActivate
void MFrame::OnActivate( bool bActivate)
{
	m_bMoving = false;

	MWidget::OnActivate( bActivate);
}


// OnNcHitTest
int MFrame::OnNcHitTest( MEvent* pEvent)
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
			MRECT _caption( _wr.x, _wr.y, _wr.w, (HasLookAndFill() ? ((MFrameLook*)GetLookAndFill())->GetTitlebarHeight() : MTITLEBAR_HEIGHT));
			if ( m_bShowTitlebar == true  &&  _caption.InPoint( pEvent->Pos) == true)
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


// glueSetIcon
void MFrame::glueSetIcon( const char* szIcon)
{
	if ( szIcon == NULL  ||  szIcon[ 0] == 0)
	{
		m_pIcon = NULL;
		return;
	}

	MBitmap* pIcon = MBitmapManager::Get( szIcon);
	if ( pIcon != NULL)		SetIcon( pIcon);
}


// glueGetIcon
const char* MFrame::glueGetIcon()
{
	if ( m_pIcon == NULL)		return "";
	return m_pIcon->GetName();
}













// MFrameLook
MFrameLook::MFrameLook()
{
	SetTextAlign( MAM_LEFT | MAM_VCENTER);

	m_nTitlebarHeight = MTITLEBAR_HEIGHT;
	m_bTitleShadow = false;
	m_ptTitleOffset = MPOINT( 0, 0);
	m_rIconRect = MRECT( 1, 1, 16, 16);
	m_rMagneticectOffset = MRECT( 0, 0, 0, 0);
}


void MFrameLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	MLookBase::OnDrawBorder( pWidget, pDC);

	MBitmap* pIcon = ((MFrame*)pWidget)->GetIcon();
	if ( pIcon != NULL)
	{
		pDC->SetBitmap( pIcon);
		pDC->Draw( m_rIconRect);
	}
}


void MFrameLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	MFrame* pFrame = (MFrame*)pWidget;

	// Titlebar
	if ( pFrame->IsShowTitlebar() == true)
	{
		MRECT r = pFrame->GetClientRect();
		r.h = MTITLEBAR_HEIGHT;

		if ( pFrame->IsShowBorder() == true)
			OnDrawTitlebar( pFrame, r, pDC);
	}
}

void MFrameLook::OnDrawTitlebar( MFrame* pFrame, MRECT& r, MDrawContext* pDC)
{
	pDC->SetFont( GetFont());

	// Titlebar
	if ( Draw( pDC, r, MWP_TITLEBAR, pFrame->GetState()) == false)
	{
		// Draw default
		pDC->SetColor( MColorTable::TITLEBAR[ pFrame->GetStateNum()]);
		pDC->FillRectangle( r.x + 1, r.y + 1, r.w - 1, r.h - 1);
	}


	r.x += m_ptTitleOffset.x;
	r.y += m_ptTitleOffset.y;


	// Shadow
	if ( m_bTitleShadow == true)
	{
		MRECT sr = r;
		sr.x++;
		sr.y++;
		pDC->SetColor( 0, 0, 0, 128);
		pDC->Text( sr, pFrame->GetText(), GetTextAlign());
	}

	// Title
	pDC->SetColor( GetFontColor( MWP_TITLEBAR, pFrame->GetState(), MColorTable::TITLE[ pFrame->GetStateNum()]));
	pDC->Text( r, pFrame->GetText(), GetTextAlign());
}

} // namespace mint3