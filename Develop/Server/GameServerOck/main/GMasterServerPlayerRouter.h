#pragma once

#include "GServerPlayerRouter.h"

class GMasterServerPlayerRouter: public GServerPlayerRouter
{
public:
	GMasterServerPlayerRouter();
	virtual ~GMasterServerPlayerRouter();
	
	virtual void RouteDeletePlayer(int nAID, const MUID& uidPlayer) override;

	virtual void RoutePlayerState_InWorldReq(const GEntityPlayer* pEntityPlayer) override;
	virtual void RoutePlayerInFieldNotify(const GEntityPlayer* pEntityPlayer, MUID uidField, int nFieldID) override;
};
