#include "StdAfx.h"
#include "GConditionsSystem.h"
#include "GConditionsInfo.h"
#include "GConditionInfo.h"
#include "GEntityPlayer.h"
#include "GConditionsInfoMgr.h"
#include "GPlayerTalent.h"
#include "GItemConditionsInfoMgr.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "GPlayerQObjective.h"
#include "GPlayerFactions.h"
#include "GPartySystem.h"
#include "GTalentInfoMgr.h"

GConditionsSystem::GConditionsSystem()
{
}

GConditionsSystem::~GConditionsSystem()
{
}

bool GConditionsSystem::Check(GEntityPlayer* pPlayer, GConditionsInfo* pConditionsInfo, vector<GCondition_Leaf*>& vecFailConditionLeaf)
{
	if (NULL == pPlayer) return false;
	if (NULL == pConditionsInfo) return true;

	return m_ConditionChecker.Check(pConditionsInfo->m_pConditionComposite, pPlayer, vecFailConditionLeaf);
}

bool GConditionsSystem::Check(GEntityPlayer* pPlayer, int nConditionID)
{
	if (NULL == pPlayer) return false;

	GConditionsInfo* pCondtionInfo = gmgr.pCondtionsInfoMgr->Get(nConditionID);
	if (NULL == pCondtionInfo)		return true;

	return Check(pPlayer, pCondtionInfo);
}

bool GConditionsSystem::CheckForPartyConjunction(GEntityPlayer* pPlayer, int nConditionID)
{
	vector<GEntityPlayer*> vecMember;

	if (!gsys.pPartySystem->IsPartyMember(pPlayer))
	{
		vecMember.push_back(pPlayer);
	}
	else
	{
		MUID uidParty = pPlayer->GetPartyUID();
		GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
		_ASSERT(pParty != NULL);

		for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
		{
			MUID uidMember = it->first;

			GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);
			if (NULL == pMember) continue;

			vecMember.push_back(pMember);
		}
	}

	return CheckGroup(vecMember, nConditionID, false);
}

bool GConditionsSystem::CheckForPartyDisjunction(GEntityPlayer* pPlayer, int nConditionID)
{
	vector<GEntityPlayer*> vecMember;

	if (!gsys.pPartySystem->IsPartyMember(pPlayer))
	{
		vecMember.push_back(pPlayer);
	}
	else
	{
		MUID uidParty = pPlayer->GetPartyUID();
		GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
		_ASSERT(pParty != NULL);

		for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
		{
			MUID uidMember = it->first;

			GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);
			if (NULL == pMember) continue;

			vecMember.push_back(pMember);
		}
	}
	
	return CheckGroup(vecMember, nConditionID, true);
}

bool GConditionsSystem::CheckGroup(vector<GEntityPlayer*> vecPlayer, int nConditionID, bool isDisjunction)
{
	if (true == vecPlayer.empty()) return false;

	for each(GEntityPlayer* pPlayer in vecPlayer)
	{
		if (isDisjunction == Check(pPlayer, nConditionID)) return isDisjunction;
	}

	return !isDisjunction;
}


bool GConditionsSystem::CheckForItem( GEntityPlayer* pPlayer, int nConditionID, vector<GCondition_Leaf*>& vecFailConditionLeaf/*=vector<GCondition_Leaf*>()*/ )
{
	if (NULL == pPlayer) return false;

	GConditionsInfo* pCondtionInfo = gmgr.pItemConditionsInfoMgr->Get(nConditionID);
	if (NULL == pCondtionInfo)		return true;

	return Check(pPlayer, pCondtionInfo, vecFailConditionLeaf);
}
