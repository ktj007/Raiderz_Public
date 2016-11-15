#include "stdafx.h"
#include "XDummyBot_Echo.h"
#include "XDummyMaster.h"
#include "XNPCInfo.h"
#include "XPost_GM.h"

XDummyBot_Echo::XDummyBot_Echo(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo)
: XDummyBot_Roam(pAgent, pAgentInfo)
{
	wstring strEchoTickTime = pAgentInfo->GetValue(L"echo_tick");
	m_fEchoTickTime = (float)_wtoi(strEchoTickTime.c_str());

	wstring strDataSize = pAgentInfo->GetValue(L"data_size");
	m_nDataSize = _wtoi(strDataSize.c_str());

	wstring strRouteType = pAgentInfo->GetValue(L"route_type");
	m_nRouteType = _wtoi(strRouteType.c_str());

}

XDummyBot_Echo::~XDummyBot_Echo()
{

}

void XDummyBot_Echo::OnRun(float fDelta)
{
	m_fElapsedPostTime += fDelta;

	if (m_fElapsedPostTime >= m_fEchoTickTime)
	{
		vector<int> vecData;

		vecData.resize(m_nDataSize);

		for (int i = 0; i < m_nDataSize; i++)
		{
			vecData[i] = i;
		}


		XBIRDPOSTCMD3(m_pAgent->GetClient(), MC_DEBUG_ECHO_REQ, MCommandParameterInt(m_nRouteType), MCommandParameterInt(m_nDataSize), MCommandParameterBlob(vecData));

		m_fElapsedPostTime = 0.0f;
	}
}

minet::MCommandResult XDummyBot_Echo::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	XDummyBot::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_FIELD_START_GAME:
		{
			m_fElapsedPostTime = 0.0f;
		}
		break;
	case MC_FIELD_CHANGE_FIELD:
		{
			XBIRDPOSTCMD1(pClient, MC_GM_REBIRTH_REQ, MCommandParameterVector(m_vPosition));
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
			XBIRDPOSTCMD0(pClient, MC_GM_GOD_REQ);
		}
		break;
	default:
		{
		}
		break;
	}

	return CR_FALSE;
}

