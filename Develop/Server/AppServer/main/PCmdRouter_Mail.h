#pragma once

#include "STypes.h"
#include "CTransData.h"

class PCmdRouter_Mail
{
public:
	PCmdRouter_Mail();
	virtual ~PCmdRouter_Mail();

	virtual void TransNewMailReq(MUID uidGameServer, CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo);
};
