#include "stdafx.h"
#include "XFieldInfo.h"
#include "XSensorManager.h"
#include "XSensor.h"
#include "XSensorController.h"

SUITE(Sensor)
{
	class FSensor
	{
	public:
		FSensor()
		{
			SetupFieldInfo();
		}
		~FSensor()
		{

		}
		void SetupFieldInfo()
		{
			AddSensorInfo(1, false);
			AddSensorInfo(2, false);
			AddSensorInfo(3, false);
			AddSensorInfo(4, false);
			AddSensorInfo(5, false);

			AddSensorInfo(100, true);
			AddSensorInfo(101, true);
		}

		void AddSensorInfo(unsigned long nID, bool bLocal, vec3 vPoint=vec3::ZERO)
		{
			XSensorInfo info = MakeSensorInfo(nID, bLocal, vPoint);

			m_FieldInfo.m_mapSensorByID.insert(SENSOR_INFO_MAP_BY_ID::value_type(nID,
				info));
		}

		XSensorInfo MakeSensorInfo(unsigned long nID, bool bLocal, vec3 vPoint)
		{
			XSensorInfo info;
			info.strGroupName = L"none";
			info.nID = nID;
			info.bLocal = bLocal;
			info.vPoint = vPoint;
			return info;
		}

		void InitMgr_1_2_100()
		{
			vector<TD_TRIGGER_SENSOR_INFO>	vecSensors;

			vecSensors.push_back(TD_TRIGGER_SENSOR_INFO(1, SENSOR_GATE_TYPE_NA));
			vecSensors.push_back(TD_TRIGGER_SENSOR_INFO(2, SENSOR_GATE_TYPE_NA));
			vecSensors.push_back(TD_TRIGGER_SENSOR_INFO(100, SENSOR_GATE_TYPE_NA));

			m_Mgr.Init(&m_FieldInfo, vecSensors);
		}

		XFieldInfo		m_FieldInfo;
		XSensorManager		m_Mgr;
	};

	TEST_FIXTURE(FSensor, TestSensorManager_Insert)
	{
		InitMgr_1_2_100();
		CHECK_EQUAL(4, m_Mgr.GetCount()); //로컬 센서 추가

		// id 1
		XSensor* pSensor = m_Mgr.Find(1);
		ASSERT_CHECK(pSensor != NULL);

		CHECK_EQUAL(1, pSensor->GetID());
		ASSERT_CHECK(pSensor->GetInfo() != NULL);
		CHECK_EQUAL(false, pSensor->GetInfo()->bLocal);
		CHECK_EQUAL(vec3::ZERO, pSensor->GetInfo()->vPoint);

		// id 100
		pSensor = m_Mgr.Find(100);
		ASSERT_CHECK(pSensor != NULL);

		CHECK_EQUAL(100, pSensor->GetID());
		CHECK_EQUAL(true, pSensor->GetInfo()->bLocal);

		// id 500 - 없음
		pSensor = m_Mgr.Find(500);
		CHECK(pSensor == NULL);
	}

	TEST_FIXTURE(FSensor, TestSensorManager_Init_Again)
	{
		InitMgr_1_2_100();
		CHECK_EQUAL(4, m_Mgr.GetCount());

		// id 1
		XSensor* pSensor1 = m_Mgr.Find(1);
		XSensor* pSensor2 = m_Mgr.Find(2);
		XSensor* pSensor100 = m_Mgr.Find(100);


		vector<TD_TRIGGER_SENSOR_INFO>	vecSensors;
		vecSensors.push_back(TD_TRIGGER_SENSOR_INFO(1, SENSOR_GATE_TYPE_NA));
		vecSensors.push_back(TD_TRIGGER_SENSOR_INFO(2, SENSOR_GATE_TYPE_NA));
		vecSensors.push_back(TD_TRIGGER_SENSOR_INFO(3, SENSOR_GATE_TYPE_NA));
		vecSensors.push_back(TD_TRIGGER_SENSOR_INFO(4, SENSOR_GATE_TYPE_NA));
		vecSensors.push_back(TD_TRIGGER_SENSOR_INFO(5, SENSOR_GATE_TYPE_NA));

		// 1,2는 있었고 3,4,5는 새로 추가됨
		m_Mgr.Init(&m_FieldInfo, vecSensors);

		CHECK_EQUAL(7, m_Mgr.GetCount());

		// 다시 find해도 기존의 포인터와 같음
		XSensor* pNewSensor1 = m_Mgr.Find(1);
		XSensor* pNewSensor2 = m_Mgr.Find(2);
		
		CHECK(pNewSensor1 == pSensor1);
		CHECK(pNewSensor2 == pSensor2);
	}

	TEST_FIXTURE(FSensor, TestSensorManager_Clear)
	{
		InitMgr_1_2_100();

		CHECK_EQUAL(4, m_Mgr.GetCount());

		m_Mgr.Clear();
		CHECK_EQUAL(0, m_Mgr.GetCount());
	}

	TEST_FIXTURE(FSensor, TestSensorManager_RefreshSensors)
	{
		// 섹터 인스턴스를 모두 갱신한다.

		InitMgr_1_2_100();

		vector<TD_TRIGGER_SENSOR_INFO>	vecNewSensors;

		vecNewSensors.push_back(TD_TRIGGER_SENSOR_INFO(1, SENSOR_GATE_TYPE_NA));
		vecNewSensors.push_back(TD_TRIGGER_SENSOR_INFO(4, SENSOR_GATE_TYPE_NA));
		vecNewSensors.push_back(TD_TRIGGER_SENSOR_INFO(5, SENSOR_GATE_TYPE_NA));
		vecNewSensors.push_back(TD_TRIGGER_SENSOR_INFO(101, SENSOR_GATE_TYPE_NA));

		m_Mgr.Init(&m_FieldInfo, vecNewSensors);

		CHECK_EQUAL(5, m_Mgr.GetCount());

		// id 1
		XSensor* pSensor = m_Mgr.Find(1);
		ASSERT_CHECK(pSensor != NULL);
		CHECK_EQUAL(1, pSensor->GetID());

		// id 2
		pSensor = m_Mgr.Find(2);
		CHECK(pSensor == NULL);

		// id 4
		pSensor = m_Mgr.Find(4);
		ASSERT_CHECK(pSensor != NULL);
		CHECK_EQUAL(4, pSensor->GetID());

	}

	TEST(TestSensorManager_ConvertGateType)
	{
		CHECK_EQUAL(TGT_NONE, XSensorManager::ConvertGateType(SENSOR_GATE_TYPE_NA));
		CHECK_EQUAL(TGT_AUTO, XSensorManager::ConvertGateType(SENSOR_GATE_TYPE_AUTO));
		CHECK_EQUAL(TGT_MANUAL, XSensorManager::ConvertGateType(SENSOR_GATE_TYPE_MANUAL));
	}

	TEST_FIXTURE(FSensor, TestUsableSensor)
	{
		InitMgr_1_2_100();

		// 1번 센서 켜짐
		vector<TD_TRIGGER_SENSOR_INFO>	vecNewSensors;
		vecNewSensors.push_back(TD_TRIGGER_SENSOR_INFO(1, SENSOR_GATE_TYPE_NA));
		m_Mgr.Init(&m_FieldInfo, vecNewSensors);

		XSensorController* pController = m_Mgr.GetController();

		vector<uint32> vecList = pController->GetEnteredSensorID();
		CHECK_EQUAL(vecList.size(), 0);

		pController->Update(&m_FieldInfo, vec3::ZERO);

		vecList = pController->GetEnteredSensorID();

		CHECK_EQUAL(vecList.size(), 1);
	}
}