#include "stdafx.h"
#include "GEntityMgr.h"
#include "GUTHelper_Field.h"
#include "MockField.h"
#include "FBaseNPC.h"

SUITE(EntityMgr)
{
	class FEntityMgr : public FBaseNPC
	{
	public:
		FEntityMgr() 
		{
			m_pField = GUTHelper_Field::DefaultMockField();

			m_pNPCInfo = NewNPCInfo(m_nNPCID);

			m_pGatherNPCInfo[GATHER_MINE] = NewNPCInfo(2);
			m_pGatherNPCInfo[GATHER_HERB] = NewNPCInfo(3);
			m_pGatherNPCInfo[GATHER_RELIC] = NewNPCInfo(4);
			m_pGatherNPCInfo[GATHER_LUMBER] = NewNPCInfo(5);

			for (int i = 0; i< MAX_GATHER_TYPE; i++)
			{
				m_pGatherNPCInfo[i]->nNpcType = NPC_TYPE_GATHER;
				m_pGatherNPCInfo[i]->nGatherType = GATHER_TYPE(i);
			}

			m_EntityMgr.Create();
		}
		virtual ~FEntityMgr()
		{
			m_pField->Destroy();

			m_EntityMgr.Destroy();
		}

		GEntityNPC* NewAndCreateEntityNPC(GNPCInfo* pNPCInfo)
		{
			MUID uidNPC = SUnitTestUtil::NewUID();
			GEntityNPC* pEntityNPC = new GEntityNPC();
			pEntityNPC->Create(uidNPC);

			pEntityNPC->InitFromNPCInfo(pNPCInfo, NULL);

			return pEntityNPC;
		}

		MockField*	m_pField;
		GNPCInfo*	m_pNPCInfo;
		GNPCInfo*	m_pGatherNPCInfo[MAX_GATHER_TYPE];

		static const int	m_nNPCID = 1;

		GEntityMgr	m_EntityMgr;
	};

	TEST_FIXTURE(FEntityMgr, EntityMgr_AddNPC_AssignUIID)
	{

		MUID uidNPC = SUnitTestUtil::NewUID();
		GEntityNPC* pEntityNPC = new GEntityNPC();
		pEntityNPC->Create(uidNPC);


		CHECK_EQUAL(UIID_INVALID, pEntityNPC->GetUIID());
		CHECK_EQUAL(uidNPC, pEntityNPC->GetUID());


		m_EntityMgr.AddEntity(pEntityNPC);

		// npc니까 10001부터
		CHECK_EQUAL(NPC_UIID_SEED+1, pEntityNPC->GetUIID());
	}

	TEST_FIXTURE(FEntityMgr, EntityMgr_AddNPC_AssignUIID2)
	{
		MUID uidNPC1 = SUnitTestUtil::NewUID();
		MUID uidNPC2 = SUnitTestUtil::NewUID();
		MUID uidNPC3 = SUnitTestUtil::NewUID();

		GEntityNPC* pEntityNPC1 = new GEntityNPC();
		GEntityNPC* pEntityNPC2 = new GEntityNPC();
		GEntityNPC* pEntityNPC3 = new GEntityNPC();

		pEntityNPC1->Create(uidNPC1);
		pEntityNPC2->Create(uidNPC2);
		pEntityNPC3->Create(uidNPC3);

		CHECK_EQUAL(UIID_INVALID, pEntityNPC1->GetUIID());
		CHECK_EQUAL(UIID_INVALID, pEntityNPC2->GetUIID());
		CHECK_EQUAL(UIID_INVALID, pEntityNPC3->GetUIID());

		m_EntityMgr.AddEntity(pEntityNPC1);
		CHECK_EQUAL(NPC_UIID_SEED+1, pEntityNPC1->GetUIID());

		m_EntityMgr.AddEntity(pEntityNPC2);
		CHECK_EQUAL(NPC_UIID_SEED+2, pEntityNPC2->GetUIID());

		pEntityNPC2->Destroy();
		m_EntityMgr.RemoveEntity(pEntityNPC2, true);


		m_EntityMgr.AddEntity(pEntityNPC3);

		// NPC2를 Remove하고 UIID를 재활용하므로 2가 맞음
		CHECK_EQUAL(NPC_UIID_SEED+2, pEntityNPC3->GetUIID());	
	}


	TEST_FIXTURE(FEntityMgr, EntityMgr_FindNPC_UIID)
	{
		MUID uidNPC1 = SUnitTestUtil::NewUID();
		MUID uidNPC2 = SUnitTestUtil::NewUID();
		MUID uidNPC3 = SUnitTestUtil::NewUID();

		GEntityNPC* pEntityNPC1 = new GEntityNPC();
		GEntityNPC* pEntityNPC2 = new GEntityNPC();
		GEntityNPC* pEntityNPC3 = new GEntityNPC();

		pEntityNPC1->Create(uidNPC1);
		pEntityNPC2->Create(uidNPC2);
		pEntityNPC3->Create(uidNPC3);

		m_EntityMgr.AddEntity(pEntityNPC1);
		m_EntityMgr.AddEntity(pEntityNPC2);
		m_EntityMgr.AddEntity(pEntityNPC3);

		const UIID nNPC_UIID1 = pEntityNPC1->GetUIID();
		const UIID nNPC_UIID2 = pEntityNPC2->GetUIID();
		const UIID nNPC_UIID3 = pEntityNPC3->GetUIID();

		GEntityNPC* pFind = m_EntityMgr.FindNPC(nNPC_UIID1);
		ASSERT_CHECK(pFind != NULL);
		CHECK_EQUAL(nNPC_UIID1, pFind->GetUIID());
		CHECK(pFind == pEntityNPC1);

		pFind = m_EntityMgr.FindNPC(nNPC_UIID2);
		ASSERT_CHECK(pFind != NULL);
		CHECK_EQUAL(nNPC_UIID2, pFind->GetUIID());
		CHECK(pFind == pEntityNPC2);

		pFind = m_EntityMgr.FindNPC(nNPC_UIID3);
		ASSERT_CHECK(pFind != NULL);
		CHECK_EQUAL(nNPC_UIID3, pFind->GetUIID());
		CHECK(pFind == pEntityNPC3);
	}

	TEST_FIXTURE(FEntityMgr, TestGatherEntityCounter)
	{
		GGatherEntityCounter counter;
		CHECK_EQUAL(0, counter.GetCount(GATHER_MINE));

		counter.Inc(m_pGatherNPCInfo[GATHER_HERB]);
		CHECK_EQUAL(1, counter.GetCount(GATHER_HERB));
		CHECK_EQUAL(0, counter.GetCount(GATHER_MINE));

		counter.Dec(m_pGatherNPCInfo[GATHER_HERB]);
		CHECK_EQUAL(0, counter.GetCount(GATHER_HERB));
		CHECK_EQUAL(0, counter.GetCount(GATHER_MINE));
	}

	TEST_FIXTURE(FEntityMgr, TestGatherEntityCounter_Negative)
	{
		GGatherEntityCounter counter;
		CHECK_EQUAL(0, counter.GetCount(GATHER_MINE));
		CHECK_EQUAL(0, counter.GetCount(GATHER_HERB));
		CHECK_EQUAL(0, counter.GetCount(GATHER_RELIC));
		CHECK_EQUAL(0, counter.GetCount(GATHER_LUMBER));

		// 채집 엔티티가 아니면 카운팅하지 않는다.
		counter.Inc(m_pNPCInfo);
		CHECK_EQUAL(0, counter.GetCount(GATHER_MINE));
		CHECK_EQUAL(0, counter.GetCount(GATHER_HERB));
		CHECK_EQUAL(0, counter.GetCount(GATHER_RELIC));
		CHECK_EQUAL(0, counter.GetCount(GATHER_LUMBER));

		// 카운트가 0 이하로 안떨어진다.
		counter.Dec(m_pGatherNPCInfo[GATHER_HERB]);
		counter.Dec(m_pGatherNPCInfo[GATHER_HERB]);
		counter.Dec(m_pGatherNPCInfo[GATHER_HERB]);

		CHECK_EQUAL(0, counter.GetCount(GATHER_MINE));
		CHECK_EQUAL(0, counter.GetCount(GATHER_HERB));
		CHECK_EQUAL(0, counter.GetCount(GATHER_RELIC));
		CHECK_EQUAL(0, counter.GetCount(GATHER_LUMBER));
	}

	TEST_FIXTURE(FEntityMgr, EntityMgr_GetGatherEntityCount)
	{
		GEntityNPC* pGatherMine = NewAndCreateEntityNPC(m_pGatherNPCInfo[GATHER_MINE]);
		m_EntityMgr.AddEntity(pGatherMine);

		MUID uidMine = pGatherMine->GetUID();

		CHECK_EQUAL(1, m_EntityMgr.GetEntityQty(ETID_NPC));

		CHECK_EQUAL(1, m_EntityMgr.GetGatherEntityCount(GATHER_MINE));
		CHECK_EQUAL(0, m_EntityMgr.GetGatherEntityCount(GATHER_HERB));
		CHECK_EQUAL(0, m_EntityMgr.GetGatherEntityCount(GATHER_RELIC));
		CHECK_EQUAL(0, m_EntityMgr.GetGatherEntityCount(GATHER_LUMBER));

		// mine 제거
		pGatherMine->Destroy();
		m_EntityMgr.RemoveEntity(pGatherMine, true);

		CHECK_EQUAL(0, m_EntityMgr.GetGatherEntityCount(GATHER_MINE));
		CHECK_EQUAL(0, m_EntityMgr.GetGatherEntityCount(GATHER_HERB));
		CHECK_EQUAL(0, m_EntityMgr.GetGatherEntityCount(GATHER_RELIC));
		CHECK_EQUAL(0, m_EntityMgr.GetGatherEntityCount(GATHER_LUMBER));
		
	}
}