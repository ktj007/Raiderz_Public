#include "stdafx.h"
#include "XUIChattingBox.h"
#include "Mint.h"
#include "MLookManager.h"
#include "XConsoleCmds.h"
#include "XPost_Msg.h"
#include "XStrings.h"


#define DEFAULT_CHATBOXID		1000
#define DEFAULT_CHATTABID		1000
#define MARGIN					2
#define RESIZEBOX_SIZE			5
#define MAGNETDIST				10
#define MAGNETIC_BORDERSIZE		5
#define MAX_MESSAGECACHE		50
#define TIME_LAZYSEND			300


enum CLICK_BUTTON { NONE = 0, CHATBOX, ADDER, REMOVER, PROPERTY };





/************************************************************************
 class : XChatButton
*************************************************************************/
// XChatButton
XChatButton::XChatButton( const char* szName, MWidget* pParent, MListener* pListener)
:MButton( szName, pParent, pListener)
{
}






/************************************************************************
 class : XChatScrollBar
*************************************************************************/

// XChatScrollBar
XChatScrollBar::XChatScrollBar( const char* szName, MWidget* pParent, MListener* pListener)
: MScrollBar( szName, pParent, pListener, MSBS_VERT | MSBS_LEFTALIGN | MSBS_ALWAYSSHOW)
{
}


// CalculateAlignRect
void XChatScrollBar::CalculateAlignRect()
{
	if ( IsAligned()  &&  m_pParent != NULL)
	{
		int _w = GetWidth();
		MRECT r = m_pParent->GetClientRect();
		if ( IsHorz())
		{
			if ( m_nStyle & MSBS_TOPALIGN)		SetRect( 0, -GetWidth(), r.w - GetWidth(), _w);
			else /* Bottom align */				SetRect( 0, r.h, r.w - GetWidth(), _w);
		}
		else /* Vertical */
		{
			if ( m_nStyle & MSBS_LEFTALIGN)		SetRect( -GetWidth(), 0, _w, r.h - GetWidth());
			else /* Right align */				SetRect( r.w, 0, _w, r.h - GetWidth());
		}
	}
}







/************************************************************************
 class : XChatTab
*************************************************************************/

// XChatTab
XChatTab::XChatTab( const char* szName, MWidget* pParent, MListener* pListener)
: MButton( szName, pParent, pListener)
{
}


// ~XChatTab
XChatTab::~XChatTab()
{
}


// OnDrawBorder
void XChatTab::OnDrawBorder( MDrawContext* pDC)
{
	float fRatio = (float)( (XChatBox*)GetParent())->GetFadeRatio();
	pDC->SetOpacity( max( XUIChattingBox::GetInstance()->GetOpacity(), fRatio));

	MButton::OnDrawBorder( pDC);
}


// OnDraw
void XChatTab::OnDraw( MDrawContext* pDC)
{
	float _old_opacity = ( GetCheck() == true)  ?  pDC->SetOpacity( 1.0f)  :  pDC->GetOpacity();

	MButton::OnDraw( pDC);

	pDC->SetOpacity( _old_opacity);
}


// OnDblClick
void XChatTab::OnDblClick( MEvent* pEvent)
{
	MWidget::OnClick( pEvent);

	XUIChattingBox::GetInstance()->EventClick( m_dwUserData, PROPERTY);
}


// OnDragBegin
bool XChatTab::OnDragBegin( MDragData* pDragData, MPOINT& pt)
{
	if ( XUIChattingBox::GetInstance()->IsFixed() == false)
	{
		DWORD dwUserData = GetUserData();
		if ( dwUserData > 0  &&  dwUserData != DEFAULT_CHATTABID)
		{
			MDragObject* pObject = new MDragObject( m_strText, m_pIcon, dwUserData);
			pDragData->AddObject( pObject);


			if ( HasLookAndFill())
			{
				MFont* pFont = m_pLookAndFill->GetFont();
				if ( pFont)
				{
					pDragData->SetFont( m_pLookAndFill->GetFont());
					pDragData->SetHotSpot( MPOINT( 0, pFont->GetHeight()));
				}
			}

			XUIChattingBox::GetInstance()->SetForcedShowState( 1);
		}
	}

	return MWidget::OnDragBegin( pDragData, pt);
}


// OnDragEnd
bool XChatTab::OnDragEnd( MDragData* pDragData, MPOINT& pt, bool bDropped)
{
	XUIChattingBox::GetInstance()->SetForcedShowState( 0);


	// Drop from TAP to null
	if ( bDropped == false)
	{
		XUIChattingBox* pChattingBox = XUIChattingBox::GetInstance();
		if ( pChattingBox == NULL)				return true;

		XChatCtrl* pChatCtrl = pChattingBox->GetChatCtrl( m_dwUserData);
		if ( pChatCtrl == NULL)					return true;

		XChatBox* pChatBox = pChatCtrl->GetChatBox();
		if ( pChatBox == NULL)					return true;


		if ( pChatBox->GetChatCtrlCountInBox() > 1)
		{
			pChatBox->DetachChatCtrl( pChatCtrl);

			MRECT r = pChatBox->GetRect();
			r.x = pt.x;
			r.y = pt.y;

			XChatBox* pNewChatBox = pChattingBox->CreateNewChatBox( XUIChattingBox::GetInstance()->GetNewChatBoxID(), r);
			pNewChatBox->AttachChatCtrl( pChatCtrl);
		}
	}

	return true;
}


// OnDrop
bool XChatTab::OnDrop( MWidget* pSender, MDragData* pDragData, MPOINT& pt)
{
	// Drop from TAP to TAP
	if ( strcmp( pSender->GetClassName(), MINT_CHATTINGBOXTAB) != 0)
		return true;


	DWORD dwUserData = pDragData->GetItemData( 0);
	if ( dwUserData == 0)						return true;

	
	XUIChattingBox* pChattingBox = XUIChattingBox::GetInstance();
	if ( pChattingBox == NULL)					return true;

	
	// Source
	XChatCtrl* pSrcChatCtrl = pChattingBox->GetChatCtrl( (int)dwUserData);
	if ( pSrcChatCtrl == NULL)					return true;

	XChatBox* pSrcChatBox = pSrcChatCtrl->GetChatBox();
	if ( pSrcChatBox == NULL)					return true;


	// Target
	XChatCtrl* pTarChatCtrl = pChattingBox->GetChatCtrl( (int)m_dwUserData);
	if ( pTarChatCtrl == NULL)					return true;

	XChatBox* pTarChatBox = pTarChatCtrl->GetChatBox();
	if ( pTarChatBox == NULL)					return true;


	if ( pSrcChatCtrl == pTarChatCtrl)			return true;


	pSrcChatBox->DetachChatCtrl( pSrcChatCtrl);
	if ( pSrcChatBox != pTarChatBox)
	{
		if ( pSrcChatBox->GetChatCtrlCountInBox() <= 0)
		{
			pSrcChatBox->Delete();
			delete pSrcChatBox;
		}
	}

	pTarChatBox->AttachChatCtrl( pSrcChatCtrl, pTarChatCtrl);

	return true;
}







/************************************************************************
 class : XChatView
*************************************************************************/

// XChatView
XChatView::XChatView( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	m_bEnableFocus = false;
	m_Anchor = MAnchorStyle( true, true, true, true);

	m_pScrollBar = new XChatScrollBar( NULL, this, this);

	m_pEndLine = new MButton( NULL, this, this);
	m_pEndLine->Enable( false);
	m_pEndLine->SetAnchor( MAnchorStyle( true, false, false, true));
	m_pEndLine->SetRect( -16, m_Rect.h - 16, 16, 16);

	m_nTextSize = CTS_NORMAL;

	m_tMove.SetEnable( false);
	m_tMove.SetArrivalTime( 400);

	m_bUpdateScrollBar = false;
	m_bArrangeChildren = false;
}


// ~XChatView
XChatView::~XChatView()
{
	m_Children.clear();

	if ( m_pScrollBar != NULL)
	{
		delete m_pScrollBar;
		m_pScrollBar = NULL;
	}

	if ( m_pEndLine != NULL)
	{
		delete m_pEndLine;
		m_pEndLine = NULL;
	}
}


