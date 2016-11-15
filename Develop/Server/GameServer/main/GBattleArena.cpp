#include "StdAfx.h"
#include "GBattleArena.h"
#include "GDef.h"
#include "GEntityPlayer.h"
#include "GPlayerBattleArena.h"
#include "GGlobal.h"
#include "GPartySystem.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GFieldInfoMgr.h"
#include "GFieldMgr.h"
#include "GDynamicFieldMaster.h"
#include "GConst.h"
#include "GSTLUtil.h"
#include "GRebirthSystem.h"
#include "GDynamicFieldPolicy_BattleArena.h"
#include "GGlobal.h"
#include "GPlayerFieldDynamic.h"
#include "GPlayerObjectManager.h"


#define NOTIFY_ARENA_EVENT(obs_type, set, event)		\
	for each (obs_type* pOvr in set)					\
		pOvr->event;									\


GBattleArena::GBattleArena(MUID uid, GameRule* pRule)
: m_UID(uid)
, m_nStatus(INVALID)
, m_pGameRule(pRule)
{
}

GBattleArena::~GBattleArena(void)
{
	SAFE_DELETE(m_pGameRule);
}


void GBattleArena::RegisterLisener( Lisener* pLisener )
{
	VALID(pLisener);
	m_vecLiseners.push_back(pLisener);
}

void GBattleArena::Join( MUID uidTeam, BattleArena::Team& team )
{
	VALID(team.uidParty.IsValid());
	VALID(m_mapEntries.find(uidTeam) == m_mapEntries.end());
	VALID(!team.IsInvalid());

	m_mapEntries.insert(std::make_pair(uidTeam, team));

	// 투기장 UID 설정
	SetBattleArenaUIDToTeam(team);

	OnJoin(team);
}

GBattleArena::Status GBattleArena::GetStatus() const
{
	return m_nStatus;
}

void GBattleArena::ChangeStatus( Status nStatus )
{
	Status nOldStatus = m_nStatus;
	m_nStatus = nStatus;

	switch (nOldStatus)
	{
	case PREPARE:		OnPrepareLeave();		break;
	case COUNTING:		OnCountingLeave();		break;
	case PLAYING:		OnPlayingLeave();		break;
	case SCOREBOARD:	OnScoreBoardLeave();	break;
	//case FINISH:		OnFinishLeave();		break;
	}

	switch (m_nStatus)
	{
	case PREPARE:		OnPrepareEnter();		break;
	case COUNTING:		OnCountingEnter();		break;
	case PLAYING:		OnPlayingEnter();		break;
	case SCOREBOARD:	OnScoreBoardEnter();	break;
	case FINISH:		OnFinishEnter();		break;
	}
}

bool GBattleArena::Update( float fDelta )
{
	// 규칙 업데이트
	if (GetStatus() == PLAYING)
	{
		UpdateGameRules(fDelta);
	}
	
	switch (m_nStatus)
	{
	case PREPARE:		OnPrepareUpdate(fDelta);	break;
	case COUNTING:		OnCountingUpdate(fDelta);	break;
	case PLAYING:		OnPlayingUpdate(fDelta);	break;
	case SCOREBOARD:	OnScoreBoardUpdate(fDelta);	break;
	case FINISH:		OnFinishUpdate(fDelta);		break;
	}

	if (m_nStatus == FINISH)
	{
		OnFinishLeave();
		return true;
	}

	return false;
}

void GBattleArena::OnJoin( const BattleArena::Team& team )
{
	if (m_nStatus != PREPARE)
		return;	// 참가자 대기 상태에만 참가 가능

	// 참가자가 둘 이상이면 게임 시작
	if (m_mapEntries.size() >= 2)
	{
		ChangeStatus(COUNTING);
	}
}

void GBattleArena::OnPrepareEnter()
{
	
}

void GBattleArena::OnCountingEnter()
{
	// 시작 마커 설정
	ExtractStartingMarkers();

	// 동적 필드를 생성하고, 참여자들 참가시키기
	MoveAllTeamToTrialField();

	// 팀 상대 변경
	for (BattleArena::TEAM_MAP::iterator it = m_mapEntries.begin();
		it != m_mapEntries.end();
		++it)
	{
		it->second.nStatus = BattleArena::Team::PLAYING;
	}

	// 종료 조건 초기화
	InitGameRules();

	m_rgrStartingCounter.SetTickTime(GConst::ARENA_GAME_COUNT);
	m_rgrStartingCounter.Start();

	// 게임 카운트 패킷을 전송
	RouteToAllTeam(MakeNewCommand(MC_BATTLEARENA_COUNTING, 1, NEW_FLOAT(GConst::ARENA_GAME_COUNT)));
}

