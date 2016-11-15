#include "stdafx.h"
#include "GAITest.h"
#include "GNPCInfoMgr.h"
#include "GNPCInfo.h"
#include "GGlobal.h"
#include "GSharedFieldMaster.h"
#include "GConfig_Arg.h"
#include "GConfig.h"
#include "GFieldMgr.h"
#include "GMath.h"
#include "GFieldInfoMgr.h"

void GAITest::OnCreate()
{
	InitInfo();	
	ChangeNPCInfoForTest();

	PickUPField();

	InitFieldTestSpawns();

	InitFieldSpawnInfo();
}

void GAITest::ChangeNPCInfoForTest()
{
	for (GNPCInfoMgr::iterator itor = gmgr.pNPCInfoMgr->begin(); itor != gmgr.pNPCInfoMgr->end(); ++itor)
	{
		GNPCInfo* pNPCInfo = static_cast<GNPCInfo*>((*itor).second);
		
		if (pNPCInfo->nNpcType != NPC_TYPE_MONSTER)
			continue;

		if (pNPCInfo->bRooted)
			continue;
		
		if (pNPCInfo->m_Talents.empty())
			continue;

		if (pNPCInfo->IsBoss())
			continue;
		
		pNPCInfo->nAA = NAAT_ALWAYS;
		pNPCInfo->nInitAttackable = NAT_ALWAYS;
		pNPCInfo->nSightRange = 3000;
		pNPCInfo->m_nLuaSpawnCount = 0;
		
		m_vecNPCIDs.push_back(pNPCInfo->nID);
	}

}

int GAITest::GetRandomNPCID()
{
	if (m_vecNPCIDs.empty()) return 0;
	int index = GMath::RandomNumber(0, (int)m_vecNPCIDs.size()-1);
	return m_vecNPCIDs[index];
}

const SPAWN_INFO* GAITest::GetRandomSpawnInfo() const
{
	const SPAWN_INFO_MAP& mapSpawnInfo = m_pField->GetInfo()->GetSpawnInfo();
	if (mapSpawnInfo.empty()) return NULL;

	int nIndex = GMath::RandomNumber(0, (int)mapSpawnInfo.size()-1);
	for (SPAWN_INFO_MAP::const_iterator itor = mapSpawnInfo.begin(); itor != mapSpawnInfo.end(); ++itor)
	{
		if (nIndex <= 0) return &(*itor).second;
		nIndex--;
	}

	return NULL;
}

void GAITest::OnUpdate(float fDelta)
{	
	if (m_Info.nTestType == AITEST_STRESS_COMBAT_ONE_FIELD) Update_Type1();
	else if (m_Info.nTestType == AITEST_STRESS_COMBAT_ALL_FIELD) Update_Type2();
}

void GAITest::InitFieldTestSpawns()
{
	for (int i = 0; i < 10000;)
	{
		GFieldMgr::FIELD_BY_UID_MAP& field_map = gmgr.pFieldMgr->GetFieldMap();
		GFieldMgr::FIELD_BY_UID_MAP::iterator itField = field_map.begin();
		for (int j=0; j<GMath::RandomNumber(0, field_map.size()-1); ++j)
		{
			++itField;
		}
		GField* pField = itField->second;
		if (!pField->GetInfo())
			continue;

		vec3 vPoint;
		vPoint.x = GMath::RandomNumber(pField->GetInfo()->MinX(), pField->GetInfo()->MaxX());
		vPoint.y = GMath::RandomNumber(pField->GetInfo()->MinY(), pField->GetInfo()->MaxY());
		vPoint.z = 50000;
		if (!pField->PickingZ(vPoint, vPoint))
		{
			//mlog3("invalid test_spawn position, field_id(%d), spawn_id(%d), pos(%.1f %.1f %.1f)\n",
			//	pField->GetID(),
			//	i,
			//	vPoint.x, vPoint.y, vPoint.z);

			continue;
		}
		pField->Spawn(GetRandomNPCID(), vPoint);
		++i;
	}
}


void GAITest::InitFieldSpawnInfo()
{
	for (GFieldInfoMgr::iterator itor = gmgr.pFieldInfoMgr->begin(); itor != gmgr.pFieldInfoMgr->end(); ++itor)
	{
		GFieldInfo* pFieldInfo = (*itor).second;
		pFieldInfo->RemoveSpawnTime();
	}
}

void GAITest::Update_Type1()
{
	if (!m_pField) return;

	int nNPCCount = m_pField->GetEntityQty(ETID_NPC);
	int nNewCount = m_Info.nMaxNPCCount - nNPCCount;

	for (int i = 0; i < nNewCount; i++)
	{
		const SPAWN_INFO* pSpawnInfo = GetRandomSpawnInfo();
		if (pSpawnInfo != NULL)
		{
			int nNPCID = GetRandomNPCID();
			if (nNPCID == 0) continue;
			m_pField->SpawnNPC(nNPCID, pSpawnInfo, pSpawnInfo->vPoint, pSpawnInfo->vDir);
		}
	}
}

void GAITest::Update_Type2()
{
	if (!m_pField) return;

	int nNPCCount = m_pField->GetEntityQty(ETID_NPC);
	int nNewCount = m_Info.nMaxNPCCount - nNPCCount;

	for (int i = 0; i < nNewCount; i++)
	{
		const SPAWN_INFO* pSpawnInfo = GetRandomSpawnInfo();
		if (pSpawnInfo != NULL)
		{
			int nNPCID = GetRandomNPCID();
			if (nNPCID == 0) continue;
			m_pField->SpawnNPC(nNPCID, pSpawnInfo, pSpawnInfo->vPoint, pSpawnInfo->vDir);
		}
	}
}

void GAITest::InitInfo()
{
	m_Info.nTestType = GConfig::m_nAITestType;
	m_Info.nMaxNPCCount = GConfig::m_nNPCCount;
	m_Info.nFieldID = GConfig::m_nFieldID;
}

void GAITest::PickUPField()
{
	GSharedFieldMaster* pSharedField = gmgr.pFieldMgr->GetSharedFieldMaster(m_Info.nFieldID);
	if (pSharedField)
	{
		for (int i = 0; i < 4; i++)
		{
			m_pField = pSharedField->GetChannel(i);
			if (m_pField != NULL) break;
		}
	}

	if (m_Info.nTestType == AITEST_STRESS_COMBAT_ONE_FIELD &&
		m_pField == NULL) mlog("AITestError : no field(%d)\n", m_Info.nFieldID);
}
