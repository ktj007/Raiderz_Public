#include "stdafx.h"
#include "XExpCaption.h"

#define EXP_CAPTION_POS_STAY_TIME	(0.0f)

XExpCaption::XExpCaption( MID nID, rs3::RSceneNode* pSceneNode )
: XCombatCaption(nID, pSceneNode)
{

}

bool XExpCaption::OnUpdate( float fDelta, RMatrix& viewInverse )
{
	if (m_fElapsedTime > EXP_CAPTION_POS_STAY_TIME + 1.0f) return false;

	UpdatePosition(m_fElapsedTime, fDelta);

	uint32 nColor = UpdateColor(m_fElapsedTime);
	SetupRenderInfo(viewInverse, nColor);

	return true;
}

void XExpCaption::Setup( int nExp, vec3& pos )
{
	wchar_t szText[32] = L"";
	swprintf_s(szText, L"+%d Exp", nExp);
	uint32 nColor = XCONST::CAPTION_EFFECT_COLOR_MY_EXP;

	SetColor(nColor);

	m_vPosition = pos;
	SetText(szText);
	SetVisible(true);

	SetScale(0.5f);
}

void XExpCaption::UpdatePosition( const float fElapsedTime, const float fDelta )
{
	// pos
	if (fElapsedTime <= EXP_CAPTION_POS_STAY_TIME) return;
	float fTheTime = fElapsedTime - EXP_CAPTION_POS_STAY_TIME;

	if (fTheTime > 0.8f)
	{
		float fOffset = (fTheTime - 0.8f) * (-25.f) + 5.0f;
		m_vPosition.z += (fOffset * fDelta);
	}
	else if (fTheTime > 0.5f)
	{
		m_vPosition.z += (20.0f * fDelta);
	}
}

DWORD XExpCaption::UpdateColor( const float fElapsedTime )
{
	if (fElapsedTime <= EXP_CAPTION_POS_STAY_TIME) return GetColor();
	float fTheTime = fElapsedTime - EXP_CAPTION_POS_STAY_TIME;

	// color
	DWORD dwColor = GetColor();
	if (fTheTime > 0.5f)
	{
		unsigned char alpha = 255 - (unsigned char)(255 * (fTheTime - 0.5f) * 2);

		dwColor &= 0x00ffffff;
		dwColor = (alpha << 24) | dwColor;
	}


	return dwColor;
}
