#include "StdAfx.h"
#include "XDummyBot_MoveChannel.h"
#include "XBirdDummyAgent.h"
#include "XDummyMaster.h"
#include "XDummyHelper.h"
#include "CCommandResultTable.h"
#include "XFieldInfo.h"

const float TEST_TIMEOUT_SEC = 30.0f;

XDummyBot_MoveChannel::XDummyBot_MoveChannel(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
, m_ePhase(PHASE_NONE)
{
	m_rgltTimeout.Stop();
	m_rgltWaitingForChangeChannel.Stop();
}

void XDummyBot_MoveChannel::OnRun(float fDelta)
{
	switch (m_ePhase)
	{
	case PHASE_1_WAITING_RECV_CHANNEL_LIST:
		if (m_rgltTimeout.IsReady(fDelta))
		{
			mlog3("XDummyBot_MoveChannel, Timeout! PHASE_1_WAITING_RECV_CHANNEL_LIST. Retry!\n");
			OnTestStarter();
		}
		break;
	case PHASE_2_WAITING_RECV_RESPONSE_PREPARE_CHANGE_CHANNEL:
		if (m_rgltTimeout.IsReady(fDelta))
		{
			mlog3("XDummyBot_MoveChannel, Timeout! PHASE_2_WAITING_RECV_RESPONSE_PREPARE_CHANGE_CHANNEL. Retry!\n");
			OnTestStarter();
		}
		break;
	case PHASE_3_WAITING_PREPARE_TIME:
		if (m_rgltWaitingForChangeChannel.IsReady(fDelta))
		{
			m_rgltWaitingForChangeChannel.Stop();

			XBirdClient* pClient = m_pAgent->GetClient();
			XBIRDPOSTCMD0(pClient, MC_FIELD_REQUEST_CHANGE_CHANNEL);

			SetPhase(PHASE_4_WAITING_RECV_CHANGE_FIELD);
		}
		break;
	case PHASE_4_WAITING_RECV_CHANGE_FIELD:
		if (m_rgltTimeout.IsReady(fDelta))
		{
			m_rgltTimeout.Stop();

			OnError("XDummyBot_MoveChannel, Timeout! PHASE_4_WAITING_RECV_CHANGE_FIELD");
		}
		break;
	}
}

void XDummyBot_MoveChannel::StartTimeoutTimer()
{
	m_rgltTimeout.SetTickTime(TEST_TIMEOUT_SEC);
	m_rgltTimeout.Start();
}

void XDummyBot_MoveChannel::OnTestStarter()
{
	if (XBirdDummyAgent::DAS_ERROR == m_pAgent->GetStatus()) return;

	XBirdClient* pClient = m_pAgent->GetClient();
	XBIRDPOSTCMD0(pClient, MC_FIELD_REQUEST_CHANNEL_LIST);

	StartTimeoutTimer();

	SetPhase(PHASE_1_WAITING_RECV_CHANNEL_LIST);
}

void XDummyBot_MoveChannel::OnTestChecker(RESULT_INFO* pResultInfo)
{
	if (NULL == pResultInfo) return;

	if (m_stExpectedDestInfo.nFieldID != pResultInfo->nFieldID)
		OnError("XDummyBot_MoveChannel, Not Matched nFieldID");

	if (m_stExpectedDestInfo.nChannelID != pResultInfo->nChannelID)
		OnError("XDummyBot_MoveChannel, Not Matched ChannelID");
}

void XDummyBot_MoveChannel::OnError(string strLog)
{
	m_pAgent->SetStatusError();

	mlog3(strLog.c_str());
	mlog3("\n");
}

//////////////////////////////////////////////////////////////////////////
/// Command Handler

MCommandResult XDummyBot_MoveChannel::OnCommand(XBirdDummyAgent* pAgent, MCommand* pCmd)
{
	XDummyBot::OnCommand(pAgent, pCmd);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCmd->GetID())
	{
	case MC_FIELD_START_GAME:					return OnFieldStartGame(pAgent, pClient, pCmd);
	case MC_FIELD_CHANNEL_LIST:					return OnFieldChannelList(pAgent, pClient, pCmd);
	case MC_FIELD_PREPARE_CHANGE_CHANNEL_RES:	return OnFieldPrepareChangeChannelRes(pAgent, pClient, pCmd);
	case MC_FIELD_CHANGE_FIELD:					return OnFieldChangeField(pAgent, pClient, pCmd);
	}

	return CR_FALSE;
}

MCommandResult XDummyBot_MoveChannel::OnFieldStartGame(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd)
{
	OnTestStarter();

	return CR_TRUE;
}

MCommandResult XDummyBot_MoveChannel::OnFieldChannelList(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd)
{
	if (!pCmd->GetBlob(m_vecChannelID,		0))	return CR_ERROR;
	if (!pCmd->GetBlob(m_vecChannelStateList,	1))	return CR_ERROR;

	if (m_vecChannelID.empty())
	{
		OnError("XDummyBot_MoveChannel, Channel List is Empty");
		return CR_ERROR;
	}

	int nIgnoreChannelID = pAgent->GetLoginChannelID();
	int nChannelID = XDummyHelper::GetRandomIDFromList(m_vecChannelID, nIgnoreChannelID);
	if (INVALID_ID == nChannelID)
	{
		OnError("XDummyBot_MoveChannel, Not Exist Changable Channel");
		return CR_ERROR;
	}

	XBIRDPOSTCMD1(pClient, MC_FIELD_PREPARE_CHANGE_CHANNEL_REQ, MCommandParameterInt(nChannelID));

	m_stExpectedDestInfo.nFieldID = pAgent->GetLoginFieldID();
	m_stExpectedDestInfo.nChannelID = nChannelID;

	StartTimeoutTimer();

	SetPhase(PHASE_2_WAITING_RECV_RESPONSE_PREPARE_CHANGE_CHANNEL);

	return CR_TRUE;
}

MCommandResult XDummyBot_MoveChannel::OnFieldPrepareChangeChannelRes(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd)
{
	m_rgltWaitingForChangeChannel.SetTickTime(CHANNEL_CHANGE_PREPARE_TIME);
	m_rgltWaitingForChangeChannel.Start();

	SetPhase(PHASE_3_WAITING_PREPARE_TIME);

	return CR_TRUE;
}

MCommandResult XDummyBot_MoveChannel::OnFieldChangeField(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd)
{
	int nFieldID;
	int nChannelID;
	if (!pCmd->GetParameter(&nFieldID,		0, MPT_INT))	return CR_ERROR;
	if (!pCmd->GetParameter(&nChannelID,	1, MPT_INT))	return CR_ERROR;

	if (!IsPhase(PHASE_4_WAITING_RECV_CHANGE_FIELD)) return CR_FALSE;

	RESULT_INFO resultInfo;
	resultInfo.nFieldID = nFieldID;
	resultInfo.nChannelID = nChannelID;
	OnTestChecker(&resultInfo);

	return CR_FALSE;
}