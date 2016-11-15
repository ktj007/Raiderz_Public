#include "stdafx.h"
#include "XUIPresentationBox.h"



#define DEFAULT_ITEMHEIGHTMARGIN		3
#define DEFAULT_LIFETIME				8000




// XPresentChild
XPresentChild::XPresentChild( const char* szName, MWidget* pParent, MListener* pListener)
: MTextView( szName, pParent, pListener), m_bNotFadeIn(false)
{
	m_bStatic = true;
	m_bShowBorder = false;

	m_nLifeTime = DEFAULT_LIFETIME;

	GetScrollBar()->SetAlwaysHide( true);
	SetAnchor( MAnchorStyle( true, true, true, false));
	SetWrapping( false);

	m_tFade.SetReverse( true);
	m_tFade.SetArrivalTime( 1000);
}

void XPresentChild::SetText( const char* szText)
{
	MTextView::SetText( szText);

	m_tFade.SetReverse( false);
	m_tFade.Start();
}

void XPresentChild::Disappear()
{
	m_tFade.SetReverse( true);
}

void XPresentChild::OnDraw( MDrawContext* pDC)
{
	if (m_bNotFadeIn && m_tFade.IsReversed() == false)
	{
		MTextView::OnDraw( pDC);
	}
	else
	{
		float opacity = pDC->GetOpacity() * min( 1.0, m_tFade.GetRatio());
		float old_opacity = pDC->SetOpacity( opacity);

		MTextView::OnDraw( pDC);

		pDC->SetOpacity( old_opacity);
	}


	if ( m_tFade.IsReversed() == false  &&  m_tFade.GetElapsed() > m_nLifeTime)	m_tFade.SetReverse( true);
	else if ( m_tFade.IsReversed() == true  &&  m_tFade.IsArrival() == true)			Show( false);
}

void XPresentChild::SetFadeTime( DWORD dwTime, bool bNotFadeIn/*=false*/ )
{
	m_bNotFadeIn = bNotFadeIn;
	m_tFade.SetArrivalTime( dwTime);
}

void XPresentChild::SetLifeTime( DWORD dwLifeTime )
{
	m_nLifeTime = dwLifeTime;
}









// XUIPresentationBox
XUIPresentationBox::XUIPresentationBox( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	m_bStatic = true;
	ShowBorder( false);

	m_pTextLook = NULL;
	m_tMove.SetEnable( false);
	m_tMove.SetArrivalTime( 1000);
	m_dwFadeTime = 1000;
	m_bArrangeChildren = false;
}


// ~XUIPresentationBox
XUIPresentationBox::~XUIPresentationBox()
{
	Clear();
}


// Add
void XUIPresentationBox::Add( const char* szString, int nIcon)
{
	Add(szString, nIcon, DEFAULT_LIFETIME / 1000.0f);
}

void XUIPresentationBox::Add( const char* szString, int nIcon, float fLifeTime )
{
	MRECT r = GetClientRect();

	int nHeight = m_pTextLook ? m_pTextLook->GetFont()->GetHeight() : 24;
	char buff[ 256] = { 0};

	switch ( nIcon)
	{
	case PRESENTATION_ICON_EXCLAMATION	: sprintf_s( buff, "{BITMAP name=\"iconDefExclamation\" w=%d h=%d} ", nHeight, nHeight);		break;
	case PRESENTATION_ICON_INFORMATION	: sprintf_s( buff, "{BITMAP name=\"iconDefInformation\" w=%d h=%d} ", nHeight, nHeight);		break;
	case PRESENTATION_ICON_QUESTION		: sprintf_s( buff, "{BITMAP name=\"iconDefQuestion\" w=%d h=%d} ", nHeight, nHeight);		break;
	case PRESENTATION_ICON_STOP			: sprintf_s( buff, "{BITMAP name=\"iconDefStop\" w=%d h=%d} ", nHeight, nHeight);			break;
	}

	string strMsg;
	strMsg += "{ALIGN hor=\"center\" ver=\"center\"}";
	strMsg += buff;
	strMsg += szString;

	bool nNotFadeIn = false;

	if (!m_Children.empty())
	{
		list< _CHILD>::iterator itor = m_Children.begin();
		if ((*itor).m_pTextView->IsVisibled())
			nNotFadeIn = true;
	}

	XPresentChild* pTextView = new XPresentChild( NULL, this, this);
	if ( m_pTextLook != NULL)		pTextView->SetLookAndFill( m_pTextLook);
	pTextView->SetRect( 0, 0, r.w, r.h);
	pTextView->SetFadeTime( m_dwFadeTime, nNotFadeIn);
	pTextView->SetLifeTime( DWORD((fLifeTime + 0.001f) * 1000) );
	pTextView->SetText( strMsg.c_str());


	// Resize and reposition
	pTextView->SetSize( r.w, pTextView->GetPageSize().h + DEFAULT_ITEMHEIGHTMARGIN);


	// Add child list
	m_Children.push_back( _CHILD( pTextView));

	if ( m_Children.size() > 3)
	{
		(*m_Children.begin())._delete();
		m_Children.pop_front();
	}


	m_tMove.SetEnable( true);
	m_tMove.Start();

	ArrangeChildren();
}


// Clear
void XUIPresentationBox::Clear()
{
	for ( list< _CHILD>::iterator itr = m_Children.begin();  itr != m_Children.end();  itr++)
		(*itr)._delete();

	m_Children.clear();
}


// OnPreTranslateMsg
MRESULT XUIPresentationBox::OnPreTranslateMsg( MEvent* pEvent)
{
	// Do not proceed message
	return MNOTPROCESSED;
}


// OnRun
void XUIPresentationBox::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);


	// Arrange child
	if ( m_bArrangeChildren == true  ||  m_tMove.IsEnabled() == true)
	{
		int nPosY = GetClientRect().h;

		for ( list< _CHILD>::reverse_iterator itr = m_Children.rbegin();  itr != m_Children.rend();  itr++)
		{
			MTextView* pTextView = (*itr).m_pTextView;
			int _h = (*itr).m_pTextView->GetSize().h;

			pTextView->SetPosition( pTextView->GetPosition().x, nPosY - _h);

			if ( itr == m_Children.rbegin())
			{
				nPosY -= _h * m_tMove.GetRatio();
				pTextView->SetOpacity( 1.0f);
			}
			else
			{
				nPosY -= _h;
				pTextView->SetOpacity( 0.6f);
			}
		}

		if ( m_tMove.IsArrival() == true)		m_tMove.SetEnable( false);
		m_bArrangeChildren = false;
	}
}


// OnSize
void XUIPresentationBox::OnSize( int w, int h)
{
	MWidget::OnSize( w, h);

	ArrangeChildren();
}
