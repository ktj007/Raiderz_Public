#pragma once

#include "XFieldInfo.h"

class XSensorManager;
class XModuleEntity;

class XSensorController
{
private:
	vector<TD_TRIGGER_SENSOR_INFO> m_vecUsableSensorID;
	list<uint32>		m_listEnteredSensorID;
	list<tstring>		m_listEnteredSensorGroupName;
	XSensorManager*		m_pManager;

	void UpdateUsableSensorTrigger(vec3& vMyPos );
	void UpdateUsableGroupSensorTrigger( XFieldInfo* pFieldInfo, vec3& vMyPos );

	void CheckNowFocusedSensorIsInvalid( SENSOR_INFO_MAP_BY_ID &mapSensorByID );

	void EnterSensor(uint32 nSensorID);
	void LeaveSensor(uint32 nSensorID);
	bool IsEnteredSensor(uint32 nSensorID);

	void EnterSensorGroup(tstring strGroupName);
	void LeaveSensorGroup(tstring strGroupName);
	bool IsEnteredSensorGroup(tstring strGroupName);

public:
	XSensorController(XSensorManager* pSensorManager) : m_pManager(pSensorManager) {}

	void Clear();
	void EnteredSensor_PostLeave();

	void Update(XFieldInfo* pFieldInfo, vec3 vMyPos);;
	void CheckEnteredSensor();

	uint32 GetFocusedSensorID(void);
	vector<uint32> GetEnteredSensorID(void);
};
