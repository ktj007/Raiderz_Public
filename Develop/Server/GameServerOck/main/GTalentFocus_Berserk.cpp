#include "StdAfx.h"
#include "GTalentFocus_Berserk.h"
#include "GConst.h"
#include "GEntityPlayer.h"

GTalentFocus_Berserk::GTalentFocus_Berserk(GEntityPlayer* pOnwer)
: GTalentFocus(pOnwer)
, m_nHitTalentCounter(0)
{
	AddEnableWeapon(WEAPON_TWO_HANDED);
	AddEnableWeapon(WEAPON_DUAL_WIELD);
	AddEnableWeapon(WEAPON_2H_BLUNT);

	m_rgrRelayHit.SetTickTime(GConst::FOCUS_BERSERK_RELAYHIT_TIME);
}

GTalentFocus_Berserk::~GTalentFocus_Berserk(void)
{
}

bool GTalentFocus_Berserk::CheckActivate()
{
	if (!CheckLicense())
		return false; // 라이센스 가지고 있지 않음

	return (GetHitCount() >= GetMaxHitCount());
}

void GTalentFocus_Berserk::Active()
{
	__super::Active();

	ResetCounter();
}

bool GTalentFocus_Berserk::Update( float fDelta )
{
	if (!IsActive())
	{
		if (m_rgrRelayHit.IsReady(fDelta))
			ResetCounter();
	}

	return false;
}

float GTalentFocus_Berserk::GetDurationTime()
{
	return GConst::FOCUS_BERSERK_DURATION_TIME + (float)m_pOwner->GetPassiveValue(TEPT_FOCUS_BERSERK_EXTENDTIME);
}

void GTalentFocus_Berserk::OnHitMeleeTalent(GTalentInfo* pHitTalentInfo)
{
	VALID(pHitTalentInfo);

	if (!CheckLicense())
		return; // 라이센스 가지고 있지 않음
	if (!pHitTalentInfo->m_bBubbleHit)	
		return; // 버블 히트 가능 탤런트가 아님
	if (IsActive())						
		return; // 이미 활성화가 되어있음
	if (!CheckEquipItem())				
		return; // 필요한 장비가 장착되어있지 않음

	m_nHitTalentCounter++;
	m_rgrRelayHit.Start();
}

void GTalentFocus_Berserk::ResetCounter()
{
	m_nHitTalentCounter = 0;
}

int GTalentFocus_Berserk::GetHitCount()
{
	return m_nHitTalentCounter;
}

int GTalentFocus_Berserk::GetMaxHitCount()
{
	return GConst::FOCUS_BERSERK_HIT_COUNT - m_pOwner->GetPassiveValue(TEPT_FOCUS_BERSERK_DECREASE_MAXBUBBLE);
}

bool GTalentFocus_Berserk::CheckLicense() const 
{
	return (m_pOwner->HasPassiveTalent(TEPT_FOCUS_USE_BERSERK));
}