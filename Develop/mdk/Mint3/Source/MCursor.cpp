#include "stdafx.h"
#include "MCursor.h"
#include "MBitmap.h"
#include "MDrawContext.h"
#include "MAnimation.h"

namespace mint3 {

MCursor::MCursor(const char* szName) : m_pAnimationTemplate(NULL), m_pBitmap(NULL), m_nSceneNum(0)
{
	SetName(szName);
	m_dwTimer = timeGetTime();
}

void MCursor::SetBitmap(MBitmap* pBitmap)
{
	m_pBitmap = pBitmap;
	OnSetBitmap(pBitmap);
}

void MCursor::SetAnimationTemplate(MAnimationTemplate* pAnimationTemplate)
{
	m_pAnimationTemplate = pAnimationTemplate;
	OnSetAnimationTemplate(pAnimationTemplate);
}

void MCursor::Draw(MDrawContext* pDC, int x, int y)
{
	if(m_pAnimationTemplate)
	{
		bool bTrue = true;
		int nPlayDir = 1;

		m_pAnimationTemplate->UpdateSceneNum( MPM_FORWARD, m_nSceneNum, bTrue, nPlayDir, m_dwTimer);
	}

	OnDraw( pDC, x, y);
}


//////////////////////////////////////////////////////////////////////////
// Mint 의 bitmap 펑션을 사용 하는 기본 커서

MDefaultCursor::MDefaultCursor(const char* szName) : MCursor(szName)
{
}

void MDefaultCursor::OnDraw(MDrawContext* pDC, int x, int y)
{
	// 애니메이션 커서
	if(m_pAnimationTemplate)
	{
		int nSplitCount = m_pAnimationTemplate->GetSplitCount();

		MRECT rectScene;
		rectScene.w = m_pBitmap->GetWidth() / nSplitCount;
		rectScene.h = m_pBitmap->GetHeight();
		rectScene.x = rectScene.w * m_nSceneNum;
		rectScene.y = 0;
		pDC->SetBitmap(m_pBitmap);
		pDC->Draw( x, y, rectScene.w, rectScene.h, rectScene.x, rectScene.y, rectScene.w, rectScene.h);
	}else
	if(m_pBitmap)
	{
		pDC->SetBitmap(m_pBitmap);
		pDC->Draw(x, y);
	}
}

} // namespace mint3