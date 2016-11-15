#include "stdafx.h"
#include "LWorldLocatorServer.h"
#include "CCommandTable_WorldLocator.h"
#include "MUDPCommandHandler.h"
#include "CTransData.h"
#include "LDBManager.h"
#include "LGlobal.h"


using namespace minet;

static const float WORLD_CHECK_TICK_TIME = 30.f;


LWorldLocatorServer::LWorldLocatorServer()
{
	m_pCommandHandler->AddHandler(MWC_COMM_WORLD_INFO_REQ, OnWorldInfoRequest);
	m_worldCheckRegulator.SetTickTime(WORLD_CHECK_TICK_TIME);
	m_worldCheckRegulator.Stop();
}

LWorldLocatorServer::~LWorldLocatorServer()
{	
}

bool LWorldLocatorServer::Create(int nPort)
{
	if (!__super::Create(nPort))
	{
		return false;
	}

	m_worldCheckRegulator.Start();
	
	return true;
}

void LWorldLocatorServer::Update(float fDeltaSecond)
{
	if (m_worldCheckRegulator.IsReady(fDeltaSecond))
	{
		OnUpdate();
	}
}

void LWorldLocatorServer::OnUpdate(void)
{
	gsys.pDBManager->WorldGetList();
}

void LWorldLocatorServer::SetWorldInfo(const vector<CSWorldInfo>& vecWorldInfo)
{
	// 정보 갱신
	MCriticalSection::Guard guard(m_csWorldInfoLock);
	m_vecWorldInfo = vecWorldInfo;
}

IMPL_WL_CMD_HANDLER(LWorldLocatorServer, OnWorldInfoRequest)
{
	// UDPSocket 쓰레드에서 호출됨으로, 락을 겁니다.
	LWorldLocatorServer* pThis = dynamic_cast<LWorldLocatorServer*>(pHandler);
	if (pThis == NULL)	return false;

	MCriticalSection::Guard guard(pThis->m_csWorldInfoLock);


	// 값 적용
	vector<TD_WORLD_INFO> vecWorldInfo;
	for each(const CSWorldInfo& info in pThis->m_vecWorldInfo)
	{
		TD_WORLD_INFO tdInfo;
		
		tdInfo.nID = info.nID;
		wcsncpy_s(tdInfo.strName, info.strName.c_str(), _TRUNCATE);
		wcsncpy_s(tdInfo.strIP, info.strIP.c_str(), _TRUNCATE);
		tdInfo.nCurrentPlayerCount = info.nCurrentPlayerCount;
		tdInfo.nMaxPlayerCount = info.nMaxPlayerCount;
		tdInfo.nType = info.nType;

		vecWorldInfo.push_back(tdInfo);
	}


	// 월드 정보 전달
	MCommand* pNewCmd = new MCommand();
	pNewCmd->SetID(MWC_COMM_WORLD_INFO_RES);
	pNewCmd->AddParameter(NEW_BLOB(vecWorldInfo));

	pHandler->PostCommand(dwIP, wRawPort, pNewCmd);


	return true;
}
