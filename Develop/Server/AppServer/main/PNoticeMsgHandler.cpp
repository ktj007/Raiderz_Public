#include "stdafx.h"
#include "PNoticeMsgHandler.h"
#include "PGlobal.h"
#include "PCommandCenter.h"

PNoticeMsgHandler::PNoticeMsgHandler()
{
	// do nothing
}

PNoticeMsgHandler::~PNoticeMsgHandler()
{
	// do nothing
}

bool PNoticeMsgHandler::OnRequest(const minet::MCommand* pCmd)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewServerCommandNoticeRes(m_strMsg);
	gsys.pCommandCenter->RouteToAll(pNewCmd);

	return true;
}
