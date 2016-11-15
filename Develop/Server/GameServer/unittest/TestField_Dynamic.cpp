#include "stdafx.h"
#include "GUTHelper.h"
#include "GNPCInfo.h"
#include "MockField.h"
#include "GUTHelper.h"
#include "GEntityPlayer.h"
#include "GTestWrapper.h"
#include "GFieldSystem.h"
#include "GUnitTestUtil.h"
#include "GDynamicFieldMaster.h"
#include "GEntityNPC.h"
#include "GDynamicField.h"
#include "GParty.h"
#include "GConst.h"
#include "GSoulBindingInfo.h"
#include "GSoulBindingInfoMgr.h"
#include "GCommandTable.h"
#include "GServer.h"
#include "GFieldMgr.h"
#include "GPlayerObjectManager.h"
#include "GQuestInfoMgr.h"
#include "GSharedField.h"
#include "GFieldInfoMgr.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "GDynamicFieldPolicy.h"
#include "GPlayerField.h"
#include "GQuestSystem.h"
#include "FBaseFieldInfo.h"
#include "GPartyActiveFieldGroupStorage.h"
#include "GPlayerFieldDynamic.h"
#include "GUTHelper_Party.h"
#include "GProxyFieldManager.h"
#include "GMasterServerFacade.h"
#include "GTestForward.h"
#include "GRebirthSystem.h"
#include "GPlayerSystem.h"
#include "FBasePlayer.h"
#include "GQuestInfo.h"
#include "FBaseMockLink.h"
#include "GQObjectiveInfo.h"
#include "GQuestCompleter.h"
#include "GQuestCanceler.h"
#include "GQuestFailer.h"
#include "GQObjUpdater.h"


const int TEST_FIELD_GROUP_ID = 10000;
const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

class TestServerSys : public GServer
{
public:
	virtual MUID	NewUID()
	{
		return SUnitTestUtil::NewUID();
	}
};

SUITE(Field_Dynamic)
{
	struct FFieldDynamic : FBaseFieldInfo, FBasePlayer
	{
		FFieldDynamic()
		{
			InitTestNPC();
			InitTestField();

			InitTestSharedFieldInfos();
			InitTestDynamicFieldInfos();
		}
		virtual ~FFieldDynamic()
		{
			m_pField->Update(1.0f);
			m_pField->Destroy();
			SAFE_DELETE(m_pField);
		}
		void InitTestNPC()
		{
			m_pNPCInfo = new GNPCInfo;
			m_pNPCInfo->nID = 1;
			gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);
		}
		void InitTestField()
		{
			m_pFieldInfo =AddFieldInfo(100, 50000.0f, 50000.0f);
			m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);				
		}
		void InitTestSharedFieldInfos()
		{
			for (int i=100; i<=102; i++)
			{
				// 공유 필드
				{
					GFieldInfo* pFieldInfo = AddFieldInfo(i, 50000.0f, 50000.0f);
					pFieldInfo->m_bDynamic = false;
					pFieldInfo->m_nDefaultChannelCount = 0;
				}
			}
		}
		void InitTestDynamicFieldInfos()
		{
			FIELD_GROUP_INFO* info = new FIELD_GROUP_INFO;
			info->nID = TEST_FIELD_GROUP_ID;
			info->setRunningQuest.insert(100);
			bool bStartField = true;
			for (int i=100; i<=102; i++)
			{
				// 동적 필드
				{
					GFieldInfo* pFieldInfo = AddFieldInfo(1000+i, 50000.0f, 50000.0f);
					pFieldInfo->m_bDynamic = true;
					pFieldInfo->m_bBoundCheck = true;
					pFieldInfo->m_rtBoundary.left = 0;
					pFieldInfo->m_rtBoundary.top = 0;
					pFieldInfo->m_rtBoundary.right = 50000.0f;
					pFieldInfo->m_rtBoundary.bottom = 50000.0f;
					MARKER_INFO marker(1, pFieldInfo->m_nFieldID, vec3(1000, 1000, 1000));
					pFieldInfo->InsertMarkerInfo(marker);

					info->vecFieldInfo.push_back(pFieldInfo);

					if (true == bStartField)
					{
						info->pStartFieldInfo = pFieldInfo;
						info->pStartMarkerInfo = pFieldInfo->FindMarker(1);
						bStartField = false;
					}
				}

			}

			gmgr.pFieldInfoMgr->InsertGroup(info);
		}

		void EnterDynamicField(GEntityPlayer* pPlayer, int nDestFieldGroupID, int nDestFieldID, vec3 vDestPos = vec3(1000,1000,1000))
		{
			VALID(pPlayer);

			GField* pSource = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);
			MARKER_INFO markerStart;
			markerStart.nFieldID = pSource->GetID();
			pPlayer->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);
			if (!pPlayer->GetPlayerField().GateToTrial(nDestFieldGroupID))
			{
				CHECK(FALSE && "동적 필드 생성 실패"); 
				return;
			}

			pPlayer->GetPlayerField().Gate(nDestFieldID, vDestPos, vec3(0,1,0), false);
			gmgr.pFieldMgr->Update(0.0f);
			pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GNPCInfo*		m_pNPCInfo;
		GFieldInfo*		m_pFieldInfo;

		GTestMgrWrapper<GFieldMgr>					m_FieldMgr;
//		GTestSysWrapper2<GServer, TestServerSys>	m_ServerSys;
		GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgr;
		DECLWRAPPER_NPCMgr;
	};


TEST_FIXTURE(FFieldDynamic, Create)
{
	GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
	if (!pPlayer->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}
	
	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer);
	CHECK_EQUAL(pFieldGroup->GetQty(), 3);
	CHECK(pFieldGroup->Get(1100) != NULL);
	CHECK(pFieldGroup->Get(1101) != NULL);
	CHECK(pFieldGroup->Get(1102) != NULL);
}

