#include "stdafx.h"
#include "MResource.h"
#include "MWidget.h"
#include "Mint.h"
#include "MScrollBar.h"
#include "MButton.h"
#include "MEdit.h"
#include "MEditCtrl.h"
#include "MTextView.h"
#include "MPopupMenu.h"
#include "MWLua.h"
#include "MScriptManager.h"
#include "MWidgetManager.h"
#include "MLookManager.h"
#include "MintProfiler.h"
#include "MDebug.h"
#include <stack>
#include <math.h>


namespace mint3
{


#define DEFAULT_TOOLTIPWIDTH		250
#define DEFAULT_TOOLTIPOFFSET		MPOINT( 30, 30)


MWidget* MWidget::m_pCapturedWidget		= NULL;
MWidget* MWidget::m_pFocusedWidget		= NULL;
MWidget* MWidget::m_pMouseOverWidget	= NULL;

MEventArgs			MWidget::m_EventArgs;
MDragEventArgs		MWidget::m_DragEventArgs;
MPopupEventArgs		MWidget::m_PopupEventArgs;

bool MWidget::m_RunEventOnPosition		= false;
bool MWidget::m_RunEventOnSize			= false;

static DWORD		g_dwUnnamedCount = 0;




MRECT _intersect( const MRECT& src_r, const MRECT& tar_r)
{
	int x1 = max( src_r.x, tar_r.x);
	int y1 = max( src_r.y, tar_r.y);
	int x2 = min( src_r.x + src_r.w,  tar_r.x + tar_r.w);
	int y2 = min( src_r.y + src_r.h,  tar_r.y + tar_r.h);

	return MRECT( x1,  y1,  x2 - x1,  y2 - y1);
}






// Constructor
MWidget::MWidget( const char* szName /*=NULL*/, MWidget* pParent /*=NULL*/, MListener* pListener /*=NULL*/)
{
	m_pParent = pParent;

	if ( m_pParent)			m_pParent->AddChild( this);
	if ( pListener)			SetListener( pListener);


	m_pWndDisable = NULL;
	m_pLookAndFill = NULL;
	if ( szName != NULL)	m_strName = szName;
	m_Rect = MRECT( 100, 100, 100, 100);
	m_Anchor = MAnchorStyle( true, false, true, false);
	m_bModalState = false;
	m_nModalResult = 0;
	m_bShow = true;
	m_bEnable = true;
	m_bEnableFocus = false;
	m_fOpacity = 1.0f;
	m_bShowBorder = true;
	m_bZOrderChangable = false;
	m_bStatic = false;

	m_bLButtonDown = false;
	m_bRButtonDown = false;
	m_ptToolTipOffset = DEFAULT_TOOLTIPOFFSET;

	m_bDeclToScript = false;

#ifndef _PUBLISH
	m_bShowRect = false;
#endif

	if ( m_strName.empty() == false)
	{
		// 이름이 있으면 관리자에 추가한다
		Mint::GetInstance()->m_pWidgetManager->AddWidget( m_strName.c_str(), this);
	}
}


// Destructor
MWidget::~MWidget()
{
	RemoveFromParent();


	DeleteChildren();
	m_MessageFilter.clear();
	m_pWndDisable = NULL;

	if ( MWidget::m_pFocusedWidget == this)		ReleaseFocus();
	if ( MWidget::m_pCapturedWidget == this)	ReleaseCapture();
	if ( MWidget::m_pMouseOverWidget == this)	ReleaseMouseOver();


	if ( m_strName.empty() == false)
		Mint::GetInstance()->m_pWidgetManager->RemoveWidget( this);


	if ( m_bDeclToScript == true)
		Mint::GetInstance()->GetScriptManager()->ReleaseWidget( m_strName.c_str());

	MGetMint()->GetTimerManager()->Remove( this);
}


// Event
MRESULT MWidget::Event( MEvent* pEvent)
{
	// Message filter(hooking)
	if ( m_MessageFilter.empty() == false)
	{
		for ( list< MWidget*>::iterator itr = m_MessageFilter.begin();  itr != m_MessageFilter.end();  itr++)
		{
			if ( (*itr)->OnPreFilterMessage( pEvent) == true)
				return MTRANSLATED;
		}
	}


	// Pre-translate message
	MRESULT nResult = OnPreTranslateMsg( pEvent);
	if ( nResult != MNOTPROCESSED)							return nResult;


	// Dispatch message
	nResult = MNOTPROCESSED;
	if ( DispatchModalWnd( pEvent, &nResult) == true)		return nResult;
	else if ( DispatchMouse( pEvent, &nResult) == true)		return nResult;
	else if ( DispatchKey( pEvent, &nResult) == true)		return nResult;


	MEvent LocalEvent;
	MakeLocalEvent( &LocalEvent, pEvent);
	bool bResult = OnEvent( &LocalEvent, GetListener());
	return ( bResult ?  MTRANSLATED : MNOTPROCESSED);
}


// DispatchModalWnd
bool MWidget::DispatchModalWnd( MEvent* pEvent, MRESULT* nResult)
{
	// parent modal window
	if ( m_pWndDisable != NULL  &&  m_bModalState == false)
	{
		*nResult = m_pWndDisable->Event( pEvent);

		// Translate mouse event
		if ( *nResult == MNOTPROCESSED  &&  (pEvent->nMessage > MWM_MOUSEFIRST  &&  pEvent->nMessage < MWM_MOUSELAST))
		{
			// Hit test
			MRECT r = GetWindowRect();
			if ( r.InPoint( pEvent->Pos) == true)
			{
				// Translate message
				switch ( pEvent->nMessage)
				{
				case MWM_LBUTTONDOWN :
				case MWM_LBUTTONDBLCLK :
				case MWM_RBUTTONDOWN :
				case MWM_RBUTTONDBLCLK :
					BringToTop();
					break;
				}

				*nResult = MTRANSLATED;
			}
		}

		return true;
	}

	*nResult = MNOTPROCESSED;
	return false;
}


// DispatchMouse
bool MWidget::DispatchMouse( MEvent* pEvent, MRESULT* nResult)
{
	if ( pEvent->nMessage > MWM_MOUSEFIRST  &&  pEvent->nMessage < MWM_MOUSELAST)
	{
		// Make local event
		MEvent LocalEvent;
		MakeLocalEvent( &LocalEvent, pEvent);


		// Hit test
		LocalEvent.nHitTest = OnNcHitTest( pEvent);
		if ( LocalEvent.nHitTest == MHTCAPTION)
		{
			if ( m_bStatic == true  ||  IsEnabled() == false  ||  IsMovable() == false)
				LocalEvent.nHitTest = MHTCLIENT;
		}
		else if ( LocalEvent.nHitTest > MHTSIZEFIRST  &&  LocalEvent.nHitTest < MHTSIZELAST)
		{
			if ( m_bStatic == true  ||  IsEnabled() == false  ||  IsResizable() == false)
				LocalEvent.nHitTest = MHTCLIENT;
		}


		// Check static
		if ( m_bStatic == true)
		{
			if ( LocalEvent.nHitTest <= 0)
			{
				*nResult = MNOTPROCESSED;
			}
			else
			{
				// Send message to descendants
				MRESULT _result = MNOTPROCESSED;
				for ( list<MWidget*>::reverse_iterator itr = m_Children.rbegin();  itr != m_Children.rend();  itr++)
				{
					MWidget* pChild = (*itr);
					if ( pChild->GetShow() == false)	continue;

					_result = pChild->Event( pEvent);
					if ( _result != MNOTPROCESSED)		break;
				}

				*nResult = _result;
			}

			return true;
		}


		// Check captured
		if ( MWidget::m_pCapturedWidget == this)
		{
			if ( LocalEvent.nHitTest <= 0)		ReleaseMouseOver();
			else								SetMouseOver();


			// Translate message
			switch ( pEvent->nMessage)
			{
			case MWM_LBUTTONUP :
			case MWM_RBUTTONUP :
				ReleaseCapture();
				break;
			}


			// Call event handler
			OnEvent( &LocalEvent, GetListener());

			*nResult = MTRANSLATED;
			return true;
		}
		else
		{
			// Fail to hit test
			if ( LocalEvent.nHitTest <= 0)
			{
				// Translate message
				switch ( pEvent->nMessage)
				{
				case MWM_LBUTTONDOWN :
				case MWM_LBUTTONDBLCLK :
				case MWM_RBUTTONDOWN :
				case MWM_RBUTTONDBLCLK :
					if ( MWidget::m_pFocusedWidget == this)		ReleaseFocus();
					break;
				}

				if ( MWidget::m_pMouseOverWidget == this)		ReleaseMouseOver();

				*nResult = MNOTPROCESSED;
				return true;
			}


			// Disabled
			if ( GetEnable() == false)
			{
				SetMouseOver();
				*nResult = MTRANSLATED;
				return true;
			}


			// Send message to descendants
			for ( list<MWidget*>::reverse_iterator itr = m_Children.rbegin();  itr != m_Children.rend();  itr++)
			{
				MWidget* pChild = (*itr);
				if ( pChild->GetShow() == false)	continue;

				MRESULT _result = pChild->Event( pEvent);
				if ( _result != MNOTPROCESSED)
				{
					*nResult = _result;
					return true;
				}
			}


			// Set cursor
			string strCursor = MCURSOR_ARROW;
			switch ( LocalEvent.nHitTest)
			{
			case MHTCAPTION :		strCursor = MCURSOR_SIZEALL;	break;
			case MHTLEFT :
			case MHTRIGHT :			strCursor = MCURSOR_SIZEWE;		break;
			case MHTTOP :
			case MHTBOTTOM :		strCursor = MCURSOR_SIZENS;		break;
			case MHTTOPLEFT :
			case MHTBOTTOMRIGHT :	strCursor = MCURSOR_SIZENWSE;	break;
			case MHTTOPRIGHT :
			case MHTBOTTOMLEFT :	strCursor = MCURSOR_SIZENESW;	break;
			}

			OnSetCursor( &LocalEvent, &strCursor);

			if ( MCursorSystem::IsExist( strCursor.c_str()) == false)
				strCursor = MCURSOR_ARROW;

			MCursorSystem::Set( strCursor.c_str());
			
			
			// Disabled focus
			if ( IsEnableFocus() == false)
			{
				*nResult = MNOTPROCESSED;
				return true;
			}

			
			// Set mouse over
			SetMouseOver();


			// Translate message
			switch ( pEvent->nMessage)
			{
			case MWM_LBUTTONDOWN :
			case MWM_LBUTTONDBLCLK :
			case MWM_RBUTTONDOWN :
			case MWM_RBUTTONDBLCLK :
				// Bring to top
				BringToTop();

				// Set focus
				SetFocus();

				// Set capture
				SetCapture();

				break;
			}

			// Call event handler
			OnEvent( &LocalEvent, GetListener());

			*nResult = MTRANSLATED;
			return true;
		}
	}

	else if ( pEvent->nMessage == MWM_MOUSEWHEEL)
	{
		// Make local event
		MEvent LocalEvent;
		MakeLocalEvent( &LocalEvent, pEvent);


		// Hit test
		LocalEvent.nHitTest = OnNcHitTest( pEvent);
		if ( LocalEvent.nHitTest <= MHTNOWHERE)
		{
			*nResult = MNOTPROCESSED;
			return true;
		}


		// Send message to descendants
		if ( GetCapture() == false)
		{
			for ( list<MWidget*>::reverse_iterator itr = m_Children.rbegin();  itr != m_Children.rend();  itr++)
			{
				MWidget* pChild = (*itr);
				if ( pChild->GetShow() == false)		continue;

				MRESULT _result = pChild->Event( pEvent);
				if ( _result != MNOTPROCESSED)
				{
					*nResult = _result;
					return true;
				}
			}
		}


		if ( m_bStatic == true  ||  IsLayered() == true)
		{
			*nResult = MNOTPROCESSED;
			return true;
		}


		// Call event handler
		if ( OnEvent( &LocalEvent, GetListener()) == true)
			*nResult = MTRANSLATED;

		return true;
	}


	*nResult = MNOTPROCESSED;
	return false;
}


// DispatchKey
bool MWidget::DispatchKey( MEvent* pEvent, MRESULT* nResult)
{
	if ( pEvent->nMessage > MWM_KEYFIRST  &&  pEvent->nMessage < MWM_KEYLAST)
	{
		// Call event handler
		MEvent LocalEvent;
		MakeLocalEvent( &LocalEvent, pEvent);
		bool _return = OnEvent( &LocalEvent, GetListener());

		*nResult = (_return == true) ? MTRANSLATED : MNOTPROCESSED;
		return true;
	}

	*nResult = MNOTPROCESSED;
	return false;
}


// EventHotKey
MRESULT MWidget::EventHotKey( unsigned int nHotKey)
{
	// Send message to descendants
	for ( list<MWidget*>::reverse_iterator itr = m_Children.rbegin();  itr != m_Children.rend();  itr++)
	{
		MWidget* pChild = (*itr);
		if ( pChild->GetShow() == false  ||  pChild->GetEnable() == false)		continue;

		MRESULT nResult = pChild->EventHotKey( nHotKey);
		if ( nResult != MNOTPROCESSED)		return nResult;
	}

	if ( OnQueryHotKey( nHotKey) == true)
	{
		OnHotKey( nHotKey);

		return MTRANSLATED;
	}

	return MNOTPROCESSED;
}


// Run
void MWidget::Run( DWORD dwCurrTime)
{
	// Begin update profile
	MGetMintProfiler()->WriteUpdateProfileBegin( this);


	// Update
	OnRun( dwCurrTime);


	// End update profile
	MGetMintProfiler()->WriteUpdateProfileEnd( this);


	// Update child
	for ( list<MWidget*>::iterator itr = m_Children.begin();  itr != m_Children.end();  ++itr)
	{
		if ( (*itr)->GetShow() == true)
			(*itr)->Run( dwCurrTime);
	}
}


// Draw
void MWidget::Draw( MDrawContext* pDC)
{
	// Begin render profile
	MGetMintProfiler()->WriteRenderProfileBegin( this);


	if ( GetDrawState() == MDS_NOTDRAW)
	{
		// End render profile
		MGetMintProfiler()->WriteRenderProfileEnd( this);
		return;
	}


	// 위젯이 부모 영역을 벗어나는지 확인
	if ( m_pParent != NULL  &&  m_bModalState == false)
	{
		MRECT pr = m_pParent->GetClientRect();
		if ( m_Rect.y > (pr.h + 10)  ||  (m_Rect.y + m_Rect.h) < -10  ||  m_Rect.x > (pr.w + 10)  ||  (m_Rect.x + m_Rect.w) < -10)
		{
			// End render profile
			MGetMintProfiler()->WriteRenderProfileEnd( this);
			return;
		}
	}

	
	// Get parent window clipping region, origin and opacity
	MRECT	old_clip = pDC->GetClipRect();
	MPOINT	old_orgn = pDC->GetOrigin();
	float	old_opacity = pDC->GetOpacity();


	// Set origin and opacity
	pDC->SetOrigin( old_orgn.x + m_Rect.x,  old_orgn.y + m_Rect.y);
	pDC->SetOpacity( old_opacity * m_fOpacity);


	// Pre-draw
	OnPreDraw( pDC);

	
	// Draw border
	if ( m_bShowBorder == true)
	{
		if ( m_bModalState == true)
		{
			MPOINT _dc_orgn = pDC->GetOrigin();
			pDC->SetClipRect( 0, 0, MGetMint()->GetWorkspaceWidth() - 1, MGetMint()->GetWorkspaceHeight() - 1);
			pDC->SetOrigin( _dc_orgn);
		}

		OnDrawBorder( pDC);
	}


	// Get window clipping region, origin and opacity
	MRECT	wro = GetWorkRectOffset();
	MPOINT	orgn = pDC->GetOrigin();
	MRECT	clip;
	
	if ( m_bModalState == true)
	{
		MRECT wr = GetWindowRect();
		MRECT _clip = MRECT( wr.x + wro.x, wr.y + wro.y, wr.w + wro.w, wr.h + wro.h);
		clip = _intersect( MRECT( 0, 0, MGetMint()->GetWorkspaceWidth() - 1, MGetMint()->GetWorkspaceHeight() - 1), _clip);
	}
	else
	{
		MRECT _clip = MRECT( orgn.x + wro.x, orgn.y + wro.y, m_Rect.w + wro.w, m_Rect.h + wro.h);
		clip = _intersect( old_clip, _clip);
	}

	if ( clip.w < 1  ||  clip.h < 1)
	{
		// Restore origin and opacity
		pDC->SetOrigin( old_orgn);
		pDC->SetOpacity( old_opacity);

		// End render profile
		MGetMintProfiler()->WriteRenderProfileEnd( this);
		return;
	}

	
	// Set clipping region, origin
	MRECT	wr = GetWorkRect();
	orgn.x += wr.x;
	orgn.y += wr.y;

	pDC->SetClipRect( clip);
	pDC->SetOrigin( orgn);


	// Draw widget
	OnDraw( pDC);


	// Show rectangle
#ifndef _PUBLISH
	if ( m_bShowRect == true)
	{
		float fOldOpacity = pDC->SetOpacity( 1.0f);
		MCOLOR cOldColor = pDC->SetColor( 255, 255, 255, 128 + (int)( 128 * sin( (double)MGetMint()->GetUpdateTime() * 0.01)));
		pDC->Rectangle( 0, 0, m_Rect.w, m_Rect.h);
		pDC->SetOpacity( fOldOpacity);
		pDC->SetColor( cOldColor);
	}
#endif


	// End render profile
	MGetMintProfiler()->WriteRenderProfileEnd( this);


	// Draw child widgets
	if ( m_Children.size() > 0)
	{
		for ( list<MWidget*>::iterator itor = m_Children.begin(); itor != m_Children.end(); ++itor)
		{
			MWidget* pChild = (*itor);
			if ( pChild == NULL)	continue;
			pChild->Draw( pDC);
		}
	}


	// Restore clipping region, origin and opacity
	pDC->SetClipRect( old_clip);
	pDC->SetOrigin( old_orgn);
	pDC->SetOpacity( old_opacity);
}


// GetState
const char* MWidget::GetState()
{
	if ( IsEnabled() == false)			return MWS_DISABLE;
	else if ( IsFocus() == true)		return MWS_FOCUS;
	return MWS_NORMAL;
}

int MWidget::GetStateNum()
{
	if ( IsEnabled() == false)			return MWSN_DISABLE;
	else if ( IsFocus() == true)		return MWSN_FOCUS;
	return MWSN_NORMAL;
}



// SetParent
void MWidget::SetParent( MWidget* pParent)
{
	if ( m_pParent == pParent)	return;

	RemoveFromParent();

	m_pParent = pParent;
	if ( m_pParent)		m_pParent->AddChild( this);
}



// GetChild
MWidget* MWidget::GetChild( int i)
{
	list< MWidget*>::iterator itor = m_Children.begin();
	for ( int index = 0;  index < i;  ++index, ++itor)
	{
		if ( itor == m_Children.end())
			break;
	}

	return ( (itor != m_Children.end())	 ?  (*itor) : NULL);
}


// GetChildIndex
int MWidget::GetChildIndex( MWidget* pWidget)
{
	int index = 0;
	for ( list<MWidget*>::iterator itor = m_Children.begin();  itor != m_Children.end();  ++itor)
	{
		MWidget* pChild = (*itor);
		if ( pChild == pWidget)		return index;
		index++;
	}

	return -1;
}


// WindowFromPoint
MWidget* MWidget::WindowFromPoint( MPOINT& pt, bool bEnFocus)
{
	if ( GetShow() == false)	return NULL;

	if ( m_pWndDisable != NULL)
	{
		MWidget* pWidget = m_pWndDisable->WindowFromPoint( pt);
		if ( pWidget != NULL)	return pWidget;
	}

	else if ( GetWindowRect().InPoint( pt) == true)
	{
		for ( list< MWidget*>::reverse_iterator itor = m_Children.rbegin();  itor != m_Children.rend();  ++itor)
		{
			MWidget* pWidget = (*itor)->WindowFromPoint( pt, bEnFocus);
			if ( pWidget != NULL)	return pWidget;
		}

		if ( bEnFocus == true)
		{
			if ( m_bEnableFocus == true  &&  m_bStatic == false)
				return this;

			return NULL;
		}
		else
			return this;
	}

	return NULL;
}


// ToolTipWindowFromPoint
MWidget* MWidget::ToolTipWindowFromPoint( MPOINT& pt)
{
	if ( GetShow() == false)
		return NULL;
	

	if ( m_pWndDisable != NULL)
	{
		MWidget* pWidget = m_pWndDisable->ToolTipWindowFromPoint( pt);
		return pWidget;
	}


	MEvent _event;
	_event.Pos = pt;
	if ( OnNcHitTest( &_event) == MHTNOWHERE)
		return NULL;

	for ( list< MWidget*>::reverse_iterator itor = m_Children.rbegin();  itor != m_Children.rend();  ++itor)
	{
		MWidget* pWidget = (*itor)->ToolTipWindowFromPoint( pt);
		if ( pWidget != NULL)
			return pWidget;
	}

	if ( m_bStatic == true  &&  m_strToolTip.empty() == true)
		return NULL;

	if ( m_bEnableFocus == true  ||  m_strToolTip.empty() == false)
		return this;
	
	return NULL;
}


// BeginModalState
void MWidget::BeginModalState()
{
	if ( m_pParent == NULL  ||  m_pParent->m_pWndDisable != NULL)	return;

	m_pParent->m_pWndDisable = this;
	m_bModalState = true;
	m_nModalResult = 0;

	BringToTop();
	SetFocus();
	Show( true);
}


// EndModalState
void MWidget::EndModalState()
{
	m_pParent->m_pWndDisable = NULL;
	m_bModalState = false;

	Show( false);
}


// AddChild
void MWidget::AddChild( MWidget* pWidget)
{
	list< MWidget*>::reverse_iterator itr;
	for ( itr = m_Children.rbegin();  itr != m_Children.rend();  itr++)
	{
		if ( (*itr)->GetAlwaysTop() == false)
			break;
	}

	if ( itr != m_Children.rend())
		m_Children.insert( itr.base(), pWidget);
	else
		m_Children.push_front( pWidget);

	pWidget->m_pParent = this;
}


// RemoveChild
void MWidget::RemoveChild( MWidget* pWidget)
{
	for ( list<MWidget*>::iterator itor = m_Children.begin();  itor != m_Children.end();  ++itor)
	{
		MWidget* pChild = (*itor);
		if ( pChild == pWidget)
		{
			pWidget->m_pParent = NULL;
			m_Children.erase( itor);
			return;
		}
	}
}


// DeleteChildren
void MWidget::DeleteChildren()
{
	while ( !m_Children.empty())
	{
		MWidget* pWidget = (*m_Children.begin());
		m_Children.erase( m_Children.begin());
		delete pWidget;
	}
}


// RemoveFromParent
void MWidget::RemoveFromParent()
{
	if ( m_pParent != NULL)
		m_pParent->RemoveChild( this);
}


// AddMessageFilter
void MWidget::AddMessageFilter( MWidget* pHooker)
{
	if ( pHooker == NULL)			return;

	for ( list< MWidget*>::iterator itr = m_MessageFilter.begin();  itr != m_MessageFilter.end();  itr++)
		if ( pHooker == (*itr))		return;

	m_MessageFilter.push_back( pHooker);
}


// DeleteMessageFilter
void MWidget::DeleteMessageFilter( MWidget* pHooker)
{
	if ( pHooker == NULL)			return;

	for ( list< MWidget*>::iterator itr = m_MessageFilter.begin();  itr != m_MessageFilter.end();  itr++)
	{
		if ( pHooker == (*itr))
		{
			m_MessageFilter.erase( itr);
			return;
		}
	}
}


// SetLookAndFill
void MWidget::SetLookAndFill( MLookBase* pLook)
{
	if ( pLook == m_pLookAndFill)		return;
	m_pLookAndFill = pLook;

	OnChangedLookAndFill();
}

void MWidget::SetLookAndFill( const char* szLookAndFill)
{
	if ( szLookAndFill == NULL  ||  szLookAndFill[ 0] == 0)
	{
		if ( m_pLookAndFill != NULL)
		{
			m_pLookAndFill = NULL;
			OnChangedLookAndFill();
		}
	}
	else
	{
		MLookBase* pLook = MGetMint()->m_pLookManager->Get( szLookAndFill);
		if ( pLook != NULL  &&  pLook != m_pLookAndFill)
		{
			m_pLookAndFill = pLook;
			OnChangedLookAndFill();
		}
	}
}


// SetText
void MWidget::SetText( const char* szText)
{
	if ( szText == NULL)	return;

	m_strText = szText;
}


// SetRect
void MWidget::SetRect( int x, int y, int w, int h)
{
	SetPosition( x, y);
	SetSize( w, h);
}


// GetClientRect
MRECT MWidget::GetClientRect()
{
	MRECT r = GetWorkRect();
	return MRECT( 0, 0, r.w, r.h);
}


// GetWorkRect
MRECT MWidget::GetWorkRect()
{
	MRECT r;
	if ( HasLookAndFill() == false)		r = MRECT( 0, 0, m_Rect.w, m_Rect.h);
	else								r = m_pLookAndFill->GetWorkRect( this);
	
	MScrollBar* pScrollBar = GetScrollBar();
	if ( pScrollBar  &&  pScrollBar->GetShow())
	{
		int _w = pScrollBar->GetWidth();

		if ( pScrollBar->IsHorz())
		{
			r.h -= pScrollBar->GetWidth();
			if ( pScrollBar->GetStyle() & MSBS_TOPALIGN)	r.y += _w;
		}
		else
		{
			r.w -= pScrollBar->GetWidth();
			if ( pScrollBar->GetStyle() & MSBS_LEFTALIGN)	r.x += _w;
		}
	}
	return r;
}


// GetWorkRectOffset
MRECT MWidget::GetWorkRectOffset()
{
	if ( HasLookAndFill() == false)		return MRECT( 0, 0, 0, 0);
	return m_pLookAndFill->GetWorkRectOffset();
}


// GetBkgrndRect
MRECT MWidget::GetBkgrndRect()
{
	if ( HasLookAndFill() == false)		return m_Rect;
	return m_pLookAndFill->GetBkgrndRect( this);
}


// GetBkgrndRectOffset
MRECT MWidget::GetBkgrndRectOffset()
{
	if ( HasLookAndFill() == false)		return MRECT( 0, 0, 0, 0);
	return m_pLookAndFill->GetBkgrndRectOffset();
}


// GetWindowRect
MRECT MWidget::GetWindowRect()
{
	if ( m_pParent == NULL)
		return m_Rect;

	MRECT wndrect = m_pParent->GetWindowRect();
	MRECT workrect = m_pParent->GetWorkRect();
	return MRECT( wndrect.x + workrect.x + m_Rect.x,  wndrect.y + workrect.y + m_Rect.y,  m_Rect.w,  m_Rect.h);
}


// SetPosition
void MWidget::SetPosition( int x, int y)
{
	if ( m_Rect.x == x  &&  m_Rect.y == y)		return;

	m_Rect.x = x;
	m_Rect.y = y;

	if ( MWidget::m_RunEventOnPosition == true)
		return;

	OnPosition( x, y);
}


// SetSize
void MWidget::SetSize( int w, int h)
{
	if ( w == m_Rect.w  &&  h == m_Rect.h)
		return;


	MRECT _rect = m_Rect;
	m_Rect.w = w;
	m_Rect.h = h;

	if ( MWidget::m_RunEventOnSize == true)
		return;

	OnSize( w, h);

	
	int dw = m_Rect.w - _rect.w;
	int dh = m_Rect.h - _rect.h;
	if ( dw == 0  &&  dh == 0)
		return;

	for ( list< MWidget*>::iterator itor = m_Children.begin();  itor != m_Children.end();  ++itor)
	{
		MWidget* pChild = (*itor);

		MRECT r = pChild->GetRect();
		MAnchorStyle anchor = pChild->GetAnchor();

		if ( anchor.bLeft == true  &&  anchor.bRight == true)		r.w += dw;
		else if ( anchor.bRight == true)							r.x += dw;
		else if ( anchor.bLeft == false  &&  anchor.bRight == false)
		{
			// 좌우 앵커가 모두 없다면 위젯의 위치에 따라서 가상으로 붙여준다
			int _w = (int)max( 10.0f,  (float)(_rect.w - r.w) * 0.33f);
			if ( r.x < _w)											;					// left anchor
			else if ( (_rect.w - ( r.x + r.w)) < _w)				r.x += dw;			// right anchor
			else													r.x += dw >> 1;		// middle
		}

		if ( (anchor.bTop == true) && (anchor.bBottom == true))		r.h += dh;
		else if (anchor.bBottom == true)							r.y += dh;
		else if ( anchor.bTop == false  &&  anchor.bBottom == false)
		{
			// 상하 앵커가 모두 없다면 위젯의 위치에 따라서 가상으로 붙여준다
			int _h = (int)max( 10.0f,  (float)(_rect.h - r.h) * 0.33f);
			if ( r.y < _h)											;					// top anchor
			else if ( (_rect.h - ( r.y + r.h)) < _h)				r.y += dh;			// bottom anchor
			else													r.y += dh >> 1;		// middle
		}

		pChild->SetRect( r);
	}
}


// Show
bool MWidget::Show( bool bShow)
{
	if ( m_bShow == bShow)					return m_bShow;
	m_bShow = bShow;

	if ( m_bShow == false)
	{
		// End modal state
		if ( m_bModalState == true)			EndModalState();
	}


	OnShow( m_bShow);
	OnActivate( bShow);

	return !m_bShow;
}


// IsVisibled
bool MWidget::IsVisibled()
{
	if ( m_bShow == false)		return false;
	if ( m_pParent == NULL)		return true;

	return ( m_pParent->IsVisibled() ? true : false);
}


// Enable
void MWidget::Enable( bool bEnable)
{
	if ( m_bEnable == bEnable)		return;
	m_bEnable = bEnable;

	OnEnable( bEnable);

	if ( m_bShow == true)
		OnActivate( bEnable);
}


// IsEnabled
bool MWidget::IsEnabled()
{
	if ( m_bEnable == false)	return false;
	if ( m_pParent == NULL)		return true;
	return ( m_pParent->IsEnabled() ? true : false);
}


// SendNotify
bool MWidget::SendNotify( MWidget* pSender, MEventMsg nMsg)
{
	if ( pSender == NULL)		return false;

	return OnCommand( pSender, nMsg);
}


// SendUserArguments
bool MWidget::SendUserArguments( const char* szArgs, bool bSendChild)
{
	bool bRetVal = false;

	if ( bSendChild == true)
	{
		for ( list< MWidget*>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
		{
			if ( (*itr)->SendUserArguments( szArgs, bSendChild) == true)
				bRetVal = true;
		}
	}

	if ( OnUserArgument( szArgs) == true)
		bRetVal = true;

	return bRetVal;
}


// SetFocus
void MWidget::SetFocus()
{
	if ( m_bEnableFocus == false  ||  MWidget::m_pFocusedWidget == this)	return;

	if ( MWidget::m_pFocusedWidget != NULL)		MWidget::m_pFocusedWidget->ReleaseFocus();

	MWidget::m_pFocusedWidget = this;

	OnSetFocus();
}


// ReleaseFocus
void MWidget::ReleaseFocus()
{
	MWidget* pFocus = MWidget::m_pFocusedWidget;
	MWidget::m_pFocusedWidget = NULL;

	if ( pFocus == this)		OnKillFocus();
}


// SetMouseOver
void MWidget::SetMouseOver()
{
	if ( MWidget::m_pMouseOverWidget == this)		return;
	else if ( MWidget::m_pMouseOverWidget != NULL)	MWidget::m_pMouseOverWidget->OnLeave();
	MWidget::m_pMouseOverWidget = this;
	OnEnter();
}


// ReleaseMouseOver
void MWidget::ReleaseMouseOver()
{
	if ( MWidget::m_pMouseOverWidget != NULL)		MWidget::m_pMouseOverWidget->OnLeave();
	MWidget::m_pMouseOverWidget = NULL;
}


// IsMouseIncluded
bool MWidget::IsMouseIncluded()
{
	MWidget* pWidget = m_pMouseOverWidget;
	while ( pWidget != NULL)
	{
		if ( pWidget == this)		return true;

		pWidget = pWidget->GetParent();		
	}

	return false;
}


// SetTimer
void MWidget::SetTimer( int nElapse, DWORD dwEventData)
{
	MGetMint()->GetTimerManager()->Add( this, nElapse, dwEventData);
}


// KillTimer
void MWidget::KillTimer()
{
	MGetMint()->GetTimerManager()->Remove( this);
}


// BindScriptEvent
void MWidget::BindScriptEvent( string& strEventName, string& strScript)
{
	// 이름이 없으면 임의로 지어준다
	if ( m_strName.empty() == true)
	{
		char _buff[ 256];
		sprintf_s( _buff, "__unnamed%u", g_dwUnnamedCount++);
		m_strName = _buff;
	}

	MEventMsg nMsg = FindEventMsgFromName( strEventName.c_str());
	if ( nMsg != MEVENT_UNKNOWN)	m_EventHandler.SetFlagScriptEvents( nMsg);

	Mint::GetInstance()->GetScriptManager()->AddScriptEvent( m_strName, strEventName, strScript);
}


// 위젯을 맨 위로 올림
void MWidget::BringToTop()
{
	if ( m_pParent == NULL)		return;

	if ( m_bZOrderChangable == true  &&  m_pParent->m_Children.size() > 1)
	{
		m_pParent->m_Children.remove( this);

		list< MWidget*>::reverse_iterator itr;
		for ( itr = m_pParent->m_Children.rbegin();  itr != m_pParent->m_Children.rend();  itr++)
		{
			if ( (*itr)->GetAlwaysTop() == false)
				break;
		}

		if ( itr != m_pParent->m_Children.rend())
			m_pParent->m_Children.insert( itr.base(), this);
		else
			m_pParent->m_Children.push_front( this);
	}

	m_pParent->BringToTop();
}


// MakeLocalEvent
void MWidget::MakeLocalEvent( MEvent* pLoalEvent, const MEvent* pEvent)
{
	memcpy( pLoalEvent, pEvent, sizeof( MEvent));

	MRECT windowrect = GetWindowRect();
	MRECT workrect = GetWorkRect();
	pLoalEvent->Pos.x -= windowrect.x + workrect.x;
	pLoalEvent->Pos.y -= windowrect.y + workrect.y;
}


// GetDrawState
int MWidget::GetDrawState()
{
	if ( m_bShow == false  ||  m_fOpacity == 0.0f)		return MDS_NOTDRAW;
	return MDS_DRAW;
}


// InitMouseState
void MWidget::InitMouseState() 
{ 
	m_bLButtonDown = false;
	m_bRButtonDown = false;
}


// EventResizeLayer
void MWidget::EventResizeLayer()
{
	if ( IsLayered() == false)		return;

	
	if ( m_Rect.w != MGetMint()->GetWorkspaceWidth()  ||  m_Rect.h != MGetMint()->GetWorkspaceHeight())
		SetSize( MGetMint()->GetWorkspaceWidth(), MGetMint()->GetWorkspaceHeight());

	if ( m_Rect.x != 0  ||  m_Rect.y != 0)
		SetPosition( 0, 0);


	for ( list< MWidget*>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
		(*itr)->EventResizeLayer();
}


// EventDragBegin
bool MWidget::EventDragBegin( MDragData* pDragData, MPOINT& pt)
{
	m_bLButtonDown = false;
	m_bRButtonDown = false;

	if ( MGetMint()->GetDragManager()->IsExternAdded() == true)
		return MWidget::OnDragBegin( pDragData, pt);

	return OnDragBegin( pDragData, pt);
}


// EventDrawItemBkgrnd
void MWidget::EventDrawItemBkgrnd( MDrawContext* pDC, int index, int subitem, MRECT& r)
{
	OnDrawItemBkgrnd( pDC, index, subitem, r);
}


// EventPreDrawItem
bool MWidget::EventPreDrawItem( MDrawContext* pDC, int index, int subitem, MRECT& r)
{
	return OnPreDrawItem( pDC, index, subitem, r);
}


// EventDrawItem
void MWidget::EventDrawItem( MDrawContext* pDC, int index, int subitem, MRECT& r)
{
	OnDrawItem( pDC, index, subitem, r);
}


// OnPreFilterMessage
bool MWidget::OnPreFilterMessage( MEvent* pEvent)
{
	return false;
}


// OnPreTranslateMsg
MRESULT MWidget::OnPreTranslateMsg( MEvent* pEvent)
{
	return MNOTPROCESSED;
}


// OnEvent
bool MWidget::OnEvent( MEvent* pEvent, MListener* pListener)
{
	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN:
		m_bLButtonDown = true;
		OnLButtonDown( pEvent);
		break;

	case MWM_LBUTTONUP:
		{
			bool bIsLButtonDown = m_bLButtonDown;

			m_bLButtonDown = false;
			OnLButtonUp( pEvent);
			if ( bIsLButtonDown == true  &&  MWidget::m_pMouseOverWidget == this)
			{
				if ( pListener != NULL)
				{
					if ( pListener->OnCommand( this, MEVENT_CLICK) == false)
						OnClick( pEvent);
				}
				else
					OnClick( pEvent);
			}
			break;
		}

	case MWM_LBUTTONDBLCLK:
		m_bLButtonDown = true;
		if ( pListener != NULL)
		{
			if ( pListener->OnCommand( this, MEVENT_DBLCLICK) == false)
				OnDblClick( pEvent);
		}
		else
			OnDblClick( pEvent);
		break;

	case MWM_RBUTTONDOWN:
		m_bRButtonDown = true;
		OnRButtonDown( pEvent);
		break;

	case MWM_RBUTTONUP:
		{
			bool bIsRButtonDown = m_bRButtonDown;

			m_bRButtonDown = false;
			OnRButtonUp( pEvent);
			if ( bIsRButtonDown == true  &&  MWidget::m_pMouseOverWidget == this)
			{
				if ( pListener != NULL)
				{
					if ( pListener->OnCommand( this, MEVENT_RCLICK) == false)
						OnRClick( pEvent);
				}
				else
					OnRClick( pEvent);
			}
			break;
		}

	case MWM_RBUTTONDBLCLK:
		m_bRButtonDown = true;
		break;

	case MWM_MOUSEMOVE:
		OnMouseMove( pEvent);
		break;

	case MWM_MOUSEWHEEL:
		OnMouseWheel( pEvent);
		break;

	case MWM_KEYDOWN:
		{
			OnKeyDown( pEvent);

			switch ( pEvent->nKey)
			{
			case VK_RETURN :
				if ( pListener == NULL  ||  pListener->OnCommand( this, MEVENT_OK) == false)		OnOK();
				break;

			case VK_ESCAPE:
				if ( pListener == NULL  ||  pListener->OnCommand( this, MEVENT_CANCEL) == false)	OnCancel();
				break;
			}
			break;
		}

	case MWM_KEYUP:
		OnKeyUp( pEvent);
		break;

	case MWM_CHAR:
		OnChar( pEvent);
		break;
	};

	return false;
}


// OnQueryHotKey
bool MWidget::OnQueryHotKey( unsigned int nHotKey)
{
	return false;
}


// OnHotKey
void MWidget::OnHotKey( unsigned int nHotKey)
{
}


// OnCommand
bool MWidget::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if ( pWidget == GetScrollBar()  &&  nMsg == MEVENT_VALUECHANGED)
	{
		OnScrollBarChanged( GetScrollBar()->GetScrollValue());
		return true;
	}

	return false;
}


// OnLoaded
void MWidget::OnLoaded()
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnScriptEvent( this, MEVENT_LOADED);
}


