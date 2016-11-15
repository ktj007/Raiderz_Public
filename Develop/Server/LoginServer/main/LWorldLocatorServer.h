#pragma once

#include "MUDPNetServer.h"
#include "CSDef.h"

class LWorldLocatorServer: public minet::MUDPNetServer
{
public:
	LWorldLocatorServer();
	virtual ~LWorldLocatorServer();

	virtual bool Create(int nPort);

	void Update(float fDeltaSecond);
	void OnUpdate(void);

	void SetWorldInfo(const vector<CSWorldInfo>& vecWorldInfo);

	DECL_WL_CMD_HANDLER(OnWorldInfoRequest);

private:	
	MRegulator			m_worldCheckRegulator;
	MCriticalSection	m_csWorldInfoLock;
	vector<CSWorldInfo>	m_vecWorldInfo;
};
