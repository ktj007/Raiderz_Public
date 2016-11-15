#include "StdAfx.h"
#include "XDummyBot_Whisper.h"
#include "XBirdClient.h"
#include "XBirdDummyAgent.h"
#include "CSMsgCommandHelper.h"


XDummyBot_Whisper::XDummyBot_Whisper(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
, m_Phase(PHASE_INIT)
{
	m_rgltWaitToStart.SetTickTime(5);
	m_rgltTimout.SetTickTime(60.0f);
}

XDummyBot_Whisper::~XDummyBot_Whisper(void)
{
}

MCommandResult XDummyBot_Whisper::OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand)
{
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_MSG_RES:
		{
			int nType;
			wstring strMsg;
			int nAdditional;

			if (!pCommand->GetParameter(&nType, 0, MPT_INT))		return CR_ERROR;
			if (!pCommand->GetParameter(strMsg, 1, MPT_WSTR))		return CR_ERROR;
			if (!pCommand->GetParameter(&nAdditional, 4, MPT_INT))	return CR_ERROR;

			if (MT_WHISPER == nType)
			{
				if (m_bEvenNumber)
				{
					if (L"1" != strMsg)
					{
						mlog("Error! XDummyBot_Whisper, MT_WHISPER, I'm EvenNumber. But String is EvenNumber\n");
						ChangePhase(PHASE_ERROR);
						pAgent->SetStatusError();
					}
				}
				else
				{
					if (L"0" != strMsg)
					{
						mlog("Error! XDummyBot_Whisper, MT_WHISPER, I'm not EvenNumber. But String is not EvenNumber\n");
						ChangePhase(PHASE_ERROR);
						pAgent->SetStatusError();
					}
				}

				ChangePhase(PHASE_REQ_WHISPER);
			}
			else if (MT_WHISPER_BACK == nType)
			{
				if (m_bEvenNumber)
				{
					if (L"0" != strMsg)
					{
						mlog("Error! XDummyBot_Whisper, MT_WHISPER_BACK, I'm EvenNumber. But String is not EvenNumber\n");
						ChangePhase(PHASE_ERROR);
						pAgent->SetStatusError();
					}
				}
				else
				{
					if (L"1" != strMsg)
					{
						mlog("Error! XDummyBot_Whisper, MT_WHISPER_BACK, I'm not EvenNumber. But String is EvenNumber\n");
						ChangePhase(PHASE_ERROR);
						pAgent->SetStatusError();
					}
				}
			}
		}
		break;
	}

	return CR_TRUE;
}

void XDummyBot_Whisper::OnRun(float fDelta)
{
	switch (m_Phase)
	{
	case PHASE_INIT:
		{
			Init();
		}
		break;
	case PHASE_WAIT_TO_START:
		{
			if (m_rgltWaitToStart.IsReady(fDelta))
				ChangePhase(PHASE_REQ_WHISPER);
		}
		break;
	case PHASE_REQ_WHISPER:
		{
			CSMsgCommandHelper	m_msgHelper;

			MCommand* pNewCmd = NULL;
			pNewCmd = m_msgHelper.MakeNewClientCommandWhisperReq(m_strMsg, m_strTargetName);

			pNewCmd->SetReceiverUID(m_pAgent->GetClient()->GetServerUID());
			pNewCmd->SetSenderUID(m_pAgent->GetClient()->GetUID());

			XBirdPostCommand(m_pAgent->GetClient(), pNewCmd);

			m_rgltTimout.Start();

			ChangePhase(PHASE_WAIT_WHISPER);
		}
		break;
	case PHASE_WAIT_WHISPER:
		{
			if (m_rgltTimout.IsReady(fDelta))
			{
				mlog("Error! XDummyBot_Whisper, Timeout!!\n");
				ChangePhase(PHASE_ERROR);
				m_pAgent->SetStatusError();
				break;
			}
		}
		break;
	}
}

void XDummyBot_Whisper::Init()
{
	m_bEvenNumber = true;
	m_strMsg = L"0";

	int nID = m_pAgent->GetID();
	int nTraderBotID = nID + 1;
	if (1 == nID % 2)
	{
		m_bEvenNumber = false;
		nTraderBotID = nID - 1;
		m_strMsg = L"1";
	}

	XBirdDummyAgentMgr* pAgentMgr = m_pAgent->GetAgentMgr();
	if (NULL == pAgentMgr) return;
	XBirdDummyAgent* pTraderAgent = pAgentMgr->GetAgentByID(nTraderBotID);
	if (NULL == pTraderAgent) return;

	m_strTargetName = pTraderAgent->GetLoginID();


	m_rgltWaitToStart.Start();
	ChangePhase(PHASE_WAIT_TO_START);
}
