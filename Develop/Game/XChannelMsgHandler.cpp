#include "stdafx.h"
#include "XChannelMsgHandler.h"
#include "XController.h"
#include "XChatChannelManager.h"
#include "XChatChannel.h"

XChannelMsgHandler::XChannelMsgHandler()
{
	// do nothing
}

XChannelMsgHandler::~XChannelMsgHandler()
{
	// do nothing
}

bool XChannelMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	if (!XGETCFG_GAME_SHOWPARTYMSG)	return true;

	if (gg.controller && !m_strMsg.empty())
	{
		MUID uidChannel = m_msgHelper.GetReceiverUID(pCmd);
		XChatChannel* pChannel = info.chat_channel->Find(uidChannel);

		if (pChannel)
		{
			wstringstream strStream;

			strStream	<< L"[" << pChannel->GetName() << L"]"
						<< L"["	<< m_msgHelper.GetSenderName(pCmd) << L"]: "
						<< m_strMsg;

			gg.controller->OnChat(MT_CHANNEL, strStream.str());
		}
	}
		
	return true;
}
