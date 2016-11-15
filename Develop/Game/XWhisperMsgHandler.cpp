#include "stdafx.h"
#include "XWhisperMsgHandler.h"
#include "XController.h"
#include "XStrings.h"

XWhisperMsgHandler::XWhisperMsgHandler()
{
	// do nothing
}

XWhisperMsgHandler::~XWhisperMsgHandler()
{
	// do nothing
}

bool XWhisperMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	if (!XGETCFG_GAME_SHOWWHISPERMSG)	return true;

	if (gg.controller && !m_strMsg.empty())
	{
		wstring strSenderName = m_msgHelper.GetSenderName(pCmd);
		gvar.Game.strLastWhisper = strSenderName;

/*		wstringstream strStream;

		strStream	<< L"[" << strSenderName << L"]ÀÇ ±Ó¼Ó¸»: "
					<< m_strMsg;
*/
		gg.controller->OnChat(MT_WHISPER, FormatString( XGetStr( L"CHATTING_WHISPERFROM"), FSParam( strSenderName, m_strMsg)));
	}

	return true;
}
