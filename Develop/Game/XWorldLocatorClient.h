#pragma once

#include "MUDPNetClient.h"
#include "XGlobalVar_Network.h"
#include "MSync.h"

// protected 상속. 기존 초기화 방식을 숨기고 재정의합니다.
class XWorldLocatorClient: protected minet::MUDPNetClient
{
public:
	enum RunState
	{
		STOP,
		START,
		RUN,
		FINISH,
	};

public:
	XWorldLocatorClient();
	virtual ~XWorldLocatorClient();
	
	bool Start(const vector<wstring>& vecIP, int nPort);
	void Update(float fDelta);

	DECL_WL_CMD_HANDLER(OnWorldInfoResponse);

private:
	bool Connect(void);

private:
	MCriticalSection	m_csWorldInfoLock;
	vector<CSWorldInfo>	m_vecWorldInfo;
	bool				m_isUpdate;
	
	MRegulator			m_RetryRegulator;	
	queue<wstring>		m_queueIP;
	int					m_nPort;	

	RunState			m_nState;
};
