#include "StdAfx.h"
#include "GDuelMgr.h"
#include "GEntityPlayer.h"
#include "GPlayerDoing.h"
#include "CCommandResultTable.h"
#include "CCommandTable.h"
#include "GCommandCenter.h"
#include "GDuel.h"
#include "GSTLUtil.h"
#include "GGlobal.h"
#include "GServer.h"
#include "GCommand.h"
#include "GRelationChecker.h"
#include "GPartySystem.h"
#include "GModuleCombat.h"

GDuelMgr::GDuelMgr(GField* pField)
: m_pField(pField)
{
}

GDuelMgr::~GDuelMgr(void)
{
	Fini();
}

void GDuelMgr::Fini()
{
	SAFE_DELETE_MAP(m_mapDuels);
}

bool GDuelMgr::CheckDuelRequest( GEntityPlayer* pRequester, GEntityPlayer* pTargetPlayer )
{
	if (pRequester->GetUID() == pTargetPlayer->GetUID())	return pRequester->FailAndRouteSystemMsg(CR_FAIL_DUEL_OPPONENT_IS_YOU);
	if (pRequester->HasDuel())								return pRequester->FailAndRouteSystemMsg(CR_FAIL_DUEL_ALREADY_DUELING);
	if (pTargetPlayer->HasDuel())							return pRequester->FailAndRouteSystemMsg(CR_FAIL_DUEL_TARGET_IS_DUELING);
	if (pRequester->IsStayDynamicField())					return pRequester->FailAndRouteSystemMsg(CR_FAIL_DUEL_INVALID_FIELD);
	if (pTargetPlayer->IsStayDynamicField())				return pRequester->FailAndRouteSystemMsg(CR_FAIL_DUEL_INVALID_FIELD);	
	if (pTargetPlayer->GetDoing().IsBusy())				return pRequester->FailAndRouteSystemMsg(CR_FAIL_DUEL_OPPONENT_IS_BUSY);	
	GRelationChecker relationChecker;
	if (relationChecker.IsEnemy(pRequester, pTargetPlayer))	return pRequester->FailAndRouteSystemMsg(CR_FAIL_DUEL_OPPONENT_IS_ENEMY);	
	if (false == pRequester->GetDoing().IsEndableBusy())
		return false;

	if (pRequester->IsNowCombat())					return pRequester->FailAndRouteSystemMsg(CR_FAIL_DUEL_DONT_DURING_COMBAT);	
	if (pTargetPlayer->IsNowCombat())				return pRequester->FailAndRouteSystemMsg(CR_FAIL_DUEL_TARGET_IS_COMBATING);	

	return true;
}

bool GDuelMgr::CheckPartyDuelRequest( GEntityPlayer* pRequester, const vector<GEntityPlayer*>& pFighters1, const vector<GEntityPlayer*>& pFighters2 )
{
	for each (GEntityPlayer* each in pFighters1)
	{
		if (each->HasDuel())
			return pRequester->FailAndRouteSystemMsg(CR_FAIL_DUEL_TARGET_IS_DUELING);	

		if (each->IsNowCombat())
			return pRequester->FailAndRouteSystemMsg(CR_FAIL_DUEL_TARGET_IS_COMBATING);	
	}

	for each (GEntityPlayer* each in pFighters2)
	{
		if (each->HasDuel())
			return pRequester->FailAndRouteSystemMsg(CR_FAIL_DUEL_TARGET_IS_DUELING);	

		if (each->IsNowCombat())
			return pRequester->FailAndRouteSystemMsg(CR_FAIL_DUEL_TARGET_IS_COMBATING);	
	}

	return true;
}

bool GDuelMgr::DuelRequest( GEntityPlayer* pRequester, MUID uidTargetPlayer )
{
	VALID_RET(pRequester, false);

	GEntityPlayer* pTargetPlayer = pRequester->FindPlayer(uidTargetPlayer);
	RVALID_RET(pTargetPlayer, false);

	if (!CheckDuelRequest(pRequester, pTargetPlayer))
		return false;
	
	pRequester->GetDoing().EndDoing();
	CreateDuel(pRequester, pTargetPlayer);
	RouteQuestion(pRequester, pTargetPlayer);

	return true;
}

bool GDuelMgr::DuelRequest( const vector<GEntityPlayer*>& pFighters1, const vector<GEntityPlayer*>& pFighters2 )
{
	VALID_RET(!pFighters1.empty(), false);
	VALID_RET(!pFighters2.empty(), false);

	GEntityPlayer* pLeader1 = NULL;
	GEntityPlayer* pLeader2 = NULL;
	{
		GParty* pParty = gsys.pPartySystem->FindParty(pFighters1.front()->GetPartyUID());
		if (!pParty)
			return false;

		pLeader1 = m_pField->FindPlayer(pParty->GetLeader());
	}
	{
		GParty* pParty = gsys.pPartySystem->FindParty(pFighters2.front()->GetPartyUID());
		if (!pParty)
			return false;

		pLeader2 = m_pField->FindPlayer(pParty->GetLeader());
	}
	

	VALID_RET(pLeader1, false);
	VALID_RET(pLeader2, false);

	if (!CheckDuelRequest(pLeader1, pLeader2))
		return false;

	if (!CheckPartyDuelRequest(pLeader1, pFighters1, pFighters2))
		return false;

	for each (GEntityPlayer* each in pFighters1)
	{
		if (!each)
			return false;

		each->GetDoing().EndDoing();
	}

	GDuel* pDuel = CreateDuel(pFighters1, pFighters2);
	VALID_RET(pDuel, false);
	
	RouteQuestion(pLeader1, pLeader2);

	return true;
}

