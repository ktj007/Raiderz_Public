#include "stdafx.h"
#include "FBaseScriptGlue.h"
#include "MockField.h"
#include "GNullGlueNPC.h"
#include "GUTHelper_Field.h"
#include "GNPCInteraction.h"
#include "GFieldInfoMgr.h"
#include "CCommandTable.h"
#include "GTestForward.h"
#include "GFieldNPCSession.h"
#include "GFieldNPCSessionMgr.h"

const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

SUITE(TestScriptFunction_Field)
{
	struct Fixture: public FBaseScriptGlue
	{
		Fixture()
		{ 
			m_pSpawnInfo = NULL;
			m_nInvalidNPCID = -1;

			MWLua::class_<MockField>(WLUA->GetState(), "MockField")		
				.inh<GField>()
				.def(MWLua::constructor<>())
				;
		}

		~Fixture() 
		{
			SAFE_DELETE(m_pSpawnInfo);
		}

		int				m_nInvalidNPCID;
		SPAWN_INFO*		m_pSpawnInfo;		
		int				m_nInvalidSpawnID;		
	};

	TEST_FIXTURE(Fixture, TSFF_GetID)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field)\n"
			"	return Field:GetID()\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		int nRet = 0;

		WCALL1(WLUA, pszTestFuncName, m_pField->GetDelegator(), &nRet);
		CHECK_EQUAL(nRet, m_pField->GetID());
	}

	TEST_FIXTURE(Fixture, TSFF_Spawn)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, NPCID)\n"
			"	return Field:Spawn(NPCID, vec3(0,0,0));\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GGlueNPC* pRet = NULL;

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_nInvalidNPCID, &pRet);
		CHECK_EQUAL(m_pField->GetNPCQty(m_pNPCInfo->nID), 0);		
		CHECK_EQUAL(pRet, GNullGlueNPC::GetNull());
		
		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_pNPCInfo->nID, &pRet);
		CHECK_EQUAL(m_pField->GetNPCQty(m_pNPCInfo->nID), 1);
		CHECK_EQUAL(m_pNPCInfo->nID, pRet->GetID());
	}

	TEST_FIXTURE(Fixture, TSFF_Despawn)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, NPCID)\n"
			"	return Field:Despawn(NPCID, false);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC1 = m_Helper.NewEntityNPC(m_pField, vec3(0, 0, 0), m_pNPCInfo->nID);
		GEntityNPC* pNPC2 = m_Helper.NewEntityNPC(m_pField, vec3(0, 0, 0), m_pNPCInfo->nID);

		GGlueNPC* pRet = NULL;

		CHECK_EQUAL(2, m_pField->GetNPCQty(m_pNPCInfo->nID));
		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_pNPCInfo->nID, WNULL);
		pNPC1->UpdateAI(0.0f);
		pNPC2->UpdateAI(0.0f);
		m_pField->Update(0.0f);
		CHECK_EQUAL(0, m_pField->GetNPCQty(m_pNPCInfo->nID));
		gmgr.pNPCMgr->Clear();
	}

	TEST_FIXTURE(Fixture, TSFF_SpawnLimit)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, NPCID, StartPos, LimitTime)\n"
			"	Field:SpawnLimited(NPCID, StartPos, LimitTime);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		float fLimitTime = 5.0f;

		WCALL4(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_pNPCInfo->nID, vec3(1.0f, 1.0f, 1.0f), fLimitTime, WNULL);
		CHECK_EQUAL(m_pField->GetNPCQty(m_pNPCInfo->nID), 1);		
		m_pField->GetSpawn()->Update(fLimitTime);
		gmgr.pNPCMgr->Update(0.1f);	// DespawnJob Run
		m_pField->Update(0.1f);		// DeleteMe 처리
		CHECK_EQUAL(m_pField->GetNPCQty(m_pNPCInfo->nID), 0);
		gmgr.pNPCMgr->Clear();
	}


	TEST_FIXTURE(Fixture, TSFF_SpawnDelay)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, NPCID, StartPos, DelayTime)\n"
			"	Field:SpawnDelay(NPCID, StartPos, DelayTime);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		float fDelayTime = 5.0f;

		WCALL4(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_pNPCInfo->nID, vec3(1.0f, 1.0f, 1.0f), fDelayTime, WNULL);
		CHECK_EQUAL(m_pField->GetNPCQty(m_pNPCInfo->nID), 0);
		m_pField->GetSpawn()->Update(fDelayTime - 0.1f);
		CHECK_EQUAL(m_pField->GetNPCQty(m_pNPCInfo->nID), 0);
		m_pField->GetSpawn()->Update(0.1f);
		CHECK_EQUAL(m_pField->GetNPCQty(m_pNPCInfo->nID), 1);
		CHECK_EQUAL(true, m_pField->GetSpawn()->IsSpawnDelayEmpty());
	}

	
	TEST_FIXTURE(Fixture, TSFF_SpawnByID)
	{
		// 스폰정보 생성
		m_pField->GetSpawn()->Fini();
		m_pSpawnInfo = new SPAWN_INFO();
		m_pSpawnInfo->nID = 1122;
		m_pSpawnInfo->bCheckSpawnPointValidation = false;
		m_pSpawnInfo->vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));
		m_pFieldInfo->InsertSpawnInfo(*m_pSpawnInfo);
		m_pField->GetSpawn()->Init();
		
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, nSpawnID)\n"
			"	return Field:SpawnByID(nSpawnID);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GGlueNPC* pRet = NULL;

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_nInvalidSpawnID, &pRet);
		CHECK_EQUAL(m_pField->GetNPCQty(m_pNPCInfo->nID), 0);		
		CHECK_EQUAL(pRet, GNullGlueNPC::GetNull());
		
		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_pSpawnInfo->nID, &pRet);
		CHECK_EQUAL(m_pField->GetNPCQty(m_pNPCInfo->nID), 1);
		CHECK_EQUAL(pRet->GetID(), m_pNPCInfo->nID);
	}	

	TEST_FIXTURE(Fixture, TSFF_DespawnByID)
	{
		// 스폰정보 생성
		m_pField->GetSpawn()->Fini();
		m_pSpawnInfo = new SPAWN_INFO();
		m_pSpawnInfo->nID = 1122;
		m_pSpawnInfo->nRespawnTime = 1; // 1초후에 리스폰됨
		m_pSpawnInfo->bCheckSpawnPointValidation = false;
		m_pSpawnInfo->vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));
		m_pFieldInfo->InsertSpawnInfo(*m_pSpawnInfo);
		m_pField->GetSpawn()->Init();
		m_pField->Update(0.0f); // 스폰될 시간을 줌


		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, nSpawnID)\n"
			"	return Field:DespawnByID(nSpawnID, false);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->GetSpawnNPC(m_pSpawnInfo->nID);
		GGlueNPC* pRet = NULL;

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_nInvalidSpawnID, &pRet);		
		CHECK_EQUAL(pRet, GNullGlueNPC::GetNull());
		CHECK_EQUAL(m_pField->GetNPCQty(m_pNPCInfo->nID), 1);
		
		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_pSpawnInfo->nID, &pRet);
		CHECK_EQUAL(pRet, pNPC->GetDelegator());
		m_pField->Update(0.0f);
		CHECK_EQUAL(m_pField->GetNPCQty(m_pNPCInfo->nID), 0);
	}

	TEST_FIXTURE(Fixture, TSFF_GetSpawnInstance)
	{
		// 스폰정보 생성
		m_pField->GetSpawn()->Fini();
		m_pSpawnInfo = new SPAWN_INFO();
		m_pSpawnInfo->nID = 1122;
		m_pSpawnInfo->bCheckSpawnPointValidation = false;
		m_pSpawnInfo->vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));
		m_pFieldInfo->InsertSpawnInfo(*m_pSpawnInfo);
		m_pField->GetSpawn()->Init();
		m_pField->Update(0.0f); // 스폰될 시간을 줌

		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, nSpawnID)\n"
			"	return Field:GetSpawnNPC(nSpawnID);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }		

		GEntityNPC* pNPC = m_pField->GetSpawnNPC(m_pSpawnInfo->nID);
		GGlueNPC* pRet=NULL;

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_nInvalidSpawnID, &pRet);		
		CHECK_EQUAL(pRet, GNullGlueNPC::GetNull());

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_pSpawnInfo->nID, &pRet);
		CHECK_EQUAL(pRet, pNPC->GetDelegator());
	}

	TEST_FIXTURE(Fixture, TSFF_EnableSensor)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, SpawnID)\n"
			"	return Field:EnableSensor(SpawnID)\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }		
	
		m_pField->EnableSensor(m_SensorInfo.nID, false);
		bool bRet = false;

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_SensorInfo.nID, &bRet);		
		CHECK_EQUAL(true, m_pField->IsEnableSensor(m_SensorInfo.nID));
		CHECK_EQUAL(true, bRet);
	}

	TEST_FIXTURE(Fixture, TSFF_DisableSensor)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, SpawnID)\n"
			"	return Field:DisableSensor(SpawnID)\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }		

		m_pField->EnableSensor(m_SensorInfo.nID, true);
		bool bRet = false;

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_SensorInfo.nID, &bRet);
		CHECK_EQUAL(true, bRet);
		CHECK_EQUAL(false, m_pField->IsEnableSensor(m_SensorInfo.nID));		
	}

	TEST_FIXTURE(Fixture, TSFF_CheckSensor)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, SensorID)\n"
			"	return Field:CheckSensor(SensorID)\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		bool bRet = false;

		m_pField->EnableSensor(m_SensorInfo.nID, true);		
		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_SensorInfo.nID, &bRet);
		CHECK_EQUAL(true, bRet);

		m_pField->EnableSensor(m_SensorInfo.nID, false);	
		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_SensorInfo.nID, &bRet);
		CHECK_EQUAL(false, bRet);
	}

	TEST_FIXTURE(Fixture, TSFF_GetNPCCount)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, NPCID)\n"
			"	return Field:GetNPCCount(NPCID)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		int nRet=0;

		GEntityNPC* pNPC1 = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_pNPCInfo->nID, &nRet);
		CHECK_EQUAL(nRet, 1);

		GEntityNPC* pNPC2 = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), m_pNPCInfo->nID, &nRet);
		CHECK_EQUAL(nRet, 2);
	}

	TEST_FIXTURE(Fixture, TSFF_SetTimer)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, TimerID, fIntervalTime)\n"
			"	return Field:SetTimer(TimerID, fIntervalTime)\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		int nTimerID = 1;
		float fIntervalTime = 2.0f;
		bool bRet = false;

		WCALL3(WLUA, pszTestFuncName, m_pField->GetDelegator(), nTimerID, fIntervalTime, &bRet);
		CHECK_EQUAL(true, bRet);
		CHECK_EQUAL(true, m_pField->IsTimer(nTimerID));
	}

	TEST_FIXTURE(Fixture, TSFF_KillTimer)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, TimerID)\n"
			"	return Field:KillTimer(TimerID)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		int nTimerID = 1;
		float fIntervalTime = 2.0f;
		m_pField->SetTimer(nTimerID, fIntervalTime);		
		bool bRet = false;

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), nTimerID, &bRet);
		CHECK_EQUAL(true, bRet);
		CHECK_EQUAL(false, m_pField->IsTimer(nTimerID));
	}

	TEST_FIXTURE(Fixture, TSFF_GetMarkerPos)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, MarkerID)\n"
			"	return Field:GetMarkerPos(MarkerID)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		MARKER_INFO* pMarkerInfo = GUTHelper_Field::InsertNewMarkerInfo(m_pFieldInfo);
		pMarkerInfo->vPoint = vec3(10.0f,10.0f,10.0f);
		vec3 vRet = vec3::ZERO;

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), pMarkerInfo->nID, &vRet);
		CHECK_EQUAL(vRet, pMarkerInfo->vPoint);
	}

	TEST_FIXTURE(Fixture, TSFF_GetMarkerDir)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, MarkerID)\n"
			"	return Field:GetMarkerDir(MarkerID)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		MARKER_INFO* pMarkerInfo = GUTHelper_Field::InsertNewMarkerInfo(m_pFieldInfo);
		pMarkerInfo->vDir = vec3(10.0f,10.0f,10.0f);
		vec3 vRet = vec3::ZERO;

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), pMarkerInfo->nID, &vRet);
		CHECK_EQUAL(vRet, pMarkerInfo->vDir);
	}

	TEST_FIXTURE(Fixture, TSFF_EnableInteraction)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, NPCID)\n"
			"	return Field:EnableInteraction(NPCID)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GNPCInfo* pNPCInfo = m_Helper.NewNPCInfo();
		GEntityNPC* pNPC1 = m_Helper.NewEntityNPC(m_pField, vec3(1000, 1000, 1000), pNPCInfo->nID);
		GEntityNPC* pNPC2 = m_Helper.NewEntityNPC(m_pField, vec3(1000, 1000, 1000), pNPCInfo->nID);
		pNPC1->GetNPCInteraction().Disable();
		pNPC2->GetNPCInteraction().Disable();

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), pNPCInfo->nID, WNULL);
		CHECK_EQUAL(true, pNPC1->GetNPCInteraction().IsEnable());
		CHECK_EQUAL(true, pNPC2->GetNPCInteraction().IsEnable());
	}

	TEST_FIXTURE(Fixture, TSFF_DisableInteraction)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, NPCID)\n"
			"	return Field:DisableInteraction(NPCID)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GNPCInfo* pNPCInfo = m_Helper.NewNPCInfo();
		GEntityNPC* pNPC1 = m_Helper.NewEntityNPC(m_pField, vec3(1000, 1000, 1000), pNPCInfo->nID);
		GEntityNPC* pNPC2 = m_Helper.NewEntityNPC(m_pField, vec3(1000, 1000, 1000), pNPCInfo->nID);
		pNPC1->GetNPCInteraction().Enable();
		pNPC2->GetNPCInteraction().Enable();

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), pNPCInfo->nID, WNULL);
		CHECK_EQUAL(false, pNPC1->GetNPCInteraction().IsEnable());
		CHECK_EQUAL(false, pNPC2->GetNPCInteraction().IsEnable());
	}

	TEST_FIXTURE(Fixture, ActivateSpawnGroup)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, SpawnGroupID)\n"
			"    return Field:ActivateSpawnGroup(SpawnGroupID)\n"
			"end\n";

		ASSERT_CHECK(m_Helper.RunLuaString(pszScript, pszTestFuncName));
		
		
		SPAWN_INFO spawnInfo1;
		spawnInfo1.nFieldID = m_pField->GetID();
		spawnInfo1.vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));
		spawnInfo1.nGroupID = 1;
		m_pField->GetSpawn()->AddSpawn(1, &spawnInfo1);

		SPAWN_INFO spawnInfo2;
		spawnInfo2.nFieldID = m_pField->GetID();
		spawnInfo2.vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));
		spawnInfo2.nGroupID = 2;
		m_pField->GetSpawn()->AddSpawn(2, &spawnInfo2);

		SPAWN_INFO spawnInfo3;
		spawnInfo3.nFieldID = m_pField->GetID();
		spawnInfo3.vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));
		spawnInfo3.nGroupID = INVALID_SPAWN_GROUP_ID;
		m_pField->GetSpawn()->AddSpawn(3, &spawnInfo3);


		GEntityNPC* pNPC1 = m_pField->Spawn(1);
		GEntityNPC* pNPC2 = m_pField->Spawn(2);
		GEntityNPC* pNPC3 = m_pField->Spawn(3);


		CHECK(!pNPC1->IsActivateUpdate());
		CHECK(!pNPC2->IsActivateUpdate());
		CHECK(pNPC3->IsActivateUpdate());


		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), pNPC2->GetGroupID(), WNULL);

		CHECK(!pNPC1->IsActivateUpdate());
		CHECK(pNPC2->IsActivateUpdate());
		CHECK(pNPC3->IsActivateUpdate());
	}

	TEST_FIXTURE(Fixture, PlayBGM)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, BGMName)\n"
			"	return Field:PlayBGM(BGMName)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer*	m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		MockLink* pLink = GUTHelper::NewLink(m_pPlayer, true);
		pLink->AddAllowedID(MC_ENV_PLAY_BGM);

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), "TestBGM", WNULL);		
		
		CHECK_EQUAL(MC_ENV_PLAY_BGM, pLink->GetCommand(0).GetID());
		wstring strName;
		pLink->GetCommand(0).GetParameter(strName, 0, MPT_WSTR);
		CHECK(L"TestBGM" == strName);

		GUTHelper::ClearLinks();
	}

	TEST_FIXTURE(Fixture, Notice)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, Msg)\n"
			"	Field:Notice(Msg)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer*	m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		MockLink* pLink = GUTHelper::NewLink(m_pPlayer, true);
		pLink->AddAllowedID(MC_MSG_RES);

		WCALL2(WLUA, pszTestFuncName, m_pField->GetDelegator(), "TestString", WNULL);		

		CHECK_EQUAL(MC_MSG_RES, pLink->GetCommand(0).GetID());
		int nMsgType = 0;
		pLink->GetCommand(0).GetParameter(&nMsgType, 0, MPT_INT);
		CHECK_EQUAL(MT_NOTICE, nMsgType);

		GUTHelper::ClearLinks();
	}

	TEST_FIXTURE(Fixture, MakeSession)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field, NPC1, NPC2, NPC3)\n"
			"	return Field:MakeSession('Name', {NPC1, NPC2, NPC3})\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC1 = m_Helper.NewEntityNPC(m_pField, vec3(1000, 1000, 1000), m_pNPCInfo->nID);
		GEntityNPC* pNPC2 = m_Helper.NewEntityNPC(m_pField, vec3(1000, 1000, 1000), m_pNPCInfo->nID);
		GEntityNPC* pNPC3 = m_Helper.NewEntityNPC(m_pField, vec3(1000, 1000, 1000), m_pNPCInfo->nID);

		GFieldNPCSession* pRet=NULL;
		WCALL4(WLUA, pszTestFuncName, m_pField->GetDelegator(), pNPC1->GetDelegator(), pNPC2->GetDelegator(), pNPC3->GetDelegator(), &pRet);		
		CHECK_EQUAL(3, pRet->GetNPCQty());
	}
	
	TEST_FIXTURE(Fixture, FindSession)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Field)\n"
			"	return Field:FindSession('Name')\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GFieldNPCSession* session = GFieldNPCSession::New("Name", m_pField->GetUID());
		m_pField->GetSession()->Add(session);	

		GFieldNPCSession* pRet=NULL;
		WCALL1(WLUA, pszTestFuncName, m_pField->GetDelegator(), &pRet);		
		CHECK(pRet != NULL);
	}
}
