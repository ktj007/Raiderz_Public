#include "StdAfx.h"
#include "GBattleArenaMgr.h"
#include "GGlobal.h"
#include "GServer.h"
#include "GSTLUtil.h"
#include "GEntityPlayer.h"
#include "GPlayerBattleArena.h"

GBattleArenaMgr::GBattleArenaMgr(void)
: m_pMatcher(NULL)
{
	CreateDefaultMatcher(); // 임시
}

GBattleArenaMgr::~GBattleArenaMgr(void)
{
	SAFE_DELETE(m_pMatcher);
	SAFE_DELETE_MAP(m_mapBattleArena);
	ClearGameRules();
}

void GBattleArenaMgr::Update( float fDelta )
{
	for (BATTLEARENA_MAP::iterator it = m_mapBattleArena.begin();
		it != m_mapBattleArena.end();)
	{
		GBattleArena* pBattleArena = it->second;
		_ASSERT(pBattleArena);
		if (pBattleArena->Update(fDelta))
		{
			it = m_mapBattleArena.erase(it);

			// 투기장 만료됨
			OnBattleArenaFinished(pBattleArena->GetUID(), pBattleArena);
		}
		else
		{
			++it;
		}
	}
}

void GBattleArenaMgr::CreateDefaultMatcher()
{
	m_pMatcher = new GBattleArenaMatcher();
	m_pMatcher->SetLisener(this);
}

bool GBattleArenaMgr::RegisterTeam( GEntityPlayer* pPlayer )
{
	return GetMatcher().RegisterTeam(pPlayer);
}

bool GBattleArenaMgr::DeregisterTeam( GEntityPlayer* pPlayer )
{
	return GetMatcher().DeregisterTeam(pPlayer);
}

bool GBattleArenaMgr::DeregisterTeam( MUID uidTeam )
{
	return GetMatcher().DeregisterTeam(uidTeam);
}

void GBattleArenaMgr::OnDeleteTeam(MUID uidTeam)
{
	BattleArena::Team team = FindTeam(uidTeam);
	VALID(!team.IsInvalid());

	GBattleArena* pBattleArena = FindBattleArena(team.uidBattleArena);
	if (pBattleArena)
	{
		pBattleArena->OnDeleteTeam(team);
	}
	
	DeregisterTeam(uidTeam);
}

bool GBattleArenaMgr::IsEntryTeam( GEntityPlayer* pPlayer )
{
	return GetMatcher().IsEntryTeam(pPlayer);
}

bool GBattleArenaMgr::IsEntryTeam( MUID uidTeam )
{
	return GetMatcher().IsEntryTeam(uidTeam);
}

size_t GBattleArenaMgr::GetTeamQty() const
{
	return GetMatcher().GetTeamQty();
}

size_t GBattleArenaMgr::GetBattleArenaQty() const
{
	return m_mapBattleArena.size();
}

BattleArena::Team GBattleArenaMgr::FindTeam( MUID uidTeam ) const
{
	return GetMatcher().Lookup(uidTeam);
}

GBattleArena* GBattleArenaMgr::FindBattleArena( MUID uidBattleArena ) const
{
	BATTLEARENA_MAP::const_iterator it = m_mapBattleArena.find(uidBattleArena);
	if (it == m_mapBattleArena.end())
		return NULL;
	return it->second;
}

GBattleArena* GBattleArenaMgr::NewBattleArena(GBattleArena::GameRule* pRule)
{
	return new GBattleArena(gsys.pServer->NewUID(), pRule);
}

void GBattleArenaMgr::CreateBattleArena( BattleArena::Team& teamRequester, BattleArena::Team& teamOpponent )
{
	VALID(!teamRequester.IsInvalid());
	VALID(!teamOpponent.IsInvalid());
	VALID(m_pMatcher);
	VALID(!m_vecGameRules.empty());
	
	GBattleArena* pBattleArena = NewBattleArena(m_vecGameRules.front()->Clone());
	VALID(pBattleArena);
	pBattleArena->RegisterLisener(m_pMatcher);
	VALID(FindBattleArena(pBattleArena->GetUID()) == NULL);
	m_mapBattleArena.insert(std::make_pair(pBattleArena->GetUID(), pBattleArena));

	// 투기장 초기화
	pBattleArena->ChangeStatus(GBattleArena::PREPARE);

	// 투기장에 인트리 입장
	pBattleArena->Join(teamRequester.UID, teamRequester);
	pBattleArena->Join(teamOpponent.UID, teamOpponent);
}

void GBattleArenaMgr::DestroyBattleArena( GBattleArena* pBattleArena )
{
	VALID(pBattleArena);
	VALID(pBattleArena->GetStatus() == GBattleArena::FINISH);
		
	SAFE_DELETE(pBattleArena);
}


void GBattleArenaMgr::OnMatched( BattleArena::Team& teamRequester, BattleArena::Team& teamOpponent )
{
	CreateBattleArena(teamRequester, teamOpponent);
}

void GBattleArenaMgr::OnBattleArenaFinished( MUID uidBattleArena, GBattleArena* pBattleArena )
{
	DestroyBattleArena(pBattleArena);
}

void GBattleArenaMgr::AddGameRule( GBattleArena::GameRule* pRule )
{
	VALID(pRule);
	m_vecGameRules.push_back(pRule);
}

void GBattleArenaMgr::ClearGameRules()
{
	SAFE_DELETE_VECTOR(m_vecGameRules);
}


#include "GMatchRule_SameTeamMember.h"
#include "GGameRule_DeathMatch.h"
#include "GConst.h"

void GBattleArenaMgr::CreateTestRules()
{
	// 매칭 규칙
	GetMatcher().ClearRules();
	GetMatcher().AddRule(new GMatchRule_SameTeamMember());

	// 종료 규칙
	ClearGameRules();
	GGameRule_DeathMatch* pGameRule = new GGameRule_DeathMatch();
	pGameRule->SetMaxLifeCounter(GConst::ARENA_DEATHMATCH_MAX_SCORE);
	AddGameRule(pGameRule);
}

void GBattleArenaMgr::TryRejoin( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);
	MUID uidBattleArena = pPlayer->GetPlayerBattleArena().GetBattleArenaUID();
	GBattleArena* pBattleArena = FindBattleArena(uidBattleArena);
	if (!pBattleArena)
		return;

	pBattleArena->OnRejoin(pPlayer);
}

bool GBattleArenaMgr::IsBattleArenaOpponent(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2) const
{
	VALID_RET(pPlayer1, false);
	VALID_RET(pPlayer2, false);

	// 같은 투기장에 있는 멤버인가?
	if (pPlayer1->GetPlayerBattleArena().GetBattleArenaUID() !=
		pPlayer2->GetPlayerBattleArena().GetBattleArenaUID())
	{
		return false;
	}

	// 게임 진행중인가?
	if (pPlayer1->GetPlayerBattleArena().GetStatus() != GBattleArena::PLAYING)
		return false;

	// 같은 파티인가?
	if (pPlayer1->GetPartyUID() == pPlayer2->GetPartyUID())
	{
		return false;
	}

	return true;
}

vec3 GBattleArenaMgr::GetSpotPos(GEntityPlayer* pPlayer) const
{
	VALID_RET(pPlayer, vec3::ZERO);

	MUID uidBattleArena = pPlayer->GetPlayerBattleArena().GetBattleArenaUID();
	GBattleArena* pBattleArena = FindBattleArena(uidBattleArena);
	if (!pBattleArena)
		return vec3::ZERO;

	BattleArena::Team team = FindTeam(pPlayer->GetPartyUID());

	return pBattleArena->GetSpotPos(pPlayer, team);
}