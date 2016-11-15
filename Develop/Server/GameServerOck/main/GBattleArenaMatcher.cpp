#include "StdAfx.h"
#include "GBattleArenaMatcher.h"
#include "GDef.h"
#include "GEntityPlayer.h"
#include "GSTLUtil.h"
#include "GGlobal.h"
#include "GPartySystem.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "CCommandResultTable.h"

GBattleArenaMatcher::GBattleArenaMatcher(void)
: m_pLisener(NULL)
{
}

GBattleArenaMatcher::~GBattleArenaMatcher(void)
{
	ClearRules();
}

bool GBattleArenaMatcher::RegisterTeam( GEntityPlayer* pPlayer )
{
	VALID_RET(pPlayer, false);

	if (IsEntryTeam(pPlayer))
	{
		pPlayer->FailAndRouteSystemMsg(CR_FAIL_BATTLEARENA_ALREADY_REGISTER);
		return false;	// 이미 참가되어 있음
	}

	if (!AddTeam(pPlayer))
		return false;

	OnRegisterTeam(m_mapTeams[pPlayer->GetPartyUID()]);	// call by reference를 위해서...

	return true;
}

bool GBattleArenaMatcher::DeregisterTeam( GEntityPlayer* pPlayer )
{
	VALID_RET(pPlayer, false);

	if (!gsys.pPartySystem->IsPartyMember(pPlayer))
	{
		pPlayer->FailAndRouteSystemMsg(CR_FAIL_BATTLEARENA_NEED_PARTY);
		return false;	// 파티가 없음
	}

	if (!IsEntryTeam(pPlayer))
	{
		pPlayer->FailAndRouteSystemMsg(CR_FAIL_BATTLEARENA_NOT_REGISTER);
		return false;	// 등록되어 있지 않음
	}

	MUID uidParty = pPlayer->GetPartyUID();
	BattleArena::TEAM_MAP::iterator it = m_mapTeams.find(uidParty);	
	BattleArena::Team team = it->second;

	if (team.nStatus != BattleArena::Team::WAITING_FOR_MATCH)
	{
		pPlayer->FailAndRouteSystemMsg(CR_FAIL_BATTLEARENA_ALREADY_PLAYING);
		return false;	// 대전중인 참가그룹
	}

	OnDeregisterTeam(m_mapTeams[team.UID]);	// call by reference를 위해서...

	// 참가자 등록취소
	DeleteTeam(team);

	return true;
}

bool GBattleArenaMatcher::DeregisterTeam( MUID uidTeam )
{
	if (!IsEntryTeam(uidTeam))
	{
		return false;	// 등록되어 있지 않음
	}

	BattleArena::TEAM_MAP::iterator it = m_mapTeams.find(uidTeam);	
	BattleArena::Team team = it->second;

	OnDeregisterTeam(m_mapTeams[team.UID]);	// call by reference를 위해서...

	// 참가자 등록취소
	DeleteTeam(team);

	return true;
}

bool GBattleArenaMatcher::IsEntryTeam( GEntityPlayer* pPlayer )
{
	VALID_RET(pPlayer, false);

	// 파티가 없음
	if (!gsys.pPartySystem->IsPartyMember(pPlayer))
	{
		return false;
	}

	return IsEntryTeam(pPlayer->GetPartyUID());
}

bool GBattleArenaMatcher::IsEntryTeam( MUID uidTeam )
{
	BattleArena::TEAM_MAP::iterator it = m_mapTeams.find(uidTeam);
	if (it == m_mapTeams.end())
		return false;

	return true;	
}

BattleArena::Team GBattleArenaMatcher::Lookup(MUID uidTeam) const
{
	BattleArena::TEAM_MAP::const_iterator it = m_mapTeams.find(uidTeam);
	if (it == m_mapTeams.end())
		return BattleArena::Team();	// 파티가 등록자 목록에 없음

	return it->second;
}

void GBattleArenaMatcher::RouteToTeam( const BattleArena::Team& team, MCommand* pCommand )
{
	VALID(pCommand);
	GetPartySys()->RouteToAllMembers(team.UID, pCommand);
}

void GBattleArenaMatcher::AddRule( Rule* pRule )
{
	VALID(pRule); m_vecRules.push_back(pRule);
}

void GBattleArenaMatcher::ClearRules()
{
	SAFE_DELETE_VECTOR(m_vecRules);
}

bool GBattleArenaMatcher::CheckRules( const BattleArena::Team& teamRequester, const BattleArena::Team& teamOpponent )
{
	for each(Rule* each in m_vecRules)
	{
		if (each->IsMatch(teamRequester, teamOpponent))
			return true;
	}
	return false;
}


