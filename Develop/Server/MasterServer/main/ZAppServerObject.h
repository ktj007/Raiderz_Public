#pragma once

#include "MUID.h"
#include "SCommObject.h"

#include "ZAppServerInfoManager.h"

struct TD_LOGIN_GAME_SERVER_INFO;

class ZAppServerObject : public SCommObject
{
private:
	ZAppServerInfo* m_pAppServerInfo;
	uint32 m_nIP;
	int m_nPort;
	SAppServerMode m_eMode;

public:
	ZAppServerObject(MUID uidObject, ZAppServerInfo* pAppServerInfo);	
	~ZAppServerObject();

	uint32 GetIP(void);
	int GetPort(void);
	SAppServerMode GetMode(void);
	void SetIP(uint32 nIP);
	void SetPort(int nPort);
	void SetMode(SAppServerMode eAppServerMode);

	int GetID() const { return m_pAppServerInfo->m_nID; }
};
