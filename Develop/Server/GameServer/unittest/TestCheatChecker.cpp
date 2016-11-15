#include "StdAfx.h"
#include "GTestWrapper.h"
#include "GUnitTestUtil.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "MockLink.h"
#include "CCommandTable.h"
#include "GEntityPlayer.h"
#include "GModuleCombat.h"
#include "GServer.h"
#include "GFieldMgr.h"
#include "GSharedField.h"
#include "GFieldInfoMgr.h"
#include "GPlayerField.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"
#include "GCheatChecker.h"
#include "GPlayerSetterForTest.h"
#include "GConst.h"

SUITE(CheatChecker)
{
	struct FCheatChecker : public FBaseMockLink
	{
		class MockCheatChecker : public GCheatChecker
		{
		public:
			MOCK_METHOD3(OnLog_Disconnect, void(CHEAT_TYPE nCheatType, int nCheatSubType, GEntityPlayer* pPlayer));
			MOCK_METHOD3(OnLog_Warning, void(CHEAT_TYPE nCheatType, int nCheatSubType, GEntityPlayer* pPlayer));
		};

		FCheatChecker()
		{ 
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField, vec3(100,100,0));
			m_pLinkTester = NewLink(m_pPlayer);

			const_cast<PLAYER_INFO*>(m_pPlayer->GetPlayerInfo())->nPlayerGrade = PLAYER_GRADE_NORMAL;

			swprintf_s(m_pPlayer->GetPlayerInfo()->szName, L"Tester");
			m_fMoveSpeed = 400;
			m_pPlayer->GetPlayerInfo()->fSpeed = m_fMoveSpeed;

			pMockCheatChecker = new MockCheatChecker();
			m_pPlayer->GetPlayerSetterForTest().SetPlayerCheatChecker(pMockCheatChecker);
			pMockCheatChecker->Init(m_pPlayer);

			gsys.pServer->FrameCounterClear();
		}


		~FCheatChecker() 
		{
			m_pField->Destroy();
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

		GTalentInfo* MakeKnockbackTalent(int nDamage, int nTalentID = 1)
		{
			GTalentInfo* pPlayerTalentInfo = m_Helper.NewMeleeTalentInfo(nTalentID);	
			m_Helper.AddSimpleHitInfo(pPlayerTalentInfo);	// 바로 판정하는 탤런트임
			pPlayerTalentInfo->Cooking();
			pPlayerTalentInfo->m_nMinDamage = nDamage;
			pPlayerTalentInfo->m_nMaxDamage = nDamage;
			pPlayerTalentInfo->m_MotionFactorGroup.var[MF_KNOCKBACK].nValue = 100;
			pPlayerTalentInfo->m_MotionFactorGroup.var[MF_KNOCKBACK].nWeight = 50;
			return pPlayerTalentInfo;
		}

		void MoveNormaly()
		{
			gsys.pSystem->Sleep(1.0f);	// 주기적으로 이동 패킷을 보내므로
			m_pField->Update(1.0f); // 치트 주기를 1초마다 하므로
			TD_PC_MOVE	tdMove;
			tdMove.vTarPos = m_pPlayer->GetPos()+vec3(m_fMoveSpeed, 0, 0);
			m_pLinkTester->OnRecv(MC_ACTION_MOVE_REQ, 1, NEW_SINGLE_BLOB(&tdMove, sizeof(TD_PC_MOVE)));
		}

		void MoveIllegal()
		{
			gsys.pSystem->Sleep(1.0f);	// 주기적으로 이동 패킷을 보내므로
			m_pField->Update(1.0f); // 치트 주기를 1초마다 하므로
			TD_PC_MOVE	tdMove;
			tdMove.vTarPos = m_pPlayer->GetPos()+vec3(m_fMoveSpeed*10, 0, 0);	// 가능한 범위보다 멀리 이동
			m_pLinkTester->OnRecv(MC_ACTION_MOVE_REQ, 1, NEW_SINGLE_BLOB(&tdMove, sizeof(TD_PC_MOVE)));
		}

		void MoveShallowIllegal()
		{
			gsys.pSystem->Sleep(1.0f);	// 주기적으로 이동 패킷을 보내므로
			m_pField->Update(1.0f); // 치트 주기를 1초마다 하므로
			TD_PC_MOVE	tdMove;
			tdMove.vTarPos = m_pPlayer->GetPos()+vec3(m_fMoveSpeed*GConst::CHEAT_SPEEDHACK_WARNING_LIMIT_DISTANCE_RATE*3+1, 0, 0);	// 가능한 범위보다 멀리 이동
			m_pLinkTester->OnRecv(MC_ACTION_MOVE_REQ, 1, NEW_SINGLE_BLOB(&tdMove, sizeof(TD_PC_MOVE)));
		}

		void WarpIllegal()
		{
			gsys.pSystem->Sleep(1.0f);	// 주기적으로 이동 패킷을 보내므로
			m_pField->Update(1.0f); // 치트 주기를 1초마다 하므로
			TD_PC_MOVE	tdMove;
			tdMove.vTarPos = m_pPlayer->GetPos()+vec3(GConst::CHEAT_WARPHACK_LIMIT_DISTANCE+1, 0, 0);	// 가능한 범위보다 멀리 이동
			m_pLinkTester->OnRecv(MC_ACTION_MOVE_REQ, 1, NEW_SINGLE_BLOB(&tdMove, sizeof(TD_PC_MOVE)));
		}

		void CheckNoCheat()
		{
			EXPECT_CALL(*pMockCheatChecker, OnLog_Warning(testing::_, testing::_, testing::_)).Times(0);
			EXPECT_CALL(*pMockCheatChecker, OnLog_Disconnect(testing::_, testing::_, testing::_)).Times(0);
		}

		MockCheatChecker* pMockCheatChecker;
		GUTHelper		m_Helper;
		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		MockLink*		m_pLinkTester;
		float			m_fMoveSpeed;

		GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
		GTestMgrWrapper<GFieldMgr>					m_FieldMgr;
		GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>  m_FakeSystemWrapper;
	};

	TEST_FIXTURE(FCheatChecker, PlayerMoveSpeed)
	{
		CHECK_EQUAL(400, m_pPlayer->GetPlayerInfo()->fSpeed);
	}

	TEST_FIXTURE(FCheatChecker, RequestMove_Normal)
	{
		CheckNoCheat();
		MoveNormaly();
	}

	TEST_FIXTURE(FCheatChecker, RequestMove_CheatWarpHack)
	{
		EXPECT_CALL(*pMockCheatChecker, OnLog_Disconnect(CT_POSITION, POSCHEAT_POS_WARP, m_pPlayer));
		WarpIllegal();
	}

	TEST_FIXTURE(FCheatChecker, RequestMove_CheatSpeedHackExecute)
	{
		EXPECT_CALL(*pMockCheatChecker, OnLog_Disconnect(CT_POSITION, POSCHEAT_POS_SPEEDHACK, m_pPlayer));

		for (int i=0; i<GConst::CHEAT_SPEEDHACK_DISCONNECT_EXECUTE_COUNT; ++i)
		{
			if (i%2 == 0)
				m_pPlayer->SetPos(vec3(0,0,0));

			MoveIllegal();
		}
	}

	TEST_FIXTURE(FCheatChecker, RequestMove_CheatWarningExecute)
	{
		EXPECT_CALL(*pMockCheatChecker, OnLog_Warning(CT_POSITION, POSCHEAT_POS_SPEEDHACK, m_pPlayer));

		for (int i=0; i<GConst::CHEAT_SPEEDHACK_WARNING_EXECUTE_COUNT; ++i)
		{
			if (i%2 == 0)
				m_pPlayer->SetPos(vec3(0,0,0));

			MoveShallowIllegal();
		}
	}

	TEST_FIXTURE(FCheatChecker, LocalWarp)
	{
		CheckNoCheat();;
		m_pPlayer->GetPlayerField().Gate(0, vec3(15000, 15000, 15000), vec3(0, 1, 0), false);
		MoveNormaly();	// 워프후에 이동해도 문제 없다.
	}

	TEST_FIXTURE(FCheatChecker, FieldWarp)
	{
		CheckNoCheat();
		int nTestFieldID = 7777;
		GField* pOhterField = MakeSharedField(nTestFieldID);
		m_pPlayer->GetPlayerField().Gate(nTestFieldID, vec3(15000, 15000, 15000), vec3(0, 1, 0), false);
		gmgr.pFieldMgr->Update(0.1f);
		m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
		CHECK_EQUAL(m_pPlayer->GetField(), pOhterField);	// 필드이동 성공
		MoveNormaly();	// 워프후에 이동해도 문제 없다.
	}

	TEST_FIXTURE(FCheatChecker, MotionfactorMove)
	{
		CheckNoCheat();
		GTalentInfo* pKnockbackTalentInfo = MakeKnockbackTalent(1);
		GEntityPlayer* pAttacker = GUTHelper::NewEntityPlayer(
			m_pField, 
			vec3(m_pPlayer->GetPos()+vec3(10,0,0)));
		vec3 vTesterOldPos = m_pPlayer->GetPos();
		TALENT_TARGET_INFO target_info;
		target_info.uidTarget = m_pPlayer->GetUID();
		pAttacker->GetModuleCombat()->UseTalent(pKnockbackTalentInfo, target_info, true);
		m_pField->Update(0.0f);	// 탤런트를 쓰기 위한 틱
		m_pField->Update(0.0f);	// 넉백을 위한 이동을 위한 틱
		CHECK_EQUAL(false, vTesterOldPos.IsEqual(m_pPlayer->GetPos()));	// 뒤로 밀려났는가?
		MoveNormaly();	// 모션팩터 이동후에 이동해도 문제 없다.
	}
}
