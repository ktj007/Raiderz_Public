#pragma once

#include "CTransData.h"
#include "CSTriggerInfo.h"

class XSensor;
class XFieldInfo;
class XSensorController;

/// 센서 인스턴스 관리자
class XSensorManager
{
public:
	typedef map<int, XSensor*>		XSensorMap;
private:
	XSensorMap				m_SensorMap;
	XSensorController*		m_pController;
	XFieldInfo*				m_pFieldInfo;

	void Insert(XFieldInfo* pFieldInfo, TD_TRIGGER_SENSOR_INFO tdSensorInfo);
	void InsertSensor(XFieldInfo* pFieldInfo);

public:
	XSensorManager();
	~XSensorManager();
	
	void Init(XFieldInfo* pFieldInfo, vector<TD_TRIGGER_SENSOR_INFO>& vecID);
	void Clear();

	static GATE_TYPE ConvertGateType(TD_SENSOR_GATE_TYPE tdGateType);

	XSensorController* GetController() { return m_pController; }
	XSensorMap& GetSensorMap() { return m_SensorMap; }

	// for Test
	XSensor* Find(int nSensorID);
	size_t GetCount() { return m_SensorMap.size(); }
};