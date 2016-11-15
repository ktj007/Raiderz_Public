#include "stdafx.h"
#include "PCmdRouter_Mail.h"
#include "PCommandCenter.h"
#include "PGlobal.h"
#include "CCommandTable_App.h"

PCmdRouter_Mail::PCmdRouter_Mail()
{
	// do nothing
}

PCmdRouter_Mail::~PCmdRouter_Mail()
{
	// do nothing
}

void PCmdRouter_Mail::TransNewMailReq(MUID uidGameServer, CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo)
{
	MCommand* pNewCmd = gsys.pCommandCenter->MakeNewCommand(MPC_MAIL_TRANS_NEW_MAIL, 
															2, 
															NEW_INT(nCID), 
															NEW_SINGLE_BLOB(pInfo, sizeof(TD_MAIL_MAILBOX_MAIL_INFO))
															);

	gsys.pCommandCenter->SendCommand(uidGameServer, pNewCmd);
}
