#include "stdafx.h"
#include "XPartyMsgHandler.h"
#include "XController.h"
#include "XModuleActorControl.h"

XPartyMsgHandler::XPartyMsgHandler()
{
	// do nothing
}

XPartyMsgHandler::~XPartyMsgHandler()
{
	// do nothing
}

bool XPartyMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	wstring strSenderName = m_msgHelper.GetSenderName(pCmd);
	wstring strMsg = m_strMsg;
	FilterPvPArea(strSenderName, strMsg);

	if (XGETCFG_GAME_SHOWPARTYMSG)
	{
		if (gg.controller && !m_strMsg.empty())
		{
			wstringstream strStream;

			strStream	<< L"[Party]"
						<< L"[" << strSenderName << L"] "
						<< strMsg;

			gg.controller->OnChat(MT_PARTY, strStream.str());
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