// OnChangedLookAndFill
void MWidget::OnChangedLookAndFill()
{
}


// OnRun
void MWidget::OnRun( DWORD dwCurrTime)
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnScriptEvent( this, MEVENT_UPDATE);
}


// OnPreDraw
void MWidget::OnPreDraw( MDrawContext* pDC)
{
	MWidget::m_EventArgs.SetFromEvent( this, NULL);
	m_EventHandler.OnScriptEvent( this, MEVENT_PREDRAW);
}


// OnDrawBorder
void MWidget::OnDrawBorder( MDrawContext* pDC)
{
	if ( m_pLookAndFill)	m_pLookAndFill->OnDrawBorder( this, pDC);
}


// OnDrawItemBkgrnd
void MWidget::OnDrawItemBkgrnd( MDrawContext* pDC, int index, int subitem, MRECT& r)
{
	MWidget::m_EventArgs.SetFromEvent( this, NULL);
	MWidget::m_EventArgs.m_nItemIndex = index;
	MWidget::m_EventArgs.m_nItemSubItem = subitem;
	MWidget::m_EventArgs.m_ItemRect = r;
	m_EventHandler.OnScriptEvent( this, MEVENT_DRAWITEMBKGRND);
}


// OnPreDrawItem
bool MWidget::OnPreDrawItem( MDrawContext* pDC, int index, int subitem, MRECT& r)
{
	MWidget::m_EventArgs.SetFromEvent( this, NULL);
	MWidget::m_EventArgs.m_nItemIndex = index;
	MWidget::m_EventArgs.m_nItemSubItem = subitem;
	MWidget::m_EventArgs.m_ItemRect = r;
	return m_EventHandler.OnScriptEvent( this, MEVENT_PREDRAWITEM);
}


