#include "StdAfx.h"
#include "GRelationSelector.h"
#include "CSChrInfo.h"
#include "GEntityActor.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GGlobal.h"
#include "GDuelMgr.h"
#include "GBattleArenaMgr.h"
#include "GPlayerPVPArea.h"
#include "GDuel.h"
#include "GPlayerFactions.h"
#include "GConfig.h"
#include "GFactionRelationInfo.h"
#include "GFactionRelationInfoMgr.h"
#include "GPartySystem.h"
#include "CSFactionInfo.h"
#include "GSharedField.h"

RELATION_TYPE GRelationSelector::Select(GEntityActor* pReqActor, GEntityActor* pTarActor)
{
	ENTITY_TYPEID nReqType = pReqActor->GetTypeID();
	ENTITY_TYPEID nTarType = pTarActor->GetTypeID();

	if (ETID_PLAYER == nReqType && ETID_PLAYER == nTarType)
	{
		GEntityPlayer* pReqPlayer = ToEntityPlayer(pReqActor);
		GEntityPlayer* pTarPlayer = ToEntityPlayer(pTarActor);

		return ReqPlayerTarPlayer(pReqPlayer, pTarPlayer);
	}
	else if (ETID_PLAYER == nReqType && ETID_NPC == nTarType)
	{
		GEntityPlayer* pReqPlayer = ToEntityPlayer(pReqActor);
		GEntityNPC* pTarNPC = ToEntityNPC(pTarActor);

		return ReqPlayerTarNPC(pReqPlayer, pTarNPC);
	}
	else if (ETID_NPC == nReqType && ETID_PLAYER == nTarType)
	{
		GEntityNPC* pReqNPC = ToEntityNPC(pReqActor);
		GEntityPlayer* pTarPlayer = ToEntityPlayer(pTarActor);

		return ReqNPCTarPlayer(pReqNPC, pTarPlayer);
	}
	else if (ETID_NPC == nReqType && ETID_NPC == nTarType)
	{
		GEntityNPC* pReqNPC = ToEntityNPC(pReqActor);
		GEntityNPC* pTarNPC = ToEntityNPC(pTarActor);

		return ReqNPCTarNPC(pReqNPC, pTarNPC);
	}

	return RT_ALL;
}

RELATION_TYPE GRelationSelector::ReqPlayerTarPlayer(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	if (pReqPlayer->GetField() &&
		pReqPlayer->GetField()->GetDuelMgr()->IsDuelOpponent(pReqPlayer, pTarPlayer))
	{
		return RT_ENEMY;
	}

	if (true == gmgr.pBattleArenaMgr->IsBattleArenaOpponent(pReqPlayer, pTarPlayer) ||
		true == pReqPlayer->GetPlayerPVPArea().IsHittable(pTarPlayer))
	{
		return RT_ENEMY;
	}

	if (true == pTarPlayer->IsDead())
	{
		return RT_ALLYDEAD;
	}

	if (gsys.pPartySystem->IsSamePartyMember(pReqPlayer, pTarPlayer))
	{
		if (pReqPlayer->HasDuel())
		{
			GDuel* pDuel = pReqPlayer->GetDuel();
			if (!pDuel->IsDuelMember(pTarPlayer))
				return RT_NONE; // 결투 중에 같은 편이 아님

			if (pDuel->IsLoser(pReqPlayer->GetUID()))
				return RT_NONE; // 결투 패배자는 참여할 수 없음
		}

		if (pTarPlayer->HasDuel())
		{
			GDuel* pDuel = pTarPlayer->GetDuel();
			if (!pDuel->IsDuelMember(pReqPlayer))
				return RT_NONE; // 결투 중에 같은 편이 아님

			if (pDuel->IsLoser(pReqPlayer->GetUID()))
				return RT_NONE; // 결투 패배자는 참여할 수 없음
		}

		return RT_PARTY;
	}

	if (pTarPlayer->HasDuel())
		return RT_NONE; // 결투 중인 사람은 우호효과 받지 못함

	return RT_ALLY;
}

RELATION_TYPE GRelationSelector::ReqPlayerTarNPC(GEntityPlayer* pReqPlayer, GEntityNPC* pTarNPC)
{	
	if (DUEL_FLAG_NPCID == pTarNPC->GetNPCInfo()->nID)
	{
		return RT_NONE;
	}

	FACTION_RELATION_TYPE nFRT = pReqPlayer->GetPlayerFactions().GetRelation(pTarNPC->GetNPCInfo()->m_nFactionLossID);
	if (FRT_GOOD <= nFRT)
	{
		if (true == pTarNPC->IsDead())
		{
			return RT_ALLYDEAD;
		}
		else
		{
			return RT_ALLY;
		}
	}

	if (pTarNPC->IsNoneAttackable())
		return RT_NONE;

	if (true == pTarNPC->IsAlwaysAttackable())
	{
		return RT_ENEMY;
	}

	if (true == pTarNPC->IsFactionAttackable())
	{
		if (FRT_BAD >= nFRT)
		{
			return RT_ENEMY;
		}
	}

	return RT_ALL;
}

RELATION_TYPE GRelationSelector::ReqNPCTarNPC(GEntityNPC* pReqNPC, GEntityNPC* pTarNPC)
{
	if (DUEL_FLAG_NPCID == pTarNPC->GetNPCInfo()->nID)
	{
		return RT_NONE;
	}

	if (true == pReqNPC->IsEnemy(pTarNPC->GetUID()) || 
		CONFIG_AUTO_TEST_AI == GConfig::m_strAutoTestType)
	{
		return RT_ENEMY;
	}

	FACTION_RELATION_TYPE nFRT = gmgr.pFactionRelationInfoMgr->GetRelation(pReqNPC->GetNPCInfo()->m_nFactionLossID, pTarNPC->GetNPCInfo()->m_nFactionLossID);
	if (FRT_NONE == nFRT) return RT_ALL;

	if (FRT_GOOD <= nFRT)
	{
		if (true == pTarNPC->IsDead())
		{
			return RT_ALLYDEAD;
		}
		else
		{
			return RT_ALLY;
		}
	}

	if (pTarNPC->IsNoneAttackable())
		return RT_NONE;

	if (true == pTarNPC->IsAlwaysAttackable())
	{
		return RT_ENEMY;
	}

	if (true == pTarNPC->IsFactionAttackable())
	{
		if (FRT_BAD >= nFRT)
		{
			return RT_ENEMY;
		}
	}
	
	return RT_ALL;
}

RELATION_TYPE GRelationSelector::ReqNPCTarPlayer(GEntityNPC* pReqNPC, GEntityPlayer* pTarPlayer)
{
	if (true == pReqNPC->IsEnemy(pTarPlayer->GetUID()) ||
		NAAT_ALWAYS == pReqNPC->GetAAType())
	{
		return RT_ENEMY;
	}

	FACTION_RELATION_TYPE nFRT = pTarPlayer->GetPlayerFactions().GetRelation(pReqNPC->GetNPCInfo()->m_nFactionLossID);
	if (FRT_GOOD <= nFRT)
	{
		if (true == pTarPlayer->IsDead())
		{
			return RT_ALLYDEAD;
		}
		else
		{
			return RT_ALLY;
		}
	}

	if (NAAT_FACTION == pReqNPC->GetAAType())
	{
		if (FRT_BAD >= nFRT)
		{
			return RT_ENEMY;
		}			
	}

	return RT_ALL;
}

