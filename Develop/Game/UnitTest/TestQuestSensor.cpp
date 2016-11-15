#include "stdafx.h"
#include "XFieldInfo.h"
#include "XSensorManager.h"
#include "XSensorController.h"
#include "MMockNetClient.h"
#include "XNetwork.h"


SUITE(QuestSensor)
{
	class FSensor
	{
	public:
		FSensor()
		{
			m_pNetAgent = new minet::MTestNetAgent(global.net->GetClient());
			SetupFieldInfo();
		}
		~FSensor()
		{
			SAFE_DELETE(m_pNetAgent);
		}
		void SetupFieldInfo()
		{
			AddSensorInfo(999, false);
		}
		void AddSensorInfo(unsigned long nID, bool bLocal, vec3 vPoint=vec3::ZERO)
		{
			XSensorInfo info = MakeSensorInfo(nID, bLocal, vPoint);
			m_FieldInfo.m_mapSensorByID.insert(SENSOR_INFO_MAP_BY_ID::value_type(nID, info));
		}
		XSensorInfo MakeSensorInfo(unsigned long nID, bool bLocal, vec3 vPoint)
		{
			XSensorInfo info;
			info.strGroupName = L"none";
			info.nID = nID;
			info.bLocal = bLocal;
			info.vPoint = vPoint;
			info.fRadius = 50;
			info.bAutoParty = true;
			info.nQuestID = 999;

			return info;
		}
		void InitMgr()
		{
			vector<TD_TRIGGER_SENSOR_INFO>	vecSensors;

			m_Mgr.Init(&m_FieldInfo, vecSensors);
		}
		void InitQuestMgr()
		{
			/// 999 퀘스트 추가
			XQuestInfo* pQuestInfo = static_cast<XQuestInfo*>(info.quest->NewQuestInfo());
			pQuestInfo->nID = 999;
			info.quest->Insert(pQuestInfo);
		}

		XFieldInfo		m_FieldInfo;
		XSensorManager	m_Mgr;
		minet::MTestNetAgent*	m_pNetAgent;

	};

	TEST_FIXTURE(FSensor, TestSensorManager_Insert)
	{
		InitQuestMgr();
		InitMgr();
		CHECK_EQUAL(1, m_Mgr.GetCount()); //퀘스트 센서 추가
	}

	TEST_FIXTURE(FSensor, QuestSensor_Enter_NoHaveQuest)
	{
		InitQuestMgr();
		InitMgr();
		CHECK_EQUAL(1, m_Mgr.GetCount()); //퀘스트 센서 추가

		XSensorController* pController = m_Mgr.GetController();
		vector<uint32> vecList = pController->GetEnteredSensorID();
		CHECK_EQUAL(vecList.size(), 0);

		pController->Update(&m_FieldInfo, vec3::ZERO);

		CHECK_EQUAL(MC_AUTOPARTY_CHANGE_STATE_REQ, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());

		vecList = pController->GetEnteredSensorID();
		CHECK_EQUAL(vecList.size(), 1);
	}

	TEST_FIXTURE(FSensor, QuestSensor_EnterLeave_HaveQuest)
	{
		InitQuestMgr();
		InitMgr();
		CHECK_EQUAL(1, m_Mgr.GetCount()); //퀘스트 센서 추가

		gvar.MyInfo.Quests.Insert(999);

		// Enter
		XSensorController* pController = m_Mgr.GetController();
		vector<uint32> vecList = pController->GetEnteredSensorID();
		CHECK_EQUAL(vecList.size(), 0);

		pController->Update(&m_FieldInfo, vec3::ZERO);

		CHECK_EQUAL(MC_AUTOPARTY_ENQUEUE_REQ, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());

		vecList = pController->GetEnteredSensorID();
		CHECK_EQUAL(vecList.size(), 1);

		// Leave
		m_pNetAgent->ResetSendCommands();

		pController->Update(&m_FieldInfo, vec3(100.f, 100.f, 0.f));

		CHECK_EQUAL(MC_AUTOPARTY_DEQUEUE_REQ, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());

		vecList = pController->GetEnteredSensorID();
		CHECK_EQUAL(vecList.size(), 0);
	}
}