// AddChat
bool XChatView::AddChat( const char* szString)
{
	MRECT r = GetClientRect();

	string strMsg = "{SECONDENT dent=20}";
	strMsg += szString;

	MTextView* pTextView = new MTextView( NULL, this, this);
	MLookBase* pLook = XUIChattingBox::GetInstance()->GetTextLook( m_nTextSize);
	if ( pLook != NULL)
		pTextView->SetLookAndFill( pLook);

	pTextView->SetEnableFocus( false);
	pTextView->SetStatic( true);
	pTextView->SetAnchor( MAnchorStyle( true, true, true, false));
	pTextView->GetScrollBar()->SetAlwaysHide( true);
	pTextView->SetWrapping( false);
	pTextView->SetRect( 0, m_Rect.h, r.w, 100);
	pTextView->SetText( strMsg.c_str());

	// Resize and reposition
	pTextView->SetSize( r.w, pTextView->GetPageSize().h);


	// Add child list
	m_Children.push_back( _CHILD( pTextView));

	if ( m_Children.size() > MAX_MESSAGECACHE)
	{
		(*m_Children.begin())._delete();
		m_Children.pop_front();
	}


	// End line button blanking
	if ( m_pScrollBar->GetScrollValue() != m_pScrollBar->GetMax())
	{
		m_pEndLine->SetBlanking( true);
		m_pEndLine->Enable( true);
	}


	m_tMove.SetEnable( true);
	m_tMove.Start();


	m_bUpdateScrollBar = true;
	m_bArrangeChildren = true;

	return true;
}


// ClearChat
void XChatView::ClearChat()
{
	for ( list< _CHILD>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
		(*itr)._delete();

	m_Children.clear();


	m_pScrollBar->SetScrollRange( 0, 0);
	m_pScrollBar->SetScrollValue( 0);
}


// ScrollPage
void XChatView::ScrollPage( int delta)
{
	int _value = m_pScrollBar->GetScrollValue() + delta;

	if ( _value > m_pScrollBar->GetMax())	m_pScrollBar->SetScrollValue( m_pScrollBar->GetMax());
	else if ( _value < 0)					m_pScrollBar->SetScrollValue( 0);

	m_pScrollBar->SetScrollValue( _value);

	if ( m_pScrollBar->GetScrollValue() == m_pScrollBar->GetMax())
	{
		m_pEndLine->SetBlanking( false);
		m_pEndLine->Enable( false);
	}
	else
		m_pEndLine->Enable( true);

	m_bArrangeChildren = true;
}


// SetTextSize
void XChatView::SetTextSize( int nSize)
{
	if ( m_nTextSize == nSize)
		return;

	m_nTextSize = nSize;
	MLookBase* pLook = XUIChattingBox::GetInstance()->GetTextLook( nSize);
	if ( pLook == NULL)
		return;

	for ( list< _CHILD>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
		(*itr).m_pTextView->SetLookAndFill( pLook);

	m_bUpdateScrollBar = true;
	m_bArrangeChildren = true;
}


// OnCommand
bool XChatView::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if ( pWidget == m_pEndLine  &&  nMsg == MEVENT_CLICK)
	{
		m_pEndLine->SetBlanking( false);
		m_pEndLine->Enable( false);

		m_pScrollBar->SetScrollValue( m_pScrollBar->GetMax());

		m_bArrangeChildren = true;
		return true;
	}

	return MWidget::OnCommand( pWidget, nMsg, szArgs);
}


// OnEvent
bool XChatView::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)		return true;


	switch( pEvent->nMessage)
	{
	case MWM_MOUSEWHEEL :
		ScrollPage( -(int)( (float)(pEvent->nDelta * 0.1f)));
		return true;
	}

	return false;
}


// OnRun
void XChatView::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);


	// Reposition end-line button
	if ( m_pEndLine != NULL)
	{
		MRECT _r = GetClientRect();
		int nWidth = m_pScrollBar  ?  m_pScrollBar->GetWidth() : 16;
		m_pEndLine->SetRect( -nWidth, _r.h - nWidth, nWidth, nWidth);
	}


	// Update scrollbar
	if ( m_bUpdateScrollBar == true)
	{
		MRECT r = GetClientRect();

		int nHeight = 0;
		for ( list< _CHILD>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
			nHeight += (*itr).m_pTextView->GetSize().h;


		if ( nHeight <= r.h)
		{
			m_pScrollBar->SetScrollRange( 0, 0);
			m_pScrollBar->SetScrollValue( 0);
		}
		else
		{
			bool bLastLine = false;
			int _val = m_pScrollBar->GetScrollValue();
			if ( _val >= m_pScrollBar->GetMax())	bLastLine = true;

			int _max = nHeight - r.h;
			m_pScrollBar->SetScrollRange( 0, _max);
			m_pScrollBar->SetScrollValue( bLastLine == true ? _max : min( _val, _max));
		}

		m_bUpdateScrollBar = false;
		m_bArrangeChildren = true;
	}


	// Arrange child
	if ( m_bArrangeChildren == true  ||  m_tMove.IsEnabled() == true)
	{
		MRECT r = GetClientRect();
		float fRatio = (float)m_tMove.GetRatio();
		int nPosY = GetClientRect().h + (m_pScrollBar->GetMax() - m_pScrollBar->GetScrollValue());

		for ( list< _CHILD>::reverse_iterator itr = m_Children.rbegin();  itr != m_Children.rend();  itr++)
		{
			MTextView* pTextView = (*itr).m_pTextView;

			MSIZE _page_size = pTextView->GetPageSize();
			pTextView->SetSize( r.w, _page_size.h + 1);

			if ( itr == m_Children.rbegin())
			{
				pTextView->SetPosition( 0, nPosY - _page_size.h + (int)( 5.0f * (1.0f - fRatio)) );

				nPosY -= (int)( (float)_page_size.h * fRatio);
				pTextView->SetOpacity( fRatio * 0.5f + 0.5f);
			}
			else
			{
				pTextView->SetPosition( 0, nPosY - _page_size.h);
				pTextView->SetOpacity( 1.0f);

				nPosY -= _page_size.h;
			}
		}

		if ( m_tMove.IsArrival() == true)		m_tMove.SetEnable( false);
		m_bArrangeChildren = false;
	}
}


// OnSize
void XChatView::OnSize( int w, int h)
{
	MWidget::OnSize( w, h);

	m_bUpdateScrollBar = true;
	m_bArrangeChildren = true;
}


// OnScrollBarChanged
void XChatView::OnScrollBarChanged( int nPos)
{
	MWidget::OnScrollBarChanged( nPos);

	if ( m_pScrollBar->GetScrollValue() == m_pScrollBar->GetMax())
	{
		m_pEndLine->SetBlanking( false);
		m_pEndLine->Enable( false);
	}
	else
		m_pEndLine->Enable( true);

	m_bArrangeChildren = true;
}











/************************************************************************
 class : XChatCtrl
*************************************************************************/

// XChatCtrl
XChatCtrl::XChatCtrl( MWidget* pParent)
{
	m_pChatTab = NULL;
	m_pChatView = NULL;


	if ( pParent == NULL)		return;
	MRECT _pr = pParent->GetClientRect();
	int _height = XUIChattingBox::GetInstance()->GetHeaderHeight();

	
	m_pChatTab = new XChatTab( NULL, pParent, pParent);
	if ( m_pChatTab == NULL)
	{
		m_pChatTab = NULL;
		m_pChatView = NULL;

		return;
	}

	m_pChatTab->SetType( MBT_PUSHBTN);
	m_pChatTab->SetRect( 0, 0, 50, _height);
	m_pChatTab->SetUserData( 0);


	m_pChatView = new XChatView( NULL, pParent, pParent);
	if ( m_pChatView == NULL)
	{
		m_pChatTab = NULL;
		m_pChatView = NULL;

		return;
	}
	m_pChatView->Show( false);
	m_pChatView->SetRect( 0, _height + 5, _pr.w, _pr.h - _height - 5);
}


// XChatCtrl
XChatCtrl::~XChatCtrl()
{
	Delete();
}


// GetChatBox
XChatBox* XChatCtrl::GetChatBox()
{
	return (XChatBox*)m_pChatTab->GetParent();
}


// Delete
bool XChatCtrl::Delete()
{
	if ( m_pChatTab != NULL)
	{
		m_pChatTab->RemoveFromParent();
		delete m_pChatTab;
		m_pChatTab = NULL;
	}

	if ( m_pChatView != NULL)
	{
		m_pChatView->RemoveFromParent();
		delete m_pChatView;
		m_pChatView = NULL;
	}

	return true;
}


