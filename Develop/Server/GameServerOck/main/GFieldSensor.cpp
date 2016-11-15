#include "stdafx.h"
#include "GFieldSensor.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GConditionsSystem.h"
#include "GFieldInfo.h"
#include "GField.h"
#include "GConst.h"

void GFieldSensor::MakeUsableSensorInfo( GField* pField, GEntityPlayer* pReqPlayer, vector<TD_TRIGGER_SENSOR_INFO>& vecMarkers )
{
	const GFieldInfo* pFieldInfo = pField->GetInfo();
	VALID(pFieldInfo);
	VALID(pReqPlayer);

	vector<int> vecRawMarkers;
	GetNearbySensors(pFieldInfo, pReqPlayer->GetPos(), SECTOR_SIZE*2, vecRawMarkers);

	for each (int nMarkerID in vecRawMarkers)
	{
		const SENSOR_INFO* pMarker = pFieldInfo->FindSensor(nMarkerID);
		if (!pMarker) 
			continue;

		if (pMarker->bLocal)
			continue; // 로컬 센서는 무시

		if (pMarker->bAutoParty)
			continue; // 파티매칭용 센서는 무시

		if (!IsEnableSensor(pField, *pMarker))
			continue;

		TD_TRIGGER_SENSOR_INFO info;
		info.nSensorID = nMarkerID;
		info.nGateType = GetSensorGateType(pReqPlayer, *pMarker);

		vecMarkers.push_back(info);
	}

	if (GConst::LOG_SENSOR)
	{
		mlog("[SensorLog] MakeUsableSensorList (owner:%s(%u), size:%u)\n", MLocale::ConvUTF16ToAnsi(pReqPlayer->GetName()).c_str(), pReqPlayer->GetUID(), vecMarkers.size());
		for each (const TD_TRIGGER_SENSOR_INFO& each in vecMarkers)
		{
			mlog("\t%d\n", each.nSensorID);
		}
	}
}

void GFieldSensor::GetNearbySensors( const GFieldInfo* pFieldInfo, vec3 vCenter, float fRange, vector<int>& vecSensors )
{
	for each (const SENSOR_INFO_MAP_BY_ID::value_type& pairEach in pFieldInfo->GetSensorInfoContainer())
	{
		float fDistance = pairEach.second.vPoint.DistanceTo(vCenter);

		if (fDistance < fRange+pairEach.second.fRadius)
		{
			vecSensors.push_back(pairEach.first);
		}
	}
}

TD_SENSOR_GATE_TYPE GFieldSensor::GetSensorGateType( GEntityPlayer* pEntityPlayer, const SENSOR_INFO& infoSensor )
{
	VALID_RET(pEntityPlayer, SENSOR_GATE_TYPE_NA);

	for (size_t i = 0; i < infoSensor.vecWarpInfos.size(); i++)
	{
		const SENSOR_WARP_INFO& info = infoSensor.vecWarpInfos[i];

		if (gsys.pConditionsSystem->Check(pEntityPlayer, info.nConditionID))
		{
			if (info.eGateType == TGT_AUTO) 
				return SENSOR_GATE_TYPE_AUTO;
			else if (info.eGateType == TGT_MANUAL) 
				return SENSOR_GATE_TYPE_MANUAL;
		}
	}
	
	return SENSOR_GATE_TYPE_NA;
}

bool GFieldSensor::IsEnableSensor( GField* pField, const SENSOR_INFO& infoSensor )
{
	VALID_RET(pField, false);
	return pField->IsEnableSensor(infoSensor.nID);
}