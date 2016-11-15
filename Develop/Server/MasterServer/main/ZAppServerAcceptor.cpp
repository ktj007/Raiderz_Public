#include "StdAfx.h"
#include "ZAppServerAcceptor.h"
#include "ZAppServerObject.h"
#include "ZAppServerObjectManager.h"
#include "ZAppServerInfo.h"
#include "ZAppServerInfoManager.h"
#include "ZGlobal.h"
#include "ZCommandCenter.h"
#include "ZCommandTable.h"
#include "ZConfig.h"


bool ZAppServerAcceptor::IsAlreadyExist()
{
	ZAppServerObject* pAppServerObject = gmgr.pAppServerObjectManager->FindAppServerFromServerID(m_nServerID);
	if (pAppServerObject != NULL) return true;

	return false;
}

bool ZAppServerAcceptor::Accept()
{
	ZAppServerInfo* pAppServerInfo = gmgr.pAppServerInfoManager->GetInfo(m_nServerID);
	if (pAppServerInfo == NULL)
	{
		return false;
	}

	if (m_vecAppServerList.empty())
	{
		return false;
	}

	// 서버 오브젝트 생성
	CreateAppServerObject(m_uidClient, pAppServerInfo);

	mlog("App Server Logined : ServerID(%d)\n", m_nServerID);

	return true;
}

minet::MCommand* ZAppServerAcceptor::MakeResponseCommand()
{
	minet::MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MAC_COMM_RESPONSE_LOGIN
															, m_uidClient
															, 3
															, NEW_INT(CR_SUCCESS)
															, NEW_UID(m_uidClient)
															, NEW_UCHAR(ZConfig::m_nServerMode));
	return pNewCommand;
}

minet::MCommand* ZAppServerAcceptor::MakeFailCommand(int nCommandResult)
{
	MCommand* pFailCommand = gsys.pCommandCenter->MakeNewCommand(MAC_COMM_RESPONSE_LOGIN, 
															m_uidClient, 
															3, 
															NEW_INT(nCommandResult), 
															NEW_UID(m_uidClient),
															NEW_UCHAR(ZConfig::m_nServerMode));
	return pFailCommand;
}

void ZAppServerAcceptor::CreateAppServerObject( MUID uidClient, ZAppServerInfo* pAppServerInfo )
{
	ZAppServerObject* pAppServerObject = gmgr.pAppServerObjectManager->NewAppServerObject(uidClient, pAppServerInfo);

	// 현재 채팅기능의 단일모드만 지원
	TD_APP_SERVER_INFO& info = m_vecAppServerList.front();
	pAppServerObject->SetIP(info.nIP);
	pAppServerObject->SetPort(info.nPort);
	pAppServerObject->SetMode((SAppServerMode)info.nAppServerMode);

	gmgr.pAppServerObjectManager->AddObject(pAppServerObject);	
}
