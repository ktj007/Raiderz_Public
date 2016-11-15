#include "stdafx.h"
#include "MCursor.h"
#include "MBitmap.h"
#include "MDrawContext.h"
#include "MAnimationTemplate.h"


namespace mint3
{


MCursor::MCursor( const char* szName)
{
	SetName( szName);

	m_pBitmap = NULL;
	m_pAnimationTemplate = NULL;
	m_nSceneNum = 0;
	m_ptHotSpot = MPOINT( 0, 0);
	m_dwTimer = timeGetTime();
}


void MCursor::SetBitmap( MBitmap* pBitmap)
{
	if ( m_pBitmap == pBitmap)		return;
	m_pBitmap = pBitmap;

	OnSetBitmap( pBitmap);
}


void MCursor::SetAnimationTemplate( MAnimationTemplate* pAnimationTemplate)
{
	if ( m_pAnimationTemplate == pAnimationTemplate)	return;
	m_pAnimationTemplate = pAnimationTemplate;

	OnSetAnimationTemplate( pAnimationTemplate);
}


void MCursor::Draw( MDrawContext* pDC, int x, int y)
{
	if ( m_pAnimationTemplate)
	{
		bool bTrue = true;
		int nPlayDir = 1;

		m_pAnimationTemplate->UpdateSceneNum( MPM_FORWARD, m_nSceneNum, bTrue, nPlayDir, m_dwTimer);
	}

	OnDraw( pDC, x, y);
}









MDefaultCursor::MDefaultCursor( const char* szName)
: MCursor( szName)
{
}


void MDefaultCursor::OnDraw( MDrawContext* pDC, int x, int y)
{
	if ( m_pAnimationTemplate != NULL)
	{
		int nSplitCount = m_pAnimationTemplate->GetSplitCount();

		MRECT rectScene;
		rectScene.w = m_pBitmap->GetWidth() / nSplitCount;
		rectScene.h = m_pBitmap->GetHeight();
		rectScene.x = rectScene.w * m_nSceneNum;
		rectScene.y = 0;

		pDC->SetBitmap( m_pBitmap);
		pDC->Draw( x, y, rectScene.w, rectScene.h, rectScene.x, rectScene.y, rectScene.w, rectScene.h);
	}

	else if ( m_pBitmap != NULL)
	{
		pDC->SetBitmap( m_pBitmap);
		pDC->Draw( x, y);
	}
}


}
