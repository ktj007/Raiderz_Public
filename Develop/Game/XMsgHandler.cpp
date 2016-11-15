#include "stdafx.h"
#include "XMsgHandler.h"
#include "XPlayerPVP.h"
#include "XPvPAreaLogic.h"
#include "XGameState.h"
#include "CSStringFilter.h"

XMsgHandler::XMsgHandler()
{
	// do nothing
}

XMsgHandler::~XMsgHandler()
{
	// do nothing
}

bool XMsgHandler::OnPreResponse(const minet::MCommand* pCmd)
{
	// 메세지 길이 제한
	if (CHATTING_LIMIT_SIZE < m_strMsg.length())
	{
		m_strMsg = m_strMsg.substr(0, CHATTING_LIMIT_SIZE);
	}

	// 시스템 문자 교체
	tr1::wregex pattern(L"(\\{|\\})");
	m_strMsg = tr1::regex_replace(m_strMsg, pattern, wstring(L" "));
	
	// 비속어 필터
	switch (m_eType)
	{
	case MT_SAY:
	case MT_SHOUT:
	case MT_AUCTION:
	case MT_WHISPER:
	case MT_PARTY:
	case MT_GUILD:
	case MT_CHANNEL:
	case MT_FIELD:
	case MT_GLOBAL:
	case MT_WHISPER_BACK:
		{
			if (GetStringFilter()->IsValidStr(m_strMsg.c_str()))
				break;
			
			m_strMsg = GetStringFilter()->Filter(m_strMsg.c_str());
		}
		break;
	default:
		// do nothing - 비속어 없음.
		break;
	}
	
	return true;
}

bool XMsgHandler::OnRequest(const minet::MCommand* pCmd)
{
	// do nothing
	return true;
}

bool XMsgHandler::FilterPvPArea(wstring& strSenderName, wstring& strMsg)
{
	if (gg.currentgamestate)
	{
		XPvPAreaLogic* pPvPAreaLogic = gg.currentgamestate->GetPvPArea();
		if (pPvPAreaLogic->IsInPvPArea())
		{
			XPlayer* pPlayer = gg.omgr->FindPlayerInPvP(strSenderName.c_str());
			if (pPlayer && pPvPAreaLogic->IsEnemy(pPlayer))
			{
				XPvPAreaLogic* pPvPAreaLogic = gg.currentgamestate->GetPvPArea();
				if ( pPvPAreaLogic)
				{
					wstring strName = pPvPAreaLogic->GetTeamName( pPlayer->GetPlayerPVP()->GetPvPAreaTeam());
					if ( strName.empty() == false)		strSenderName = strName;
				}

				wstring str = strMsg;
				strMsg = pPvPAreaLogic->MakeEnemyChatMsg(str.c_str());
				return true;
			}
		}
	}

	return false;
}
