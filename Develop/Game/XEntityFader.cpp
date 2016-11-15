#include "stdafx.h"
#include "XEntityFader.h"
#include "XCharacter.h"
#include "RMesh.h"

XEntityFader::XEntityFader( XCharacter* pActor ) : m_pActor(pActor), m_bDoing(false), m_bAlpha2PassRendering(false)
{
	if(pActor && pActor->m_pMesh)
		m_bAlpha2PassRendering = pActor->m_pMesh->IsAlpha2PassRendering();
}

void XEntityFader::Start( float fAlpha, float fTime )
{
	if (m_pActor == NULL) return;

	Start(m_pActor->GetVisibility(), fAlpha, fTime);
}

void XEntityFader::Start( float fAlphaFrom, float fAlphaTo, float fTime )
{
	m_fElapsedTime = 0.0f;
	m_fStartAlpha = fAlphaFrom;
	m_fEndAlpha = fAlphaTo;
	m_fFadeTime = fTime;

	if (m_fFadeTime <= 0.0f) m_fFadeTime = 0.0001f;

	m_bDoing = true;
}

void XEntityFader::Update( float fDeltaTime )
{
	if (m_bDoing == false) return;

	m_fElapsedTime += fDeltaTime;

	float t = (m_fElapsedTime / m_fFadeTime);
	if (t > 1.0f) t = 1.0f;

	float fCurrAlpha = MMath::LinearInterpolation(m_fStartAlpha, m_fEndAlpha, t);

	SetVisibility(fCurrAlpha);

	if (m_fElapsedTime >= m_fFadeTime)
	{
		Stop();
	}
}

void XEntityFader::SetVisibility( float fCurrAlpha )
{
	if (m_pActor)
	{
		if (fCurrAlpha >= 1.0f && 
			m_bAlpha2PassRendering == false &&
			(m_pActor->m_pMesh && m_pActor->m_pMesh->IsAlpha2PassRendering() == false)) // 메시가 알파2패스 렌더 속성이면 무조건 알파2패스로
		{
			m_pActor->SetAlpha2PassRendering(false);
		}
		else
		{
			// 알파2패스는 불투명하게 그려지면 안된다.
			if (fCurrAlpha >= 1.f)
				fCurrAlpha = 0.9999f;

			m_pActor->SetAlpha2PassRendering(true);
		}

		m_pActor->SetVisibility(fCurrAlpha);
	}
}

void XEntityFader::Stop()
{
	m_bDoing = false;
}