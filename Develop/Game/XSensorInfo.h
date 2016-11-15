#pragma once

#include "CSTriggerInfo.h"

enum SENSOR_RESULT
{
	SENSOR_RESULT_NONE = 0,
	SENSOR_RESULT_ENTER = 1,
	SENSOR_RESULT_LEAVE
};

class XSensorInfo : public SENSOR_INFO
{
public:
	wstring		strGroupName;	///< 그룹 이름
	int			nQuestID;

public:
	XSensorInfo();
	~XSensorInfo();
};

