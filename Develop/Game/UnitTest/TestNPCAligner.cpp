#include "stdafx.h"
#include "XNPCAligner.h"
#include "XInterpolation.h"
#include "FTestGame.h"
#include "XNonPlayer.h"

SUITE(NPCAligner)
{
	class TestNPCAligner : public XNPCAligner
	{
	public:

		TestNPCAligner() : XNPCAligner() {}
		virtual ~TestNPCAligner() {}
	};

	class FNPCAligner : public FTestGame
	{
	public:
		FNPCAligner()
		{
			XUnitTestHelper::NewNPCInfo(1);

			// NPC ÀÔÀå
			const MUID uidNPC = global.system->GenerateLocalUID();
			m_pNPC = m_Helper.InNPC(uidNPC, 1, vec3(1000.0f, 1000.0f, 0.0f));

		}
		~FNPCAligner()
		{

		}

		void SetEnemyList()
		{
			vector<TD_ENEMY_INFO>& enemylist = gvar.Game.EnemyInfo.GetEnemyList();

			TD_ENEMY_INFO stInfo;
			stInfo.nUIID = m_pNPC->GetUIID();
			enemylist.push_back(stInfo);
		}

		XNonPlayer*			m_pNPC;
		TestNPCAligner		m_NPCAligner;
	};


	TEST_FIXTURE(FNPCAligner, TestCheckStunAndRooted)
	{
		XBuffAttribute buffAttribute;

		CHECK_EQUAL(true, XNPCAligner::XAlignChecker::CheckStunAndRooted(false, buffAttribute));
	}

	TEST_FIXTURE(FNPCAligner, TestCheckStunAndRooted_NotAligned_BecauseOfStun)
	{
		XBuffAttribute buffAttribute;

		CHECK_EQUAL(false, XNPCAligner::XAlignChecker::CheckStunAndRooted(true, buffAttribute));
	}

	TEST_FIXTURE(FNPCAligner, TestCheckStunAndRooted_NotAligned_BecauseOfBuff)
	{
		XBuffAttribute buffAttribute;
		buffAttribute.bRooted = true;

		CHECK_EQUAL(false, XNPCAligner::XAlignChecker::CheckStunAndRooted(false, buffAttribute));
		CHECK_EQUAL(false, XNPCAligner::XAlignChecker::CheckStunAndRooted(true, buffAttribute));
	}

	TEST_FIXTURE(FNPCAligner, TestOnEnterState_TalentState)
	{
		SetEnemyList();
		m_NPCAligner.OnEnterState(m_pNPC, NPC_ACTION_STATE_TALENT);
		CHECK_EQUAL(true, m_NPCAligner.IsEnabled());
	}

	TEST_FIXTURE(FNPCAligner, TestOnEnterState_MoveState)
	{
		SetEnemyList();
		m_pNPC->GetInfo()->bAlignEnemy = true;

		m_NPCAligner.OnEnterState(m_pNPC, NPC_ACTION_STATE_MOVE);
		CHECK_EQUAL(true, m_NPCAligner.IsEnabled());

		m_NPCAligner.OnEnterState(m_pNPC, NPC_ACTION_STATE_NA);
		CHECK_EQUAL(true, m_NPCAligner.IsEnabled());
	}

	TEST_FIXTURE(FNPCAligner, TestOnEnterState_MoveState_Negative)
	{
		SetEnemyList();
		m_pNPC->GetInfo()->bAlignEnemy = false;

		m_NPCAligner.OnEnterState(m_pNPC, NPC_ACTION_STATE_MOVE);
		CHECK_EQUAL(false, m_NPCAligner.IsEnabled());

		m_NPCAligner.OnEnterState(m_pNPC, NPC_ACTION_STATE_NA);
		CHECK_EQUAL(false, m_NPCAligner.IsEnabled());
	}

	TEST_FIXTURE(FNPCAligner, TestOnEnterState_Negative)
	{
		SetEnemyList();
		m_NPCAligner.OnEnterState(m_pNPC, NPC_ACTION_STATE_NONE);
		CHECK_EQUAL(false, m_NPCAligner.IsEnabled());

		m_NPCAligner.OnEnterState(m_pNPC, NPC_ACTION_STATE_HIT);
		CHECK_EQUAL(false, m_NPCAligner.IsEnabled());

		m_NPCAligner.OnEnterState(m_pNPC, NPC_ACTION_STATE_DIE);
		CHECK_EQUAL(false, m_NPCAligner.IsEnabled());

	}


	TEST_FIXTURE(FNPCAligner, TestOnExitState_TalentState)
	{
		SetEnemyList();
		m_NPCAligner.OnEnterState(m_pNPC, NPC_ACTION_STATE_TALENT);

		m_NPCAligner.OnExitState(m_pNPC);
		CHECK_EQUAL(false, m_NPCAligner.IsEnabled());
	}

	TEST_FIXTURE(FNPCAligner, TestOnExitState_MoveState)
	{
		SetEnemyList();
		m_pNPC->GetInfo()->bAlignEnemy = true;

		m_NPCAligner.OnEnterState(m_pNPC, NPC_ACTION_STATE_MOVE);
		CHECK_EQUAL(true, m_NPCAligner.IsEnabled());

		m_NPCAligner.OnExitState(m_pNPC);
		CHECK_EQUAL(false, m_NPCAligner.IsEnabled());
	}

}