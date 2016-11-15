#include "stdafx.h"
#include "MToolTipManager.h"
#include "Mint.h"


namespace mint3
{


#define WAITTIME_MOUSESTOP			200


// MToolTipManager
MToolTipManager::MToolTipManager()
{
	m_pWidget = NULL;
	m_pToolTip = NULL;
	m_dwTimer = 0;
	m_ptCursor = MPOINT( -1, -1);
	m_bQueryToolTip = false;
}

MToolTipManager::~MToolTipManager()
{
	m_pWidget = NULL;
	if ( m_pToolTip != NULL)
	{
		delete m_pToolTip;
		m_pToolTip = NULL;
	}
}

void MToolTipManager::Reset()
{
	m_pWidget = NULL;
	if ( m_pToolTip != NULL)
	{
		delete m_pToolTip;
		m_pToolTip = NULL;
	}
	m_dwTimer = timeGetTime();
	m_bQueryToolTip = false;
}


void MToolTipManager::ResetCursor()
{
	m_ptCursor = MPOINT( -1, -1);
}


bool MToolTipManager::PostProcessToolTipEvent( MWidget* pMainFrame, MEvent* pEvent)
{
	if ( pEvent->nMessage == MWM_MOUSEMOVE)
	{
		m_ptCursor = pEvent->Pos;

		if ( m_bQueryToolTip == true)
		{
			MWidget* pWidget = pMainFrame->ToolTipWindowFromPoint( m_ptCursor);
			if ( pWidget == m_pWidget)	return false;
		}

		Reset();

		m_dwTimer = timeGetTime();
	}

	return false;
}

bool MToolTipManager::PostProcessToolTipRun( MWidget* pMainFrame, DWORD dwCurrTime)
{
	if ( m_bQueryToolTip == false  &&  (timeGetTime() - m_dwTimer) > WAITTIME_MOUSESTOP)
	{
		Reset();

		m_bQueryToolTip = true;


		if ( MGetMint()->GetDragManager()->IsDragging() == true)
			return false;

		
		if ( m_ptCursor.x < 0  ||  m_ptCursor.y < 0)
			return false;


		m_pWidget = pMainFrame->ToolTipWindowFromPoint( m_ptCursor);
		if ( m_pWidget != NULL)
		{
			m_pToolTip = m_pWidget->EventToolTip();
			if ( m_pToolTip != NULL)
			{
				MSIZE workspaceSize( MGetMint()->GetWorkspaceWidth(), MGetMint()->GetWorkspaceHeight());
				MPOINT pt = CalcToolTipPosition(m_ptCursor, m_pWidget->GetToolTipOffset(), workspaceSize, m_pToolTip->GetSize());
				m_pToolTip->SetPosition( pt);
				m_pToolTip->SetOpacity( 0.0f);
				m_pToolTip->Show( true);

				m_dwOpacityTimer = timeGetTime();
			}
		}

		return true;
	}

	if ( m_pToolTip != NULL  &&  m_pToolTip->GetShow() == true)
	{
		double fElapsed = (double)( timeGetTime() - m_dwOpacityTimer) / 250.0;
		m_pToolTip->SetOpacity( min( 1.0f, (float)fElapsed));
		m_pToolTip->Run( dwCurrTime);
	}
	
	return false;
}

bool MToolTipManager::ChangedToolTipText( MWidget* pWidget)
{
	if ( m_pWidget != pWidget)
		return false;

	Reset();

	m_bQueryToolTip = true;


	if ( MGetMint()->GetDragManager()->IsDragging() == true)
		return false;


	if ( m_ptCursor.x < 0  ||  m_ptCursor.y < 0)
		return false;


	m_pWidget = pWidget;
	if ( m_pWidget != NULL)
	{
		m_pToolTip = m_pWidget->EventToolTip();
		if ( m_pToolTip != NULL)
		{
			MSIZE workspaceSize( MGetMint()->GetWorkspaceWidth(), MGetMint()->GetWorkspaceHeight());
			MPOINT pt = CalcToolTipPosition(m_ptCursor, m_pWidget->GetToolTipOffset(), workspaceSize, m_pToolTip->GetSize());
			m_pToolTip->SetPosition( pt);
			m_pToolTip->SetOpacity( 1.0f);
			m_pToolTip->Show( true);
		}
	}

	return true;
}

mint3::MPOINT MToolTipManager::CalcToolTipPosition( const MPOINT& ptCursor, const MPOINT& ptOffset, const MSIZE& workspaceSize, const MSIZE& toolTipSize )
{
	MPOINT pt( ptCursor);

	if ( pt.x < 0)																pt.x = 0;
	else if ( (pt.x + toolTipSize.w + ptOffset.x) > (workspaceSize.w - 10))		pt.x = max( 0, pt.x - toolTipSize.w - ptOffset.x);
	else																		pt.x += ptOffset.x;

	if ( pt.y < 0)																pt.y = 0;
	else if ( (pt.y + toolTipSize.h + ptOffset.y) > (workspaceSize.h - 10))		pt.y = max( 0, pt.y - toolTipSize.h - ptOffset.y);
	else																		pt.y += ptOffset.y;

	return pt;
}

void MToolTipManager::DrawToolTipImage( MDrawContext* pDC)
{
	if ( m_pToolTip != NULL  &&  m_pToolTip->GetShow() == true)
		m_pToolTip->Draw( pDC);
}


} // namespace mint3
