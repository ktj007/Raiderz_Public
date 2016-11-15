#include "stdafx.h"
#include "GPlayerSensorCacheUpdater.h"
#include "GEntityPlayer.h"
#include "FBaseFieldInfo.h"
#include "FBasePlayer.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"
#include "GSharedField.h"
#include "GFieldMgr.h"
#include "CCommandTable.h"

SUITE(PlayerSensorCacheUpdater)
{
	class FPlayerSensorCacheUpdater : public FBaseFieldInfo, public FBasePlayer, public FBaseMockLink
	{
	public:
		FPlayerSensorCacheUpdater() : m_nSensorIDInc(0)
		{
			m_pFieldInfo = AddFieldInfo(1, 10000.0f, 10000.0f);
			AddSensorInfo(vec3(1000.0f, 1000.0f, 0.0f), TGT_AUTO);
			AddSensorInfo(vec3(1000.0f, 1200.0f, 0.0f), TGT_AUTO);

			// 멀리 있다
			AddSensorInfo(vec3(9000.0f, 9000.0f, 0.0f), TGT_AUTO);

			m_pField = GUTHelper_Field::NewSharedField(m_pFieldInfo);

			m_pEntityPlayer = NewEntityPlayer(m_pField, vec3(1000.0f, 1000.0f, 0.0f));

			m_pLink = NewLink(m_pEntityPlayer);

			m_pLink->ResetCommands();
		}
		virtual ~FPlayerSensorCacheUpdater()
		{
			m_pEntityPlayer->Destroy();
			m_pField->Destroy();
		}
		void AddSensorInfo(vec3 vPos, GATE_TYPE nGateType)
		{
			m_nSensorIDInc++;

			SENSOR_INFO sensorInfo;
			sensorInfo.nID = m_nSensorIDInc;
			sensorInfo.vPoint = vPos;

			SENSOR_WARP_INFO warp;
			warp.nFieldID = 2;
			warp.eGateType = nGateType;

			sensorInfo.vecWarpInfos.push_back(warp);

			m_pFieldInfo->InsertSensor(sensorInfo);
		}

		GFieldInfo* m_pFieldInfo;
		GField* m_pField;
		GEntityPlayer* m_pEntityPlayer;
		MockLink* m_pLink;
		int m_nSensorIDInc;

		GTestMgrWrapper<GFieldMgr>				m_FieldMgrWrapper;
	};

	TEST_FIXTURE(FPlayerSensorCacheUpdater, TestPlayerSensorCacheUpdater_UpdateNotSend)
	{
		m_pEntityPlayer->UpdateUsableSensor(true);

		// 이미 섹터 정보에서 센서 정보를 보냈기 때문
		CHECK_EQUAL(0, m_pLink->GetCommandCount());	
	}

	TEST_FIXTURE(FPlayerSensorCacheUpdater, TestPlayerSensorCacheUpdater_Update)
	{
		const SENSOR_INFO* pSensorInfo = m_pFieldInfo->FindSensor(1);
		const_cast<SENSOR_INFO*>(pSensorInfo)->vecWarpInfos[0].eGateType = TGT_NONE;	// auto에서 none으로 변경

		m_pEntityPlayer->UpdateUsableSensor(true);
		CHECK_EQUAL(1, m_pLink->GetCommandCount());	

		CHECK_EQUAL(MC_TRIGGER_UPDATE_USABLE_SENSOR, m_pLink->GetCommandID(0));

		minet::MCommand& command = m_pLink->GetCommand(0);
		MCommandParameter* pParam0 = command.GetParameter(0);
		CHECK_EQUAL(2, pParam0->GetBlobCount());

		TD_TRIGGER_SENSOR_INFO* td_info = static_cast<TD_TRIGGER_SENSOR_INFO*>(pParam0->GetPointer());

		CHECK_EQUAL(1, td_info[0].nSensorID);
		CHECK_EQUAL(SENSOR_GATE_TYPE_NA, td_info[0].nGateType);		// TGT_NONE로 바꿨으므로

		CHECK_EQUAL(2, td_info[1].nSensorID);
		CHECK_EQUAL(SENSOR_GATE_TYPE_AUTO, td_info[1].nGateType);

	}
}