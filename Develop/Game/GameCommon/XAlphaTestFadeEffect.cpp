#include "stdafx.h"
#include "XAlphaTestFadeEffect.h"
#include "XCharacter.h"

XAlphaTestFadeEffect::XAlphaTestFadeEffect()
: XEffectBase()
, m_fStartDurationTime(0.0f)
, m_fEndDurationTime(0.0f)
, m_dwColor1(0)
, m_dwColor2(0)
, m_fSize1(0.0f)
, m_fSize2(0.0f)
, m_bInverse(false)
{

}

XAlphaTestFadeEffect::~XAlphaTestFadeEffect()
{

}

bool XAlphaTestFadeEffect::Create( MUID& uid, float fStartDurationTime, float fEndDurationTime )
{
	m_UID = uid;

	m_fStartDurationTime = fStartDurationTime;
	m_fEndDurationTime = fEndDurationTime;

	if(m_fStartDurationTime > 0)
		m_fCurrentTime = 0.0f;
	else if(m_fEndDurationTime > 0)
		m_fCurrentTime = m_fEndDurationTime;

	return true;
}

void XAlphaTestFadeEffect::UpdateXEffect( float fDelta )
{
	m_fD = FadeUpdate(fDelta);

	if(m_fD <= 0.0f || m_fD >= 1.0f)
		m_bDelMe = true;
}

float XAlphaTestFadeEffect::FadeUpdate( float fDelta )
{
	float fd = 0.0f;

	if(m_fStartDurationTime > 0.0f)
	{
		m_fCurrentTime += fDelta;
		fd = m_fCurrentTime / m_fStartDurationTime;

		if(fd > 1.0f)
			fd = 1.0f;
	}
	else if(m_fEndDurationTime > 0.0f)
	{
		m_fCurrentTime -= fDelta;
		fd = m_fCurrentTime / m_fEndDurationTime;

		if(fd < 0.0f)
			fd = 0.0f;
	}

	return fd;
}

void XAlphaTestFadeEffect::SetFadeEffectColor( DWORD& dwColor1, DWORD& dwColor2, float& fSize1, float& fSize2 )
{
	m_dwColor1	= dwColor1;
	m_dwColor2	= dwColor2;

	m_fSize1	= fSize1;
	m_fSize2	= fSize2;
}