void GBattleArenaMatcher::OnRegisterTeam( BattleArena::Team& team )
{
	VALID(team.uidParty.IsValid());

	// 대상 엔트리들에게 등록되었음 알림
	RouteRegister(team);

	OnTryMatch(team);
}

void GBattleArenaMatcher::OnDeregisterTeam( BattleArena::Team& team )
{
	VALID(team.uidParty.IsValid());

	// 대상 엔트리들에게 등록 취소되었음 알림
	RouteDeregister(team);
}


void GBattleArenaMatcher::OnTryMatch( BattleArena::Team& team )
{
	if (m_mapTeams.size() < 2)
		return;

	VALID(!m_vecRules.empty());

	for (BattleArena::TEAM_MAP::iterator it = m_mapTeams.begin();
		 it != m_mapTeams.end();
		 ++it)
	{
		BattleArena::Team& teamOpp = it->second;
		if (team.UID == teamOpp.UID)
			continue;	// 요청자면 무시

		if (teamOpp.IsMatched())
			continue;	// 진행중인 팀이면 무시

		if (!CheckRules(team, teamOpp))
			continue;	// 매칭 대상자가 아님

		// 매칭 성공
		OnMatched(team, teamOpp);

		break;
	}
}

void GBattleArenaMatcher::OnMatched( BattleArena::Team& teamRequester, BattleArena::Team& teamOpponent )
{
	// 대상 엔트리들에게 매칭이 되었음을 알림
	MCommand* pNewCmd = MakeNewCommand(MC_BATTLEARENA_MATCHED, 0, NULL);
	RouteToTeam(teamRequester, pNewCmd->Clone());
	RouteToTeam(teamOpponent, pNewCmd->Clone());
	SAFE_DELETE(pNewCmd);
	
	// 엔트리들 상태를 변경
	teamRequester.nStatus = BattleArena::Team::WAITING_FOR_PLAY;
	teamOpponent.nStatus = BattleArena::Team::WAITING_FOR_PLAY;

	// 콜백
	if (m_pLisener)
		m_pLisener->OnMatched(teamRequester, teamOpponent);
}

void GBattleArenaMatcher::OnBattleArenaComplete( const BattleArena::TEAM_MAP& mapTeams )
{
	for (BattleArena::TEAM_MAP::const_iterator it = mapTeams.begin();
		it != mapTeams.end();
		++it)
	{
		const BattleArena::Team& team = it->second;
		// 참가자 등록취소
		DeleteTeam(team);
	}
}

void GBattleArenaMatcher::RouteRegister( BattleArena::Team& team )
{
	MCommand* pNewCmd = MakeNewCommand(MC_BATTLEARENA_REGISTER, 1,
		NEW_INT(BattleArena::GAMETYPE_DEATHMATCH));	
	RouteToTeam(team, pNewCmd);
}

void GBattleArenaMatcher::RouteDeregister( BattleArena::Team& team )
{
	MCommand* pNewCmd = MakeNewCommand(MC_BATTLEARENA_DEREGISTER, 0, NULL);
	RouteToTeam(team, pNewCmd);
}

bool GBattleArenaMatcher::AddTeam( GEntityPlayer* pPlayer )
{
	if (!gsys.pPartySystem->IsPartyMember(pPlayer))
	{
		pPlayer->FailAndRouteSystemMsg(CR_FAIL_BATTLEARENA_NEED_PARTY);
		return false;	// 파티가 없음
	}

	// 참가자 등록
	MUID uidParty = pPlayer->GetPartyUID();

	BattleArena::Team team;
	team.UID = uidParty;
	team.nStatus = BattleArena::Team::WAITING_FOR_MATCH;
	team.uidParty = uidParty;
	m_mapTeams.insert(std::make_pair(team.UID, team));

	return true;
}

void GBattleArenaMatcher::DeleteTeam( const BattleArena::Team &team )
{
	m_mapTeams.erase(team.UID);
}

bool GBattleArenaMatcher::IsEnemy(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2)
{
	VALID_RET(pPlayer1, false);
	VALID_RET(pPlayer2, false);

	if (!gsys.pPartySystem->IsPartyMember(pPlayer1))		return false;
	if (!gsys.pPartySystem->IsPartyMember(pPlayer2))		return false;

	BattleArena::TEAM_MAP::iterator it1 = m_mapTeams.find(pPlayer1->GetPartyUID());
	if (it1 == m_mapTeams.end())
		return false;

	BattleArena::TEAM_MAP::iterator it2 = m_mapTeams.find(pPlayer2->GetPartyUID());
	if (it2 == m_mapTeams.end())
		return false;

	return (*it1).second.UID != (*it2).second.UID;
}
