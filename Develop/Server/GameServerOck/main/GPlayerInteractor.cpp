#include "StdAfx.h"
#include "GPlayerInteractor.h"

bool GPlayerInteractor::Interaction(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer)
{
	if (NULL == pReqPlayer) return false;
	if (NULL == pTarPlayer) return false;

	return true;
}