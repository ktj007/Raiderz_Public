#include "stdafx.h"
#include "GStandAloneModeMailRouter.h"
#include "GPlayerObject.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GMailSystem.h"

GStandAloneModeMailRouter::GStandAloneModeMailRouter()
{
	// do nothing
}

GStandAloneModeMailRouter::~GStandAloneModeMailRouter()
{
	// do nothing
}

void GStandAloneModeMailRouter::RouteNewMailToReceiver(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pMailInfo)
{
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(nCID);

	if (pPlayerObject == NULL)
	{
		return;
	}


	GEntityPlayer* pPlayer = pPlayerObject->GetEntity();
	gsys.pMailSystem->UpdateMailBox(pPlayer, pMailInfo);
}

void GStandAloneModeMailRouter::RetryTransNewMail(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pMailInfo)
{
	//	do nothing
}
