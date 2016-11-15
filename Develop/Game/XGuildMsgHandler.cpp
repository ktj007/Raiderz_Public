#include "stdafx.h"
#include "XGuildMsgHandler.h"
#include "XController.h"

XGuildMsgHandler::XGuildMsgHandler()
{
	// do nothing
}

XGuildMsgHandler::~XGuildMsgHandler()
{
	// do nothing
}

bool XGuildMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	if (!XGETCFG_GAME_SHOWGUILDMSG)	return true;

	if (gg.controller && !m_strMsg.empty())
	{
		wstringstream strStream;

		strStream	<< L"[Guild]"		
					<< L"["	<< m_msgHelper.GetSenderName(pCmd) << L"]: "
					<< m_strMsg;

		gg.controller->OnChat(MT_GUILD, strStream.str());
	}

	return true;
}
