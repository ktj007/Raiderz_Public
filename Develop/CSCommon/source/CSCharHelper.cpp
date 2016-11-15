#include "stdafx.h"
#include "CSCharHelper.h"
#include "CSDef.h"

float CSCharHelper::CalcValidInteractionDistance( float fTarColRadius, int nInteractionDistance )
{
	float fRet;
	
	if (nInteractionDistance > 0)
	{
		fRet = (float)nInteractionDistance;
	}
	else
	{
		fRet = (fTarColRadius * GAME_INTERACTION_DISTANCE_XY_MULTIPLICATION) + GAME_INTERACTION_DISTANCE_XY_ADDITION;
	}

	return fRet;
}