// OnDrawItem
void MWidget::OnDrawItem( MDrawContext* pDC, int index, int subitem, MRECT& r)
{
	MWidget::m_EventArgs.SetFromEvent( this, NULL);
	MWidget::m_EventArgs.m_nItemIndex = index;
	MWidget::m_EventArgs.m_nItemSubItem = subitem;
	MWidget::m_EventArgs.m_ItemRect = r;
	m_EventHandler.OnScriptEvent( this, MEVENT_DRAWITEM);
}


// OnDraw
void MWidget::OnDraw( MDrawContext* pDC)
{
	if ( m_pLookAndFill)	m_pLookAndFill->OnDraw( this, pDC);

	MWidget::m_EventArgs.SetFromEvent( this, NULL);
	m_EventHandler.OnScriptEvent( this, MEVENT_DRAW);
}


// OnShow
void MWidget::OnShow( bool bShow)
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnCommand( this, MEVENT_SHOW);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_SHOW);
}


// OnEnable
void MWidget::OnEnable( bool bEnabled)
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnCommand( this, MEVENT_ENABLE);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_ENABLE);
}


// OnActivate
void MWidget::OnActivate( bool bActivate)
{
	if ( bActivate == false)
	{
		if ( MWidget::m_pFocusedWidget == this)		ReleaseFocus();
		if ( MWidget::m_pCapturedWidget == this)	ReleaseCapture();
		if ( MWidget::m_pMouseOverWidget == this)	ReleaseMouseOver();

		InitMouseState();
	}

	m_bLButtonDown = false;
	m_bRButtonDown = false;


	for ( list< MWidget*>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
	{
		MWidget* pChild = (*itr);
		if ( pChild)	pChild->OnActivate( bActivate);
	}
}


// OnSetFocus
void MWidget::OnSetFocus()
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnCommand( this, MEVENT_SETFOCUS);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_SETFOCUS);
}


