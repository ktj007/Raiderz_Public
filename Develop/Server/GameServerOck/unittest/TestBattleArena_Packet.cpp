#include "stdafx.h"
#include "GTestForward.h"
#include "GPartySystem.h"
#include "GBattleArenaMgr.h"
#include "GPlayerBattleArena.h"
#include "GMatchRule_SameTeamMember.h"
#include "GGameRule_DeathMatch.h"
#include "GBattleArenaDefine.h"
#include "GGlobal.h"
#include "GServer.h"
#include "GSharedField.h"
#include "GPlayerObjectManager.h"
#include "GParty.h"
#include "GFieldFactory.h"
#include "FBasePlayer.h"
#include "GDynamicFieldMaster.h"

SUITE(BattleArena_Packet)
{
	struct FBattleArena : public FBasePlayer
	{
		FBattleArena()
		{
			// 초기 필드 만들기
			GFieldInfo* pSFieldInfo = new GFieldInfo;
			pSFieldInfo->m_bDynamic = false;
			pSFieldInfo->m_nFieldID = 777;
			pSFieldInfo->m_nDefaultChannelCount = 1;
			pSFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
			gmgr.pFieldInfoMgr->Insert(pSFieldInfo);
			gmgr.pFieldMgr->GetFactory()->CreateSharedFieldMaster(pSFieldInfo);
			m_pField = gmgr.pFieldMgr->GetFactory()->CreateSharedField(gsys.pServer->NewUID(), pSFieldInfo->m_nFieldID, 0);
		
			// 플레이어 생성
			MakePlayer(m_pPlayerBlueLeader, L"BlueLeader", &m_pLinkBlueLeader);
			MakePlayer(m_pPlayerBlueMember1, L"BlueMember1", &m_pLinkBlueMember1);
			MakePlayer(m_pPlayerBlueMember2, L"BlueMember2", &m_pLinkBlueMember2);
			MakePlayer(m_pPlayerRedLeader, L"RedLeader", &m_pLinkRedLeader);
			MakePlayer(m_pPlayerRedMember1, L"RedMember1", &m_pLinkRedMember1);
			MakePlayer(m_pPlayerRedMember2, L"RedMember2", &m_pLinkRedMember2);
			
			// 파티 생성
			MakeParty(m_pPlayerBlueLeader, m_pPlayerBlueMember1, m_pPlayerBlueMember2);
			MakeParty(m_pPlayerRedLeader, m_pPlayerRedMember1, m_pPlayerRedMember2);
			gsys.pServer->FrameCounterClear();

			// 투기장 필드그룹 정보
			FIELD_GROUP_INFO* info = new FIELD_GROUP_INFO;
			info->nID = GConst::ARENA_FIELDGROUP_ID;
			// 투기장 필드 정보
			GFieldInfo* pFieldInfo = new GFieldInfo;
			pFieldInfo->m_bDynamic = true;
			pFieldInfo->m_nFieldID = GConst::ARENA_FIELD_ID;
			pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
			// 시작 마커 정보
			MARKER_INFO markerStarting;
			markerStarting.nID = 1;
			markerStarting.nFieldID = GConst::ARENA_FIELD_ID;
			markerStarting.vPoint = vec3(1000,1000,1000);
			markerStarting.vDir = vec3(0,1,0);
			pFieldInfo->InsertMarkerInfo(markerStarting);	// 1번 시작점
			markerStarting.nID = 2;
			markerStarting.nFieldID = GConst::ARENA_FIELD_ID;
			markerStarting.vPoint = vec3(2000,2000,2000);
			markerStarting.vDir = vec3(0,0,1);
			pFieldInfo->InsertMarkerInfo(markerStarting);	// 2번 시작점
			info->vecFieldInfo.push_back(pFieldInfo);
			gmgr.pFieldInfoMgr->Insert(pFieldInfo);
			gmgr.pFieldInfoMgr->InsertGroup(info);

			gmgr.pBattleArenaMgr->AddGameRule(new GGameRule_DeathMatch);
		}

		~FBattleArena()
		{
			test.network.ClearLinks();
		}

		void MakePlayer(GEntityPlayer*& pPlayer, std::wstring strName, MockLink** pMockLink=NULL)
		{
			pPlayer = NewEntityPlayer(m_pField, vec3(100,100,0));

			if (pMockLink)
			{
				*pMockLink = test.network.NewLink(pPlayer);
				SetIgnoreCommonCommand(*pMockLink);
			}
			swprintf_s(pPlayer->GetPlayerInfo()->szName, strName.c_str());
			vecAllPlayers.push_back(pPlayer);
		}

		void MakeParty(GEntityPlayer* pLeader, GEntityPlayer* pMember, GEntityPlayer* pMember2)
		{
			GParty* pParty = GUTHelper_Party::CreateParty(pLeader);
			GUTHelper_Party::AddMember(pParty, pMember);
			GUTHelper_Party::AddMember(pParty, pMember2);
		}

		void BeginCommandRecord()
		{
			m_pLinkBlueLeader->ResetCommands();
			m_pLinkBlueMember1->ResetCommands();
			m_pLinkBlueMember2->ResetCommands();
			m_pLinkRedLeader->ResetCommands();
			m_pLinkRedMember1->ResetCommands();
			m_pLinkRedMember2->ResetCommands();
		}

		void SetIgnoreCommonCommand(MockLink* pLink)
		{
			pLink->AddIgnoreID(MC_BATTLEARENA_KILL_INFO);
			pLink->AddIgnoreID(MC_FIELD_IN_NPC);	
			pLink->AddIgnoreID(MC_FIELD_OUT_NPC);
			pLink->AddIgnoreID(MC_FIELD_IN_PLAYER);
			pLink->AddIgnoreID(MC_FIELD_OUT_PLAYER);
			pLink->AddIgnoreID(MC_FIELD_SECTOR_ENTITY_INFO);
			pLink->AddIgnoreID(MC_TRIGGER_UPDATE_USABLE_SENSOR);
			pLink->AddIgnoreID(MC_ACTION_USE_TALENT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT);
			pLink->AddIgnoreID(MC_CHAR_ENEMY_INFO);
			pLink->AddIgnoreID(MC_NPC_CHANGE_COMBAT_TARGET);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT_NO_MF);
			pLink->AddIgnoreID(MC_FIELD_SET_TIME);
			pLink->AddIgnoreID(MC_CHAR_ENEMY_INFO_CLEAR);
			pLink->AddIgnoreID(MC_NPCINTERACTION_END);
			pLink->AddIgnoreID(MC_NPCINTERACTION_ICON);
			pLink->AddIgnoreID(MC_PARTY_REFRESH_INFO_ALL);
			pLink->AddIgnoreID(MC_PARTY_REFRESH_INFO);
			pLink->AddIgnoreID(MC_PARTY_NOTIFY_JOIN);
			pLink->AddIgnoreID(MC_PARTY_INVITE_QUESTION);
			pLink->AddIgnoreID(MC_ACTION_DIE);
			pLink->AddIgnoreID(MC_FIELD_PREPARING_TO_LOAD);
			pLink->AddIgnoreID(MC_CHAR_MYINFO);
			pLink->AddIgnoreID(MC_TIME_SYNCH);
			pLink->AddIgnoreID(MC_MSG_RES);
		}

		void WaitForPlayCount(int nCount=GConst::ARENA_GAME_COUNT)
		{
			for (int i=0; i<nCount; i++)
			{
				gsys.pServer->Update(1.0f);
			}
		}

		void WaitForScoreBoardCount()
		{
			for (int i=0; i<GConst::ARENA_SCOREBOARD_COUNT; i++)
			{
				gsys.pServer->Update(1.0f);
			}
		}
		void SetMatchRule_SameEntryMember()
		{
			gmgr.pBattleArenaMgr->GetMatcher().ClearRules();
			gmgr.pBattleArenaMgr->GetMatcher().AddRule(new GMatchRule_SameTeamMember());
		}

		void SetFinishRule_DeathMatch(int nMaxLifeCounter = 1)
		{
			gmgr.pBattleArenaMgr->ClearGameRules();
			GGameRule_DeathMatch* pFinishRule = new GGameRule_DeathMatch();
			pFinishRule->SetMaxLifeCounter(nMaxLifeCounter);
			gmgr.pBattleArenaMgr->AddGameRule(pFinishRule);
		}

		void CheckRegister(MockLink* m_pLink, int nExpectedGameType)
		{
			const minet::MCommand& Command = m_pLink->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_BATTLEARENA_REGISTER);
			if (Command.GetParameterCount() < 1)						 { CHECK(false); return; }
			int nGameType;
			if (!Command.GetParameter(&nGameType,	0, MPT_INT))		{ CHECK(false); return; }
			CHECK_EQUAL(nGameType,	nExpectedGameType);

			m_pLink->PopFrontCommand();
		}

		void CheckDeregister(MockLink* m_pLink)
		{
			const minet::MCommand& Command = m_pLink->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_BATTLEARENA_DEREGISTER);

			m_pLink->PopFrontCommand();
		}

		void CheckMatched(MockLink* m_pLink)
		{
			const minet::MCommand& Command = m_pLink->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_BATTLEARENA_MATCHED);

			m_pLink->PopFrontCommand();
		}

		void CheckGameCounting(MockLink* m_pLink, float fExpectedCount)
		{
			const minet::MCommand& Command = m_pLink->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_BATTLEARENA_COUNTING);

			if (Command.GetParameterCount() < 1)						 { CHECK(false); return; }
			float fCount;
			if (!Command.GetParameter(&fCount,	0, MPT_FLOAT))			{ CHECK(false); return; }
			CHECK_EQUAL(fCount,	fExpectedCount);

			m_pLink->PopFrontCommand();
		}

		void CheckGameStart(MockLink* m_pLink)
		{
			const minet::MCommand& Command = m_pLink->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_BATTLEARENA_GAMESTART);

			m_pLink->PopFrontCommand();
		}

		void CheckChangeField(MockLink* m_pLink)
		{
			const minet::MCommand& Command = m_pLink->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_FIELD_CHANGE_FIELD);

			m_pLink->PopFrontCommand();
		}

		void CheckReadyForDeathmatch(MockLink* m_pLink, int nExpectedLife)
		{
			const minet::MCommand& Command = m_pLink->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_BATTLEARENA_READY_DEATHMATCH);

			if (Command.GetParameterCount() < 1)						 { CHECK(false); return; }
			int nLife;
			if (!Command.GetParameter(&nLife,	0, MPT_INT))			{ CHECK(false); return; }
			CHECK_EQUAL(nLife,	nExpectedLife);

			m_pLink->PopFrontCommand();
		}

		void CheckDeathmatchUpdate(MockLink* m_pLink)
		{
			const minet::MCommand& Command = m_pLink->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_BATTLEARENA_DEATHMATCH_UPDATE);

			m_pLink->PopFrontCommand();
		}

		void CheckScoreboardForDeathmatch(MockLink* m_pLink)
		{
			const minet::MCommand& Command = m_pLink->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_BATTLEARENA_SCOREBOARD_DEATHMATCH);

			m_pLink->PopFrontCommand();
		}

		void CheckFinished(MockLink* m_pLink)
		{
			const minet::MCommand& Command = m_pLink->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_BATTLEARENA_FINISHED);

			m_pLink->PopFrontCommand();
		}

		void CheckDie(MockLink* m_pLink)
		{
			const minet::MCommand& Command = m_pLink->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_BATTLEARENA_DIE);

			m_pLink->PopFrontCommand();
		}

		GSharedField*	m_pField;
		GEntityPlayer*	m_pPlayerBlueLeader;
		GEntityPlayer*	m_pPlayerBlueMember1;
		GEntityPlayer*	m_pPlayerBlueMember2;
		GEntityPlayer*	m_pPlayerRedLeader;
		GEntityPlayer*	m_pPlayerRedMember1;
		GEntityPlayer*	m_pPlayerRedMember2;
		MockLink*		m_pLinkBlueLeader;
		MockLink*		m_pLinkBlueMember1;
		MockLink*		m_pLinkBlueMember2;
		MockLink*		m_pLinkRedLeader;
		MockLink*		m_pLinkRedMember1;
		MockLink*		m_pLinkRedMember2;
		vector<GEntityPlayer*> vecAllPlayers;

		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_BuffMgr;
		GTestMgrWrapper<GBattleArenaMgr>		BattleArenaMgr;
		GTestMgrWrapper<GPlayerObjectManager>	PlayerObjetMgr;
		DECLWRAPPER_NPCMgr;
	};

	TEST_FIXTURE(FBattleArena, Register)
	{
		SetMatchRule_SameEntryMember();
		BeginCommandRecord();
		CHECK_EQUAL(0, gmgr.pBattleArenaMgr->GetBattleArenaQty());
		CHECK_EQUAL(0, gmgr.pBattleArenaMgr->GetTeamQty());
		m_pLinkBlueLeader->OnRecv(MC_BATTLEARENA_REGISTER_REQ,		0, NULL);
		CheckRegister(m_pLinkBlueLeader, BattleArena::GAMETYPE_DEATHMATCH);
		CheckRegister(m_pLinkBlueMember1, BattleArena::GAMETYPE_DEATHMATCH);
		CHECK_EQUAL(0, gmgr.pBattleArenaMgr->GetBattleArenaQty());
		CHECK_EQUAL(1, gmgr.pBattleArenaMgr->GetTeamQty());
		m_pLinkRedLeader->OnRecv(MC_BATTLEARENA_REGISTER_REQ,		0, NULL);
		CheckRegister(m_pLinkRedLeader, BattleArena::GAMETYPE_DEATHMATCH);
		CheckRegister(m_pLinkRedMember1, BattleArena::GAMETYPE_DEATHMATCH);
		CHECK_EQUAL(1, gmgr.pBattleArenaMgr->GetBattleArenaQty());
		CHECK_EQUAL(2, gmgr.pBattleArenaMgr->GetTeamQty());
	}

	TEST_FIXTURE(FBattleArena, Deregister)
	{
		SetMatchRule_SameEntryMember();
		CHECK_EQUAL(0, gmgr.pBattleArenaMgr->GetBattleArenaQty());
		CHECK_EQUAL(0, gmgr.pBattleArenaMgr->GetTeamQty());
		m_pLinkBlueLeader->OnRecv(MC_BATTLEARENA_REGISTER_REQ,		0, NULL);
		CHECK_EQUAL(0, gmgr.pBattleArenaMgr->GetBattleArenaQty());
		CHECK_EQUAL(1, gmgr.pBattleArenaMgr->GetTeamQty());
		BeginCommandRecord();
		m_pLinkBlueLeader->OnRecv(MC_BATTLEARENA_DEREGISTER_REQ,		0, NULL);
		CHECK_EQUAL(m_pLinkBlueLeader->GetCommand(0).GetID(), MC_BATTLEARENA_DEREGISTER);
		CheckDeregister(m_pLinkBlueLeader);
		CHECK_EQUAL(0, gmgr.pBattleArenaMgr->GetBattleArenaQty());
		CHECK_EQUAL(0, gmgr.pBattleArenaMgr->GetTeamQty());
	}

	TEST_FIXTURE(FBattleArena, GameFlow)
	{
		SetMatchRule_SameEntryMember();
		SetFinishRule_DeathMatch(2);	// 2점 내기
		BeginCommandRecord();
		m_pLinkBlueLeader->OnRecv(MC_BATTLEARENA_REGISTER_REQ,		0, NULL);
		m_pLinkRedLeader->OnRecv(MC_BATTLEARENA_REGISTER_REQ,		0, NULL);
		// 매칭 시스템에 등록됨
		CheckRegister(m_pLinkBlueLeader, BattleArena::GAMETYPE_DEATHMATCH);
		CheckRegister(m_pLinkBlueMember1, BattleArena::GAMETYPE_DEATHMATCH);
		CheckRegister(m_pLinkRedLeader, BattleArena::GAMETYPE_DEATHMATCH);
		CheckRegister(m_pLinkRedMember1, BattleArena::GAMETYPE_DEATHMATCH);
		// 매칭됨
		CheckMatched(m_pLinkBlueLeader);
		CheckMatched(m_pLinkBlueMember1);
		CheckMatched(m_pLinkRedLeader);
		CheckMatched(m_pLinkRedMember1);
		// 게임 카운팅 시작
		CheckChangeField(m_pLinkBlueLeader);
		CheckChangeField(m_pLinkBlueMember1);
		CheckChangeField(m_pLinkRedLeader);
		CheckChangeField(m_pLinkRedMember1);
		RUN_LIST(GEntityPlayer, vecAllPlayers, GetPlayerField().GetFieldEntry().OnEnterFieldComplete());
		CheckReadyForDeathmatch(m_pLinkBlueLeader, 2);
		CheckReadyForDeathmatch(m_pLinkBlueMember1, 2);
		CheckReadyForDeathmatch(m_pLinkRedLeader, 2);
		CheckReadyForDeathmatch(m_pLinkRedMember1, 2);
		CheckGameCounting(m_pLinkBlueLeader, GConst::ARENA_GAME_COUNT);
		CheckGameCounting(m_pLinkBlueMember1, GConst::ARENA_GAME_COUNT);
		CheckGameCounting(m_pLinkRedLeader, GConst::ARENA_GAME_COUNT);
		CheckGameCounting(m_pLinkRedMember1, GConst::ARENA_GAME_COUNT);

		// 게임 시작
		WaitForPlayCount();
		CheckGameStart(m_pLinkBlueLeader);
		CheckGameStart(m_pLinkBlueMember1);
		CheckGameStart(m_pLinkRedLeader);
		CheckGameStart(m_pLinkRedMember1);
		// 1회 죽음
		m_pPlayerRedLeader->doDie();
		CheckDie(m_pLinkBlueLeader);
		CheckDie(m_pLinkBlueMember1);
		CheckDie(m_pLinkRedLeader);
		CheckDie(m_pLinkRedMember1);
		CheckDeathmatchUpdate(m_pLinkBlueLeader);
		CheckDeathmatchUpdate(m_pLinkBlueMember1);
		CheckDeathmatchUpdate(m_pLinkRedLeader);
		CheckDeathmatchUpdate(m_pLinkRedMember1);
		// 2회 죽음
		m_pPlayerRedMember1->doDie();
		CheckDie(m_pLinkBlueLeader);
		CheckDie(m_pLinkBlueMember1);
		CheckDie(m_pLinkRedLeader);
		CheckDie(m_pLinkRedMember1);
		CheckDeathmatchUpdate(m_pLinkBlueLeader);
		CheckDeathmatchUpdate(m_pLinkBlueMember1);
		CheckDeathmatchUpdate(m_pLinkRedLeader);
		CheckDeathmatchUpdate(m_pLinkRedMember1);
		// 게임 종료, 점수판 보기
		CheckScoreboardForDeathmatch(m_pLinkBlueLeader);
		CheckScoreboardForDeathmatch(m_pLinkBlueMember1);
		CheckScoreboardForDeathmatch(m_pLinkRedLeader);
		CheckScoreboardForDeathmatch(m_pLinkRedMember1);
		// 게임 마무리
		WaitForScoreBoardCount();
		CheckFinished(m_pLinkBlueLeader);
		CheckFinished(m_pLinkBlueMember1);
		CheckFinished(m_pLinkRedLeader);
		CheckFinished(m_pLinkRedMember1);

		gmgr.pFieldMgr->Update(0.0f);	// 투기장 필드가 삭제될 틱

		// MC_CHANGE_FILED, HP 갱신, 부활 패킷 남음
		CHECK_EQUAL(4, m_pLinkBlueLeader->GetCommandCount());
		CHECK_EQUAL(4, m_pLinkBlueMember1->GetCommandCount());
		CHECK_EQUAL(4, m_pLinkRedLeader->GetCommandCount());
		CHECK_EQUAL(4, m_pLinkRedMember1->GetCommandCount());
	}

	TEST_FIXTURE(FBattleArena, RejoinInCounting)
	{
		// 게임규칙 추가
		gmgr.pBattleArenaMgr->GetMatcher().ClearRules();
		gmgr.pBattleArenaMgr->GetMatcher().AddRule(new GMatchRule_SameTeamMember());
		gmgr.pBattleArenaMgr->ClearGameRules();
		GGameRule_DeathMatch* pFinishRule = new GGameRule_DeathMatch();
		pFinishRule->SetMaxLifeCounter(2);	// 데스매치 라이프를 1로 설정
		gmgr.pBattleArenaMgr->AddGameRule(pFinishRule);
		CHECK_EQUAL(true, m_pPlayerBlueLeader->GetPlayerBattleArena().RegisterEntry());
		CHECK_EQUAL(true, m_pPlayerRedLeader->GetPlayerBattleArena().RegisterEntry());

		// 새로운 투기장이 생성됨
		GBattleArena* pBattleArena1 = gmgr.pBattleArenaMgr->FindBattleArena(m_pPlayerBlueLeader->GetPlayerBattleArena().GetBattleArenaUID());
		RUN_LIST(GEntityPlayer, vecAllPlayers, GetPlayerField().GetFieldEntry().OnEnterFieldComplete());
		WaitForPlayCount(1);	// 1초 지남

		// 게임중에 접속 종료
		GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(m_pPlayerRedMember2->GetPlayerField().GetDynamicFieldGroupUID());

		GGateInfo rGateInfo = m_pPlayerRedMember2->GetPlayerField().GetGateInfo();
		rGateInfo.nFieldID = m_pPlayerRedMember2->GetFieldID();
		rGateInfo.vPosition = m_pPlayerRedMember2->GetPos();
		m_pPlayerRedMember2->GetPlayerField().SetGateInfo(rGateInfo);

		m_pPlayerRedMember2->Destroy();
		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->GetPlayer(m_pPlayerRedMember2->GetUID());
		TVALID(pPlayerObj);
		gmgr.pPlayerObjectManager->ErasePlayer(pPlayerObj->GetUID());

		// 재접속
		BeginCommandRecord();
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);
		pPlayerObj->OnEndLogin();
		m_pLinkRedMember2->OnRecv(MC_CHAR_GAMESTART,		2, NEW_INT(-1), NEW_VEC(vec3::ZERO));

		CHECK_EQUAL(3, m_pLinkRedMember2->GetCommandCount());

		// 필드 이동 패킷
		{
			const minet::MCommand& Command = m_pLinkRedMember2->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_FIELD_CHANGE_FIELD);
		}

		// 게임규칙 패킷
		{
			const minet::MCommand& Command = m_pLinkRedMember2->GetCommand(1);
			CHECK_EQUAL(Command.GetID(), MC_BATTLEARENA_READY_DEATHMATCH);

			if (Command.GetParameterCount() < 1)						 { CHECK(false); return; }
			int nMaxScore;
			if (!Command.GetParameter(&nMaxScore,	0, MPT_INT))		{ CHECK(false); return; }
			CHECK_EQUAL(2,	nMaxScore);	// 1초 지난 시간
		}
		
		// 카운팅 패킷
		{
			const minet::MCommand& Command = m_pLinkRedMember2->GetCommand(2);
			CHECK_EQUAL(Command.GetID(), MC_BATTLEARENA_COUNTING);

			if (Command.GetParameterCount() < 1)						 { CHECK(false); return; }
			float fWaitCount;
			if (!Command.GetParameter(&fWaitCount,	0, MPT_FLOAT))		{ CHECK(false); return; }
			CHECK_EQUAL(GConst::ARENA_GAME_COUNT-1.0f,	fWaitCount);	// 1초 지난 시간
		}
	}

	TEST_FIXTURE(FBattleArena, RejoinInPlaying)
	{
		// 게임규칙 추가
		gmgr.pBattleArenaMgr->GetMatcher().ClearRules();
		gmgr.pBattleArenaMgr->GetMatcher().AddRule(new GMatchRule_SameTeamMember());
		gmgr.pBattleArenaMgr->ClearGameRules();
		GGameRule_DeathMatch* pFinishRule = new GGameRule_DeathMatch();
		pFinishRule->SetMaxLifeCounter(2);	// 데스매치 라이프를 1로 설정
		gmgr.pBattleArenaMgr->AddGameRule(pFinishRule);
		CHECK_EQUAL(true, m_pPlayerBlueLeader->GetPlayerBattleArena().RegisterEntry());
		CHECK_EQUAL(true, m_pPlayerRedLeader->GetPlayerBattleArena().RegisterEntry());

		// 새로운 투기장이 생성됨
		GBattleArena* pBattleArena1 = gmgr.pBattleArenaMgr->FindBattleArena(m_pPlayerBlueLeader->GetPlayerBattleArena().GetBattleArenaUID());
		RUN_LIST(GEntityPlayer, vecAllPlayers, GetPlayerField().GetFieldEntry().OnEnterFieldComplete());
		WaitForPlayCount();

		// 게임중에 접속 종료
		GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(m_pPlayerRedMember2->GetPlayerField().GetDynamicFieldGroupUID());

		GGateInfo rGateInfo = m_pPlayerRedMember2->GetPlayerField().GetGateInfo();
		rGateInfo.nFieldID = m_pPlayerRedMember2->GetFieldID();
		rGateInfo.vPosition = m_pPlayerRedMember2->GetPos();
		m_pPlayerRedMember2->GetPlayerField().SetGateInfo(rGateInfo);

		m_pPlayerRedMember2->Destroy();
		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->GetPlayer(m_pPlayerRedMember2->GetUID());
		TVALID(pPlayerObj);
		gmgr.pPlayerObjectManager->ErasePlayer(pPlayerObj->GetUID());

		// 재접속
		BeginCommandRecord();
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);
		pPlayerObj->OnEndLogin();
		m_pLinkRedMember2->OnRecv(MC_CHAR_GAMESTART,		2, NEW_INT(-1), NEW_VEC(vec3::ZERO));

		CHECK_EQUAL(2, m_pLinkRedMember2->GetCommandCount());

		// 파티 검사
		MUID uidParty = m_pPlayerRedMember2->GetPartyUID();		
	 	GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
		TVALID(pParty);
		CHECK_EQUAL(3, pParty->GetMemberCount());
		CHECK_EQUAL(3, pParty->GetOnlineMemberCount());


		// 필드 이동 패킷
		{
			const minet::MCommand& Command = m_pLinkRedMember2->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_FIELD_CHANGE_FIELD);

			if (Command.GetParameterCount() < 4)						 { CHECK(false); return; }
			int nFieldID;
			if (!Command.GetParameter(&nFieldID,	0, MPT_INT))		{ CHECK(false); return; }
			CHECK_EQUAL(GConst::ARENA_FIELD_ID,	nFieldID);
		}

		// 재접속 패킷
		{
			const minet::MCommand& Command = m_pLinkRedMember2->GetCommand(1);
			CHECK_EQUAL(Command.GetID(), MC_BATTLEARENA_REJOIN_DEATHMATCH);

			if (Command.GetParameterCount() < 6)						 { CHECK(false); return; }
			float fGameElaspedTime;
			int nMaxLife;
			MUID uidTeam[2];
			int nScore[2];
			if (!Command.GetParameter(&fGameElaspedTime,	0, MPT_FLOAT))	{ CHECK(false); return; }
			if (!Command.GetParameter(&nMaxLife,	1, MPT_INT))			{ CHECK(false); return; }
			if (!Command.GetParameter(&uidTeam[0],	2, MPT_UID))			{ CHECK(false); return; }
			if (!Command.GetParameter(&uidTeam[1],	3, MPT_UID))			{ CHECK(false); return; }
			if (!Command.GetParameter(&nScore[0],	4, MPT_INT))			{ CHECK(false); return; }
			if (!Command.GetParameter(&nScore[1],	5, MPT_INT))			{ CHECK(false); return; }
			CHECK_EQUAL(0.0f,	fGameElaspedTime);
			CHECK_EQUAL(2,	nMaxLife);
			CHECK_EQUAL(m_pPlayerBlueLeader->GetPartyUID(),	uidTeam[0]);
			CHECK_EQUAL(m_pPlayerRedLeader->GetPartyUID(),	uidTeam[1]);
			CHECK_EQUAL(2,	nScore[0]);
			CHECK_EQUAL(2,	nScore[1]);
		}

		m_pLinkRedMember2->ResetCommands();

		// 1회 죽음
		m_pPlayerRedLeader->doDie();
		CheckDie(m_pLinkBlueLeader);
		CheckDie(m_pLinkBlueMember1);
		CheckDie(m_pLinkBlueMember2);
		CheckDie(m_pLinkRedLeader);
		CheckDie(m_pLinkRedMember1);
		CheckDie(m_pLinkRedMember2);
		CheckDeathmatchUpdate(m_pLinkBlueLeader);
		CheckDeathmatchUpdate(m_pLinkBlueMember1);
		CheckDeathmatchUpdate(m_pLinkBlueMember2);
		CheckDeathmatchUpdate(m_pLinkRedLeader);
		CheckDeathmatchUpdate(m_pLinkRedMember1);
		CheckDeathmatchUpdate(m_pLinkRedMember2);
		// 2회 죽음
		m_pPlayerRedMember1->doDie();
		CheckDie(m_pLinkBlueLeader);
		CheckDie(m_pLinkBlueMember1);
		CheckDie(m_pLinkBlueMember2);
		CheckDie(m_pLinkRedLeader);
		CheckDie(m_pLinkRedMember1);
		CheckDie(m_pLinkRedMember2);
		CheckDeathmatchUpdate(m_pLinkBlueLeader);
		CheckDeathmatchUpdate(m_pLinkBlueMember1);
		CheckDeathmatchUpdate(m_pLinkBlueMember2);
		CheckDeathmatchUpdate(m_pLinkRedLeader);
		CheckDeathmatchUpdate(m_pLinkRedMember1);
		CheckDeathmatchUpdate(m_pLinkRedMember2);
		// 게임 종료, 점수판 보기
		CheckScoreboardForDeathmatch(m_pLinkBlueLeader);
		CheckScoreboardForDeathmatch(m_pLinkBlueMember1);
		CheckScoreboardForDeathmatch(m_pLinkBlueMember2);
		CheckScoreboardForDeathmatch(m_pLinkRedLeader);
		CheckScoreboardForDeathmatch(m_pLinkRedMember1);
		CheckScoreboardForDeathmatch(m_pLinkRedMember2);
		// 게임 마무리
		WaitForScoreBoardCount();
		CheckFinished(m_pLinkBlueLeader);
		CheckFinished(m_pLinkBlueMember1);
		CheckFinished(m_pLinkBlueMember2);
		CheckFinished(m_pLinkRedLeader);
		CheckFinished(m_pLinkRedMember1);
		CheckFinished(m_pLinkRedMember2);
	}

	TEST_FIXTURE(FBattleArena, RejoinAfterBrokenField)
	{
		// 게임규칙 추가
		gmgr.pBattleArenaMgr->GetMatcher().ClearRules();
		gmgr.pBattleArenaMgr->GetMatcher().AddRule(new GMatchRule_SameTeamMember());
		gmgr.pBattleArenaMgr->ClearGameRules();
		GGameRule_DeathMatch* pFinishRule = new GGameRule_DeathMatch();
		pFinishRule->SetMaxLifeCounter(2);	// 데스매치 라이프를 1로 설정
		gmgr.pBattleArenaMgr->AddGameRule(pFinishRule);
		CHECK_EQUAL(true, m_pPlayerBlueLeader->GetPlayerBattleArena().RegisterEntry());
		CHECK_EQUAL(true, m_pPlayerRedLeader->GetPlayerBattleArena().RegisterEntry());

		// 새로운 투기장이 생성됨
		GBattleArena* pBattleArena1 = gmgr.pBattleArenaMgr->FindBattleArena(m_pPlayerBlueLeader->GetPlayerBattleArena().GetBattleArenaUID());
		RUN_LIST(GEntityPlayer, vecAllPlayers, GetPlayerField().GetFieldEntry().OnEnterFieldComplete());
		WaitForPlayCount();
		// 게임 종료됨
		m_pPlayerRedLeader->doDie();
		m_pPlayerRedMember1->doDie();

		// 점수판 뜨고 종료되기전에 한명 종료
		GGateInfo rGateInfo = m_pPlayerRedMember2->GetPlayerField().GetGateInfo();
		rGateInfo.nFieldID = m_pPlayerRedMember2->GetFieldID();
		rGateInfo.vPosition = m_pPlayerRedMember2->GetPos();
		m_pPlayerRedMember2->GetPlayerField().SetGateInfo(rGateInfo);

		m_pPlayerRedMember2->Destroy();
		GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->GetPlayer(m_pPlayerRedMember2->GetUID());
		TVALID(pPlayerObj);
		gmgr.pPlayerObjectManager->ErasePlayer(pPlayerObj->GetUID());
		
		// 게임 종료후 재접속
		WaitForScoreBoardCount();
		gmgr.pPlayerObjectManager->AddPlayer(pPlayerObj);
		pPlayerObj->OnEndLogin();
		m_pLinkRedMember2->OnRecv(MC_CHAR_GAMESTART,		2, NEW_INT(-1), NEW_VEC(vec3::ZERO));

		// 원래의 공유필드로 돌아옴
		RUN_LIST(GEntityPlayer, vecAllPlayers, GetPlayerField().GetFieldEntry().OnEnterFieldComplete());
		CHECK_EQUAL(777, m_pPlayerRedMember2->GetFieldID());	
	}
}

