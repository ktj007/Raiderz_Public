#include "StdAfx.h"
#include "XDummyBot_PositionSpreader.h"
#include "XBirdDummyAgent.h"
#include "XDummyMaster.h"
#include "XFieldInfo.h"

const float DEFAULT_SPREADER_SPACE_LENGTH = 1000;

int XDummyBot_PositionSpreader::m_nDestFieldID = 0;
float XDummyBot_PositionSpreader::m_fFieldMin_x = 0;
float XDummyBot_PositionSpreader::m_fFieldMin_y = 0;
float XDummyBot_PositionSpreader::m_fFieldMax_x = 0;
float XDummyBot_PositionSpreader::m_fFieldMax_y = 0;
float XDummyBot_PositionSpreader::m_fSpaceLength = DEFAULT_SPREADER_SPACE_LENGTH;
vec3 XDummyBot_PositionSpreader::m_vTargetPos = vec3(0,0,0);

XDummyBot_PositionSpreader::XDummyBot_PositionSpreader(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
, m_ePhase(PHASE_0_STARTING)
{

}

bool XDummyBot_PositionSpreader::InitSpreader(XBirdDummyAgent* pAgent)
{
	wstring strFieldID = m_pAgentInfo->GetValue(L"dest_field");
	m_nDestFieldID = _wtoi(strFieldID.c_str());

	wstring strSpaceLength = m_pAgentInfo->GetValue(L"space_length");
	m_fSpaceLength = _wtoi(strSpaceLength.c_str());

	if (0 == m_fSpaceLength) m_fSpaceLength = DEFAULT_SPREADER_SPACE_LENGTH;
	
	wstring strFieldMinX = m_pAgentInfo->GetValue(L"min_x");
	m_fFieldMin_x = _wtoi(strFieldMinX.c_str());

	wstring strFieldMinY = m_pAgentInfo->GetValue(L"min_y");
	m_fFieldMin_y = _wtoi(strFieldMinY.c_str());

	wstring strFieldMaxX = m_pAgentInfo->GetValue(L"max_x");
	m_fFieldMax_x = _wtoi(strFieldMaxX.c_str());

	wstring strFieldMaxY = m_pAgentInfo->GetValue(L"max_y");
	m_fFieldMax_y = _wtoi(strFieldMaxY.c_str());

	if (0 == m_fFieldMin_x &&
		0 == m_fFieldMin_y &&
		0 == m_fFieldMax_x &&
		0 == m_fFieldMax_y)
	{
		m_fFieldMin_x = 1;
		m_fFieldMin_y = 1;
		m_fFieldMax_x = 10000;
		m_fFieldMax_y = 10000;
	}

	if (m_fFieldMin_x > m_fFieldMax_x ||
		m_fFieldMin_y > m_fFieldMax_y)
	{
		return false;
	}

	m_vTargetPos.x = m_fFieldMin_x;
	m_vTargetPos.y = m_fFieldMin_y;
	m_vTargetPos.z = 3000;

	return true;
}

void XDummyBot_PositionSpreader::OnRun(float fDelta)
{
	if (PHASE_1_CHECK != m_ePhase) return;

	// Timeout üũ
	if (m_rgltTimeout.IsActive() && m_rgltTimeout.IsReady(fDelta))
	{
		char szLog[512]="";
		sprintf_s(szLog, 512, "Timeout! (LoginFieldID=%d, LoginPos(%f, %f), DestFieldID=%d, DestPos(%f, %f))"
			, m_pAgent->GetLoginFieldID(), m_pAgent->GetLoginFieldStartPos().x, m_pAgent->GetLoginFieldStartPos().y
			, m_stExpectedDestInfo.nFieldID, m_stExpectedDestInfo.vTargetPos.x, m_stExpectedDestInfo.vTargetPos.y);
		OnError(szLog);
		return;
	}

	if (m_pAgent->GetLoginFieldID() == m_stExpectedDestInfo.nFieldID)
		//&&
		//(int)m_pAgent->GetLoginFieldStartPos().x/1000 == (int)m_stExpectedDestInfo.vTargetPos.x/1000 &&
		//(int)m_pAgent->GetLoginFieldStartPos().y/1000 == (int)m_stExpectedDestInfo.vTargetPos.y/1000)
	{
		m_pAgent->SetStatusComplete();
		m_ePhase = PHASE_2_COMPLETE;
		return;
	}
}

minet::MCommandResult XDummyBot_PositionSpreader::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCmd )
{
	if (PHASE_0_STARTING != m_ePhase) return CR_FALSE;

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCmd->GetID())
	{
	case MC_FIELD_START_GAME:	return OnFieldStartGame(pAgent, pClient, pCmd);
	}

	return CR_FALSE;
}
minet::MCommandResult XDummyBot_PositionSpreader::OnFieldStartGame( XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd )
{
	if (0 == m_nDestFieldID)
	{
		if (false == InitSpreader(pAgent))
		{
			pAgent->SetStatusError();
			return CR_FALSE;
		}
	}
	else
	{
		m_vTargetPos.x += m_fSpaceLength;
		if (m_fFieldMax_x < m_vTargetPos.x)
		{
			m_vTargetPos.x = m_fFieldMin_x;

			m_vTargetPos.y += m_fSpaceLength;
			if (m_fFieldMax_y < m_vTargetPos.y)
			{
				m_vTargetPos.y = m_fFieldMin_y;
			}
		}
	}

	XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
	XBIRDPOSTCMD2(pClient, MC_GM_MOVE_REQ, MCommandParameterInt(m_nDestFieldID), MCommandParameterVector(m_vTargetPos));

	m_stExpectedDestInfo.nFieldID = m_nDestFieldID;
	m_stExpectedDestInfo.vTargetPos = m_vTargetPos;

	m_ePhase = PHASE_1_CHECK;

	const float TIMEOUT_SEC = 180.0f;

	m_rgltTimeout.SetTickTime(TIMEOUT_SEC);
	m_rgltTimeout.Start();

	m_bRun = true;
	
	return CR_FALSE;
}

void XDummyBot_PositionSpreader::OnError(string strLog)
{
	USES_CONVERSION_EX;

	m_pAgent->SetStatusError();

	char szErrorLog[1024] = {0};

	sprintf_s(szErrorLog, 1024, "(%s) XDummyBot_PositionSpreader::OnError(), %s\n", W2A_EX(m_pAgent->GetLoginID(), 0), strLog.c_str());

	mlog3(szErrorLog);
}