// OnKillFocus
void MWidget::OnKillFocus()
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnCommand( this, MEVENT_KILLFOCUS);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_KILLFOCUS);
}


// OnPosition
void MWidget::OnPosition( int x, int y)
{
	MWidget::m_RunEventOnPosition = true;

	MWidget::m_EventArgs.SetFromEvent( this);
	MWidget::m_EventArgs.m_ItemRect = m_Rect;
	if ( m_EventHandler.OnScriptEvent( this, MEVENT_POSITION) == true)
		m_Rect = MWidget::m_EventArgs.m_ItemRect;

	MWidget::m_RunEventOnPosition = false;
}


// OnSize
void MWidget::OnSize( int w, int h)
{
	MWidget::m_RunEventOnSize = true;

	MWidget::m_EventArgs.SetFromEvent( this);
	MWidget::m_EventArgs.m_ItemRect = m_Rect;
	if ( m_EventHandler.OnScriptEvent( this, MEVENT_SIZE) == true)
		m_Rect = MWidget::m_EventArgs.m_ItemRect;

	MWidget::m_RunEventOnSize = false;
}


// OnLButtonDown
void MWidget::OnLButtonDown( MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	m_EventHandler.OnScriptEvent( this, MEVENT_LBUTTONDOWN);
}


// OnLButtonUp
void MWidget::OnLButtonUp( MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	m_EventHandler.OnScriptEvent( this, MEVENT_LBUTTONUP);
}


