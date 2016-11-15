#include "stdafx.h"
#include "XFader.h"
#include "MMath.h"

XFader::XFader()
{
	m_bDoing		= false;

	m_fElapsedTime	= 0.0f;
	m_fStartAlpha	= 1.0f;
	m_fEndAlpha		= 1.0f;
	m_fCurrAlpha	= 1.0f;

	m_eEffectFaderState		= EFS_NONE;
}

XFader::~XFader()
{

}

void XFader::Update( float fDelta )
{
	if (m_bDoing == false) 
		return;

	m_fElapsedTime += fDelta;

	float t = (m_fElapsedTime / m_fFadeTime);
	if (t > 1.0f) 
		t = 1.0f;

	m_fCurrAlpha = MMath::LinearInterpolation(m_fStartAlpha, m_fEndAlpha, t);

	if (m_fElapsedTime >= m_fFadeTime)
	{
		m_bDoing = false;
	}
}

void XFader::Start( float fStartAlpha, float fEndAlpha, float fTime )
{
	m_fElapsedTime	= 0.0f;
	m_fStartAlpha	= fStartAlpha;
	m_fCurrAlpha	= fStartAlpha;
	m_fEndAlpha		= fEndAlpha;
	m_fFadeTime		= fTime;

	if (m_fFadeTime <= 0.0f) 
		m_fFadeTime = 0.0001f;

	m_bDoing		= true;
}
