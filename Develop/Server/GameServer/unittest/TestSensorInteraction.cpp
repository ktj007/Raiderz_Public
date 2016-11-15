#include "stdafx.h"
#include "GUTHelper.h"
#include "GFieldInfoMgr.h"
#include "GUTHelper_Field.h"
#include "GFieldMgr.h"
#include "MockLink.h"
#include "CCommandTable.h"
#include "GTestWrapper.h"


SUITE(SensorInteraction)
{
	const int TEST_FIELD_ID = 100;
	const int TEST_SENSOR_ID_10 = 10;
	const int TEST_SENSOR_ID_11 = 11;
	const int TEST_SENSOR_ID_12 = 12;
	const int TEST_SENSOR_ID_13 = 13;
	const int TEST_SENSOR_ID_14 = 14;
	const int TEST_SENSOR_ID_15 = 15;
	const int TEST_SENSOR_ID_16 = 16;
	const int TEST_SENSOR_ID_17 = 17;

	const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);

	struct Fixture
	{
		Fixture()
		{
			gsys.pScriptManager->Init();

			InitField();
			InitPlayer();
			InitSensorScript();
		}
		~Fixture()
		{
			gsys.pScriptManager->Fini();
		}
		void InitField()
		{
			m_pFieldInfo = new GFieldInfo;
			m_pFieldInfo->m_strFieldFile = L"mockfield";
			m_pFieldInfo->m_nFieldID = TEST_FIELD_ID;
			m_pFieldInfo->SetFieldMaxXY_ForTest(50000.0f, 50000.0f);
			InitSensors();
			gmgr.pFieldInfoMgr->Insert(m_pFieldInfo);

			m_pField = GUTHelper_Field::NewSharedField(m_pFieldInfo);
			gmgr.pFieldMgr->AddField((GField*)m_pField);
		}
		void InitSensors()
		{
			{
				SENSOR_INFO infoSensor;
				infoSensor.nID = TEST_SENSOR_ID_10;
				infoSensor.fRadius = 1000.0f;
				infoSensor.vPoint = vCommonPos;
				m_pFieldInfo->InsertSensor(infoSensor);
			}
			{
				SENSOR_INFO infoSensor;
				infoSensor.nID = TEST_SENSOR_ID_11;
				infoSensor.fRadius = 1000.0f;
				infoSensor.vPoint = vCommonPos;
				m_pFieldInfo->InsertSensor(infoSensor);
			}
			{
				SENSOR_INFO infoSensor;
				infoSensor.nID = TEST_SENSOR_ID_12;
				infoSensor.fRadius = 1000.0f;
				infoSensor.vPoint = vCommonPos;
				m_pFieldInfo->InsertSensor(infoSensor);
			}
			{
				SENSOR_INFO infoSensor;
				infoSensor.nID = TEST_SENSOR_ID_13;
				infoSensor.fRadius = 1000.0f;
				infoSensor.vPoint = vCommonPos;
				m_pFieldInfo->InsertSensor(infoSensor);
			}
			{
				SENSOR_INFO infoSensor;
				infoSensor.nID = TEST_SENSOR_ID_14;
				infoSensor.fRadius = 1000.0f;
				infoSensor.vPoint = vCommonPos;
				m_pFieldInfo->InsertSensor(infoSensor);
			}
			{
				SENSOR_INFO infoSensor;
				infoSensor.nID = TEST_SENSOR_ID_15;
				infoSensor.fRadius = 1000.0f;
				infoSensor.vPoint = vCommonPos;
				m_pFieldInfo->InsertSensor(infoSensor);
			}
			{
				SENSOR_INFO infoSensor;
				infoSensor.nID = TEST_SENSOR_ID_16;
				infoSensor.fRadius = 1000.0f;
				infoSensor.vPoint = vCommonPos;
				m_pFieldInfo->InsertSensor(infoSensor);
			}
			{
				SENSOR_INFO infoSensor;
				infoSensor.nID = TEST_SENSOR_ID_17;
				infoSensor.fRadius = 1000.0f;
				infoSensor.vPoint = vCommonPos;
				m_pFieldInfo->InsertSensor(infoSensor);
			}
		}
		void InitPlayer()
		{
			GEntityPlayer* pPlayer = m_Helper.NewEntityPlayer((GField*)m_pField, vCommonPos);
			m_pLink = GUTHelper::NewLink(pPlayer->GetUID());

			pPlayer->AddEnteredSensorID(TEST_SENSOR_ID_10);
			pPlayer->AddEnteredSensorID(TEST_SENSOR_ID_11);
			pPlayer->AddEnteredSensorID(TEST_SENSOR_ID_12);
			pPlayer->AddEnteredSensorID(TEST_SENSOR_ID_13);
			pPlayer->AddEnteredSensorID(TEST_SENSOR_ID_14);
			pPlayer->AddEnteredSensorID(TEST_SENSOR_ID_15);
			pPlayer->AddEnteredSensorID(TEST_SENSOR_ID_16);
			pPlayer->AddEnteredSensorID(TEST_SENSOR_ID_17);
		}
		void InitSensorScript()
		{
			string strTableName = MAKE_LUATABLE_FIELD(TEST_FIELD_ID);
			MAKE_TABLE(strTableName.c_str());

			{
				const char* pszScript0 = 
					"function %s(this, Actor)\n"
					"		TEST_VAR0 = 1;\n"
					"end";
				char szTestFuncName[128];
				sprintf_s(szTestFuncName, 128, "Field_%d:OnSensorInteract_%d", TEST_FIELD_ID, TEST_SENSOR_ID_10);
				if (!m_Helper.RunLuaString(pszScript0, szTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }
			}
			{
				const char* pszScript1 = 
					"function %s(this, Actor)\n"
					"		TEST_VAR1 = 1;\n"
					"end";
				char szTestFuncName[128];
				sprintf_s(szTestFuncName, 128, "Field_%d:OnSensorInteract_%d", TEST_FIELD_ID, TEST_SENSOR_ID_11);
				if (!m_Helper.RunLuaString(pszScript1, szTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }
			}
			{
				const char* pszScript2 = 
					"function %s(this, Actor)\n"
					"		TEST_VAR2 = 1;\n"
					"end";
				char szTestFuncName[128];
				sprintf_s(szTestFuncName, 128, "Field_%d:OnSensorInteract_%d", TEST_FIELD_ID, TEST_SENSOR_ID_12);
				if (!m_Helper.RunLuaString(pszScript2, szTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }
			}
			{
				const char* pszScript3 = 
					"function %s(this, Actor)\n"
					"		TEST_VAR3 = 1;\n"
					"end";
				char szTestFuncName[128];
				sprintf_s(szTestFuncName, 128, "Field_%d:OnSensorInteract_%d", TEST_FIELD_ID, TEST_SENSOR_ID_13);
				if (!m_Helper.RunLuaString(pszScript3, szTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }
			}
			{
				const char* pszScript4 = 
					"function %s(this, Actor)\n"
					"		TEST_VAR4 = 1;\n"
					"end";
				char szTestFuncName[128];
				sprintf_s(szTestFuncName, 128, "Field_%d:OnSensorInteract_%d", TEST_FIELD_ID, TEST_SENSOR_ID_14);
				if (!m_Helper.RunLuaString(pszScript4, szTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }
			}
			{
				const char* pszScript5 = 
					"function %s(this, Actor)\n"
					"		TEST_VAR5 = 1;\n"
					"end";
				char szTestFuncName[128];
				sprintf_s(szTestFuncName, 128, "Field_%d:OnSensorInteract_%d", TEST_FIELD_ID, TEST_SENSOR_ID_15);
				if (!m_Helper.RunLuaString(pszScript5, szTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }
			}
			{
				const char* pszScript6 = 
					"function %s(this, Actor)\n"
					"		TEST_VAR6 = 1;\n"
					"end";
				char szTestFuncName[128];
				sprintf_s(szTestFuncName, 128, "Field_%d:OnSensorInteract_%d", TEST_FIELD_ID, TEST_SENSOR_ID_16);
				if (!m_Helper.RunLuaString(pszScript6, szTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }
			}
			{
				const char* pszScript7 = 
					"function %s(this, Actor)\n"
					"		TEST_VAR7 = 1;\n"
					"end";
				char szTestFuncName[128];
				sprintf_s(szTestFuncName, 128, "Field_%d:OnSensorInteract_%d", TEST_FIELD_ID, TEST_SENSOR_ID_17);
				if (!m_Helper.RunLuaString(pszScript7, szTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }
			}
		}

		MCommandResult OnRecv_MC_TRRIGGER_SENSOR_INTERACTION_REQ(vector<uint32>& vecSensorID)
		{
			return m_pLink->OnRecv(MC_TRIGGER_SENSOR_INTERACTION_REQ, 1, NEW_BLOB(vecSensorID));
		}

		GUTHelper		m_Helper;
		GFieldInfo*		m_pFieldInfo;
		GSharedField*	m_pField;
		MockLink*		m_pLink;

		GTestSysWrapper<GScriptManager>				m_ScriptMgr;
		GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
		GTestMgrWrapper<GFieldMgr>					m_FieldMgr;
	};

	TEST_FIXTURE(Fixture, SensorInteraction_Success_Overlapped)
	{
		vector<uint32> vecSensorID;
		vecSensorID.push_back(TEST_SENSOR_ID_10);
		vecSensorID.push_back(TEST_SENSOR_ID_11);
		vecSensorID.push_back(TEST_SENSOR_ID_12);
		vecSensorID.push_back(TEST_SENSOR_ID_13);
		vecSensorID.push_back(TEST_SENSOR_ID_14);
		vecSensorID.push_back(TEST_SENSOR_ID_15);
		vecSensorID.push_back(TEST_SENSOR_ID_16);
		vecSensorID.push_back(TEST_SENSOR_ID_17);
		OnRecv_MC_TRRIGGER_SENSOR_INTERACTION_REQ(vecSensorID);

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR0"), 1);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR1"), 1);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR2"), 1);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR3"), 1);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR4"), 1);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR5"), 1);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR6"), 1);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR7"), 1);
	}

	TEST_FIXTURE(Fixture, SensorInteraction_Failure_Overlapped_Overflow)
	{
		/// 4 개를 초과한 센서를 중첩해서 밟음.
		vector<uint32> vecSensorID;
		vecSensorID.push_back(TEST_SENSOR_ID_10);
		vecSensorID.push_back(TEST_SENSOR_ID_11);
		vecSensorID.push_back(TEST_SENSOR_ID_12);
		vecSensorID.push_back(TEST_SENSOR_ID_13);
		vecSensorID.push_back(TEST_SENSOR_ID_14);
		vecSensorID.push_back(TEST_SENSOR_ID_15);
		vecSensorID.push_back(TEST_SENSOR_ID_16);
		vecSensorID.push_back(TEST_SENSOR_ID_17);
		vecSensorID.push_back(18);
		CHECK_EQUAL(MAX_INTERACTABLE_SENSOR_COUNT+1, vecSensorID.size());
		OnRecv_MC_TRRIGGER_SENSOR_INTERACTION_REQ(vecSensorID);

		/// 처리되지 않음.
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR0"), 0);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR1"), 0);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR2"), 0);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR3"), 0);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR4"), 0);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR5"), 0);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR6"), 0);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR7"), 0);
	}
}