TEST_FIXTURE(FFieldDynamic, CreateDifferentGroup)
{
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
	if (!pPlayer2->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	CHECK(pPlayer1->GetPlayerField().GetDynamicFieldGroupUID() != 
		  pPlayer2->GetPlayerField().GetDynamicFieldGroupUID());
}

TEST_FIXTURE(FFieldDynamic, DestroyByCommand)
{
	GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
	if (!pPlayer->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer);
	MUID uidGroup = pFieldGroup->GetUID();

	CHECK_EQUAL(gmgr.pFieldMgr->GetFieldQty(), 3);
	pFieldGroup->DeleteMe();
	gsys.pServer->Update(0.1f);
	CHECK_EQUAL(gmgr.pFieldMgr->GetFieldQty(), 0);
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidGroup), (GDynamicFieldMaster*)NULL);
}

TEST_FIXTURE(FFieldDynamic, DontGateToNotCreatedDynamicField)
{
	GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);	// 동적 필드를 만든 플레이어

	CHECK_EQUAL(pPlayer->GetField(), m_pField);
	pPlayer->GetPlayerField().Gate(1101, vec3(1000, 1000, 1000), vec3(0,1,0), false);	// 동적 필드로 이동 시도
	gsys.pServer->Update(0.1f);
	pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	CHECK_EQUAL(pPlayer->GetField(), m_pField); // 이동 실패
}

TEST_FIXTURE(FFieldDynamic, WarpSharedToDynamic)
{
	GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);	// 동적 필드를 만든 플레이어

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	if (!pPlayer->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer);
	GDynamicField* pField1100 = pFieldGroup->Get(1100);
	GDynamicField* pField1101 = pFieldGroup->Get(1101);
	GDynamicField* pField1102 = pFieldGroup->Get(1102);

	CHECK_EQUAL((GField*)NULL, pPlayer->GetField());
	gsys.pServer->Update(0.1f);
	pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	CHECK_EQUAL(pField1100, pPlayer->GetField());
}

TEST_FIXTURE(FFieldDynamic, WarpDynamicToDynamic)
{
	GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);	
	
	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	if (!pPlayer->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer);
	GDynamicField* pField1100 = pFieldGroup->Get(1100);
	GDynamicField* pField1101 = pFieldGroup->Get(1101);
	GDynamicField* pField1102 = pFieldGroup->Get(1102);
	GEntityPlayer* pMovePlayer = GUTHelper::NewEntityPlayer(pField1100, vCommonPos, MUID(0,0), markerStart);	// 동적 필드간 이동을 하는 플레이어
	pMovePlayer->GetPlayerField().SetDynamicFieldGroupUID(pPlayer->GetPlayerField().GetDynamicFieldGroupUID());		// 동적 필드 공유

	CHECK_EQUAL(pMovePlayer->GetField(), pField1100);
	pMovePlayer->GetPlayerField().Gate(1101, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gsys.pServer->Update(0.1f);
	pMovePlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	CHECK_EQUAL(pMovePlayer->GetField(), pField1101);
}

TEST_FIXTURE(FFieldDynamic, WarpDynamicToShared)
{
	GSharedField* pSharedField100 = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);
	GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	if (!pPlayer->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer);
	GDynamicField* pField1100 = pFieldGroup->Get(1100);
	GDynamicField* pField1101 = pFieldGroup->Get(1101);
	GDynamicField* pField1102 = pFieldGroup->Get(1102);
	GEntityPlayer* pMovePlayer = GUTHelper::NewEntityPlayer(pField1100, vCommonPos, MUID(0,0), markerStart);	// 동적 필드간 이동을 하는 플레이어
	pMovePlayer->GetPlayerField().SetDynamicFieldGroupUID(pPlayer->GetPlayerField().GetDynamicFieldGroupUID());		// 동적 필드 공유
	pMovePlayer->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	CHECK_EQUAL(pMovePlayer->GetField(), pField1100);
	pMovePlayer->GetPlayerField().Gate(100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gsys.pServer->Update(0.1f);
	pMovePlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	CHECK_EQUAL(pMovePlayer->GetField(), pSharedField100);
}

TEST_FIXTURE(FFieldDynamic, SpawnRunningWhenDynamicFieldStating)
{
	GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

	FIELD_GROUP_INFO* pGroupInfo = new FIELD_GROUP_INFO;
	pGroupInfo->nID = 10001;
	GFieldInfo* pFieldInfo = new GFieldInfo;
	pFieldInfo->m_bDynamic = true;
	pFieldInfo->m_nFieldID = 789;
	pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
	MARKER_INFO marker(1, pFieldInfo->m_nFieldID, vec3(1000, 1000, 1000));
	pFieldInfo->InsertMarkerInfo(marker);

	SPAWN_INFO info;
	info.nID = 200;
	info.vPoint = vec3(1000, 1000, 1000);
	info.bCheckSpawnPointValidation = false;
	SPAWN_NPC_INFO infoNPC;
	infoNPC.nID = 1;
	infoNPC.nPickRate = MAX_SPAWN_RATE;
	info.vecNPCInfoList.push_back(infoNPC);
	pFieldInfo->InsertSpawnInfo(info);

	pGroupInfo->pStartFieldInfo = pFieldInfo;
	pGroupInfo->pStartMarkerInfo = pFieldInfo->FindMarker(1);
	pGroupInfo->vecFieldInfo.push_back(pFieldInfo);

	gmgr.pFieldInfoMgr->Insert(pFieldInfo);
	gmgr.pFieldInfoMgr->InsertGroup(pGroupInfo);

	if (!pPlayer->GetPlayerField().GateToTrial(10001))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer);
	if (!pFieldGroup)	{ CHECK(FALSE && "동적 필드 생성 실패"); return; }

	GDynamicField* pField1000 = pFieldGroup->Get(789);
	if (!pField1000)	{ CHECK(FALSE && "동적 필드 생성 실패"); return; }

	pField1000->Update(1.0f); // 스폰을 위한 틱
	CHECK_EQUAL(pField1000->GetEntityQty(ETID_NPC),1);
}

