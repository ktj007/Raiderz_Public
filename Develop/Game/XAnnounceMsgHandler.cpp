#include "stdafx.h"
#include "XAnnounceMsgHandler.h"
#include "XController.h"
#include "XModuleActorControl.h"

XAnnounceMsgHandler::XAnnounceMsgHandler()
{
	// do nothing
}

XAnnounceMsgHandler::~XAnnounceMsgHandler()
{
	// do nothing
}

bool XAnnounceMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	if (XGETCFG_GAME_SHOWCHATMSG)
	{
		if (gg.controller && !m_strMsg.empty())
		{
			wstringstream strStream;

			strStream	<< L"[Announcement]: "
						<< m_strMsg;

			gg.controller->OnChat(MT_ANNOUNCE, strStream.str());
		}
	}

	return true;
}
