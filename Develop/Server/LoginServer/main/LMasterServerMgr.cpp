#include "StdAfx.h"
#include "LMasterServerMgr.h"

LMasterServerMgr::LMasterServerMgr(void)
{
}

LMasterServerMgr::~LMasterServerMgr(void)
{
	FIni();
}

void LMasterServerMgr::Init()
{
	
}

void LMasterServerMgr::FIni()
{
	for each (LMasterServer* each in m_vecServers)
	{
		SAFE_DELETE(each);
	}
}

void LMasterServerMgr::Add( const LMasterServerInfo& ServerInfo )
{
	m_vecServers.push_back(new LMasterServer(ServerInfo));
}

LMasterServer* LMasterServerMgr::Find( int nIndex )
{
	if (nIndex < 0 || nIndex >= (int)m_vecServers.size())		return NULL;

	return m_vecServers[nIndex];
}

void LMasterServerMgr::GetServerList( vector<TD_MasterServerInfo>& vecServers )
{
	for each (LMasterServer* each in m_vecServers)
	{
		TD_MasterServerInfo tdServer;

		wcsncpy_s(tdServer.szName, each->GetInfo().strName.c_str(), _TRUNCATE);
		vecServers.push_back(tdServer);
	}
}
