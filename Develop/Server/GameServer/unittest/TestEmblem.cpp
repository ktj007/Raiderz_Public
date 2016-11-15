#include "StdAfx.h"
#include "GTestWrapper.h"
#include "SUnitTestUtil.h"
#include "GUTHelper.h"
#include "GTestForward.h"
#include "GEmblemInfoMgr.h"
#include "CSEmblemInfo.h"
#include "GConditionsInfo.h"
#include "GConditionsInfoMgr.h"
#include "GConditionInfo.h"
#include "FBasePlayer.h"
#include "FBaseNPC.h"
#include "GPlayerEmblem.h"
#include "GHateTable.h"

SUITE(Emblem)
{
	const int EMBLEM_ID = 123;
	const int CONDITION_ID = 111;
	const int NPC_ID = 777;

	struct FEmblem : public FBasePlayer, public FBaseNPC
	{
		FEmblem()
		{ 
			m_pField = GUTHelper_Field::DefaultMockField();
		}

		~FEmblem() 
		{
			m_pField->Destroy();
		}

		CSEmblemInfo* CreateEmblemInfo(int nID, int nConditionID)
		{
			CSEmblemInfo* pEmblem = new CSEmblemInfo();
			pEmblem->nID = nID;
			pEmblem->nConditionID = nConditionID;
			gmgr.pEmblemInfoMgr->Insert(pEmblem);
			return pEmblem;
		}

		GCondition_Leaf* CreateConditionInfo(int nID)
		{
			GCondition_Leaf* pNewConditionInfo = m_Helper.NewConditionLeaf(m_Helper.NewConditionsInfo(nID));
			return pNewConditionInfo;
		}

		GCondition_Leaf* CreateKillCondition(int nID, int NPCID)
		{
			GCondition_Leaf* pNewConditionInfo = CreateConditionInfo(nID);
			pNewConditionInfo->m_nCT = CT_KILL;
			pNewConditionInfo->m_vecParam1.push_back(NPCID);
			return pNewConditionInfo;
		}

		void CookEmblemMgr()
		{
			gmgr.pEmblemInfoMgr->OnLoadCompleted();
		}

		void CreateKillEmblem()
		{
			CreateEmblemInfo(EMBLEM_ID, CONDITION_ID);
			CreateKillCondition(CONDITION_ID, NPC_ID);
			CookEmblemMgr();
		}

		MockField*		m_pField;
		GUTHelper		m_Helper;
		GTestMgrWrapper<GConditionsInfoMgr>	_conditionsinfo_mgr;
		GTestMgrWrapper<GEmblemInfoMgr>	_emblem_mgr;
	};

	TEST_FIXTURE(FEmblem, ClassifyKillEmblem)
	{
		CreateEmblemInfo(EMBLEM_ID, CONDITION_ID);
		CreateKillCondition(CONDITION_ID, NPC_ID);

		vector<const CSEmblemInfo*> vecEmblems;
		vecEmblems = gmgr.pEmblemInfoMgr->GetKillEmblem(NPC_ID);
		CHECK_EQUAL(0U, vecEmblems.size());
		CookEmblemMgr();
		vecEmblems = gmgr.pEmblemInfoMgr->GetKillEmblem(NPC_ID);
		CHECK_EQUAL(1U, vecEmblems.size());
	}

	TEST_FIXTURE(FEmblem, ObtainKillEmblem)
	{
		CreateKillEmblem();
		GEntityPlayer* pPlayer = NewEntityPlayer(m_pField);
		GEntityNPC* pNPC = NewNPC(m_pField, NPC_ID);

		CHECK_EQUAL(false, pPlayer->GetEmblem().CheckEmblem(EMBLEM_ID));
		CHECK_EQUAL(false, pPlayer->GetEmblem().HasKilled(NPC_ID));
		pNPC->GetHateTable().AddPoint_FoundEnemy(pPlayer);
		pNPC->doDie();
		CHECK_EQUAL(true, pPlayer->GetEmblem().CheckEmblem(EMBLEM_ID));
		CHECK_EQUAL(true, pPlayer->GetEmblem().HasKilled(NPC_ID));
	}
}