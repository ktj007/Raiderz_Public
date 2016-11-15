#pragma once

enum EFFECT_FADER_STATE
{
	EFS_NONE,
	EFS_START,
	EFS_END,
};

class XFader
{
private:
	float			m_fStartAlpha;
	float			m_fEndAlpha;
	float			m_fCurrAlpha;
	float			m_fFadeTime;
	float			m_fElapsedTime;
	bool			m_bDoing;

	EFFECT_FADER_STATE	m_eEffectFaderState;

public:
	XFader();
	virtual ~XFader();
	
	void			Start(float fStartAlpha, float fEndAlpha, float fTime);
	void			Update( float fDelta );
	bool			IsDoing() { return m_bDoing; }
	float			GetCurrAlpha() { return m_fCurrAlpha; }

	EFFECT_FADER_STATE GetFaderState() { return m_eEffectFaderState; }
	void			SetFaderState(EFFECT_FADER_STATE eState) { m_eEffectFaderState = eState; }
};