void GBattleArena::OnPlayingEnter()
{
	VALID(!m_mapEntries.empty());
	
	// 엔트리 UID 구조체 만들기
	size_t i=0; 
	vector<MUID> vecTeamUIDs;	
	vecTeamUIDs.resize(m_mapEntries.size());
	for (BattleArena::TEAM_MAP::iterator it = m_mapEntries.begin();
		it != m_mapEntries.end();
		++it, ++i)
	{
		vecTeamUIDs[i] = it->first;
	}

	// 게임 시작 패킷을 전송	
	RouteToAllTeam(MakeNewCommand(MC_BATTLEARENA_GAMESTART, 1, NEW_BLOB(&vecTeamUIDs[0], sizeof(MUID), vecTeamUIDs.size())));
}

void GBattleArena::OnScoreBoardEnter()
{
	// 이벤트 전송
	for each (Lisener* each in m_vecLiseners)
	{
		each->OnBattleArenaShowScoreBoard(m_mapEntries); 
	}

	m_rgrScoreBoardCounter.SetTickTime(GConst::ARENA_SCOREBOARD_COUNT);
	m_rgrScoreBoardCounter.Start();
}

void GBattleArena::OnFinishEnter()
{
	// 게임 피니쉬 패킷을 전송
	RouteToAllTeam(MakeNewCommand(MC_BATTLEARENA_FINISHED, 0, NULL));
	// 투기장 UID 제거
	ClearBattleArenaUIDToTeam();
}

void GBattleArena::OnPrepareLeave()
{

}

void GBattleArena::OnCountingLeave()
{

}

void GBattleArena::OnPlayingLeave()
{
}

void GBattleArena::OnScoreBoardLeave()
{

}

void GBattleArena::OnFinishLeave()
{
	// 엔트리 멤버 모두 부활
	RebirthToAllTeam();
	// 엔트리 멤버 모두 회복
	FullHealToAllTeam();
	// 참가자들 원래 필드로 되돌려보내기
	MoveAllTeamToEnterPos();

	// 이벤트 전송
	for each (Lisener* each in m_vecLiseners)
	{
		each->OnBattleArenaComplete(m_mapEntries); 
	}
}

void GBattleArena::OnPrepareUpdate(float fDelta)
{
}

void GBattleArena::OnCountingUpdate(float fDelta)
{
	if (!m_rgrStartingCounter.IsReady(fDelta))
		return;

	ChangeStatus(PLAYING);
}

void GBattleArena::OnPlayingUpdate(float fDelta)
{
}

void GBattleArena::OnScoreBoardUpdate( float fDelta )
{
	if (!m_rgrScoreBoardCounter.IsReady(fDelta))
		return;

	ChangeStatus(FINISH);
}

void GBattleArena::OnFinishUpdate( float fDelta )
{

}
const BattleArena::Team* GBattleArena::FindTeam(MUID uidTeam) const
{
	BattleArena::TEAM_MAP::const_iterator it = m_mapEntries.find(uidTeam);
	if (it == m_mapEntries.end())
		return NULL;

	return &(it->second);
}

void GBattleArena::InitGameRules()
{
	m_pGameRule->Init(m_vecStaringMarkers, m_mapEntries);
}

bool GBattleArena::CheckGameRules(MUID& uidWinner)
{
	return m_pGameRule->IsFinished(m_mapEntries, uidWinner);
}

void GBattleArena::UpdateGameRules( float fDelta )
{
	m_pGameRule->Update(fDelta);
}

void GBattleArena::OnTeamMemberDie( GEntityPlayer* pPlayer, const BattleArena::Team& team )
{
	if (GetStatus() != PLAYING)
		return;	// 게임중일때만 유효

	m_pGameRule->OnTeamMemberDie(pPlayer, team);
	ProcessFinish();
}

void GBattleArena::OnRebirthTeamMemberBySelf( GEntityPlayer* pPlayer, const BattleArena::Team& team )
{
	if (GetStatus() != PLAYING)
		return;	// 게임중일때만 유효

	m_pGameRule->OnRebirthTeamMemberBySelf(pPlayer, team);
	ProcessFinish();
}

void GBattleArena::OnRebirthTeamMemberByOther( GEntityPlayer* pPlayer, const BattleArena::Team& team )
{
	if (GetStatus() != PLAYING)
		return;	// 게임중일때만 유효

	m_pGameRule->OnRebirthTeamMemberByOther(pPlayer, team);
	ProcessFinish();
}

