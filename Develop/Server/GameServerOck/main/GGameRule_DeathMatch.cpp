#include "StdAfx.h"
#include "GGameRule_DeathMatch.h"
#include "GDef.h"
#include "GPlayerBattleArena.h"
#include "GGlobal.h"
#include "GPartySystem.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GConst.h"
#include "GCommandCenter.h"
#include "GRebirthSystem.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"

GGameRule_DeathMatch::GGameRule_DeathMatch()
: m_nMaxLifeCounter(5)
, m_fElapsedTime(0.0f)
{
}

GBattleArena::GameRule* GGameRule_DeathMatch::Clone()
{
	GGameRule_DeathMatch* pClone = new GGameRule_DeathMatch();
	pClone->SetMaxLifeCounter(m_nMaxLifeCounter);
	return pClone;
}

void GGameRule_DeathMatch::SetMaxLifeCounter( int nMaxLifeCounter )
{
	m_nMaxLifeCounter = nMaxLifeCounter;
}

void GGameRule_DeathMatch::Init( const vector<const MARKER_INFO*> vecStaringMarkers, const BattleArena::TEAM_MAP& mapEntries )
{
	VALID(vecStaringMarkers.size() >= 2);

	m_mapTeams = mapEntries;

	vector<const MARKER_INFO*>::const_iterator itMarkers = vecStaringMarkers.begin();
	for (BattleArena::TEAM_MAP::const_iterator it = m_mapTeams.begin();
		it != m_mapTeams.end();
		++it, ++itMarkers)
	{
		MUID uidTeam = it->first;
		m_mapLifeCounter.insert(std::make_pair(uidTeam, m_nMaxLifeCounter));
		m_mapStaringMarkers.insert(std::make_pair(uidTeam, *itMarkers));
	}

	// 데스매치용 준비 패킷 날리기
	MCommand* pNewCmd = MakeNewCommand(MC_BATTLEARENA_READY_DEATHMATCH, 1, 
		NEW_INT(m_nMaxLifeCounter)
		);
	RouteToAllTeam(pNewCmd);
}

void GGameRule_DeathMatch::Update( float fDelta )
{
	m_fElapsedTime += fDelta;

	for (REBIRTHTIME_MAP::iterator it = m_mapRebirthTimers.begin();
		it != m_mapRebirthTimers.end();)
	{
		pair<MRegulator,GEntityPlayer*>& each_pair = it->second;
		MRegulator& regulator = each_pair.first;
		GEntityPlayer* pPlayer = each_pair.second;
		if (regulator.IsReady(fDelta))
		{
			it = m_mapRebirthTimers.erase(it);
			// 자동부활될 때가 됐음
			Rebirth(pPlayer);

		}
		else
		{
			++it;
		}
	}
}

bool GGameRule_DeathMatch::IsFinished( const BattleArena::TEAM_MAP& mapTeams, MUID& uidWinner )
{
	// 종료할때 아무도 없다면 조용히 처리
	if (m_mapTeams.empty())
	{
		return true;
	}

	// 한팀만 존재하는 경우 승리처리
	if (m_mapTeams.size() == 1)
	{
		uidWinner = m_mapTeams.begin()->first;
		OnFinished(uidWinner);
		return true;
	}

	// 점수 계산
	size_t nSurviveTeamQty = mapTeams.size();
	for (BattleArena::TEAM_MAP::const_iterator it = m_mapTeams.begin();
		it != m_mapTeams.end();
		++it)
	{
		MUID uidTeam = it->first;
		VALID_RET(m_mapLifeCounter.find(uidTeam) != m_mapLifeCounter.end(), false);

		// 남은 라이프가 없다면 전멸
		int nLife = m_mapLifeCounter[uidTeam];
		if (nLife <= 0)
			--nSurviveTeamQty;
		else
			uidWinner = uidTeam;
	}

	// 엔트리가 하나만 남으면 승리자
	if (nSurviveTeamQty == 1)
	{
		OnFinished(uidWinner);
		return true;
	}

	// 여러 엔트리가 남았다면 승패가 갈리지 않았음
	return false;
}

void GGameRule_DeathMatch::RouteToTeam( const BattleArena::Team& team, MCommand* pCommand )
{
	VALID(pCommand);
	GetPartySys()->RouteToAllMembers(team.UID, pCommand);
}

void GGameRule_DeathMatch::RouteToAllTeam( MCommand* pCommand )
{
	VALID(pCommand);
	for (BattleArena::TEAM_MAP::iterator it = m_mapTeams.begin();
		it != m_mapTeams.end();
		++it)
	{
		BattleArena::Team& team = it->second;
		RouteToTeam(team, pCommand->Clone());
	}
	SAFE_DELETE(pCommand);
}

