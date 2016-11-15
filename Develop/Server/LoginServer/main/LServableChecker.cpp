#include "StdAfx.h"
#include "LServableChecker.h"
#include "LServerInfo.h"
#include "LGlobal.h"
#include "LServer.h"
#include "LMasterClient.h"
#include "LDBManager.h"
#include "LConfig.h"


void LServableChecker::Reset()
{
	m_ePhase = SCP_WAIT;
	m_vecServerStatusList.clear();
	m_fnServerStartCallback = NULL;
}

bool LServableChecker::Start(std::vector<TD_SERVER_LIST_NODE>& vecServerListFromMasterServer)
{
	if (vecServerListFromMasterServer.empty()) return false;

	for (int i = 0; i < (int)vecServerListFromMasterServer.size(); ++i)
	{
		if (SERVER_LOGIN == vecServerListFromMasterServer[i].nServerType) continue;

		SERVER_STATUS_NODE node;
		node.nServerID = vecServerListFromMasterServer[i].nServerID;
		m_vecServerStatusList.push_back(node);
	}

	m_ePhase = SCP_STARTING;
	return true;
}

void LServableChecker::UpdateServerStatusInfoByDB(int nServerID, bool bIsRunning, bool bServable)
{
	for (size_t i = 0; i < m_vecServerStatusList.size(); ++i)
	{
		if (nServerID == m_vecServerStatusList[i].nServerID)
		{
			m_vecServerStatusList[i].bIsRunning = bIsRunning;
			m_vecServerStatusList[i].bServable = bServable;
			break;
		}
	}
}

bool LServableChecker::Check(const float fDelta)
{
	if (LConfig::m_bStandAlone)
	{
		if (SERVER_STAT_ACTIVE != gsys.pServer->GetServerInfo().nStat)
		{
			gsys.pServer->GetServerInfo().nStat = SERVER_STAT_ACTIVE;

			if (m_fnServerStartCallback)
				m_fnServerStartCallback();
		}
		return true;
	}

	if		(GetPhase() == SCP_WAIT)		return false;
	
	if		(GetPhase() == SCP_STARTING)	return CheckPhase_Starting();
	else if (GetPhase() == SCP_RUNNING)		return CheckPhase_Running();
	return false;
}

bool LServableChecker::CheckPhase_Starting()
{
	if (!Check_IsAllServerStart())
	{
		vector<int> vecServerIDList;
		for (size_t i = 0; i < m_vecServerStatusList.size(); ++i)
		{
			vecServerIDList.push_back(m_vecServerStatusList[i].nServerID);
		}
		gsys.pDBManager->GetServerStatusList(LConfig::m_nWorldID, vecServerIDList);
		return false;
	}

	m_ePhase = SCP_RUNNING;
	return CheckPhase_Running();
}
bool LServableChecker::Check_IsAllServerStart()
{
	bool bRet = true;
	for (size_t i = 0; i < m_vecServerStatusList.size(); ++i)
	{
		if (false == m_vecServerStatusList[i].bIsRunning ||
			false == m_vecServerStatusList[i].bServable)
		{
			if (bRet)
			{
				mlog("Not Servable. Not Started All servers\n");
				bRet = false;
			}

			gsys.pServer->GetServerInfo().nStat = SERVER_STAT_INACTIVE;
			mlog("\tServer ID = %d, Run = %d, Servable = %d\n"
				, m_vecServerStatusList[i].nServerID
				, (int)m_vecServerStatusList[i].bIsRunning
				, (int)m_vecServerStatusList[i].bServable);
		}
	}
	return bRet;
}

bool LServableChecker::CheckPhase_Running()
{
	if (!Check_IsConnectedMasterServer()) return false;

	if (SERVER_STAT_ACTIVE != gsys.pServer->GetServerInfo().nStat)
	{
		gsys.pServer->GetServerInfo().nStat = SERVER_STAT_ACTIVE;
		mlog("server start! (Servable)\n");

		if (m_fnServerStartCallback)
			m_fnServerStartCallback();
	}

	return true;
}
bool LServableChecker::Check_IsConnectedMasterServer()
{
	if (false == gsys.pServer->GetMasterClient()->GetNetClient()->IsConnected())
	{
		gsys.pServer->GetServerInfo().nStat = SERVER_STAT_INACTIVE;
		mlog("Not Servable. Not Connected To MasterServer.\n");
		return false;
	}
	return true;
}