// OnRButtonDown
void MWidget::OnRButtonDown( MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	m_EventHandler.OnScriptEvent( this, MEVENT_RBUTTONDOWN);
}


// OnRButtonUp
void MWidget::OnRButtonUp( MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	m_EventHandler.OnScriptEvent( this, MEVENT_RBUTTONUP);
}


// OnMouseMove
void MWidget::OnMouseMove( MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	m_EventHandler.OnScriptEvent( this, MEVENT_MOUSEMOVE);
}


// OnClick
void MWidget::OnClick( MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	m_EventHandler.OnCommand( this, MEVENT_CLICK);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_CLICK);
}


// OnRClick
void MWidget::OnRClick( MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	m_EventHandler.OnCommand( this, MEVENT_RCLICK);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_RCLICK);
}


// OnDblClick
void MWidget::OnDblClick( MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	m_EventHandler.OnCommand( this, MEVENT_DBLCLICK);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_DBLCLICK);
}


// OnMouseWheel
void MWidget::OnMouseWheel( MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	m_EventHandler.OnCommand( this, MEVENT_MOUSEWHEEL);
}


// OnKeyDown
void MWidget::OnKeyDown( MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	m_EventHandler.OnScriptEvent( this, MEVENT_KEYDOWN);
}


// OnKeyUp
void MWidget::OnKeyUp( MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	m_EventHandler.OnScriptEvent( this, MEVENT_KEYUP);
}


