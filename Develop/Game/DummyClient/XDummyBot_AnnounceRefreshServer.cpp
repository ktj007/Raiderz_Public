#include "StdAfx.h"
#include "XDummyBot_AnnounceRefreshServer.h"
#include "XBirdDummyAgent.h"
#include "XDummyMaster.h"
#include "XFieldInfo.h"
#include "CCommandTable.h"


XDummyBot_AnnounceRefreshServer::XDummyBot_AnnounceRefreshServer(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
, m_ePhase(PHASE_0_ANNOUNCE)
, m_nCounting(10)
{
	const float TIME_TIMEOUT_SEC = 60.f; ///< 1분
	m_rgltTimeout.SetTickTime(TIME_TIMEOUT_SEC);
	m_rgltTimeout.Start();

	m_bRun = true;
	m_strAnnounceMsg = L"서버 리프래쉬 알림 봇 : 서버를 종료 합니다.('㉦')/";
}

bool XDummyBot_AnnounceRefreshServer::InitParam(XBirdDummyAgent* pAgent)
{
	wstring strSec = m_pAgentInfo->GetValue(L"sec");
	m_nCounting = _wtoi(strSec.c_str());

	return true;
}

void XDummyBot_AnnounceRefreshServer::OnRun(float fDelta)
{
	if (m_rgltTimeout.IsReady(fDelta))
	{
		OnError("Timeout!!");
		::PostQuitMessage(0);
		return;
	}

	if (PHASE_1_COUNTING != m_ePhase) return;
	if (!m_rgltCounting.IsActive()) return;

	if (m_rgltCounting.IsReady(fDelta))
	{
		const float TIME_ONE_SEC = 1.0f;
		m_rgltCounting.SetTickTime(TIME_ONE_SEC);

		if (0 == m_nCounting)
		{
			m_ePhase = PHASE_2_END;
			::PostQuitMessage(0);
			return;
		}

		wchar_t szAnnounceMsg[256] = L"";
		wsprintf(szAnnounceMsg, L"서버 리프래쉬 알림 봇 : %d 초 뒤 서버를 종료합니다.", m_nCounting--);

		XBIRDPOSTCMD5(m_pAgent->GetClient(), MC_MSG_REQ, MCommandParameterInt(MT_ANNOUNCE), MCommandParameterWideString(szAnnounceMsg), MCommandParameterBlob(), MCommandParameterBlob(), MCommandParameterInt(0));
	}

}

minet::MCommandResult XDummyBot_AnnounceRefreshServer::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCmd )
{
	if (PHASE_0_ANNOUNCE != m_ePhase) return CR_FALSE;

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCmd->GetID())
	{
	case MC_FIELD_START_GAME:	return OnFieldStartGame(pAgent, pClient, pCmd);
	}

	return CR_FALSE;
}
minet::MCommandResult XDummyBot_AnnounceRefreshServer::OnFieldStartGame( XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd )
{
	if (0 == m_strAnnounceMsg.size())
	{
		if (false == InitParam(pAgent))
		{
			pAgent->SetStatusError();
			return CR_FALSE;
		}
	}

	XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));

	XBIRDPOSTCMD5(pClient, MC_MSG_REQ, MCommandParameterInt(MT_ANNOUNCE), MCommandParameterWideString(m_strAnnounceMsg.c_str()), MCommandParameterBlob(), MCommandParameterBlob(), MCommandParameterInt(0));

	m_ePhase = PHASE_1_COUNTING;

	const float TIME_START_ANNOUNCE_SEC = 5.0f;

	m_rgltCounting.SetTickTime(TIME_START_ANNOUNCE_SEC);
	m_rgltCounting.Start();

	return CR_FALSE;
}

void XDummyBot_AnnounceRefreshServer::OnError(string strLog)
{
	USES_CONVERSION_EX;

	m_pAgent->SetStatusError();

	char szErrorLog[1024] = {0};

	sprintf_s(szErrorLog, 1024, "(%s) XDummyBot_AnnounceRefreshServer::OnError(), %s\n", W2A_EX(m_pAgent->GetLoginID(), 0), strLog.c_str());

	mlog3(szErrorLog);
}