void GBattleArena::OnDisconnect( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);

	m_pGameRule->OnDisconnect(pPlayer);
	ProcessFinish();
}

void GBattleArena::OnDeleteTeam( const BattleArena::Team& team )
{
	m_mapEntries.erase(team.UID);

	m_pGameRule->OnDeleteTeam(team);
	ProcessFinish();
}

void GBattleArena::OnRejoin( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);

	GBattleArena::Status nStatus = GetStatus();

	// 게임 규칙에 맞는 정보 전송
	m_pGameRule->OnRejoin(nStatus, pPlayer);
	ProcessFinish();

	// 투기장 공용 정보 전송
	switch(nStatus)
	{
	case COUNTING:	
		{
			// 남은시간 만큼의 게임 카운트 패킷을 전송
			pPlayer->RouteToMe(MakeNewCommand(MC_BATTLEARENA_COUNTING, 1, NEW_FLOAT(GConst::ARENA_GAME_COUNT-m_rgrStartingCounter.GetElapsedTime())));
		} break;	
	case PREPARE:		{} break;	// 준비중일때는 무시 (다른 플레이어도 진입중이므로)
	case PLAYING:		{} break;	// 진행중일때는 별다른 처리 없음 (규칙에서 대부분 처리)
	case SCOREBOARD:	{} break;	// 점수판 보는 중에는 진입 불가
	case FINISH:		{} break;	// 종료되는 중에는 진입 불가
	}
}

vec3 GBattleArena::GetSpotPos( GEntityPlayer* pPlayer, const BattleArena::Team& team )
{
	VALID_RET(pPlayer, vec3::ZERO);

	return m_pGameRule->GetSpotPos(pPlayer, team);
}

void GBattleArena::RouteToTeam( const BattleArena::Team& team, MCommand* pCommand )
{
	VALID(pCommand);
	GetPartySys()->RouteToAllMembers(team.UID, pCommand);
}

void GBattleArena::RouteToAllTeam( MCommand* pCommand )
{
	VALID(pCommand);
	for (BattleArena::TEAM_MAP::iterator it = m_mapEntries.begin();
		it != m_mapEntries.end();
		++it)
	{
		BattleArena::Team& team = it->second;
		RouteToTeam(team, pCommand->Clone());
	}
	SAFE_DELETE(pCommand);
}

bool GBattleArena::MoveAllTeamToTrialField()
{
	VALID_RET(!m_vecStaringMarkers.empty(), false);	// 투기장 필드 없음

	bool bFirstPlayer = true;

	int nCount = 0;
	for (BattleArena::TEAM_MAP::iterator it = m_mapEntries.begin();
		it != m_mapEntries.end();
		++it)
	{
		BattleArena::Team& team = it->second;
		_ASSERT(team.uidParty.IsValid());

		GParty* pParty = gsys.pPartySystem->FindParty(team.uidParty);
		_ASSERT(pParty != NULL);
		if (pParty == NULL)	continue;

		for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
		{
			MUID uidMember = it->first;

			GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidMember);
			if (pPlayer == NULL)	continue;


			const MARKER_INFO* pStartingMarker = m_vecStaringMarkers[nCount];

			GATE_INFO_DYNAMIC sGateInfo(DFT_BATTLEARENA, GConst::ARENA_FIELDGROUP_ID, GConst::ARENA_FIELD_ID, pStartingMarker->vPoint, pStartingMarker->vDir);
			pPlayer->GetPlayerField().GetFieldDynamic().GateRequest(&sGateInfo);
			
			if (bFirstPlayer)
			{
				bFirstPlayer = false;

				MUID uidCreatedBattleArenaField = pPlayer->GetPlayerField().GetDynamicFieldGroupUID();

				GDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(uidCreatedBattleArenaField);
				if (NULL == pDynamicFieldMaster) return false;

				GDynamicFieldPolicy_BattleArena* pBattleArenaPolicy = static_cast<GDynamicFieldPolicy_BattleArena*>(pDynamicFieldMaster->GetPolicy());
				RegisterLisener(pBattleArenaPolicy);

				SetFieldGroupUID(uidCreatedBattleArenaField);
			}
		}

		++nCount;
	}

	return true;
}