// AddChat
void XChatCtrl::AddChat( int nType, const char* szChat)
{
	// Filtering
	for ( vector< int>::iterator itr = m_TypeFilter.begin();  itr != m_TypeFilter.end();  itr++)
	{
		if ( (*itr) == nType)
			return;
	}

	if ( m_pChatView != NULL)
		m_pChatView->AddChat( szChat);
}


// Show
void XChatCtrl::Show( bool bShow)
{
	if ( m_pChatTab != NULL)		m_pChatTab->SetCheck( bShow);
	if ( m_pChatView != NULL)		m_pChatView->Show( bShow);
}











/************************************************************************
 class : XChatBox
*************************************************************************/

// XChatBox
XChatBox::XChatBox( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	m_bEnableFocus = true;
	m_bZOrderChangable = true;
	m_Anchor = MAnchorStyle( false, false, false, false);


	m_nID = 0;
	m_pCurSel = NULL;
	m_pChainedParent = NULL;
	m_pChainedChild = NULL;
	m_bMoving = false;
	m_nResizing = 0;
	m_bArrangeChatCtrlTab = true;


	m_pShowProperty = new XChatButton( NULL, this, this);
	m_pShowProperty->SetAnchor( MAnchorStyle( false, true, true, false));
	m_pShowProperty->SetRect( 0, 0, 16, 16);

	m_pRemoveChatCtrl = new XChatButton( NULL, this, this);
	m_pRemoveChatCtrl->SetAnchor( MAnchorStyle( true, false, true, false));
	m_pRemoveChatCtrl->SetRect( 0, 0, 16, 16);

	m_pAddChatCtrl = new XChatButton( NULL, this, this);
	m_pAddChatCtrl->SetAnchor( MAnchorStyle( true, false, true, false));
	m_pAddChatCtrl->SetRect( 0, 0, 16, 16);


	m_tFade.SetReverse( false);
	m_tFade.SetArrivalTime( 300);


	// Message hooking
	m_pFiltered = pParent;
	while ( m_pFiltered != (MWidget*)MGetMint()->GetMainFrame())
		m_pFiltered = m_pFiltered->GetParent();

	m_pFiltered->AddMessageFilter( this);
}


// ~XChatBox
XChatBox::~XChatBox()
{
	if ( m_pFiltered != NULL)
		m_pFiltered->DeleteMessageFilter( this);


	// Delete child
	Delete();
}


// GetDrawState
int XChatBox::GetDrawState()
{
	if ( m_bShow == false)		return MDS_NOTDRAW;
	return MDS_DRAW;
}


// GetChatCtrl
XChatCtrl* XChatBox::GetChatCtrl( int nID)
{
	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		XChatCtrl* pChatCtrl = (*itr);
		if ( pChatCtrl->GetID() == nID)		return pChatCtrl;
	}

	
	if ( m_pChainedChild != NULL)
		return m_pChainedChild->GetChatCtrl( nID);
	
	return NULL;
}


// GetChatCtrlIndex
XChatCtrl* XChatBox::GetChatCtrlIndex( int nIndex)
{
	list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();

	for ( int i = 0;  i < nIndex;  i++,  itr++) ;
	return (*itr);
}


// CreateNewChatBox
XChatBox* XChatBox::CreateNewChatBox( int nID, MRECT& r)
{
	if ( nID == m_nID)
	{
		SetRect( r);
		return this;
	}

	if ( m_pChainedChild != NULL)		return m_pChainedChild->CreateNewChatBox( nID, r);


	XChatBox* pChatBox = new XChatBox( NULL, m_pParent, m_pParent);
	if ( pChatBox != NULL)
	{
		pChatBox->SetID( nID);
		pChatBox->SetRect( r);
		pChatBox->SetChainedParent( this);
		if ( XUIChattingBox::GetInstance()->GetLookAndFill() != NULL)
			pChatBox->SetLookAndFill( XUIChattingBox::GetInstance()->GetLookAndFill());

		if ( XUIChattingBox::GetInstance()->GetAddBtnLook() != NULL)
			pChatBox->m_pAddChatCtrl->SetLookAndFill( XUIChattingBox::GetInstance()->GetAddBtnLook());

		if ( XUIChattingBox::GetInstance()->GetRemoveBtnLook() != NULL)
			pChatBox->m_pRemoveChatCtrl->SetLookAndFill( XUIChattingBox::GetInstance()->GetRemoveBtnLook());

		if ( XUIChattingBox::GetInstance()->GetPropBtnLook() != NULL)
			pChatBox->m_pShowProperty->SetLookAndFill( XUIChattingBox::GetInstance()->GetPropBtnLook());

		m_pChainedChild = pChatBox;
		return pChatBox;
	}

	return NULL;
}


// CreateNewChatCtrl
XChatCtrl* XChatBox::CreateNewChatCtrl( int nChatBoxID, int nChatCtrlID, const char* szName, MBitmap* pIcon)
{
	if ( nChatBoxID != m_nID)
	{
		if ( m_pChainedChild != NULL)		return m_pChainedChild->CreateNewChatCtrl( nChatBoxID, nChatCtrlID, szName, pIcon);
		return NULL;
	}


	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		if ( (*itr)->GetID() != nChatCtrlID)	continue;


		XChatTab* pChatTab = (*itr)->GetTab();
		if ( pChatTab != NULL)
		{
			int nSize = XUIChattingBox::GetInstance()->GetHeaderHeight();
			pChatTab->SetIcon( pIcon, MSIZE( nSize, nSize));
			pChatTab->SetText( szName);
		}

		return (*itr);
	}


	XChatCtrl* pChatCtrl = new XChatCtrl( this);
	pChatCtrl->SetID( nChatCtrlID);

	XChatTab* pChatTab = pChatCtrl->GetTab();
	if ( pChatTab != NULL)
	{
		int nSize = XUIChattingBox::GetInstance()->GetHeaderHeight();
		pChatTab->SetIcon( pIcon, MSIZE( nSize, nSize));
		pChatTab->SetText( szName);

		if ( XUIChattingBox::GetInstance()->GetTabBtnLook() != NULL)
			pChatTab->SetLookAndFill( XUIChattingBox::GetInstance()->GetTabBtnLook());
	}

	XChatView* pView = pChatCtrl->GetView();
	if ( pView != NULL)
	{
		if ( XUIChattingBox::GetInstance()->GetScrollBarLook() != NULL)
		{
			pView->GetScrollBar()->SetLookAndFill( XUIChattingBox::GetInstance()->GetScrollBarLook());
			pView->GetScrollBar()->SetThumbLength( XUIChattingBox::GetInstance()->GetThumbLength());
		}

		if ( XUIChattingBox::GetInstance()->GetEndLineBtnLook() != NULL)
			pView->GetEndLineBtn()->SetLookAndFill( XUIChattingBox::GetInstance()->GetEndLineBtnLook());
	}


	m_ChatCtrls.push_back( pChatCtrl);

	if ( m_ChatCtrls.size() == 1)
	{
		pChatCtrl->Show( true);
		m_pCurSel = pChatCtrl;
	}


	SelChangedChatCtrlTab( pChatCtrl);

	m_bArrangeChatCtrlTab = true;
	return pChatCtrl;
}


// DeleteChatCtrl
bool XChatBox::DeleteChatCtrl( int nChatCtrlID)
{
	// 기본 채팅박스는 지우지 않는다
	if ( nChatCtrlID == DEFAULT_CHATTABID)
		return false;


	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		XChatCtrl* pChatCtrl = (*itr);
		if ( pChatCtrl->GetID() != nChatCtrlID)		continue;


		// 메인 채팅창에서 채팅박스가 하나밖에 없으면 지우지 않는다
		if ( XUIChattingBox::GetInstance() == this  &&  m_ChatCtrls.size() == 1)
			continue;


		DetachChatCtrl( pChatCtrl);
		delete pChatCtrl;

		return true;
	}


	if ( m_pChainedChild != NULL)
	{
		bool bRetVal = m_pChainedChild->DeleteChatCtrl( nChatCtrlID);
		return bRetVal;
	}

	return false;
}


// Delete
void XChatBox::Delete()
{
	RemoveFromParent();

	if ( m_ChatCtrls.empty() == false)
	{
		for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
			delete (*itr);

		m_ChatCtrls.clear();
	}

	if ( m_pChainedParent != NULL)		m_pChainedParent->SetChainedChild( m_pChainedChild);
	if ( m_pChainedChild != NULL)		m_pChainedChild->SetChainedParent( m_pChainedParent);
}


