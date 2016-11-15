#include "stdafx.h"
#include "SUnitTest.h"
#include "MockEntityActor.h"
#include "GCollisionTree.h"
#include "GGlobal.h"
#include "GFieldInfoMgr.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GUTHelper_NPC.h"
#include "GCollision.h"
#include "FBaseGame.h"
#include "GTestForward.h"

SUITE(COLLISION2)
{

bool CheckPickZTest_ForUnitTest(int nFieldID, vec3& vPos)
{
	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
	if (pFieldInfo == NULL) return false;

	if (pFieldInfo->GetCollisionTree() == NULL) 
	{
		return false;
	}

	if ((vPos.x <= pFieldInfo->MinX()) || (vPos.x >= pFieldInfo->MaxX()) ||
		(vPos.y <= pFieldInfo->MinY()) || (vPos.y >= pFieldInfo->MaxY()) ) 
	{
		return false;
	}

	vec3 vOriginPos = vPos;
	vec3 vToPos = vPos;
	const float SAFE_GAP = 1.0f;
	float maxz = 999999.f;
	float minz = -1.f;
	if (pFieldInfo->GetCollisionTree())
	{
		maxz = pFieldInfo->GetCollisionTree()->GetBoundingBox().maxz;
		minz = pFieldInfo->GetCollisionTree()->GetBoundingBox().minz;
	}
	vOriginPos.z	= min(vPos.z + 200.0f, maxz);
	vToPos.z		= minz - SAFE_GAP;

	// 지형과 충돌처리
	rs3::RCollisionPickInfo CollInfo;
	CollInfo.m_inPickOrigin = vOriginPos;
	CollInfo.m_inPickTo		= vToPos;

	if (pFieldInfo->GetCollisionTree()->Pick(CollInfo) == true)
	{
		vPos.z = CollInfo.m_outPosition.z;
		return true;
	}

	return false;
}

	TEST3(TestPickZ)
	{
		int nFieldID = 110;
		vec3 vPos = vec3(47151.9f, 16453.3f, -292.7f);


		// 이런식으로 Pick 테스트를 하면 됨.
		//CHECK(CheckPickZTest_ForUnitTest(nFieldID, vPos));
	}

	class TestCollision : public GCollision
	{
	public:
		using GCollision::IsCheckableOverlappedCol;
	};

	class FCollision : public FBaseGame
	{
	public:
		FCollision()
		{
			MockField* pField = GUTHelper_Field::DefaultMockField();

			m_pNPCInfo = GUTHelper_NPC::NewNPCInfo(1);

			m_pNPC = GUTHelper_NPC::SpawnNPC(pField, 1);
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		}
		virtual ~FCollision()
		{

		}

		DECLWRAPPER_NPCInfoMgr;
		GNPCInfo*			m_pNPCInfo;
		GEntityNPC*			m_pNPC;
		GEntityPlayer*		m_pPlayer;

	};
	TEST_FIXTURE(FCollision, TestCollision_IsCheckableOverlappedCol)
	{
		// 일반적으로 겹쳐져 있는 것도 판정 체크함
		CHECK_EQUAL(true, TestCollision::IsCheckableOverlappedCol(m_pNPC, m_pPlayer));
		CHECK_EQUAL(true, TestCollision::IsCheckableOverlappedCol(m_pPlayer, m_pNPC));
	}

	TEST_FIXTURE(FCollision, TestCollision_IsCheckableOverlappedCol_Negative)
	{
		// 공격자가 NPC인데 공중이나 땅 속에 있으면 겹쳐져 있는 것 판정안함
		m_pNPCInfo->nAltitude = AT_SKY;

		CHECK_EQUAL(false, TestCollision::IsCheckableOverlappedCol(m_pNPC, m_pPlayer));


		m_pNPCInfo->nAltitude = AT_UNDERGROUND;

		CHECK_EQUAL(false, TestCollision::IsCheckableOverlappedCol(m_pNPC, m_pPlayer));

	}

}


