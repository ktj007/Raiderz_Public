#include "StdAfx.h"
#include "GTestWrapper.h"
#include "SUnitTestUtil.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "MockLink.h"
#include "GDuelMgr.h"
#include "GDuel.h"
#include "CCommandTable.h"
#include "GEntityPlayer.h"
#include "GModuleCombat.h"
#include "GCombatCalculator.h"
#include "GConst.h"
#include "GModuleAI.h"
#include "GPartySystem.h"
#include "GFieldSystem.h"
#include "GDynamicFieldMaster.h"
#include "TestCombatCalculator.h"
#include "GTestForward.h"
#include "GServer.h"
#include "GSharedField.h"
#include "GPlayerField.h"
#include "FBaseMockLink.h"
#include "GPlayerFieldDynamic.h"

#define KILL_DAMAGE 99999
#define RUNAWAY_DISTANCE 1000+DUEL_RADIUS


SUITE(PartyDuel)
{
	vec3 vCommonPos = vec3(1000, 1000, 0);

	struct FPartyDuel : public FBaseMockLink
	{
		FPartyDuel()
		{ 
			m_bOldPvPMode = GConst::PVP_MODE;
			GConst::PVP_MODE = false;

			// 결투깃발 NPCID 등록
			GNPCInfo* pinfoNPC = new GNPCInfo();
			pinfoNPC->nID = DUEL_FLAG_NPCID;
			pinfoNPC->nMaxHP = 50;
			
			gmgr.pNPCInfoMgr->Insert(pinfoNPC);
		
			m_pField = GUTHelper_Field::NewMockField();
			m_pPlayerRequester	= NewPlayer(L"Requester", m_pLinkRequester);
			m_pPlayerTarget		= NewPlayer(L"Target", m_pLinkTarget);
			m_pPlayerRequester2	= NewPlayer(L"Requester2", m_pLinkRequester2);
			m_pPlayerTarget2	= NewPlayer(L"Target2", m_pLinkTarget2);
				
			MakeParty(m_pPlayerRequester, m_pPlayerRequester2);
			MakeParty(m_pPlayerTarget, m_pPlayerTarget2);

			SetIgnoreCommonCommandDefaultPlayers();
			gsys.pServer->FrameCounterClear();
		}

		void MakeParty(GEntityPlayer* pLeader, GEntityPlayer* pMember)
		{
			GParty* pParty = GUTHelper_Party::CreateParty(pLeader);
			GUTHelper_Party::AddMember(pParty, pMember);
		}

		~FPartyDuel() 
		{
			GConst::PVP_MODE = m_bOldPvPMode;
			m_pField->Destroy();
		}

		GEntityPlayer* NewPlayer(const wchar_t* pszName, MockLink*& pLink)
		{
			GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
			swprintf_s(pPlayer->GetPlayerInfo()->szName, pszName);
			pLink = NewLink(pPlayer);
			return pPlayer;
		}

		GTalentInfo* MakeSimpleTalent(int nDamage, int nTalentID = 1)
		{
			GTalentInfo* pMagicTalent = test.talent.NewMagicTalentInfo(nTalentID);
			test.talent.SetTalentDamage(pMagicTalent, nDamage);
			pMagicTalent->m_fDurationTime = 1.0f;
			pMagicTalent->m_fCastingTime = 0.0f;
			pMagicTalent->m_Resist.m_nDifficulty = 255;	// 저항 불가능하게 수정
			pMagicTalent->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
			pMagicTalent->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ENEMY;
			pMagicTalent->Cooking();
			return pMagicTalent;
		}

		GField* MakeSharedField(int nFieldID)
		{
			GFieldInfo* pFieldInfo = new GFieldInfo;
			pFieldInfo->m_bDynamic = false;
			pFieldInfo->m_nFieldID = nFieldID;
			pFieldInfo->m_nDefaultChannelCount = 0;
			pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
			gmgr.pFieldInfoMgr->Insert(pFieldInfo);

			GSharedField* pTestField = GUTHelper_Field::NewSharedField(pFieldInfo, 1);
			VALID_RET(pTestField, NULL);
			return pTestField;
		}

		void MakeDynamicField(int nGroupID, int nFieldID)
		{
			FIELD_GROUP_INFO* info = new FIELD_GROUP_INFO;
			info->nID = nGroupID;
			GFieldInfo* pFieldInfo = new GFieldInfo;
			pFieldInfo->m_bDynamic = true;
			pFieldInfo->m_nFieldID = nFieldID;
			pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);

			MARKER_INFO marker;
			marker.nID = 1;
			marker.nFieldID = nFieldID;
			marker.vPoint = vCommonPos;
			pFieldInfo->InsertMarkerInfo(marker);

			info->vecFieldInfo.push_back(pFieldInfo);
			info->pStartFieldInfo = pFieldInfo;
			info->pStartMarkerInfo = pFieldInfo->FindMarker(1);

			gmgr.pFieldInfoMgr->Insert(pFieldInfo);
			gmgr.pFieldInfoMgr->InsertGroup(info);
		}

		void SetIgnoreCommonCommand(MockLink* pLink)
		{
			pLink->AddIgnoreID(MC_ACTION_DIE);	
			pLink->AddIgnoreID(MC_NPC_NARRATION);	
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
		}

		void SetIgnoreCommonCommandDefaultPlayers()
		{
			SetIgnoreCommonCommand(m_pLinkRequester);
			SetIgnoreCommonCommand(m_pLinkTarget);
		}

		void BeginCommandRecord()
		{
			m_pLinkRequester->ResetCommands();
			m_pLinkTarget->ResetCommands();
		}

		void WaitDuelCount()
		{
			for (int i=0; i<DUEL_READY_COUNT; i++)
			{
				m_pField->Update(1.0f);
			}
		}

		void DuelQuestion()
		{
			m_pLinkRequester->OnRecv(MC_DUEL_PARTY_REQUEST,		1, NEW_UID(m_pPlayerTarget->GetUID()));
		}

		void DuelRespond(bool bAnswer)
		{
			m_pLinkTarget->OnRecv(MC_DUEL_QUESTION_RESPOND, 1, NEW_BOOL(bAnswer));
		}

		void DuelStart()
		{
			DuelQuestion();
			DuelRespond(true);
			WaitDuelCount();
		}

		void DuelFinish(GEntityPlayer* pLoser)
		{
			GParty* pParty = gsys.pPartySystem->FindParty(pLoser->GetPartyUID());
			if (!pParty)
				return;

			for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
			{
				MUID uidMember = it->first;

				GEntityPlayer* pMember = m_pField->FindPlayer(uidMember);
				if (NULL == pMember) continue;

				pMember->SetPos(vec3(RUNAWAY_DISTANCE, 0, 0));	// 도망침
			}

			m_pField->Update(0.1f);
		}

		void CheckDuelCancel(bool bRequesterOnly=false)
		{
			m_pField->Update(0.0f);
			CHECK_EQUAL(m_pLinkRequester->GetCommand(0).GetID(), MC_DUEL_CANCEL);
			if (!bRequesterOnly)
			{
				CHECK_EQUAL(m_pLinkTarget->GetCommand(0).GetID(), MC_DUEL_CANCEL);
			}
		}

		void CheckDuelFinished(GEntityPlayer* pWinner, GEntityPlayer* pLoser)
		{
			MockLink* pWinnerLink = pWinner==m_pPlayerRequester?m_pLinkRequester:m_pLinkTarget;
			const minet::MCommand& Command = pWinnerLink->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_DUEL_FINISHED);
			if (Command.GetParameterCount() < 2)						 { CHECK(false); return; }
			MUID uidWinner;
			MUID uidLoser;
			if (!Command.GetParameter(&uidWinner,	0, MPT_UID))		{ CHECK(false); return; }
			if (!Command.GetParameter(&uidLoser,	1, MPT_UID))		{ CHECK(false); return; }
			CHECK_EQUAL(uidWinner, pWinner->GetUID());
			CHECK_EQUAL(uidLoser, pLoser->GetUID());
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GEntityPlayer*	m_pPlayerRequester;
		GEntityPlayer*	m_pPlayerTarget;
		GEntityPlayer*	m_pPlayerRequester1;
		GEntityPlayer*	m_pPlayerTarget1;
		GEntityPlayer*	m_pPlayerRequester2;
		GEntityPlayer*	m_pPlayerTarget2;
		MockLink*		m_pLinkRequester;
		MockLink*		m_pLinkTarget;
		MockLink*		m_pLinkRequester1;
		MockLink*		m_pLinkTarget1;
		MockLink*		m_pLinkRequester2;
		MockLink*		m_pLinkTarget2;
		bool			m_bOldPvPMode;

		GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
		GTestMgrWrapper<GFieldMgr>					m_FieldMgr;
		GTestMgrWrapper<GTalentInfoMgr>				m_TalentInfoMgr;
		GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgrWrapper;
		GTestSysWrapper2<GAttackDamageCalculator, SimpleAttackDamageCalculator>	m_AttackDamageCalculator;
		GTestSysWrapper2<GCombatCalculator, SimpleCombatCalculator>				m_CombatCalculator;
		GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>						m_FakeSystemWrapper;
		DECLWRAPPER_NPCMgr;
	};

	TEST_FIXTURE(FPartyDuel, DuelRequest)
	{
		DuelQuestion();
	}

	TEST_FIXTURE(FPartyDuel, IsCombatInDuel)
	{
		DuelStart();
		CHECK_EQUAL(m_pPlayerRequester->HasDuel(), true);
		CHECK_EQUAL(m_pPlayerTarget->HasDuel(), true);
		CHECK_EQUAL(m_pPlayerRequester2->HasDuel(), true);
		CHECK_EQUAL(m_pPlayerTarget2->HasDuel(), true);
		DuelFinish(m_pPlayerRequester);
		CHECK_EQUAL(m_pPlayerRequester->HasDuel(), false);
		CHECK_EQUAL(m_pPlayerTarget->HasDuel(), false);
		CHECK_EQUAL(m_pPlayerRequester2->HasDuel(), false);
		CHECK_EQUAL(m_pPlayerTarget2->HasDuel(), false);
	}

	TEST_FIXTURE(FPartyDuel, Hittable)
	{
		DuelStart();
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(10);
		pAtkTalent->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ENEMY;
		// 적
		CHECK_EQUAL(true, m_pPlayerRequester->IsHittable(m_pPlayerTarget, pAtkTalent));
		// 같은 파티
		CHECK_EQUAL(false, m_pPlayerRequester->IsHittable(m_pPlayerRequester2, pAtkTalent));
		// 구경꾼
		GEntityPlayer* pObserver = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		CHECK_EQUAL(false, m_pPlayerRequester->IsHittable(pObserver, pAtkTalent));

		// 1번적 죽음
		GTalentInfo* pKillTalentInfo = MakeSimpleTalent(KILL_DAMAGE);
		test.talent.UseTalent(m_pPlayerRequester, pKillTalentInfo, m_pPlayerTarget->GetUID());

		// 이미 탈락한 적
		CHECK_EQUAL(false, m_pPlayerRequester->IsHittable(m_pPlayerTarget, pAtkTalent));
		CHECK_EQUAL(true, m_pPlayerRequester->IsHittable(m_pPlayerTarget2, pAtkTalent));
	}

	TEST_FIXTURE(FPartyDuel, DuelKilled)
	{
		DuelStart();
		// 1번적 죽음
		GTalentInfo* pKillTalentInfo = MakeSimpleTalent(KILL_DAMAGE);
		test.talent.UseTalent(m_pPlayerRequester, pKillTalentInfo, m_pPlayerTarget->GetUID());
		m_pField->Update(0.0f);
		CHECK_EQUAL(false, m_pPlayerTarget->IsDead());
	}

	TEST_FIXTURE(FPartyDuel, Command_DuelKilled)
	{
		DuelStart();
		BeginCommandRecord();
		// 1번적 죽음
		GTalentInfo* pKillTalentInfo = MakeSimpleTalent(KILL_DAMAGE);
		test.talent.UseTalent(m_pPlayerRequester, pKillTalentInfo, m_pPlayerTarget->GetUID());
		m_pField->Update(0.0f);
		const minet::MCommand& Command = m_pLinkRequester->GetCommand(0);
		TVALID_EQ(m_pLinkRequester->GetCommand(0).GetID(), MC_DUEL_DEFEATED);
		MUID uidDefeater;
		if (!Command.GetParameter(&uidDefeater,	0, MPT_UID))		{ CHECK(false); return; }
		CHECK_EQUAL(uidDefeater, m_pPlayerTarget->GetUID());
	}

	TEST_FIXTURE(FPartyDuel, SameDuelObject)
	{
		DuelStart();
		TVALID_EQ(m_pPlayerRequester->GetDuel(), m_pPlayerTarget->GetDuel());
	}

	TEST_FIXTURE(FPartyDuel, GetLeader)
	{
		DuelStart();
		CHECK_EQUAL(m_pPlayerRequester, m_pPlayerRequester->GetDuel()->GetLeader1());
		CHECK_EQUAL(m_pPlayerTarget, m_pPlayerRequester->GetDuel()->GetLeader2());
	}

	TEST_FIXTURE(FPartyDuel, DuelCalcCenterPosition)
	{
		m_pPlayerRequester->SetPos(vec3(1000, 1000, 0));
		m_pPlayerTarget->SetPos(vec3(2000, 2000, 0));
		m_pPlayerRequester2->SetPos(vec3(3000, 3000, 0));
		m_pPlayerTarget2->SetPos(vec3(4000, 4000, 0));

		DuelStart();
		CHECK(m_pPlayerRequester->GetDuel()->GetCenterPos().IsEqual(vec3(1500, 1500, 0)));
	}

	TEST_FIXTURE(FPartyDuel, DontHealingByObserver_AlliedCase)
	{
		// 플레이어가 쓰는 탤런트
		GTalentInfo* pTalentInfo = new GTalentInfo();
		m_Helper.SetMagicTargetTalentInfo(pTalentInfo, 1);
		pTalentInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALLIED;
		pTalentInfo->m_nMaxHeal = pTalentInfo->m_nMinHeal = 10;		// hp를 10 올려줌
		gmgr.pTalentInfoMgr->Insert(pTalentInfo);

		GEntityPlayer* pObserver = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

		// 힐 가능
		m_pPlayerTarget->SetHP(100);
		CHECK_EQUAL(m_pPlayerTarget->GetHP(), 100);
		test.talent.UseTalent(pObserver, pTalentInfo, m_pPlayerTarget->GetUID());
		CHECK_EQUAL(m_pPlayerTarget->GetHP(), 110);

		// 결투 시작
		DuelStart();

		// 힐 불가능
		m_pPlayerTarget->SetHP(100);
		CHECK_EQUAL(m_pPlayerTarget->GetHP(), 100);
		test.talent.UseTalent(pObserver, pTalentInfo, m_pPlayerTarget->GetUID());
		CHECK_EQUAL(m_pPlayerTarget->GetHP(), 100);

		// 결투 종료
		DuelFinish(m_pPlayerRequester);

		// 결투가 끝나면 다시 피격될 수 없음
		m_pPlayerTarget->SetHP(100);
		CHECK_EQUAL(m_pPlayerTarget->GetHP(), 100);
		test.talent.UseTalent(pObserver, pTalentInfo, m_pPlayerTarget->GetUID());
		CHECK_EQUAL(m_pPlayerTarget->GetHP(), 110);
	}

	TEST_FIXTURE(FPartyDuel, DontHealingByObserver_PartyCase)
	{
		// 플레이어가 쓰는 탤런트
		GTalentInfo* pTalentInfo = new GTalentInfo();
		m_Helper.SetMagicTargetTalentInfo(pTalentInfo, 1);
		pTalentInfo->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_PARTY;
		pTalentInfo->m_nMaxHeal = pTalentInfo->m_nMinHeal = 10;		// hp를 10 올려줌
		gmgr.pTalentInfoMgr->Insert(pTalentInfo);

		GEntityPlayer* pObserver = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		
		// 힐 가능
		m_pPlayerTarget->SetHP(100);
		CHECK_EQUAL(m_pPlayerTarget->GetHP(), 100);
		test.talent.UseTalent(pObserver, pTalentInfo, m_pPlayerTarget->GetUID());
		CHECK_EQUAL(m_pPlayerTarget->GetHP(), 100);

		// 결투 시작
		DuelStart();

		// 힐 불가능
		MakeParty(pObserver, m_pPlayerTarget);
		m_pPlayerTarget->SetHP(100);
		CHECK_EQUAL(m_pPlayerTarget->GetHP(), 100);
		test.talent.UseTalent(pObserver, pTalentInfo, m_pPlayerTarget->GetUID());
		CHECK_EQUAL(m_pPlayerTarget->GetHP(), 100);

		// 결투 종료
		DuelFinish(m_pPlayerRequester);

		// 결투가 끝나면 다시 피격될 수 없음
		m_pPlayerTarget->SetHP(100);
		CHECK_EQUAL(m_pPlayerTarget->GetHP(), 100);
		test.talent.UseTalent(pObserver, pTalentInfo, m_pPlayerTarget->GetUID());
		CHECK_EQUAL(m_pPlayerTarget->GetHP(), 110);
	}

	TEST_FIXTURE(FPartyDuel, LifeCycle)
	{
		CHECK(NULL == m_pPlayerRequester->GetDuel());
		CHECK(NULL == m_pPlayerTarget->GetDuel());
		CHECK(NULL == m_pPlayerRequester2->GetDuel());
		CHECK(NULL == m_pPlayerTarget2->GetDuel());
		DuelStart();
		CHECK(NULL != m_pPlayerRequester->GetDuel());
		CHECK(NULL != m_pPlayerTarget->GetDuel());
		CHECK(NULL != m_pPlayerRequester2->GetDuel());
		CHECK(NULL != m_pPlayerTarget2->GetDuel());
		DuelFinish(m_pPlayerRequester);
		CHECK(NULL == m_pPlayerRequester->GetDuel());
		CHECK(NULL == m_pPlayerTarget->GetDuel());
		CHECK(NULL == m_pPlayerRequester2->GetDuel());
		CHECK(NULL == m_pPlayerTarget2->GetDuel());
	}

	TEST_FIXTURE(FPartyDuel, ResetWhenChangeField)
	{
		int nTestSharedFieldID = 100;
		GField* pOhterField = MakeSharedField(nTestSharedFieldID);
		DuelStart();
		m_pPlayerRequester->GetPlayerField().Gate(nTestSharedFieldID, vec3(1000, 1000, 1000), vec3(0,1,0), false);
		m_pField->Update(0.0f); // 결투 종료 틱
		// 필드 이동
		gmgr.pFieldMgr->Update(0.0f); 
		m_pPlayerRequester->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
		CHECK(NULL == m_pPlayerRequester->GetDuel());
	}
}