// AttachChatCtrl
bool XChatBox::AttachChatCtrl( XChatCtrl* pChatCtrl, XChatCtrl* pInsert)
{
	if ( pChatCtrl == NULL)		return false;


	if ( pInsert != NULL  &&  pInsert->GetID() == DEFAULT_CHATTABID)
	{
		list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();
		for ( ;  itr != m_ChatCtrls.end();  itr++)
		{
			if ( (*itr) == pInsert)
			{
				itr++;
				break;
			}
		}

		pInsert = ( itr != m_ChatCtrls.end())  ?  (*itr)  :  NULL;
	}


	if ( pInsert == NULL)
	{
		m_ChatCtrls.push_back( pChatCtrl);

		XChatTab* pChatTab = pChatCtrl->GetTab();
		if ( pChatTab != NULL)
		{
			pChatTab->RemoveFromParent();
			AddChild( pChatTab);
			pChatTab->SetListener( this);
		}

		XChatView* pChatView = pChatCtrl->GetView();
		if ( pChatView != NULL)
		{
			pChatView->RemoveFromParent();
			AddChild( pChatView);
			pChatView->SetListener( this);
		}

		SelChangedChatCtrlTab( pChatCtrl);

		m_bArrangeChatCtrlTab = true;
		return true;
	}

	else
	{
		for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
		{
			if ( (*itr) != pInsert)	continue;


			m_ChatCtrls.insert( itr, pChatCtrl);

			XChatTab* pChatTab = pChatCtrl->GetTab();
			if ( pChatTab != NULL)
			{
				pChatTab->RemoveFromParent();
				AddChild( pChatTab);
				pChatTab->SetListener( this);
			}

			XChatView* pChatView = pChatCtrl->GetView();
			if ( pChatView != NULL)
			{
				pChatView->RemoveFromParent();
				AddChild( pChatView);
				pChatView->SetListener( this);
			}

			SelChangedChatCtrlTab( pChatCtrl);
			
			m_bArrangeChatCtrlTab = true;
			return true;
		}
	}

	return false;
}


// DetachChatCtrl
bool XChatBox::DetachChatCtrl( XChatCtrl* pChatCtrl)
{
	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		if ( (*itr) != pChatCtrl)		continue;

		XChatTab* pChatTab = (*itr)->GetTab();
		if ( pChatTab != NULL)
		{
			pChatTab->RemoveFromParent();
			pChatTab->SetListener( NULL);
		}

		XChatView* pChatView = (*itr)->GetView();
		if ( pChatView != NULL)
		{
			pChatView->RemoveFromParent();
			pChatTab->SetListener( NULL);
		}


		m_ChatCtrls.erase( itr);

		m_bArrangeChatCtrlTab = true;

		if ( m_ChatCtrls.empty() == false)		SelChangedChatCtrlTab( (*m_ChatCtrls.begin()) );
		else									m_pCurSel = NULL;

		return true;
	}

	return false;
}


// SetChatTabName
void XChatBox::SetChatTabName( int nChatCtrlID, const char* szName)
{
	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		if ( (*itr)->GetID() != nChatCtrlID)		continue;


		XChatTab* pChatTab = (*itr)->GetTab();
		if ( pChatTab != NULL)		pChatTab->SetText( szName);

		m_bArrangeChatCtrlTab = true;

		return;
	}


	if ( m_pChainedChild != NULL)		m_pChainedChild->SetChatTabName( nChatCtrlID, szName);
}


// GetChatTabName
const char* XChatBox::GetChatTabName( int nChatCtrlID)
{
	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		if ( (*itr)->GetID() != nChatCtrlID)		continue;


		XChatTab* pChatTab = (*itr)->GetTab();
		if ( pChatTab != NULL)		return pChatTab->GetText();

		return "";
	}


	if ( m_pChainedChild != NULL)		return m_pChainedChild->GetChatTabName( nChatCtrlID);
	return "";
}


// AddChat
void XChatBox::AddChat( int nType, const char* szChat)
{
	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
		(*itr)->AddChat( nType, szChat);


	if ( m_pChainedChild != NULL)		return m_pChainedChild->AddChat( nType, szChat);
}


// ClearChat
void XChatBox::ClearChat()
{
	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
		(*itr)->ClearChat();

	if ( m_pChainedChild != NULL)		return m_pChainedChild->ClearChat();
}


// SetTabCurSel
void XChatBox::SetTabCurSel( int nChatBoxID, int nChatCtrlID)
{
	if ( nChatBoxID != m_nID)
	{
		if ( m_pChainedChild != NULL)
			m_pChainedChild->SetTabCurSel( nChatBoxID, nChatCtrlID);
		return;
	}

	for ( list< XChatCtrl*>::iterator i = m_ChatCtrls.begin();  i != m_ChatCtrls.end();  i++)
	{
		if ( (*i)->GetID() == nChatCtrlID)
		{
			SelChangedChatCtrlTab( *i);
			break;
		}
	}
}


// GetTabCurSel
int XChatBox::GetTabCurSel( int nChatBoxID)
{
	if ( nChatBoxID != m_nID)
	{
		if ( m_pChainedChild != NULL)
			return m_pChainedChild->GetTabCurSel( nChatBoxID);
		else
			return -1;
	}

	return ( m_pCurSel != NULL  ?  m_pCurSel->GetID()  :  -1);
}


// AddTypeFilter
void XChatBox::AddTypeFilter( int nChatCtrlID, int nType)
{
	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		XChatCtrl* pChatCtrl = (*itr);
		if ( pChatCtrl->GetID() != nChatCtrlID)		continue;

		pChatCtrl->AddTypeFilter( nType);
		return;
	}

	if ( m_pChainedChild != NULL)		return m_pChainedChild->AddTypeFilter( nChatCtrlID, nType);
}


// ClearTypeFilter
void XChatBox::ClearTypeFilter( int nChatCtrlID)
{
	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		XChatCtrl* pChatCtrl = (*itr);
		if ( pChatCtrl->GetID() != nChatCtrlID)		continue;

		pChatCtrl->ClearTypeFilter();
		return;
	}

	if ( m_pChainedChild != NULL)		return m_pChainedChild->ClearTypeFilter( nChatCtrlID);
}


// GetTypeFilterCount
int XChatBox::GetTypeFilterCount( int nChatCtrlID)
{
	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		if ( (*itr)->GetID() != nChatCtrlID)		continue;

		return (int)(*itr)->GetTypeFilterCount();
	}

	if ( m_pChainedChild != NULL)		return m_pChainedChild->GetTypeFilterCount( nChatCtrlID);
	return 0;
}


// GetTypeFilter
int XChatBox::GetTypeFilter( int nChatCtrlID, int i)
{
	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		if ( (*itr)->GetID() != nChatCtrlID)		continue;

		return (*itr)->GetTypeFilter( i);
	}

	if ( m_pChainedChild != NULL)		return m_pChainedChild->GetTypeFilter( nChatCtrlID, i);
	return 0;
}


// SetTextSize
void XChatBox::SetTextSize( int nChatCtrlID, int nSize)
{
	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		if ( (*itr)->GetID() != nChatCtrlID)		continue;

		return (*itr)->SetTextSize( nSize);
	}

	if ( m_pChainedChild != NULL)		return m_pChainedChild->SetTextSize( nChatCtrlID, nSize);
}


// GetTextSize
int XChatBox::GetTextSize( int nChatCtrlID)
{
	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		if ( (*itr)->GetID() != nChatCtrlID)		continue;

		return (*itr)->GetTextSize();
	}

	if ( m_pChainedChild != NULL)		return m_pChainedChild->GetTextSize( nChatCtrlID);

	return CTS_NORMAL;
}


// SelChangedChatCtrlTab
void XChatBox::SelChangedChatCtrlTab( XChatCtrl* pChatCtrl)
{
	bool bFind = false;
	for ( list< XChatCtrl*>::iterator i = m_ChatCtrls.begin();  i != m_ChatCtrls.end();  i++)
	{
		if ( (*i) == pChatCtrl)
		{
			bFind = true;
			break;
		}
	}

	if ( bFind == false)
		return;

	for ( list< XChatCtrl*>::iterator i = m_ChatCtrls.begin();  i != m_ChatCtrls.end();  i++)
	{
		if ( (*i) != pChatCtrl)
			(*i)->Show( false);
	}

	// Show tab
	pChatCtrl->Show( true);
	m_pCurSel = pChatCtrl;

	m_bArrangeChatCtrlTab = true;
}


