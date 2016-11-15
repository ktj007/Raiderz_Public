#include "stdafx.h"
#include "XShoutFakeMsgHandler.h"
#include "XController.h"

XShoutFakeMsgHandler::XShoutFakeMsgHandler()
{
	// do nothing
}

XShoutFakeMsgHandler::~XShoutFakeMsgHandler()
{
	// do nothing
}

bool XShoutFakeMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	if (!XGETCFG_GAME_SHOWCHATMSG)	return true;

	if (gg.controller && !m_strMsg.empty())
	{
		gg.controller->OnChat(MT_SHOUT, m_strMsg);
	}

	return true;
}
