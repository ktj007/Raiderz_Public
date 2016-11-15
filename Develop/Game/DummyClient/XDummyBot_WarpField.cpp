#include "StdAfx.h"
#include "XDummyBot_WarpField.h"
#include "XBirdDummyAgent.h"
#include "XDummyMaster.h"
#include "XDummyHelper.h"
#include "CCommandResultTable.h"
#include "XFieldInfo.h"
#include "MCsvParser.h"


XDummyBot_WarpField::XDummyBot_WarpField(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
, m_bPostGMInit(false)
, m_ePhase(PHASE_0_STARTING)
{
	m_rgltTimeout.Stop();
	m_rgltWarpDelay.Stop();

	if (!InitTestFieldList())
		return;

	InitTestDelayTime();
}

bool XDummyBot_WarpField::InitTestFieldList()
{
	wstring strFieldList = m_pAgentInfo->GetValue(L"fieldlist");
	if (strFieldList.empty())
	{
		OnError("XDummyBot_WarpField, fieldlist attribute is empty");
		return false;
	}

	MCsvParser csv_parser;

	USES_CONVERSION_EX;
	csv_parser.LoadFromStream(W2A_EX(strFieldList.c_str(), 0));
	int nColCount = csv_parser.GetColCount(0);

	char szBuff[1024];
	for (int nCol = 0; nCol < nColCount; nCol++)
	{
		if (csv_parser.GetData(nCol, 0, szBuff, 1024))
		{
			m_vecTestFieldID.push_back(atoi(szBuff));
		}
	}
	
	if (m_vecTestFieldID.empty())
	{
		OnError("InitTestFieldList(), Not Exist Valid FieldList");
		return false;
	}

	return true;
}
void XDummyBot_WarpField::InitTestDelayTime()
{
	/// 워프 시도할 때의 Delay Time 계산
	const float DEFAULT_DELAY_WARP_SEC = 1.0f;
	float fWarpDelaySec = 0.0f;

	wstring strWarpDelayMS = m_pAgentInfo->GetValue(L"warp_delay_ms");
	if (strWarpDelayMS.empty())
		fWarpDelaySec = DEFAULT_DELAY_WARP_SEC;
	else
		fWarpDelaySec = (0.001f * _wtoi(strWarpDelayMS.c_str()));

	m_rgltWarpDelay.SetTickTime(fWarpDelaySec);

	/// 워프 체크 대기 Timeout
	const float DEFAULT_TIMEOUT_WARP_SEC = 300.0f;
	float fWarpTimeoutSec = 0.0f;

	wstring strWarpTimeoutMS = m_pAgentInfo->GetValue(L"warp_timeout_ms");
	if (strWarpTimeoutMS.empty())
		fWarpTimeoutSec = DEFAULT_TIMEOUT_WARP_SEC;
	else
		fWarpTimeoutSec = (0.001f * _wtoi(strWarpTimeoutMS.c_str()));

	m_rgltTimeout.SetTickTime(fWarpTimeoutSec);
}

void XDummyBot_WarpField::OnRun(float fDelta)
{
	if (XBirdDummyAgent::DAS_ERROR == m_pAgent->GetStatus()) return;

	if (m_bPostGMInit == false)
	{
		XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
		XBIRDPOSTCMD0(m_pAgent->GetClient(), MC_GM_GOD_REQ);
		m_bPostGMInit = true;
	}

	switch (GetPhase())
	{
	case PHASE_1_WARP:	OnRun_Warp(fDelta); return;
	case PHASE_2_CHECK:	OnRun_Check(fDelta); return;
	}
}

void XDummyBot_WarpField::OnRun_Warp(float fDelta)
{
	if (m_rgltWarpDelay.IsActive() && !m_rgltWarpDelay.IsReady(fDelta))
		return;
	m_rgltWarpDelay.Stop();

	int nIgnoreFieldID = m_pAgent->GetLoginFieldID();
	int nFieldID = XDummyHelper::GetRandomIDFromList(m_vecTestFieldID, nIgnoreFieldID);
	if (nFieldID == INVALID_ID)
	{
		OnError("Random FieldID is INVALID!");
		return;
	}

	XBirdClient* pClient = m_pAgent->GetClient();
	XBIRDPOSTCMD2(pClient, MC_GM_MOVE_REQ, MCommandParameterInt(nFieldID), MCommandParameterVector(m_pAgent->GetCenter()));

	SetPhase(PHASE_2_CHECK);

	m_stExpectedDestInfo.nFieldID = nFieldID;
	m_pAgent->SetLoginField(0, 0, m_pAgent->GetCenter());
	StartTimeoutTimer();
}

void XDummyBot_WarpField::OnRun_Check(float fDelta)
{
	if (m_rgltTimeout.IsActive() && m_rgltTimeout.IsReady(fDelta))
	{
		char szLog[512]="";
		sprintf_s(szLog, 512, "Timeout! (PreFieldID=%d, DestFieldID=%d, Delta=%f)"
			, m_pAgent->GetPreStayFieldID(), m_stExpectedDestInfo.nFieldID, fDelta);
		OnError(szLog);
		return;
	}
	
	return;
}

void XDummyBot_WarpField::OnError(string strLog)
{
	m_pAgent->SetStatusError();

	char szErrorLog[1024] = {0};
	
	sprintf_s(szErrorLog, 1024, "(%S) XDummyBot_WarpField::OnError(), %s\n", m_pAgent->GetLoginID(), strLog.c_str());

	mlog3(szErrorLog);
}

//////////////////////////////////////////////////////////////////////////
/// Command Handler

MCommandResult XDummyBot_WarpField::OnCommand(XBirdDummyAgent* pAgent, MCommand* pCmd)
{
	XDummyBot::OnCommand(pAgent, pCmd);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCmd->GetID())
	{
	case MC_FIELD_CHANGE_FIELD:			return OnFieldChangeField(pAgent, pClient, pCmd);
	case MC_FIELD_START_GAME:			return OnFieldStartGame(pAgent, pClient, pCmd);
	case MC_FIELD_CANCEL_CHANGE_FIELD:	return OnFieldCancelChangeField(pAgent, pClient, pCmd);
	}

	return CR_FALSE;
}

