#include "StdAfx.h"
#include "GReserveCmdManager.h"
#include "GReserveCmd.h"
#include "AStlUtil.h"
#include "GSystem.h"
#include "GGlobal.h"


GReserveCmdManager::GReserveCmdManager(void)
{
}

GReserveCmdManager::~GReserveCmdManager(void)
{
	Clear();
}

void GReserveCmdManager::Update()
{
	uint32 nNowTime = gsys.pSystem->GetNowTime();

	set<int> setTimeOverCID;
	for each (const MAP_RESERVE_CMD::value_type& val in m_mapReserveCmd)
	{
		const int& nCID = val.first;
		GReserveCmd* pReserveCmd = val.second;
		if (NULL == pReserveCmd) continue;

		if (true == pReserveCmd->IsTimeOver(nNowTime))
		{
			setTimeOverCID.insert(nCID);
		}		
	}

	for each (const int& nTimeOverCID in setTimeOverCID)
	{
		DeleteReserveCmd(nTimeOverCID);
	}	
}

void GReserveCmdManager::Clear()
{
	SAFE_DELETE_MAP(m_mapReserveCmd);
}

void GReserveCmdManager::Reserve(vector<int>& vecCID, MCommand* pCmd)
{
	for each (int nCID in vecCID)
	{
		Reserve(nCID, pCmd);
	}
}

void GReserveCmdManager::Reserve(int nCID, MCommand* pCmd)
{
	if (NULL == pCmd) return;

	MAP_RESERVE_CMD::iterator itor = m_mapReserveCmd.find(nCID);
	if (m_mapReserveCmd.end() == itor)
	{
		GReserveCmd* pNewReservedCmd = new GReserveCmd(pCmd);
		m_mapReserveCmd.insert(MAP_RESERVE_CMD::value_type(nCID, pNewReservedCmd));
	}
	else
	{
		GReserveCmd* pReserveCmd = itor->second;
		pReserveCmd->Insert(pCmd);
	}
}

GReserveCmd* GReserveCmdManager::GetReserveCmd(int nCID)
{
	MAP_RESERVE_CMD::iterator itor = m_mapReserveCmd.find(nCID);
	if (m_mapReserveCmd.end() == itor) return NULL;

	GReserveCmd* pReserveCmd = itor->second;

	return pReserveCmd;
}

void GReserveCmdManager::RunAndDeleteReserveCmd(int nCID)
{
	RunReserveCmd(nCID);
	DeleteReserveCmd(nCID);
}

void GReserveCmdManager::RunReserveCmd(int nCID)
{
	GReserveCmd* pReserveCmd = GetReserveCmd(nCID);
	if (NULL == pReserveCmd) return;

	pReserveCmd->Run();	
}

void GReserveCmdManager::DeleteReserveCmd(int nCID)
{
	GReserveCmd* pReserveCmd = GetReserveCmd(nCID);
	if (NULL == pReserveCmd) return;

	SAFE_DELETE(pReserveCmd);	
	m_mapReserveCmd.erase(nCID);
}