void GDuelMgr::DuelQuestionRespond( GDuel* pDuel, bool bAnswer )
{
	RVALID(pDuel);			

	if (bAnswer)
	{
		DuelAccept(pDuel);
	}
	else
	{
		DuelCancel(pDuel);
	}
}

void GDuelMgr::DuelAccept( GDuel* pDuel )
{
	VALID(pDuel);
	pDuel->Ready();
}

void GDuelMgr::DuelCancel( GDuel* pDuel )
{
	VALID(pDuel);
	pDuel->Cancel();
	EraseDuel(pDuel);
}

bool GDuelMgr::DuelKilled(GEntityPlayer* pDeadMan, MUID uidKiller)
{
	VALID_RET(pDeadMan, false);

	GDuel* pDuel = pDeadMan->GetDuel();
	VALID_RET(pDuel, false);

	// 결투 상대에게 죽은 경우
	bool bDuelKilled = false;
	if (pDuel->IsDuelMember(uidKiller))
	{
		pDeadMan->SetHP(1);
		pDeadMan->GetModuleCombat()->SetInvincible(1.0f); // 결투 후 1초간 무적처리 (강화 및 추가적인 디버프로 인한 죽음 막기용도)
		bDuelKilled = true;
	}

	pDuel->Killed(pDeadMan->GetUID());

	return bDuelKilled;
}

void GDuelMgr::Disconnect( GEntityPlayer* pDisconnector )
{
	VALID(pDisconnector);

	GDuel* pDuel = pDisconnector->GetDuel();
	if (!pDuel)		return;

	pDuel->Disconnect( pDisconnector );
}

GDuel* GDuelMgr::CreateDuel(GEntityPlayer* pRequester, GEntityPlayer* pTargetPlayer)
{
	VALID_RET(pRequester, NULL);
	VALID_RET(pTargetPlayer, NULL);
	VALID_RET(pRequester->GetField(), NULL);

	MUID uidNew = gsys.pServer->NewUID();
	GDuel* pNew = new GDuel(uidNew, m_pField, pRequester, pTargetPlayer);
	InsertDuel(uidNew, pNew);

	return pNew;
}

GDuel* GDuelMgr::CreateDuel(const vector<GEntityPlayer*>& pFighters1, const vector<GEntityPlayer*>& pFighters2)
{
	MUID uidNew = gsys.pServer->NewUID();
	GDuel* pNew = new GDuel(uidNew, m_pField, pFighters1, pFighters2);
	InsertDuel(uidNew, pNew);

	return pNew;
}

void GDuelMgr::EraseDuel( GDuel* pDuel )
{
	VALID(pDuel);

	MUID uidDuel = pDuel->GetUID();
	MAP_DUEL::iterator iter = m_mapDuels.find(uidDuel);
	if (iter == m_mapDuels.end())		return;
	
	SAFE_DELETE(pDuel);
	m_mapDuels.erase(uidDuel);
}

GDuel* GDuelMgr::FindDuel( MUID uidDuel )
{
	MAP_DUEL::iterator iter = m_mapDuels.find(uidDuel);
	if (iter == m_mapDuels.end())		return NULL;
	return iter->second;
}

int GDuelMgr::GetCount()
{
	return (int)m_mapDuels.size();
}

void GDuelMgr::Update( float fDelta )
{
	MAP_DUEL::iterator iter = m_mapDuels.begin();
	for (; iter!=m_mapDuels.end();)
	{
		GDuel* pDuel = iter->second;
		pDuel->Update(fDelta);

		if (pDuel->IsFinished())
		{
			SAFE_DELETE(pDuel);
			iter = m_mapDuels.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void GDuelMgr::RouteQuestion( GEntityPlayer* pRequester, GEntityPlayer* pTargetPlayer )
{
	VALID(pRequester);
	VALID(pTargetPlayer);

	MCommand* pNewCmd = MakeNewCommand(MC_DUEL_QUESTION,
		3,
		NEW_UID(pRequester->GetUID()),
		NEW_WSTR(pRequester->GetPlayerInfo()->szName),
		NEW_BOOL(pTargetPlayer->IsNowCombat()));

	pTargetPlayer->RouteToMe(pNewCmd);
}

bool GDuelMgr::IsDuelOpponent(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2) const
{
	VALID_RET(pPlayer1, false);
	VALID_RET(pPlayer2, false);

	GDuel* pDuel = pPlayer1->GetDuel();
	if (!pDuel)
		return false;

	if (pDuel->GetState() != DUEL_FIGHTING)	return false;

	// 결투 상대가 있는가?
	return (pDuel->IsEnemy(pPlayer1->GetUID(), pPlayer2->GetUID()));
}

void GDuelMgr::InsertDuel( const MUID& uidDuel, GDuel* pDuel )
{
	m_mapDuels.insert(MAP_DUEL::value_type(uidDuel, pDuel));
}