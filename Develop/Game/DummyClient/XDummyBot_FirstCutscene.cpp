#include "StdAfx.h"
#include "XDummyBot_FirstCutscene.h"
#include "XDummyMaster.h"
#include "CCommandResultTable.h"

const float TIME_CUTSCENE = 60.0f;


XDummyBot_FirstCutscene::XDummyBot_FirstCutscene(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot(pAgent, pAgentInfo)
, m_Phase(PHASE_INIT)
{
	m_rgltCutscene.SetTickTime(TIME_CUTSCENE);
}

XDummyBot_FirstCutscene::~XDummyBot_FirstCutscene(void)
{
}


MCommandResult XDummyBot_FirstCutscene::OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand)
{
	__super::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_CUTSCENE_BEGIN_ME:
		{
			m_rgltCutscene.Start();
			ChangePhase(PHASE_WAIT_CUTSCENE);
		}
		break;
	case MC_CUTSCENE_END_ME:
		{
			pAgent->SetStatusComplete();
		}
		break;
	}

	return CR_TRUE;
}

void XDummyBot_FirstCutscene::OnRun(float fDelta)
{
	XBirdClient* pClient = m_pAgent->GetClient();

	switch(m_Phase)
	{
	case PHASE_INIT:
		{
			XBIRDPOSTCMD1(pClient, MC_CUTSCENE_BEGIN_REQ, MCommandParameterInt(109000));
			ChangePhase(PHASE_REQ_CUTSCENE);
		}
		break;
	case PHASE_WAIT_CUTSCENE:
		{
			if (m_rgltCutscene.IsReady(fDelta))
			{
				m_rgltCutscene.Stop();

				XBIRDPOSTCMD1(pClient, MC_CUTSCENE_END_REQ, MCommandParameterInt(109000));
				ChangePhase(PHASE_REQ_CUTSCENE_END);
			}
		}
		break;
	}
}