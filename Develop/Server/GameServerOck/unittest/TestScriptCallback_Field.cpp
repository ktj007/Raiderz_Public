#include "stdafx.h"
#include "FBaseScriptCallback.h"
#include "GEnvManager.h"
#include "GServer.h"
#include "GPlayerCutscene.h"
#include "GWorld.h"
#include "GGluePlayer.h"

const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

SUITE(ScriptCallback_Field)
{
	struct Fixture: public FBaseScriptCallback
	{
		Fixture()			{}
		virtual ~Fixture() 	{}	
	};

	TEST_FIXTURE(Fixture, Field_OnWeatherChanged)
	{
		string strTableName = MAKE_LUATABLE_FIELD(m_pFieldInfo->m_nFieldID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Field_100:OnWeatherChanged(this, Weather)\n"
			" if (Weather == WEATHER_SNOWY) then\n"
			"	TEST_VAR = this:GetID();\n"
			" end\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GWeatherMgr* pWeatherMgr = new GWeatherMgr();
		pWeatherMgr->Init(m_pFieldInfo, GConst::CONST_ENV_WEATHER_TICK_SEC);
		m_pField->SetWeatherMgr(pWeatherMgr);
		m_pField->ChangeWeather(WEATHER_SNOWY);

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), m_pField->GetID());
	}

	TEST_FIXTURE(Fixture, Field_OnTimeChanged)
	{
		GTestMgrWrapper<GEnvManager> EnvMgr;
		GEnvParam envParam;
		EnvMgr.Get()->Create(envParam);

		string strTableName = MAKE_LUATABLE_FIELD(m_pFieldInfo->m_nFieldID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Field_100:OnTimeChanged(this, Time)\n"
			" if (Time == TIME_NIGHT) then\n"
			"	TEST_VAR = this:GetID();\n"
			" end\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		gmgr.pEnvManager->Create(envParam);
		gsys.pServer->Update(0.1f);
		gmgr.pEnvManager->SetTime(TIME_NIGHT);
		gsys.pServer->Update(0.1f);

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), m_pField->GetID());
	}

	TEST_FIXTURE(Fixture, Field_OnCreate_OnDestroy)
	{
		string strTableName = MAKE_LUATABLE_FIELD(m_pFieldInfo->m_nFieldID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Field_100:OnCreate(this)\n"
			" TEST_VAR = this:GetID();\n"
			"end\n"
			"function Field_100:OnDestroy(this)\n"
			" TEST_VAR = this:GetID();\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 0);

		GField Field(MUID(0, 100), NULL);
		Field.Create(m_pFieldInfo);
		Field.OnCreated();

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), Field.GetID());

		Field.Destroy();

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), Field.GetID());
	}

	TEST_FIXTURE(Fixture, Field_OnTimer)
	{
		GTestMgrWrapper<GEnvManager>							EnvMgr;
		GEnvParam envParam;
		EnvMgr.Get()->Create(envParam);

		string strTableName = MAKE_LUATABLE_FIELD(m_pFieldInfo->m_nFieldID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Field_100:OnTimer(this, TimerID)\n"
			"	TEST_VAR = TimerID;\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		// 일회성 콜백
		m_pField->SetTimer(707, 100.0f, false);
		CHECK(WLUA->GetVar<int>("TEST_VAR") != 707);	// 시간이 안지났다
		m_pField->Update(100.0f);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 707);	// 시간이 지나서 호출됨
		WRUNSTRING(WLUA, "TEST_VAR = 0");		// 테스트값 초기화
		m_pField->Update(100.0f);
		CHECK(WLUA->GetVar<int>("TEST_VAR") != 707);	// 만료됐다

		// 반복 콜백
		m_pField->SetTimer(707, 100.0f, false);		
		m_pField->Update(100.0f);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 707);	// 시간이 지나서 호출됨
		m_pField->Update(100.0f);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 707);	// 반복되서 호출됨
		WRUNSTRING(WLUA, "TEST_VAR = 0");		// 테스트값 초기화
		WCALL2(WLUA, "CancelTimer", m_pField->GetUID(), 707, WNULL);
		m_pField->Update(100.0f);
		CHECK(WLUA->GetVar<int>("TEST_VAR") != 707);	// 만료됐다
	}

	TEST_FIXTURE(Fixture, Field_OnSpawn)
	{
		SPAWN_INFO* pSpawninfo = new SPAWN_INFO();
		pSpawninfo->nID = 1122;
		pSpawninfo->strName = "1122";
		pSpawninfo->bCheckSpawnPointValidation = false;
		pSpawninfo->vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));
		pSpawninfo->vPoint = vec3(1234.1f, 5678.2f, 9012.3f);	

		string strTableName = MAKE_LUATABLE_FIELD(m_pFieldInfo->m_nFieldID);
		MAKE_TABLE(strTableName.c_str());
		const char* pszScript = 
			"function Field_100:OnSpawn(this, SpawnInfo)\n"
			" TEST_VAR = 1;\n"
			" EXPECT_SpawnID = SpawnInfo.SpawnID;\n"
			" EXPECT_NPC = SpawnInfo.NPC;\n"
			" EXPECT_Field = SpawnInfo.Field;\n"
			"end\n"
			;
		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir, pSpawninfo);

		if (!WLUA->IsExistGlobal("TEST_VAR"))	{ CHECK(FALSE && "field callback not called\n"); return; }
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);
		CHECK_EQUAL(WLUA->GetVar<int>("EXPECT_SpawnID"), 1122);
		CHECK_EQUAL(WLUA->GetVar<GGlueNPC*>("EXPECT_NPC"), pNPC->GetDelegator());
		CHECK_EQUAL(WLUA->GetVar<GGlueField*>("EXPECT_Field"), m_pField->GetDelegator());

		pNPC->doDie();
		pNPC->Destroy();
		SAFE_DELETE(pNPC);
		SAFE_DELETE(pSpawninfo);
	}

	TEST_FIXTURE(Fixture, Field_OnDie)
	{
		SPAWN_INFO* pSpawninfo = new SPAWN_INFO();
		pSpawninfo->nID = 1122;
		pSpawninfo->strName = "1122";
		pSpawninfo->bCheckSpawnPointValidation = false;
		pSpawninfo->vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));
		pSpawninfo->vPoint = vec3(1234.1f, 5678.2f, 9012.3f);	

		string strTableName = MAKE_LUATABLE_FIELD(m_pFieldInfo->m_nFieldID);
		MAKE_TABLE(strTableName.c_str());
		const char* pszScript = 
			"function Field_100:OnDie(this, DespawnInfo)\n"
			" TEST_VAR = 1;\n"
			" EXPECT_SpawnID = DespawnInfo.SpawnID;\n"
			" EXPECT_NPC = DespawnInfo.NPC;\n"
			" EXPECT_Field = DespawnInfo.Field;\n"
			"end\n"
			;
		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir, pSpawninfo);
		pNPC->doDie();

		if (!WLUA->IsExistGlobal("TEST_VAR"))	{ CHECK(FALSE && "field callback not called\n"); return; }
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);
		CHECK_EQUAL(WLUA->GetVar<int>("EXPECT_SpawnID"), 1122);
		CHECK_EQUAL(WLUA->GetVar<GGlueNPC*>("EXPECT_NPC"), pNPC->GetDelegator());
		CHECK_EQUAL(WLUA->GetVar<GGlueField*>("EXPECT_Field"), m_pField->GetDelegator());

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
		SAFE_DELETE(pSpawninfo);
	}

	TEST_FIXTURE(Fixture, EndCutscene)
	{
		string strTableName = MAKE_LUATABLE_FIELD(m_pFieldInfo->m_nFieldID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Field_100:OnEndCutscene(this, Player, CutsceneID)\n"
			"	TEST_VAR = CutsceneID\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		int nCutsceneID = 777;
		
		GEntityPlayer*	m_pCutscenePlayer = test.player.NewPlayer(m_pField);
		m_pCutscenePlayer->GetCutscene().BeginCutscene(nCutsceneID);
		m_pCutscenePlayer->GetCutscene().EndCutscene(nCutsceneID);

		CHECK_EQUAL(nCutsceneID, WLUA->GetVar<int>("TEST_VAR"));
	}


	TEST_FIXTURE(Fixture, OnMarkerArrive)
	{
		MARKER_INFO* pMarkerInfo = test.field.InsertNewMarkerInfo(m_pField->GetInfo(), 1);
		vec3 vSpawnPos = vec3(pMarkerInfo->vPoint.x+100.0f, pMarkerInfo->vPoint.y, pMarkerInfo->vPoint.z);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vSpawnPos);		

		string strTableName = MAKE_LUATABLE_FIELD(m_pFieldInfo->m_nFieldID);
		MAKE_TABLE(strTableName.c_str());
		const char* pszScript = 
			"function Field_100:OnMarkerArrive_1(this, NPC)\n"
			" TEST_VAR = 1;\n"
			" EXPECT_NPC = NPC;\n"			
			"end\n"
			;
		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		pNPC->MoveToMarker(pMarkerInfo->nID);
		gsys.pWorld->Update(100.0f);
		gsys.pWorld->Update(1.0f);
		
		if (!WLUA->IsExistGlobal("TEST_VAR"))	{ CHECK(FALSE && "field callback not called\n"); return; }		
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR"), 1);
		CHECK_EQUAL(WLUA->GetVar<GGlueNPC*>("EXPECT_NPC"), pNPC->GetDelegator());
	}

	TEST_FIXTURE(Fixture, OnEvent)
	{
		string strTableName = MAKE_LUATABLE_FIELD(m_pFieldInfo->m_nFieldID);
		MAKE_TABLE(strTableName.c_str());

		const char* pszScript = 
			"function Field_100:OnEvent_Apple(this, Actor)\n"
			"	TEST_VAR = 777\n"
			"end\n"
			;

		if (!RunLuaString(pszScript))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer*	m_pPlayer = test.player.NewPlayer(m_pField);
		m_pPlayer->GetDelegator()->RunFieldEvent("Apple");

		CHECK_EQUAL(777, WLUA->GetVar<int>("TEST_VAR"));
	}

}


