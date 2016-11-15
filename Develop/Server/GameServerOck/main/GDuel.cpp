#include "StdAfx.h"
#include "GDuel.h"
#include "CCommandTable.h"
#include "GCommandCenter.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GMath.h"
#include "GServer.h"
#include "GCommand.h"
#include "GEntityNPC.h"
#include "GModuleBuff.h"
#include "GConst.h"
#include "GPartySystem.h"

GDuel::GDuel( MUID uid, GField* pDuelField, GEntityPlayer* pFighter1, GEntityPlayer* pFighter2 )
: m_UID(uid)
, m_nState(DUEL_QUESTION)
, m_pField(pDuelField)
, m_nWinTeam(TEAM_NONE)
, m_nReadyCount(DUEL_READY_COUNT)
, m_pFlag(NULL)
, m_nObserveCount(0)
{
	assert(pFighter1);
	assert(pFighter2);
	assert(m_pField);

	InsertFighter(TEAM_1, pFighter1);
	InsertFighter(TEAM_2, pFighter2);

	CalcCenterPoint();
	CreateFlag();
}

GDuel::GDuel( MUID uid, GField* pDuelField, const vector<GEntityPlayer*>& pFighters1, const vector<GEntityPlayer*>& pFighters2 )
: m_UID(uid)
, m_nState(DUEL_QUESTION)
, m_pField(pDuelField)
, m_nWinTeam(TEAM_NONE)
, m_nReadyCount(DUEL_READY_COUNT)
, m_pFlag(NULL)
{
	assert(m_pField);

	for each (GEntityPlayer* each in pFighters1)
		InsertFighter(TEAM_1, each);
	for each (GEntityPlayer* each in pFighters2)
		InsertFighter(TEAM_2, each);

	CalcCenterPoint();
	CreateFlag();
}

GDuel::~GDuel()
{
	for each (MUID each in m_vecAllFighters)
	{
		GEntityPlayer* pPlayer = GetPlayer(each);
		if (!pPlayer)
			continue;

		pPlayer->SetDuel(NULL);
	}
	
	DestroyFlag();
}

