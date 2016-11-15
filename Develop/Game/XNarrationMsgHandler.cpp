#include "stdafx.h"
#include "XNarrationMsgHandler.h"

XNarrationMsgHandler::XNarrationMsgHandler()
{
	// do nothing
}

XNarrationMsgHandler::~XNarrationMsgHandler()
{
	// do nothing
}

bool XNarrationMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	if (!XGETCFG_GAME_SHOWCHATMSG)	return true;
	
	global.ui->OnPresentationUpper(m_strMsg);

	return true;
}
