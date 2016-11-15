#include "stdafx.h"
#include "ZAppLogic.h"
#include "STransData_M2G.h"
#include "ZAppServerObjectManager.h"
#include "ZAppServerObject.h"
#include "ZCommandCenter.h"
#include "ZCommandTable.h"

ZAppLogic::ZAppLogic()
{
	// do nothing
}

ZAppLogic::~ZAppLogic()
{
	// do nothing
}

void ZAppLogic::RequestAppServerList(MUID uidClient)
{
	// 앱서버 목록 수집
	vector<TD_APP_SERVER_INFO> vecAppServerList;

	SCommObjectManager::SCommObjectMap& mapClient = gmgr.pAppServerObjectManager->GetObjects();
	for each(SCommObjectManager::SCommObjectMap::value_type pairClient in mapClient)
	{
		ZAppServerObject* pClient = (ZAppServerObject*)pairClient.second;
		_ASSERT(pClient != NULL);

		TD_APP_SERVER_INFO appServer;
		appServer.nAppServerMode = (int)pClient->GetMode();
		appServer.nIP = pClient->GetIP();
		appServer.nPort = pClient->GetPort();

		vecAppServerList.push_back(appServer);
	}

	// 목록 전달
	MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MMC_COMM_RESPONSE_APP_SERVER_LIST,
														uidClient,
														1,
														NEW_BLOB(vecAppServerList)
														);	

	gsys.pCommandCenter->PostCommand(pCmd);
}
