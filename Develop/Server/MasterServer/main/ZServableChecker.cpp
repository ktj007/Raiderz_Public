#include "StdAfx.h"
#include "ZServableChecker.h"
#include "ZServerInfo.h"
#include "ZGlobal.h"
#include "ZServer.h"

#include "ZLoginServerInfoManager.h"
#include "ZGameServerInfoManager.h"
#include "ZAppServerInfoManager.h"

#include "ZLoginServerFacade.h"
#include "ZGameServerObjectManager.h"
#include "ZAppServerObjectManager.h"


bool ZServableChecker::Check(const float fDelta)
{
	if (!Check_RequiredServerCount()) return false;
	if (!Check_CurrntServerCount()) return false;

	if (SERVER_STAT_ACTIVE != gsys.pServer->GetServerInfo().nStat)
	{
		gsys.pServer->GetServerInfo().nStat = SERVER_STAT_ACTIVE;
		mlog("server start! (Servable)\n");
	}

	return true;
}

bool ZServableChecker::Check_RequiredServerCount()
{
	int nMaxRequiredLoginServerCount = gmgr.pLoginServerInfoManager->GetSize();
	if (0 == nMaxRequiredLoginServerCount)
	{
		gsys.pServer->GetServerInfo().nStat = SERVER_STAT_INACTIVE;
		mlog("Not Servable. Required LoginServer Count is Zero\n");
		return false;
	}

	int nMaxRequiredGameServerCount = gmgr.pGameServerInfoManager->GetSize();
	if (0 == nMaxRequiredGameServerCount)
	{
		gsys.pServer->GetServerInfo().nStat = SERVER_STAT_INACTIVE;
		mlog("Not Servable. Required GameServer Count is Zero\n");
		return false;
	}

	int nMaxRequiredAppServerCount = gmgr.pAppServerInfoManager->GetSize();
	if (0 == nMaxRequiredAppServerCount)
	{
		gsys.pServer->GetServerInfo().nStat = SERVER_STAT_INACTIVE;
		mlog("Not Servable. Required AppServer Count is Zero\n");
		return false;
	}

	return true;
}

bool ZServableChecker::Check_CurrntServerCount()
{
	if (gmgr.pLoginServerFacade->IsConnected() == false)
	{
		gsys.pServer->GetServerInfo().nStat = SERVER_STAT_INACTIVE;
		mlog("Not Servable. LoginServer is not connected. (ID= %d)\n", gmgr.pLoginServerFacade->GetID());
		return false;
	}

	int nMaxRequiredAppServerCount = gmgr.pAppServerInfoManager->GetSize();
	int nCurrAppServerCount = gmgr.pAppServerObjectManager->GetClientsCount();
	if (nMaxRequiredAppServerCount != nCurrAppServerCount)
	{
		gsys.pServer->GetServerInfo().nStat = SERVER_STAT_INACTIVE;
		mlog("Not Servable. AppServer Count = %d(%d)\n", nCurrAppServerCount, nMaxRequiredAppServerCount);
		return false;
	}

	int nMaxRequiredGameServerCount = gmgr.pGameServerInfoManager->GetSize();
	int nCurrGameServerCount = gmgr.pGameServerObjectManager->GetClientsCount();
	ZGameServerInfoMap::iterator itrGSInfo = gmgr.pGameServerInfoManager->GetServerInfoMap().begin();
	for (; itrGSInfo != gmgr.pGameServerInfoManager->GetServerInfoMap().end(); ++itrGSInfo)
	{
		ZGameServerInfo* pGSInfo = (*itrGSInfo).second;
		ZGameServerObject* pGSObj = gmgr.pGameServerObjectManager->FindGameServer(pGSInfo->m_nID);
		if (NULL == pGSObj)
		{
			gsys.pServer->GetServerInfo().nStat = SERVER_STAT_INACTIVE;
			mlog("Not Servable. Not Exist GameServer Object(ID=%d). Count(%d/%d)\n"
				, pGSInfo->m_nID, nCurrGameServerCount, nMaxRequiredGameServerCount);
			return false;
		}
	}

	return true;
}
