#include "stdafx.h"
#include "XScreenFader.h"
#include "XUISystem.h"
#include "MMath.h"

void XScreenFader::Start( float fStartAlpha, float fEndAlpha, float fTime )
{
	m_fElapsedTime	= 0.0f;
	m_fStartAlpha	= fStartAlpha;
	m_fFadingAlpha	= fStartAlpha;
	m_fEndAlpha		= fEndAlpha;
	m_fFadeTime		= fTime;

	if (m_fFadeTime <= 0.0f) 
		m_fFadeTime = 0.0001f;

	m_bActive = true;
}

void XScreenFader::FadeInStart( float fTime )
{
	Start(1.f, 0.f, fTime);
}

void XScreenFader::FadeOutStart( float fTime )
{
	Start(0.f, 1.f, fTime);
}

void XScreenFader::FadeOutIn(float fTime)
{
	Start(0.f, 1.f, fTime);

	m_bFadeOutIn = true;
}

void XScreenFader::Update( float fDelta )
{
	if (m_bActive == false) return;

	m_fElapsedTime += fDelta;

	float t = (m_fElapsedTime / m_fFadeTime);
	if (t > 1.0f) t = 1.0f;

	m_fFadingAlpha = MMath::LinearInterpolation(m_fStartAlpha, m_fEndAlpha, t);

	if (m_fElapsedTime >= m_fFadeTime)
	{
		if (m_bFadeOutIn)
		{
			Start(1.f, 0.f, m_fFadeTime);

			m_bFadeOutIn = false;
		}
		else
		{
			m_bActive = false;
		}
	}
}

void XScreenFader::Draw()
{
	if (m_bActive == false) return;

	global.ui->GetDC()->BeginDraw();

	int max_width = global.ui->GetMint()->GetWorkspaceWidth();
	int max_height = global.ui->GetMint()->GetWorkspaceHeight();

	unsigned char alpha = (unsigned char)(255 * m_fFadingAlpha);
	global.ui->GetDC()->SetColor(MCOLOR(0, 0, 0, alpha));
	global.ui->GetDC()->FillRectangle(0, 0, max_width, max_height);

	global.ui->GetDC()->EndDraw();
}

void XScreenFader::FadeUpdate( float fAlpha )
{
	m_fFadingAlpha = fAlpha;
	m_bActive = true;
}

