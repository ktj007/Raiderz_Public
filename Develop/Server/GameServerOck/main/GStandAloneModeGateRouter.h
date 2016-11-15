#pragma once

#include "GServerGateRouter.h"

struct GGateInfo;

class GStandAloneModeGateRouter : public GServerGateRouter, public MTestMemPool<GStandAloneModeGateRouter>
{
public:
	GStandAloneModeGateRouter()		{}
	~GStandAloneModeGateRouter()	{}

	void PrepareChangeChannelReq(MUID uidRequestPlayer, int nChannelID) override;
	void CancelChangeChannel(MUID uidRequestPlayer) override;
	void AskGateChangeChannel(MUID uidRequestPlayer, const GGateInfo* pGateInfo) override;
};