void GGameRule_DeathMatch::OnTeamMemberDie( GEntityPlayer* pPlayer, const BattleArena::Team& team )
{
	VALID(pPlayer);
	VALID(m_mapLifeCounter.find(team.UID) != m_mapLifeCounter.end());

	AddRebirthTimer(pPlayer->GetUID(), pPlayer);

	// 죽음 패킷 날림
	RouteDie(pPlayer->GetUID());

	// 라이프 줄어듬
	--m_mapLifeCounter[team.UID];

	// L"xxx가 yyy를 죽였습니다." 패킷
	RouteKill(pPlayer);

	// 점수 패킷 알려줌
	RouteScore();
}

void GGameRule_DeathMatch::OnRebirthTeamMemberBySelf( GEntityPlayer* pPlayer, const BattleArena::Team& team )
{
	VALID(pPlayer);
	
	EraseRebirthTimer(pPlayer->GetUID());
}

void GGameRule_DeathMatch::OnRebirthTeamMemberByOther( GEntityPlayer* pPlayer, const BattleArena::Team& team )
{
	VALID(pPlayer);
	VALID(m_mapLifeCounter.find(team.UID) != m_mapLifeCounter.end());

	EraseRebirthTimer(pPlayer->GetUID());

	// 줄어든 라이프 회복됨
	++m_mapLifeCounter[team.UID];

	// 점수 패킷 알려줌
	RouteScore();
}

void GGameRule_DeathMatch::OnDisconnect( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);

	EraseRebirthTimer(pPlayer->GetUID());
}

void GGameRule_DeathMatch::OnDeleteTeam( const BattleArena::Team& team )
{
	m_mapTeams.erase(team.UID);
}

void GGameRule_DeathMatch::OnFinished(MUID uidWinner)
{
	VALID(m_mapLifeCounter.size() >= 2);

	// 엔트리 점수 구조체 만들기
	size_t i=0; 
	vector<int> vecTeamScores;
	vecTeamScores.resize(m_mapLifeCounter.size(), 0);
	for (BattleArena::TEAM_MAP::const_iterator it = m_mapTeams.begin();
		it != m_mapTeams.end();
		++it, ++i)
	{
		vecTeamScores[i] = m_mapLifeCounter[it->first];
	}

	// 데스매치용 점수 패킷 날리기
	MCommand* pNewCmd = MakeNewCommand(MC_BATTLEARENA_SCOREBOARD_DEATHMATCH, 5, 
		NEW_FLOAT(GConst::ARENA_SCOREBOARD_COUNT),
		NEW_UID(uidWinner),
		NEW_INT(vecTeamScores[0]),
		NEW_INT(vecTeamScores[1]),
		NEW_FLOAT(m_fElapsedTime)
		);
	RouteToAllTeam(pNewCmd);
}

void GGameRule_DeathMatch::OnRejoin( GBattleArena::Status nGameStatus, GEntityPlayer* pPlayer )
{
	VALID(pPlayer);

	// 게임 진행중이면 재접속 패킷을 날려줌
	if (nGameStatus == GBattleArena::PLAYING)
	{
		VALID(m_mapTeams.find(pPlayer->GetPartyUID()) != m_mapTeams.end());
		VALID(m_mapTeams.size() >= 2);
		VALID(m_mapLifeCounter.size() >= 2);
		VALID(m_mapTeams.size() == m_mapLifeCounter.size());

		// 엔트리 점수 구조체 만들기
		size_t i=0; 		
		vector<MUID> vecTeamUIDs;
		vector<int> vecTeamScores;
		vecTeamScores.resize(m_mapLifeCounter.size(), 0);
		for (BattleArena::TEAM_MAP::const_iterator it = m_mapTeams.begin();
			it != m_mapTeams.end();
			++it, ++i)
		{
			vecTeamUIDs.push_back(it->first);
			vecTeamScores[i] = m_mapLifeCounter[it->first];
		}

		// 데스매치용 재접속 패킷 날리기
		MCommand* pNewCmd = MakeNewCommand(MC_BATTLEARENA_REJOIN_DEATHMATCH, 6, 
			NEW_FLOAT(m_fElapsedTime),
			NEW_INT(m_nMaxLifeCounter),
			NEW_UID(vecTeamUIDs[0]),
			NEW_UID(vecTeamUIDs[1]),
			NEW_INT(vecTeamScores[0]),
			NEW_INT(vecTeamScores[1])
			);
		pPlayer->RouteToMe(pNewCmd);
	}
	// 게임 진행 대기 중이면 규칙 패킷을 날려줌
	else
	{
		MCommand* pNewCmd = MakeNewCommand(MC_BATTLEARENA_READY_DEATHMATCH, 1, 
			NEW_INT(m_nMaxLifeCounter)
			);
		pPlayer->RouteToMe(pNewCmd);
	}
}