// IsMovable
bool XChatBox::IsMovable()
{
	return !XUIChattingBox::GetInstance()->IsFixed();
}


// IsResizable
bool XChatBox::IsResizable()
{
	return !XUIChattingBox::GetInstance()->IsFixed();
}


// FindChatBox
XChatBox* XChatBox::FindChatBox( int nID)
{
	if ( nID == m_nID)				return this;

	if ( m_pChainedChild != NULL)	return m_pChainedChild->FindChatBox( nID);
	return NULL;
}


// FindChatCtrl
XChatCtrl* XChatBox::FindChatCtrl( int nID)
{
	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		if ( (*itr)->GetID() == nID)
			return (*itr);
	}

	if ( m_pChainedChild != NULL)	return m_pChainedChild->FindChatCtrl( nID);
	return NULL;
}


// EventRefresh
void XChatBox::EventRefresh()
{
	m_bArrangeChatCtrlTab = true;

	if ( m_pChainedChild != NULL)
		m_pChainedChild->EventRefresh();
}


// InitMouseState
void XChatBox::InitMouseState()
{
	MWidget::InitMouseState();

	m_bMoving = false;
	m_nResizing = 0;

	XUIChattingBox::GetInstance()->SetForcedShowState( 0);
}


// OnEvent
bool XChatBox::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)		return true;


	switch( pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN:
		if ( pEvent->nHitTest == MHTCAPTION)
		{
			m_ptLastCursor = pEvent->Pos;
			m_bMoving = true;

			XUIChattingBox::GetInstance()->SetForcedShowState( 1);
		}
		else if ( pEvent->nHitTest > MHTSIZEFIRST  &&  pEvent->nHitTest < MHTSIZELAST)
		{
			m_ptLastCursor = pEvent->Pos;
			m_nResizing = pEvent->nHitTest;

			XUIChattingBox::GetInstance()->SetForcedShowState( 1);
		}
		break;

	case MWM_LBUTTONUP:
		m_bMoving = false;
		m_nResizing = HTNOWHERE;
		XUIChattingBox::GetInstance()->SetForcedShowState( 0);
		break;

	case MWM_MOUSEMOVE:
		if ( IsCapture() == false)
			break;

		else if ( m_bMoving == true)
		{
			MPOINT _diff( pEvent->Pos.x - m_ptLastCursor.x, pEvent->Pos.y - m_ptLastCursor.y);
			int sx1 = m_Rect.x + _diff.x - MAGNETIC_BORDERSIZE;
			int sy1 = m_Rect.y + _diff.y - MAGNETIC_BORDERSIZE;
			int sw = m_Rect.w + ( MAGNETIC_BORDERSIZE << 1);
			int sh = m_Rect.h + ( MAGNETIC_BORDERSIZE << 1);
			int sx2 = sx1 + sw;
			int sy2 = sy1 + sh;

			if ( sx1 < MAGNETDIST)
			{
				sx1 = 0;
				sx2 = sw;
			}
			else if ( sx2 > (MGetWorkspaceWidth() - MAGNETDIST))
			{
				sx2 = MGetWorkspaceWidth();
				sx1 = sx2 - sw;
			}

			if ( sy1 < MAGNETDIST)
			{
				sy1 = 0;
				sy2 = sh;
			}
			else if ( sy2 > ( MGetWorkspaceHeight() - MAGNETDIST))
			{
				sy2 = MGetWorkspaceHeight();
				sy1 = sy2 - sh;
			}


			XChatBox* pChatBox = XUIChattingBox::GetInstance();
			while ( pChatBox != NULL)
			{
				if ( pChatBox != this)
				{
					MRECT tr = pChatBox->GetRect();
					int tx1 = tr.x - MAGNETIC_BORDERSIZE;
					int ty1 = tr.y - MAGNETIC_BORDERSIZE;
					int tx2 = tx1 + tr.w + ( MAGNETIC_BORDERSIZE << 1);
					int ty2 = ty1 + tr.h + ( MAGNETIC_BORDERSIZE << 1);


					if ( sx1 < (tx2 + MAGNETDIST)  &&  sx2 > (tx1 - MAGNETDIST)  &&
						sy1 < (ty2 + MAGNETDIST)  &&  sy2 > (ty1 - MAGNETDIST))
					{
						if ( abs( sx1 - tx1) < MAGNETDIST)		sx1 = tx1;
						if ( abs( sx1 - tx2) < MAGNETDIST)		sx1 = tx2;
						if ( abs( sx2 - tx1) < MAGNETDIST)		sx1 = tx1 - sw;
						if ( abs( sx2 - tx2) < MAGNETDIST)		sx1 = tx2 - sw;
						if ( abs( sy1 - ty1) < MAGNETDIST)		sy1 = ty1;
						if ( abs( sy1 - ty2) < MAGNETDIST)		sy1 = ty2;
						if ( abs( sy2 - ty1) < MAGNETDIST)		sy1 = ty1 - sh;
						if ( abs( sy2 - ty2) < MAGNETDIST)		sy1 = ty2 - sh;
					}
				}

				pChatBox = pChatBox->GetChainedChild();
			}

			SetPosition( sx1 + MAGNETIC_BORDERSIZE, sy1 + MAGNETIC_BORDERSIZE);
			return true;
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
	}


	return false;
}


// OnCommand
bool XChatBox::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if ( MWidget::OnCommand( pWidget, nMsg, szArgs) == true)	return true;

	
	if ( pWidget == m_pAddChatCtrl  &&  nMsg == MEVENT_CLICK)
	{
		MBitmap* pIcon = NULL;

		if ( m_ChatCtrls.begin() != m_ChatCtrls.end()  &&  (*m_ChatCtrls.begin())->GetTab() != NULL)
			pIcon = (*m_ChatCtrls.begin())->GetTab()->GetIcon();

		XChatCtrl* pChatCtrl = CreateNewChatCtrl( m_nID, XUIChattingBox::GetInstance()->GetNewChatCtrlID(), MLocale::ConvUTF16ToAnsi( XGetStr( L"CHATTING_NEW")).c_str(), pIcon);
		if ( pChatCtrl == NULL)
			return false;

		XUIChattingBox::GetInstance()->EventClick( pChatCtrl->GetID(), ADDER);
		return true;
	}
	
	else if ( pWidget == m_pRemoveChatCtrl  &&  nMsg == MEVENT_CLICK)
	{
		if ( m_pCurSel != NULL)
		{
			XUIChattingBox::GetInstance()->EventClick( m_pCurSel->GetID(), REMOVER);
			return true;
		}
	}

	else if ( pWidget == m_pShowProperty  &&  nMsg == MEVENT_CLICK)
	{
		if ( m_pCurSel != NULL)
		{
			XUIChattingBox::GetInstance()->EventClick( m_pCurSel->GetID(), PROPERTY);
			return true;
		}
	}

	else if ( nMsg == MEVENT_VALUECHANGED)
	{
		for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
		{
			XChatCtrl* pChatCtrl = (*itr);
			if ( pWidget != pChatCtrl->GetTab())		continue;

			SelChangedChatCtrlTab( pChatCtrl);

			return true;
		}
	}

	else if ( nMsg == MEVENT_CLICK)
	{
		for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
		{
			XChatCtrl* pChatCtrl = (*itr);
			if ( pWidget != pChatCtrl->GetTab())		continue;

			XUIChattingBox::GetInstance()->EventClick( pChatCtrl->GetID(), CHATBOX);
			return true;
		}
	}

	else if ( nMsg == MEVENT_RCLICK)
	{
		for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
		{
			XChatCtrl* pChatCtrl = (*itr);
			if ( pWidget != pChatCtrl->GetTab())		continue;

			XUIChattingBox::GetInstance()->EventRClick( pChatCtrl->GetID(), CHATBOX);
			return true;
		}
	}

	return false;
}


