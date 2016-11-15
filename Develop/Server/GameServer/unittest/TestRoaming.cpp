#include "stdafx.h"
#include "GUTHelper.h"
#include "MockLink.h"
#include "MockField.h"
#include "GTestWrapper.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GModuleAI.h"
#include "GFieldMgr.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"

SUITE(ROAMING)
{
	struct Fixture : public FBaseMockLink
	{
		Fixture()
		{ 
			m_pFieldInfo = new GFieldInfo();
			m_pFieldInfo->m_strFieldFile = L"mockfield";
			m_pFieldInfo->m_nFieldID = 100;
			m_pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);

			m_pField = GUTHelper_Field::NewMockField();
			m_pField->SetFieldInfo(m_pFieldInfo);

			m_pNPCInfo = new GNPCInfo();
			m_pNPCInfo->nID = 1;
			m_pNPCInfo->fRunSpeed = 100.0f;
			m_pNPCInfo->fSpeed = 100.0f;
			m_pNPCInfo->fRotateSpeed = 0.0f;

			gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);
		}

		~Fixture() 
		{
			SAFE_DELETE(m_pField);
			SAFE_DELETE(m_pFieldInfo);
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GFieldInfo*		m_pFieldInfo;
		GNPCInfo*		m_pNPCInfo;

		GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgr;

	};

	TEST_FIXTURE(Fixture, TestSimpleRoaming)
	{
		// 필드 생성
		// SPAWN_INFO에 로밍 정보 입력
		// NPC 생성
		// 약 10초간 업데이트
		// 보낸 커맨드가 제대로 날라갔는지 체크


		std::auto_ptr<SPAWN_INFO> apSpawninfo(new SPAWN_INFO());
		apSpawninfo->bCheckSpawnPointValidation = false;
		apSpawninfo->vPoint = vec3(1000.0f, 1000.0f, 0.0f);
		apSpawninfo->fRoamRadius = 100;
		apSpawninfo->fRoamDepth = 100;
		apSpawninfo->vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, apSpawninfo->vPoint);
		MockLink* pLink = NewLink(pPlayer);

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, apSpawninfo->vPoint, vec3(0,1,0), apSpawninfo.get());

		for (int i = 0; i < 600; i++)
		{
			m_pField->Update(0.1f);
		}
	}
}