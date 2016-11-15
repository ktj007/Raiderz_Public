#include "stdafx.h"
#include "MockEntityNPC.h"
#include "GModuleAI.h"
#include "MockJobMgr.h"
#include "GTestForward.h"
#include "GJobT_Warp.h"
#include "GJobT_GainBuff.h"
#include "GJobT_Despawn.h"
#include "GJobT_Die.h"
#include "GJobT_Assist.h"
#include "GJobT_Attack.h"
#include "GJobT_BreakPart.h"


SUITE(InstantJob)
{
	class MockNPC: public MockEntityNPC
	{
	public:
		MockNPC()
		{
			isRunInstantBuffJob = false;
			isRunDespawnJob = false;
			isRunDieJob = false;
			isRunAssistJob = false;
			isRunAttackJob = false;
			pNPCTarget = NULL;
		}

		void SetField(GField* pField)		{ m_pField = pField; }

		virtual bool GainBuff(int nBuffID, GTalentInfo* pTalentInfo=NULL, GEntityActor* pUser=NULL) override
		{
			isRunInstantBuffJob = true;
			return true;
		}

		virtual void doDespawn(bool bSpawnEnable) override
		{
			isRunDespawnJob = true;
		}

		virtual void doDie(void) override
		{
			isRunDieJob = true;
		}

		virtual void doAssist(GEntityNPC* pAlliedNPC) override
		{
			isRunAssistJob = true;
		}

		virtual void doAttack(MUID uidTarget, int nDamage) override
		{
			isRunAttackJob = true;
		}

		virtual GEntityPlayer*	FindPlayer(const MUID& uidPlayer) override
		{
			return NULL;
		}

		virtual GEntityNPC*	FindNPC(const MUID& uidNPC) const override
		{
			return pNPCTarget;
		}

	public:				
		bool isRunInstantBuffJob;
		bool isRunDespawnJob;
		bool isRunDieJob;
		bool isRunAssistJob;
		bool isRunAttackJob;
		GEntityNPC* pNPCTarget;
	};


	class Fixture
	{
	public:
		Fixture(): ai(&npc)	{
			npc.Create(SUnitTestUtil::NewUID());
		}
		~Fixture() {
			npc.Destroy();
		}

		void RunJob(GInstantJob* job)
		{
			mgr.Push(job);
			mgr.Run(0.0f);
		}

	public:
		MockNPC npc;
		GModuleAI ai;
		MockJobMgr mgr;
	};

	TEST_FIXTURE(Fixture, CheckWarpJob)
	{
		MockField* pField = GUTHelper_Field::DefaultMockField();
		npc.SetField(pField);

		vec3 vOld = npc.GetPos();

		GJobT_Warp* job = new GJobT_Warp(&ai, vec3(1000,1000,1000), vec3(), true);
		RunJob(job);		
		CHECK(npc.GetPos().IsEqual(vec3(1000,1000,1000)));	// 위치가 바뀌었음
	}

	TEST_FIXTURE(Fixture, CheckInstantBuffJob)
	{
		GJobT_GainBuff* job = new GJobT_GainBuff(&ai, 0);		
		RunJob(job);
		CHECK(npc.isRunInstantBuffJob);
	}

	TEST_FIXTURE(Fixture, CheckDespawn)
	{
		GJobT_Despawn* job = new GJobT_Despawn(&ai, true);
		RunJob(job);
		CHECK(npc.isRunDespawnJob);
	}

	TEST_FIXTURE(Fixture, CheckDie)
	{
		GJobT_Die* job = new GJobT_Die(&ai, NULL);
		RunJob(job);
		CHECK(npc.isRunDieJob);
	}

	TEST_FIXTURE(Fixture, CheckAssist)
	{
		MockNPC target;
		npc.pNPCTarget = &target;
		GJobT_Assist* job = new GJobT_Assist(&ai, target.GetUID());
		RunJob(job);
		CHECK(npc.isRunAssistJob);
	}

	TEST_FIXTURE(Fixture, CheckAttack)
	{
		GJobT_Attack* job = new GJobT_Attack(&ai, npc.GetUID(), 0);
		RunJob(job);
		npc.Update(0.0f);
		CHECK(npc.isRunAttackJob);
	}
}