void GDuel::RouteToFighters(MCommand* pNewCmd)
{
	VALID(pNewCmd);

	for each (MUID each in m_vecAllFighters)
	{
		GEntityPlayer* pPlayer = GetPlayer(each);
		if (!pPlayer)
			continue;

		pNewCmd->AddReceiver(pPlayer->GetUID());
	}

	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GDuel::Update( float fDelta )
{
	CheckRunaway();
	if (CheckFinish())
	{
		if (m_nState==DUEL_FIGHTING)
			Finished();
		else
			Cancel();
		return;
	}

	if (m_nState == DUEL_READY)
	{
		if (m_rgrReady.IsReady(fDelta))
		{
			--m_nReadyCount;
			RouteReadyCount();

			if (m_nReadyCount <= 0)
			{
				m_rgrReady.Stop();
				Duel();
			}
		}
	}
}

bool GDuel::CheckFinish()
{
	if (m_vecFighters1.size() == m_setLosers1.size())	
	{
		m_nWinTeam = TEAM_2;
		return true;
	}

	if (m_vecFighters2.size() == m_setLosers2.size())	
	{
		m_nWinTeam = TEAM_1;
		return true;
	}

	return false;
}

void GDuel::Ready()
{
	VALID(m_nState==DUEL_QUESTION);

	if (!CheckReady())
	{
		Cancel();
	}

	m_nState = DUEL_READY;
	m_rgrReady.SetTickTime(1.0f);
	m_rgrReady.Start();
	
	// 결투 이벤트
	for each (MUID each in m_vecFighters1)
	{
		GEntityPlayer* pPlayer = GetPlayer(each);
		if (!pPlayer)
			continue;

		pPlayer->OnDuelReady(m_vecFighters2);
	}
	for each (MUID each in m_vecFighters2)
	{
		GEntityPlayer* pPlayer = GetPlayer(each);
		if (!pPlayer)
			continue;

		pPlayer->OnDuelReady(m_vecFighters1);
	}

	RouteReady();
	RouteReadyCount();
}

void GDuel::Duel()
{
	VALID(m_nState==DUEL_READY);

	m_nState = DUEL_FIGHTING;

	// 결투 이벤트
	for each (MUID each in m_vecFighters1)
	{
		GEntityPlayer* pPlayer = GetPlayer(each);
		if (!pPlayer)
			continue;

		pPlayer->OnDuelStart(m_vecFighters2);
	}
	for each (MUID each in m_vecFighters2)
	{
		GEntityPlayer* pPlayer = GetPlayer(each);
		if (!pPlayer)
			continue;

		pPlayer->OnDuelStart(m_vecFighters1);
	}

	RouteFight();
}

void GDuel::Cancel()
{
	m_nState = DUEL_FINISH;
	
	// 결투 이벤트
	for each (MUID each in m_vecFighters1)
	{
		GEntityPlayer* pPlayer = GetPlayer(each);
		if (!pPlayer)
			continue;

		pPlayer->SetDuel(NULL);
		pPlayer->OnDuelFinish(m_vecFighters2);
	}
	for each (MUID each in m_vecFighters2)
	{
		GEntityPlayer* pPlayer = GetPlayer(each);
		if (!pPlayer)
			continue;

		pPlayer->SetDuel(NULL);
		pPlayer->OnDuelFinish(m_vecFighters1);
	}

	RouteCancel();
}

void GDuel::Finished()
{
	VALID(m_nState==DUEL_FIGHTING);

	m_nState = DUEL_FINISH;

	// 결투 이벤트
	for each (MUID each in m_vecFighters1)
	{
		GEntityPlayer* pPlayer = GetPlayer(each);
		if (!pPlayer)
			continue;

		pPlayer->SetDuel(NULL);
		pPlayer->OnDuelFinish(m_vecFighters2);
	}
	for each (MUID each in m_vecFighters2)
	{
		GEntityPlayer* pPlayer = GetPlayer(each);
		if (!pPlayer)
			continue;

		pPlayer->SetDuel(NULL);
		pPlayer->OnDuelFinish(m_vecFighters1);
	}

	Cleanup();

	RouteFinished();	
}

void GDuel::Killed( MUID uidDeadMan )
{
	InsertLoser(uidDeadMan);
	GEntityPlayer* pDeadMan = GetPlayer(uidDeadMan);
	if (!pDeadMan)
		return;

	RemoveAllDebuff(pDeadMan);
	RouteDefeated(pDeadMan);
}

void GDuel::Disconnect( GEntityPlayer* pDisconnector )
{
	VALID(pDisconnector);
	InsertLoser(pDisconnector->GetUID());
}

bool GDuel::IsDuelMember( MUID uidInvoker )
{
	for each (MUID each in m_vecAllFighters)
	{
		if (uidInvoker == each)
			return true;
	}

	return false;
}

bool GDuel::IsDuelMember( GEntityActor* pInvoker )
{
	VALID_RET(pInvoker, false);
	return IsDuelMember(pInvoker->GetUID());
}

void GDuel::CalcCenterPoint()
{
	VALID(!m_vecFighters1.empty());
	VALID(!m_vecFighters2.empty());
	GEntityPlayer* pLeader1 = GetLeader1();
	VALID(pLeader1);
	GEntityPlayer* pLeader2 = GetLeader2();
	VALID(pLeader2);

	vec3 v1 = pLeader1->GetPos();
	vec3 v2 = pLeader2->GetPos();

	float fDistance = v1.DistanceTo(v2);
	vec3 vDir = v2 - v1;
	GMath::NormalizeZ0(vDir);

	m_vCenterPos = v1 + (vDir*(fDistance/2));
}

void GDuel::CreateFlag()
{
	GEntityNPC::ReserveUserdata(DUEL_FLAG_NPCID, 3);
	m_pFlag = m_pField->SpawnNPC(DUEL_FLAG_NPCID, NULL, m_vCenterPos, vec3(0.0f,1.0f,0.0f));
	VALID(m_pFlag);
	m_pFlag->AddRef();
	m_pFlag->SetUserdata(1, GetUID().High);
	m_pFlag->SetUserdata(2, GetUID().Low);
}

void GDuel::DestroyFlag()
{
	if (!m_pFlag)	return;
	m_pFlag->DeleteMe();
	m_pFlag->Drop();
}

bool GDuel::CheckReady()
{
	for each (MUID each in m_vecAllFighters)
	{
		if (!GetPlayer(each))
			return false;
	}
	VALID_RET(m_pField, false);
	VALID_RET(m_pFlag, false);
	VALID_RET(m_vCenterPos != vec3::ZERO, false);
		
	return true;
}

bool GDuel::CheckRunawayImpl(MUID uid)
{
	GEntityPlayer* pPlayer = GetPlayer(uid);
	if (!pPlayer)
		return true; // 플레이어 없음

	if (pPlayer->GetField() == NULL)
		return true; // 필드 없음

	if (pPlayer->GetField() != m_pField)
		return true; // 다른 필드
	
	if (m_vCenterPos.DistanceTo(pPlayer->GetPos()) > DUEL_RADIUS)
		return true; // 결투 중점으로부터 도망쳤는지 체크

	return false;
}

void GDuel::CheckRunaway()
{
	for each (MUID uid in m_vecAllFighters)
	{
		if (IsLoser(uid))
			continue;

		if (!CheckRunawayImpl(uid))
			continue;

		InsertLoser(uid);

		GEntityPlayer* pLoser = GetPlayer(uid);
		if (!pLoser)
			continue;

		RemoveAllDebuff(pLoser);
		RouteDefeated(pLoser);
	}
}

GDuel::DUEL_TEAM GDuel::GetOpponentTeam(DUEL_TEAM nTeam)
{
	return (nTeam == TEAM_1)?TEAM_2:TEAM_1;
}

vector<MUID>& GDuel::GetOpponentTeamUIDList( DUEL_TEAM nTeam )
{
	return GetTeamUIDList(GetOpponentTeam(nTeam));
}

vector<MUID>& GDuel::GetTeamUIDList( DUEL_TEAM nTeam )
{
	switch(nTeam)
	{
	case TEAM_1:	return m_vecFighters1; 
	case TEAM_2:	return m_vecFighters2; 
	}

	static vector<MUID> null_muid_vector;
	return null_muid_vector;
}

vector<MUID>& GDuel::GetAllFightersUIDList()
{
	return m_vecAllFighters;
}

void GDuel::RouteReady()
{
	VALID(!m_vecFighters1.empty());
	VALID(!m_vecFighters2.empty());

	MCommand* pNewCmd = NULL;
	if (IsSingleDuel())
	{
		// 1대1 결투
		pNewCmd = MakeNewCommand(MC_DUEL_READY, 2,
			NEW_UID(m_vecFighters1.front()),
			NEW_UID(m_vecFighters2.front()));
	}
	else
	{
		// 다대다 전투
		pNewCmd = MakeNewCommand(MC_DUEL_PARTY_READY, 2,
			NEW_BLOB(m_vecFighters1),
			NEW_BLOB(m_vecFighters2));
	}
	
	RouteToFighters(pNewCmd);
}

void GDuel::RouteFight()
{
	VALID(!m_vecFighters1.empty());
	VALID(!m_vecFighters2.empty());

	MCommand* pNewCmd = NULL;
	if (IsSingleDuel())
	{
		// 1대1 결투
		pNewCmd = MakeNewCommand(MC_DUEL_FIGHT, 2,
			NEW_UID(m_vecFighters1.front()),
			NEW_UID(m_vecFighters2.front()));
	}
	else
	{
		// 다대다 전투
		pNewCmd = MakeNewCommand(MC_DUEL_PARTY_FIGHT, 1,
			NEW_BLOB(m_vecAllFighters));
	}

	RouteToFighters(pNewCmd);
}

void GDuel::RouteFinished()
{
	VALID(m_nWinTeam != TEAM_NONE);
	vector<MUID> vecWinners = GetTeamUIDList(m_nWinTeam);
	vector<MUID> vecLosers = GetOpponentTeamUIDList(m_nWinTeam);
	VALID(!vecWinners.empty());
	VALID(!vecLosers.empty());

	MCommand* pNewCmd = NULL;
	if (IsSingleDuel())
	{
		// 1대1 결투
		pNewCmd = MakeNewCommand(MC_DUEL_FINISHED, 2,
			NEW_UID(vecWinners.front()),
			NEW_UID(vecLosers.front()));
	}
	else
	{
		GEntityPlayer* pWinner = m_nWinTeam == TEAM_1?GetLeader1():GetLeader2();
		VALID(pWinner);
		GEntityPlayer* pLoser = m_nWinTeam == TEAM_1?GetLeader2():GetLeader1();
		VALID(pLoser);

		// 다대다 전투
		pNewCmd = MakeNewCommand(MC_DUEL_PARTY_FINISHED, 5,
			NEW_INT(m_nWinTeam),
			NEW_UID(pWinner->GetUID()),
			NEW_UID(pLoser->GetUID()),
			NEW_INT(vecWinners.size()),
			NEW_INT(vecLosers.size())
			);
	}

	GEntityPlayer* pWinner = GetPlayer(vecWinners.front());
	if (NULL == pWinner) return;

	pWinner->RouteToThisCell(pNewCmd);
}

void GDuel::RouteDefeated(GEntityPlayer* pDefeater)
{
	VALID(pDefeater);
	
	MCommand* pNewCmd = MakeNewCommand(MC_DUEL_DEFEATED, 1,
		NEW_UID(pDefeater->GetUID()));

	pDefeater->RouteToThisCell(pNewCmd);
}

void GDuel::RouteCancel()
{
	VALID(!m_vecFighters1.empty());
	VALID(!m_vecFighters2.empty());

	MCommand* pNewCmd = NULL;
	if (IsSingleDuel())
	{
		// 1대1 결투
		pNewCmd = MakeNewCommand(MC_DUEL_CANCEL, 2,
			NEW_UID(m_vecFighters1.front()),
			NEW_UID(m_vecFighters2.front()));
	}
	else
	{
		// 다대다 전투
		pNewCmd = MakeNewCommand(MC_DUEL_PARTY_CANCEL, 1,
			NEW_BLOB(m_vecAllFighters));
	}

	RouteToFighters(pNewCmd);
}

void GDuel::RouteReadyCount()
{
	MCommand* pNewCmd = MakeNewCommand(MC_DUEL_READY_COUNT, 1, NEW_INT(m_nReadyCount));
	RouteToFighters(pNewCmd);
}

void GDuel::Cleanup()
{
	for each (MUID each in m_vecAllFighters)
	{
		GEntityPlayer* pPlayer = GetPlayer(each);
		if (!pPlayer)
			continue;

		RemoveAllDebuff(pPlayer);
	}
}

void GDuel::RemoveAllDebuff( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);
	GModuleBuff* pcBuff = pPlayer->GetModuleBuff();
	pcBuff->RemoveAllDebuff();
}

