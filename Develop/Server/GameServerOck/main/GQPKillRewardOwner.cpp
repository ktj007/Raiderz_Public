#include "StdAfx.h"
#include "GQPKillRewardOwner.h"
#include "GEntityActor.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GPlayerPVPArea.h"
#include "GConst.h"
#include "GPartySystem.h"
#include "GDuelMgr.h"

GQPKillRewardOwner::GQPKillRewardOwner(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
, m_nLastDamageTime(ULONG_MAX)
{
	m_pOwner->AttachObserver(this);
}

GQPKillRewardOwner::~GQPKillRewardOwner(void)
{
	m_pOwner->DetachObserver(this);
}

void GQPKillRewardOwner::OnDamage(GEntityActor* pOwner, MUID uidAttacker, int nDamage, EFFECT_SOURCE_TYPE nEffectSourceType, int nEffectSourceID )
{
	GEntityPlayer* pAttacker = gmgr.pPlayerObjectManager->GetEntity(uidAttacker);
	if (NULL == pAttacker) return;
	if (false == m_pOwner->GetPlayerPVPArea().IsLocateAndBelongTeam()) return;
	if (false == pAttacker->GetPlayerPVPArea().IsLocateAndBelongTeam()) return;
	if (m_pOwner->GetPlayerPVPArea().GetTeam() == pAttacker->GetPlayerPVPArea().GetTeam()) return;

	vector<GEntityPlayer*> vecNeighborMember = gsys.pPartySystem->CollectNeighborMember(pAttacker, GConst::QUESTPVP_REWARDOWNER_DISTANCE);

	DWORD nNowTime = gsys.pSystem->GetNowTime();
	ClearPastRewardOwner(nNowTime);

	for each (GEntityPlayer* pNeighborMember in vecNeighborMember)
	{
		if (m_pOwner->GetPlayerPVPArea().GetTeam() == pNeighborMember->GetPlayerPVPArea().GetTeam()) continue;

		Insert(pNeighborMember->GetCID(), pAttacker->GetPartyUID());
	}	

	SaveLastDamageTime(nNowTime);
}

void GQPKillRewardOwner::ClearPastRewardOwner(DWORD nNowTime)
{
	if (ULONG_MAX == m_nLastDamageTime) return;
	if (GConst::QUESTPVP_REWARDOWNER_RELEASETIME > nNowTime - m_nLastDamageTime) return;

	Clear();
}

void GQPKillRewardOwner::SaveLastDamageTime(DWORD nNowTime)
{
	m_nLastDamageTime = nNowTime;
}

void GQPKillRewardOwner::Clear()
{
	__super::Clear();
	m_nLastDamageTime = 0;
}