MCommandResult XDummyBot_WarpField::OnFieldChangeField(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd)
{
	if (PHASE_2_CHECK != GetPhase()) return CR_FALSE;
	if (INVALID_ID == m_stExpectedDestInfo.nFieldID) return CR_FALSE;
	if (XBirdDummyAgent::DAS_ERROR == pAgent->GetStatus())
	{
		pAgent->SetStatusRunning();

		char szrLog[1024] = {0};
		sprintf_s(szrLog, 1024, "(%S) XDummyBot_WarpField::OnFieldStartGame(), Restoration!\n", m_pAgent->GetLoginID());
		mlog3(szrLog);
	}

	int nFieldID;
	if (!pCmd->GetParameter(&nFieldID,		0, MPT_INT))	return CR_ERROR;

	if (m_stExpectedDestInfo.nFieldID != nFieldID)
	{
		char szLog[512]="";
		sprintf_s(szLog, 512, "Failed! Not Matched Result Field, Current=%d, Expect=%d, Result=%d"
			, m_pAgent->GetPreStayFieldID(), m_stExpectedDestInfo.nFieldID, nFieldID);
		OnError(szLog);
	}

	m_stExpectedDestInfo.Reset();
	StopTimeoutTimer();

	return CR_FALSE;
}

MCommandResult XDummyBot_WarpField::OnFieldStartGame(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd)
{
	if (XBirdDummyAgent::DAS_ERROR == pAgent->GetStatus())
	{
		pAgent->SetStatusRunning();

		char szrLog[1024] = {0};
		sprintf_s(szrLog, 1024, "(%S) XDummyBot_WarpField::OnFieldStartGame(), Restoration!\n", m_pAgent->GetLoginID());
		mlog3(szrLog);
	}

	SetPhase(PHASE_1_WARP);
	m_rgltWarpDelay.Start();

	return CR_TRUE;
}

MCommandResult XDummyBot_WarpField::OnFieldCancelChangeField(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd)
{
	pAgent->SetStatusRunning();

	char szrLog[1024] = {0};
	sprintf_s(szrLog, 1024, "(%S) XDummyBot_WarpField::OnFieldCancelChangeField()\n", m_pAgent->GetLoginID());
	mlog3(szrLog);

	SetPhase(PHASE_1_WARP);
	m_rgltWarpDelay.Start();

	return CR_TRUE;
}

//////////////////////////////////////////////////////////////////////////

void XDummyBot_WarpField::StartTimeoutTimer()
{
	m_rgltTimeout.Start();
}
void XDummyBot_WarpField::StopTimeoutTimer()
{
	m_rgltTimeout.Stop();
}

void XDummyBot_WarpField::SetPhase(BOT_PHASE ePhase)
{
	m_ePhase = ePhase;
}