GEntityPlayer* GDuel::GetPlayer( MUID uidPlayer )
{
	VALID_RET(m_pField, NULL);
	return m_pField->FindPlayer(uidPlayer);
}

void GDuel::InsertFighter( DUEL_TEAM nTeam, GEntityPlayer* pPlayer )
{
	VALID(pPlayer);
	pPlayer->SetDuel(this);
	MUID uidPlayer = pPlayer->GetUID();
	
	switch (nTeam)
	{
	case TEAM_1:
		m_vecFighters1.push_back(uidPlayer);
		break;
	case TEAM_2:
		m_vecFighters2.push_back(uidPlayer);
		break;
	default:
		VALID(false);
	}
	
	m_mapDuelTeamIndex.insert(make_pair(uidPlayer, nTeam));
	m_vecAllFighters.push_back(uidPlayer);
}

GDuel::DUEL_TEAM GDuel::GetTeam( MUID uidPlayer )
{
	for each (MUID each in m_vecFighters1)
	{
		if (each == uidPlayer)
			return TEAM_1;
	}
	for each (MUID each in m_vecFighters2)
	{
		if (each == uidPlayer)
			return TEAM_2;
	}	

	return TEAM_NONE;
}

void GDuel::InsertLoser( MUID uidPlayer )
{
	DUEL_TEAM nTeam = GetTeam(uidPlayer);
	if (nTeam == TEAM_NONE)
		return;

	if (m_nState == DUEL_FINISH)	
		return;	// 동일한 틱에 이미 결투상대자가 죽었다.

	switch (nTeam)
	{
	case TEAM_1:
		m_setLosers1.insert(uidPlayer);
		break;
	case TEAM_2:
		m_setLosers2.insert(uidPlayer);
		break;
	}
}

