#include "stdafx.h"
#include "XAuctionMsgHandler.h"
#include "XController.h"

XAuctionMsgHandler::XAuctionMsgHandler()
{
	// do nothing
}

XAuctionMsgHandler::~XAuctionMsgHandler()
{
	// do nothing
}

bool XAuctionMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	if (!XGETCFG_GAME_SHOWCHATMSG)	return true;

	if (gg.controller && !m_strMsg.empty())
	{
		wstringstream strStream;

		strStream	<< L"[Auction]"
					<< L"[" << m_msgHelper.GetSenderName(pCmd) << L"]: "
					<< m_strMsg;

		gg.controller->OnChat(MT_AUCTION, strStream.str());
	}

	return true;
}
