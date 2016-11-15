#include "stdafx.h"
#include "MockJobTask.h"
#include "GTestWrapper.h"
#include "MockField.h"
#include "GUTHelper.h"
#include "GFieldMgr.h"
#include "MockEntityNPC.h"
#include "GJob_Roam.h"

SUITE(JobRoam)
{
	TEST(VectorAfterNormalizeStillSame)
	{
		vec3 dir = vec3(0.2f, 0.4f, -0.3f);
		dir.Normalize();

		vec3 dir2 = dir;
		dir2.Normalize();

		CHECK_CLOSE(dir.x, dir2.x, 0.1f);
		CHECK_CLOSE(dir.y, dir2.y, 0.1f);
		CHECK_CLOSE(dir.z, dir2.z, 0.1f);
	}

	TEST(CheckLocalMatrixRotate)
	{
		vec3 vDestPos;
		vec3 vAdjPos(-1000, -1000, -1000);
		vec3 npc_pos = vec3(0.0f, 0.0f, 0.0f);

		mat4 matTM;
		{
			vec3 npc_dir = vec3(1.0f, 0.0f, 0.0f);
			npc_dir.Normalize();

			matTM.SetLocalMatrix(vec3::ZERO, npc_dir, vec3::AXISZ);

			vDestPos = npc_pos + vAdjPos * matTM;

			CHECK_CLOSE(vDestPos.x, -1000, 0.1f);
		}

		{
			vec3 npc_dir = vec3(0.0f, 1.0f, 0.0f);
			npc_dir.Normalize();

			matTM.SetLocalMatrix(vec3::ZERO, npc_dir, vec3::AXISZ);

			vDestPos = npc_pos + vAdjPos * matTM;
			CHECK_CLOSE(vDestPos.y, -1000, 0.1f);
		}

		{
			vec3 npc_dir = vec3(0.0f, 0.0f, 1.0f);
			npc_dir.Normalize();

			matTM.SetLocalMatrix(vec3::ZERO, npc_dir, vec3::AXISZ);

			vDestPos = npc_pos + vAdjPos * matTM;
			CHECK_CLOSE(vDestPos.z, -1000, 0.1f);
		}
	}


	class MockJobRoam: public GJob_Roam
	{
	public:
		MockJobRoam(GModuleAI* pmAI, float fRadius, float fDurationTime): GJob_Roam(pmAI, fRadius, fDurationTime)	{}
		virtual void OnRoaming() override {}
	};

	TEST(JobRoamStartWithoutSpawnInfo)
	{
		MockEntityNPC npc;		
		MockJobRoam jobRoam(npc.GetModuleAI(), 300.0f, 10.0f);		
		SPAWN_INFO spawnInfo;

		npc.SetSpawnInfo(&spawnInfo);
		CHECK(jobRoam.Start());

		npc.SetSpawnInfo(NULL);
		CHECK(jobRoam.Start());		
	}

	/*struct TaskFixture_WithNoDelete
	{
		TaskFixture_WithNoDelete() 
		{ 
			pTask = new MockTask(&mNPC);
			pTestTask = NULL;
		}

		~TaskFixture_WithNoDelete() 
		{

		}

		MockModuleAI mAI;
		GEntityNPC mNPC;
		MockTask* pTask;
		MockTask* pTestTask;
	};*/


	//TEST_FIXTURE(TaskFixture_WithNoDelete, RoamingIsMustMoveFromSpawnInfo)
	//{
	//	SAFE_DELETE(pTask); // 이 테스트는 pTask를 쓰지 않는다.

	//	GUTHelper		m_Helper;
	//	GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgr;
	//	const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
	//	const vec3 VCommonDir = vec3(0,1,0);

	//	MockField* pField = m_Helper.DefaultMockField();
	//	GNPCInfo* m_pNPCInfo = new GNPCInfo;
	//	m_pNPCInfo->nID = 1;
	//	m_pNPCInfo->nMaxHP = 50000;
	//	gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);

	//	SPAWN_INFO* pSpawninfo = new SPAWN_INFO();
	//	pSpawninfo->bCheckSpawnPointValidation = false;
	//	pSpawninfo->qNPCInfoList.push_back(SPAWN_NPC_INFO(1));
	//	pSpawninfo->vPoint = vec3(1234.1f, 5678.2f, 9012.3f);
	//	pSpawninfo->fRoamRadius = 1000.0f;
	//	pSpawninfo->fRoamDepth = 200.0f;
	//	GEntityNPC* pNPC = pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, VCommonDir, pSpawninfo);
	//	GModuleAI* pmAI = pNPC->GetModuleAI();
	//	if (!pmAI) 	{ CHECK(FALSE && "ModuleAI is null.");	 return; }

	//	pmAI->Update(1.0f);	// 잡이 큐에서 Current로 이동된다.
	//	pmAI->Update(1.0f);	// ReturnToSpawn job expired.
	//	pmAI->Update(1.0f);	// new job income from mission queue
	//	pmAI->Update(1.0f); // load from job queue

	//	GJob* pJob = pmAI->GetCurrentJob();
	//	if (!pJob) 	{ CHECK(FALSE && "Job is null.");	 return; }
	//	CHECK_EQUAL(pJob->GetID(), GJOB_ROAM);
	//	GJob_Roam* pMoveJob = static_cast<GJob_Roam*>(pJob);
	//	CHECK_EQUAL(pMoveJob->GetRadius(), 1000.0f);

	//	SAFE_DELETE(pSpawninfo);
	//	SAFE_DELETE(pNPC);
	//	pField->Destroy();
	//}
}