TEST_FIXTURE(FFieldDynamic, SynchFieldGroupWhenUpdatedPartyMember)
{
	GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>	FakeSystemWrapper;

	MockPlayerObject* pPlayerObject = new MockPlayerObject(SUnitTestUtil::NewUID());
	pPlayerObject->Create();
	gmgr.pPlayerObjectManager->AddPlayer(pPlayerObject);
	GEntityPlayer* pPlayer1 = pPlayerObject->GetEntity();
	pPlayer1->EnterToField(m_pField, vec3(0.0f, 0.0f, 0.0f));
	GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);	
	GProxyFieldManager* pProxyFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
	pProxyFieldManager->AddFieldGroup(pFieldGroup->GetUID(), pFieldGroup->GetInfo(), 1, DFT_TRIAL);
	
	GParty* pParty = GUTHelper_Party::CreateParty(pPlayer1, pPlayer2);

	MUID uidFieldGroup;	
	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);	
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup), pFieldGroup);

	GUTHelper_Party::DoOfflineMember(pParty, pPlayer1);
	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);
		
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup), (GDynamicFieldMaster*)NULL);
	gmgr.pPlayerObjectManager->DeleteAllPlayer();
}

TEST_FIXTURE(FFieldDynamic, SynchFieldGroupWhenFieldEnterAndLeave)
{
	GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>	FakeSystemWrapper;

	GField* pField = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);

	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
	GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	GParty* pParty = GUTHelper_Party::CreateParty(pPlayer1, pPlayer2);

	MUID uidFieldGroup;

	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup), (GDynamicFieldMaster*)NULL);

	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gsys.pServer->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

		
	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);	
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup), pFieldGroup);

	// Warp to shared-field
	pPlayer1->GetPlayerField().Gate(100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gsys.pServer->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);	
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup), pFieldGroup);
	gsys.pSystem->Sleep((float)GConst::DYNAMICFIELD_EXPIRED_TIME);
	gmgr.pFieldMgr->Update((float)GConst::DYNAMICFIELD_EXPIRED_TIME);
	gmgr.pFieldMgr->Update(0.1f);	// self-destruction tick

	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup), (GDynamicFieldMaster*)NULL);
}

TEST_FIXTURE(FFieldDynamic, ShareFieldGroupInParty)
{
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
	GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);
	pPlayer2->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	GParty* pParty = GUTHelper_Party::CreateParty(pPlayer1, pPlayer2);

	MUID uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);	
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup), (GDynamicFieldMaster*)NULL);

	pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gsys.pServer->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	pPlayer2->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID);

	pPlayer2->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gsys.pServer->Update(0.1f);
	pPlayer2->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer1->GetPlayerField().GetDynamicFieldGroupUID(), pPlayer2->GetPlayerField().GetDynamicFieldGroupUID());
}

TEST_FIXTURE(FFieldDynamic, ExpiredByPartyQuest_NotHaveActiveQuest)
{
	GTestMgrWrapper<GQuestInfoMgr>							QuestInfoMgr;
	GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>	FakeSystemWrapper;

	GField* pField = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);	
	GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);
	
	GParty* pParty = GUTHelper_Party::CreateParty(pPlayer1, pPlayer2);
	
	 // ==== When not have active quest ===
	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}
	gsys.pServer->Update(0.1f);

	MUID uidFieldGroup;
	GDynamicFieldMaster* pFieldGroup;
	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);	
	CHECK(NULL != gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup));

	pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);

	MARKER_INFO markerExit;
	markerExit.nFieldID = 100;
	markerExit.vPoint = vec3(0,0,0);
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerExit);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gsys.pServer->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);	
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup), pFieldGroup);

	// Dynamic-field expired due not have active quest
	gmgr.pFieldMgr->Update(0.1f); // Set Expire Trigger
	gsys.pSystem->Sleep((float)GConst::DYNAMICFIELD_EXPIRED_TIME);
	gmgr.pFieldMgr->Update((float)GConst::DYNAMICFIELD_EXPIRED_TIME);

	// Warp To shared-field
	gsys.pServer->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	
	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);
	CHECK_EQUAL((GDynamicFieldMaster*)NULL, gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup));
}

TEST_FIXTURE(FFieldDynamic, ExpiredByPartyQuest_GiveQuestAndFailed)
{
	const float INFINITY_TIME = 1000000;

	GTestMgrWrapper<GQuestInfoMgr>							QuestInfoMgr;
	GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>	FakeSystemWrapper;

	GField* pField = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);	
	GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	GParty* pParty = GUTHelper_Party::CreateParty(pPlayer1, pPlayer2);

	MUID uidFieldGroup;
	GDynamicFieldMaster* pFieldGroup;

	// ==== When Give Quest and failed ===
	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup), pFieldGroup);

	// Active Quest Recvieve
	GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer1, 100);
	pPlayerQuest->GetInfo()->nTrialFieldGroupID = pFieldGroup->GetInfo()->nID;
	
	// Dynamic-field is still alive
	gmgr.pFieldMgr->Update(0.1f); // Set Expire Trigger
	gsys.pSystem->Sleep(INFINITY_TIME);
	gmgr.pFieldMgr->Update(INFINITY_TIME);

	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);	
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup), pFieldGroup);

	// Quest Failed
	gsys.pQuestSystem->GetFailer().Fail(pPlayer1, pPlayerQuest->GetInfo()->nID);
	pPlayer1->GetQuests().Cancel(pPlayerQuest->GetInfo()->nID);

	// Dynamic-field expired
	gmgr.pFieldMgr->Update(0.1f); // Set Expire Trigger
	gsys.pSystem->Sleep((float)GConst::DYNAMICFIELD_EXPIRED_TIME);
	gmgr.pFieldMgr->Update((float)GConst::DYNAMICFIELD_EXPIRED_TIME);

	// Warp To shared-field
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);	
	CHECK_EQUAL((GDynamicFieldMaster*)NULL, gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup));
}

