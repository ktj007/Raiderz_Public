#pragma once

class XCharacter;

class XEntityFader
{
private:
	XCharacter*		m_pActor;
	float			m_fStartAlpha;
	float			m_fEndAlpha;
	float			m_fFadeTime;
	float			m_fElapsedTime;
	bool			m_bDoing;
	bool			m_bAlpha2PassRendering;
	void SetVisibility( float fCurrAlpha );
public:
	XEntityFader(XCharacter* pActor);
	void Start(float fAlpha, float fTime);
	void Start(float fAlphaFrom, float fAlphaTo, float fTime);
	void Stop();
	void Update(float fDeltaTime);
	bool IsDoing() { return m_bDoing; }
};