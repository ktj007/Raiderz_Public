#include "stdafx.h"
#include "MockField.h"
#include "GEntityNPC.h"
#include "SUnitTestUtil.h"
#include "GPlayerObjectManager.h"
#include "GHateTable.h"
#include "GNPCFirstStrike.h"

MockField* DefaultMockField::m_pDefaultMockField = NULL;


MockField* DefaultMockField::Get()
{
	if (m_pDefaultMockField == NULL)
	{
		m_pDefaultMockField = new MockField(SUnitTestUtil::NewUID());
	}
	return m_pDefaultMockField;
}

void DefaultMockField::Free()
{
	if (!m_pDefaultMockField) return;

	delete m_pDefaultMockField;
	m_pDefaultMockField = NULL;
}
//////////////////////////////////////////////////////////////////////////////

GFieldInfo MockField::s_FieldInfo;


MockField::MockField( MUID& uid ) : GField(uid, NULL), m_bInitSector(false)
{
	CreateDelegator();
}

MockField::~MockField()
{
	m_pEntityMgr->Destroy();
//	m_SectorMgr.Destroy();
	m_pFieldGrid->Clear();
	SAFE_DELETE(m_pWeatherMgr);
	SAFE_DELETE(m_pDelegator);
	GetSpawn()->Fini();
}

GEntityNPC* MockField::SpawnTestNPC(GNPCInfo* pNPCInfo, const vec3& pos/*=vec3(1, 1, 1)*/, const vec3& dir/*=vec3(1, 1, 1)*/, SPAWN_INFO* pSpawnInfo)
{
	GEntityNPC* pNPC = NULL;

	if (NULL != pSpawnInfo)
	{
		pSpawnInfo->bCheckSpawnPointValidation = false;
		pSpawnInfo->vPoint = pos;
		pSpawnInfo->vDir = dir;
		GetSpawn()->AddSpawn(pSpawnInfo->nID, pSpawnInfo);

		pNPC = GetSpawn()->Spawn(pSpawnInfo->nID);
	}
	else
	{
		pNPC = new GEntityNPC();
		CreateNPCAndEndterField(pNPC, pNPCInfo, pos, dir);
	}

	VALID_RET(pNPC, NULL);

	InitNPCEtc(pNPC);	

	return pNPC;
}

bool MockField::CreateNPCAndEndterField(GEntityNPC* pNPC, GNPCInfo* pNPCInfo, const vec3& pos, const vec3& dir)
{
	pNPC->Create(SUnitTestUtil::NewNPCUID());

	if (pNPC->InitFromNPCInfo(pNPCInfo, NULL, 0) == false)
	{
		delete pNPC;
		return false;
	}

	pNPC->EnterToField(this, pos);
	pNPC->SetDir(dir);
	pNPC->SetFacingDir(dir);

	return true;
}

void MockField::InitNPCEtc( GEntityNPC* pNPC )
{
	pNPC->SetRegenActive(false);
	pNPC->GetHateTable().EnableTimeout(false);
	pNPC->GetNPCFirstStrike().SetEnableSpawnDelay(false);
}

MUID MockField::NewUID()
{
	return SUnitTestUtil::NewUID();
}

void MockField::Destroy()
{
	m_pInfo = &s_FieldInfo;

	// 파티원의 경우 삭제 시, 오브젝트메니저를 참조한다. 잘못된 포인터로 접근을 막기위해 모두 지운다.
	gmgr.pPlayerObjectManager->DeleteAllPlayer();
	m_pEntityMgr->Destroy();
}

void MockField::CreateSectors()
{
	if (m_bInitSector) return;

	//_SectorMgr.Init(m_pInfo, m_UID);
	MUID uidField = GetUID();
	m_pFieldGrid->Init(uidField, m_pInfo);
	
	m_pSpawn->Init();
	InitSensor();

	m_bInitSector = true;
}

bool MockField::Create()
{
	m_pEntityMgr->Clear();

	s_FieldInfo.SetFieldMinXY_ForTest(0.0f, 0.0f);
	s_FieldInfo.SetFieldMaxXY_ForTest(SECTOR_SIZE*12, SECTOR_SIZE*12);

	if (!m_pInfo)
	{
		m_pInfo = &s_FieldInfo;
	}

	CreateSectors();

	return true;
}

bool MockField::PickingZ( vec3& outPos, vec3 vOriginPos, float fPickAddedOriginZ /*= PICKING_ADDED_VALUE_Z*/ )
{
	if (m_bExceptToPickingZIsZero)
	{
		outPos = vOriginPos;
		outPos.z = 0.0f;
	}
	else
	{
		outPos = vOriginPos;
	}

	return true;
}