TEST_FIXTURE(FFieldDynamic, ExpiredByPartyQuest_GiveQuestAndDrop)
{
	const float INFINITY_TIME = 1000000;

	GTestMgrWrapper<GQuestInfoMgr>							QuestInfoMgr;
	GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>	FakeSystemWrapper;

	GField* pField = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);	
	GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	GParty* pParty = GUTHelper_Party::CreateParty(pPlayer1, pPlayer2);

	MUID uidFieldGroup;
	GDynamicFieldMaster* pFieldGroup;

	// ==== When Give Quest and drop ===
	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	// Active Quest Recvieve
	GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer1, 100);

	// Dynamic-field is still alive
	gmgr.pFieldMgr->Update(0.1f); // Set Expire Trigger
	gsys.pSystem->Sleep(INFINITY_TIME);
	gmgr.pFieldMgr->Update(INFINITY_TIME);

	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);	
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup), pFieldGroup);

	// Quest Drop
	gsys.pQuestSystem->GetCanceler().Cancel(pPlayer1, pPlayerQuest->GetInfo()->nID);
	
	// Dynamic-field expired
	gmgr.pFieldMgr->Update(0.1f); // Set Expire Trigger
	gsys.pSystem->Sleep((float)GConst::DYNAMICFIELD_EXPIRED_TIME);
	gmgr.pFieldMgr->Update((float)GConst::DYNAMICFIELD_EXPIRED_TIME);

	// Warp To shared-field
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	
	uidFieldGroup = pParty->FindField(TEST_FIELD_GROUP_ID);	
	CHECK_EQUAL((GDynamicFieldMaster*)NULL, gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup));
}

TEST_FIXTURE(FFieldDynamic, ExpiredByIndividualQuest)
{
	const float INFINITY_TIME = 1000000;

	GTestMgrWrapper<GQuestInfoMgr>							QuestInfoMgr;
	GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>	FakeSystemWrapper;

	GField* pField100 = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(pField100, vCommonPos);	

	// ==== When not have active quest ===
	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}
	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	GDynamicField* pField1100 = pFieldGroup->Get(1100);

	MARKER_INFO markerExit;
	markerExit.nFieldID = 100;
	markerExit.vPoint = vec3(0,0,0);
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerExit);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);

	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	CHECK_EQUAL(pField1100, pPlayer1->GetField());

	// Dynamic-field expired due not have active quest
	gmgr.pFieldMgr->Update(0.1f); // Set Expire Trigger
	gsys.pSystem->Sleep((float)GConst::DYNAMICFIELD_EXPIRED_TIME);
	gmgr.pFieldMgr->Update((float)GConst::DYNAMICFIELD_EXPIRED_TIME);

	// Warp To shared-field
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	CHECK_EQUAL(pPlayer1->GetField(), pField100);


	// ==== When Give Quest and failed ===
	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	pField1100 = pFieldGroup->Get(1100);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	CHECK_EQUAL(pField1100, pPlayer1->GetField());

	// Active Quest Recvieve
	GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer1, 100);

	// Dynamic-field is still alive
	gmgr.pFieldMgr->Update(0.1f); // Set Expire Trigger
	gsys.pSystem->Sleep(INFINITY_TIME);
	gmgr.pFieldMgr->Update(INFINITY_TIME);
	CHECK_EQUAL(pField1100, pPlayer1->GetField());

	// Quest Failed
	gsys.pQuestSystem->GetFailer().Fail(pPlayer1, pPlayerQuest->GetInfo()->nID);
	pPlayer1->GetQuests().Cancel(pPlayerQuest->GetInfo()->nID);

	// Dynamic-field expired
	gmgr.pFieldMgr->Update(0.1f); // Set Expire Trigger
	gsys.pSystem->Sleep((float)GConst::DYNAMICFIELD_EXPIRED_TIME);
	gmgr.pFieldMgr->Update((float)GConst::DYNAMICFIELD_EXPIRED_TIME);

	// Warp To shared-field
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	CHECK_EQUAL(pField100, pPlayer1->GetField());



	// ==== When Give Quest and drop ===
	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	pField1100 = pFieldGroup->Get(1100);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	// Active Quest Recvieve
	pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer1, 100);
	pPlayerQuest->GetInfo()->nTrialFieldGroupID = pFieldGroup->GetInfo()->nID;

	// Dynamic-field is still alive
	gmgr.pFieldMgr->Update(0.1f); // Set Expire Trigger
	gsys.pSystem->Sleep(INFINITY_TIME);
	gmgr.pFieldMgr->Update(INFINITY_TIME);
	CHECK_EQUAL(pPlayer1->GetField()->GetUID(), pField1100->GetUID());

	// Quest Drop
	gsys.pQuestSystem->GetCanceler().Cancel(pPlayer1, pPlayerQuest->GetInfo()->nID);

	// Dynamic-field expired
	gmgr.pFieldMgr->Update(0.1f); // Set Expire Trigger
	gsys.pSystem->Sleep((float)GConst::DYNAMICFIELD_EXPIRED_TIME);
	gmgr.pFieldMgr->Update((float)GConst::DYNAMICFIELD_EXPIRED_TIME);

	// Warp To shared-field
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	CHECK_EQUAL(pPlayer1->GetField(), pField100);
}

TEST_FIXTURE(FFieldDynamic, IfTwiceEnterToFieldGroupThenPlaceAtSame)
{
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);	
	MARKER_INFO markerExit;
	markerExit.nFieldID = 100;
	markerExit.vPoint = vec3(0,0,0);
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerExit);

	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup2 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pFieldGroup1->GetUID(), pFieldGroup2->GetUID());
}