// OnChar
void MWidget::OnChar( MEvent* pEvent)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	m_EventHandler.OnScriptEvent( this, MEVENT_CHAR);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_DBLCLICK);
}


// OnScrollBarChanged
void MWidget::OnScrollBarChanged( int nPos)
{
}


// OnSelChanged
void MWidget::OnSelChanged()
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnCommand( this, MEVENT_SELCHANGE);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_SELCHANGE);
}


// OnValueChanged
void MWidget::OnValueChanged()
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnCommand( this, MEVENT_VALUECHANGED);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_VALUECHANGED);
}


// OnItemClick
void MWidget::OnItemClick( MEvent* pEvent, int nIndex)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	MWidget::m_EventArgs.m_nItemIndex = nIndex;
	m_EventHandler.OnCommand( this, MEVENT_ITEMCLICK);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_ITEMCLICK);
}


// OnItemDblClick
void MWidget::OnItemDblClick( MEvent* pEvent, int nIndex)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	MWidget::m_EventArgs.m_nItemIndex = nIndex;
	m_EventHandler.OnCommand( this, MEVENT_ITEMDBLCLICK);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_ITEMDBLCLICK);
}


// OnItemRClick
void MWidget::OnItemRClick( MEvent* pEvent, int nIndex)
{
	MWidget::m_EventArgs.SetFromEvent( this, pEvent);
	MWidget::m_EventArgs.m_nItemIndex = nIndex;
	m_EventHandler.OnCommand( this, MEVENT_ITEMRCLICK);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_ITEMRCLICK);
}


