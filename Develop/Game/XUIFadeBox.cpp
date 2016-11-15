#include "stdafx.h"
#include "XUIFadeBox.h"




// XUIFadeBox
XUIFadeBox::XUIFadeBox( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	m_tFade.SetReverse( true);
	m_tFade.SetArrivalTime( 300);



	// Message hooking
	m_pFiltered = NULL;
	if ( pParent == NULL)		return;

	// 레이어 위젯을 찾은 다음...
	MWidget* m_pFiltered = pParent;
	while ( 1)
	{
		if ( m_pFiltered->IsLayered() == true)			break;
		else if ( m_pFiltered->GetParent() == NULL)		break;
		m_pFiltered = m_pFiltered->GetParent();
	}

	// 해당 레이어 위젯의 부모에게 메시지 후킹을 건다
	if ( m_pFiltered->GetParent() != NULL)
		m_pFiltered = m_pFiltered->GetParent();

	m_pFiltered->AddMessageFilter( this);
}


// ~XUIFadeBox
XUIFadeBox::~XUIFadeBox()
{
	if ( m_pFiltered != NULL)		m_pFiltered->DeleteMessageFilter( this);
}


// GetDrawState
int XUIFadeBox::GetDrawState()
{
	if ( m_bShow == false  &&  m_tFade.IsArrival() == false)		return true;
	return MWidget::GetDrawState();
}


// OnPreFilterMessage
bool XUIFadeBox::OnPreFilterMessage( MEvent* pEvent)
{
	if ( pEvent->nMessage > MWM_MOUSEFIRST  &&  pEvent->nMessage < MWM_MOUSELAST) 
	{
		if ( global.ui->IsMouseVisible() == false)			m_tFade.SetReverse( true);
		else
		{
			if ( GetWindowRect().InPoint( pEvent->Pos))		m_tFade.SetReverse( false);
			else											m_tFade.SetReverse( true);
		}
	}

	return false;
}


// OnRun
void XUIFadeBox::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	if ( global.ui->IsMouseVisible() == false)		m_tFade.SetReverse( true);
}


// OnPreDraw
void XUIFadeBox::OnPreDraw( MDrawContext* pDC)
{
	float ratio = (float)m_tFade.GetRatio();
	pDC->SetOpacity( pDC->GetOpacity() * ratio);
}