bool GDuel::IsEnemy( MUID uidPlayer, MUID uidOpponent )
{
	DUEL_TEAM nTeam = GetTeam(uidPlayer);
	const vector<MUID>& vecOpponents = GetOpponentTeamUIDList(nTeam);
	for each (MUID each in vecOpponents)
	{
		switch (nTeam)
		{
		case TEAM_1:
			if (m_setLosers1.find(uidPlayer) != m_setLosers1.end())
				return false; // 이미 패배한 참여자
			if (m_setLosers2.find(uidOpponent) != m_setLosers2.end())
				return false; // 이미 패배한 참여자
			break;
		case TEAM_2:
			if (m_setLosers2.find(uidPlayer) != m_setLosers2.end())
				return false; // 이미 패배한 참여자
			if (m_setLosers1.find(uidOpponent) != m_setLosers1.end())
				return false; // 이미 패배한 참여자
			break;
		default:
			VALID_RET(false, false);
		}

		if (each == uidOpponent)
			return true;
	}

	return false;
}

bool GDuel::IsLoser( MUID uid )
{
	if (m_setLosers1.find(uid) != m_setLosers1.end())
		return true;

	if (m_setLosers2.find(uid) != m_setLosers2.end())
		return true;

	return false;
}

bool GDuel::IsSingleDuel()
{
	if (GConst::DUEL_PARTY_TEST)
		return false;

	return (m_vecFighters1.size() == 1 &&
			m_vecFighters2.size() == 1);
}

