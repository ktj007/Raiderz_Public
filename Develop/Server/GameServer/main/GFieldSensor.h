#pragma once

#include "CTransData.h"
#include "CSTriggerInfo.h"

class GField;
class GFieldInfo;
class GEntityPlayer;

class GFieldSensor : public MTestMemPool<GFieldSensor>
{
private:
	void GetNearbySensors(const GFieldInfo* pFieldInfo, vec3 vCenter, float fRange, vector<int>& vecSensors);

	TD_SENSOR_GATE_TYPE GetSensorGateType( GEntityPlayer* pEntityPlayer, const SENSOR_INFO& infoSensor );
	bool IsEnableSensor(GField* pField, const SENSOR_INFO& infoSensor);

public:
	GFieldSensor() {}
	~GFieldSensor() {}
	void MakeUsableSensorInfo(GField* pField, GEntityPlayer* pReqPlayer, vector<TD_TRIGGER_SENSOR_INFO>& vecMarkers);
};
