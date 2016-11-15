#include "StdAfx.h"
#include "GUTHelper_Field.h"
#include "MockField.h"
#include "GFieldMgr.h"
#include "GFieldFactory.h"
#include "SUnitTestUtil.h"
#include "GFieldInfoMgr.h"
#include "GServer.h"
#include "GTestForward.h"

MockField* GUTHelper_Field::DefaultMockField()
{
	MockField* pField = DefaultMockField::Get();
	pField->Create();
	return pField;
}

GFieldInfo* GUTHelper_Field::NewFieldInfo(int nID)
{
	if (nID == INVALID_ID)
	{
		nID = SUnitTestUtil::NewID();
	}

	GFieldInfo* pNewInfo = new GFieldInfo();
	pNewInfo->m_nDefaultChannelCount = 0;
	pNewInfo->m_nFieldID = nID;
	pNewInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);

	gmgr.pFieldInfoMgr->Insert(pNewInfo);

	return pNewInfo;
}

FIELD_GROUP_INFO* GUTHelper_Field::NewFieldGroupInfo(GFieldInfo* pFieldInfo, int nGroupID)
{
	if (NULL == pFieldInfo)
	{
		pFieldInfo = NewFieldInfo();
	}

	const MARKER_INFO* pNewMarkerInfo = InsertNewMarkerInfo(pFieldInfo);

	if (0 == nGroupID)
	{
		nGroupID = SUnitTestUtil::NewID();
	}

	FIELD_GROUP_INFO* pNewInfo = new FIELD_GROUP_INFO();
	pNewInfo->nID = nGroupID;
	pNewInfo->pStartFieldInfo = pFieldInfo;
	pNewInfo->vecFieldInfo.push_back(pFieldInfo);
	pNewInfo->pStartMarkerInfo = pNewMarkerInfo;

	gmgr.pFieldInfoMgr->InsertGroup(pNewInfo);

	return pNewInfo;	
}

MARKER_INFO* GUTHelper_Field::InsertNewMarkerInfo(const GFieldInfo* pFieldInfo, uint32 nMarkerID)
{
	if (NULL == pFieldInfo) return NULL;

	if (0 == nMarkerID)
	{
		nMarkerID = SUnitTestUtil::NewUINT32ID();
	}

	MARKER_INFO markerInfo;
	markerInfo.nID = nMarkerID;
	markerInfo.nFieldID = pFieldInfo->m_nFieldID;
	markerInfo.vPoint = vec3(1000, 1000, 1000);

	const_cast<GFieldInfo*>(pFieldInfo)->InsertMarkerInfo(markerInfo);

	return const_cast<MARKER_INFO*>(pFieldInfo->FindMarker(markerInfo.nID));
}

SENSOR_INFO* GUTHelper_Field::InsertNewSensorInfo( const GFieldInfo* pFieldInfo, uint32 nSensorID)
{
	if (NULL == pFieldInfo) return NULL;

	if (0 == nSensorID)
	{
		nSensorID = SUnitTestUtil::NewUINT32ID();
	}

	SENSOR_INFO sensorInfo;
	sensorInfo.nID = nSensorID;
	const_cast<GFieldInfo*>(pFieldInfo)->InsertSensor(sensorInfo);

	return const_cast<SENSOR_INFO*>(pFieldInfo->FindSensor(sensorInfo.nID));
}

SPAWN_INFO* GUTHelper_Field::InsertNewSpawnInfo( const GFieldInfo* pFieldInfo, SPAWN_ID nSpawnID, int nNPCID)
{
	if (NULL == pFieldInfo) return NULL;

	if (0 == nSpawnID)
	{
		nSpawnID = SUnitTestUtil::NewUINT32ID();
	}

	GNPCInfo* pNewNPCInfo = test.npc.NewNPCInfo(nNPCID);

	SPAWN_INFO spawnInfo;
	spawnInfo.nID = nSpawnID;
	spawnInfo.vecNPCInfoList.push_back(pNewNPCInfo->nID);
	const_cast<GFieldInfo*>(pFieldInfo)->InsertSpawnInfo(spawnInfo);

	return const_cast<SPAWN_INFO*>(pFieldInfo->FindSpawnInfo(spawnInfo.nID));
}


GSharedField* GUTHelper_Field::NewSharedField( GFieldInfo* pFieldInfo, int nChannelID/*=1*/ )
{
	if (!pFieldInfo)
	{
		pFieldInfo = NewFieldInfo();
		pFieldInfo->m_bDynamic = false;
	}

	if (gmgr.pFieldMgr->GetSharedFieldMaster(pFieldInfo->m_nFieldID) == NULL)
	{
		gmgr.pFieldMgr->GetFactory()->CreateSharedFieldMaster(pFieldInfo);
	}

	return gmgr.pFieldMgr->GetFactory()->CreateSharedField(gsys.pServer->NewUID(), pFieldInfo->m_nFieldID, nChannelID);
}


GDynamicFieldMaster* GUTHelper_Field::NewTrialFieldMaster(FIELD_GROUP_INFO* pFieldGroupInfo)
{
	if (NULL == pFieldGroupInfo)
	{
		pFieldGroupInfo = NewFieldGroupInfo();
		const_cast<GFieldInfo*>(pFieldGroupInfo->pStartFieldInfo)->m_bDynamic = true;
	}	

	return gmgr.pFieldMgr->CreateDynamicFieldMaster(pFieldGroupInfo->nID, DFT_TRIAL, MUID::Invalid());	
}

GDynamicFieldMaster* GUTHelper_Field::NewBattleArenaFieldMaster(FIELD_GROUP_INFO* pFieldGroupInfo)
{
	if (NULL == pFieldGroupInfo)
	{
		pFieldGroupInfo = NewFieldGroupInfo();
		const_cast<GFieldInfo*>(pFieldGroupInfo->pStartFieldInfo)->m_bDynamic = true;
	}	

	return gmgr.pFieldMgr->CreateDynamicFieldMaster(pFieldGroupInfo->nID, DFT_BATTLEARENA, MUID::Invalid());
}

MockField* GUTHelper_Field::NewMockField(GFieldInfo* pFieldInfo)
{
	MUID uidField = SUnitTestUtil::NewUID();
	MockField* pField = new MockField(uidField);
	if (pFieldInfo)
	{
		pField->SetFieldInfo(pFieldInfo);
	}
	pField->Create();
	return pField;
}

void GUTHelper_Field::NewMockFieldAndPlayer(MockField*& pNewField, GEntityPlayer*& pNewEntityPlayer, vec3 vPlayerPos /*= vec3::ZERO*/, MUID uidNewPlayer/*=MUID(0,0)*/)
{
	pNewField = NewMockField();
	pNewEntityPlayer = test.player.NewPlayer(pNewField, vPlayerPos, uidNewPlayer);
}