void GGameRule_DeathMatch::RouteScore()
{
	VALID(m_mapTeams.size() >= 2);
	VALID(m_mapLifeCounter.size() >= 2);
	VALID(m_mapTeams.size() == m_mapLifeCounter.size());

	// 엔트리 점수 구조체 만들기
	size_t i=0; 
	vector<int> vecTeamScores;
	vecTeamScores.resize(m_mapTeams.size());
	for (BattleArena::TEAM_MAP::const_iterator it = m_mapTeams.begin();
		it != m_mapTeams.end();
		++it, ++i)
	{
		vecTeamScores[i] = m_mapLifeCounter[it->first];
	}


	// 데스매치용 점수 패킷 날리기
	MCommand* pNewCmd = MakeNewCommand(MC_BATTLEARENA_DEATHMATCH_UPDATE, 1, 
										NEW_BLOB(&vecTeamScores[0], sizeof(int), vecTeamScores.size()));
	RouteToAllTeam(pNewCmd);
}

void GGameRule_DeathMatch::RouteDie(const MUID& uidPlayer)
{
	MCommand* pNewCmd = MakeNewCommand(MC_BATTLEARENA_DIE, 2, 
		NEW_UID(uidPlayer),
		NEW_FLOAT(GConst::ARENA_DEATHMATCH_REBIRTH_TIME));
	RouteToAllTeam(pNewCmd);
}

void GGameRule_DeathMatch::AddRebirthTimer( const MUID& uidPlayer, GEntityPlayer* pPlayer )
{
	MRegulator rgrRebirth;
	rgrRebirth.SetTickTime(GConst::ARENA_DEATHMATCH_REBIRTH_TIME);
	rgrRebirth.Start();

	pair<MRegulator,GEntityPlayer*> each_pair;
	each_pair.first = rgrRebirth;
	each_pair.second = pPlayer;
	m_mapRebirthTimers.insert(REBIRTHTIME_MAP::value_type(uidPlayer, each_pair));
}

void GGameRule_DeathMatch::EraseRebirthTimer( const MUID& uidPlayer )
{
	m_mapRebirthTimers.erase(uidPlayer);
}

void GGameRule_DeathMatch::Rebirth( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);
	MUID uidTeam = pPlayer->GetPartyUID();
	VALID(m_mapStaringMarkers.find(uidTeam) != m_mapStaringMarkers.end());

	const MARKER_INFO* pStartingMarker = m_mapStaringMarkers[uidTeam];
	VALID(pStartingMarker);
	gsys.pRebirthSystem->AutoRebirthInBattleArena(pPlayer, pStartingMarker->vPoint, pStartingMarker->vDir);
}

void GGameRule_DeathMatch::RouteKill( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);

	// 죽은 사람
	MUID uidVictim = pPlayer->GetUID();
	

	// 죽인 사람
	MUID uidKiller;	
	GEntityPlayer* pKiller = gmgr.pPlayerObjectManager->GetEntity(pPlayer->GetKiller());
	if (pKiller != NULL)
	{
		uidKiller = pKiller->GetUID();
	}


	// 죽인 기술
	int nKillingTalentID = INVALID_ID;	
	if (pPlayer->GetKillTalent() &&	!CSTalentInfoHelper::IsNormalAttackTalent(pPlayer->GetKillTalent()->m_nID))
	{
		nKillingTalentID = pPlayer->GetKillTalent()->m_nID;
	}


	// 결과 통보
	MCommand* pNewCmd = MakeNewCommand(MC_BATTLEARENA_KILL_INFO, 3,
										NEW_UID(uidKiller),
										NEW_UID(uidVictim),
										NEW_INT(nKillingTalentID)
										);

	RouteToAllTeam(pNewCmd);
}

vec3 GGameRule_DeathMatch::GetSpotPos(GEntityPlayer* pPlayer, const BattleArena::Team& team)
{
	VALID_RET(pPlayer, vec3::ZERO);

	VALID_RET(m_mapStaringMarkers.find(team.UID) != m_mapStaringMarkers.end(), vec3::ZERO);

	return m_mapStaringMarkers[team.UID]->vPoint;
}