TEST_FIXTURE(FFieldDynamic, DynamicfieldMaintainByOhterPartyMemberEntered)
{
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);	
	GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);	
	
	GField* pField = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);
	pPlayer2->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	GParty* pParty = GUTHelper_Party::CreateParty(pPlayer1, pPlayer2);

	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}
	GDynamicFieldMaster* pFieldGroup1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();	

	gmgr.pFieldMgr->Update(0.1f);	
	CHECK(pFieldGroup1->GetPolicy()->IsExpiredCounting());

	if (!pPlayer2->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}
	GDynamicFieldMaster* pFieldGroup2 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer2);

	pPlayer2->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer2->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pFieldGroup1->GetUID(), pFieldGroup2->GetUID());

	GPlayerQuest* pPlayerQuest = NULL;

	pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer2, 100);
	gmgr.pFieldMgr->Update(0.1f);
	CHECK(!pFieldGroup1->GetPolicy()->IsExpiredCounting());	

	gsys.pQuestSystem->GetCanceler().Cancel(pPlayer2, 100);
	gmgr.pFieldMgr->Update(0.1f);
	CHECK(pFieldGroup1->GetPolicy()->IsExpiredCounting());

	pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer2, 100);
	gmgr.pFieldMgr->Update(0.1f);
	CHECK(!pFieldGroup1->GetPolicy()->IsExpiredCounting());

	pPlayer2->GetPlayerField().Gate(100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer2->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	gmgr.pFieldMgr->Update(0.1f);
	CHECK(pFieldGroup1->GetPolicy()->IsExpiredCounting());
}

TEST_FIXTURE(FFieldDynamic, Checkpoint_Save)
{
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);	

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	GFieldInfo* pFieldInfo1100 = gmgr.pFieldInfoMgr->Find(1100);
	if (!pFieldInfo1100) { CHECK(FALSE && "1100번 필드정보 없음"); return; }

	{
		SENSOR_INFO sensorCheckpoint1;
		sensorCheckpoint1.nID = 70;
		sensorCheckpoint1.vPoint = vec3(2000, 2000, 2000);
		sensorCheckpoint1.fRadius = 500;
		sensorCheckpoint1.nCheckpointMarkerID = 50;
		pFieldInfo1100->InsertSensor(sensorCheckpoint1);
	}

	vec3 vCheckpoint1 = vec3(4000,4000,4000);
	{
		MARKER_INFO markerCheckpoint1;
		markerCheckpoint1.nID = 50;
		markerCheckpoint1.nFieldID = 1100;
		markerCheckpoint1.vPoint = vCheckpoint1;
		pFieldInfo1100->InsertMarkerInfo(markerCheckpoint1);
	}
	
	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer1->GetPlayerField().GetFieldDynamic().GetCheckpoint().IsInvalid(), true);
	
	pPlayer1->SetPos(vec3(2000, 2000, 2000));
	pPlayer1->EnterSensor(70);

	CHECK_EQUAL(pPlayer1->GetPlayerField().GetFieldDynamic().GetCheckpoint().vPoint.IsEqual(vCheckpoint1), true);	// 체크포인트 갱신
}

TEST_FIXTURE(FFieldDynamic, Checkpoint_WarpBindingpointWhenDeadWithHaventCheckpoint)
{
	GTestMgrWrapper<GSoulBindingInfoMgr>	m_SoulBindingInfoMgrWrapper;
	GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;

	GField* pField = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);
	GEntityPlayer* pPlayer = test.player.NewPlayer(pField);

	// 소울 바인딩 등록
	MARKER_INFO* pMarkerSoulbinding = GUTHelper_Field::InsertNewMarkerInfo(m_pFieldInfo);
	GSoulBindingInfo* pSoulBindingInfo = m_Helper.NewSoulBindingInfo(pMarkerSoulbinding);
	gsys.pPlayerSystem->SaveSoulBinding(pPlayer, pSoulBindingInfo->m_nID);

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	// 트라이얼필드 이동
	if (!pPlayer->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer);
	GDynamicField* pField1100 = pFieldGroup1->Get(1100);

	pPlayer->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer->GetFieldUID(), pField1100->GetUID());

	// 안전지대 등록없이 죽음
	pPlayer->doDie();
	gsys.pRebirthSystem->RebirthToCheckPoint(pPlayer);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(m_pFieldInfo->m_nFieldID, pPlayer->GetFieldID());	// 소울바인딩 위치에서 부활
}

