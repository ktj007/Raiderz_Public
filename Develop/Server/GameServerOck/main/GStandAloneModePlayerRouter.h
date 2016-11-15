#pragma once

#include "GServerPlayerRouter.h"

class GStandAloneModePlayerRouter: public GServerPlayerRouter
{
public:
	GStandAloneModePlayerRouter();
	virtual ~GStandAloneModePlayerRouter();
	
	virtual void RouteDeletePlayer(int nAID, const MUID& uidPlayer) override;

	virtual void RoutePlayerState_InWorldReq(const GEntityPlayer* pEntityPlayer) override;
	virtual void RoutePlayerInFieldNotify(const GEntityPlayer* pEntityPlayer, MUID uidField, int nFieldID) override;
};
