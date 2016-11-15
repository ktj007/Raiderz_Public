#include "StdAfx.h"
#include "ZLoginServerAcceptor.h"
#include "ZLoginServerFacade.h"
#include "ZLoginServerInfo.h"
#include "ZLoginServerInfoManager.h"
#include "ZGlobal.h"
#include "ZCommandCenter.h"
#include "ZCommandTable.h"
#include "ZGameServerInfoManager.h"
#include "ZAppServerInfoManager.h"
#include "STransData.h"
#include "ZConfig.h"


bool ZLoginServerAcceptor::IsAlreadyExist()
{
	if (gmgr.pLoginServerFacade->IsConnected()) return true;
	return false;
}

bool ZLoginServerAcceptor::Accept()
{
	ZLoginServerInfo* pLoginServerInfo = gmgr.pLoginServerInfoManager->GetInfo(m_nServerID);
	if (pLoginServerInfo == NULL)
	{
		return false;
	}

	// 로그인 서버 연결됨
	gmgr.pLoginServerFacade->Connected(m_uidClient, pLoginServerInfo);

	mlog("Login Server Logined : ServerID(%d)\n", m_nServerID);

	return true;
}

minet::MCommand* ZLoginServerAcceptor::MakeResponseCommand()
{
	std::vector<TD_SERVER_LIST_NODE> vecServerList;

	// 마스터서버 추가
	{
		TD_SERVER_LIST_NODE tdNode;
		tdNode.nServerType = SERVER_MASTER;
		tdNode.nServerID = ZConfig::m_nMyServerID;
		vecServerList.push_back(tdNode);
	}

	// 게임서버 목록 추가
	ZGameServerInfoMap::iterator itrGameServer = gmgr.pGameServerInfoManager->GetServerInfoMap().begin();
	for (; itrGameServer != gmgr.pGameServerInfoManager->GetServerInfoMap().end(); ++itrGameServer)
	{
		ZGameServerInfo* pServerInfo = (*itrGameServer).second;

		TD_SERVER_LIST_NODE tdNode;
		tdNode.nServerType = SERVER_GAME;
		tdNode.nServerID = pServerInfo->m_nID;

		vecServerList.push_back(tdNode);
	}

	// 앱서버 목록 추가
	ZAppServerInfoMap::iterator itrAppServer = gmgr.pAppServerInfoManager->GetServerInfoMap().begin();
	for (; itrAppServer != gmgr.pAppServerInfoManager->GetServerInfoMap().end(); ++itrAppServer)
	{
		ZAppServerInfo* pServerInfo = (*itrAppServer).second;

		TD_SERVER_LIST_NODE tdNode;
		tdNode.nServerType = SERVER_APPLICATION;
		tdNode.nServerID = pServerInfo->m_nID;

		vecServerList.push_back(tdNode);
	}

	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MLC_COMM_RESPONSE_LOGIN
															, m_uidClient
															, 4
															, NEW_INT(CR_SUCCESS)
															, NEW_UID(m_uidClient)
															, NEW_UCHAR(ZConfig::m_nServerMode)
															, NEW_BLOB(vecServerList));
	return pNewCommand;
}

minet::MCommand* ZLoginServerAcceptor::MakeFailCommand(int nCommandResult)
{
	MCommand* pFailCommand = gsys.pCommandCenter->MakeNewCommand(MLC_COMM_RESPONSE_LOGIN
															, m_uidClient
															, 4
															, NEW_INT(nCommandResult) 
															, NEW_UID(m_uidClient)
															, NEW_UCHAR(ZConfig::m_nServerMode)
															, NEW_BLOB(NULL, sizeof(TD_SERVER_LIST_NODE), 0));
	return pFailCommand;
}
