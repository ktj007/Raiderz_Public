#include "stdafx.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "SUnitTestUtil.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "GPlayerObjectManager.h"
#include "GUTHelper_Field.h"

SUITE(Entity)
{
	TEST(EntityReferenceCounter)
	{
		GEntityMgr	entity_mgr;
		GEntityPlayer* pEntityPlayer = new GEntityPlayer();
		
		CHECK_EQUAL(pEntityPlayer->GetReferenceCount(), 1);

		pEntityPlayer->Create(SUnitTestUtil::NewUID());		
		
		// Player의 경우 EntityMgr에서 PlayerObjectManager로 destroy를 위임한다.
		{
			MUID uidPlayer = pEntityPlayer->GetUID();
			MockPlayerObject* pPlayer = new MockPlayerObject(uidPlayer);
			pPlayer->Create(pEntityPlayer);
			gmgr.pPlayerObjectManager->AddPlayer(pPlayer);
		}
	
		entity_mgr.AddEntity(pEntityPlayer);

		CHECK_EQUAL(entity_mgr.GetEntityQty(ETID_PLAYER), 1);

		pEntityPlayer->AddRef();
		CHECK_EQUAL(pEntityPlayer->GetReferenceCount(), TFT_SIZE+2);

		pEntityPlayer->DeleteMe();

		// 아직 mgr에서 update를 안했으니 2
		CHECK_EQUAL(pEntityPlayer->GetReferenceCount(), TFT_SIZE+2);

		entity_mgr.Update(1.0f);

		// update에서 한번 Drop을 했으니 1
		CHECK_EQUAL(pEntityPlayer->GetReferenceCount(), 1);

		// mgr에선 이미 지웠다.
		CHECK_EQUAL(entity_mgr.GetEntityQty(ETID_PLAYER), 0);

		// 이 시점에서 실제로 delete 됨.
		pEntityPlayer->Drop();
	}

	TEST(NPCQtyInEntityMgr)
	{
		MockField* m_pField = GUTHelper_Field::NewMockField();

		GNPCInfo	infoNPC1;
		infoNPC1.nID = 1;
		GNPCInfo	infoNPC2;
		infoNPC2.nID = 2;

		CHECK_EQUAL(m_pField->GetNPCQty(1), 0);
		CHECK_EQUAL(m_pField->GetNPCQty(2), 0);

		GEntity* pFirst=NULL;
		GEntity* pSecond=NULL;
		{
			GEntityNPC* pNPC = m_pField->SpawnTestNPC(&infoNPC1);
			pFirst = pNPC;
		}

		CHECK_EQUAL(m_pField->GetNPCQty(1), 1);
		CHECK_EQUAL(m_pField->GetNPCQty(2), 0);

		{
			GEntityNPC* pNPC = m_pField->SpawnTestNPC(&infoNPC1);
			pSecond = pNPC;
		}
		
		CHECK_EQUAL(m_pField->GetNPCQty(1), 2);
		CHECK_EQUAL(m_pField->GetNPCQty(2), 0);

		{
			GEntityNPC* pNPC = m_pField->SpawnTestNPC(&infoNPC2);
		}

		CHECK_EQUAL(m_pField->GetNPCQty(1), 2);
		CHECK_EQUAL(m_pField->GetNPCQty(2), 1);
		
		{
			m_pField->RemoveEntity(pFirst);
		}

		CHECK_EQUAL(m_pField->GetNPCQty(1), 1);
		CHECK_EQUAL(m_pField->GetNPCQty(2), 1);

		{
			m_pField->RemoveEntity(pSecond);
		}

		CHECK_EQUAL(m_pField->GetNPCQty(1), 0);
		CHECK_EQUAL(m_pField->GetNPCQty(2), 1);

		pFirst->Destroy();
		pSecond->Destroy();
		SAFE_DELETE(pFirst);
		SAFE_DELETE(pSecond);
		m_pField->Destroy();
		SAFE_DELETE(m_pField);
	}

	TEST(IsBackDirection)
	{
		GEntity entity;
		vec3 vecTargetPos;
		
		entity.SetPos(vec3(0.0f, 0.0f, 0.0f));	// 중앙에 서서,
		entity.SetDir(vec3(1.0f, 0.0f, 0.0f));	// x방향을 바라보다.
			

		// 뒤
		vecTargetPos.Set(-1.0f, 0.0f, 0.0f);
		CHECK(entity.IsBack(vecTargetPos));
		
		// 앞
		vecTargetPos.Set(1.f, 0.f, 0.f);
		CHECK(!entity.IsBack(vecTargetPos));

		// 옆 - 정확히 90도 옆은 앞쪽에 포함
		vecTargetPos.Set(0.f, 1.0f, 0.f);
		CHECK(!entity.IsBack(vecTargetPos));

		// 대각선 뒤
		vecTargetPos.Set(-1.f, 1.0f, 0.f);
		CHECK(entity.IsBack(vecTargetPos));
	}
} // SUITE(AI)
