#include "stdafx.h"
#include "XSayMsgHandler.h"
#include "XController.h"
#include "XModuleActorControl.h"
#include "XStrings.h"

XSayMsgHandler::XSayMsgHandler()
{
	// do nothing
}

XSayMsgHandler::~XSayMsgHandler()
{
	// do nothing
}

bool XSayMsgHandler::OnResponse(const minet::MCommand* pCmd)
{
	if (!XGETCFG_GAME_SHOWCHATMSG)	return true;

	wstring strSenderName = m_msgHelper.GetSenderName(pCmd);
	wstring strMsg = m_strMsg;
	FilterPvPArea( strSenderName, strMsg);

	GPlayerGrade nPlayerGrade = (GPlayerGrade)m_nAdditional;

	if (gg.controller && !m_strMsg.empty())
	{
		// 말머리
/*		wstringstream strStream;

		strStream	<< L"[" << strSenderName << L"]의 말: "
					<< strMsg;
*/

		// GM 대화면, 안내처럼 취급
		CSMsgType nMsgType = MT_SAY;
		
		if (nPlayerGrade == PLAYER_GRADE_GM)
		{
			nMsgType = MT_ANNOUNCE;
		}

		gg.controller->OnChat(nMsgType, FormatString(XGetStr(L"CHATTING_SAY"), FSParam( strSenderName, strMsg)));
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
