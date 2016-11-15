#include "StdAfx.h"
#include "GStandAloneModeChatChannelRouter.h"


bool GStandAloneModeChatChannelRouter::CreateChannelReq(CID cidPlayer, wstring strChannelName)
{
	return true;
}

bool GStandAloneModeChatChannelRouter::EnterChannelReq(CID cidPlayer, wstring strChannelName)
{
	return true;
}

bool GStandAloneModeChatChannelRouter::LeaveChannelReq(CID cidPlayer, MUID uidChannel)
{
	return true;
}

bool GStandAloneModeChatChannelRouter::KickPlayerReq(CID cidPlayer, MUID uidChannel, wstring strTargetPlayerName)
{
	return true;
}

bool GStandAloneModeChatChannelRouter::BanPlayerReq(CID cidPlayer, MUID uidChannel, wstring strTargetPlayerName)
{
	return true;
}

bool GStandAloneModeChatChannelRouter::UnbanPlayerReq(CID cidPlayer, MUID uidChannel, wstring strBanedPlayerName)
{
	return true;
}

bool GStandAloneModeChatChannelRouter::ChangeHostReq(CID cidPlayer, MUID uidChannel, wstring strTargetPlayerName)
{
	return true;
}
