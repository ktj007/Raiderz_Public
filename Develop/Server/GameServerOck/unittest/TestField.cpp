#include "stdafx.h"
#include "GFieldInfoMgr.h"
#include "GUTHelper.h"
#include "GNPCInfo.h"
#include "MockField.h"
#include "GUTHelper.h"
#include "GEntityPlayer.h"
#include "GTestWrapper.h"
#include "GFieldSystem.h"
#include "SUnitTestUtil.h"
#include "GEntityNPC.h"
#include "CCommandTable.h"
#include "MockLink.h"
#include "GFieldMgr.h"
#include "FBaseFieldInfo.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"
#include "GPlayerObjectManager.h"
#include "GSharedField.h"

SUITE(Field)
{
	const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
	const vec3 VCommonDir = vec3(0,1,0);

	class FField : public FBaseFieldInfo, public FBaseMockLink
	{
	public:
		FField()
		{
			m_pNPCInfo = new GNPCInfo();
			m_pNPCInfo->nID = 1;
			gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);

			m_pNPCInfo_Huge = new GNPCInfo();
			m_pNPCInfo_Huge->nID = 300;
			m_pNPCInfo_Huge->bHuge = true;
			gmgr.pNPCInfoMgr->Insert(m_pNPCInfo_Huge);

			m_pFieldInfo = AddFieldInfo(100, 50000.0f, 50000.0f);

			m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		}
		virtual ~FField()
		{
			m_pField->Update(1.0f);
			m_pField->Destroy();
			SAFE_DELETE(m_pField);
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GNPCInfo*		m_pNPCInfo;
		GNPCInfo*		m_pNPCInfo_Huge;
		GFieldInfo*		m_pFieldInfo;

		GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
		GTestMgrWrapper<GFieldMgr>					m_FieldMgr;
		GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgr;
	};

	#define SECTOR(X,Y)		vec3(TEST_SECTOR_SIZE*(X),TEST_SECTOR_SIZE*(Y), 0)

	void CheckSectorRouted(MockLink* pLink, int nNPCCounter);

	TEST_FIXTURE(FField, RouteSectorinfoToMe)
	{
		const float TEST_SECTOR_SIZE = SECTOR_SIZE;

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, SECTOR(2,2), VCommonDir);
		MockLink* pLink = NewLink(pPlayer);
		pLink->AddIgnoreID(MC_FIELD_IN_PLAYER);
		pLink->AddIgnoreID(MC_NPCINTERACTION_ICON);
		
		{
			// -prepare-
			pPlayer->SetPos(SECTOR(0,0));

			// BOTTOMRIGHT
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(1,1));	
			CheckSectorRouted(pLink, 1);
		}
		
		{
			// -prepare-
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(2,0));
			CheckSectorRouted(pLink, 0);

			// BOTTOM
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(2,1));
			CheckSectorRouted(pLink, 1);
		}
		
		{
			// -prepare-
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(4,0));
			CheckSectorRouted(pLink, 0);

			// BOTTOMLEFT
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(3,1));
			CheckSectorRouted(pLink, 1);
		}
		
		{
			// -prepare-
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(0,2));
			CheckSectorRouted(pLink, 0);

			// RIGHT
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(1,2));
			CheckSectorRouted(pLink, 1);
		}
		
		{
			// -prepare-
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(4,2));
			CheckSectorRouted(pLink, 0);

			// LEFT
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(3,2));
			CheckSectorRouted(pLink, 1);
		}
		
		{
			// -prepare-
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(0,4));
			CheckSectorRouted(pLink, 0);

			// TOPRIGHT
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(1,3));
			CheckSectorRouted(pLink, 1);
		}

		{
			// -prepare-
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(2,4));
			CheckSectorRouted(pLink, 0);

			// TOP
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(2,3));
			CheckSectorRouted(pLink, 1);
		}
		
		{
			// -prepare-
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(4,4));
			CheckSectorRouted(pLink, 0);

			// TOPLEFT
			pLink->ResetCommands();
			pPlayer->SetPos(SECTOR(3,3));
			CheckSectorRouted(pLink, 1);
		}

		pLink->ResetCommands();
	}

	void CheckSectorRouted(MockLink* pLink, int nNPCCounter)
	{
		minet::MCommand* pCommand = pLink->GetCommand(0).Clone();
		if (!pCommand)
			return;

		CHECK_EQUAL(pLink->GetCommand(0).GetID(), MC_FIELD_SECTOR_ENTITY_INFO);
		MCommandParameter* pParam1 = pCommand->GetParameter(1);
		CHECK_EQUAL(pParam1->GetBlobCount(), nNPCCounter);	// NPC counter
		delete pCommand;
	}

	TEST_FIXTURE(FField, TestCommand_OnEnterAndLeaveField)
	{
		GTestMgrWrapper<GPlayerObjectManager> wrpPlayerObjectManager;

		GPlayerObject* pPrevPlayer = m_Helper.NewMockPlayerObject(m_pField, vec3(1000,1000,0));
		MockLink* pPrevPlayerLink = NewLink(pPrevPlayer->GetEntity());

		GPlayerObject* pTestPlayer = m_Helper.NewMockPlayerObject(m_pField, vec3(1000,1000,0));
		MockLink* pTestPlayerLink = NewLink(pTestPlayer->GetEntity());

		// 접속하면 IN_PLAYER 커맨드 전송
		CHECK_EQUAL(MC_FIELD_IN_PLAYER, pPrevPlayerLink->GetCommand(0).GetID());

		pTestPlayer->GetEntity()->LeaveFromField();

		CHECK_EQUAL(MC_FIELD_OUT_PLAYER, pPrevPlayerLink->GetCommand(1).GetID());
	}

	class FField_UsingGravity : public FField
	{
	public:
		FField_UsingGravity() : FField() 
		{
			m_pField->ExceptToPickingZIsZero(true);
		}
		virtual ~FField_UsingGravity() {}

		GEntityNPC* SpawnForUsingGravityTest(NPC_TYPE nNpcType, vec3 vPos)
		{
			m_pNPCInfo->nNpcType = nNpcType;

			SPAWN_INFO spawnInfo;
			spawnInfo.nFieldID = m_pField->GetID();

			vec3 vDir = vec3::AXISY;

			GEntityNPC* pNPC = m_pField->SpawnNPC(m_pNPCInfo->nID,
				&spawnInfo,
				vPos,
				vDir);

			return pNPC;
		}
	};


	TEST_FIXTURE(FField_UsingGravity, TestSpawnButNPCDontUseGravity)
	{
		// object 타입은 중력의 영향을 안받는다.
		GEntityNPC* pNPC = SpawnForUsingGravityTest(NPC_TYPE_OBJECT, vec3(100.0f, 100.0f, 100.0f));

		vec3 vNPCPos = pNPC->GetPos();
		CHECK_EQUAL(100.0f, vNPCPos.z);
	}

	TEST_FIXTURE(FField_UsingGravity, TestSpawnNPCWhichUseGravity)
	{
		GEntityNPC* pNPC = SpawnForUsingGravityTest(NPC_TYPE_MONSTER, vec3(100.0f, 100.0f, 100.0f));

		vec3 vNPCPos = pNPC->GetPos();
		CHECK_CLOSE(0.0f, vNPCPos.z, 0.001f);
	}

	TEST(TestAllowSectorHeightLimit)
	{
		GFieldInfo info;

		info.m_fSectorHeight = 0.0f;
		CHECK(!info.IsSectorHeightLimitField());
		CHECK(info.IsAllowSectorHeightLimit(100.0f));

		info.m_fSectorHeight = 100.0f;		
		CHECK(info.IsSectorHeightLimitField());
		CHECK(info.IsAllowSectorHeightLimit(50.0f));
		CHECK(info.IsAllowSectorHeightLimit(100.0f));
		CHECK(!info.IsAllowSectorHeightLimit(150.0f));
	}

	TEST_FIXTURE(FField, TestFieldCapacity)
	{
		m_pFieldInfo->m_nCapacity = 10;
		GSharedField* pField = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);
		
		CHECK_EQUAL(10,		pField->GetCapacity());
	}

	TEST_FIXTURE(FField, TestFieldCapacity_FIELDINFO_IS_NULL)
	{
		GSharedField* pField = GUTHelper_Field::NewSharedField(NULL, 1);
//		m_pField->SetInfo(NULL);
		CHECK_EQUAL(CHANNEL_PLAYER_LIMIT,		pField->GetCapacity());
	}

} // SUITE