TEST_FIXTURE(FFieldDynamic, Checkpoint_WarpBindingpointWhenDeadWithCheckpoint)
{
	GTestMgrWrapper<GSoulBindingInfoMgr>	m_SoulBindingInfoMgrWrapper;
	GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectMgrWrapper;

	MARKER_INFO markerSoulbinding;
	markerSoulbinding.nID = 50;
	markerSoulbinding.nFieldID = 100;
	markerSoulbinding.vPoint = vec3(100,100,100);
	m_pFieldInfo->InsertMarkerInfo(markerSoulbinding);

	// 소울바인딩 등록
	MockPlayerObject* m_pPlayerObject = new MockPlayerObject(SUnitTestUtil::NewUID());
	m_pPlayerObject->Create();
	gmgr.pPlayerObjectManager->AddPlayer(m_pPlayerObject);
	GEntityPlayer* pPlayer1 = m_pPlayerObject->GetEntity();
	pPlayer1->EnterToField(m_pField, vec3(0,0,0));

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	GSoulBindingInfo* m_pSoulBindingInfo = new GSoulBindingInfo();
	m_pSoulBindingInfo->m_nID = 1;
	m_pSoulBindingInfo->m_pMarkerInfo = &markerSoulbinding;
	gmgr.pSoulBindingInfoMgr->Insert(m_pSoulBindingInfo);

	gsys.pPlayerSystem->SaveSoulBinding(pPlayer1, m_pSoulBindingInfo->m_nID);

	// 센서&마커 등록
	GFieldInfo* pFieldInfo1100 = gmgr.pFieldInfoMgr->Find(1100);
	if (!pFieldInfo1100) { CHECK(FALSE && "1100번 필드정보 없음"); return; }

	{
		SENSOR_INFO sensorCheckpoint1;
		sensorCheckpoint1.nID = 70;
		sensorCheckpoint1.vPoint = vec3(2000, 2000, 2000);
		sensorCheckpoint1.fRadius = 500;
		sensorCheckpoint1.nCheckpointMarkerID = 50;
		pFieldInfo1100->InsertSensor(sensorCheckpoint1);

		SENSOR_INFO sensorCheckpoint2;
		sensorCheckpoint2.nID = 71;
		sensorCheckpoint2.vPoint = vec3(3000, 3000, 3000);
		sensorCheckpoint2.fRadius = 500;
		sensorCheckpoint2.nCheckpointMarkerID = 51;
		pFieldInfo1100->InsertSensor(sensorCheckpoint2);
	}

	vec3 vCheckpoint1 = vec3(4000,4000,4000);
	vec3 vCheckpoint2 = vec3(8000,8000,8000);
	{
		MARKER_INFO markerCheckpoint1;
		markerCheckpoint1.nID = 50;
		markerCheckpoint1.nFieldID = 1100;
		markerCheckpoint1.vPoint = vCheckpoint1;
		pFieldInfo1100->InsertMarkerInfo(markerCheckpoint1);

		MARKER_INFO markerCheckpoint2;
		markerCheckpoint2.nID = 51;
		markerCheckpoint2.nFieldID = 1100;
		markerCheckpoint2.vPoint = vCheckpoint2;
		pFieldInfo1100->InsertMarkerInfo(markerCheckpoint2);
	}

	// 트라이얼필드 이동
	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	GDynamicField* pField1100 = pFieldGroup1->Get(1100);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer1->GetFieldUID(), pField1100->GetUID());
	CHECK_EQUAL(pPlayer1->GetPos().IsEqual(vec3(1000,1000,1000)), true);

	// 안전지대 등록후 죽음
	pPlayer1->SetPos(vec3(2000, 2000, 2000));
	pPlayer1->EnterSensor(70);
	pPlayer1->doDie();
	gsys.pRebirthSystem->RebirthToCheckPoint(pPlayer1);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer1->GetFieldUID(), pField1100->GetUID());
	CHECK_EQUAL(pPlayer1->GetPos().IsEqual(vCheckpoint1), true);

	// 다시 한번 죽음
	pPlayer1->doDie();
	gsys.pRebirthSystem->RebirthToCheckPoint(pPlayer1);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer1->GetFieldUID(), pField1100->GetUID());
	CHECK_EQUAL(pPlayer1->GetPos().IsEqual(vCheckpoint1), true);

	// 다른 안전지대에 등록후 죽음
	pPlayer1->SetPos(vec3(3000, 3000, 3000));
	pPlayer1->EnterSensor(71);
	pPlayer1->doDie();
	gsys.pRebirthSystem->RebirthToCheckPoint(pPlayer1);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer1->GetFieldUID(), pField1100->GetUID());
	CHECK_EQUAL(pPlayer1->GetPos().IsEqual(vCheckpoint2), true);

	pField1100->Destroy();
}

TEST_FIXTURE(FFieldDynamic, DynamicfieldExpiredWhenDisconnectedDuringFieldLoading)
{
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);	

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);
	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	
	CHECK_EQUAL(pFieldGroup1->HasPreservedEnterPlayers(), true);

	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pFieldGroup1->HasPreservedEnterPlayers(), false);

	// 로딩 완료가 안되었음
	pPlayer1->GetPlayerField().Gate(1101, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	
	CHECK_EQUAL(pFieldGroup1->HasPreservedEnterPlayers(), true);

	gmgr.pPlayerObjectManager->DeletePlayer(pPlayer1->GetUID());

	CHECK_EQUAL(pFieldGroup1->HasPreservedEnterPlayers(), false);
}

TEST_FIXTURE(FFieldDynamic, DynamicfieldStayWhenDropQuestByPlayer)
{
	GField* pField100 = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(pField100, vCommonPos);	

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);
	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	GDynamicField* pField1100 = pFieldGroup1->Get(1100);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer1->GetField()->GetUID(), pField1100->GetUID());
	
	GPlayerQuest* pPlayerQuest = test.quest.GiveNewPlayerQuest(pPlayer1, 100);
	pPlayerQuest->GetInfo()->nTrialFieldGroupID = pFieldGroup1->GetInfo()->nID;		// Set trial-field
	gsys.pQuestSystem->GetCanceler().Cancel(pPlayer1, 100);


	// 퀘스트 포기해도 필드에 잔류할 수 있다.
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer1->GetField()->GetUID(), pField1100->GetUID());
}