GEntityPlayer* GDuel::GetLeaderImpl(const vector<MUID>& vecFighters)
{
	if (vecFighters.empty())
		return NULL;

	GEntityPlayer* pAnyone = GetPlayer(vecFighters.front());
	if (!pAnyone)
		return NULL;

	GParty* pParty = gsys.pPartySystem->FindParty(pAnyone->GetPartyUID());
	if (!pParty)
		return pAnyone;

	return GetPlayer(pParty->GetLeader());
}

GEntityPlayer* GDuel::GetLeader1()
{
	if (IsSingleDuel())
	{
		return GetPlayer(m_vecFighters1.front());
	}

	return GetLeaderImpl(m_vecFighters1);
}

GEntityPlayer* GDuel::GetLeader2()
{
	if (IsSingleDuel())
	{
		return GetPlayer(m_vecFighters2.front());
	}

	return GetLeaderImpl(m_vecFighters2);
}

bool GDuel::IsObservable() const
{
	return (GetObserveCount() < GConst::DUEL_LIMIT_OBSERVE_COUNT);
}

void GDuel::IncreaseObserveCount()
{
	++m_nObserveCount;
}

void GDuel::DecreaseObserveCount()
{
	--m_nObserveCount;
}

int GDuel::GetObserveCount() const
{
	return m_nObserveCount;
}

