#include "stdafx.h"
#include "GTalentEventChecker.h"
#include "GTalentInfo.h"
#include "GTalent.h"
#include "GEntityActor.h"

GTalentEventChecker::GTalentEventChecker() 
: m_nCurrActEvent(0)
{

}

bool GTalentEventChecker::CheckActEvent( GTalentInfo* pTalentInfo, float fActElapsedTime, CSTalentEventInfo** ppOutEventInfo )
{
	*ppOutEventInfo = NULL;

	if (IsCheckActEventDone(pTalentInfo))
		return false;

	vector<CSTalentEventInfo>& vecEvents = pTalentInfo->m_Events.m_vecActEvents;
	if (fActElapsedTime >= vecEvents[m_nCurrActEvent].m_fTime1)
	{
		*ppOutEventInfo = &vecEvents[m_nCurrActEvent];
		m_nCurrActEvent++;
		return true;
	}

	return false;
}

bool GTalentEventChecker::IsCheckActEventDone( GTalentInfo* pTalentInfo )
{
	vector<CSTalentEventInfo>& vecEvents = pTalentInfo->m_Events.m_vecActEvents;
	if (vecEvents.empty()) return true;
	if (m_nCurrActEvent >= vecEvents.size()) return true;
	return false;
}

void GTalentEventChecker::OnStart( GTalent* pTalent )
{
	PFI_B(3510, "GTalentEventChecker::OnStart");

	m_TalentProjectile.Init(pTalent);

	PFI_E(3510);
}

void GTalentEventChecker::OnUpdatePhaseAct( GTalent* pTalent, float fDelta )
{
	GTalentInfo* pTalentInfo = pTalent->GetInfo();
	VALID(pTalentInfo);

	PFI_BLOCK_THISFUNC(3520);

	CSTalentEventInfo* pTalentEventInfo = NULL;
	while (CheckActEvent(pTalent->GetInfo(), pTalent->GetActElapsedTime(), &pTalentEventInfo))
	{
		if (!pTalentEventInfo)
			continue;

		switch (pTalentEventInfo->m_nEvent)
		{
			case TE_LAUNCH_PROJECTILE:
				{
					//mlog("GTalentEventChecker::OnUpdatePhaseAct() -> time: %.1f/%.1f,  type: %d\n", 
					//	pTalent->GetActElapsedTime(),
					//	pTalentEventInfo->m_fTime1,
					//	pTalentEventInfo->m_ProjectileInfo.m_nType);

					switch(pTalentEventInfo->m_ProjectileInfo.m_nType)
					{
					case TPT_MISSILE:
						{	
							vec3 vTargetPos = pTalent->GetInitTargetPos();
							m_TalentProjectile.LaunchMissile(vTargetPos, pTalentEventInfo->m_ProjectileInfo);
						}break;
					case TPT_MISSILE_GUIDED:
						{
							const TALENT_TARGET_INFO& infoTarget = pTalent->GetTarget();
							if (infoTarget.IsInvalid())		return;
							m_TalentProjectile.LaunchGuidedMissile(infoTarget.uidTarget, infoTarget.nCapsuleGroupIndex, infoTarget.nCapsuleIndex, pTalentEventInfo->m_ProjectileInfo);
						}break;
					case TPT_HITCAPSULE_GUIDED:
						// 서버에서는 발사체를 사용하지 않음
						break;
					}
				}break;
			case TE_EXTRAACTIVE_1:
			case TE_EXTRAACTIVE_2:
				{
					GEntityActor* pTarget = NULL;
					const TALENT_TARGET_INFO& infoTarget = pTalent->GetTarget();
					if (infoTarget.IsValid())
					{
						pTarget =
							pTalent->GetOwner()->FindActor(infoTarget.uidTarget);
					}

					GExtraActiveTalentRunner ExtraActiveTalentRunner;
					ExtraActiveTalentRunner.Apply(pTalent->GetOwner(), vec3(pTalentEventInfo->m_vLocal.x, pTalentEventInfo->m_vLocal.y, 0.0f), pTalentInfo, pTarget);
				}break;
			case TE_DELAYED_ACT:
				{
					pTalent->ApplyDelayedAct();
				}break;
		}
	}
}