TEST_FIXTURE(FFieldDynamic, DynamicfieldStayWhenDropQuestByPartymember)
{
	GField* pField100 = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(pField100, vCommonPos);	
	GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(pField100, vCommonPos);	

	GParty* pParty = GUTHelper_Party::CreateParty(pPlayer1, pPlayer2);

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);
	pPlayer2->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);
	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}
	if (!pPlayer2->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	GDynamicFieldMaster* pFieldGroup2 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer2);
	GDynamicField* pField1100 = pFieldGroup1->Get(1100);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	pPlayer2->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer2->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer1->GetField()->GetUID(), pField1100->GetUID());
	CHECK_EQUAL(pPlayer2->GetField()->GetUID(), pField1100->GetUID());
	
	GPlayerQuest* pPlayerQuest1 = test.quest.GiveNewPlayerQuest(pPlayer1, 100);
	pPlayerQuest1->GetInfo()->nTrialFieldGroupID = pFieldGroup1->GetInfo()->nID;		// Set trial-field
	GPlayerQuest* pPlayerQuest2 = test.quest.GiveNewPlayerQuest(pPlayer2, 100);
	pPlayerQuest2->GetInfo()->nTrialFieldGroupID = pFieldGroup2->GetInfo()->nID;		// Set trial-field

	// 한명만 트라이얼 퀘스트 포기
	gsys.pQuestSystem->GetCanceler().Cancel(pPlayer1, 100);

	// 퀘스트 포기해도 필드에 잔류할 수 있다.
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer1->GetField()->GetUID(), pField1100->GetUID());

	if (!gmgr.pFieldMgr->GetDynamicFieldMaster(pFieldGroup1->GetUID()))	{ CHECK(FALSE && "필드그룹이 이미 삭제되었음"); return; }
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(pFieldGroup1->GetUID())->GetUID(), pFieldGroup1->GetUID());	// 필드그룹 남아있음

	// 모두가 트라이얼 퀘스트 포기	
	gsys.pQuestSystem->GetCanceler().Cancel(pPlayer2, 100);
	
	// 퀘스트 포기해도 필드에 잔류할 수 있다.
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer2->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer2->GetField()->GetUID(), pField1100->GetUID());
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(pFieldGroup1->GetUID())->GetUID(), pFieldGroup1->GetUID());	// 필드그룹 남아있음
}

TEST_FIXTURE(FFieldDynamic, DynamicfieldStayWhenDropQuestByRunner)
{
	GField* pField100 = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(pField100, vCommonPos);	
	GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(pField100, vCommonPos);	
	GEntityPlayer* pPlayer3 = GUTHelper::NewEntityPlayer(pField100, vCommonPos);	

	GParty* pParty = GUTHelper_Party::CreateParty(pPlayer1);
	GUTHelper_Party::AddMember(pParty, pPlayer2);
	GUTHelper_Party::AddMember(pParty, pPlayer3);

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);
	pPlayer2->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);
	pPlayer3->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}
	if (!pPlayer2->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}
	if (!pPlayer3->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}
	GDynamicFieldMaster* pFieldGroup1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	GDynamicFieldMaster* pFieldGroup2 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer2);
	GDynamicFieldMaster* pFieldGroup3 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer3);
	
	GDynamicField* pField1100 = pFieldGroup1->Get(1100);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	pPlayer2->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer2->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	pPlayer3->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer3->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer1->GetField()->GetUID(), pField1100->GetUID());

	// 1, 2번 플레이어 퀘스트 받음
	GPlayerQuest* pPlayerQuest1 = test.quest.GiveNewPlayerQuest(pPlayer1, 100);
	pPlayerQuest1->GetInfo()->nTrialFieldGroupID = pFieldGroup1->GetInfo()->nID;		// Set trial-field
	GPlayerQuest* pPlayerQuest2 = test.quest.GiveNewPlayerQuest(pPlayer2, 100);
	pPlayerQuest2->GetInfo()->nTrialFieldGroupID = pFieldGroup2->GetInfo()->nID;		// Set trial-field
	
	// 1번 플레이어 퀘스트 포기
	gsys.pQuestSystem->GetCanceler().Cancel(pPlayer1, 100);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	pPlayer3->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	CHECK_EQUAL(pPlayer3->GetField()->GetUID(), pField1100->GetUID());	// 아직 동적필드에 있음

	// 2번 플레이어 퀘스트 포기
	gsys.pQuestSystem->GetCanceler().Cancel(pPlayer2, 100);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer2->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	pPlayer3->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	CHECK_EQUAL(pPlayer3->GetField()->GetUID(), pField1100->GetUID());

	CHECK_EQUAL(pPlayer1->GetField()->GetUID(), pField1100->GetUID());	
	CHECK_EQUAL(pPlayer2->GetField()->GetUID(), pField1100->GetUID());
}

