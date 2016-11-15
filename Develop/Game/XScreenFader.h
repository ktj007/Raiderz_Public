#pragma once

class XScreenFader
{
private:
	bool	m_bActive;
	bool	m_bFadeOutIn;

	float	m_fStartAlpha;
	float	m_fEndAlpha;
	float	m_fFadingAlpha;

	float	m_fFadeTime;
	float	m_fElapsedTime;

	void	Start(float fStartAlpha, float fEndAlpha, float fTime);

public:
	XScreenFader()
		:m_bActive(false)
		, m_bFadeOutIn(false)
		, m_fStartAlpha(0.f)
		, m_fEndAlpha(0.f)
		, m_fFadingAlpha(0.f)
		, m_fFadeTime(0.f)
		, m_fElapsedTime(0.f) {}

	void	FadeOutIn(float fTime);
	void	FadeInStart( float fTime );
	void	FadeOutStart( float fTime );
	void	Update(float fDelta);
	void	Draw();

	void	FadeUpdate(float fDelta);
	void	FadeEnd()	{ m_bActive = false; }

	bool	IsActive() { return m_bActive; }
};
