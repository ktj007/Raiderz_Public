#include "stdafx.h"
#include "SUnitTest.h"
#include "GDef.h"
#include "GTalentInfo.h"
#include "GTalent.h"
#include "GNPCInfo.h"
#include "GNPCInfoMgr.h"
#include "GNPCMgr.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "MockField.h"
#include "SUnitTestUtil.h"
#include "GGlobal.h"


// 잘못들어가 있는 NPC 정보들이 있는지 테스트
TEST3(TestNPCInfo)
{
	GNPCInfoMgr* pNPCInfoMgr = gmgr.pNPCInfoMgr;

	int nNPCInfoSize = static_cast<int>(pNPCInfoMgr->size());

	for (GNPCInfoMgr::iterator itorNPC = pNPCInfoMgr->begin(); itorNPC != pNPCInfoMgr->end(); ++itorNPC)
	{
		GNPCInfo* pNPCInfo = static_cast<GNPCInfo*>((*itorNPC).second);
		
		// MODE
		size_t nModeSize = pNPCInfo->vecModes.size();
		for (size_t i = 0; i < nModeSize; i++)
		{
			GNPCInfo* pModeNPCInfo = static_cast<GNPCInfo*>(pNPCInfo->vecModes[i]);
			CHECK(pModeNPCInfo->nMode-1 == i);
			CHECK(pModeNPCInfo->nID == pNPCInfo->nID);
		}


	}

}

