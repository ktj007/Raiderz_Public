#include "stdafx.h"
#include "XDummyBot_Spawn.h"
#include "XDummyMaster.h"
#include "XPost_GM.h"
#include "XDummyCollision.h"
#include "CSMsgCommandHelper.h"

#include "XNPCInfo.h"

void XDummyBot_Spawn::InitNPCInfoForTest()
{
	for (XNPCInfoMgr::iterator itor = info.npc->begin(); itor != info.npc->end(); ++itor)
	{
		XNPCInfo* pNPCInfo = static_cast<XNPCInfo*>((*itor).second);


		if (m_bNPCOnly && pNPCInfo->nNpcType != NPC_TYPE_NPC)
		{
			continue;
		}

		m_vecNPCIDs.push_back(pNPCInfo->nID);
	}
}


int XDummyBot_Spawn::GetRandomNPCID()
{
	if (m_vecNPCIDs.empty()) return 0;

	int index = XMath::RandomNumber(0, (int)m_vecNPCIDs.size()-1);
	int nNPCID = m_vecNPCIDs[index];

	return nNPCID;
}


XDummyBot_Spawn::XDummyBot_Spawn(XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo) 
: XDummyBot(pAgent, pAgentInfo), m_nLastPostTime(0), m_vCenter(pAgent->GetCenter()), m_fRadius(1000.0f), m_fTick(1.0f), m_bNPCOnly(false), m_fDespawnRate(0.2f)
{
	m_fTick = pAgentInfo->GetFloatValue(L"tick");
	m_bNPCOnly = pAgentInfo->GetBoolValue(L"npc_only");
	m_fDespawnRate = pAgentInfo->GetFloatValue(L"despawn_rate");
	

	InitNPCInfoForTest();

}


XDummyBot_Spawn::~XDummyBot_Spawn()
{

}


void XDummyBot_Spawn::OnRun(float fDelta)
{
	uint32 nNowTime = XGetNowTime();
	if (nNowTime - m_nLastPostTime <= uint32(m_fTick * 1000)) return;

	m_nLastPostTime = nNowTime;

	Post();
}


minet::MCommandResult XDummyBot_Spawn::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCommand )
{
	XDummyBot::OnCommand(pAgent, pCommand);

	XBirdClient* pClient = pAgent->GetClient();
	switch(pCommand->GetID())
	{
	case MC_CHAR_MYINFO:
		{
		}
		break;
	case MC_FIELD_START_GAME:
		{
			XBIRDPOSTCMD2(pClient, MC_GM_SET_ME_REQ, MCommandParameterWideString(L"grade"), MCommandParameterWideString(L"3"));
		}
		break;
	default:
		{
		}
		break;
	}

	return CR_FALSE;
}


void XDummyBot_Spawn::Post()
{
	XBirdClient* pClient = m_pAgent->GetClient();

	if (XMath::RandomNumber(0.0f, 1.0f) <= m_fDespawnRate)
	{
		for (list<int>::iterator itor = m_listNPC.begin(); itor != m_listNPC.end(); ++itor)
		{
			int npc_id = (*itor);
			XBIRDPOSTCMD2(pClient, MC_GM_DESPAWNBYID_REQ, MCommandParameterInt(npc_id), MCommandParameterFloat(1000.0f));
		}

		XBIRDPOSTCMD1(pClient, MC_GM_RANGE_KILL_NPC_REQ, MCommandParameterFloat(1000.0f));

		return;
	}
	int nNPCID = GetRandomNPCID();

	vec3 vSpawnPos = m_pAgent->GetCenter();
	XBIRDPOSTCMD3(pClient, MC_GM_REQUEST_SPAWN, MCommandParameterInt(nNPCID), MCommandParameterInt(1), MCommandParameterVector(vSpawnPos));

	m_listNPC.push_back(nNPCID);
}

