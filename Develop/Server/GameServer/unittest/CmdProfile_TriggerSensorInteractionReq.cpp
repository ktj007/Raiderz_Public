#include "stdafx.h"
#include "GUnitTestRunner.h"
#include "SUnitTest.h"
#include "FBaseFieldInfo.h"
#include "FBaseFieldMgr.h"
#include "GUTHelper_Field.h"
#include "GUTHelper.h"
#include "GPlayerField.h"
#include "GPlayerFieldEntry.h"
#include "GTestWrapper.h"
#include "MockDBMAnager.h"
#include "GConfig.h"
#include "GConditionsSystem.h"
#include "GFieldFactory.h"
#include "GSharedFieldMaster.h"
#include "GSharedField.h"
#include "GDynamicFieldMaster.h"
#include "GDynamicField.h"
#include "GServer.h"


SUITE(CmdProfile_TriggerSensorInteractionReq)
{
	const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
	const vec3 vCommonDir = vec3(0,1,0);

	class SuccessConditionsSystem: public GConditionsSystem
	{
	public:
		SuccessConditionsSystem()
		{
		}

		virtual bool Check(GEntityPlayer* pPlayer, int nConditionID) override
		{
			return true;
		}
	};

	struct FStandAlone
	{
		FStandAlone() { GConfig::m_bStandAlone = true; }
		~FStandAlone() { GConfig::m_bStandAlone = false; }
	};

	struct Fixture : public FStandAlone, public FBaseFieldMgr
	{
		Fixture()
		{
			InitSharedField();
			InitPlayer();
		}
		virtual ~Fixture()
		{
			
		}
		void InitSharedField()
		{
			MUID uidField;
			uidField = gsys.pServer->NewUID();
			gmgr.pFieldMgr->AddSharedField(uidField, 3, 1);	///< 인젠 자유항

			uidField = gsys.pServer->NewUID();
			gmgr.pFieldMgr->AddSharedField(uidField, 107, 1);	///< 테레스 평원

			uidField = gsys.pServer->NewUID();
			gmgr.pFieldMgr->AddSharedField(uidField, 110, 1);	///< 렌고트 부락
		}
		void InitPlayer()
		{
			GFieldMgr::SHARED_FIELD_MASTER_MAP::iterator itr = gmgr.pFieldMgr->GetSharedFieldMap().begin();
			GSharedField* pField = itr->second->GetChannel(1);
			m_pPlayer = m_Helper.NewEntityPlayer((GField*)pField, vCommonPos);
		}

		void SimulateInteractionSensor(int nFieldID, const vec3& vSensorPos, int nSensorID)
		{
			m_pPlayer->GetPlayerField().Gate(nFieldID, vSensorPos, vCommonDir, false);
			gmgr.pFieldMgr->Update(0.f);
			m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

			m_pPlayer->AddEnteredSensorID(nSensorID);

			vector<uint32> vecSensor;
			vecSensor.push_back(nSensorID);
			{
				AutoTestCaseTimer(AutoTestCaseTimer::SFormat(L"Field:%d, Sensor:%d", nFieldID, nSensorID));
				OnRecv_MC_TRIGGER_SENSOR_INTERACTION_REQ(vecSensor);
			}

			m_pPlayer->RemoveEnteredSensorID(nSensorID);
		}

		void OnRecv_MC_TRIGGER_SENSOR_INTERACTION_REQ(vector<uint32>& vecSensorID)
		{
			m_pPlayer->SensorInteraction(vecSensorID);
			//m_pLink->OnRecv(MC_TRIGGER_SENSOR_INTERACTION_REQ
			//				, 1
			//				, NEW_BLOB(vecSensorID));
		}

		GUTHelper m_Helper;
		MUID m_uidField;

		GEntityPlayer* m_pPlayer;
		GTestSysWrapper2<GDBManager, MockDBManager> m_DBManager;
		GTestSysWrapper2<GConditionsSystem, SuccessConditionsSystem> m_SuccessConditionsSystem;
	};
	
	TEST_FIXTURE4(Fixture, CmdProfile_TriggerSensorInteractionReq_InteractAllSharedFieldAllSensor)
	{
		GFieldMgr::SHARED_FIELD_MASTER_MAP::iterator itr = gmgr.pFieldMgr->GetSharedFieldMap().begin();
		for (; itr != gmgr.pFieldMgr->GetSharedFieldMap().end(); ++itr)
		{
			GSharedField* pSharedField = itr->second->GetChannel(1);

			SENSOR_INFO_MAP_BY_ID::const_iterator itrSensor = pSharedField->GetInfo()->GetSensorInfoContainer().begin();
			for (; itrSensor != pSharedField->GetInfo()->GetSensorInfoContainer().end(); ++itrSensor)
			{
				const SENSOR_INFO& refSensor = itrSensor->second;

				SimulateInteractionSensor(pSharedField->GetInfo()->m_nFieldID, refSensor.vPoint, refSensor.nID);
			}
		}
	}

	TEST_FIXTURE4(Fixture, CmdProfile_TriggerSensorInteractionReq_InteractAllSharedFieldAllSensor_For100)
	{
		GFieldMgr::SHARED_FIELD_MASTER_MAP::iterator itr = gmgr.pFieldMgr->GetSharedFieldMap().begin();
		for (; itr != gmgr.pFieldMgr->GetSharedFieldMap().end(); ++itr)
		{
			GSharedField* pSharedField = itr->second->GetChannel(1);

			SENSOR_INFO_MAP_BY_ID::const_iterator itrSensor = pSharedField->GetInfo()->GetSensorInfoContainer().begin();
			for (; itrSensor != pSharedField->GetInfo()->GetSensorInfoContainer().end(); ++itrSensor)
			{
				const SENSOR_INFO& refSensor = itrSensor->second;

				for (int i = 0; i < 100; ++i)
				{
					SimulateInteractionSensor(pSharedField->GetInfo()->m_nFieldID, refSensor.vPoint, refSensor.nID);
				}
			}
		}
	}

	TEST_FIXTURE4(Fixture, CmdProfile_TriggerSensorInteractionReq_InteractAllDynamicFieldAllSensor)
	{
		// 동적필드 모두 1개씩 생성
		GFieldInfoMgr::FIELD_GROUP_MAP::iterator itrFieldGroupInfo = gmgr.pFieldInfoMgr->GetFieldGroupMap().begin();
		for (; itrFieldGroupInfo != gmgr.pFieldInfoMgr->GetFieldGroupMap().end(); ++itrFieldGroupInfo)
		{
			FIELD_GROUP_INFO* pFieldGroupInfo = itrFieldGroupInfo->second;
			gmgr.pFieldMgr->CreateDynamicFieldMaster(pFieldGroupInfo->nID, DFT_TRIAL, m_pPlayer->GetUID());
		}

		GFieldMgr::DYNAMIC_FIELD_GROUP_MAP::iterator itrFIeldGroup = gmgr.pFieldMgr->GetDynamicFieldGroupMap().begin();
		for (; itrFIeldGroup != gmgr.pFieldMgr->GetDynamicFieldGroupMap().end(); ++itrFIeldGroup)
		{
			GDynamicFieldMaster* pFieldGroup = itrFIeldGroup->second;

			MAP_DYNAMICFIELD::iterator itrDynamicField = pFieldGroup->GetFieldContainer().begin();
			for (; itrDynamicField != pFieldGroup->GetFieldContainer().end(); ++itrDynamicField)
			{
				GDynamicField* pDynamicField = itrDynamicField->second;

				SENSOR_INFO_MAP_BY_ID::const_iterator itrSensor = pDynamicField->GetInfo()->GetSensorInfoContainer().begin();
				for (; itrSensor != pDynamicField->GetInfo()->GetSensorInfoContainer().end(); ++itrSensor)
				{
					const SENSOR_INFO& refSensor = itrSensor->second;

					m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(pFieldGroup->GetUID());
					SimulateInteractionSensor(pDynamicField->GetInfo()->m_nFieldID, refSensor.vPoint, refSensor.nID);
				}
			}
		}
	}

	TEST_FIXTURE4(Fixture, CmdProfile_TriggerSensorInteractionReq_InteractAllDynamicFieldAllSensor_For10)
	{
		// 동적필드 모두 1개씩 생성
		GFieldInfoMgr::FIELD_GROUP_MAP::iterator itrFieldGroupInfo = gmgr.pFieldInfoMgr->GetFieldGroupMap().begin();
		for (; itrFieldGroupInfo != gmgr.pFieldInfoMgr->GetFieldGroupMap().end(); ++itrFieldGroupInfo)
		{
			FIELD_GROUP_INFO* pFieldGroupInfo = itrFieldGroupInfo->second;
			gmgr.pFieldMgr->CreateDynamicFieldMaster(pFieldGroupInfo->nID, DFT_TRIAL, m_pPlayer->GetUID());
		}

		GFieldMgr::DYNAMIC_FIELD_GROUP_MAP::iterator itrFIeldGroup = gmgr.pFieldMgr->GetDynamicFieldGroupMap().begin();
		for (; itrFIeldGroup != gmgr.pFieldMgr->GetDynamicFieldGroupMap().end(); ++itrFIeldGroup)
		{
			GDynamicFieldMaster* pFieldGroup = itrFIeldGroup->second;

			MAP_DYNAMICFIELD::iterator itrDynamicField = pFieldGroup->GetFieldContainer().begin();
			for (; itrDynamicField != pFieldGroup->GetFieldContainer().end(); ++itrDynamicField)
			{
				GDynamicField* pDynamicField = itrDynamicField->second;

				SENSOR_INFO_MAP_BY_ID::const_iterator itrSensor = pDynamicField->GetInfo()->GetSensorInfoContainer().begin();
				for (; itrSensor != pDynamicField->GetInfo()->GetSensorInfoContainer().end(); ++itrSensor)
				{
					const SENSOR_INFO& refSensor = itrSensor->second;

					for (int i = 0; i < 10; ++i)
					{
						m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(pFieldGroup->GetUID());
						SimulateInteractionSensor(pDynamicField->GetInfo()->m_nFieldID, refSensor.vPoint, refSensor.nID);
					}
				}
			}
		}
	}

	TEST_FIXTURE4(Fixture, CmdProfile_TriggerSensorInteractionReq_InteractDynamicField_1070540_1070542_Sensor_3)
	{
		GDynamicFieldMaster* pFieldGroup = NULL;

		GFieldInfoMgr::FIELD_GROUP_MAP::iterator itrFieldGroupInfo = gmgr.pFieldInfoMgr->GetFieldGroupMap().find(1070540);
		if (itrFieldGroupInfo != gmgr.pFieldInfoMgr->GetFieldGroupMap().end())
		{
			FIELD_GROUP_INFO* pFieldGroupInfo = itrFieldGroupInfo->second;
			pFieldGroup = gmgr.pFieldMgr->CreateDynamicFieldMaster(pFieldGroupInfo->nID, DFT_TRIAL, m_pPlayer->GetUID());
		}
		VALID(NULL != pFieldGroup);

		MAP_DYNAMICFIELD::iterator itrDynamicField = pFieldGroup->GetFieldContainer().find(1070542);
		if (itrDynamicField != pFieldGroup->GetFieldContainer().end())
		{
			GDynamicField* pDynamicField = itrDynamicField->second;

			SENSOR_INFO_MAP_BY_ID::const_iterator itrSensor = pDynamicField->GetInfo()->GetSensorInfoContainer().find(3);
			if (itrSensor != pDynamicField->GetInfo()->GetSensorInfoContainer().end())
			{
				const SENSOR_INFO& refSensor = itrSensor->second;

				for (int i = 0; i < 10; ++i)
				{
					m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(pFieldGroup->GetUID());
					SimulateInteractionSensor(pDynamicField->GetInfo()->m_nFieldID, refSensor.vPoint, refSensor.nID);
				}
			}
		}
	}

}