// OnRun
void XChatBox::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);


	// Set fade
	if ( global.ui->IsMouseVisible() == false)								m_tFade.SetReverse( true);
	else if ( XUIChattingBox::GetInstance()->GetForcedShowState() > 0)		m_tFade.SetReverse( false);
	else if ( IsMouseIncluded() == true)									m_tFade.SetReverse( false);
	else																	m_tFade.SetReverse( true);

	if ( XUIChattingBox::GetInstance()->IsFixed() == true)
	{
		m_pAddChatCtrl->Show( false);
		m_pRemoveChatCtrl->Show( false);
	}
	else
	{
		m_pAddChatCtrl->Show( true);
		m_pAddChatCtrl->SetOpacity( (float)m_tFade.GetRatio());

		if ( m_pCurSel != NULL  &&  m_pCurSel->GetID() == DEFAULT_CHATTABID)
		{
			m_pRemoveChatCtrl->Show( false);
		}
		else
		{
			m_pRemoveChatCtrl->Show( true);
			m_pRemoveChatCtrl->SetOpacity( (float)m_tFade.GetRatio());
		}
	}

	m_pShowProperty->SetOpacity( (float)m_tFade.GetRatio());


	// Arrange tabs
	if ( m_bArrangeChatCtrlTab == true)
	{
		MRECT r = GetClientRect();
		MLookBase* pTabLook = XUIChattingBox::GetInstance()->GetTabBtnLook();
		MFont* pFont = pTabLook  ?  pTabLook->GetFont()  :  MFontManager::Get( NULL);
		MRECT wo = pTabLook->GetWorkRectOffset();
		bool bFixed = XUIChattingBox::GetInstance()->IsFixed();

		int h = XUIChattingBox::GetInstance()->GetHeaderHeight();
		int x = -pTabLook->GetBkgrndRectOffset().x;
		for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
		{
			XChatTab* pChatTab = (*itr)->GetTab();
			MRECT _r = pChatTab->GetRect();
			_r.w = min( 200, max( 20, pFont->GetWidth( pChatTab->GetText()) - wo.w));
			if ( bFixed == false  &&  m_pCurSel != NULL  &&  (*itr) == m_pCurSel  &&  (*itr)->GetID() != DEFAULT_CHATTABID)
				_r.w += XUIChattingBox::GetInstance()->GetRemoverSize().w + 5;
			pChatTab->SetRect( x, _r.y, _r.w, h);

			x += _r.w;
		}

		MSize s = XUIChattingBox::GetInstance()->GetAdderSize();
		m_pAddChatCtrl->SetRect( x + 3, r.y + ((h - s.h) >> 1), s.w, s.h);

		MRECT tr = m_pCurSel ? m_pCurSel->GetTab()->GetRect() : MRECT( 0, 0, 0, 0);
		s = XUIChattingBox::GetInstance()->GetRemoverSize();
		m_pRemoveChatCtrl->SetRect( tr.x + tr.w - s.w + ( wo.w + wo.x), tr.y + ((tr.h - s.h) >> 1), s.w, s.h);

		s = XUIChattingBox::GetInstance()->GetPropertySize();
		m_pShowProperty->SetRect( r.w - s.w, r.y + ((h - s.h) >> 1), s.w, s.h);

		m_bArrangeChatCtrlTab = false;
	}


	// Check rectangle
	MRECT rectBkgrndOffset = GetBkgrndRectOffset();
	int x1 = m_Rect.x + rectBkgrndOffset.x;
	int y1 = m_Rect.y + rectBkgrndOffset.y;
	int x2 = x1 + m_Rect.w + rectBkgrndOffset.w;
	int y2 = y1 + m_Rect.h + rectBkgrndOffset.h;
	if ( x1 < 0)						m_Rect.x = -rectBkgrndOffset.x;
	if ( y1 < 0)						m_Rect.y = -rectBkgrndOffset.y;
	if ( x2 > MGetWorkspaceWidth())		m_Rect.x = MGetWorkspaceWidth() - m_Rect.w - rectBkgrndOffset.w;
	if ( y2 > MGetWorkspaceHeight())	m_Rect.y = MGetWorkspaceHeight() - m_Rect.h - rectBkgrndOffset.h;



	// Delete child
	if ( m_pChainedChild != NULL  &&  m_pChainedChild->IsEmpty() == true)
	{
		XChatBox* pChild = m_pChainedChild;

		if ( m_pChainedChild->GetChainedChild() != NULL)
			m_pChainedChild->GetChainedChild()->SetChainedParent( this);

		m_pChainedChild = m_pChainedChild->GetChainedChild();

		pChild->SetChainedParent( NULL);
		pChild->SetChainedChild( NULL);
		delete pChild;
	}
}


// OnDrawBorder
void XChatBox::OnDrawBorder( MDrawContext* pDC)
{
	float fRatio = (float)m_tFade.GetRatio();
	pDC->SetOpacity( max( XUIChattingBox::GetInstance()->GetOpacity(), fRatio));

	if ( m_pLookAndFill  &&  m_pLookAndFill->IsValidAssign( MWP_BORDER, MWS_NORMAL) == true)
	{
		MRECT r = m_pLookAndFill->GetBkgrndRect( this);
		MRECT* ar = m_pLookAndFill->GetAssignRect( MWP_BORDER, MWS_NORMAL);
		pDC->SetBitmap( m_pLookAndFill->GetBitmap());

		int nLongW = ar[ 0].w, nLongH = ar[ 0].h;
		if ( m_pLookAndFill->GetBitmapStyle() == DRAWBITMAP_STRETCH)
		{
			nLongW = r.w - ( ar[4].w + ar[5].w);
			nLongH = r.h - ( ar[2].h + ar[7].h);
		}
		nLongW = max( 0, nLongW);
		nLongH = max( 0, nLongH);


		MRECT stThinRect[9];
		memcpy(stThinRect, ar, sizeof(MRECT) * 9);

		stThinRect[1].x = stThinRect[4].x = stThinRect[6].x = 0;
		stThinRect[1].y = stThinRect[2].y = stThinRect[3].y = 0;

		stThinRect[0].x	= ar[4].w;
		stThinRect[2].x = ar[1].w;
		stThinRect[7].x = ar[6].w;

		stThinRect[0].y = stThinRect[4].y = stThinRect[5].y = ar[2].h;
		stThinRect[6].y = stThinRect[7].y = stThinRect[8].y = ar[2].h + nLongH;

		stThinRect[3].x = r.w - ar[3].w;
		stThinRect[5].x = ar[4].w + nLongW;
		stThinRect[8].x = r.w - ar[8].w;

		stThinRect[0].h = stThinRect[4].h = stThinRect[5].h = nLongH;

		stThinRect[2].w = r.w - ar[1].w - ar[3].w;
		stThinRect[0].w = r.w - ar[4].w - ar[5].w;
		stThinRect[7].w = r.w - ar[6].w - ar[8].w;


		for ( int i = 0;  i < 9;  i++)
		{
			if ( i == 2)
				continue;

			if ( ar[i].w  > 0  &&  ar[i].h > 0)
				pDC->Draw( stThinRect[i].x + r.x, stThinRect[i].y + r.y, stThinRect[i].w, stThinRect[i].h,  ar[i].x, ar[i].y, ar[i].w, ar[i].h);
		}


		MRECT br = m_pCurSel ? m_pCurSel->GetTab()->GetRect() : MRECT( 0, 0, 0, 0);
		MRECT wo = m_pLookAndFill->GetWorkRectOffset();
		MRECT bo = m_pLookAndFill->GetBkgrndRectOffset();

		int _width = stThinRect[2].w + stThinRect[1].w + bo.x;
		int _x = stThinRect[2].x + r.x;
		int _w = max( 0, min( _width, br.x - stThinRect[1].w + wo.x - bo.x));
		if ( _w > 0)
			pDC->Draw( _x, stThinRect[2].y + r.y, _w, stThinRect[2].h, ar[2].x, ar[2].y, ar[2].w, ar[2].h);

		_x = _x + _w + br.w;
		_w = _width - _x;
		if ( _x < ( r.x + _width)  &&  _w > 0)
			pDC->Draw( _x, stThinRect[2].y + r.y, _w, stThinRect[2].h, ar[2].x, ar[2].y, ar[2].w, ar[2].h);
	}
}


// OnDraw
void XChatBox::OnDraw( MDrawContext* pDC)
{
	pDC->SetOpacity( 1.0f);

	MWidget::OnDraw( pDC);
}


// OnSetFocus
void XChatBox::OnSetFocus()
{
	MWidget::OnSetFocus();

	BringToTop();
}