bool GBattleArena::MoveAllTeamToEnterPos()
{
	// 시작 필드 위치로 이동
	for (BattleArena::TEAM_MAP::iterator it = m_mapEntries.begin();
		it != m_mapEntries.end();
		++it)
	{
		BattleArena::Team& team = it->second;
		_ASSERT(team.uidParty.IsValid());

		GParty* pParty = gsys.pPartySystem->FindParty(team.uidParty);
		_ASSERT(pParty != NULL);
		if (pParty == NULL)	continue;

		for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
		{
			MUID uidMember = it->first;
			GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidMember);
			if (pPlayer == NULL)	continue;

			pPlayer->GetPlayerField().GetFieldDynamic().GateToEnterPos(pPlayer->GetFieldID());
		}
	}

	return true;
}

void GBattleArena::RebirthToAllTeam()
{
	// 시작 필드 위치로 이동
	for (BattleArena::TEAM_MAP::iterator it = m_mapEntries.begin();
		it != m_mapEntries.end();
		++it)
	{
		BattleArena::Team& team = it->second;
		_ASSERT(team.uidParty.IsValid());
		
		GParty* pParty = gsys.pPartySystem->FindParty(team.uidParty);
		_ASSERT(pParty != NULL);
		if (pParty == NULL)	continue;

		for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
		{
			MUID uidMember = it->first;
			GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidMember);
			if (pPlayer == NULL)	continue;
		
			gsys.pRebirthSystem->AutoRebirthInBattleArena(pPlayer, pPlayer->GetPos(), pPlayer->GetDir());
		}
	}
}

void GBattleArena::FullHealToAllTeam()
{
	// 시작 필드 위치로 이동
	for (BattleArena::TEAM_MAP::iterator it = m_mapEntries.begin();
		it != m_mapEntries.end();
		++it)
	{
		BattleArena::Team& team = it->second;
		_ASSERT(team.uidParty.IsValid());

		GParty* pParty = gsys.pPartySystem->FindParty(team.uidParty);
		_ASSERT(pParty != NULL);
		if (pParty == NULL)	continue;

		for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
		{
			MUID uidMember = it->first;
			GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidMember);
			if (pPlayer == NULL)	continue;

			pPlayer->SetHP(pPlayer->GetMaxHP());
			pPlayer->SetEN(pPlayer->GetMaxEN());
			pPlayer->SetSTA(pPlayer->GetMaxSTA());
			pPlayer->RouteSimpleStatus(pPlayer->GetUID()); // 자기 자신에게 정보를 알려줌
		}
	}
}

void GBattleArena::SetBattleArenaUIDToTeam( BattleArena::Team &team )
{
	VALID(team.uidParty.IsValid());

	team.uidBattleArena = GetUID();

	GParty* pParty = gsys.pPartySystem->FindParty(team.uidParty);
	VALID(pParty != NULL);

	for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
	{
		MUID uidMember = it->first;
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidMember);
		if (pPlayer == NULL)	continue;

		pPlayer->GetPlayerBattleArena().SetBattleArenaUID(GetUID());
	}
}

void GBattleArena::ClearBattleArenaUIDToTeam()
{
	for (BattleArena::TEAM_MAP::iterator it = m_mapEntries.begin();
		it != m_mapEntries.end();
		++it)
	{
		BattleArena::Team& team = it->second;
		_ASSERT(team.uidParty.IsValid());
		
		team.uidBattleArena.SetInvalid();

		GParty* pParty = gsys.pPartySystem->FindParty(team.uidParty);
		_ASSERT(pParty != NULL);
		if (pParty == NULL)	continue;

		for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
		{
			MUID uidMember = it->first;
			GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidMember);
			if (pPlayer == NULL)	continue;

			pPlayer->GetPlayerBattleArena().SetBattleArenaUID(MUID::Invalid());
		}		
	}
}

void GBattleArena::ExtractStartingMarkers()
{
	m_vecStaringMarkers.clear();
	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(GConst::ARENA_FIELD_ID);
	VALID(pFieldInfo);
	for (size_t i=0; i<m_mapEntries.size(); ++i)
	{
		const MARKER_INFO* pMarker = pFieldInfo->FindMarker(i+1);
		VALID(pMarker);
		m_vecStaringMarkers.push_back(pMarker);
	}
}

void GBattleArena::ProcessFinish()
{
	MUID uidWinner;
	if (CheckGameRules(uidWinner))
	{
		m_uidWinner = uidWinner;
		ChangeStatus(SCOREBOARD);
	}
}

bool GBattleArena::IsGamePreparing() const
{
	return (GetStatus() == PREPARE) || (GetStatus() == COUNTING);
}

bool GBattleArena::IsGamePlaying() const
{
	return (GetStatus() == PLAYING);
}

bool GBattleArena::IsGameFinishing() const
{
	return (GetStatus() == SCOREBOARD) || (GetStatus() == FINISH);
}