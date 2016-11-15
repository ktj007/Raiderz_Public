#include "stdafx.h"
#include "MView.h"
#include "MColorTable.h"

namespace mint3
{

MView::MView( const char* szName, MWidget* pParent, MListener* pListener) : MWidget( szName, pParent, pListener)
{
	char szTmp[ 256];

	sprintf_s( szTmp, "%s_Panel", szName);
	m_pPanel = new MPanel( szTmp, this, this);
	m_pPanel->SetBorderStyle( MBS_NONE);
	m_pPanel->Show( true);


	sprintf_s( szTmp, "%s_ScrollbarVer", szName);
	m_pScrollbarVer = new MScrollBar( szTmp, this, this);
	m_pScrollbarVer->SetType( MSBT_VERTICAL);
	m_pScrollbarVer->SetRange( 0, 100);
	m_pScrollbarVer->Show( true);

	SetClipByParent( true);
}


MView::~MView()
{
}


void MView::OnDraw( MDrawContext* pDC)
{
	if ( (m_pPanel != NULL) && (m_pScrollbarVer != NULL))
	{
		int nWidth = m_Rect.w;
		int nHeight = m_Rect.h;

		for ( int i = 0;  i < m_pPanel->GetChildCount();  i++)
		{
			MWidget* pWidget = m_pPanel->GetChild( i);
			if ( pWidget == NULL)
				continue;


			nWidth  = max( nWidth,   ( pWidget->GetRect().x + pWidget->GetRect().w + 10));
			nHeight = max( nHeight,  ( pWidget->GetRect().y + pWidget->GetRect().h + 10));
		}


		m_pScrollbarVer->Show( (nHeight > m_Rect.h) ? true : false);


		float fPos = -(nHeight - m_Rect.h) * (float)m_pScrollbarVer->GetValue() / 100.0f + 0.5f;
		m_pPanel->SetBounds( 0, (int)fPos, nWidth, nHeight);
	}


#ifdef _DEBUG
	if ( m_pPanel->GetChildCount() < 1)
	{
		pDC->SetOpacity( 0.3f);
		pDC->SetColor( MColorTable::NORMAL );
		pDC->Rectangle( GetClientRect());
		pDC->SetOpacity( 1.0f);

	}
#endif
}


bool MView::OnEvent( MEvent* pEvent, MListener* pListener)
{
	MWidget::OnEvent( pEvent, pListener);
	

#define MAX_WHEEL_RANGE		10

	switch( pEvent->nMessage)
	{
		case MWM_MOUSEWHEEL:
		{
			int nDelta = m_pScrollbarVer->GetValue();

			if ( pEvent->nDelta > 0)		nDelta -= min( pEvent->nDelta,  MAX_WHEEL_RANGE);
			else if ( pEvent->nDelta < 0)	nDelta -= max( pEvent->nDelta, -MAX_WHEEL_RANGE);

			if ( nDelta < 0)				nDelta = 0;	
			else if ( nDelta > 100)			nDelta = 100;

			m_pScrollbarVer->SetValue( nDelta);

			break;
		}
	}


	return false;
}


} // namespace mint3
