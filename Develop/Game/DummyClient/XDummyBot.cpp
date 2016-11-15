#include "stdafx.h"
#include "XDummyBot.h"
#include "XBirdDummyAgent.h"
#include "XDummyMaster.h"

XDummyBot::XDummyBot(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo) : m_pAgent(pAgent), m_pAgentInfo(pAgentInfo), m_bRun(false)
{
	assert(NULL!=m_pAgent);
	assert(NULL!=m_pAgentInfo);
}

XDummyBot::~XDummyBot()
{

}

void XDummyBot::Run(float fDelta)
{
	if (m_bRun)
	{
		OnRun(fDelta);
	}
}

void XDummyBot::OnRun(float fDelta)
{

}

minet::MCommandResult XDummyBot::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_FIELD_START_GAME:
		{
			m_bRun = true;
		}
		break;
	}

	return CR_FALSE;
}

void XDummyBot::PostMove(vec3 vPos)
{
	if (vec3::ZERO != m_pAgent->GetCenter())
	{
		XBIRDPOSTCMD2(m_pAgent->GetClient(), MC_GM_MOVE_REQ, MCommandParameterInt(m_pAgent->GetLoginFieldID()), MCommandParameterVector(vPos));
	}	
}

const wchar_t* XDummyBot::GetAgentName()
{
	return m_pAgent->GetLoginID();
}