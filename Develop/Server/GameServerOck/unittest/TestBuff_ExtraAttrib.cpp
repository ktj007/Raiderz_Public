#include "stdafx.h"
#include "GTestForward.h"
#include "GModuleBuff.h"
#include "GServer.h"
#include "GFieldMgr.h"
#include "GPlayerObjectManager.h"
#include "GEntity.h"
#include "FBasePlayer.h"
#include "GModuleCombat.h"
#include "CCommandResultTable.h"

SUITE(Buff_ExtraAttrib)
{
	struct FBuff : public FBasePlayer
	{
		FBuff()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = NewEntityPlayer(m_pField, vec3(100,100,0));
			m_pLinkTester = test.network.NewLink(m_pPlayer);
			swprintf_s(m_pPlayer->GetPlayerInfo()->szName, L"Tester");

			gsys.pServer->FrameCounterClear();
			SetIgnoreCommonCommand(m_pLinkTester);
		}

		virtual ~FBuff()
		{
			test.network.ClearLinks();
			m_pField->Destroy();
		}

		void BeginCommandRecord()
		{
			m_pLinkTester->ResetCommands();
		}

		void SetIgnoreCommonCommand(MockLink* pLink)
		{
			pLink->AddIgnoreID(MC_ACTION_USE_TALENT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT_NO_MF);
		}

		GTalentInfo* CreateTestTalent_BuffPassiveExtra(int nBuffID, float fDurationTime, BUFF_PASSIVE_EXTRA_ATTRIB_ eAttrib)
		{
			GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
			pBuff->m_Condition.nEffectConditionType = TC_BUFF_GAIN;
			pBuff->m_nPassiveExtraAttrib = eAttrib;
			GTalentInfo* pBuffTalent=test.buff.NewBuffTalent(pBuff);
			pBuff->m_fDuration = fDurationTime;

			return pBuffTalent;
		}

		GTalentInfo* CreateTestTalent_BuffActiveExtra(int nBuffID, float fDurationTime, BUFF_ACTIVE_EXTRA_ATTRIB_ eAttrib)
		{
			GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
			pBuff->m_nActiveExtraAttrib = eAttrib;
			pBuff->m_Condition.nEffectConditionType = TC_BUFF_GAIN;
			GTalentInfo* pBuffTalent=test.buff.NewBuffTalent(pBuff);
			pBuff->m_fDuration = fDurationTime;

			return pBuffTalent;
		}


		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;		
		MockLink*		m_pLinkTester;

		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_BuffMgr;
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectManager;
	};

	TEST_FIXTURE(FBuff, Invincibility)
	{
		float fDurationTime = 5.0f;
		int nBuffID = 123;
		GTalentInfo* pBuffTalent = CreateTestTalent_BuffPassiveExtra(nBuffID, fDurationTime, BUFPEA_INVINCIBILITY);
		
		TVALID(m_pPlayer->GetModuleBuff());

		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(false, m_pPlayer->GetModuleBuff()->IsNowInvincibility());

		// 버프 얻기
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(true, m_pPlayer->GetModuleBuff()->IsNowInvincibility());

		// 버프 잃음
		m_pField->Update(fDurationTime);
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(false, m_pPlayer->GetModuleBuff()->IsNowInvincibility());
	}

	TEST_FIXTURE(FBuff, Invincibility__Actual)
	{
		float fDurationTime = 5.0f;
		int nBuffID = 123;
		GTalentInfo* pBuffTalent = CreateTestTalent_BuffPassiveExtra(nBuffID, fDurationTime, BUFPEA_INVINCIBILITY);
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(10);	// 피해 10을 주는 탤런트

		// 적 생성
		GNPCInfo		m_NPCInfo;
		m_NPCInfo.nID = 1;
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vec3(100,100,0), vec3(0,1,0));

		// 공격시도
		m_pPlayer->SetHP(100);
		test.talent.UseTalent(pNPC, p1DmgTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(90, m_pPlayer->GetHP());

		// 버프 얻기
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());

		// 공격시도
		m_pPlayer->SetHP(100);
		test.talent.UseTalent(pNPC, p1DmgTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(100, m_pPlayer->GetHP());

		// 버프 잃음
		m_pField->Update(fDurationTime);

		// 공격시도
		m_pPlayer->SetHP(100);
		test.talent.UseTalent(pNPC, p1DmgTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(90, m_pPlayer->GetHP());
	}

	TEST_FIXTURE(FBuff, DisableAllTalent)
	{
		float fDurationTime = 5.0f;
		int nBuffID = 123;
		GTalentInfo* pBuffTalent = CreateTestTalent_BuffPassiveExtra(nBuffID, fDurationTime, BUFPEA_DISABLE_TALENTS);

		TVALID(m_pPlayer->GetModuleBuff());

		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(false, m_pPlayer->GetModuleBuff()->IsDisableAllTalent());

		// 버프 얻기
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(true, m_pPlayer->GetModuleBuff()->IsDisableAllTalent());

		// 버프 잃음
		m_pField->Update(fDurationTime);
		CHECK_EQUAL(false, m_pPlayer->HasBuff(nBuffID));
		CHECK_EQUAL(false, m_pPlayer->GetModuleBuff()->IsDisableAllTalent());
	}

	TEST_FIXTURE(FBuff, DisableAllTalent__Actual)
	{
		float fDurationTime = 5.0f;
		int nBuffID = 123;
		GTalentInfo* pBuffTalent = CreateTestTalent_BuffPassiveExtra(nBuffID, fDurationTime, BUFPEA_DISABLE_TALENTS);
		GTalentInfo* p1DmgTalent = test.talent.MakeDamageTalent(10);	// 피해 10을 주는 탤런트

		// 적 생성
		GNPCInfo		m_NPCInfo;
		m_NPCInfo.nID = 1;
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vec3(100,100,0), vec3(0,1,0));

		// 공격시도
		pNPC->SetHP(100);
		test.talent.UseTalent(m_pPlayer, p1DmgTalent, pNPC->GetUID());
		CHECK_EQUAL(90, pNPC->GetHP());

		// 버프 얻기
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());

		// 공격시도
		pNPC->SetHP(100);
		test.talent.UseTalent(m_pPlayer, p1DmgTalent, pNPC->GetUID());
		CHECK_EQUAL(100, pNPC->GetHP());

		// 버프 잃음
		m_pField->Update(fDurationTime);

		// 공격시도
		pNPC->SetHP(100);
		test.talent.UseTalent(m_pPlayer, p1DmgTalent, pNPC->GetUID());
		CHECK_EQUAL(90, pNPC->GetHP());
	}

	TEST_FIXTURE(FBuff, DisableAllTalent__WithCancelCurrentTalent)
	{
		float fDurationTime = 5.0f;
		int nBuffID = 123;
		GTalentInfo* pBuffTalent = CreateTestTalent_BuffPassiveExtra(nBuffID, fDurationTime, BUFPEA_DISABLE_TALENTS);
		pBuffTalent->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		GTalentInfo* pTempTalent = test.talent.NewMeleeTalentInfo();
		pTempTalent->m_fCastingTime = 10.0f;
		pTempTalent->Cooking();
		
		// 적 생성
		GNPCInfo		m_NPCInfo;
		m_NPCInfo.nID = 1;
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vec3(100,100,0), vec3(0,1,0));

		// 탤런트 사용
		CHECK_EQUAL(false, pNPC->IsNowUsingTalent());
		test.talent.UseTalent(pNPC, pTempTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, pNPC->IsNowUsingTalent());

		// 버프 얻기
		test.talent.UseTalent(m_pPlayer, pBuffTalent, pNPC->GetUID());
		pNPC->GetModuleCombat()->Update(0.0f);
		
		// 탤런트 취소됨
		CHECK_EQUAL(false, pNPC->IsNowUsingTalent());
	}

	// 같은 계열의 탤런트는 취소됨
	TEST_FIXTURE(FBuff, DisableTalent__WithCancelCurrentTalent)
	{
		float fDurationTime = 5.0f;
		int nBuffID = 123;
		GTalentInfo* pBuffTalent = CreateTestTalent_BuffPassiveExtra(nBuffID, fDurationTime, BUFPEA_DISABLE_TALENT_MELEE);
		pBuffTalent->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		GTalentInfo* pTempTalent = test.talent.NewMeleeTalentInfo();
		pTempTalent->m_fCastingTime = 10.0f;
		pTempTalent->Cooking();

		// 적 생성
		GNPCInfo		m_NPCInfo;
		m_NPCInfo.nID = 1;
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vec3(100,100,0), vec3(0,1,0));

		// 탤런트 사용
		CHECK_EQUAL(false, pNPC->IsNowUsingTalent());
		test.talent.UseTalent(pNPC, pTempTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, pNPC->IsNowUsingTalent());

		// 버프 얻기
		test.talent.UseTalent(m_pPlayer, pBuffTalent, pNPC->GetUID());
		pNPC->GetModuleCombat()->Update(0.0f);

		// 탤런트 취소됨
		CHECK_EQUAL(false, pNPC->IsNowUsingTalent());
	}

	// 다른 계열의 탤런트는 취소되지 않음
	TEST_FIXTURE(FBuff, DisableTalent__WithCancelCurrentTalent2)
	{
		float fDurationTime = 5.0f;
		int nBuffID = 123;
		GTalentInfo* pBuffTalent = CreateTestTalent_BuffPassiveExtra(nBuffID, fDurationTime, BUFPEA_DISABLE_TALENT_RANGE);
		pBuffTalent->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		GTalentInfo* pTempTalent = test.talent.NewMeleeTalentInfo();
		pTempTalent->m_fCastingTime = 10.0f;
		pTempTalent->Cooking();

		// 적 생성
		GNPCInfo		m_NPCInfo;
		m_NPCInfo.nID = 1;
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vec3(100,100,0), vec3(0,1,0));

		// 탤런트 사용
		CHECK_EQUAL(false, pNPC->IsNowUsingTalent());
		test.talent.UseTalent(pNPC, pTempTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, pNPC->IsNowUsingTalent());

		// 버프 얻기
		test.talent.UseTalent(m_pPlayer, pBuffTalent, pNPC->GetUID());
		pNPC->GetModuleCombat()->Update(0.0f);

		// 탤런트 취소되지 않음
		CHECK_EQUAL(true, pNPC->IsNowUsingTalent());
	}

	// m_bIgnoreDisabled 이 true인 탤런트는 취소되지 않음
	TEST_FIXTURE(FBuff, DisableTalent__WithCancelCurrentTalent3)
	{
		float fDurationTime = 5.0f;
		int nBuffID = 123;
		GTalentInfo* pBuffTalent = CreateTestTalent_BuffPassiveExtra(nBuffID, fDurationTime, BUFPEA_DISABLE_TALENT_MELEE);
		pBuffTalent->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		pBuffTalent->m_Buff1.EffectInfo.m_nPoint = CSEffectInfo::POINT_TARGET;
		GTalentInfo* pTempTalent = test.talent.NewMeleeTalentInfo();
		pTempTalent->m_bIgnoreMesmerize = true;
		pTempTalent->m_fCastingTime = 10.0f;
		pTempTalent->Cooking();

		// 적 생성
		GNPCInfo		m_NPCInfo;
		m_NPCInfo.nID = 1;
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&m_NPCInfo, vec3(100,100,0), vec3(0,1,0));

		// 탤런트 사용
		CHECK_EQUAL(false, pNPC->IsNowUsingTalent());
		test.talent.UseTalent(pNPC, pTempTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, pNPC->IsNowUsingTalent());

		// 버프 얻기
		test.talent.UseTalent(m_pPlayer, pBuffTalent, pNPC->GetUID());
		pNPC->GetModuleCombat()->Update(0.0f);

		// 탤런트 취소되지 않음
		CHECK_EQUAL(true, pNPC->IsNowUsingTalent());
	}

	TEST_FIXTURE(FBuff, DisableTalent)
	{
		float fDurationTime = 5.0f;
		int nBuffID = 123;
		GTalentInfo* pBuffTalent = CreateTestTalent_BuffPassiveExtra(nBuffID, fDurationTime, BUFPEA_DISABLE_TALENT_MELEE);

		TVALID(m_pPlayer->GetModuleBuff());

		CHECK_EQUAL(false, m_pPlayer->GetModuleBuff()->IsDisableTalent(TC_MELEE));

		// 버프 얻기
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->GetModuleBuff()->IsDisableTalent(TC_MELEE));
		CHECK_EQUAL(CR_SUCCESS, m_pPlayer->GetModuleCombat()->UseTalent(pBuffTalent,m_pPlayer->GetUID(), false, false));

		// 버프 잃음
		m_pField->Update(fDurationTime);
		CHECK_EQUAL(false, m_pPlayer->GetModuleBuff()->IsDisableTalent(TC_MELEE));
	}

	TEST_FIXTURE(FBuff, DisableTalentForMesmerize)
	{
		float fDurationTime = 5.0f;
		int nBuffID = 123;
		GTalentInfo* pBuffTalent = CreateTestTalent_BuffPassiveExtra(nBuffID, fDurationTime, BUFPEA_SLEEP);

		TVALID(m_pPlayer->GetModuleBuff());

		CHECK_EQUAL(false, m_pPlayer->GetModuleBuff()->IsMesmerize());

		// 버프 얻기
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->GetModuleBuff()->IsMesmerize());
		CHECK_EQUAL(CR_FAIL_USE_TALENT_DISABLED, m_pPlayer->GetModuleCombat()->UseTalent(pBuffTalent,m_pPlayer->GetUID(), false, false));

		// 버프 잃음
		m_pField->Update(fDurationTime);
		CHECK_EQUAL(false, m_pPlayer->GetModuleBuff()->IsMesmerize());
	}

	TEST_FIXTURE(FBuff, Die)
	{
		float fDurationTime = 5.0f;
		int nBuffID = 123;
		GTalentInfo* pBuffTalent = CreateTestTalent_BuffActiveExtra(nBuffID, fDurationTime, BUFAEA_DIE);

		TVALID(m_pPlayer->GetModuleBuff());

		CHECK_EQUAL(false, m_pPlayer->IsDead());

		// 버프 얻기
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->IsDead());
	}

	TEST_FIXTURE(FBuff, Despawn)
	{
		GEntityNPC* pNPC = GUTHelper_NPC::SpawnNPC(m_pField);

		float fDurationTime = 5.0f;
		int nBuffID = 123;
		GTalentInfo* pBuffTalent = CreateTestTalent_BuffActiveExtra(nBuffID, fDurationTime, BUFAEA_DESPAWN_ONCE);

		TVALID(pNPC->GetModuleBuff());

		CHECK_EQUAL(false, pNPC->IsDeleteMe());

		// 버프 얻기
		test.talent.UseTalent(pNPC, pBuffTalent, pNPC->GetUID());		
		CHECK_EQUAL(true, !!pNPC->IsDeleteMe());
	}
	
	struct FBuffRoot :public FBuff
	{
		FBuffRoot()
		{
			CreateRootBuff();
			InitTestPosDir();
			AddAnotherPlayerOnSameField();
		}

		void CreateRootBuff()
		{
			m_nBuffID = 123;
			m_fDurationTime = 5.0f;
			m_pBuffTalent = CreateTestTalent_BuffPassiveExtra(m_nBuffID, m_fDurationTime, BUFPEA_ROOT);
		}

		void InitTestPosDir()
		{
			m_pPlayer->SetPos(vec3(0,0,0));
			m_pPlayer->SetDir(vec3(0,0,1));

			m_vPosA = vec3(100, 100, 0);
			m_vDirA = vec3(1, 0, 0);

			m_vPosB = vec3(200, 200, 0);
			m_vDirB = vec3(0, 1, 0);
		}

		void AddAnotherPlayerOnSameField()
		{
			m_pAnotherPlayer = NewEntityPlayer(m_pField, vec3(500,500,0));
			m_pAnotherPlayerLink = test.network.NewLink(m_pAnotherPlayer);
			m_pAnotherPlayerLink->AddIgnoreID(MC_ACTION_USE_TALENT);
			m_pAnotherPlayerLink->AddIgnoreID(MC_ACTION_TALENT_HIT_NO_MF);
			m_pAnotherPlayerLink->AddIgnoreID(MC_BUFF_GAIN);
			m_pAnotherPlayerLink->AddIgnoreID(MC_BUFF_LOST);
			m_pAnotherPlayerLink->AddIgnoreID(MC_ACTION_CANCEL_TALENT);
			m_pAnotherPlayerLink->AddIgnoreID(MC_ACTION_GUARD_RELEASED);
		}

		void UseRootBuffTalent()
		{
			test.talent.UseTalent(m_pPlayer, m_pBuffTalent, m_pPlayer->GetUID());
		}

		void LostRootBuff()
		{
			m_pField->Update(m_fDurationTime);
		}

		int m_nBuffID;
		float m_fDurationTime;
		GTalentInfo* m_pBuffTalent;

		vec3 m_vPosA;
		vec3 m_vDirA;

		vec3 m_vPosB;
		vec3 m_vDirB;

		GEntityPlayer* m_pAnotherPlayer;
		MockLink* m_pAnotherPlayerLink;
	};

	TEST_FIXTURE(FBuffRoot, Root__Actual_Move)
	{
		TD_PC_MOVE stMoveInfo_A;
		stMoveInfo_A.nFlags = MOVEMENTFLAG_FORWARD;
		stMoveInfo_A.vTarPos = m_vPosA;
		stMoveInfo_A.svDir.SetX(m_vDirA.x);
		stMoveInfo_A.svDir.SetY(m_vDirA.y);

		TD_PC_MOVE stMoveInfo_B;
		stMoveInfo_B.nFlags = MOVEMENTFLAG_FORWARD;
		stMoveInfo_B.vTarPos = m_vPosB;
		stMoveInfo_B.svDir.SetX(m_vDirB.x);
		stMoveInfo_B.svDir.SetY(m_vDirB.y);

		// B 위치로 이동 시도
		m_pLinkTester->OnRecv(MC_ACTION_MOVE_REQ, 1, NEW_SINGLE_BLOB(&stMoveInfo_B, sizeof(TD_PC_MOVE)));
		CHECK_EQUAL(m_vPosB, m_pPlayer->GetPos());
		CHECK_EQUAL(1, m_pAnotherPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_ACTION_MOVE, m_pAnotherPlayerLink->GetCommand(0).GetID());
		m_pAnotherPlayerLink->ResetCommands();

		UseRootBuffTalent();

		// A 위치로 이동 시도
		m_pLinkTester->OnRecv(MC_ACTION_MOVE_REQ, 1, NEW_SINGLE_BLOB(&stMoveInfo_A, sizeof(TD_PC_MOVE)));
		CHECK_EQUAL(m_vPosB, m_pPlayer->GetPos());
		CHECK_EQUAL(0, m_pAnotherPlayerLink->GetCommandCount());

		LostRootBuff();

		// A 위치로 이동 시도
		m_pLinkTester->OnRecv(MC_ACTION_MOVE_REQ, 1, NEW_SINGLE_BLOB(&stMoveInfo_A, sizeof(TD_PC_MOVE)));
		CHECK_EQUAL(m_vPosA, m_pPlayer->GetPos());
		CHECK_EQUAL(1, m_pAnotherPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_ACTION_MOVE, m_pAnotherPlayerLink->GetCommand(0).GetID());
		m_pAnotherPlayerLink->ResetCommands();
	}

	// 루트가 걸려도 클라이언트가 요구한 MoveStop은 처리해주기로 수정 (넉백같은 외부의 이동요인때문)
	//TEST_FIXTURE(FBuffRoot, Root__Actual_Stop)
	//{
	//	// B 위치에서 스톱 시도
	//	m_pLinkTester->OnRecv(MC_ACTION_STOP_REQ, 2, NEW_VEC(m_vPosB), NEW_VEC(m_vDirB));
	//	CHECK_EQUAL(m_vPosB, m_pPlayer->GetPos());
	//	CHECK_EQUAL(1, m_pAnotherPlayerLink->GetCommandCount());
	//	CHECK_EQUAL(MC_ACTION_STOP, m_pAnotherPlayerLink->GetCommand(0).GetID());
	//	m_pAnotherPlayerLink->ResetCommands();

	//	UseRootBuffTalent();

	//	// A 위치에서 스톱 시도
	//	m_pLinkTester->OnRecv(MC_ACTION_STOP_REQ, 2, NEW_VEC(m_vPosA), NEW_VEC(m_vDirA));
	//	CHECK_EQUAL(m_vPosB, m_pPlayer->GetPos());
	//	CHECK_EQUAL(0, m_pAnotherPlayerLink->GetCommandCount());

	//	LostRootBuff();

	//	// A 위치에서 스톱 시도
	//	m_pLinkTester->OnRecv(MC_ACTION_STOP_REQ, 2, NEW_VEC(m_vPosA), NEW_VEC(m_vDirA));
	//	CHECK_EQUAL(m_vPosA, m_pPlayer->GetPos());
	//	CHECK_EQUAL(1, m_pAnotherPlayerLink->GetCommandCount());
	//	CHECK_EQUAL(MC_ACTION_STOP, m_pAnotherPlayerLink->GetCommand(0).GetID());
	//	m_pAnotherPlayerLink->ResetCommands();
	//}

	TEST_FIXTURE(FBuffRoot, Root__Actual_MoveStop)
	{
		// B 위치에서 이동 정지 시도
		m_pLinkTester->OnRecv(MC_ACTION_STOP_MOVE_REQ, 1, NEW_VEC(m_vPosB));
		CHECK_EQUAL(m_vPosB, m_pPlayer->GetPos());
		CHECK_EQUAL(1, m_pAnotherPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_ACTION_STOP_MOVE, m_pAnotherPlayerLink->GetCommand(0).GetID());
		m_pAnotherPlayerLink->ResetCommands();

		UseRootBuffTalent();

		// A 위치에서 이동 정지 시도
		m_pLinkTester->OnRecv(MC_ACTION_STOP_MOVE_REQ, 1, NEW_VEC(m_vPosA));
		CHECK_EQUAL(m_vPosB, m_pPlayer->GetPos());
		CHECK_EQUAL(0, m_pAnotherPlayerLink->GetCommandCount());

		LostRootBuff();

		// A 위치에서 이동 정지 시도
		m_pLinkTester->OnRecv(MC_ACTION_STOP_MOVE_REQ, 1, NEW_VEC(m_vPosA));
		CHECK_EQUAL(m_vPosA, m_pPlayer->GetPos());
		CHECK_EQUAL(1, m_pAnotherPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_ACTION_STOP_MOVE, m_pAnotherPlayerLink->GetCommand(0).GetID());
		m_pAnotherPlayerLink->ResetCommands();
	}

	TEST_FIXTURE(FBuffRoot, Root__Actual_Rotate)
	{
		// B 방향으로 회전 시도
		m_pLinkTester->OnRecv(MC_ACTION_ROTATE_REQ, 1, NEW_SVEC2(m_vDirB));
		CHECK_EQUAL(m_vDirB, m_pPlayer->GetDir());
		CHECK_EQUAL(1, m_pAnotherPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_ACTION_ROTATE, m_pAnotherPlayerLink->GetCommand(0).GetID());
		m_pAnotherPlayerLink->ResetCommands();

		UseRootBuffTalent();

		// A 방향으로 회전 시도
		m_pLinkTester->OnRecv(MC_ACTION_ROTATE_REQ, 1, NEW_SVEC2(m_vDirA));
		CHECK_EQUAL(m_vDirB, m_pPlayer->GetDir());
		CHECK_EQUAL(0, m_pAnotherPlayerLink->GetCommandCount());

		LostRootBuff();

		// A 방향으로 회전 시도
		m_pLinkTester->OnRecv(MC_ACTION_ROTATE_REQ, 1, NEW_SVEC2(m_vDirA));
		CHECK_EQUAL(m_vDirA, m_pPlayer->GetDir());
		CHECK_EQUAL(1, m_pAnotherPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_ACTION_ROTATE, m_pAnotherPlayerLink->GetCommand(0).GetID());
		m_pAnotherPlayerLink->ResetCommands();
	}

	TEST_FIXTURE(FBuff, Invisibility)
	{
		float fDurationTime = 5.0f;
		int nBuffID = 123;
		GTalentInfo* pBuffTalent = CreateTestTalent_BuffPassiveExtra(nBuffID, fDurationTime, BUFPEA_INVISIBILITY);

		TVALID(m_pPlayer->GetModuleBuff());

		CHECK_EQUAL(false, m_pPlayer->IsNowInvisibility());

		// 버프 얻기
		test.talent.UseTalent(m_pPlayer, pBuffTalent, m_pPlayer->GetUID());
		CHECK_EQUAL(true, m_pPlayer->IsNowInvisibility());
	}

	TEST_FIXTURE(FBuff, InvisibilityBuffCooking)
	{
		int nBuffID = 123;
		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		pBuff->m_nPassiveExtraAttrib = BUFPEA_INVISIBILITY;
		TVALID_EQ(0, pBuff->m_Condition.infoRelease.vecAdditionType.size());
		pBuff->Cooking();
		TVALID_EQ(4, pBuff->m_Condition.infoRelease.vecAdditionType.size());
		CHECK_EQUAL(TC_FINISH_TALENT, pBuff->m_Condition.infoRelease.vecAdditionType[0]);
		CHECK_EQUAL(TC_HIT_ME, pBuff->m_Condition.infoRelease.vecAdditionType[1]);
		CHECK_EQUAL(TC_HIT_ENEMY, pBuff->m_Condition.infoRelease.vecAdditionType[2]);
		CHECK_EQUAL(TC_BUFF_HIT, pBuff->m_Condition.infoRelease.vecAdditionType[3]);
	}

}