// OnDragBegin
bool MWidget::OnDragBegin( MDragData* pDragData, MPOINT& pt)
{
	MWidget::m_EventArgs.SetFromEvent( this);
	MWidget::m_DragEventArgs.Set( this, pDragData, pt, false);

	if ( m_EventHandler.ExistScriptEvent( MEVENT_DRAGBEGIN) == true)
	{
		if ( m_EventHandler.OnScriptEvent( this, MEVENT_DRAGBEGIN) == false)
		{
			pDragData->Clear();
			return false;
		}
	}

	if ( pDragData->IsEmpty() == true)		return false;

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_DRAGBEGIN);

	return true;
}


// OnDragEnd
bool MWidget::OnDragEnd( MDragData* pDragData, MPOINT& pt, bool bDropped)
{
	MWidget::m_EventArgs.SetFromEvent( this);
	MWidget::m_DragEventArgs.Set( this, pDragData, pt, bDropped);
	m_EventHandler.OnScriptEvent( this, MEVENT_DRAGEND);
	
	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_DRAGEND);
	return false;
}


// OnDrop
bool MWidget::OnDrop( MWidget* pSender, MDragData* pDragData, MPOINT& pt)
{
	MWidget::m_EventArgs.SetFromEvent( this);
	MWidget::m_DragEventArgs.Set( pSender, pDragData, pt, false);

	if ( m_EventHandler.ExistScriptEvent( MEVENT_DROP) == true)
	{
		if ( m_EventHandler.OnScriptEvent( this, MEVENT_DROP) == true)
		{
			if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_DROP);

			return true;
		}
	}

	return false;
}


// OnToolTip
MWidget* MWidget::OnToolTip()
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnScriptEvent( this, MEVENT_TOOLTIP);

	if ( m_strToolTip.empty())		return NULL;

	const char _token[] = "{divide}";
	string::size_type _find_tok = m_strToolTip.find( _token);

	// Create multiple tooltip widget
	if ( _find_tok != string::npos)
	{
		MWidget* pToolTip = new MWidget( NULL, NULL, NULL);

		int _count = 0;
		int _height = 0;
		string _buff = m_strToolTip + string( _token);
		while ( true)
		{
			_find_tok = _buff.find( _token);
			if ( _find_tok == string::npos)
				break;

			string _str = string( _buff.begin(), _buff.begin() + _find_tok);
			if ( _str.empty() == true)
				break;

			_buff = string( _buff.begin() + _find_tok + strlen( _token), _buff.end());


			MTextView* _child = new MTextView( NULL, pToolTip, NULL);
			_child->SetLookAndFill( MDEFAULT_TOOLTIPLOOK);
			_child->GetScrollBar()->SetAlwaysHide( true);
			_child->SetSize( DEFAULT_TOOLTIPWIDTH, 100);
			_child->SetWrapping( false);
			_child->SetText( _str.c_str());


			MSIZE _size = _child->GetPageSize();
			_height = max( _height, _size.h);

			_count++;
		}

		if ( _count == 0)
		{
			delete pToolTip;
			return NULL;
		}


		MLookBase* pLook = MGetMint()->m_pLookManager->Get( MDEFAULT_TOOLTIPLOOK);
		MRECT _bgrect = pLook ? pLook->GetBkgrndRectOffset() : MRECT( 0, 0, 0, 0);

		pToolTip->SetSize( (DEFAULT_TOOLTIPWIDTH + _bgrect.w) * _count, 100);
		MSIZE workspaceSize( MGetMint()->GetWorkspaceWidth(), MGetMint()->GetWorkspaceHeight());
		MPOINT _pos = MToolTipManager::CalcToolTipPosition( MGetMint()->GetLastCursorPos(), pToolTip->GetToolTipOffset(), workspaceSize, pToolTip->GetSize());

		bool bRightAlign = false;
		if ( _pos.x < MGetMint()->GetLastCursorPos().x)		bRightAlign = true;


		MPOINT _p( -_bgrect.x + ( bRightAlign  ?  ( DEFAULT_TOOLTIPWIDTH + _bgrect.w) * ( _count - 1) : 0), -_bgrect.y);
		list< MWidget*>& _list = pToolTip->GetChilList();
		for ( list< MWidget*>::iterator itr = _list.begin();  itr != _list.end();  itr++)
		{
			MWidget* _child = (*itr);
			_child->SetRect( _p.x, _p.y, DEFAULT_TOOLTIPWIDTH, _height);

			MRECT _r = _child->GetBkgrndRect();
			_r.x += _p.x;
			_r.y += _p.y;

			MSIZE _size = pToolTip->GetSize();
			pToolTip->SetSize( max( _r.x + _r.w, _size.w), max( _r.y + _r.h, _size.h));

			_p.x += bRightAlign  ?  -( DEFAULT_TOOLTIPWIDTH + _bgrect.w)  :  ( DEFAULT_TOOLTIPWIDTH + _bgrect.w);
			_p.y += 25;
		}

		return pToolTip;
	}

	// Create default tooltip widget
	else
	{
		MTextView* pToolTip = new MTextView( NULL, NULL, NULL);
		pToolTip->SetLookAndFill( MDEFAULT_TOOLTIPLOOK);
		pToolTip->GetScrollBar()->SetAlwaysHide( true);
		pToolTip->SetSize( DEFAULT_TOOLTIPWIDTH, 100);
		pToolTip->SetWrapping( false);
		pToolTip->SetText( m_strToolTip.c_str());


		MSIZE _size = pToolTip->GetPageSize();
		pToolTip->SetSize( _size);

		return pToolTip;
	}

	return NULL;
}



// OnOK
void MWidget::OnOK()
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnCommand( this, MEVENT_OK);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_OK);
}


