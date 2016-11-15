#include "stdafx.h"
#include "XHitFrameFreezer.h"
#include "XModuleEntity.h"
#include "XGameTransformControllerManager.h"

void XHitFrameFreezer::FreezeFrame( XObject* pAttacker )
{
	m_bReserve = false;

	if (XCONST::HIT_FREEZE_TIME > 0.001f)
	{
		pAttacker->GetModuleEntity()->GetAnimationController()->FreezeFrame(XCONST::HIT_FREEZE_TIME);
	}
}

void XHitFrameFreezer::OnHit( XObject* pAttacker, const wchar_t* szCurrAni )
{
	if (XCONST::HIT_FREEZE_DELAY <= 0.001f)
	{
		FreezeFrame(pAttacker);
		return;
	}

	m_bReserve = true;
	m_fReserveElapsedTime = 0.0f;
	m_strCurrAni = szCurrAni;
}

void XHitFrameFreezer::Update( float fDelta, XObject* pAttacker, const wchar_t* szCurrAni )
{
	if (m_bReserve == false) return;
	if (wcscmp(m_strCurrAni.c_str(), szCurrAni))
	{
		m_bReserve = false;
		return;
	}

	m_fReserveElapsedTime += fDelta;

	if (m_fReserveElapsedTime >= XCONST::HIT_FREEZE_DELAY)
	{
		m_bReserve = false;
		FreezeFrame(pAttacker);
	}
}