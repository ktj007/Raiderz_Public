#pragma once

#include "XSensor.h"

class XQuestSensor : public XSensor
{
public:
	XQuestInfo*		m_pQuestInfo;
	AUTOPARTY_STATE	m_nOldState;

	bool			m_bPostEnter;
	bool			m_bPostLeave;

public:
	XQuestSensor(unsigned long nSensorID, XSensorInfo* pInfo, XQuestInfo* pQuestInfo, GATE_TYPE nGateType=TGT_NONE);
	~XQuestSensor();

	virtual SENSOR_RESULT OnUpdate( bool bEnter, vec3 vPos );
	virtual bool OnEnter(vec3 vPos);
	virtual bool OnLeave(vec3 vPos);

	void Enter();
	void Leave();

	void PostEnter();
	void PostLeave();
};

