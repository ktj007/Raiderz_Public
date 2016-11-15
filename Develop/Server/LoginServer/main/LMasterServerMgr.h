#pragma once

#include "LMasterServer.h"
#include "CTransData_Login.h"

class LMasterServerMgr
{
	vector<LMasterServer*>		m_vecServers;
public:
	LMasterServerMgr(void);
	~LMasterServerMgr(void);

	void			Init();
	void			FIni();

	void			Add(const LMasterServerInfo& ServerInfo);
	LMasterServer*	Find(int nIndex);
	void			GetServerList(vector<TD_MasterServerInfo>& vecServers);
	
};
