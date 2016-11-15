#include "stdafx.h"
#include "GAppServerMailRouter.h"
#include "GGlobal.h"
#include "GAppServerFacade.h"
#include "CTransData.h"
#include "GCommandTable.h"

GAppServerMailRouter::GAppServerMailRouter()
{
	// do nothing
}

GAppServerMailRouter::~GAppServerMailRouter()
{
	// do nothing
}

void GAppServerMailRouter::RouteNewMailToReceiver(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pMailInfo)
{
	MCommand* pNewCmd = gsys.pAppServerFacade->MakeNewCommand(MPC_MAIL_ROUTE_NEW_MAIL_TO_RECEIVER, 
																2,
																NEW_INT(nCID),
																NEW_SINGLE_BLOB(pMailInfo, sizeof(TD_MAIL_MAILBOX_MAIL_INFO))
																);

	gsys.pAppServerFacade->Route(pNewCmd);
}

void GAppServerMailRouter::RetryTransNewMail(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pMailInfo)
{
	MCommand* pNewCmd = gsys.pAppServerFacade->MakeNewCommand(MPC_MAIL_RETRY_TRANS_NEW_MAIL,
																2,
																NEW_INT(nCID),
																NEW_SINGLE_BLOB(pMailInfo, sizeof(TD_MAIL_MAILBOX_MAIL_INFO))
																);

	gsys.pAppServerFacade->Route(pNewCmd);
}
