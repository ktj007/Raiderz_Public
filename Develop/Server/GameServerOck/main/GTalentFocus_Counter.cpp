#include "StdAfx.h"
#include "GTalentFocus_Counter.h"
#include "GConst.h"
#include "GEntityPlayer.h"

GTalentFocus_Counter::GTalentFocus_Counter(GEntityPlayer* pOnwer)
: GTalentFocus(pOnwer)
, m_nGuardCount(0)
{
	AddEnableWeapon(WEAPON_1H_SLASH);
	AddEnableWeapon(WEAPON_1H_BLUNT);
}

GTalentFocus_Counter::~GTalentFocus_Counter(void)
{
}

bool GTalentFocus_Counter::CheckActivate()
{
	if (!CheckLicense())
		return false; // 라이센스 가지고 있지 않음

	if (m_nGuardCount > 0)
	{
		m_nGuardCount = 0;
		return true;
	}

	return false;
}

float GTalentFocus_Counter::GetDurationTime()
{
	return GConst::FOCUS_COUNTER_DURATION_TIME + (float)m_pOwner->GetPassiveValue(TEPT_FOCUS_COUNTER_EXTENDTIME);
}

void GTalentFocus_Counter::OnGuard(GEntityActor* pAttacker)
{
	VALID(pAttacker);

	// SEAL(pyo): 디플렉트중에 포커스 안얻게 수정
	// TEST_FIXTURE(FTalentFocus, GainCounterFocusOnOpponentDeflect)
	//
	//if (pAttacker->GetCurrMF() != MF_DEFLECT)
	//	return;		// 공격자가 디플렉트 걸릴때만 포커스를 얻음

	//m_nGuardCount++;
}

void GTalentFocus_Counter::OnAbsoluteGuard(GEntityActor* pAttacker)
{
	VALID(pAttacker);

	if (!CheckLicense())
		return; // 라이센스 가지고 있지 않음

	m_nGuardCount++;
}

bool GTalentFocus_Counter::CheckLicense() const 
{
	return (m_pOwner->HasPassiveTalent(TEPT_FOCUS_USE_COUNTER));
}