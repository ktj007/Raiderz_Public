#pragma once

#include "STypes.h"

class GEntityPlayer;

class GServerPlayerRouter
{
public:	
	virtual void RouteDeletePlayer(int nAID, const MUID& uidPlayer) = 0;

	virtual void RoutePlayerState_InWorldReq(const GEntityPlayer* pEntityPlayer) = 0;
	virtual void RoutePlayerInFieldNotify(const GEntityPlayer* pEntityPlayer, MUID uidField, int nFieldID) = 0;
};
