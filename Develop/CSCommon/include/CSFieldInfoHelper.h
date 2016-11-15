#pragma once

#include "CSCommonLib.h"
#include "MTypes.h"

class CSCOMMON_API CSFieldInfoHelper
{
public:
	static MPoint ConvertToGridPos(const float fPosX, const float fPosY, const float fFieldMinX, const float fFieldMinY);
};