TEST_FIXTURE(FFieldDynamic, DynamicfieldResetWhenCompleted)
{
	GField* pField100 = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(pField100, vCommonPos);	
	GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(pField100, vCommonPos);	
	GEntityPlayer* pPlayer3 = GUTHelper::NewEntityPlayer(pField100, vCommonPos);	

	GParty* pParty = GUTHelper_Party::CreateParty(pPlayer1);
	GUTHelper_Party::AddMember(pParty, pPlayer2);
	GUTHelper_Party::AddMember(pParty, pPlayer3);

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);
	pPlayer2->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);
	pPlayer3->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);

	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}
	if (!pPlayer2->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}
	if (!pPlayer3->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}
	GDynamicFieldMaster* pFieldGroup1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	GDynamicFieldMaster* pFieldGroup2 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer2);
	GDynamicFieldMaster* pFieldGroup3 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer3);
	GDynamicField* pField1100 = pFieldGroup1->Get(1100);

	MUID uidFieldGroup = pFieldGroup1->GetUID();

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	pPlayer2->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer2->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	pPlayer3->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer3->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer1->GetField()->GetUID(), pField1100->GetUID());

	// 1, 2번 플레이어 퀘스트 받음
	GPlayerQuest* pPlayerQuest1 = test.quest.GiveNewPlayerQuest(pPlayer1, 100);
	pPlayerQuest1->GetInfo()->nTrialFieldGroupID = pFieldGroup1->GetInfo()->nID;		// Set trial-field
	GPlayerQuest* pPlayerQuest2 = test.quest.GiveNewPlayerQuest(pPlayer2, 100);
	pPlayerQuest2->GetInfo()->nTrialFieldGroupID = pFieldGroup2->GetInfo()->nID;		// Set trial-field

	// 1번 플레이어 퀘스트 complete
	gsys.pQuestSystem->GetCompleter().Complete(pPlayer1, 100);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	pPlayer3->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup), pFieldGroup1);
	CHECK_EQUAL(pPlayer1->GetField()->GetUID(), pField1100->GetUID());	// 아직 동적필드에 있음
	CHECK_EQUAL(pPlayer2->GetField()->GetUID(), pField1100->GetUID());	// 아직 동적필드에 있음
	CHECK_EQUAL(pPlayer3->GetField()->GetUID(), pField1100->GetUID());	// 아직 동적필드에 있음

	// all leave
	pPlayer1->GetPlayerField().Gate(100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	pPlayer2->GetPlayerField().Gate(100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer2->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	pPlayer3->GetPlayerField().Gate(100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer3->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	gmgr.pFieldMgr->Update(0.1f);	// self destructor begin
	gmgr.pFieldMgr->Update(0.1f);	// delete fieldgroup
	CHECK_EQUAL(gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup), (GDynamicFieldMaster*)NULL);	// 필드그룹 소멸
}

TEST_FIXTURE(FFieldDynamic, TF_BoundaryCheck)
{
	GField* pField100 = GUTHelper_Field::NewSharedField(m_pFieldInfo, 1);
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(pField100, vCommonPos);	

	MARKER_INFO markerStart;
	markerStart.nFieldID = 100;
	pPlayer1->GetPlayerField().GetFieldDynamic().SetEnterPos(markerStart);
	if (!pPlayer1->GetPlayerField().GateToTrial(TEST_FIELD_GROUP_ID))
	{
		CHECK(FALSE && "동적 필드 생성 실패"); 
		return;
	}

	GDynamicFieldMaster* pFieldGroup1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	GDynamicField* pField1100 = pFieldGroup1->Get(1100);

	pPlayer1->GetPlayerField().Gate(1100, vec3(1000, 1000, 1000), vec3(0,1,0), false);
	gmgr.pFieldMgr->Update(0.1f);
	pPlayer1->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

	CHECK_EQUAL(pPlayer1->GetField()->GetUID(), pField1100->GetUID());

	// 섹터 0,0 으로 이동
	pPlayer1->SetPos(vec3(0,0,0)); 
	CHECK_EQUAL(0, pPlayer1->GetSectorX());
	CHECK_EQUAL(0, pPlayer1->GetSectorY());
	CHECK(NULL != gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1));
	gmgr.pFieldMgr->Update(60.0f); // 만료 제한시간 지남
	gmgr.pFieldMgr->Update(0.0f);// self destructor 작동
	gmgr.pFieldMgr->Update(0.0f);// self destructor가 소멸시킴
	CHECK(NULL == gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1));
}

TEST_FIXTURE(FFieldDynamic, TF_IsNowCombat)
{
	// 필드 진입
	MockField* pField = GUTHelper_Field::DefaultMockField();
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(pField, vCommonPos);	
	EnterDynamicField(pPlayer1, TEST_FIELD_GROUP_ID, 1100);

	// 확인
	GDynamicFieldMaster* pFieldGroup1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	TVALID(pFieldGroup1);
	CHECK_EQUAL(false, pFieldGroup1->IsNowCombat());
	GEntityNPC*	pEnemy = pField->SpawnTestNPC(m_pNPCInfo);
	MakeCombat(pPlayer1, pEnemy);
	CHECK_EQUAL(true, pFieldGroup1->IsNowCombat());
}

struct FChallengerQuest : FFieldDynamic, FBaseMockLink
{
	FChallengerQuest()
	{
		m_pNPCInfo1 = m_Helper.NewNPCInfo();

		m_pQuestInfo = test.quest.NewQuestInfo();
		m_pQuestInfo->bChallengerQuest = true;
		m_pQObejctive = new GQObjectiveInfo();
		m_pQObejctive->nID = 1;
		m_pQObejctive->nQuestID = m_pQuestInfo->nID;
		m_pQObejctive->nType = QOT_DESTROY;
		m_pQObejctive->vecParam1.push_back(m_pNPCInfo1->nID);
		m_pQObejctive->nParam2 = 1;
		gmgr.pQuestInfoMgr->MakeQOInfoCache_Destroy(m_pNPCInfo1->nID, m_pQObejctive);

		m_pQuestInfo->Insert(m_pQObejctive);

		m_pNPC = test.npc.SpawnNPC(m_pField, m_pNPCInfo1->nID);
	}
	virtual ~FChallengerQuest()
	{
		
	}

	GNPCInfo*			m_pNPCInfo1;
	GQuestInfo*			m_pQuestInfo;
	GQObjectiveInfo*	m_pQObejctive;
	GEntityNPC*			m_pNPC;
};

TEST_FIXTURE(FChallengerQuest, TF_ChallengerQuestComplete)
{
	// 필드 진입
	MockField* pField = GUTHelper_Field::DefaultMockField();
	GEntityPlayer* pPlayer1 = GUTHelper::NewEntityPlayer(pField, vCommonPos);	
	EnterDynamicField(pPlayer1, TEST_FIELD_GROUP_ID, 1100);
	GDynamicFieldMaster* pFieldGroup1 = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer1);
	TVALID(pFieldGroup1);
	pFieldGroup1->GetInfo()->bChallengerQuest = true;

	// 퀘스트 완료
	MockLink* pLink = NewLink(pPlayer1);
	pLink->IgnoreAll();
	pLink->AddAllowedID(MC_CHALLENGER_QUEST_COMPLETE);
	pPlayer1->GetQuests().Insert(0, m_pQuestInfo->nID, L"", L"");
	gsys.pQuestSystem->GetQObjUpdater().UpdDestroyQObj(pPlayer1, m_pNPCInfo1->nID, m_pQObejctive->nParam2, m_pNPC);

	CHECK_EQUAL(1, pLink->GetCommandCount());
	CHECK_EQUAL(MC_CHALLENGER_QUEST_COMPLETE, pLink->GetCommand(0).GetID());
}

} // SUITE
