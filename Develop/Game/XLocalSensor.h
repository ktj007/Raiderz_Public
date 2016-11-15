#pragma once

#include "XSensor.h"

class XLocalSensor : public XSensor
{

public:
	XLocalSensor(unsigned long nSensorID, XSensorInfo* pInfo, GATE_TYPE nGateType);
	~XLocalSensor();

	virtual bool OnEnter(vec3 vPos);
	virtual bool OnLeave(vec3 vPos);
};

