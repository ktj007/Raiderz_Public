#include "stdafx.h"
#include "GNPCInfo.h"
#include "GTestWrapper.h"
#include "SUnitTestUtil.h"
#include "MockField.h"
#include "GUTHelper.h"
#include "GFieldMgr.h"
#include "GServer.h"
#include "GEnvManager.h"
#include "GWeatherMgr.h"
#include "GFieldInfoMgr.h"
#include "GConst.h"
#include "GEntityNPC.h"
#include "GNPCDecay.h"
#include "GUTHelper_Field.h"
#include "GTestForward.h"

SUITE(Spawn)
{
	struct Fixture
	{
		Fixture()
		{ 
			m_pFieldInfo = new GFieldInfo;
			m_pFieldInfo->m_nFieldID = 100;
			m_pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
			gmgr.pFieldInfoMgr->Insert(m_pFieldInfo);

			m_pField = NULL;

			m_pNPCInfo = new GNPCInfo;
			m_pNPCInfo->nID = 1;
			wchar_t szBuff[128];
			swprintf_s(szBuff, L"NPC_%d", m_pNPCInfo->nID);
			m_pNPCInfo->strNameInLua = szBuff;
			gmgr.pNPCInfoMgr->Insert(m_pNPCInfo);
		}

		~Fixture() 
		{
			m_pField->Update(1.0f);
			gmgr.pFieldMgr->DeleteField(m_pField->GetUID());

			m_pFieldInfo->Reset();
		}
		void SetupSpawnInfo(SPAWN_INFO& spawnInfo, int nID)
		{
			spawnInfo.nID = nID;
			spawnInfo.bCheckSpawnPointValidation = false;
			spawnInfo.vPoint = vec3(0,0,0);
			spawnInfo.strName = "1122";
			spawnInfo.vecNPCInfoList.push_back(SPAWN_NPC_INFO(1));
		}

		void InitForGather(int nSpawnInfoCount, GATHER_TYPE nGatherType)
		{
			m_pNPCInfo->nNpcType = NPC_TYPE_GATHER;
			m_pNPCInfo->nGatherType = nGatherType;

			for (int i = 0; i < nSpawnInfoCount; i++)
			{
				// 첫번째 mine
				SPAWN_INFO Spawninfo;
				SetupSpawnInfo(Spawninfo, 1122 + i);
				Spawninfo.nGatherType = nGatherType;
				m_pFieldInfo->InsertSpawnInfo(Spawninfo);
			}

			m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
			gmgr.pFieldMgr->AddField(m_pField);
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		GFieldInfo*		m_pFieldInfo;
		GNPCInfo*		m_pNPCInfo;

		GTestMgrWrapper<GFieldMgr>					m_FieldMgr;
		GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
		GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgr;
		DECLWRAPPER_NPCMgr;
	};

	TEST_FIXTURE(Fixture, SpawnBasic)
	{
		SPAWN_INFO Spawninfo;
		SetupSpawnInfo(Spawninfo, 1122);
		m_pFieldInfo->InsertSpawnInfo(Spawninfo);

		m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		gmgr.pFieldMgr->AddField(m_pField);

		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));
		m_pField->Update(0.0f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));
	}

	TEST_FIXTURE(Fixture, SpawnStartTime)
	{
		SPAWN_INFO Spawninfo;
		SetupSpawnInfo(Spawninfo, 1122);
		Spawninfo.nStartTime = 124; // 124초 후에 스폰
		m_pFieldInfo->InsertSpawnInfo(Spawninfo);

		m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		gmgr.pFieldMgr->AddField(m_pField);

		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));
		m_pField->Update(0.0f);
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));	
		m_pField->Update(100.0f);
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));	
		m_pField->Update(24.0f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));	
	}

	TEST_FIXTURE(Fixture, DespawnAndRespawn)
	{
		SPAWN_INFO Spawninfo;
		SetupSpawnInfo(Spawninfo, 1122);
		Spawninfo.nStartTime = 10; // 10초 후에 스폰
		Spawninfo.nRespawnTime = 3; // 3초 후에 리스폰
		m_pFieldInfo->InsertSpawnInfo(Spawninfo);

		m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		gmgr.pFieldMgr->AddField(m_pField);

		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));
		m_pField->Update(10.0f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));	
		m_pField->GetSpawn()->Despawn(1122, true);		// 반복되게 설정 유지하면서 디스폰
		m_pField->Update(0.0f);
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));	
		m_pField->Update(2.0f);
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));	
		m_pField->Update(1.0f);	// 3초 지남
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));	
	}

	TEST_FIXTURE(Fixture, ForcedSpawn)
	{
		SPAWN_INFO Spawninfo;
		SetupSpawnInfo(Spawninfo, 1122);
		Spawninfo.nStartTime = 10; // 10초 후에 스폰
		Spawninfo.nRespawnTime = 3; // 3초 후에 리스폰
		m_pFieldInfo->InsertSpawnInfo(Spawninfo);

		m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		gmgr.pFieldMgr->AddField(m_pField);

		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));
		m_pField->GetSpawn()->Spawn(1122);
		m_pField->Update(0.0f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));	
		m_pField->Update(3.0f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));	
		m_pField->Update(10.0f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));	
	}

	TEST_FIXTURE(Fixture, SpawnTimeDuration)
	{
		SPAWN_INFO Spawninfo;
		SetupSpawnInfo(Spawninfo, 1122);
		Spawninfo.bEnableTimeList[TIME_NIGHT] = true;
		Spawninfo.bUseConditionTime = true;
		m_pFieldInfo->InsertSpawnInfo(Spawninfo);

		m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		gmgr.pFieldMgr->AddField(m_pField);

		gmgr.pEnvManager->ChangeTimeForced(TIME_DAYTIME);			// 시간대가 맞지 않음
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));
		gsys.pServer->Update(0.1f);
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));	
		gmgr.pEnvManager->ChangeTimeForced(TIME_NIGHT);
		gsys.pServer->Update(0.1f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));
		gmgr.pEnvManager->ChangeTimeForced(TIME_NIGHT);			// 같은 시간대
		gsys.pServer->Update(0.1f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));
		gmgr.pEnvManager->ChangeTimeForced(TIME_DAWN);				// 시간대가 맞지 않음
		gsys.pServer->Update(0.1f);
		gsys.pServer->Update(0.1f);	// 엔티티 제거에 소비되는 틱
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));	
	}

	TEST_FIXTURE(Fixture, SpawnTimeDetailDuration)
	{
		class MockEnvManager : public GEnvManager
		{
		protected:

		public:
			using GEnvManager::SetTimeHour;
			MockEnvManager() : GEnvManager() 
			{
				m_fNextCheckTime = m_fElapsed + 99999.0f;
			}
			virtual ~MockEnvManager() {}
		};

		GTestMgrWrapper2<GEnvManager, MockEnvManager> m_EnvManagerWrapper;
		MockEnvManager* pEnvManager = dynamic_cast<MockEnvManager*>(gmgr.pEnvManager);

		SPAWN_INFO Spawninfo;
		SetupSpawnInfo(Spawninfo, 1122);
		Spawninfo.pairEnableTime = pair<int,int>(0, 12);	// 0~12시까지만 스폰됨
		m_pFieldInfo->InsertSpawnInfo(Spawninfo);

		m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		gmgr.pFieldMgr->AddField(m_pField);

		pEnvManager->SetTimeHour(13);			// 시간 맞지 않음
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));
		gsys.pServer->Update(0.1f);
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));	
		pEnvManager->SetTimeHour(6);	
		gsys.pServer->Update(0.1f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));
		pEnvManager->SetTimeHour(7);				// 같은 시간대
		gsys.pServer->Update(0.1f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));
		pEnvManager->SetTimeHour(15);				// 시간대가 맞지 않음
		gsys.pServer->Update(0.1f);
		gsys.pServer->Update(0.1f);	// 엔티티 제거에 소비되는 틱
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));	
	}

	TEST_FIXTURE(Fixture, NoRespawnWhenChangedBetweenSameTimeDuration)
	{
		SPAWN_INFO Spawninfo;
		SetupSpawnInfo(Spawninfo, 1122);
		Spawninfo.bEnableTimeList[TIME_NIGHT] = true;
		Spawninfo.bUseConditionTime = true;
		m_pFieldInfo->InsertSpawnInfo(Spawninfo);

		m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		gmgr.pFieldMgr->AddField(m_pField);

		gmgr.pEnvManager->SetTime(TIME_NIGHT);
		gsys.pServer->Update(0.1f);

		GEntityNPC* pNPC1 = m_pField->GetSpawn()->GetNPCInstance(1122);
		if (!pNPC1)	{ CHECK(FALSE); return; }

		gmgr.pEnvManager->SetTime(TIME_NIGHT);
		gsys.pServer->Update(0.1f);

		GEntityNPC* pNPC2 = m_pField->GetSpawn()->GetNPCInstance(1122);
		if (!pNPC2)	{ CHECK(FALSE); return; }

		CHECK_EQUAL(pNPC1->GetUID(), pNPC2->GetUID());
	}

	TEST_FIXTURE(Fixture, SpawnWeatherDuration)
	{
		SPAWN_INFO Spawninfo;
		SetupSpawnInfo(Spawninfo, 1122);
		Spawninfo.bEnableWeatherList[WEATHER_HEAVY_SNOWY] = true;
		Spawninfo.bUseConditionWeather = true;
		m_pFieldInfo->InsertSpawnInfo(Spawninfo);

		m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		gmgr.pFieldMgr->AddField(m_pField);

		GWeatherMgr* pWeatherMgr = new GWeatherMgr();
		pWeatherMgr->Init(m_pFieldInfo, GConst::CONST_ENV_WEATHER_TICK_SEC);
		m_pField->SetWeatherMgr(pWeatherMgr);

		m_pField->ChangeWeather(WEATHER_CLOUDY);			// 시간대가 맞지 않음
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));
		m_pField->Update(0.1f);
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));	
		m_pField->ChangeWeather(WEATHER_HEAVY_SNOWY);	
		m_pField->Update(0.1f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));
		m_pField->ChangeWeather(WEATHER_HEAVY_SNOWY);				// 같은 시간대
		m_pField->Update(0.1f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));
		m_pField->ChangeWeather(WEATHER_RAINY);				// 시간대가 맞지 않음
		m_pField->Update(0.1f);
		m_pField->Update(0.1f);	// 엔티티 제거에 소비되는 틱
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));	
	}

	TEST_FIXTURE(Fixture, NoRespawnWhenChangedBetweenSameWeatherDuration)
	{
		SPAWN_INFO Spawninfo;
		SetupSpawnInfo(Spawninfo, 1122);
		Spawninfo.bEnableWeatherList[WEATHER_HEAVY_SNOWY] = true;
		Spawninfo.bEnableWeatherList[WEATHER_RAINY] = true;
		Spawninfo.bUseConditionWeather = true;
		m_pFieldInfo->InsertSpawnInfo(Spawninfo);

		m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		gmgr.pFieldMgr->AddField(m_pField);

		GWeatherMgr* pWeatherMgr = new GWeatherMgr();
		pWeatherMgr->Init(m_pFieldInfo, GConst::CONST_ENV_WEATHER_TICK_SEC);
		m_pField->SetWeatherMgr(pWeatherMgr);

		m_pField->ChangeWeather(WEATHER_HEAVY_SNOWY);
		m_pField->Update(0.1f);

		GEntityNPC* pNPC1 = m_pField->GetSpawn()->GetNPCInstance(1122);
		if (!pNPC1)	{ CHECK(FALSE); return; }

		m_pField->ChangeWeather(WEATHER_RAINY);
		m_pField->Update(0.1f);

		GEntityNPC* pNPC2 = m_pField->GetSpawn()->GetNPCInstance(1122);
		if (!pNPC2)	{ CHECK(FALSE); return; }

		CHECK_EQUAL(pNPC1->GetUID(), pNPC2->GetUID());
	}

	TEST_FIXTURE(Fixture, SpawnWeatherAndTimeDuration)
	{
		SPAWN_INFO Spawninfo;
		SetupSpawnInfo(Spawninfo, 1122);
		Spawninfo.bEnableWeatherList[WEATHER_HEAVY_SNOWY] = true;
		Spawninfo.bEnableTimeList[TIME_NIGHT] = true;
		Spawninfo.bUseConditionTime = true;
		Spawninfo.bUseConditionWeather = true;
		m_pFieldInfo->InsertSpawnInfo(Spawninfo);

		m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		gmgr.pFieldMgr->AddField(m_pField);

		GWeatherMgr* pWeatherMgr = new GWeatherMgr();
		pWeatherMgr->Init(m_pFieldInfo, GConst::CONST_ENV_WEATHER_TICK_SEC);
		m_pField->SetWeatherMgr(pWeatherMgr);

		gmgr.pEnvManager->SetTime(TIME_DAYTIME);			// 시간대가 맞지 않음
		m_pField->ChangeWeather(WEATHER_CLOUDY);			// 시간대가 맞지 않음
		gsys.pServer->Update(0.1f);
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));
		gmgr.pEnvManager->SetTime(TIME_NIGHT);
		m_pField->ChangeWeather(WEATHER_HEAVY_SNOWY);	
		gsys.pServer->Update(0.1f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));
		gmgr.pEnvManager->SetTime(TIME_NIGHT);
		m_pField->ChangeWeather(WEATHER_HEAVY_SNOWY);					// 같은 시간대
		gsys.pServer->Update(0.1f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));
		gmgr.pEnvManager->SetTime(TIME_DAYTIME);			// 시간대가 맞지 않음
		m_pField->ChangeWeather(WEATHER_CLOUDY);			// 시간대가 맞지 않음
		gsys.pServer->Update(0.1f);
		gsys.pServer->Update(0.1f);	// 엔티티 제거에 소비되는 틱
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));	
	}

	TEST_FIXTURE(Fixture, RemoveToSpawnManagerWhenDecay)
	{
		SPAWN_INFO Spawninfo;
		SetupSpawnInfo(Spawninfo, 1122);
		Spawninfo.nRespawnTime = 3; // 3초후에 리스폰
		m_pFieldInfo->InsertSpawnInfo(Spawninfo);
		m_pField = GUTHelper_Field::NewMockField(m_pFieldInfo);
		gmgr.pFieldMgr->AddField(m_pField);

		// NPC 스폰
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));
		m_pField->Update(0.0f);
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));
		
		// NPC 죽음
		GEntityNPC* pNPC1 = m_pField->GetSpawn()->GetNPCInstance(1122);
		TVALID(pNPC1);
		pNPC1->GetNPCDecay().SetCustomDecayTick(5.0f); // 5초뒤에 디스폰
		pNPC1->doDie();
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));

		// NPC 리스폰
		m_pField->Update(3.0f);
		CHECK_EQUAL(2, m_pField->GetEntityQty(ETID_NPC));
		GEntityNPC* pNPC2 = m_pField->GetSpawn()->GetNPCInstance(1122);
		TVALID(pNPC2);
		CHECK(pNPC1->GetUID() != pNPC2->GetUID());	// 서로 다른 NPC

		// 시체 소멸 완료
		m_pField->Update(2.0f); // 5초가 지나 디스폰이 됨
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));
	}

	TEST_FIXTURE(Fixture, TestSpawn_IsGatherEntitySpawnEnabled_NoMaxGatherInstance)
	{
		// 최대 1개만 생성가능
		m_pFieldInfo->m_nMaxGatherInstance[GATHER_MINE] = 0;	

		InitForGather(2, GATHER_MINE);

		// mine 스폰
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));
		m_pField->Update(0.0f);

		// 2개 다 스폰됨
		CHECK_EQUAL(2, m_pField->GetEntityQty(ETID_NPC));
	}

	TEST_FIXTURE(Fixture, TestSpawn_IsGatherEntitySpawnEnabled)
	{
		// 최대 1개만 생성가능
		m_pFieldInfo->m_nMaxGatherInstance[GATHER_MINE] = 1;	

		InitForGather(2, GATHER_MINE);

		// mine 스폰
		CHECK_EQUAL(0, m_pField->GetEntityQty(ETID_NPC));
		m_pField->Update(0.0f);

		// 최대 1개만 생성가능하니 하나만 스폰되어야 한다.
		CHECK_EQUAL(1, m_pField->GetEntityQty(ETID_NPC));
	}
}