// OnSize
void XChatBox::OnSize( int w, int h)
{
	m_Rect.w = min( 600, max( 200, w));
	m_Rect.h = min( 600, max( 100, h));

	MWidget::OnSize( m_Rect.w, m_Rect.h);

	m_bArrangeChatCtrlTab = true;
}


// OnRClick
void XChatBox::OnRClick( MEvent* pEvent)
{
	MWidget::OnRClick( pEvent);


	for ( list< XChatCtrl*>::iterator itr = m_ChatCtrls.begin();  itr != m_ChatCtrls.end();  itr++)
	{
		XChatTab* pChatTab = (*itr)->GetTab();
		if ( pChatTab == NULL  ||  pChatTab->GetCheck() == false)	continue;


		MEvent _event;
		_event.nMessage = MWM_RBUTTONDOWN;
		_event.Pos = MPOINT( 0, 0);
		_event.nKey = 0;
		_event.nDelta = 0;
		_event.bShift = false;
		_event.bCtrl = false;
		_event.bAlt = false;
		_event.bLButton = false;
		_event.bMButton = false;
		_event.bRButton = false;
		XUIChattingBox::GetInstance()->OnItemRClick( &_event, pChatTab->GetUserData());
	}
}


// OnDrop
bool XChatBox::OnDrop( MWidget* pSender, MDragData* pDragData, MPOINT& pt)
{
	// Drop from TAP to BOX
	if ( strcmp( pSender->GetClassName(), MINT_CHATTINGBOXTAB) != 0)
		return true;


	DWORD dwUserData = pDragData->GetItemData( 0);
	if ( dwUserData == 0)					return true;


	XUIChattingBox* pChattingBox = XUIChattingBox::GetInstance();
	if ( pChattingBox == NULL)				return true;

	XChatCtrl* pChatCtrl = pChattingBox->GetChatCtrl( (int)dwUserData);
	if ( pChatCtrl == NULL)					return true;

	XChatBox* pChatBox = pChatCtrl->GetChatBox();
	if ( pChatBox == NULL)					return true;


	if ( pChatBox->DetachChatCtrl( pChatCtrl) == false)		return true;
	if ( pChatBox != this)
	{
		if ( pChatBox->GetChatCtrlCountInBox() <= 0)
		{
			pChatBox->Delete();
			delete pChatBox;
		}
	}

	if ( AttachChatCtrl( pChatCtrl) == false)				return true;

	return true;
}


// OnNcHitTest
int XChatBox::OnNcHitTest( MEvent* pEvent)
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

			MPOINT pt( pEvent->Pos.x - r.x, pEvent->Pos.y - r.y);
			if ( pt.y >= XUIChattingBox::GetInstance()->GetHeaderHeight())
			{
				_hittest = MHTCAPTION;

				if ( pt.x < RESIZEBOX_SIZE)							_hittest = MHTLEFT;
				else if ( ( r.w - pt.x) < RESIZEBOX_SIZE)			_hittest = MHTRIGHT;

				if ( ( r.h - pt.y) < RESIZEBOX_SIZE)
				{
					if ( _hittest == MHTLEFT)						_hittest = MHTBOTTOMLEFT;
					else if ( _hittest == MHTRIGHT)					_hittest = MHTBOTTOMRIGHT;
					else											_hittest = MHTBOTTOM;
				}
			}
		}
	}

	return _hittest;
}









/************************************************************************
class : XUIChattingEdit
*************************************************************************/
XUIChattingEdit::XUIChattingEdit( const char* szName, MWidget* pParent, MListener* pListener)
: MEdit( szName, pParent, pListener)
{
	m_nPrefix = 0;
	m_strPrefixName.clear();
	m_pPrefixSelector = NULL;
}


XUIChattingEdit::~XUIChattingEdit()
{
	m_pPrefixSelector = NULL;
}


// IsActive
bool XUIChattingEdit::IsActive() const
{
	if ( m_bShow == false)						return false;
	if ( MWidget::m_pFocusedWidget == this)		return true;
	if ( m_pPrefixSelector != NULL  &&  m_pPrefixSelector->GetShow() == true)
		return true;

	return false;
}


// SetPreFix
void XUIChattingEdit::SetPrefixSelector( MPopupMenu* pSelector)
{
	m_pPrefixSelector = pSelector;
}


// OnCommand
bool XUIChattingEdit::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if ( MEdit::OnCommand( pWidget, nMsg, szArgs) == true)
		return true;


	if ( pWidget == m_pPrefixSelector  &&  nMsg == MEVENT_CLICK)
	{
		SetFocus();
	}

	return false;
}


// OnEvent
bool XUIChattingEdit::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( pEvent->nMessage == MWM_LBUTTONDOWN  &&  m_pPrefixSelector != NULL)
	{
		MRECT rgn( -42, 0, 40, m_Rect.h);
		if ( rgn.InPoint( pEvent->Pos) == true)
		{
			MRECT wr = GetWindowRect();
			MRECT r = m_pPrefixSelector->GetRect();
			MPOINT pt( wr.x, wr.y - r.h);
			m_pPrefixSelector->SetListener( this);
			m_pPrefixSelector->TrackPopupMenu( pt, this);

			return true;
		}
	}

	if ( MEdit::OnEvent( pEvent, pListener) == true)	return true;

	return false;
}


// OnDraw
void XUIChattingEdit::OnDrawBorder( MDrawContext* pDC)
{
	MEdit::OnDrawBorder( pDC);

	pDC->SetFont( "fntScript");
	pDC->SetColor( 255, 255, 255);
	pDC->Text( MRECT( 0, 0, 40, m_Rect.h), m_strPrefixName.c_str(), MAM_HCENTER | MAM_VCENTER);
}


// OnShow
void XUIChattingEdit::OnShow( bool bShow)
{
	MEdit::OnShow( bShow);

	if ( m_pPrefixSelector != NULL)			m_pPrefixSelector->Show( false);
}








/************************************************************************
 class : XUIChattingBox
*************************************************************************/
XUIChattingBox* XUIChattingBox::m_pInstance = NULL;


// XUIChattingBox
XUIChattingBox::XUIChattingBox( const char* szName, MWidget* pParent, MListener* pListener)
: XChatBox( szName, pParent, pListener)
{
	if ( XUIChattingBox::m_pInstance != NULL)	return;

	XUIChattingBox::m_pInstance = this;


	m_nID = DEFAULT_CHATBOXID;
	m_pLookTabBtn = NULL;
	m_pLookAddBtn = NULL;
	m_pLookRemoveBtn = NULL;
	m_pLookPropBtn = NULL;
	m_pLookEndLineBtn = NULL;
	for ( int i = 0;  i < CTS_MAX;  i++)		m_pLookText[ i] = NULL;
	m_pLookScrollBar = NULL;
	m_nHeaderHeight = 25;
	m_nThumbLength = 16;
	m_AdderSize = MSize( 16, 16);
	m_RemoverSize = MSize( 16, 16);
	m_PropertySize = MSize( 16, 16);
	m_nForcedShowState = 0;
	m_fOpacity = 0.4f;
	m_bFixed = false;
	m_pIcon = NULL;
	m_IconRect = MRECT( 0, 0, 32, 32);
	m_dwLazySendTimer = timeGetTime();

	m_pEdit = new XUIChattingEdit( NULL, this, this);
	m_pEdit->Show( false);
	m_pEdit->SetAnchor( MAnchorStyle( true, true, false, true));
	m_pEdit->EnableHistory( true);
}


// ~XUIChattingBox
XUIChattingBox::~XUIChattingBox()
{
	if ( XUIChattingBox::m_pInstance == this)
		XUIChattingBox::m_pInstance = NULL;
}


// SetPrefixSelector
void XUIChattingBox::SetPrefixSelector( const char* szPopupMenu)
{
	MPopupMenu* pPopupMenu = (MPopupMenu*)MGetMint()->FindWidget( szPopupMenu);
	m_pEdit->SetPrefixSelector( pPopupMenu);
}


// GetChatBoxCount
int XUIChattingBox::GetChatBoxCount()
{
	XChatBox* pLastNode = this;
	int nCount = 1;

	while ( pLastNode->GetChainedChild() != NULL)
	{
		pLastNode = pLastNode->GetChainedChild();
		nCount++;
	}

	return nCount;
}


