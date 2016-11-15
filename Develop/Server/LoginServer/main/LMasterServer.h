#pragma once

#include "MTypes.h"
#include "MUID.h"

struct LMasterServerInfo
{
	wstring	strName;
	uint8	nAddr[4];
	int		nPort;

	LMasterServerInfo()
	{
		strName = L"untitled";
		memset(nAddr, 0, sizeof(nAddr));
		nPort = 0;
	}

};
class LMasterServer
{
private: 
	LMasterServerInfo m_Info;
public:
	LMasterServer(const LMasterServerInfo& Info);
	~LMasterServer(void);

	void TryConnection(MUID uidPlayer);

	const LMasterServerInfo& GetInfo()		{ return m_Info; }
};
