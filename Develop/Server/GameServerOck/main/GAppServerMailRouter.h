#pragma once

#include "GServerMailRouter.h"

class GAppServerMailRouter: public GServerMailRouter, public MTestMemPool<GAppServerMailRouter>
{
public:
	GAppServerMailRouter();
	virtual ~GAppServerMailRouter();

	virtual void RouteNewMailToReceiver(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pMailInfo) override;
	virtual void RetryTransNewMail(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pMailInfo) override;
};