// OnCancel
void MWidget::OnCancel()
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnCommand( this, MEVENT_CANCEL);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_CANCEL);
}


// OnEnter
void MWidget::OnEnter()
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnScriptEvent( this, MEVENT_ENTER);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_ENTER);
}


// OnLeave
void MWidget::OnLeave()
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnScriptEvent( this, MEVENT_LEAVE);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_LEAVE);
}


// OnSetCursor
bool MWidget::OnSetCursor( MEvent* pEvent, string* pCursor)
{
	MWidget::m_EventArgs.SetFromEvent( this);
	MWidget::m_EventArgs.m_strArgument = *pCursor;
	
	bool bRetVal = m_EventHandler.OnScriptEvent( this, MEVENT_SETCURSOR);
	if ( bRetVal == true)
		*pCursor = MWidget::m_EventArgs.m_strArgument;

	return bRetVal;
}


// OnInitPopupMenu
void MWidget::OnInitPopupMenu( MPopupMenu* pPopupMenu)
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnScriptEvent( this, MEVENT_INITPOPUPMENU);

	if ( m_pLookAndFill)	m_pLookAndFill->OnEvent( this, MEVENT_INITPOPUPMENU);
}


// OnTimer
void MWidget::OnTimer( DWORD dwEventData)
{
	MWidget::m_EventArgs.SetFromEvent( this);
	m_EventHandler.OnScriptEvent( this, MEVENT_TIMER);
}


// OnNcHitTest
int MWidget::OnNcHitTest( MEvent* pEvent)
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
			_hittest = MHTCLIENT;
	}

	return _hittest;
}


// OnUserArgument
bool MWidget::OnUserArgument( const char* szArgument)
{
	MWidget::m_EventArgs.SetFromEvent( this);
	MWidget::m_EventArgs.m_strArgument = szArgument;
	return m_EventHandler.OnScriptEvent( this, MEVENT_USERARGUMENT);
}


// glueSetText
void MWidget::glueSetText( const char* szText)
{
	if ( szText == NULL)	SetText( "");
	else					SetText( szText);
}


// glueSetToolTip
void MWidget::glueSetToolTip( const char* szToolTip)
{
	if ( szToolTip == NULL)		SetToolTipText( "");
	else						SetToolTipText( szToolTip);
}


// glueModifyToolTip
void MWidget::glueModifyToolTip()
{
	MGetMint()->ChangedToolTipText( this);
}


// glueGetRect
MWLua::ret_int4 MWidget::glueGetRect()
{
	return MWLua::ret_int4( m_Rect.x, m_Rect.y, m_Rect.w, m_Rect.h);
}


// glueGetClientRect
MWLua::ret_int4 MWidget::glueGetClientRect()
{
	MRECT r = GetClientRect();
	return MWLua::ret_int4( r.x, r.y, r.w, r.h);
}


// glueGetWindowRect
MWLua::ret_int4 MWidget::glueGetWindowRect()
{
	MRECT r = GetWindowRect();
	return MWLua::ret_int4( r.x, r.y, r.w, r.h);
}


// glueSetScrollValue
int MWidget::glueSetScrollValue( int nValue)
{
	MScrollBar* pScrollBar = GetScrollBar();
	if ( pScrollBar == NULL)		return 0;

	return pScrollBar->SetScrollValue( nValue);
}


// glueGetScrollValue
int MWidget::glueGetScrollValue()
{
	int nValue = 0;
	
	MScrollBar* pScrollBar = GetScrollBar();
	if ( pScrollBar != NULL)		nValue = pScrollBar->GetScrollValue();
	
	return nValue;
}


// glueSetScrollThumbLen
void MWidget::glueSetScrollThumbLen( int nLength)
{
	MScrollBar* pScrollBar = GetScrollBar();
	if ( pScrollBar == NULL)		return;

	return pScrollBar->SetThumbLength( nLength);
}


// glueTrackPopupMenu
bool MWidget::glueTrackPopupMenu( const char* szPopupMenu)
{
	if ( szPopupMenu == NULL)		return false;

	MWidget* pPopupMenu = MGetMint()->FindWidget( szPopupMenu);
	if ( pPopupMenu == NULL  ||  strcmp( pPopupMenu->GetClassName(), MINT_POPUPMENU) != 0)
		return false;

	MGetMint()->ClearToolTip();

	POINT p;
	GetCursorPos( &p);
	ScreenToClient( MGetMint()->GetHWND(), &p);
	return ((MPopupMenu*)pPopupMenu)->TrackPopupMenu( MPOINT( p.x, p.y), this);
}


// glueSendUserArgument
bool MWidget::glueSendUserArgument( const char* szArgument, bool bSendChild)
{
	return SendUserArguments( szArgument, bSendChild);
}


// glueAttachDragItem
void MWidget::glueAttachDragItem( const char* szText, const char* szImage, int nUserData)
{
	MBitmap* pBitmap = NULL;
	if ( szImage != NULL)	pBitmap = MBitmapManager::Get( szImage);
	MDragObject* pObject = new MDragObject( string( szText), pBitmap, nUserData);

	MDragManager* pDragManager = MGetMint()->GetDragManager();
	pDragManager->AddDragData( this, pObject);
}




///////////////////////////////////////////////////////////////


// MClientToScreen
MPOINT MClientToScreen( MWidget* pWidget, MPOINT& p)
{
	MRECT r = pWidget->GetWindowRect();
	return MPOINT(p.x+r.x, p.y+r.y);
}

MRECT MClientToScreen( MWidget* pWidget, MRECT& p)
{
	MRECT r = pWidget->GetWindowRect();
	return MRECT( p.x+r.x, p.y+r.y, p.w, p.h);
}


// MScreenToClient
MPOINT MScreenToClient( MWidget* pWidget, MPOINT& p)
{
	MRECT r = pWidget->GetWindowRect();
	return MPOINT( p.x-r.x, p.y-r.y);
}

MRECT MScreenToClient( MWidget* pWidget, MRECT& p)
{
	MRECT r = pWidget->GetWindowRect();
	return MRECT( p.x-r.x, p.y-r.y, p.w, p.h);
}


///////////////////////////////////////////////////////////////


int GetAndPos(const char* szText)
{
	if(szText==NULL) return -1;
	int nLen = strlen(szText);
	for(int i=0; i<nLen; i++){
		if(szText[i]=='&') return i;
	}
	return -1;
}

char GetAndChar(const char* szText)
{
	int nLen = GetAndPos(szText);
	if(nLen==-1) return 0;
	return szText[nLen+1];
}

int RemoveAnd(char* szText)
{
	if(szText==NULL) return -1;
	int nPos = GetAndPos(szText);
	if(nPos==-1) return -1;
	int nLen = strlen(szText);
	for(int i=0; i<(nLen-nPos); i++){
		szText[nPos+i] = szText[nPos+i+1];
	}
	return nPos;
}

int RemoveAnd(char* szRemovedText, const char* szText)
{
	if(szText==NULL) return -1;
	strcpy(szRemovedText, szText);
	return RemoveAnd(szRemovedText);
}

int RemoveAnd(char* szRemovedFrontText, char* cUnderLineChar, char* szRemovedBackText, const char* szText)
{
	if(szText==NULL) return -1;
	int nPos = GetAndPos(szText);
	if(nPos==-1) return -1;
	memcpy(szRemovedFrontText, szText, nPos);
	szRemovedFrontText[nPos] = 0;
	*cUnderLineChar = szText[nPos+1];
	strcpy(szRemovedBackText, szText+nPos+2);
	return nPos;
}

MPOINT GetCursorPosition(void)
{
	POINT p;
	GetCursorPos(&p);
	MPOINT mp;
	mp.x = p.x;
	mp.y = p.y;
	return mp;
}



} // namespace mint3