// GetChatBoxID
int XUIChattingBox::GetChatBoxID( int nIndex)
{
	XChatBox* pChatBox = this;
	for ( int i = 0;  i < nIndex;  i++)		pChatBox = pChatBox->GetChainedChild();
	return pChatBox->GetID();
}


// GetChatBoxRect
MRECT XUIChattingBox::GetChatBoxRect( int nIndex)
{
	XChatBox* pChatBox = this;
	for ( int i = 0;  i < nIndex;  i++)		pChatBox = pChatBox->GetChainedChild();
	return pChatBox->GetRect();
}


// GetChatCtrlCount
int XUIChattingBox::GetChatCtrlCount( int nChatBoxID)
{
	XChatBox* pChatBox = this;
	while ( pChatBox != NULL)
	{
		if ( pChatBox->GetID() == nChatBoxID)
			return pChatBox->GetChatCtrlCountInBox();

		pChatBox = pChatBox->GetChainedChild();
	}
	return 0;
}


// GetChatCtrlID
int XUIChattingBox::GetChatCtrlID( int nChatBoxID, int nIndex)
{
	XChatBox* pChatBox = this;
	while ( pChatBox != NULL)
	{
		if ( pChatBox->GetID() == nChatBoxID)
			return pChatBox->GetChatCtrlIndex( nIndex)->GetID();

		pChatBox = pChatBox->GetChainedChild();
	}
	return 0;
}


// GetChatCtrlName
const char* XUIChattingBox::GetChatCtrlName( int nChatBoxID, int nIndex)
{
	XChatBox* pChatBox = this;
	while ( pChatBox != NULL)
	{
		if ( pChatBox->GetID() == nChatBoxID)
		{
			XChatTab* pChatTab = pChatBox->GetChatCtrlIndex( nIndex)->GetTab();
			if ( pChatTab != NULL)		return pChatTab->GetText();
			return "";
		}

		pChatBox = pChatBox->GetChainedChild();
	}
	return "";
}


// ActivateChatInput
void XUIChattingBox::ActivateChatInput( bool bActivate)
{
	if ( bActivate == true)
	{
		MRECT offset = GetWorkRectOffset();
		m_pEdit->SetRect( -offset.x, m_Rect.h - offset.y + 2, m_Rect.w, 23);
		m_pEdit->BeginModalState();
		m_pEdit->SetFocus();
	}
	else
	{
		m_pEdit->Show( false);
	}
}


// IsChatInputActivated
bool XUIChattingBox::IsChatInputActivated()
{
	return m_pEdit->IsActive();
}


// GetNewChatBoxID
int XUIChattingBox::GetNewChatBoxID()
{
	int nID = DEFAULT_CHATBOXID + 1;

	while ( 1)
	{
		XChatBox* pChatBox = FindChatBox( nID);
		if ( pChatBox == NULL)		break;

		nID++;
	}

	return nID;
}


// GetNewChatCtrlID
int XUIChattingBox::GetNewChatCtrlID()
{
	int nID = DEFAULT_CHATTABID + 1;

	while ( 1)
	{
		XChatCtrl* pChatCtrl = FindChatCtrl( nID);
		if ( pChatCtrl == NULL)		break;

		nID++;
	}

	return nID;
}


// SetFixed
void XUIChattingBox::SetFixed( bool bFixed)
{
	m_bFixed = bFixed;

	EventRefresh();
}


// GetInstance
XUIChattingBox* XUIChattingBox::GetInstance()
{
	return XUIChattingBox::m_pInstance;
}


// EventClick
bool XUIChattingBox::EventClick( int nChatCtrlID, int nClickItem)
{
	MWidget::m_EventArgs.SetFromEvent( this, NULL);
	MWidget::m_EventArgs.m_nItemIndex = nChatCtrlID;
	MWidget::m_EventArgs.m_nItemSubItem = nClickItem;
	return m_EventHandler.OnScriptEvent( this, MEVENT_ITEMCLICK);
}


// EventRClick
bool XUIChattingBox::EventRClick( int nChatCtrlID, int nClickItem)
{
	MWidget::m_EventArgs.SetFromEvent( this, NULL);
	MWidget::m_EventArgs.m_nItemIndex = nChatCtrlID;
	MWidget::m_EventArgs.m_nItemSubItem = nClickItem;
	return m_EventHandler.OnScriptEvent( this, MEVENT_ITEMRCLICK);
}


// OnCommand
bool XUIChattingBox::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if ( XChatBox::OnCommand( pWidget, nMsg, szArgs) == true)
		return true;


	if ( pWidget == m_pEdit)
	{
		DWORD _curr = timeGetTime();
		if ( nMsg == MEVENT_OK  &&  _curr > ( m_dwLazySendTimer + TIME_LAZYSEND))
		{
			m_dwLazySendTimer = _curr;

			wstring strTempText = MLocale::ConvAnsiToUTF16(m_pEdit->GetText());
			wstring strText = strTempText.substr(0, 255);


			m_pEdit->PushHistory();
			m_pEdit->Clear();

			if ( strText.empty() == false)
			{
				bool bDone = false;

				// 일단 먼저 채팅 명령어 수행
				if ( strText.at( 0) == '/'  &&  strText.length() > 1)
				{
					bDone = true;
					XChatCmds::Input( strText.c_str());
				}

				// '@'로 시작하면 콘솔 명령어 수행
				if ( bDone == false  &&  strText.at( 0) == '@'  &&  strText.length() > 1)
				{
					bDone = true;
					XConsoleCmds::Input( strText.c_str() + 1);
				}

				if ( bDone == false)
				{
					if ( m_pEdit->GetPrefix() > 0)
					{
						wstring strPrefix = MLocale::ConvAnsiToUTF16( m_pEdit->GetPrefixName());
						strText = wstring( L"/") + strPrefix + wstring( L" ") + strText;
						XChatCmds::Input( strText.c_str());
					}
					else
						XPostMsg_Say( strText.c_str());
				}
			}

			// OK이후에 무조건 창을 닫는다.
			global.script->GetGlueGameEvent().OnGameEvent( "CHATTINGBOX", "ACTIVATEINPUT", 0);

			return true;
		}

		else if ( nMsg == MEVENT_CANCEL  /*||  nMsg == MEVENT_KILLFOCUS*/)
		{
			global.script->GetGlueGameEvent().OnGameEvent( "CHATTINGBOX", "ACTIVATEINPUT", 0);
			global.ui->ShowMouse(true);
			return true;
		}
	}

	return false;
}


// OnRun
void XUIChattingBox::OnRun( DWORD dwCurrTime)
{
	XChatBox::OnRun( dwCurrTime);

	if ( m_pEdit != NULL  &&  m_pEdit->GetShow()  &&  m_pEdit->IsActive() == false)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "CHATTINGBOX", "ACTIVATEINPUT", 0);
		global.ui->ShowMouse(true);
	}
}


// OnLoaded
void XUIChattingBox::OnLoaded()
{
	if ( m_pLookAddBtn != NULL)
		m_pAddChatCtrl->SetLookAndFill( m_pLookAddBtn);

	if ( m_pLookRemoveBtn != NULL)
		m_pRemoveChatCtrl->SetLookAndFill( m_pLookRemoveBtn);

	if ( m_pLookPropBtn != NULL)
		m_pShowProperty->SetLookAndFill( m_pLookPropBtn);

	XChatBox::OnLoaded();
}


// OnDrawBorder
void XUIChattingBox::OnDrawBorder( MDrawContext* pDC)
{
	if ( IsChatInputActivated() == true)		m_tFade.SetReverse( false);

	XChatBox::OnDrawBorder( pDC);


	// Draw icon
	if ( m_pIcon != NULL)
	{
		pDC->SetBitmap( m_pIcon);
		pDC->Draw( m_IconRect);
	}
}


// glueCreateChatTab
void XUIChattingBox::glueCreateChatTab( int nBoxID, int nTabID, const char* szName, const char* szIcon)
{
	MBitmap* pBitmap = MBitmapManager::Get( szIcon);
	CreateNewChatCtrl( nBoxID, nTabID, szName, pBitmap);
}


// glueGetChatBoxRect
MWLua::ret_int4 XUIChattingBox::glueGetChatBoxRect( int i)
{
	MRECT r = GetChatBoxRect( i);
	return MWLua::ret_int4( r.x, r.y, r.w, r.h);
}


void XUIChattingBox::glueSetEditText(const char* szText)
{
	m_pEdit->SetText(szText);
}