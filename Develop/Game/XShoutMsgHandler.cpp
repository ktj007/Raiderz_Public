#include "stdafx.h"
#include "XShoutMsgHandler.h"
#include "XController.h"
#include "XStrings.h"

XShoutMsgHandler::XShoutMsgHandler()
{
	// do nothing
}

XShoutMsgHandler::~XShoutMsgHandler()
{
	// do nothing
}

bool XShoutMsgHandler::OnResponse(const minet::MCommand* pCmd)
{	
	if (!XGETCFG_GAME_SHOWCHATMSG)	return true;

	if (gg.controller && !m_strMsg.empty())
	{
		wstring strSenderName = m_msgHelper.GetSenderName(pCmd);
		FilterPvPArea(strSenderName, m_strMsg);

/*		wstringstream strStream;
		strStream	<< L"["	<< strSenderName << L"]ÀÇ ¿ÜÄ§: "
					<< m_strMsg;
*/
		gg.controller->OnChat(MT_SHOUT, FormatString( XGetStr( L"CHATTING_SHOUTS"), FSParam( strSenderName, m_strMsg)));
	}

	return true;
}
