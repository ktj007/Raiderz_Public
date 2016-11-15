#ifndef _XPOST_SENSOR_H
#define _XPOST_SENSOR_H

// 센서 워프목록 선택
inline void XPostSensorTriggerSelection(int nSelection)
{
	XPOSTCMD1(MC_TRIGGER_SENSOR_DESTINATION_SEL, MCommandParameterInt(nSelection));
}

// 파티원 따라 트라이얼필드 이동 선택
inline void XPosSensorTriggerFollowWarp(int nSelection)
{
	if (nSelection != 0)	return;
	XPOSTCMD0(MC_TRIGGER_SENSOR_FOLLOW_PARTY_RES);
}

// 센서안에서 인터랙트
inline void XPostSensorInteract(vector<uint32>& vecEnteredSensorID)
{
	XPOSTCMD1(MC_TRIGGER_SENSOR_INTERACTION_REQ, MCommandParameterBlob(vecEnteredSensorID));
}

#endif