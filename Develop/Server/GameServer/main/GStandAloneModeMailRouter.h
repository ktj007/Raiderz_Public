#pragma once

#include "GServerMailRouter.h"

class GStandAloneModeMailRouter: public GServerMailRouter, public MTestMemPool<GStandAloneModeMailRouter>
{
public:
	GStandAloneModeMailRouter();
	virtual ~GStandAloneModeMailRouter();

	virtual void RouteNewMailToReceiver(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pMailInfo) override;														
	virtual void RetryTransNewMail(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pMailInfo) override;
};
