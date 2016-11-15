#include "stdafx.h"
#include "XGlobalMsgHandler.h"
#include "XController.h"
#include "XModuleActorControl.h"

XGlobalMsgHandler::XGlobalMsgHandler()
{
	// do nothing
}

XGlobalMsgHandler::~XGlobalMsgHandler()
{
	// do nothing
}

bool XGlobalMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	wstring strSenderName = m_msgHelper.GetSenderName(pCmd);
	wstring strMsg = m_strMsg;
	FilterPvPArea(strSenderName, strMsg);

	if (XGETCFG_GAME_SHOWCHATMSG)
	{
		if (gg.controller && !m_strMsg.empty())
		{
			wstringstream strStream;

			strStream	<< L"[Global]"
						<< L"[" << strSenderName << L"]: "
						<< strMsg;

			gg.controller->OnChat(MT_GLOBAL, strStream.str());
		}
	}

	if (XGETCFG_GAME_SHOWCHATBUBBLE)
	{
		if (gg.omgr)
		{
			XPlayer* pPlayer = gg.omgr->FindPlayer(strSenderName.c_str());
			if (pPlayer)
			{
				XModuleActorControl* pActorControl = pPlayer->GetModuleActorControl();
				if (pActorControl)
				{
					pActorControl->OnBallonText(strSenderName, strMsg, 0.f);
				}
			}
		}
	}

	return true;
}
