#include "stdafx.h"
#include "GAppServerChatChannelRouter.h"
#include "GGlobal.h"
#include "GAppServerFacade.h"
#include "CCommandTable_App.h"

bool GAppServerChatChannelRouter::CreateChannelReq(CID cidPlayer, wstring strChannelName)
{
	MCommand* pNewCmd = gsys.pAppServerFacade->MakeNewCommand(MPC_MSG_CREATE_PRIVATE_CHANNEL_REQ,
																2,
																NEW_INT(cidPlayer),
																NEW_WSTR(strChannelName.c_str())
																);
	return gsys.pAppServerFacade->Route(pNewCmd);
}

bool GAppServerChatChannelRouter::EnterChannelReq(CID cidPlayer, wstring strChannelName)
{
	MCommand* pNewCmd = gsys.pAppServerFacade->MakeNewCommand(MPC_MSG_ENTER_CHANNEL_REQ,
																2,
																NEW_INT(cidPlayer),
																NEW_WSTR(strChannelName.c_str())
																);
	return gsys.pAppServerFacade->Route(pNewCmd);
}

bool GAppServerChatChannelRouter::LeaveChannelReq(CID cidPlayer, MUID uidChannel)
{
	MCommand* pNewCmd = gsys.pAppServerFacade->MakeNewCommand(MPC_MSG_LEAVE_CHANNEL_REQ,
																2,
																NEW_INT(cidPlayer),
																NEW_UID(uidChannel)
																);	
	return gsys.pAppServerFacade->Route(pNewCmd);
}

bool GAppServerChatChannelRouter::KickPlayerReq(CID cidPlayer, MUID uidChannel, wstring strTargetPlayerName)
{
	MCommand* pNewCmd = gsys.pAppServerFacade->MakeNewCommand(MPC_MSG_KICK_REQ,
																3,
																NEW_INT(cidPlayer),
																NEW_UID(uidChannel),
																NEW_WSTR(strTargetPlayerName.c_str())
																);
	return gsys.pAppServerFacade->Route(pNewCmd);
}

bool GAppServerChatChannelRouter::BanPlayerReq(CID cidPlayer, MUID uidChannel, wstring strTargetPlayerName)
{
	MCommand* pNewCmd = gsys.pAppServerFacade->MakeNewCommand(MPC_MSG_BAN_REQ,
																3,
																NEW_INT(cidPlayer),
																NEW_UID(uidChannel),
																NEW_WSTR(strTargetPlayerName.c_str())
																);

	return gsys.pAppServerFacade->Route(pNewCmd);
}

bool GAppServerChatChannelRouter::UnbanPlayerReq(CID cidPlayer, MUID uidChannel, wstring strBanedPlayerName)
{
	MCommand* pNewCmd = gsys.pAppServerFacade->MakeNewCommand(MPC_MSG_UNBAN_REQ,
																3,
																NEW_INT(cidPlayer),
																NEW_UID(uidChannel),
																NEW_WSTR(strBanedPlayerName.c_str())
																);
	return gsys.pAppServerFacade->Route(pNewCmd);
}

bool GAppServerChatChannelRouter::ChangeHostReq(CID cidPlayer, MUID uidChannel, wstring strTargetPlayerName)
{
	MCommand* pNewCmd = gsys.pAppServerFacade->MakeNewCommand(MPC_MSG_CHANGE_HOST_REQ,
																3,
																NEW_INT(cidPlayer),
																NEW_UID(uidChannel),
																NEW_WSTR(strTargetPlayerName.c_str())
																);

	return gsys.pAppServerFacade->Route(pNewCmd);
}
