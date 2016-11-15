#include "stdafx.h"
#include "XWhisperBackMsgHandler.h"
#include "XController.h"
#include "XStrings.h"

XWhisperBackMsgHandler::XWhisperBackMsgHandler()
{
	// do nothing
}

XWhisperBackMsgHandler::~XWhisperBackMsgHandler()
{
	// do nothing
}

bool XWhisperBackMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	if (!XGETCFG_GAME_SHOWWHISPERMSG)	return true;

	if (gg.controller && !m_strMsg.empty())
	{
/*		wstringstream strStream;

		strStream	<< L"[" << m_msgHelper.GetReceiverName(pCmd) << L"]´Ô¿¡°Ô ±Ó¼Ó¸»: "
					<< m_strMsg;
*/
		gg.controller->OnChat(MT_WHISPER_BACK, FormatString( L"CHATTING_WHISPERFROM", FSParam( m_msgHelper.GetReceiverName(pCmd), m_strMsg)));
	}

	return true;
}
