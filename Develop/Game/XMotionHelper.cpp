#include "stdafx.h"
#include "XMotionHelper.h"


bool XMotionHelper::IsIdleMotion( wstring& strMotionName )
{
	if (strMotionName == MOTION_NAME_IDLE  ||  
		strMotionName == MOTION_NAME_SWIM_IDLE ||
		strMotionName == MOTION_NAME_NPC_IDLE ||
		strMotionName == MOTION_NAME_NPC_IDLE_COMBAT)
	{
		return true;
	}
	return false;
}