#include "stdafx.h"
#include "XLocalSensor.h"
#include "XGameState.h"

XLocalSensor::XLocalSensor( unsigned long nSensorID, XSensorInfo* pInfo, GATE_TYPE nGateType )
: XSensor(nSensorID, pInfo, nGateType)
{

}

XLocalSensor::~XLocalSensor()
{

}

bool XLocalSensor::OnEnter(vec3 vPos)
{
	if (gg.currentgamestate == NULL || gg.currentgamestate->GetPlayState() == GPS_CUTSCENE) return false;

	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if (pFieldInfo==NULL) return false;

	if(global.script) global.script->GetGlueFieldEvent().OnSensorEnter(pFieldInfo, m_nID, NULL);

	return true;
}

bool XLocalSensor::OnLeave(vec3 vPos)
{
	if (gg.currentgamestate == NULL || gg.currentgamestate->GetPlayState() == GPS_CUTSCENE) return false;

	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if (pFieldInfo==NULL) return false;

	if(global.script) global.script->GetGlueFieldEvent().OnSensorLeave(pFieldInfo, m_nID, NULL);

	return true;
}
